#include "stdafx.h"
#include "VKGSRender.h"
#include "vkutils/buffer_object.h"
#include "vkutils/memory.h"
#include "Emu/RSX/Overlays/overlay_manager.h"
#include "Emu/RSX/Overlays/overlay_debug_overlay.h"
#include "Emu/Cell/Modules/cellVideoOut.h"

#include "upscalers/bilinear_pass.hpp"
#include "upscalers/fsr_pass.h"
#include "upscalers/nearest_pass.hpp"
#include "VKCompute.h"
#include "TM4KMicro.hpp"
#include "TM4KMaster.hpp"
#include "TM4KAffine.hpp"
#include "TM4KHybrid.hpp"
#include "TM4KNativeColour.hpp"
#include "TM4KTiming.hpp"
#include "TM4KParticles.hpp"
#include "VKResourceManager.h"
#include "vkutils/barriers.h"
#include "util/asm.hpp"
#include "util/video_provider.h"

#include <fstream>
#include <cstdlib>

extern atomic_t<bool> g_user_asked_for_screenshot;
extern atomic_t<recording_mode> g_recording_mode;

namespace
{
	// Twisted Metal renders its 3D scene at the configured scale, then the SPU
	// post-processing path replaces it with a nearest-neighbour 1280x704 copy.
	// Use the 16-bit HDR scene, which retains 4K geometry while avoiding much
	// of the stippled lighting in the earlier BGRA8 surface. Keep final UI.
	struct tm4k_task final : vk::compute_task
	{
		u32 input_count;
		std::vector<vk::image_view*> inputs;
		vk::image_view* output = nullptr;
		std::unique_ptr<vk::sampler> sampler;

		tm4k_task(const char* source, u32 count) : input_count(count), inputs(count)
		{
			m_src = source;
			ssbo_count = 0;
			create();
		}

		std::vector<vk::glsl::program_input> get_inputs() override
		{
			std::vector<vk::glsl::program_input> result;
			for (u32 i = 0; i < input_count; ++i)
			{
				result.push_back(vk::glsl::program_input::make(
					::glsl::glsl_compute_program, "Input" + std::to_string(i),
					vk::glsl::input_type_texture, 0, i));
			}
			result.push_back(vk::glsl::program_input::make(
				::glsl::glsl_compute_program, "OutputTexture",
				vk::glsl::input_type_storage_texture, 0, input_count));
			return result;
		}

		void bind_resources(const vk::command_buffer&) override
		{
			if (!sampler)
			{
				auto pdev = vk::get_current_renderer();
				sampler = std::make_unique<vk::sampler>(*pdev,
					VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_FALSE, 0.f, 1.f, 0.f, 0.f,
					VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST,
					VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);
			}
			for (u32 i = 0; i < input_count; ++i)
				m_program->bind_uniform({ *inputs[i], *sampler }, 0, i);
			m_program->bind_uniform({ *output }, 0, input_count);
		}

		void execute(const vk::command_buffer& cmd,
			std::initializer_list<vk::viewable_image*> sources, vk::viewable_image* destination)
		{
			ensure(sources.size() == input_count);
			u32 i = 0;
			for (auto* source : sources)
				inputs[i++] = source->get_view(rsx::default_remap_vector.with_encoding(vk::VK_REMAP_IDENTITY));
			output = destination->get_view(rsx::default_remap_vector.with_encoding(vk::VK_REMAP_IDENTITY));
			compute_task::run(cmd,
				utils::aligned_div(destination->width(), 16),
				utils::aligned_div(destination->height(), 16), 1);
		}
	};

	constexpr const char* tm4k_downsample_scene = R"glsl(
#version 450
layout(local_size_x=16, local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1,rgba8) uniform writeonly image2D OutputTexture;
void main()
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	if (any(greaterThanEqual(p, imageSize(OutputTexture)))) return;
	ivec2 size = textureSize(Input0, 0);
	ivec2 factor = max(size / imageSize(OutputTexture), ivec2(1));
	if (size == ivec2(3840,2160) && imageSize(OutputTexture) == ivec2(1280,704))
	{
		float y0=float(p.y)*float(size.y)/704.0, y1=float(p.y+1)*float(size.y)/704.0;
		vec3 sum=vec3(0); float weight=0.0;
		for(int y=int(floor(y0)); y<int(ceil(y1)); ++y)
		for(int x=0; x<3; ++x)
		{
			float w=max(0.0,min(y1,float(y+1))-max(y0,float(y)));
			vec3 hdr=max(texelFetch(Input0,ivec2(p.x*3+x,min(y,size.y-1)),0).rgb,vec3(0));
			sum+=pow(clamp((hdr*5.0)/(vec3(1)+hdr*5.0),vec3(0),vec3(1)),vec3(0.8))*w; weight+=w;
		}
		imageStore(OutputTexture,p,vec4(sum/weight,1)); return;
	}
	vec3 sum = vec3(0.0);
	for (int y = 0; y < factor.y; ++y)
	for (int x = 0; x < factor.x; ++x)
	{
		vec3 hdr=max(texelFetch(Input0, min(p * factor + ivec2(x,y), size - 1), 0).rgb,vec3(0));
		sum += pow(clamp((hdr*5.0)/(vec3(1.0)+hdr*5.0),vec3(0),vec3(1)),vec3(0.8));
	}
	imageStore(OutputTexture, p, vec4(sum / float(factor.x * factor.y), 1.0));
}
)glsl";

	constexpr const char* tm4k_downsample_guide = R"glsl(
#version 450
layout(local_size_x=16, local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
#ifdef TM4K_PACKED_ALPHA
layout(set=0,binding=1,rgba16f) uniform writeonly image2D OutputTexture;
#else
layout(set=0,binding=1,rgba8) uniform writeonly image2D OutputTexture;
#endif
#ifdef TM4K_PACKED_ALPHA
vec3 guide_working_color(vec3 decoded)
{
	// Compress linear lighting before local fitting, not packed RGB channels.
	vec3 x=max(decoded,vec3(0))*0.5;
	x=x/(vec3(1)+x);
	return mix(1.055*pow(x,vec3(1.0/2.4))-0.055,12.92*x,lessThanEqual(x,vec3(0.0031308)));
}
#endif
void main()
{
	ivec2 p=ivec2(gl_GlobalInvocationID.xy);
	if (any(greaterThanEqual(p,imageSize(OutputTexture)))) return;
	ivec2 size=textureSize(Input0,0);
	ivec2 factor=max(size/imageSize(OutputTexture),ivec2(1));
	if (size == ivec2(3840,2160) && imageSize(OutputTexture) == ivec2(1280,704))
	{
		float y0=float(p.y)*float(size.y)/704.0, y1=float(p.y+1)*float(size.y)/704.0;
		vec3 sum=vec3(0); float weight=0.0;
		for(int y=int(floor(y0)); y<int(ceil(y1)); ++y)
		for(int x=0; x<3; ++x)
		{
			float w=max(0.0,min(y1,float(y+1))-max(y0,float(y)));
			vec4 packed=texelFetch(Input0,ivec2(p.x*3+x,min(y,size.y-1)),0);
#ifdef TM4K_PACKED_ALPHA
			sum+=guide_working_color(packed.rgb*(packed.a*255.0))*w;
#else
			sum+=packed.rgb*w;
#endif
			weight+=w;
		}
		imageStore(OutputTexture,p,vec4(sum/weight,1)); return;
	}
	vec3 sum=vec3(0);
	for (int y=0;y<factor.y;++y)
	for (int x=0;x<factor.x;++x)
	{
		vec4 packed=texelFetch(Input0,min(p*factor+ivec2(x,y),size-1),0);
#ifdef TM4K_PACKED_ALPHA
		// The geometry shader writes RGB/factor and factor in Alpha.
		// Decode and tone-compress EACH texel before averaging. The merge uses
		// exactly the same per-texel transform before bilinear interpolation.
		sum+=guide_working_color(packed.rgb*(packed.a*255.0));
#else
		sum+=packed.rgb;
#endif
	}
	imageStore(OutputTexture,p,vec4(sum/float(factor.x*factor.y),1));
}
)glsl";

	constexpr const char* tm4k_fit_packed_guide = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // decoded, tone-compressed low guide
layout(set=0,binding=1) uniform sampler2D Input1; // native, correctly toned color
layout(set=0,binding=2,rgba16f) uniform writeonly image2D OutputTexture;
void main()
{
	ivec2 p=ivec2(gl_GlobalInvocationID.xy);
	ivec2 low=textureSize(Input0,0);
	if (any(greaterThanEqual(p,low))) return;
	const vec3 luma_weights=vec3(0.2126,0.7152,0.0722);
	float ma=0.0,mb=0.0,aa=0.0,ab=0.0;
	for (int y=-3;y<=3;++y)
	for (int x=-3;x<=3;++x)
	{
		ivec2 q=clamp(p+ivec2(x,y),ivec2(0),low-1);
		float a=dot(texelFetch(Input0,q,0).rgb,luma_weights);
		float b=dot(texelFetch(Input1,q,0).rgb,luma_weights);
		ma+=a;mb+=b;aa+=a*a;ab+=a*b;
	}
	ma/=49.0;mb/=49.0;aa/=49.0;ab/=49.0;
	float variance=max(aa-ma*ma,0.0);
	float covariance=ab-ma*mb;
	// Scalar luminance moments, not dot(E[RGB*RGB],luma). Channel-wise
	// fits can alter hue and exaggerate individual clipped color channels.
	float slope=clamp(max(covariance/(variance+0.0001),mb/max(ma,0.015)),0.0,2.0);
	float bias=mb-slope*ma;
	// Two halves: avoid 49 neighborhood reads per full-resolution pixel.
	imageStore(OutputTexture,p,vec4(vec3(slope),1));
	imageStore(OutputTexture,p+ivec2(low.x,0),vec4(vec3(bias),1));
}
)glsl";

	constexpr const char* tm4k_downsample_final = R"glsl(
#version 450
layout(local_size_x=16, local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1,rgba8) uniform writeonly image2D OutputTexture;
void main()
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	ivec2 low = imageSize(OutputTexture);
	if (any(greaterThanEqual(p, low))) return;
	ivec2 high = textureSize(Input0, 0);
	if(high == ivec2(3840,2160) && low == ivec2(1280,704))
	{
		ivec2 q=min(ivec2((vec2(p)+0.5)*vec2(high)/vec2(low)),high-1);
		imageStore(OutputTexture,p,texelFetch(Input0,q,0)); return;
	}
	ivec2 factor = max(ivec2(high.x / low.x, (high.y - (high.y % low.y)) / low.y), ivec2(1));
	int border = max(0, (high.y - low.y * factor.y) / 2);
	ivec2 sample_pos = min(p * factor + factor / 2 + ivec2(0,border), high - 1);
	imageStore(OutputTexture, p, texelFetch(Input0, sample_pos, 0));
}
)glsl";

	constexpr const char* tm4k_fit = R"glsl(
#version 450
layout(local_size_x=16, local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // 3x box-filtered high-res scene
layout(set=0,binding=1) uniform sampler2D Input1; // game's final 704p color
layout(set=0,binding=2) uniform sampler2D Input2; // actual presentation, including opaque 2D layers
layout(set=0,binding=3) uniform sampler2D Input3; // underlying scene color
layout(set=0,binding=4,rgba16f) uniform writeonly image2D OutputTexture;
void main()
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(OutputTexture);
	if (any(greaterThanEqual(p, size))) return;
	vec3 ma=vec3(0), mb=vec3(0), aa=vec3(0), ab=vec3(0), bb=vec3(0);
	for (int y=-3; y<=3; ++y)
	for (int x=-3; x<=3; ++x)
	{
		ivec2 q=clamp(p+ivec2(x,y),ivec2(0),size-1);
		vec3 a=texelFetch(Input0,q,0).rgb;
		vec3 b=texelFetch(Input1,q,0).rgb;
		ma+=a; mb+=b; aa+=a*a; ab+=a*b; bb+=b*b;
	}
	ma/=49.0; mb/=49.0; aa/=49.0; ab/=49.0; bb/=49.0;
	vec3 va=max(aa-ma*ma,vec3(0));
	vec3 vb=max(bb-mb*mb,vec3(0));
	vec3 cov=ab-ma*mb;
	vec3 slope=clamp(cov/(va+vec3(5.0/(255.0*255.0))),vec3(0),vec3(1.5));
	vec3 corr=clamp(cov/sqrt(max(va*vb,vec3(0.00001))),vec3(0),vec3(1));
	float confidence=clamp(dot(corr,vec3(0.333333))*2.0,0.0,1.0);
	if (all(equal(p,ivec2(0))))
	{
		// A fullscreen black-backed logo can cover a concurrently rendered menu.
		// Alpha in the PS3 presentation is not a reliable UI stencil in all paths.
		// Detect covering black, not black borders or an intrinsically dark world.
		// Only this one low-resolution invocation samples the coarse frame grid.
		ivec2 full=textureSize(Input2,0);
		ivec2 scene=textureSize(Input3,0);
		int scale=full.x/1280;
		ivec2 origin=ivec2(39,24)*scale;
		ivec2 display_size=ivec2(1202,672)*scale;
		const ivec2 grid=ivec2(32,18);
		int covering_black=0;
		int empty_scene=0;
		int display_black=0;
		for (int y=0;y<grid.y;++y)
		for (int x=0;x<grid.x;++x)
		{
			vec2 uv=(vec2(x,y)+0.5)/vec2(grid);
			ivec2 display_pixel=clamp(origin+ivec2(uv*vec2(display_size)),ivec2(0),full-1);
			ivec2 scene_pixel=clamp(ivec2(uv*vec2(scene)),ivec2(0),scene-1);
			vec3 shown=texelFetch(Input2,display_pixel,0).rgb;
			vec3 underneath=texelFetch(Input3,scene_pixel,0).rgb;
			if (max(max(shown.r,shown.g),shown.b)<(0.5/255.0)) ++display_black;
			if (max(max(underneath.r,underneath.g),underneath.b)<(0.5/255.0))
				++empty_scene;
			if (max(max(shown.r,shown.g),shown.b)<(0.5/255.0) &&
				max(max(underneath.r,underneath.g),underneath.b)>0.03)
				++covering_black;
		}
		// Negative confidence at texel zero is an explicit frame-wide UI marker.
		// No new full-resolution mask, readback, draw hook or frame history.
		// Startup captures 1054/2059/2306 have an entirely empty color
		// scene but a nonempty authoritative display logo. A contrast-only
		// overlay mask attenuates its dim printing by up to 50 byte levels.
		// Mark it as UI even though there is no visible hidden scene to cover.
		if (covering_black*100>=50*grid.x*grid.y ||
			empty_scene*100>=99*grid.x*grid.y ||
			display_black*100>=90*grid.x*grid.y) confidence=-1.0;
	}
	imageStore(OutputTexture,p,vec4(slope,confidence));
}
)glsl";

	constexpr const char* tm4k_merge = R"glsl(
#version 450
layout(local_size_x=16, local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // high-res 16-bit HDR scene
layout(set=0,binding=1) uniform sampler2D Input1; // low-res exposed HDR scene
layout(set=0,binding=2) uniform sampler2D Input2; // low-res final tone map
layout(set=0,binding=3) uniform sampler2D Input3; // same-frame color scene
layout(set=0,binding=4) uniform sampler2D Input4; // local color slope and confidence
layout(set=0,binding=5) uniform sampler2D Input5; // actual presented image, including UI
#ifdef TM4K_GUIDED
layout(set=0,binding=6) uniform sampler2D Input6; // sharp lit geometry; never output its colors
layout(set=0,binding=7) uniform sampler2D Input7; // box-filtered lit guide
#ifdef TM4K_PACKED_ALPHA
layout(set=0,binding=8) uniform sampler2D Input8; // decoded-guide slope/bias atlas
layout(set=0,binding=9,rgba8) uniform writeonly image2D OutputTexture;
#else
layout(set=0,binding=8,rgba8) uniform writeonly image2D OutputTexture;
#endif
#else
layout(set=0,binding=6,rgba8) uniform writeonly image2D OutputTexture;
#endif
#ifdef TM4K_LIT_C
// Predict native nearest expansion followed by the scene sampler's bilinear
// filtering. Comparing against an unfiltered native pixel mislabels world edges.
vec3 repeated_native_tone(vec2 uv, ivec2 high_size, ivec2 low_size)
{
	ivec2 factor=high_size/low_size;
	vec2 position=clamp(uv*vec2(high_size)-0.5,vec2(0),vec2(high_size-1));
	ivec2 a=ivec2(floor(position));
	ivec2 b=min(a+1,high_size-1);
	vec2 f=fract(position);
	if(high_size == ivec2(3840,2160) && low_size == ivec2(1280,704))
	{
		ivec2 la=clamp(ivec2(floor((vec2(a)+0.5)*vec2(low_size)/vec2(high_size))),ivec2(0),low_size-1);
		ivec2 lb=clamp(ivec2(floor((vec2(b)+0.5)*vec2(low_size)/vec2(high_size))),ivec2(0),low_size-1);
		vec3 tl=texelFetch(Input2,la,0).rgb;
		vec3 tr=texelFetch(Input2,ivec2(lb.x,la.y),0).rgb;
		vec3 bl=texelFetch(Input2,ivec2(la.x,lb.y),0).rgb;
		vec3 br=texelFetch(Input2,lb,0).rgb;
		return mix(mix(tl,tr,f.x),mix(bl,br,f.x),f.y);
	}
	vec3 tl=texelFetch(Input2,a/factor,0).rgb;
	vec3 tr=texelFetch(Input2,ivec2(b.x,a.y)/factor,0).rgb;
	vec3 bl=texelFetch(Input2,ivec2(a.x,b.y)/factor,0).rgb;
	vec3 br=texelFetch(Input2,b/factor,0).rgb;
	return mix(mix(tl,tr,f.x),mix(bl,br,f.x),f.y);
}
#endif
#ifdef TM4K_PACKED_ALPHA
vec3 guide_working_color(vec3 decoded)
{
	vec3 x=max(decoded,vec3(0))*0.5;
	x=x/(vec3(1)+x);
	return mix(1.055*pow(x,vec3(1.0/2.4))-0.055,12.92*x,lessThanEqual(x,vec3(0.0031308)));
}
vec3 decoded_guide_texel(ivec2 p)
{
	vec4 packed=texelFetch(Input6,p,0);
	return guide_working_color(packed.rgb*(packed.a*255.0));
}
vec3 decoded_guide_bilinear(vec2 uv)
{
	ivec2 size=textureSize(Input6,0);
	vec2 position=clamp(uv*vec2(size)-0.5,vec2(0),vec2(size-1));
	ivec2 a=ivec2(floor(position)),b=min(a+1,size-1);
	vec2 f=fract(position);
	// Interpolating packed RGB and Alpha first creates false cross-products.
	return mix(mix(decoded_guide_texel(a),decoded_guide_texel(ivec2(b.x,a.y)),f.x),
		mix(decoded_guide_texel(ivec2(a.x,b.y)),decoded_guide_texel(b),f.x),f.y);
}
#endif
void main()
{
	ivec2 p=ivec2(gl_GlobalInvocationID.xy);
	ivec2 full=imageSize(OutputTexture);
	if (any(greaterThanEqual(p,full))) return;
	vec4 screen=texelFetch(Input5,p,0);
	// Opaque startup/logo/transition composition is authoritative in every route.
	// In particular, do not reconstruct the hidden menu into dark logo printing.
	if (texelFetch(Input4,ivec2(0),0).a<0.0)
	{ imageStore(OutputTexture,p,screen); return; }
	ivec2 scene=textureSize(Input0,0);
	ivec2 low=textureSize(Input1,0);
	// The game's actual presentation is inset and scaled, not a 1:1 copy of
	// the scene color surface. Measured at 300% in the saved GPU frame:
	// source 3840x2112 -> display rect (117,72)..(3723,2088).
	// Mixing the two at untransformed coordinates caused doubled lettering
	// and large rectangular seams in the world.
	int scale=full.x/1280;
	ivec2 display_origin=ivec2(39,24)*scale;
	ivec2 display_size=ivec2(1202,672)*scale;
	ivec2 relative=p-display_origin;
	if (any(lessThan(relative,ivec2(0))) || any(greaterThanEqual(relative,display_size)))
	{ imageStore(OutputTexture,p,screen); return; }
	vec2 uv=(vec2(relative)+0.5)/vec2(display_size);
	vec4 original=texture(Input3,uv);
#ifdef TM4K_PACKED_ALPHA
	// A black startup/logo/transition layer is opaque, not a missing scene.
	// The old absolute RGB-difference threshold leaked a dim menu through it.
	// Keep the actual presented black pixels, including zero-alpha PS3 blits.
	if (max(max(screen.r,screen.g),screen.b)<(0.5/255.0) &&
		max(max(original.r,original.g),original.b)>(0.5/255.0))
	{ imageStore(OutputTexture,p,screen); return; }
#endif
	vec3 hdr=max(texture(Input0,uv).rgb,vec3(0));
	vec3 high=pow(clamp((hdr*5.0)/(vec3(1.0)+hdr*5.0),vec3(0),vec3(1)),vec3(0.8));
	vec3 low_scene=texture(Input1,uv).rgb;
	vec3 low_tone=texture(Input2,uv).rgb;
	vec4 fit=texture(Input4,uv);
	// Some UI is already in the color surface; later UI is only in the final
	// display. Detect the latter per pixel instead of cutting out HUD boxes.
	vec3 tone_nearest=texelFetch(Input2,clamp(ivec2(uv*vec2(low)),ivec2(0),low-1),0).rgb;
	float ui_difference=max(max(abs(original.r-tone_nearest.r),abs(original.g-tone_nearest.g)),abs(original.b-tone_nearest.b));
	float ui_gate=1.0-smoothstep(0.07,0.22,ui_difference);
	float mask=ui_gate*fit.a;
	// The HDR scene has no sky. Its black pixels cannot supply 4K detail.
	float source_gate=smoothstep(0.012,0.035,max(max(high.r,high.g),high.b));
	mask*=source_gate;
	vec3 mapped=low_tone+fit.rgb*(high-low_scene);
	vec3 fused=mix(original.rgb,clamp(mapped,vec3(0),vec3(1)),mask);
	// The two sources now have the same geometry. Differences here primarily
	// represent UI drawn after the color surface, not unrelated world pixels.
	float overlay_delta=max(max(abs(screen.r-original.r),abs(screen.g-original.g)),abs(screen.b-original.b));
	// Do not lower this globally: GPU replay showed it removes fine scene
	// reconstruction from 1.5M gameplay pixels. Use the frame UI marker instead.
	float overlay=smoothstep(0.18,0.36,overlay_delta);
	// Dark/translucent late menu UI may have insufficient RGB contrast.
	// Alpha provides independent evidence; do not lower the scene detail mask.
	float alpha_ui=smoothstep(0.10,0.20,max(screen.a-original.a,0.0));
	overlay=1.0-(1.0-overlay)*(1.0-alpha_ui);
#ifdef TM4K_GUIDED
	// LIT 0.1: joint bilateral reconstruction of the game's correctly toned
	// colors. Guide RGB influences weights only, not the output palette.
	// Keep early UI and the known gameplay HUD out of this reconstruction.
	bool hud_region=uv.y>0.82 || (uv.x<0.15 && uv.y<0.28);
	float guide_ui_gate=ui_gate;
#ifdef TM4K_LIT_C
	vec3 predicted_tone=repeated_native_tone(uv,scene,low);
	vec3 prediction_delta=abs(original.rgb-predicted_tone);
	guide_ui_gate=1.0-smoothstep(0.07,0.22,max(max(prediction_delta.r,prediction_delta.g),prediction_delta.b));
#endif
	if (!hud_region && guide_ui_gate>0.0 && overlay<1.0)
	{
#ifdef TM4K_PACKED_ALPHA
		vec3 guide=decoded_guide_bilinear(uv);
		// Clamp each atlas half independently: never blend slope with bias.
		vec2 fit_pos=clamp(uv*vec2(low),vec2(0.5),vec2(low)-0.5);
		vec2 atlas_size=vec2(textureSize(Input8,0));
		float slope=texture(Input8,fit_pos/atlas_size).r;
		vec2 position=uv*vec2(low)-0.5;
		ivec2 center=ivec2(floor(position+0.5));
		vec3 total=vec3(0);
		float weight_sum=0.0;
		for (int y=-1;y<=1;++y)
		for (int x=-1;x<=1;++x)
		{
			ivec2 q=clamp(center+ivec2(x,y),ivec2(0),low-1);
			vec3 delta=guide-texelFetch(Input7,q,0).rgb;
			vec2 spatial=vec2(q)-position;
			// Offline C-edge-palette: narrower color similarity, unchanged geometry/detail cap.
#ifdef TM4K_MICRO_E_BASE
#ifdef TM4K_DETAIL_PALETTE
			// More selective native-tone reconstruction at fine material/color
			// boundaries. Guide supplies weights, never its ungraded RGB colors.
			float weight=exp(-dot(delta,delta)/0.008-dot(spatial,spatial)/1.28);
#else
			float weight=exp(-dot(delta,delta)/0.02-dot(spatial,spatial)/1.28);
#endif
#else
			float weight=exp(-dot(delta,delta)/0.012-dot(spatial,spatial)/1.28);
#endif
			total+=weight*texelFetch(Input2,q,0).rgb;
			weight_sum+=weight;
		}
		// High-resolution guide supplies structure, NEVER its RGB palette.
		vec3 native_color=weight_sum>1e-8 ? total/weight_sum : low_tone;
		const vec3 luma_weights=vec3(0.2126,0.7152,0.0722);
		// Transfer only the zero-mean, high-resolution residual. An absolute
		// slope*guide+bias target remaps broad shading/fog from a different
		// intermediate lighting stage and can create artificial dark rims.
		// Keep the game's native low-frequency lighting and hue authoritative.
		float coarse_luma=dot(texture(Input7,uv).rgb,luma_weights);
		float fine=clamp(slope*(dot(guide,luma_weights)-coarse_luma),-0.06,0.06);
		float ratio=clamp(1.0+fine/max(dot(native_color,luma_weights),0.015),0.5,1.5);
		// One common factor preserves hue; prevent per-channel clipping.
		ratio=min(ratio,1.0/max(max(max(native_color.r,native_color.g),native_color.b),0.015));
		vec3 reconstructed=native_color*ratio;
		float decoded_gate=smoothstep(0.008,0.03,max(max(guide.r,guide.g),guide.b));
		fused=mix(fused,reconstructed,guide_ui_gate*decoded_gate);
#else
		vec3 guide=texture(Input6,uv).rgb;
		vec2 position=uv*vec2(low)-0.5;
		ivec2 center=ivec2(floor(position+0.5));
		vec3 total=vec3(0);
		float weight_sum=0.0;
#ifdef TM4K_LIT_C
		vec3 guide_total=vec3(0), guide_squared=vec3(0), guide_tone=vec3(0);
#endif
		for (int y=-1;y<=1;++y)
		for (int x=-1;x<=1;++x)
		{
			ivec2 q=clamp(center+ivec2(x,y),ivec2(0),low-1);
			vec3 low_guide=texelFetch(Input7,q,0).rgb;
			vec3 tone=texelFetch(Input2,q,0).rgb;
			vec3 delta=guide-low_guide;
			vec2 spatial=vec2(q)-position;
			float weight=exp(-dot(delta,delta)/0.02-dot(spatial,spatial)/1.28);
			total+=weight*tone;
			weight_sum+=weight;
#ifdef TM4K_LIT_C
			guide_total+=weight*low_guide;
			guide_squared+=weight*low_guide*low_guide;
			guide_tone+=weight*low_guide*tone;
#endif
		}
		if (weight_sum>1e-8)
		{
			vec3 reconstructed=total/weight_sum;
#ifdef TM4K_LIT_C
			// User-selected C, exactly LIT luma 1 @ 0.1 from the offline sweep:
			// transfer high-res local structure as luminance, not guide hue.
			vec3 mean_guide=guide_total/weight_sum;
			vec3 variance=max(guide_squared/weight_sum-mean_guide*mean_guide,vec3(0));
			vec3 covariance=guide_tone/weight_sum-mean_guide*reconstructed;
			vec3 slope=clamp(covariance/(variance+vec3(0.0004)),vec3(0),vec3(1.5));
			vec3 affine=slope*(guide-mean_guide);
			const vec3 luma_weights=vec3(0.2126,0.7152,0.0722);
			float luma=dot(reconstructed,luma_weights);
			float fine=clamp(dot(affine,luma_weights),-0.06,0.06);
			float ratio=clamp((luma+fine)/max(luma,0.02),0.5,1.5);
			reconstructed=clamp(reconstructed*ratio,vec3(0),vec3(1));
#endif
			fused=mix(fused,reconstructed,guide_ui_gate);
		}
#endif
	}
#endif
	// Keep the validated HDR scene path. Adding screen-low_tone here also
	// imports coarse presentation edges (confirmed in bright frame 5256).
	// Late UI remains protected independently by the alpha evidence above.
	imageStore(OutputTexture,p,vec4(mix(fused,screen.rgb,overlay),screen.a));
}
)glsl";

	// Experimental, conservative one-frame morphological edge AA. This is not
	// the game's SPU MLAA and not the lookup-table based reference SMAA 1x.
	// Only staircase transitions in the 4K scene are touched; flat edges, thin
	// lines and pixels likely to be UI fall back to the existing fused frame.
	constexpr const char* tm4k_morphological_aa = R"glsl(
#version 450
layout(local_size_x=16, local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // fused 4K frame
layout(set=0,binding=1) uniform sampler2D Input1; // same-frame color scene
layout(set=0,binding=2) uniform sampler2D Input2; // native-resolution tone map
layout(set=0,binding=3) uniform sampler2D Input3; // original presented frame
layout(set=0,binding=4,rgba8) uniform writeonly image2D OutputTexture;

float lum(vec3 c) { return dot(c, vec3(0.2126, 0.7152, 0.0722)); }
float sample_luma(ivec2 p, ivec2 size)
{
	return lum(texelFetch(Input0, clamp(p, ivec2(0), size - 1), 0).rgb);
}
void main()
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	ivec2 full = imageSize(OutputTexture);
	if (any(greaterThanEqual(p, full))) return;
	vec4 center = texelFetch(Input0, p, 0);
	int scale = full.x / 1280;
	ivec2 origin = ivec2(39, 24) * scale;
	ivec2 area = ivec2(1202, 672) * scale;
	ivec2 relative = p - origin;
	if (any(lessThan(relative, ivec2(2))) || any(greaterThanEqual(relative, area - 2)))
	{
		imageStore(OutputTexture, p, center);
		return;
	}

	// The color surface and the true present frame carry different UI layers.
	// Protect both, so AA cannot blur menu letters, the minimap or HUD icons.
	vec2 uv = (vec2(relative) + 0.5) / vec2(area);
	vec3 color = texture(Input1, uv).rgb;
	ivec2 low_size = textureSize(Input2, 0);
	vec3 tone = texelFetch(Input2, clamp(ivec2(uv * vec2(low_size)), ivec2(0), low_size - 1), 0).rgb;
	vec3 present = texelFetch(Input3, p, 0).rgb;
	float early_ui = max(max(abs(color.r-tone.r), abs(color.g-tone.g)), abs(color.b-tone.b));
	float late_ui = max(max(abs(present.r-color.r), abs(present.g-color.g)), abs(present.b-color.b));
	if (early_ui > 0.07 || late_ui > 0.14)
	{
		imageStore(OutputTexture, p, center);
		return;
	}

	float c = lum(center.rgb);
	float l = sample_luma(p + ivec2(-1, 0), full);
	float r = sample_luma(p + ivec2( 1, 0), full);
	float u = sample_luma(p + ivec2( 0,-1), full);
	float d = sample_luma(p + ivec2( 0, 1), full);
	float contrast = max(max(abs(c-l), abs(c-r)), max(abs(c-u), abs(c-d)));
	float threshold = max(0.055, contrast * 0.30);
	if (contrast < 0.055 || (abs(c-l) > threshold && abs(c-r) > threshold) ||
		(abs(c-u) > threshold && abs(c-d) > threshold))
	{
		// Do not erase narrow fence bars, hair or bright texture scratches.
		imageStore(OutputTexture, p, center);
		return;
	}

	vec3 result = center.rgb;
	float vertical = max(abs(c-l), abs(c-r));
	float horizontal = max(abs(c-u), abs(c-d));
	if (vertical > horizontal && vertical > threshold)
	{
		int side = abs(c-l) > abs(c-r) ? -1 : 1;
		float up_edge = abs(sample_luma(p + ivec2(0,-1), full) - sample_luma(p + ivec2(side,-1), full));
		float down_edge = abs(sample_luma(p + ivec2(0,1), full) - sample_luma(p + ivec2(side,1), full));
		// A straight edge continues on both rows. One missing continuation is
		// a likely one-pixel stair step: smooth that corner only.
		if ((up_edge > threshold) != (down_edge > threshold))
			result = mix(result, texelFetch(Input0, p + ivec2(side,0), 0).rgb, 0.20);
	}
	else if (horizontal > threshold)
	{
		int side = abs(c-u) > abs(c-d) ? -1 : 1;
		float left_edge = abs(sample_luma(p + ivec2(-1,0), full) - sample_luma(p + ivec2(-1,side), full));
		float right_edge = abs(sample_luma(p + ivec2(1,0), full) - sample_luma(p + ivec2(1,side), full));
		if ((left_edge > threshold) != (right_edge > threshold))
			result = mix(result, texelFetch(Input0, p + ivec2(0,side), 0).rgb, 0.20);
	}
	imageStore(OutputTexture, p, vec4(result, center.a));
}
)glsl";

	struct tm4k_fusion_pass final : vk::upscaler
	{
		struct scene_packet
		{
			std::unique_ptr<vk::viewable_image> hdr, color, guide, early_color, particles;
			u64 hdr_tag = 0, color_tag = 0, guide_tag = 0, early_color_tag = 0;
			u64 particle_tag = 0;
		};
		std::array<scene_packet, 3> completed_scenes;
		u32 completed_scene_next = 0;
		u64 captured_color_tag = 0;
		bool capturing_scene = false;
		u32 last_route = 0; // diagnostic: 0=no fusion, 1=HDR, 2=LIT, 3=packed Alpha
		bool full_canvas_output = false;
		std::unique_ptr<vk::viewable_image> micro_base, micro_guide, micro_logs, micro_fit, micro_output;
		std::unique_ptr<tm4k_task> micro_base_merge, micro_crop_task, micro_decode_task, micro_logs_task, micro_fit_task, micro_merge_task;
		std::unique_ptr<tm4k_task> master_merge_task;
		std::unique_ptr<tm4k_task> master_colour_fit_task;
		std::unique_ptr<vk::viewable_image> master_colour_gain;
		std::unique_ptr<vk::viewable_image> affine_guide_low, affine_colour_low, affine_raw_fit, affine_fit, affine_class;
		std::unique_ptr<tm4k_task> affine_downsample, affine_fit_task, affine_smooth_task, affine_class_task;
		std::unique_ptr<vk::viewable_image> hybrid_guide, hybrid_colour, hybrid_raw_fit, hybrid_fit, hybrid_class, hybrid_scene, hybrid_residual;
		std::unique_ptr<tm4k_task> hybrid_down_guide, hybrid_down_colour, hybrid_fit_task, hybrid_smooth_task, hybrid_class_task, hybrid_predict_task, hybrid_residual_task;
		std::unique_ptr<vk::viewable_image> native_base, native_first, native_base_low, native_target_low, native_first_low, native_gain;
		std::unique_ptr<tm4k_task> native_base_task, native_mean_task, native_target_task, native_gain_task, native_gain2_task, native_gain3_task, native_apply_task;
		std::unique_ptr<vk::viewable_image> raw_low, tone_low, coefficients, merged, aa_output, guide_low, guide_coefficients;
		tm4k_task downsample_scene{tm4k_downsample_scene,1};
		tm4k_task downsample_final{tm4k_downsample_final,1};
		tm4k_task fit{tm4k_fit,4};
		tm4k_task merge{tm4k_merge,6};
		std::unique_ptr<tm4k_task> downsample_guide, guided_merge;
		std::unique_ptr<tm4k_task> fit_packed_guide;
		std::unique_ptr<tm4k_task> morphological_aa;
		std::unique_ptr<vk::viewable_image> particle_before, particle_accum, particle_scratch, particle_output;
		std::unique_ptr<tm4k_task> particle_extract, particle_compose;
		u64 particle_tag = 0, particle_taken_tag = 0, particle_generation = 0;

		bool copy_particle_image(vk::command_buffer& cmd, vk::viewable_image* image,
			std::unique_ptr<vk::viewable_image>& destination, u32 width, u32 height, const char* name)
		{
			if (!image || !image->value || image->format()!=VK_FORMAT_R16G16B16A16_SFLOAT ||
				image->width()<width || image->height()<height) return false;
			if (!destination)
			{
				auto* device=vk::get_current_renderer();
				destination=std::make_unique<vk::viewable_image>(*device,device->get_memory_mapping().device_local,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,VK_IMAGE_TYPE_2D,image->format(),width,height,1,1,1,
					VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
			}
			if (!destination->value) return false;
			destination->set_debug_name(name);
			const VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1};
			image->push_layout(cmd,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_TRANSFER_READ_BIT,range);
			vk::insert_image_memory_barrier(cmd,destination->value,destination->current_layout,destination->current_layout,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_TRANSFER_WRITE_BIT,range);
			destination->change_layout(cmd,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			const areai rect{0,0,static_cast<int>(width),static_cast<int>(height)};
			vk::copy_image(cmd,image,destination.get(),rect,rect);
			vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
				VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_ACCESS_TRANSFER_READ_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,range);
			image->pop_layout(cmd);
			vk::insert_image_memory_barrier(cmd,destination->value,destination->current_layout,destination->current_layout,
				VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_ACCESS_TRANSFER_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
			return true;
		}

		bool begin_particles(vk::command_buffer& cmd, vk::render_target* hdr, u64 generation)
		{
			if (!hdr || hdr->width()!=3840 || hdr->height()!=2160 || !generation) return false;
			hdr->memory_barrier(cmd,rsx::surface_access::shader_read);
			if (!copy_particle_image(cmd,hdr->get_surface(rsx::surface_access::shader_read),particle_before,1920,1080,"TM4K particle before native half-pass")) return false;
			if (!particle_accum)
			{
				if (!copy_particle_image(cmd,particle_before.get(),particle_accum,1920,1080,"TM4K particle accumulation") ||
					!copy_particle_image(cmd,particle_before.get(),particle_scratch,1920,1080,"TM4K particle scratch")) return false;
			}
			if (generation!=particle_generation)
			{
				const VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1};
				vk::insert_image_memory_barrier(cmd,particle_accum->value,particle_accum->current_layout,particle_accum->current_layout,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_TRANSFER_WRITE_BIT,range);
				particle_accum->change_layout(cmd,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
				const VkClearColorValue clear{};
				vkCmdClearColorImage(cmd,particle_accum->value,particle_accum->current_layout,&clear,1,&range);
				particle_generation=generation; particle_tag=0;
			}
			return true;
		}

		void end_particles(vk::command_buffer& cmd, vk::render_target* hdr)
		{
			if (!hdr || !particle_before || !particle_accum || !particle_scratch) return;
			auto* after=hdr->get_surface(rsx::surface_access::shader_read);
			if (!after || !after->value) return;
			const VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1};
			for (auto* image : {particle_before.get(),after,particle_accum.get()})
			{
				image->push_layout(cmd,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
			}
			particle_scratch->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
			vk::insert_image_memory_barrier(cmd,particle_scratch->value,particle_scratch->current_layout,particle_scratch->current_layout,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
			if (!particle_extract) particle_extract=std::make_unique<tm4k_task>(tm4k_particles::extract,3);
			particle_extract->execute(cmd,{particle_before.get(),after,particle_accum.get()},particle_scratch.get());
			for (auto* image : {particle_before.get(),after,particle_accum.get()})
			{
				vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_ACCESS_SHADER_READ_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,range);
				image->pop_layout(cmd);
			}
			particle_accum.swap(particle_scratch);
			particle_tag=hdr->last_use_tag;
			static u64 count=0;
			if (++count<=8 || count%120==0) rsx_log.notice("TM4K particle-isolation: count=%llu hdr=%llu generation=%llu half=1920x1080 native_blend=preserved",count,particle_tag,particle_generation);
		}

		~tm4k_fusion_pass() override
		{
			for(auto* image:{&native_base,&native_first,&native_base_low,&native_target_low,&native_first_low,&native_gain})
				if(*image)vk::get_resource_manager()->dispose(*image);
			for(auto* image:{&hybrid_guide,&hybrid_colour,&hybrid_raw_fit,&hybrid_fit,&hybrid_class,&hybrid_scene,&hybrid_residual})
				if(*image)vk::get_resource_manager()->dispose(*image);
			if (master_colour_gain) vk::get_resource_manager()->dispose(master_colour_gain);
			for (auto* image : { &affine_guide_low, &affine_colour_low, &affine_raw_fit, &affine_fit, &affine_class })
				if (*image) vk::get_resource_manager()->dispose(*image);
			for (auto* image : { &micro_base, &micro_guide, &micro_logs, &micro_fit, &micro_output })
				if (*image) vk::get_resource_manager()->dispose(*image);
			for (auto& packet : completed_scenes)
				for (auto* image : { &packet.hdr, &packet.color, &packet.guide, &packet.early_color, &packet.particles })
					if (*image) vk::get_resource_manager()->dispose(*image);
			for (auto* image : { &particle_before, &particle_accum, &particle_scratch, &particle_output })
				if (*image) vk::get_resource_manager()->dispose(*image);
			for (auto* image : { &raw_low, &tone_low, &coefficients, &merged, &aa_output, &guide_low, &guide_coefficients })
				if (*image) vk::get_resource_manager()->dispose(*image);
		}

		bool capture_scene(vk::command_buffer& cmd, vk::render_target* hdr,
			vk::render_target* color, vk::render_target* guide)
		{
			// Addresses alone do not establish a semantic role: c119 is also a
			// material attachment during the next geometry pass. Accept only the
			// observed producer ordering guide -> HDR -> post-tonemap color.
			if (capturing_scene || vk::is_renderpass_open(cmd) || !hdr || !color || !guide ||
				hdr->format() != VK_FORMAT_R16G16B16A16_SFLOAT ||
				color->format() != VK_FORMAT_B8G8R8A8_UNORM || guide->format() != VK_FORMAT_B8G8R8A8_UNORM ||
				hdr->samples() != VK_SAMPLE_COUNT_1_BIT || color->samples() != VK_SAMPLE_COUNT_1_BIT || guide->samples() != VK_SAMPLE_COUNT_1_BIT ||
				hdr->width() != color->width() || hdr->height() != color->height() ||
				hdr->width() != guide->width() || hdr->height() != guide->height() ||
				(hdr->width() != 3840 && hdr->width() != 5120) || hdr->height() !=
					((rsx::tm4k_scene_2160_requested() && hdr->width()==3840) ? 2160 : hdr->width()/1280*704) ||
				!guide->last_use_tag || !hdr->last_use_tag || !color->last_use_tag ||
				guide->last_use_tag > hdr->last_use_tag || hdr->last_use_tag > color->last_use_tag)
				return false;

			static const u64 max_intra_gap = []() -> u64
			{
				const char* setting = std::getenv("TM4K_INTRA_GAP");
				if (setting && *setting)
				{
					char* end = nullptr;
					const u64 val = std::strtoull(setting, &end, 10);
					if (val > 0) return val;
				}
				return 2500;
			}();
			if (hdr->last_use_tag - guide->last_use_tag >= max_intra_gap ||
				color->last_use_tag - hdr->last_use_tag >= max_intra_gap ||
				color->last_use_tag == captured_color_tag)
				return false;

			const u64 hdr_tag = hdr->last_use_tag, color_tag = color->last_use_tag, guide_tag = guide->last_use_tag;
			capturing_scene = true;
			for (auto* source : { hdr, color, guide })
				source->memory_barrier(cmd, rsx::surface_access::shader_read);
			if (hdr->last_use_tag != hdr_tag || color->last_use_tag != color_tag || guide->last_use_tag != guide_tag)
			{
				capturing_scene = false;
				return false; // A reload is not the already-completed producer.
			}
			// Repeated color writes for the SAME HDR/guide generation must not
			// consume multiple history slots. Actual log: hdr3494/guide3489 was
			// captured with color3542 and color3591 before either source advanced.
			// Keep the early color as well: a display older than the late color
			// must still be able to select the correctly ordered early snapshot.
			scene_packet* generation = nullptr;
			for (auto& candidate : completed_scenes)
				if (candidate.hdr_tag == hdr_tag && candidate.guide_tag == guide_tag &&
					candidate.hdr && candidate.guide && candidate.early_color_tag)
				{
					generation = &candidate;
					break;
				}
			const bool new_generation = !generation;
			auto& packet = generation ? *generation : completed_scenes[completed_scene_next];
			if (new_generation)
				packet.hdr_tag = packet.color_tag = packet.guide_tag = packet.early_color_tag = 0;
			else
				packet.color_tag = 0;
			auto copy = [&](vk::render_target* source, std::unique_ptr<vk::viewable_image>& destination, const char* role)
			{
				auto* image = source->get_surface(rsx::surface_access::shader_read);
				if (!image || !image->value) return false;
				if (!destination || destination->width() != image->width() || destination->height() != image->height() || destination->format() != image->format())
				{
					if (destination) vk::get_resource_manager()->dispose(destination);
					auto* device = vk::get_current_renderer();
					destination = std::make_unique<vk::viewable_image>(*device,
						device->get_memory_mapping().device_local, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						VK_IMAGE_TYPE_2D, image->format(), image->width(), image->height(), 1, 1, 1,
						VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
						vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3, VMM_ALLOCATION_POOL_SWAPCHAIN, RSX_FORMAT_CLASS_COLOR);
				}
				if (!destination->value) return false;
				destination->set_debug_name(fmt::format("TM4K completed-%s slot=%u hdr=%llu color=%llu guide=%llu", role, static_cast<u32>(&packet - completed_scenes.data()), hdr_tag, color_tag, guide_tag));
				const VkImageSubresourceRange range{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				image->push_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
				vk::insert_image_memory_barrier(cmd, image->value, image->current_layout, image->current_layout,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, range);
				vk::insert_image_memory_barrier(cmd, destination->value, destination->current_layout, destination->current_layout,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, range);
				destination->change_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
				const areai rect{ 0, 0, static_cast<int>(image->width()), static_cast<int>(image->height()) };
				vk::copy_image(cmd, image, destination.get(), rect, rect);
				vk::insert_image_memory_barrier(cmd, destination->value, destination->current_layout, destination->current_layout,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, range);
				vk::insert_image_memory_barrier(cmd, image->value, image->current_layout, image->current_layout,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
					VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT, range);
				image->pop_layout(cmd);
				return true;
			};
			const bool copied = new_generation
				? copy(hdr, packet.hdr, "hdr") && copy(color, packet.color, "color") &&
				  copy(guide, packet.guide, "guide") && copy(color, packet.early_color, "early-color")
				: copy(color, packet.color, "color");
			if (copied)
			{
				if (new_generation)
				{
					packet.particle_tag=0;
					if (particle_tag>particle_taken_tag && particle_tag<=hdr_tag && particle_accum &&
						copy_particle_image(cmd,particle_accum.get(),packet.particles,1920,1080,"TM4K completed native particle layer"))
					{
						packet.particle_tag=particle_tag; particle_taken_tag=particle_tag;
					}
				}
				packet.hdr_tag = hdr_tag; packet.color_tag = color_tag; packet.guide_tag = guide_tag;
				captured_color_tag = color_tag;
				if (new_generation)
				{
					packet.early_color_tag = color_tag;
					completed_scene_next = (completed_scene_next + 1) % completed_scenes.size();
				}
				static u64 captures = 0;
				if (++captures <= 8 || captures % 120 == 0)
					rsx_log.notice("TM4K generation snapshot: count=%llu new=%u hdr=%llu color=%llu guide=%llu early=%llu", captures, static_cast<u32>(new_generation), hdr_tag, color_tag, guide_tag, packet.early_color_tag);
			}
			capturing_scene = false;
			return copied;
		}

		scene_packet* select_completed_scene(u64 display_tag, bool& early_color)
		{
			static const u64 max_color_gap = []() -> u64
			{
				const char* setting = std::getenv("TM4K_COHORT_WINDOW");
				if (setting && *setting)
				{
					char* end = nullptr;
					const u64 val = std::strtoull(setting, &end, 10);
					if (val > 0)
					{
						rsx_log.notice("TM4K cohort-window: max_color_gap=%llu future_sources=reject snapshots_only=1", val);
						return val;
					}
				}
				return 4000;
			}();
			scene_packet* selected = nullptr;
			u64 selected_tag = 0;
			early_color = false;
			for (auto& packet : completed_scenes)
			{
				// The presentation write, not the continually advancing RSX tag,
				// is the reference. Never import next-frame material/lighting.
				if (!packet.hdr_tag || !packet.guide_tag || !packet.hdr || !packet.guide)
					continue;
				for (const bool use_early : { false, true })
				{
					const u64 tag = use_early ? packet.early_color_tag : packet.color_tag;
					const auto* image = use_early ? packet.early_color.get() : packet.color.get();
					if (!image || !tm4k_timing::completed_color_eligible(tag, display_tag, max_color_gap))
						continue;
					if (tag > selected_tag)
					{
						selected = &packet;
						selected_tag = tag;
						early_color = use_early;
					}
				}
			}
			if (!selected)
			{
				// Emergency burst fallback during heavy collision / ramming spikes:
				// Pick the latest completed packet that does not exceed display_tag
				for (auto& packet : completed_scenes)
				{
					if (!packet.hdr_tag || !packet.guide_tag || !packet.hdr || !packet.guide)
						continue;
					for (const bool use_early : { false, true })
					{
						const u64 tag = use_early ? packet.early_color_tag : packet.color_tag;
						const auto* image = use_early ? packet.early_color.get() : packet.color.get();
						if (!image || !tag || tag > display_tag)
							continue;
						if (display_tag - tag < 12000 && tag > selected_tag)
						{
							selected = &packet;
							selected_tag = tag;
							early_color = use_early;
						}
					}
				}
				if (selected)
				{
					static u64 spike_count = 0;
					if (++spike_count <= 10 || spike_count % 60 == 0)
						rsx_log.notice("TM4K collision-spike-fallback: display_tag=%llu color_tag=%llu gap=%llu 4K preserved", display_tag, selected_tag, display_tag - selected_tag);
				}
			}
			return selected;
		}

		scene_packet* select_geometry_predecessor(const scene_packet& color_packet)
		{
			// Opt-in diagnostic candidate: frame5064 proves independently moving
			// geometry in the chosen guide is ahead of the authoritative color.
			// Preserve color/UI timing; only try the previous distinct HDR/guide
			// generation. Tags are write ordering, NOT proof of a guest frame ID.
			// Never move UVs or blend historical display images to hide the error.
			scene_packet* selected = nullptr;
			for (auto& packet : completed_scenes)
			{
				if (!packet.hdr || !packet.guide || !packet.hdr_tag || !packet.guide_tag ||
					packet.hdr_tag >= color_packet.hdr_tag || packet.guide_tag >= color_packet.guide_tag ||
					packet.hdr->width() != color_packet.hdr->width() ||
					packet.hdr->height() != color_packet.hdr->height() ||
					packet.guide->width() != color_packet.guide->width() ||
					packet.guide->height() != color_packet.guide->height())
					continue;
				if (!selected || packet.hdr_tag > selected->hdr_tag)
					selected = &packet;
			}
			return selected;
		}

		vk::viewable_image* scale_output(const vk::command_buffer&, vk::viewable_image*,
			VkImage, VkImageLayout, const VkImageBlit&, rsx::flags32_t) override
		{
			return nullptr; // This title-specific pass is not the normal output upscaler.
		}

		vk::viewable_image* run(const vk::command_buffer& cmd, vk::viewable_image* raw,
			vk::viewable_image* color, vk::viewable_image* final, bool aa_enabled,
			vk::viewable_image* guide = nullptr, bool lit_c_enabled = false, bool packed_alpha_enabled = false, bool micro_e_enabled = false)
		{
			last_route=0;
			full_canvas_output=false;
			const u32 scale=final ? final->width()/1280 : 0;
			const bool scene_2160=rsx::tm4k_scene_2160_requested() && scale==3;
			if (!raw || !color || !final || !raw->value || !color->value || !final->value ||
				// This transform is defined for an FP16 HDR source, not another
				// same-sized material/normal/packed-lighting attachment.
				raw->format() != VK_FORMAT_R16G16B16A16_SFLOAT ||
				(color->format() != VK_FORMAT_B8G8R8A8_UNORM && color->format() != VK_FORMAT_R8G8B8A8_UNORM) ||
				raw == color || raw == final || color == final ||
				raw->samples() != VK_SAMPLE_COUNT_1_BIT || color->samples() != VK_SAMPLE_COUNT_1_BIT || final->samples() != VK_SAMPLE_COUNT_1_BIT ||
				(scale != 3 && scale != 4) ||
				raw->width() != 1280*scale || raw->height() != (scene_2160 ? 2160 : 704*scale) ||
				final->width() != 1280*scale || final->height() != 720*scale ||
				raw->width() != color->width() || raw->height() != color->height() ||
				final->width() != raw->width() || final->height() < raw->height() ||
				(raw->width() % scale) || (raw->height() % scale)) return nullptr;

			// Native SPU tone/color lattice stays 1280x704 even when the host
			// scene is rasterized at 3840x2160. Do not invent 16 guest rows.
			const u32 lw=raw->width()/scale, lh=scene_2160 ? 704 : raw->height()/scale;
			if (guide && (!guide->value || guide->width()!=raw->width() || guide->height()!=raw->height() ||
				(guide->format()!=VK_FORMAT_B8G8R8A8_UNORM && guide->format()!=VK_FORMAT_R8G8B8A8_UNORM)))
				guide=nullptr;
			if (!merged || merged->width()!=final->width() || merged->height()!=final->height())
			{
			for (auto* image : { &raw_low, &tone_low, &coefficients, &merged, &aa_output, &guide_low, &guide_coefficients })
					if (*image) vk::get_resource_manager()->dispose(*image);
				auto device=vk::get_current_renderer();
				auto make_image=[&](VkFormat format,u32 w,u32 h)
				{
					return std::make_unique<vk::viewable_image>(*device,
						device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						VK_IMAGE_TYPE_2D,format,w,h,1,1,1,VK_SAMPLE_COUNT_1_BIT,
						VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
						vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
				};
				raw_low=make_image(VK_FORMAT_R8G8B8A8_UNORM,lw,lh);
				tone_low=make_image(VK_FORMAT_R8G8B8A8_UNORM,lw,lh);
				coefficients=make_image(VK_FORMAT_R16G16B16A16_SFLOAT,lw,lh);
				merged=make_image(VK_FORMAT_R8G8B8A8_UNORM,final->width(),final->height());
				if (aa_enabled)
					aa_output=make_image(VK_FORMAT_R8G8B8A8_UNORM,final->width(),final->height());
				if (!raw_low->value || !tone_low->value || !coefficients->value || !merged->value)
					return nullptr;
			}
			if (guide && !guide_low)
			{
				auto device=vk::get_current_renderer();
				guide_low=std::make_unique<vk::viewable_image>(*device,
					device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					VK_IMAGE_TYPE_2D,packed_alpha_enabled ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM,lw,lh,1,1,1,
					VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
					vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
				if (!guide_low->value) guide=nullptr;
			}
			if (guide && !guide_low->value) guide=nullptr;
			if (guide && packed_alpha_enabled && !guide_coefficients)
			{
				auto device=vk::get_current_renderer();
				guide_coefficients=std::make_unique<vk::viewable_image>(*device,
					device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					VK_IMAGE_TYPE_2D,VK_FORMAT_R16G16B16A16_SFLOAT,lw*2,lh,1,1,1,
					VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
					vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
			}
			if (guide && packed_alpha_enabled && !guide_coefficients->value) guide=nullptr;
			// First live E test is defined at the approved 3840x2160 scale only.
			// Other scales retain C; do not silently change the filter's pixel radius.
			bool use_micro=micro_e_enabled && guide && packed_alpha_enabled && scale==3 && !aa_enabled;
			static const bool master_requested = []()
			{
				const char* setting = std::getenv("TM4K_MASTER_DENSITY");
				return setting && (std::string_view(setting) == "1" || std::string_view(setting) == "2" || std::string_view(setting) == "3" || std::string_view(setting) == "4" || std::string_view(setting) == "5" || std::string_view(setting) == "6" || std::string_view(setting) == "7");
			}();
			static const bool protected_master_requested = []()
			{
				const char* setting = std::getenv("TM4K_MASTER_DENSITY");
				return setting && std::string_view(setting) == "2";
			}();
			static const bool colour_master_requested = []()
			{
				const char* setting = std::getenv("TM4K_MASTER_DENSITY");
				return setting && std::string_view(setting) == "3";
			}();
			static const bool affine_master_requested = []()
			{
				const char* setting = std::getenv("TM4K_MASTER_DENSITY");
				return setting && std::string_view(setting) == "4";
			}();
			const bool use_master = master_requested && use_micro;
			static const bool native_consistency_requested = []() { const char* s=std::getenv("TM4K_MASTER_DENSITY");return s && std::string_view(s)=="7"; }();
			static const bool native_colour_requested = []() { const char* s=std::getenv("TM4K_MASTER_DENSITY");return s && (std::string_view(s)=="6" || std::string_view(s)=="7"); }();
			static const bool hybrid_master_requested = []() { const char* s=std::getenv("TM4K_MASTER_DENSITY");return s && (std::string_view(s)=="5" || std::string_view(s)=="6" || std::string_view(s)=="7"); }();
			if (use_master && hybrid_master_requested && !hybrid_guide)
			{
				auto* device=vk::get_current_renderer();
				auto make=[&](u32 w,u32 h,const char* name) {
					auto image=std::make_unique<vk::viewable_image>(*device,device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						VK_IMAGE_TYPE_2D,VK_FORMAT_R32G32B32A32_SFLOAT,w,h,1,1,1,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
						vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);image->set_debug_name(name);return image;
				};
				hybrid_guide=make(1280,720,"TM4K hybrid native720 guide");hybrid_colour=make(1280,720,"TM4K hybrid native720 colour");
				hybrid_raw_fit=make(2560,720,"TM4K hybrid raw fields");hybrid_fit=make(2560,720,"TM4K hybrid smoothed fields");
				hybrid_class=make(1,1,"TM4K hybrid route");hybrid_scene=make(3840,2160,"TM4K hybrid linear scene");hybrid_residual=make(1280,720,"TM4K hybrid consistency");
				if(native_colour_requested) {
					native_base=make(3840,2160,"TM4K native base");native_first=make(3840,2160,"TM4K native first");
					native_base_low=make(1280,720,"TM4K native base mean");native_target_low=make(1280,720,"TM4K native target mean");
					native_first_low=make(1280,720,"TM4K native first mean");native_gain=make(1280,720,"TM4K native log gain");
				}
			}
			if (use_master && native_colour_requested)
				for(auto* image:{native_base.get(),native_first.get(),native_base_low.get(),native_target_low.get(),native_first_low.get(),native_gain.get()})
					if(!image || !image->value)return nullptr;
			if (use_master && hybrid_master_requested)
				for(auto* image:{hybrid_guide.get(),hybrid_colour.get(),hybrid_raw_fit.get(),hybrid_fit.get(),hybrid_class.get(),hybrid_scene.get(),hybrid_residual.get()})
					if(!image||!image->value)return nullptr;
			if (use_master && affine_master_requested && !affine_guide_low)
			{
				auto* device=vk::get_current_renderer();
				auto make_affine=[&](u32 width,u32 height,const char* name)
				{
					auto image=std::make_unique<vk::viewable_image>(*device,
						device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						VK_IMAGE_TYPE_2D,VK_FORMAT_R32G32B32A32_SFLOAT,width,height,1,1,1,
						VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
						vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
					image->set_debug_name(name);return image;
				};
				affine_guide_low=make_affine(lw,lh,"TM4K affine linear guide");
				affine_colour_low=make_affine(lw,lh,"TM4K affine linear native colour");
				affine_raw_fit=make_affine(lw*2,lh,"TM4K affine raw slope bias atlas");
				affine_fit=make_affine(lw*2,lh,"TM4K affine smoothed slope bias atlas");
				affine_class=make_affine(1,1,"TM4K affine scene class scores");
			}
			if (use_master && affine_master_requested)
				for (auto* image : {affine_guide_low.get(),affine_colour_low.get(),affine_raw_fit.get(),affine_fit.get(),affine_class.get()})
					if (!image || !image->value) return nullptr;
			if (use_master && colour_master_requested && !master_colour_gain)
			{
				auto* device=vk::get_current_renderer();
				master_colour_gain=std::make_unique<vk::viewable_image>(*device,
					device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					VK_IMAGE_TYPE_2D,VK_FORMAT_R16G16B16A16_SFLOAT,lw,lh,1,1,1,
					VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
					vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
				if (!master_colour_gain->value) return nullptr;
			}
			if (use_micro && !micro_base)
			{
				auto* device=vk::get_current_renderer();
				auto make_micro=[&](VkFormat format)
				{
					return std::make_unique<vk::viewable_image>(*device,
						device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						VK_IMAGE_TYPE_2D,format,3840,2160,1,1,1,VK_SAMPLE_COUNT_1_BIT,
						VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
						vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
				};
				micro_base=make_micro(VK_FORMAT_R8G8B8A8_UNORM);
				micro_guide=make_micro(VK_FORMAT_R8G8B8A8_UNORM);
				micro_logs=make_micro(VK_FORMAT_R32G32B32A32_SFLOAT);
				micro_fit=make_micro(VK_FORMAT_R32G32B32A32_SFLOAT);
				micro_output=make_micro(VK_FORMAT_R8G8B8A8_UNORM);
			}
			if (use_micro)
				for (auto* image : {micro_base.get(),micro_guide.get(),micro_logs.get(),micro_fit.get(),micro_output.get()})
					if (!image || !image->value) use_micro=false;
			if (use_micro && !micro_base_merge)
			{
				std::string source=tm4k_merge;
				source.replace(source.find("#version 450"),12,"#version 450\n#define TM4K_GUIDED 1\n#define TM4K_LIT_C 1\n#define TM4K_PACKED_ALPHA 1\n#define TM4K_MICRO_E_BASE 1");
				const char* palette_setting=std::getenv("TM4K_DETAIL_PALETTE");
				if (palette_setting && std::string_view(palette_setting)=="1")
				{
					source.insert(source.find("#version 450")+12,"\n#define TM4K_DETAIL_PALETTE 1");
					rsx_log.notice("TM4K detail palette: native-tone bilateral epsilon=0.008 old=0.02 geometry=unchanged guide_RGB=not_output");
				}
				micro_base_merge=std::make_unique<tm4k_task>(source.c_str(),9);
				micro_crop_task=std::make_unique<tm4k_task>(tm4k_micro::crop,1);
				std::string decoder=tm4k_micro::decode;
				if (use_master)
					decoder.insert(decoder.find("#version 450")+12,"\n#define TM4K_MASTER_REFERENCE 1");
				micro_decode_task=std::make_unique<tm4k_task>(decoder.c_str(),1);
				micro_logs_task=std::make_unique<tm4k_task>(tm4k_micro::logs,2);
				micro_fit_task=std::make_unique<tm4k_task>(tm4k_micro::fit,1);
				micro_merge_task=std::make_unique<tm4k_task>(tm4k_micro::merge,4);
				if (use_master)
				{
					master_merge_task=std::make_unique<tm4k_task>(hybrid_master_requested ? tm4k_hybrid::merge : (affine_master_requested ? tm4k_affine::merge : (colour_master_requested ? tm4k_master::colour_merge : (protected_master_requested ? tm4k_master::protected_merge : tm4k_master::merge))),hybrid_master_requested ? 7 : (affine_master_requested ? 6 : (colour_master_requested ? 7 : (protected_master_requested ? 6 : 4))));
					if(hybrid_master_requested) {
						if(native_colour_requested) {
							master_merge_task=std::make_unique<tm4k_task>(tm4k_native_colour::merge,5);
							native_base_task=std::make_unique<tm4k_task>(tm4k_native_colour::base,5);
							native_mean_task=std::make_unique<tm4k_task>(tm4k_native_colour::mean,1);
							std::string target_source=tm4k_native_colour::mean;target_source.insert(target_source.find("#version 450")+12,"\n#define NATIVE_SRGB 1");
							native_target_task=std::make_unique<tm4k_task>(target_source.c_str(),1);
							native_gain_task=std::make_unique<tm4k_task>(tm4k_native_colour::gain,2);
							std::string gain_source=tm4k_native_colour::gain;gain_source.insert(gain_source.find("#version 450")+12,"\n#define SECOND_PASS 1");
							native_gain2_task=std::make_unique<tm4k_task>(gain_source.c_str(),2);
							if(native_consistency_requested) {
								std::string consistency_source=tm4k_native_colour::gain;
								consistency_source.insert(consistency_source.find("#version 450")+12,"\n#define CONSISTENCY_PASS 1");
								native_gain3_task=std::make_unique<tm4k_task>(consistency_source.c_str(),2);
							}
							native_apply_task=std::make_unique<tm4k_task>(tm4k_native_colour::apply,4);
						}
						hybrid_down_guide=std::make_unique<tm4k_task>(tm4k_hybrid::down_guide,1);hybrid_down_colour=std::make_unique<tm4k_task>(tm4k_hybrid::down_colour,1);
						hybrid_class_task=std::make_unique<tm4k_task>(tm4k_hybrid::classify,2);hybrid_fit_task=std::make_unique<tm4k_task>(tm4k_hybrid::fit,3);
						hybrid_smooth_task=std::make_unique<tm4k_task>(tm4k_hybrid::smooth,4);hybrid_predict_task=std::make_unique<tm4k_task>(tm4k_hybrid::predict,5);hybrid_residual_task=std::make_unique<tm4k_task>(tm4k_hybrid::residual,3);
					}
					if (colour_master_requested) master_colour_fit_task=std::make_unique<tm4k_task>(tm4k_master::colour_fit,2);
					if (affine_master_requested)
					{
						affine_downsample=std::make_unique<tm4k_task>(tm4k_affine::downsample,1);
						affine_fit_task=std::make_unique<tm4k_task>(tm4k_affine::fit,2);
						affine_smooth_task=std::make_unique<tm4k_task>(tm4k_affine::smooth,1);
						affine_class_task=std::make_unique<tm4k_task>(tm4k_affine::classify,1);
					}
				}
			}
			if (guide && !guided_merge)
			{
				std::string source=tm4k_merge;
				source.replace(source.find("#version 450"),12,"#version 450\n#define TM4K_GUIDED 1");
				if (lit_c_enabled)
					source.insert(source.find("#define TM4K_GUIDED 1"),"#define TM4K_LIT_C 1\n");
				std::string guide_source=tm4k_downsample_guide;
				if (packed_alpha_enabled)
				{
					source.insert(source.find("#define TM4K_GUIDED 1"),"#define TM4K_PACKED_ALPHA 1\n");
					guide_source.replace(guide_source.find("#version 450"),12,"#version 450\n#define TM4K_PACKED_ALPHA 1");
					fit_packed_guide=std::make_unique<tm4k_task>(tm4k_fit_packed_guide,2);
				}
				guided_merge=std::make_unique<tm4k_task>(source.c_str(),packed_alpha_enabled ? 9 : 8);
				downsample_guide=std::make_unique<tm4k_task>(guide_source.c_str(),1);
			}
			if (aa_enabled && !aa_output)
			{
				auto device=vk::get_current_renderer();
				aa_output=std::make_unique<vk::viewable_image>(*device,
					device->get_memory_mapping().device_local,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					VK_IMAGE_TYPE_2D,VK_FORMAT_R8G8B8A8_UNORM,final->width(),final->height(),1,1,1,
					VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
					vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
				if (!aa_output->value) return nullptr;
			}

			const VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1};
			for (auto* image : { raw_low.get(), tone_low.get(), coefficients.get(), merged.get(), aa_output.get(), guide_low.get(), guide_coefficients.get() })
			{
				if (!image || !image->value) continue;
				image->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
				vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
			}
			raw->push_layout(cmd,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			color->push_layout(cmd,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			final->push_layout(cmd,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			if (guide) guide->push_layout(cmd,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			// The generic image-layout helper targets graphics shader readers;
			// our title-specific consumers are COMPUTE. Make prior attachment,
			// transfer and shader writes visible to these compute texture reads.
			for (auto* image : { raw, color, final, guide })
			{
				if (!image) continue;
				vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
			}
			downsample_scene.execute(cmd,{raw},raw_low.get());
			downsample_final.execute(cmd,{color},tone_low.get());
			if (guide) downsample_guide->execute(cmd,{guide},guide_low.get());
			for (auto* image : { raw_low.get(), tone_low.get(), guide ? guide_low.get() : nullptr })
			{
				if (!image) continue;
				vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VK_ACCESS_SHADER_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
			}
			fit.execute(cmd,{raw_low.get(),tone_low.get(),final,color},coefficients.get());
			if (guide && packed_alpha_enabled)
			{
				fit_packed_guide->execute(cmd,{guide_low.get(),tone_low.get()},guide_coefficients.get());
				vk::insert_image_memory_barrier(cmd,guide_coefficients->value,guide_coefficients->current_layout,guide_coefficients->current_layout,
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VK_ACCESS_SHADER_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
			}
			vk::insert_image_memory_barrier(cmd,coefficients->value,coefficients->current_layout,coefficients->current_layout,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				VK_ACCESS_SHADER_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
			if (use_micro)
				micro_base_merge->execute(cmd,{raw,raw_low.get(),tone_low.get(),color,coefficients.get(),final,guide,guide_low.get(),guide_coefficients.get()},merged.get());
			else if (guide && packed_alpha_enabled)
				guided_merge->execute(cmd,{raw,raw_low.get(),tone_low.get(),color,coefficients.get(),final,guide,guide_low.get(),guide_coefficients.get()},merged.get());
			else if (guide)
				guided_merge->execute(cmd,{raw,raw_low.get(),tone_low.get(),color,coefficients.get(),final,guide,guide_low.get()},merged.get());
			else
				merge.execute(cmd,{raw,raw_low.get(),tone_low.get(),color,coefficients.get(),final},merged.get());
			if (use_micro)
			{
				auto readable=[&](vk::viewable_image* image)
				{
					vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
						VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VK_ACCESS_SHADER_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
				};
				for (auto* image : {micro_base.get(),micro_guide.get(),micro_logs.get(),micro_fit.get(),micro_output.get()})
				{
					image->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
					vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
				}
				readable(merged.get());
				micro_crop_task->execute(cmd,{merged.get()},micro_base.get());
				micro_decode_task->execute(cmd,{guide},micro_guide.get());
				readable(micro_base.get()); readable(micro_guide.get());
				micro_logs_task->execute(cmd,{micro_base.get(),micro_guide.get()},micro_logs.get());
				readable(micro_logs.get());
				micro_fit_task->execute(cmd,{micro_logs.get()},micro_fit.get());
				readable(micro_fit.get());
				if (use_master && hybrid_master_requested)
				{
					for(auto* image:{hybrid_guide.get(),hybrid_colour.get(),hybrid_raw_fit.get(),hybrid_fit.get(),hybrid_class.get(),hybrid_scene.get(),hybrid_residual.get()}) {
						image->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
						vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
					}
					hybrid_down_guide->execute(cmd,{micro_guide.get()},hybrid_guide.get());hybrid_down_colour->execute(cmd,{color},hybrid_colour.get());
					hybrid_class_task->execute(cmd,{final,color},hybrid_class.get());readable(hybrid_guide.get());readable(hybrid_colour.get());readable(hybrid_class.get());
					hybrid_fit_task->execute(cmd,{hybrid_guide.get(),hybrid_colour.get(),hybrid_class.get()},hybrid_raw_fit.get());readable(hybrid_raw_fit.get());
					hybrid_smooth_task->execute(cmd,{hybrid_raw_fit.get(),hybrid_guide.get(),hybrid_colour.get(),hybrid_class.get()},hybrid_fit.get());readable(hybrid_fit.get());
					hybrid_predict_task->execute(cmd,{micro_guide.get(),hybrid_guide.get(),hybrid_colour.get(),hybrid_fit.get(),hybrid_class.get()},hybrid_scene.get());readable(hybrid_scene.get());
					hybrid_residual_task->execute(cmd,{hybrid_scene.get(),hybrid_colour.get(),hybrid_guide.get()},hybrid_residual.get());readable(hybrid_residual.get());
					if(native_colour_requested) {
						auto writable=[&](vk::viewable_image* image) {
							image->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
							vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
						};
						for(auto* image:{native_base.get(),native_first.get(),native_base_low.get(),native_target_low.get(),native_first_low.get(),native_gain.get()})writable(image);
						native_base_task->execute(cmd,{hybrid_scene.get(),micro_guide.get(),hybrid_guide.get(),hybrid_residual.get(),hybrid_class.get()},native_base.get());readable(native_base.get());
						native_mean_task->execute(cmd,{native_base.get()},native_base_low.get());readable(native_base_low.get());
						native_target_task->execute(cmd,{color},native_target_low.get());readable(native_target_low.get());
						native_gain_task->execute(cmd,{native_target_low.get(),native_base_low.get()},native_gain.get());readable(native_gain.get());
						native_apply_task->execute(cmd,{native_base.get(),native_base.get(),native_base_low.get(),native_gain.get()},native_first.get());readable(native_first.get());
						native_mean_task->execute(cmd,{native_first.get()},native_first_low.get());readable(native_first_low.get());
						writable(native_gain.get());native_gain2_task->execute(cmd,{native_target_low.get(),native_first_low.get()},native_gain.get());readable(native_gain.get());
						writable(hybrid_scene.get());native_apply_task->execute(cmd,{native_first.get(),native_base.get(),native_base_low.get(),native_gain.get()},hybrid_scene.get());readable(hybrid_scene.get());
						vk::viewable_image* corrected_scene=hybrid_scene.get();
						if(native_consistency_requested) {
							// Ping-pong into the no-longer-needed first-pass image. Never read/write one image in-place.
							writable(native_first_low.get());native_mean_task->execute(cmd,{hybrid_scene.get()},native_first_low.get());readable(native_first_low.get());
							writable(native_gain.get());native_gain3_task->execute(cmd,{native_target_low.get(),native_first_low.get()},native_gain.get());readable(native_gain.get());
							writable(native_first.get());native_apply_task->execute(cmd,{hybrid_scene.get(),native_base.get(),native_base_low.get(),native_gain.get()},native_first.get());readable(native_first.get());
							corrected_scene=native_first.get();
						}
						master_merge_task->execute(cmd,{corrected_scene,color,final,hybrid_class.get(),micro_guide.get()},micro_output.get());
						static bool native_logged=false;if(!native_logged){
							if(native_consistency_requested)rsx_log.notice("TM4K native colour mode7: three_pass_log_RGB limits=32_8_2 eps=0.0005 range=0.05 spatial=0.85 3840x2160 UI_fire_separate no_history no_extra_framebuffer live_test_pending");
							else rsx_log.notice("TM4K native colour mode6: two_pass_log_RGB eps=0.0005 range=0.05 spatial=0.85 3840x2160 UI_fire_separate no_history live_test_pending");
							native_logged=true;
						}
					}
					else master_merge_task->execute(cmd,{hybrid_scene.get(),micro_guide.get(),hybrid_guide.get(),hybrid_residual.get(),color,final,hybrid_class.get()},micro_output.get());
					static bool logged=false;if(!logged){rsx_log.notice("TM4K hybrid mode5: native720 scene4K FP32 palette_other Gemini_sepia consistency alpha_UI crop118_73.5 fire_timing_unchanged live_test_pending");logged=true;}
				}
				else if (use_master && affine_master_requested)
				{
					// Frame-local coefficients and class; no retained temporal image.
					for (auto* image : {affine_guide_low.get(),affine_colour_low.get(),affine_raw_fit.get(),affine_fit.get(),affine_class.get()})
					{
						image->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
						vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
							VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
							VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
					}
					affine_downsample->execute(cmd,{micro_guide.get()},affine_guide_low.get());
					affine_downsample->execute(cmd,{color},affine_colour_low.get());
					affine_class_task->execute(cmd,{final},affine_class.get());
					readable(affine_guide_low.get());readable(affine_colour_low.get());
					affine_fit_task->execute(cmd,{affine_guide_low.get(),affine_colour_low.get()},affine_raw_fit.get());
					readable(affine_raw_fit.get());
					affine_smooth_task->execute(cmd,{affine_raw_fit.get()},affine_fit.get());
					readable(affine_fit.get());readable(affine_class.get());
					master_merge_task->execute(cmd,{micro_guide.get(),color,final,affine_fit.get(),coefficients.get(),affine_class.get()},micro_output.get());
					static bool affine_logged=false;
					if (!affine_logged)
					{
						rsx_log.notice("TM4K affine-preview mode4: canvas=3840x2160 FP32_linear_fit radius=3 eps=0.00005 slope=0.05:1.5 smooth_radius=2 native_UI=scene_class_rectangles logo=native frame_local=1 fire_and_timing=unchanged live_validation=pending");
						affine_logged=true;
					}
				}
				else if (use_master && colour_master_requested)
				{
					readable(guide_low.get());readable(tone_low.get());
					master_colour_gain->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
					vk::insert_image_memory_barrier(cmd,master_colour_gain->value,master_colour_gain->current_layout,master_colour_gain->current_layout,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
					master_colour_fit_task->execute(cmd,{guide_low.get(),tone_low.get()},master_colour_gain.get());
					readable(master_colour_gain.get());
					master_merge_task->execute(cmd,{micro_base.get(),micro_guide.get(),micro_logs.get(),coefficients.get(),color,final,master_colour_gain.get()},micro_output.get());
					static bool colour_scene_logged=false;
					if (!colour_scene_logged)
					{
						rsx_log.notice("TM4K direct colour: full_canvas=3840x2160 scene=decoded_guide RGB_gain=low_frequency_5x5 native_UI=authoritative_display no_UI_neighbour_erosion=1 geometry_UV=unchanged fire=separate");
						colour_scene_logged=true;
					}
				}
				else if (use_master && protected_master_requested)
				{
					master_merge_task->execute(cmd,{micro_base.get(),micro_guide.get(),micro_logs.get(),coefficients.get(),color,final},micro_output.get());
					static bool direct_scene_logged=false;
					if (!direct_scene_logged)
					{
						rsx_log.notice("TM4K direct scene: full_canvas=3840x2160 source=decoded_guide native_palette_fit=0 native_UI=independent missing_atmosphere=base fire=separate");
						direct_scene_logged=true;
					}
				}
				else if (use_master)
					master_merge_task->execute(cmd,{micro_base.get(),micro_guide.get(),micro_logs.get(),coefficients.get()},micro_output.get());
				else
					micro_merge_task->execute(cmd,{micro_base.get(),micro_logs.get(),micro_fit.get(),coefficients.get()},micro_output.get());
				full_canvas_output=true;
				static bool micro_logged=false;
				if (!micro_logged)
				{
					rsx_log.notice("TM4K micro E sky-safe active: full_canvas=3840x2160 guide=decoded-Lanczos RGB8 guided_radius=2 eps=0.015 strength=1.35 raw_RGB=0 meso=0 hue_preserved=1 atmosphere_confidence=0.12:0.65 valid_support_radius=4");
					micro_logged=true;
				}
				static bool master_logged=false;
				if (use_master && !master_logged)
				{
					rsx_log.notice("TM4K master-density active: 3840x2112 guide decoded RGB*alpha_byte exposure=0.5 Reinhard sRGB -> 3840x2160; direct_scene_RGB=1 native_UI_sky_fallback=1 particle_restore=0");
					master_logged=true;
				}
			}
			if (aa_enabled)
			{
				if (!morphological_aa)
					morphological_aa = std::make_unique<tm4k_task>(tm4k_morphological_aa,4);
				vk::insert_image_memory_barrier(cmd,merged->value,merged->current_layout,merged->current_layout,
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
					VK_ACCESS_SHADER_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
				morphological_aa->execute(cmd,{merged.get(),color,tone_low.get(),final},aa_output.get());
			}
			// Restoring a render target's layout must not race our compute
			// reads. The shared shader-read helper covers vertex/fragment only.
			for (auto* image : { raw, color, final, guide })
			{
				if (!image) continue;
				vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
					VK_ACCESS_SHADER_READ_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,range);
			}
			raw->pop_layout(cmd);
			color->pop_layout(cmd);
			final->pop_layout(cmd);
			if (guide) guide->pop_layout(cmd);
			// GENERAL->TRANSFER_SRC in the shared helper assumes attachment or
			// transfer producers, not COMPUTE storage writes. Explicitly publish
			// our result before the presentation blit or a graphics upscaler reads
			// it. No CPU wait/fence and no retained image from a previous frame.
			auto* result=use_micro ? micro_output.get() : (aa_enabled ? aa_output.get() : merged.get());
			// Use the effect packet belonging to the selected geometry generation,
			// not an independently newer particle image. Static captures cannot
			// prove the live temporal mapping; this path remains explicitly opt-in.
			vk::viewable_image* particle_layer=nullptr;
			if (use_micro)
				for (auto& packet : completed_scenes)
					if (packet.hdr.get()==raw && packet.particle_tag && packet.particles)
						particle_layer=packet.particles.get();
			if (particle_layer)
			{
				if (!particle_output)
				{
					auto* device=vk::get_current_renderer();
					particle_output=std::make_unique<vk::viewable_image>(*device,device->get_memory_mapping().device_local,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,VK_IMAGE_TYPE_2D,VK_FORMAT_R8G8B8A8_UNORM,3840,2160,1,1,1,
						VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
						vk::VK_IMAGE_CREATE_ALLOW_NULL_RPCS3,VMM_ALLOCATION_POOL_SWAPCHAIN,RSX_FORMAT_CLASS_COLOR);
				}
				if (particle_output->value)
				{
					for (auto* image : {result,particle_layer,color,final})
					{
						image->push_layout(cmd,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
						vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
							VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_READ_BIT,range);
					}
					particle_output->change_layout(cmd,VK_IMAGE_LAYOUT_GENERAL);
					vk::insert_image_memory_barrier(cmd,particle_output->value,particle_output->current_layout,particle_output->current_layout,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,range);
					if (!particle_compose) particle_compose=std::make_unique<tm4k_task>(tm4k_particles::compose,5);
					particle_compose->execute(cmd,{result,particle_layer,coefficients.get(),color,final},particle_output.get());
					for (auto* image : {result,particle_layer,color,final})
					{
						vk::insert_image_memory_barrier(cmd,image->value,image->current_layout,image->current_layout,
							VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_ACCESS_SHADER_READ_BIT,VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,range);
						image->pop_layout(cmd);
					}
					result=particle_output.get();
				}
			}
			vk::insert_image_memory_barrier(cmd,result->value,result->current_layout,result->current_layout,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_ACCESS_SHADER_WRITE_BIT,VK_ACCESS_MEMORY_READ_BIT,range);
			last_route=guide ? (packed_alpha_enabled ? 3u : 2u) : 1u;
			return result;
		}
	};

	VkFormat RSX_display_format_to_vk_format(u8 format)
	{
		switch (format)
		{
		default:
			rsx_log.error("Unhandled video output format 0x%x", static_cast<s32>(format));
			[[fallthrough]];
		case CELL_VIDEO_OUT_BUFFER_COLOR_FORMAT_X8R8G8B8:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case CELL_VIDEO_OUT_BUFFER_COLOR_FORMAT_X8B8G8R8:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case CELL_VIDEO_OUT_BUFFER_COLOR_FORMAT_R16G16B16X16_FLOAT:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		}
	}
}


void VKGSRender::tm4k_capture_completed_scene()
{
	if (Emu.GetTitleID() != "BCUS98106" || swapchain_unavailable ||
		!m_current_command_buffer || vk::is_renderpass_open(*m_current_command_buffer))
		return;
	static const bool enabled = std::ifstream(fs::get_config_dir() + "tm4k-enable.flag").good();
	static const bool packed = std::ifstream(fs::get_config_dir() + "tm4k-packed-alpha-enable.flag").good();
	if (!enabled || !packed) return;
	const auto hdr = m_rtts.get_surface_at(0xc1be0000);
	const auto color = m_rtts.get_surface_at(0xc1190000);
	const auto guide = m_rtts.get_surface_at(0xc0ab0000);
	if (!hdr || !color || !guide) return;
	if (!m_tm4k_fusion) m_tm4k_fusion = std::make_unique<tm4k_fusion_pass>();
	static_cast<tm4k_fusion_pass*>(m_tm4k_fusion.get())->capture_scene(*m_current_command_buffer, hdr, color, guide);
}

bool VKGSRender::tm4k_particle_begin()
{
	if (Emu.GetTitleID()!="BCUS98106" || !m_current_command_buffer || swapchain_unavailable) return false;
	static const bool enabled=[] { const char* p=std::getenv("TM4K_PARTICLE_RESTORE"); return p && std::string_view(p)=="1"; }();
	if (!enabled || !rsx::tm4k_scene_2160_requested() ||
		(m_current_command_buffer->flags & (vk::command_buffer::cb_has_open_query|vk::command_buffer::cb_has_conditional_render)) ||
		!rsx::method_registers.blend_enabled_mask() || m_scissor.offset.x || m_scissor.offset.y ||
		m_scissor.extent.width!=1920 || m_scissor.extent.height!=1080) return false;
	bool particle_tex=false;
	for (u32 mask=current_fp_metadata.referenced_textures_mask,i=0;mask;mask>>=1,++i)
	{
		if (!(mask&1)) continue;
		const auto& texture=rsx::method_registers.fragment_textures[i];
		if (!texture.enabled()) continue;
		// Flame animation atlas strips (64px high), soft-particle depth (1920x1056), or billboard particles (128..512)
		if (texture.height()==64) particle_tex=true;
		if (texture.width()==1920 && texture.height()==1056) particle_tex=true;
		if (texture.width()==texture.height() && texture.width()>=64 && texture.width()<=512) particle_tex=true;
	}
	if (!particle_tex) return false;
	auto* hdr=m_rtts.get_surface_at(0xc1be0000);
	auto* guide=m_rtts.get_surface_at(0xc0ab0000);
	if (!hdr || !guide || !std::any_of(m_rtts.m_bound_render_targets.begin(),m_rtts.m_bound_render_targets.end(),
		[&](const auto& rtt){return std::get<1>(rtt)==hdr;})) return false;
	// end_renderpass is not an idempotent helper: it unconditionally emits
	// vkCmdEndRenderPass. WER 2026-10-03 pinpoints that call on the first
	// particle draw after the preceding render pass has already ended.
	if (vk::is_renderpass_open(*m_current_command_buffer))
		vk::end_renderpass(*m_current_command_buffer);
	m_current_command_buffer->flags |= vk::command_buffer::cb_reload_dynamic_state;
	if (!m_tm4k_fusion) m_tm4k_fusion=std::make_unique<tm4k_fusion_pass>();
	return static_cast<tm4k_fusion_pass*>(m_tm4k_fusion.get())->begin_particles(*m_current_command_buffer,hdr,guide->last_use_tag);
}

void VKGSRender::tm4k_particle_end()
{
	if (!m_tm4k_fusion || !m_current_command_buffer ||
		(m_current_command_buffer->flags & vk::command_buffer::cb_has_open_query)) return;
	if (vk::is_renderpass_open(*m_current_command_buffer))
		vk::end_renderpass(*m_current_command_buffer);
	m_current_command_buffer->flags |= vk::command_buffer::cb_reload_dynamic_state;
	static_cast<tm4k_fusion_pass*>(m_tm4k_fusion.get())->end_particles(*m_current_command_buffer,m_rtts.get_surface_at(0xc1be0000));
}

bool VKGSRender::reinitialize_swapchain()
{
	m_swapchain_dims.width = m_frame->client_width();
	m_swapchain_dims.height = m_frame->client_height();

	// Reject requests to acquire new swapchain if the window is minimized
	// The NVIDIA driver will spam VK_ERROR_OUT_OF_DATE_KHR if you try to acquire an image from the swapchain and the window is minimized
	// However, any attempt to actually renew the swapchain will crash the driver with VK_ERROR_DEVICE_LOST while the window is in this state
	if (m_swapchain_dims.width == 0 || m_swapchain_dims.height == 0)
	{
		swapchain_unavailable = true;
		return false;
	}

	// NOTE: This operation will create a hard sync point
	close_and_submit_command_buffer();
	m_current_command_buffer->reset();
	m_current_command_buffer->begin();

	for (auto &ctx : m_frame_context_storage)
	{
		if (ctx.present_image == umax)
			continue;

		// Release present image by presenting it
		frame_context_cleanup(&ctx);
	}

	// NOTE: frame_context_cleanup alters the queued_frames structure.
	while (!m_queued_frames.empty())
	{
		auto& frame = m_queued_frames.front();
		if (!frame->swap_command_buffer)
		{
			// Drop it
			m_queued_frames.pop_front();
			continue;
		}

		frame_context_cleanup(frame);
	}
	ensure(m_queued_frames.empty());

	// Discard the current upscaling pipeline if any
	m_tm4k_fusion.reset();
	m_upscaler.reset();

	// Drain all the queues
	vkDeviceWaitIdle(*m_device);

	// Clean the FBO caches
	for (u32 i = 0; i < m_swapchain->get_swap_image_count(); ++i)
	{
		vk::remove_framebuffers_with_image(m_swapchain->get_image(i));
	}

	// Reset frame context storage
	for (auto& ctx : m_frame_context_storage)
	{
		ctx.destroy(*m_device);
	}
	m_current_frame = nullptr;
	m_max_async_frames = 0;
	m_current_queue_index = 0;
	m_frame_context_storage.clear();

	// Rebuild swapchain. Old swapchain destruction is handled by the init_swapchain call
	if (!m_swapchain->init(m_swapchain_dims.width, m_swapchain_dims.height))
	{
		rsx_log.warning("Swapchain initialization failed. Request ignored [%dx%d]", m_swapchain_dims.width, m_swapchain_dims.height);
		swapchain_unavailable = true;
		return false;
	}

	// Re-initialize CPU frame contexts
	m_max_async_frames = m_swapchain->get_swap_image_count();
	m_frame_context_storage.resize(m_max_async_frames);
	for (auto& ctx : m_frame_context_storage)
	{
		ctx.init(*m_device);
	}
	m_current_queue_index = 0;
	m_current_frame = &m_frame_context_storage[0];

	// Prepare new swapchain images for use
	for (u32 i = 0; i < m_swapchain->get_swap_image_count(); ++i)
	{
		const auto target_layout = m_swapchain->get_optimal_present_layout();
		const auto target_image = m_swapchain->get_image(i);
		VkClearColorValue clear_color{};
		VkImageSubresourceRange range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

		vk::change_image_layout(*m_current_command_buffer, target_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);
		vkCmdClearColorImage(*m_current_command_buffer, target_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &range);
		vk::change_image_layout(*m_current_command_buffer, target_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, target_layout, range);
	}

	// Will have to block until rendering is completed
	vk::fence resize_fence(*m_device);

	// Flush the command buffer
	close_and_submit_command_buffer(&resize_fence);
	vk::wait_for_fence(&resize_fence);

	m_current_command_buffer->reset();
	m_current_command_buffer->begin();

	swapchain_unavailable = false;
	should_reinitialize_swapchain = false;
	m_vsync_mode = g_cfg.video.vsync;
	return true;
}

void VKGSRender::present(vk::frame_context_t *ctx)
{
	ensure(ctx->present_image != umax);

	// Partial CS flush
	ctx->swap_command_buffer->flush();

	if (!swapchain_unavailable)
	{
		switch (VkResult error = m_swapchain->present(ctx->present_wait_semaphore, ctx->present_image))
		{
		case VK_SUCCESS:
			break;
		case VK_SUBOPTIMAL_KHR:
#ifndef ANDROID
			should_reinitialize_swapchain = true;
#endif
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			swapchain_unavailable = true;
			break;
		default:
			// Other errors not part of rpcs3. This can be caused by 3rd party injectors with bad code, of which we have no control over.
			// Let the application attempt to recover instead of crashing outright.
			rsx_log.error("VkPresent returned unexpected error code %lld. Will attempt to recreate the swapchain. Please disable 3rd party injector tools.", static_cast<s64>(error));
			swapchain_unavailable = true;
			break;
		}
	}

	// Presentation image released; reset value
	ctx->present_image = -1;
}

void VKGSRender::advance_queued_frames()
{
	// Check all other frames for completion and clear resources
	check_present_status();

	// Run video memory balancer
	m_device->rebalance_memory_type_usage();
	vk::vmm_check_memory_usage();

	// m_rtts storage is double buffered and should be safe to tag on frame boundary
	m_rtts.trim(*m_current_command_buffer, vk::vmm_determine_memory_load_severity());

	// Texture cache is also double buffered to prevent use-after-free
	m_texture_cache.on_frame_end();
	m_samplers_dirty.store(true);

	vk::remove_unused_framebuffers();

	m_vertex_cache->purge();
	m_current_frame->tag_frame_end();

	m_queued_frames.push_back(m_current_frame);
	ensure(m_queued_frames.size() <= m_max_async_frames);

	m_current_queue_index = (m_current_queue_index + 1) % m_max_async_frames;
	m_current_frame = &m_frame_context_storage[m_current_queue_index];
	m_current_frame->flags |= frame_context_state::dirty;

	vk::advance_frame_counter();
}

void VKGSRender::queue_swap_request()
{
	ensure(!m_current_frame->swap_command_buffer);
	m_current_frame->swap_command_buffer = m_current_command_buffer;

	if (m_swapchain->is_headless())
	{
		m_swapchain->end_frame(*m_current_command_buffer, m_current_frame->present_image);
		close_and_submit_command_buffer();
	}
	else
	{
		close_and_submit_command_buffer(nullptr,
			m_current_frame->acquire_signal_semaphore,
			m_current_frame->present_wait_semaphore,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT);
	}

	// Set up a present request for this frame as well
	present(m_current_frame);

	// Grab next cb in line and make it usable
	m_current_command_buffer = m_primary_cb_list.next();
	m_current_command_buffer->reset();
	m_current_command_buffer->begin();

	// Set up new pointers for the next frame
	advance_queued_frames();
}

void VKGSRender::frame_context_cleanup(vk::frame_context_t *ctx)
{
	ensure(ctx->swap_command_buffer);

	// Perform hard swap here
	if (ctx->swap_command_buffer->wait(FRAME_PRESENT_TIMEOUT) != VK_SUCCESS)
	{
		// Lost surface/device, release swapchain
		swapchain_unavailable = true;
	}

	// Resource cleanup.
	{
		if (m_overlay_manager && m_overlay_manager->has_dirty())
		{
			auto ui_renderer = vk::get_overlay_pass<vk::ui_overlay_renderer>();
			m_overlay_manager->lock_shared();

			std::vector<u32> uids_to_dispose;
			uids_to_dispose.reserve(m_overlay_manager->get_dirty().size());

			for (const auto& view : m_overlay_manager->get_dirty())
			{
				ui_renderer->remove_temp_resources(view->uid);
				uids_to_dispose.push_back(view->uid);
			}

			m_overlay_manager->unlock_shared();
			m_overlay_manager->dispose(uids_to_dispose);
		}

		vk::get_resource_manager()->trim();

		vk::reset_global_resources();

		if (ctx->last_frame_sync_time > m_last_heap_sync_time)
		{
			m_last_heap_sync_time = ctx->last_frame_sync_time;

			// Heap cleanup; deallocates memory consumed by the frame if it is still held
			vk::data_heap_manager::restore_snapshot(ctx->heap_snapshot);
		}
	}

	ctx->swap_command_buffer = nullptr;

	// Remove from queued list
	while (!m_queued_frames.empty())
	{
		auto frame = m_queued_frames.front();
		m_queued_frames.pop_front();

		if (frame == ctx)
		{
			break;
		}
	}

	vk::advance_completed_frame_counter();
}

vk::viewable_image* VKGSRender::get_present_source(/* inout */ vk::present_surface_info* info, const rsx::avconf& avconfig)
{
	vk::viewable_image* image_to_flip = nullptr;

	// @FIXME: This entire function needs to be rewritten to go through the texture cache's "upload_texture" routine.
	// That method is not a 1:1 replacement due to handling of insets that is done differently here.

	// Check the surface store first
	const auto format_bpp = rsx::get_format_block_size_in_bytes(info->format);
	const auto overlap_info = m_rtts.get_merged_texture_memory_region(*m_current_command_buffer,
		info->address, info->width, info->height, info->pitch, format_bpp, rsx::surface_access::transfer_read);

	if (!overlap_info.empty())
	{
		const auto& section = overlap_info.back();
		auto surface = vk::as_rtt(section.surface);
		bool viable = false;

		if (section.base_address >= info->address)
		{
			const auto surface_width = surface->get_surface_width<rsx::surface_metrics::samples>();
			const auto surface_height = surface->get_surface_height<rsx::surface_metrics::samples>();

			if (section.base_address == info->address)
			{
				// Check for fit or crop
				viable = (surface_width >= info->width && surface_height >= info->height);
			}
			else
			{
				// Check for borders and letterboxing
				const u32 inset_offset = section.base_address - info->address;
				const u32 inset_y = inset_offset / info->pitch;
				const u32 inset_x = (inset_offset % info->pitch) / format_bpp;

				const u32 full_width = surface_width + inset_x + inset_x;
				const u32 full_height = surface_height + inset_y + inset_y;

				viable = (full_width == info->width && full_height == info->height);
			}

			if (viable)
			{
				image_to_flip = section.surface->get_surface(rsx::surface_access::transfer_read);

				std::tie(info->width, info->height) = rsx::apply_resolution_scale<true>(
					resolution_scaling_config,
					std::min(surface_width, info->width),
					std::min(surface_height, info->height));
			}
		}
	}
	else if (auto surface = m_texture_cache.find_texture_from_dimensions<true>(info->address, info->format);
			 surface && surface->get_width() >= info->width && surface->get_height() >= info->height)
	{
		// Hack - this should be the first location to check for output
		// The render might have been done offscreen or in software and a blit used to display
		image_to_flip = dynamic_cast<vk::viewable_image*>(surface->get_raw_texture());
	}

	// The correct output format is determined by the AV configuration set in CellVideoOutConfigure by the game.
	// 99.9% of the time, this will match the backbuffer fbo format used in rendering/compositing the output.
	// But in some cases, let's just say some devs are creative.
	const auto expected_format = RSX_display_format_to_vk_format(avconfig.format);

	if (!image_to_flip) [[ unlikely ]]
	{
		// Read from cell
		const auto range = utils::address_range32::start_length(info->address, info->pitch * info->height);
		const u32  lookup_mask = rsx::texture_upload_context::blit_engine_dst | rsx::texture_upload_context::framebuffer_storage;
		const auto overlap = m_texture_cache.find_texture_from_range<true>(range, 0, lookup_mask);

		for (const auto & section : overlap)
		{
			if (!section->is_synchronized())
			{
				section->copy_texture(*m_current_command_buffer, true);
			}
		}

		if (m_current_command_buffer->flags & vk::command_buffer::cb_has_dma_transfer)
		{
			// Submit for processing to lower hard fault penalty
			flush_command_queue();
		}

		m_texture_cache.invalidate_range(*m_current_command_buffer, range, rsx::invalidation_cause::read);
		image_to_flip = m_texture_cache.upload_image_simple(*m_current_command_buffer, expected_format, info->address, info->width, info->height, info->pitch);
	}
	else if (image_to_flip->format() != expected_format)
	{
		// Devs are being creative. Force-cast this to the proper pixel layout.
		auto dst_img = m_texture_cache.create_temporary_subresource_storage(
			RSX_FORMAT_CLASS_COLOR, expected_format, info->width, info->height, 1, 1, 1,
			VK_IMAGE_TYPE_2D, 0, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

		if (dst_img)
		{
			const areai src_rect = { 0, 0, static_cast<int>(info->width), static_cast<int>(info->height) };
			const areai dst_rect = src_rect;

			dst_img->change_layout(*m_current_command_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			if (vk::formats_are_bitcast_compatible(dst_img.get(), image_to_flip))
			{
				vk::copy_image(*m_current_command_buffer, image_to_flip, dst_img.get(), src_rect, dst_rect);
			}
			else
			{
				vk::copy_image_typeless(*m_current_command_buffer, image_to_flip, dst_img.get(), src_rect, dst_rect);
			}

			image_to_flip = dst_img.get();
			m_texture_cache.dispose_reusable_image(dst_img);
		}
	}

	return image_to_flip;
}

void VKGSRender::flip(const rsx::display_flip_info_t& info)
{
	// Check swapchain condition/status
	if (!m_swapchain->supports_automatic_wm_reports())
	{
		if (m_swapchain_dims.width != m_frame->client_width() + 0u ||
			m_swapchain_dims.height != m_frame->client_height() + 0u)
		{
			swapchain_unavailable = true;
		}
	}

	if (m_vsync_mode != g_cfg.video.vsync)
	{
		swapchain_unavailable = true;
	}

	if (swapchain_unavailable || should_reinitialize_swapchain)
	{
		// Reinitializing the swapchain is a failable operation. However, not all failures are fatal (e.g minimized window).
		// In the worst case, we can have the driver refuse to create the swapchain while we already deleted the previous one.
		// In such scenarios, we have to retry a few times before giving up as we cannot proceed without a swapchain.
		for (int i = 0; i < 10; ++i)
		{
			if (reinitialize_swapchain() || m_current_frame)
			{
				// If m_current_frame exists, then the initialization failure is non-fatal. Proceed as usual.
				break;
			}

			if (Emu.IsStopped())
			{
				m_frame->flip(m_context);
				rsx::thread::flip(info);
				return;
			}

			std::this_thread::sleep_for(100ms);
		}
	}

	m_profiler.start();

	ensure(m_current_frame, "Invalid swapchain setup. Resizing the game window failed.");

	if (m_current_frame == &m_aux_frame_context)
	{
		m_current_frame = &m_frame_context_storage[m_current_queue_index];
		if (m_current_frame->swap_command_buffer)
		{
			// Its possible this flip request is triggered by overlays and the flip queue is in undefined state
			frame_context_cleanup(m_current_frame);
		}

		// Swap aux storage and current frame; aux storage should always be ready for use at all times
		m_current_frame->grab_resources(m_aux_frame_context);
	}
	else if (m_current_frame->swap_command_buffer)
	{
		if (info.stats.draw_calls > 0)
		{
			// This can be 'legal' if the window was being resized and no polling happened because of swapchain_unavailable flag
			rsx_log.error("Possible data corruption on frame context storage detected");
		}

		// There were no draws and back-to-back flips happened
		frame_context_cleanup(m_current_frame);
	}

	if (info.skip_frame || swapchain_unavailable)
	{
		if (!info.skip_frame)
		{
			ensure(swapchain_unavailable);

			// Perform a mini-flip here without invoking present code
			m_current_frame->swap_command_buffer = m_current_command_buffer;
			flush_command_queue(true);
			vk::advance_frame_counter();
			frame_context_cleanup(m_current_frame);
		}

		m_frame->flip(m_context);
		rsx::thread::flip(info);
		return;
	}

	u32 buffer_width = display_buffers[info.buffer].width;
	u32 buffer_height = display_buffers[info.buffer].height;
	u32 buffer_pitch = display_buffers[info.buffer].pitch;

	u32 av_format;
	const auto& avconfig = g_fxo->get<rsx::avconf>();

	if (!buffer_width)
	{
		buffer_width = avconfig.resolution_x;
		buffer_height = avconfig.resolution_y;
	}

	if (avconfig.state)
	{
		av_format = avconfig.get_compatible_gcm_format();
		if (!buffer_pitch)
			buffer_pitch = buffer_width * avconfig.get_bpp();

		const size2u video_frame_size = avconfig.video_frame_size();
		buffer_width = std::min(buffer_width, video_frame_size.width);
		buffer_height = std::min(buffer_height, video_frame_size.height);
	}
	else
	{
		av_format = CELL_GCM_TEXTURE_A8R8G8B8;
		if (!buffer_pitch)
			buffer_pitch = buffer_width * 4;
	}

	// Scan memory for required data. This is done early to optimize waiting for the driver image acquire below.
	vk::viewable_image* image_to_flip = nullptr;
	vk::viewable_image* image_to_flip2 = nullptr;
	bool tm4k_crop_inset = false;
	bool tm4k_full_canvas = false;

	if (info.buffer < display_buffers_count && buffer_width && buffer_height)
	{
		vk::present_surface_info present_info
		{
			.address = rsx::get_address(display_buffers[info.buffer].offset, CELL_GCM_LOCATION_LOCAL),
			.format = av_format,
			.width = buffer_width,
			.height = buffer_height,
			.pitch = buffer_pitch,
			.eye = 0
		};
		image_to_flip = get_present_source(&present_info, avconfig);

		// Twisted Metal 4K experiment: verify whether the two high-resolution
		// RenderDoc sources survive until flip. This does not alter rendering.
		if (Emu.GetTitleID() == "BCUS98106")
		{
			static u32 probe_flips = 0;
			if ((++probe_flips % 120) == 0)
			{
				const auto raw_scene = m_rtts.get_surface_at(0xc0ab0000);
				const auto postfx_scene = m_rtts.get_surface_at(0x40472a80);
				const auto color_scene = m_rtts.get_surface_at(0xc1190000);
				std::ofstream probe_file(fs::get_config_dir() + "tm4k-probe.txt", std::ios::app);
				probe_file << "flip=" << probe_flips
					<< " shared_tag=" << rsx::get_shared_tag()
					<< " raw=" << static_cast<u32>(raw_scene != nullptr)
					<< " raw_tag=" << (raw_scene ? raw_scene->last_use_tag : 0)
					<< " raw_gpu=" << (raw_scene ? raw_scene->width() : 0)
					<< 'x' << (raw_scene ? raw_scene->height() : 0)
					<< " raw_size=" << (raw_scene ? raw_scene->get_surface_width<rsx::surface_metrics::samples>() : 0)
					<< 'x' << (raw_scene ? raw_scene->get_surface_height<rsx::surface_metrics::samples>() : 0)
					<< " postfx=" << static_cast<u32>(postfx_scene != nullptr)
					<< " color=" << static_cast<u32>(color_scene != nullptr)
					<< " color_tag=" << (color_scene ? color_scene->last_use_tag : 0)
					<< " postfx_tag=" << (postfx_scene ? postfx_scene->last_use_tag : 0)
					<< " color_gpu=" << (color_scene ? color_scene->width() : 0)
					<< 'x' << (color_scene ? color_scene->height() : 0)
					<< " postfx_size=" << (postfx_scene ? postfx_scene->get_surface_width<rsx::surface_metrics::samples>() : 0)
					<< 'x' << (postfx_scene ? postfx_scene->get_surface_height<rsx::surface_metrics::samples>() : 0)
					<< " flip_size=" << buffer_width << 'x' << buffer_height
					<< " image_size=" << (image_to_flip ? image_to_flip->width() : 0)
					<< 'x' << (image_to_flip ? image_to_flip->height() : 0) << '\n';
				rsx_log.notice("TM4K probe: raw=%u postfx=%u flip=%ux%u image=%ux%u",
					static_cast<u32>(raw_scene != nullptr),
					static_cast<u32>(postfx_scene != nullptr),
					buffer_width, buffer_height,
					image_to_flip ? image_to_flip->width() : 0,
					image_to_flip ? image_to_flip->height() : 0);
			}
		}

		if (avconfig.stereo_enabled) [[unlikely]]
		{
			const auto [unused, min_expected_height] = rsx::apply_resolution_scale<true>(resolution_scaling_config, RSX_SURFACE_DIMENSION_IGNORED, buffer_height + 30);
			if (image_to_flip->height() < min_expected_height)
			{
				// Get image for second eye
				const u32 image_offset = (buffer_height + 30) * buffer_pitch + display_buffers[info.buffer].offset;
				present_info.width = buffer_width;
				present_info.height = buffer_height;
				present_info.address = rsx::get_address(image_offset, CELL_GCM_LOCATION_LOCAL);
				present_info.eye = 1;

				image_to_flip2 = get_present_source(&present_info, avconfig);
			}
			else
			{
				// Account for possible insets
				const auto [unused2, scaled_buffer_height] = rsx::apply_resolution_scale<true>(resolution_scaling_config, RSX_SURFACE_DIMENSION_IGNORED, buffer_height);
				buffer_height = std::min<u32>(image_to_flip->height() - min_expected_height, scaled_buffer_height);
			}
		}

		buffer_width = present_info.width;
		buffer_height = present_info.height;

		// Opt-in: only the separate development build has this code and the
		// enable flag. The known-good desktop installation is never modified.
		if (Emu.GetTitleID() == "BCUS98106" && image_to_flip)
		{
			static const bool enabled = std::ifstream(fs::get_config_dir() + "tm4k-enable.flag").good();
			static const bool aa_enabled = std::ifstream(fs::get_config_dir() + "tm4k-aa-enable.flag").good();
			static const bool lit01_enabled = std::ifstream(fs::get_config_dir() + "tm4k-lit01-enable.flag").good();
			static const bool lit_c_enabled = std::ifstream(fs::get_config_dir() + "tm4k-lit-c-enable.flag").good();
			static const bool packed_alpha_enabled = lit_c_enabled && std::ifstream(fs::get_config_dir() + "tm4k-packed-alpha-enable.flag").good();
			static const bool micro_e_enabled = std::ifstream(fs::get_config_dir() + "tm4k-micro-e-enable.flag").good();
			if (enabled)
			{
				tm4k_crop_inset = true;
				const auto hdr_scene = m_rtts.get_surface_at(0xc1be0000);
				const auto color_scene = m_rtts.get_surface_at(0xc1190000);
				const u64 now = rsx::get_shared_tag();
				auto* native_present_image = image_to_flip;
				const u64 hdr_tag_before = hdr_scene ? hdr_scene->last_use_tag : 0;
				const u64 color_tag_before = color_scene ? color_scene->last_use_tag : 0;
				const bool hdr_format_valid = hdr_scene && hdr_scene->format() == VK_FORMAT_R16G16B16A16_SFLOAT;
				const bool color_format_valid = color_scene &&
					(color_scene->format() == VK_FORMAT_B8G8R8A8_UNORM || color_scene->format() == VK_FORMAT_R8G8B8A8_UNORM);
				const char* source_reason = !hdr_scene || !color_scene ? "missing-source" :
					!hdr_format_valid ? "not-fp16-hdr" : !color_format_valid ? "not-unorm-color" :
					!hdr_tag_before || !color_tag_before ? "unwritten-source" :
					now < hdr_tag_before || now < color_tag_before ? "future-tag" :
					now - hdr_tag_before >= 600 ? "hdr-age" :
					now - color_tag_before >= 300 ? "color-age" : "eligible";
				u32 current_route=0;
				u64 selected_guide_tag=0;
				bool selected_from_history=false;
				// Diagnostic only: report why the optional fusion is unavailable in
				// some gameplay frames. Keep the known-good presentation unchanged.
				static u32 fusion_probe_flips = 0;
				if ((++fusion_probe_flips % 120) == 0)
				{
					const u64 hdr_age = hdr_scene && now >= hdr_scene->last_use_tag ? now - hdr_scene->last_use_tag : umax;
					const u64 color_age = color_scene && now >= color_scene->last_use_tag ? now - color_scene->last_use_tag : umax;
					std::ofstream probe_file(fs::get_config_dir() + "tm4k-fusion-probe.txt", std::ios::app);
					probe_file << "flip=" << fusion_probe_flips << " shared_tag=" << now
						<< " hdr=" << static_cast<u32>(hdr_scene != nullptr)
						<< " hdr_tag=" << (hdr_scene ? hdr_scene->last_use_tag : 0)
						<< " hdr_age=" << hdr_age
						<< " hdr_size=" << (hdr_scene ? hdr_scene->width() : 0) << 'x' << (hdr_scene ? hdr_scene->height() : 0)
						<< " color=" << static_cast<u32>(color_scene != nullptr)
						<< " color_tag=" << (color_scene ? color_scene->last_use_tag : 0)
						<< " color_age=" << color_age
						<< " color_size=" << (color_scene ? color_scene->width() : 0) << 'x' << (color_scene ? color_scene->height() : 0)
						<< '\n';
				}
				if (!m_tm4k_fusion) m_tm4k_fusion = std::make_unique<tm4k_fusion_pass>();
				auto* fusion = static_cast<tm4k_fusion_pass*>(m_tm4k_fusion.get());
				const auto display_surface = m_rtts.get_surface_at(present_info.address);
				const u64 display_tag = display_surface ? display_surface->last_use_tag : 0;
				bool selected_early_color = false;
				auto* packet = fusion->select_completed_scene(display_tag, selected_early_color);
				source_reason = packet ? "completed-scene-selected" : "no-completed-scene-for-display";
				// Never independently pick currently live attachments here. They
				// can already contain the next geometry/material pass while this
				// display buffer still shows the preceding completed SPU frame.
				static vk::viewable_image* s_last_fused_image = nullptr;
				static u32 s_last_fused_width = 0, s_last_fused_height = 0;
				static u32 s_fused_latch_frames = 0;
				if (packet)
				{
					// Process-local opt-in keeps every existing best-version launcher
					// on the unchanged timing policy. No config/save/flag mutation.
					static const bool geometry_lag_enabled = []()
					{
						const char* setting = std::getenv("TM4K_GEOMETRY_LAG1");
						return setting && std::string_view(setting) == "1";
					}();
					auto* geometry_packet = geometry_lag_enabled ? fusion->select_geometry_predecessor(*packet) : nullptr;
					if (!geometry_packet) geometry_packet = packet;
					auto* hdr_image = geometry_packet->hdr.get();
					auto* color_image = selected_early_color ? packet->early_color.get() : packet->color.get();
					const u64 selected_color_tag = selected_early_color ? packet->early_color_tag : packet->color_tag;
					source_reason = "fusion-contract-rejected";
					auto* guide_image = geometry_packet->guide.get();
					selected_guide_tag = geometry_packet->guide_tag;
					selected_from_history = true;
					static u64 paired_flips = 0;
					if (++paired_flips <= 8 || paired_flips % 120 == 0)
					{
						rsx_log.notice("TM4K generation selected: display=0x%x display_tag=%llu hdr=%llu color=%llu guide=%llu gap=%llu early=%u",
							present_info.address, display_tag, packet->hdr_tag, selected_color_tag, packet->guide_tag, display_tag - selected_color_tag, static_cast<u32>(selected_early_color));
						if (geometry_lag_enabled)
							rsx_log.notice("TM4K motion-generation-test: applied=%u color_hdr=%llu color=%llu geometry_hdr=%llu geometry_guide=%llu",
								static_cast<u32>(geometry_packet != packet), packet->hdr_tag, selected_color_tag,
								geometry_packet->hdr_tag, geometry_packet->guide_tag);
					}
					if (auto fused = static_cast<tm4k_fusion_pass*>(m_tm4k_fusion.get())->run(
						*m_current_command_buffer, hdr_image, color_image, image_to_flip, aa_enabled, guide_image, lit_c_enabled, packed_alpha_enabled, micro_e_enabled))
					{
						image_to_flip = fused;
						s_last_fused_image = fused;
						s_last_fused_width = fused->width();
						s_last_fused_height = fused->height();
						s_fused_latch_frames = 0;
						tm4k_full_canvas=fusion->full_canvas_output;
						current_route=static_cast<tm4k_fusion_pass*>(m_tm4k_fusion.get())->last_route;
						source_reason = "active";
						buffer_width = fused->width();
						buffer_height = fused->height();
						static bool logged = false;
						if (!logged)
						{
							rsx_log.notice("TM4K HDR fusion active: hdr=%ux%u final=%ux%u edge_aa=%u",
								hdr_image->width(), hdr_image->height(), fused->width(), fused->height(), static_cast<u32>(aa_enabled));
							rsx_log.notice("TM4K LIT variant C configured=%u guide_available=%u",
								static_cast<u32>(lit_c_enabled),static_cast<u32>(guide_image!=nullptr));
							rsx_log.notice("TM4K packed Alpha native-hue reconstruction configured=%u guide_available=%u fit_radius=3 working_guide=FP16 relative_freshness=300",
								static_cast<u32>(packed_alpha_enabled),static_cast<u32>(guide_image!=nullptr));
							rsx_log.notice("TM4K timing-generation-test: generations=3 early_late_color=1 selection_reference=display_write_tag black_transition_ui=1");
							logged = true;
						}
					}
				}
				else if (s_last_fused_image && s_fused_latch_frames < 30)
				{
					image_to_flip = s_last_fused_image;
					buffer_width = s_last_fused_width;
					buffer_height = s_last_fused_height;
					tm4k_full_canvas = fusion->full_canvas_output;
					current_route = static_cast<tm4k_fusion_pass*>(m_tm4k_fusion.get())->last_route;
					source_reason = "latched-4k-fusion";
					++s_fused_latch_frames;
				}
				// Diagnostics only: count actual outcomes, not just enabled flags.
				// Suppress unbounded transition logging while retaining totals.
				static u64 route_frames[4]={0,0,0,0};
				static u64 route_flips=0,route_changes=0;
				static u32 previous_route=umax;
				++route_frames[current_route];
				++route_flips;
				const bool route_changed=current_route!=previous_route;
				if (route_changed) ++route_changes;
				// Labels and metadata only. Never copy, reload or wait for an image
				// for diagnostics; preserve the presentation and its UI/sky source.
				static std::string previous_source_reason;
				const bool source_changed = previous_source_reason != source_reason;
				if (source_changed || (route_changed && route_changes <= 128) || route_flips % 120 == 0)
				{
					std::ofstream source_probe(fs::get_config_dir() + "tm4k-source-contract.txt", std::ios::app);
					const auto live_guide = m_rtts.get_surface_at(0xc0ab0000);
					auto describe_source = [&](const char* role, u32 address, vk::viewable_image* image, u64 before, u64 after)
					{
						const auto label = fmt::format("TM4K %s guest=0x%x vk_format=%u", role, address,
							image ? static_cast<u32>(image->format()) : 0);
						if (image && image->value && image->debug_name() != label &&
							(std::string_view(role) != "catalog" || !image->debug_name().starts_with("TM4K ")))
							image->set_debug_name(label);
						const auto detail = fmt::format("TM4K source: flip=%llu role=%s guest=0x%x image=0x%llx format=%u samples=%u size=%ux%u before=%llu after=%llu",
							route_flips, role, address, image ? reinterpret_cast<u64>(image->value) : 0,
							image ? static_cast<u32>(image->format()) : 0, image ? static_cast<u32>(image->samples()) : 0,
							image ? image->width() : 0, image ? image->height() : 0, before, after);
						rsx_log.notice("%s", detail);
						source_probe << detail << '\n';
					};
					describe_source("hdr-candidate", 0xc1be0000, hdr_scene, hdr_tag_before, hdr_scene ? hdr_scene->last_use_tag : 0);
					describe_source("color-candidate", 0xc1190000, color_scene, color_tag_before, color_scene ? color_scene->last_use_tag : 0);
					describe_source("guide-candidate", 0xc0ab0000, live_guide,
						live_guide ? live_guide->last_use_tag : 0, live_guide ? live_guide->last_use_tag : 0);
					// image_to_flip may already be the fusion result. Identify the
					// actual final image separately instead of calling it native UI.
					describe_source("native-present", present_info.address, native_present_image, 0, 0);
					describe_source("output", present_info.address, image_to_flip, 0, 0);
					rsx_log.notice("TM4K source outcome: flip=%llu route=%u reason=%s shared_tag=%llu display=0x%x",
						route_flips, current_route, source_reason, now, present_info.address);
					source_probe << fmt::format("TM4K source outcome: flip=%llu route=%u reason=%s shared_tag=%llu display=0x%x",
						route_flips, current_route, source_reason, now, present_info.address) << '\n';
					// Inventory all local high-res color attachments, not just three
					// assumed addresses. Alternative HDR locations become observable;
					// do not select an arbitrary FP16 attachment merely by its format.
					const auto surfaces = m_rtts.get_local_color_surface_catalog();
					for (auto* surface : surfaces)
					{
						if (surface->width() < 1280 || surface->height() < 704) continue;
						describe_source("catalog", surface->get_memory_range().start, surface,
							surface->last_use_tag, surface->last_use_tag);
					}
				}
				previous_source_reason = source_reason;
				if ((route_changed && route_changes<=32) || route_flips%120==0)
				{
					const auto diagnostic_guide=lit01_enabled ? m_rtts.get_surface_at(0xc0ab0000) : nullptr;
					rsx_log.notice("TM4K route diagnostic: flip=%llu route=%u changes=%llu none=%llu hdr=%llu lit=%llu packed=%llu hdr_age=%llu color_age=%llu guide_age=%llu",
						route_flips,current_route,route_changes,route_frames[0],route_frames[1],route_frames[2],route_frames[3],
						hdr_scene && now>=hdr_scene->last_use_tag ? now-hdr_scene->last_use_tag : umax,
						color_scene && now>=color_scene->last_use_tag ? now-color_scene->last_use_tag : umax,
						diagnostic_guide && now>=diagnostic_guide->last_use_tag ? now-diagnostic_guide->last_use_tag : umax);
					if (packed_alpha_enabled)
						rsx_log.notice("TM4K guide pairing: flip=%llu selected=%llu hdr=%llu live=%llu snapshot=%u future_live=%u gap=%llu",
							route_flips,selected_guide_tag,hdr_scene ? hdr_scene->last_use_tag : 0,
							diagnostic_guide ? diagnostic_guide->last_use_tag : 0,
							static_cast<u32>(selected_from_history),
							static_cast<u32>(hdr_scene && diagnostic_guide && diagnostic_guide->last_use_tag>hdr_scene->last_use_tag),
							selected_guide_tag && hdr_scene && hdr_scene->last_use_tag>=selected_guide_tag ? hdr_scene->last_use_tag-selected_guide_tag : umax);
				}
				previous_route=current_route;
			}
		}
	}

	if (info.emu_flip)
	{
		evaluate_cpu_usage_reduction_limits();
	}

	// Prepare surface for new frame. Set no timeout here so that we wait for the next image if need be
	ensure(m_current_frame->present_image == umax);
	ensure(m_current_frame->swap_command_buffer == nullptr);

	u64 timeout = m_swapchain->get_swap_image_count() <= 2? 0ull: 100000000ull;
	while (VkResult status = m_swapchain->acquire_next_swapchain_image(m_current_frame->acquire_signal_semaphore, timeout, &m_current_frame->present_image))
	{
		switch (status)
		{
		case VK_TIMEOUT:
		case VK_NOT_READY:
		{
			// In some cases, after a fullscreen switch, the driver only allows N-1 images to be acquirable, where N = number of available swap images.
			// This means that any acquired images have to be released
			// before acquireNextImage can return successfully. This is despite the driver reporting 2 swap chain images available
			// This makes fullscreen performance slower than windowed performance as throughput is lowered due to losing one presentable image
			// Found on AMD Crimson 17.7.2


			// Whatever returned from status, this is now a spin
			timeout = 0ull;
			check_present_status();
			continue;
		}
		case VK_SUBOPTIMAL_KHR:
			should_reinitialize_swapchain = true;
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			rsx_log.warning("vkAcquireNextImageKHR failed with VK_ERROR_OUT_OF_DATE_KHR. Flip request ignored until surface is recreated.");
			swapchain_unavailable = true;
			reinitialize_swapchain();
			ensure(m_current_frame, "Could not reinitialize swapchain after VK_ERROR_OUT_OF_DATE_KHR signal!");
			continue;
		default:
			vk::die_with_error(status);
		}

		if (should_reinitialize_swapchain)
		{
			// Image is valid, new swapchain will be generated later
			break;
		}
	}

	// Confirm that the driver did not silently fail
	ensure(m_current_frame->present_image != umax);

	// Calculate output dimensions. Done after swapchain acquisition in case it was recreated.
	areai aspect_ratio;
	if (!g_cfg.video.stretch_to_display_area)
	{
		const auto converted = avconfig.aspect_convert_region({ buffer_width, buffer_height }, m_swapchain_dims);
		aspect_ratio = static_cast<areai>(converted);
	}
	else
	{
		aspect_ratio = { 0, 0, s32(m_swapchain_dims.width), s32(m_swapchain_dims.height) };
	}

	// Blit contents to screen..
	VkImage target_image = m_swapchain->get_image(m_current_frame->present_image);
	const auto present_layout = m_swapchain->get_optimal_present_layout();

	const VkImageSubresourceRange subresource_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkImageLayout target_layout = present_layout;

	VkRenderPass single_target_pass = VK_NULL_HANDLE;
	vk::framebuffer_holder* direct_fbo = nullptr;
	rsx::simple_array<vk::viewable_image*> calibration_src;

	const bool has_overlay = (m_overlay_manager && m_overlay_manager->has_visible());
	const bool user_asked_for_screenshot = g_user_asked_for_screenshot.exchange(false);
	const bool user_is_recording = (g_recording_mode != recording_mode::stopped && m_frame->can_consume_frame());
	const bool need_media_capture = user_asked_for_screenshot || user_is_recording;

	const auto render_overlays = [&](vk::framebuffer_holder* fbo, const areau& area)
	{
		if (!has_overlay) return;

		// Lock to avoid modification during run-update chain
		auto ui_renderer = vk::get_overlay_pass<vk::ui_overlay_renderer>();
		std::lock_guard lock(*m_overlay_manager);

		const areau display_area = {0, 0, static_cast<u32>(m_swapchain_dims.width), static_cast<u32>(m_swapchain_dims.height)};
		for (const auto& view : m_overlay_manager->get_views())
		{
			const areau render_area = view->use_window_space ? display_area : area;
			ui_renderer->run(*m_current_command_buffer, render_area, fbo, single_target_pass, m_texture_upload_buffer_ring_info, *view.get());
		}
	};

	// WARNING: We have to do this here. We cannot touch the acquired image on the CB and then do a hard sync on it before it is submitted to the presentation engine.
	// That introduces a WRITE_AFTER_PRESENT (from the previous present) when we later try to present on a different CB
	if (image_to_flip && need_media_capture)
	{
		const usz sshot_size = buffer_height * buffer_width * 4;

		vk::buffer sshot_vkbuf(*m_device, utils::align(sshot_size, 0x100000), m_device->get_memory_mapping().host_visible_coherent,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0, VMM_ALLOCATION_POOL_UNDEFINED);

		VkBufferImageCopy copy_info{};
		copy_info.bufferOffset = 0;
		copy_info.bufferRowLength = 0;
		copy_info.bufferImageHeight = 0;
		copy_info.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_info.imageSubresource.baseArrayLayer = 0;
		copy_info.imageSubresource.layerCount = 1;
		copy_info.imageSubresource.mipLevel = 0;
		copy_info.imageOffset.x = 0;
		copy_info.imageOffset.y = 0;
		copy_info.imageOffset.z = 0;
		copy_info.imageExtent.width = buffer_width;
		copy_info.imageExtent.height = buffer_height;
		copy_info.imageExtent.depth = 1;

		vk::image* image_to_copy = image_to_flip;

		if (g_cfg.video.record_with_overlays && has_overlay)
		{
			const auto key = vk::get_renderpass_key(m_swapchain->get_surface_format());
			single_target_pass = vk::get_renderpass(*m_device, key);
			ensure(single_target_pass != VK_NULL_HANDLE);

			if (m_overlay_recording_img)
			{
				// Validate
				if (m_overlay_recording_img->format() != image_to_flip->format() ||
					m_overlay_recording_img->width() != image_to_flip->width() ||
					m_overlay_recording_img->height() != image_to_flip->height())
				{
					// Dispose correctly
					vk::remove_framebuffers_with_image(m_overlay_recording_img.get());
					vk::get_resource_manager()->dispose(m_overlay_recording_img);
				}
			}

			if (!m_overlay_recording_img)
			{
				m_overlay_recording_img = std::make_unique<vk::image>(*m_device, m_device->get_memory_mapping().device_local, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					VK_IMAGE_TYPE_2D, image_to_flip->format(), image_to_flip->width(), image_to_flip->height(), 1, 1, 1, VK_SAMPLE_COUNT_1_BIT,
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					0, VMM_ALLOCATION_POOL_SYSTEM);
			}

			m_overlay_recording_img->change_layout(*m_current_command_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			image_to_flip->push_layout(*m_current_command_buffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

			const areai rect = areai(0, 0, buffer_width, buffer_height);
			vk::copy_image(*m_current_command_buffer, image_to_flip, m_overlay_recording_img.get(), rect, rect);

			image_to_flip->pop_layout(*m_current_command_buffer);
			m_overlay_recording_img->change_layout(*m_current_command_buffer, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			vk::framebuffer_holder* sshot_fbo = vk::get_framebuffer(*m_device, buffer_width, buffer_height, VK_FALSE, single_target_pass, { m_overlay_recording_img.get() });
			sshot_fbo->add_ref();
			render_overlays(sshot_fbo, areau(rect));
			sshot_fbo->release();

			image_to_copy = m_overlay_recording_img.get();
		}

		image_to_copy->push_layout(*m_current_command_buffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		vk::copy_image_to_buffer(*m_current_command_buffer, image_to_copy, &sshot_vkbuf, copy_info);
		image_to_copy->pop_layout(*m_current_command_buffer);

		flush_command_queue(true);
		const auto src = sshot_vkbuf.map(0, sshot_size);
		std::vector<u8> sshot_frame(sshot_size);
		memcpy(sshot_frame.data(), src, sshot_size);
		sshot_vkbuf.unmap();

		const bool is_bgra = image_to_copy->format() == VK_FORMAT_B8G8R8A8_UNORM;

		if (user_asked_for_screenshot)
		{
			m_frame->take_screenshot(std::move(sshot_frame), buffer_width, buffer_height, is_bgra);
		}
		else
		{
			m_frame->present_frame(std::move(sshot_frame), buffer_width * 4, buffer_width, buffer_height, is_bgra);
		}
	}

	if (!image_to_flip || aspect_ratio.x1 || aspect_ratio.y1)
	{
		// Clear the window background to black
		VkClearColorValue clear_black {};
		vk::change_image_layout(*m_current_command_buffer, target_image, present_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresource_range);
		vkCmdClearColorImage(*m_current_command_buffer, target_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_black, 1, &subresource_range);

		// Prevent WAW on transfer writes
		vk::insert_image_memory_barrier(
			*m_current_command_buffer,
			target_image,
			target_layout,
			target_layout,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			subresource_range
		);

		target_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	}

	const output_scaling_mode output_scaling = g_cfg.video.output_scaling.get();

	if (!m_upscaler || m_output_scaling != output_scaling)
	{
		m_output_scaling = output_scaling;

		if (m_output_scaling == output_scaling_mode::nearest)
		{
			m_upscaler = std::make_unique<vk::nearest_upscale_pass>();
		}
		else if (m_output_scaling == output_scaling_mode::fsr)
		{
			m_upscaler = std::make_unique<vk::fsr_upscale_pass>();
		}
		else
		{
			m_upscaler = std::make_unique<vk::bilinear_upscale_pass>();
		}
	}

	if (image_to_flip)
	{
		const bool use_full_rgb_range_output = g_cfg.video.full_rgb_range_output.get();
		if (Emu.GetTitleID() == "BCUS98106")
		{
			static u32 tm4k_present_probe = 0;
			if ((++tm4k_present_probe % 120) == 0)
				rsx_log.notice("TM4K present: gamma=%f full_rgb=%u stereo=%u crop_enabled=%u source=%ux%u buffer=%ux%u display=%ux%u",
					static_cast<f32>(avconfig.gamma), static_cast<u32>(use_full_rgb_range_output), static_cast<u32>(avconfig.stereo_enabled),
					static_cast<u32>(tm4k_crop_inset), image_to_flip->width(), image_to_flip->height(),
					buffer_width, buffer_height, aspect_ratio.width(), aspect_ratio.height());
		}

		if (!use_full_rgb_range_output || !rsx::fcmp(avconfig.gamma, 1.f) || avconfig.stereo_enabled) [[unlikely]]
		{
			if (image_to_flip) calibration_src.push_back(image_to_flip);
			if (image_to_flip2) calibration_src.push_back(image_to_flip2);

			if (m_output_scaling == output_scaling_mode::fsr && !avconfig.stereo_enabled) // 3D will be implemented later
			{
				// Run upscaling pass before the rest of the output effects pipeline
				// This can be done with all upscalers but we already get bilinear upscaling for free if we just out the filters directly
				VkImageBlit request = {};
				request.srcSubresource = { image_to_flip->aspect(), 0, 0, 1 };
				request.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
				request.srcOffsets[0] = { 0, 0, 0 };
				request.srcOffsets[1] = { s32(buffer_width), s32(buffer_height), 1 };
				request.dstOffsets[0] = { 0, 0, 0 };
				request.dstOffsets[1] = { aspect_ratio.width(), aspect_ratio.height(), 1 };

				for (unsigned i = 0; i < calibration_src.size(); ++i)
				{
					const rsx::flags32_t mode = (i == 0) ? UPSCALE_LEFT_VIEW : UPSCALE_RIGHT_VIEW;
					calibration_src[i] = m_upscaler->scale_output(*m_current_command_buffer, image_to_flip, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED, request, mode);
				}
			}

			vk::change_image_layout(*m_current_command_buffer, target_image, target_layout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, subresource_range);
			target_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			const auto key = vk::get_renderpass_key(m_swapchain->get_surface_format());
			single_target_pass = vk::get_renderpass(*m_device, key);
			ensure(single_target_pass != VK_NULL_HANDLE);

			direct_fbo = vk::get_framebuffer(*m_device, m_swapchain_dims.width, m_swapchain_dims.height, VK_FALSE, single_target_pass, m_swapchain->get_surface_format(), target_image);
			direct_fbo->add_ref();

			vk::get_overlay_pass<vk::video_out_calibration_pass>()->run(
				*m_current_command_buffer, areau(aspect_ratio), direct_fbo, calibration_src,
				avconfig.gamma, !use_full_rgb_range_output, avconfig.stereo_enabled, single_target_pass);

			direct_fbo->release();
		}
		else
		{
			// Do raw transfer here as there is no image object associated with textures owned by the driver (TODO)
			VkImageBlit rgn = {};
			rgn.srcSubresource = { image_to_flip->aspect(), 0, 0, 1 };
			rgn.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			rgn.srcOffsets[0] = { 0, 0, 0 };
			rgn.srcOffsets[1] = { s32(buffer_width), s32(buffer_height), 1 };
			rgn.dstOffsets[0] = { aspect_ratio.x1, aspect_ratio.y1, 0 };
			rgn.dstOffsets[1] = { aspect_ratio.x2, aspect_ratio.y2, 1 };

			// Twisted Metal presents a 1202x672 picture inside a 1280x720
			// black safety border. The 4K fusion deliberately preserves that
			// layout, so crop only at the final display blit. This also keeps
			// HUD and scene aligned, without changing either source image.
			const u32 tm4k_scale = buffer_width / 1280;
			const bool crop_tm4k = tm4k_crop_inset && !tm4k_full_canvas &&
				(tm4k_scale == 3 || tm4k_scale == 4) &&
				buffer_width == 1280 * tm4k_scale &&
				buffer_height == 720 * tm4k_scale &&
				image_to_flip->width() == buffer_width &&
				image_to_flip->height() == buffer_height;
			if (crop_tm4k)
			{
				rgn.srcOffsets[0] = { 39 * s32(tm4k_scale), 24 * s32(tm4k_scale), 0 };
				rgn.srcOffsets[1] = { 1241 * s32(tm4k_scale), 696 * s32(tm4k_scale), 1 };
			}

			if (target_layout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				vk::change_image_layout(*m_current_command_buffer, target_image, target_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresource_range);
				target_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			}

			// FSR 1's upscale path assumes a source rectangle starting at (0,0).
			// A direct linear blit honors the inset coordinates; at the original
			// 4K-to-4K output size FSR was not upscaling this image anyway.
			if (crop_tm4k || tm4k_full_canvas)
			{
				image_to_flip->push_layout(*m_current_command_buffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
				vkCmdBlitImage(*m_current_command_buffer, image_to_flip->value, image_to_flip->current_layout,
					target_image, target_layout, 1, &rgn, VK_FILTER_LINEAR);
				image_to_flip->pop_layout(*m_current_command_buffer);
			}
			else
			{
				m_upscaler->scale_output(*m_current_command_buffer, image_to_flip, target_image, target_layout, rgn, UPSCALE_AND_COMMIT | UPSCALE_DEFAULT_VIEW);
			}
		}
	}

	if (g_cfg.video.debug_overlay || has_overlay)
	{
		if (target_layout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			// Change the image layout whilst setting up a dependency on waiting for the blit op to finish before we start writing
			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.oldLayout = target_layout;
			barrier.image = target_image;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange = subresource_range;
			vkCmdPipelineBarrier(*m_current_command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &barrier);

			target_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		if (!direct_fbo)
		{
			const auto key = vk::get_renderpass_key(m_swapchain->get_surface_format());
			single_target_pass = vk::get_renderpass(*m_device, key);
			ensure(single_target_pass != VK_NULL_HANDLE);

			direct_fbo = vk::get_framebuffer(*m_device, m_swapchain_dims.width, m_swapchain_dims.height, VK_FALSE, single_target_pass, m_swapchain->get_surface_format(), target_image);
		}

		direct_fbo->add_ref();

		render_overlays(direct_fbo, areau(aspect_ratio));

		if (g_cfg.video.debug_overlay)
		{
			const auto num_dirty_textures = m_texture_cache.get_unreleased_textures_count();
			const auto texture_memory_size = m_texture_cache.get_texture_memory_in_use() / (1024 * 1024);
			const auto tmp_texture_memory_size = m_texture_cache.get_temporary_memory_in_use() / (1024 * 1024);
			const auto num_flushes = m_texture_cache.get_num_flush_requests();
			const auto num_mispredict = m_texture_cache.get_num_cache_mispredictions();
			const auto num_speculate = m_texture_cache.get_num_cache_speculative_writes();
			const auto num_misses = m_texture_cache.get_num_cache_misses();
			const auto num_unavoidable = m_texture_cache.get_num_unavoidable_hard_faults();
			const auto cache_miss_ratio = static_cast<u32>(ceil(m_texture_cache.get_cache_miss_ratio() * 100));
			const auto num_texture_upload = m_texture_cache.get_texture_upload_calls_this_frame();
			const auto num_texture_upload_miss = m_texture_cache.get_texture_upload_misses_this_frame();
			const auto texture_upload_miss_ratio = m_texture_cache.get_texture_upload_miss_percentage();
			const auto texture_copies_ellided = m_texture_cache.get_texture_copies_ellided_this_frame();
			const auto vertex_cache_hit_count = (info.stats.vertex_cache_request_count - info.stats.vertex_cache_miss_count);
			const auto vertex_cache_hit_ratio = info.stats.vertex_cache_request_count
				? (vertex_cache_hit_count * 100) / info.stats.vertex_cache_request_count
				: 0;
			const auto program_cache_lookups = info.stats.program_cache_lookups_total;
			const auto program_cache_ellided = info.stats.program_cache_lookups_ellided;
			const auto program_cache_ellision_rate = program_cache_lookups
				? (program_cache_ellided * 100) / program_cache_lookups
				: 0;

			rsx::overlays::set_debug_overlay_text(fmt::format(
				"Internal Resolution:      %s\n"
				"RSX Load:                 %3d%%\n"
				"draw calls: %17d\n"
				"submits: %20d\n"
				"draw call setup: %12dus\n"
				"vertex upload time: %9dus\n"
				"texture upload time: %8dus\n"
				"draw call execution: %8dus\n"
				"submit and flip: %12dus\n"
				"Unreleased textures: %8d\n"
				"Texture cache memory: %7dM\n"
				"Temporary texture memory: %3dM\n"
				"Flush requests: %13d  = %2d (%3d%%) hard faults, %2d unavoidable, %2d misprediction(s), %2d speculation(s)\n"
				"Texture uploads: %12u (%u from CPU - %02u%%, %u copies avoided)\n"
				"Vertex cache hits: %10u/%u (%u%%)\n"
				"Program cache lookup ellision: %u/%u (%u%%)",
				info.stats.framebuffer_stats.to_string(resolution_scaling_config, !backend_config.supports_hw_msaa),
				get_load(), info.stats.draw_calls, info.stats.submit_count, info.stats.setup_time, info.stats.vertex_upload_time,
				info.stats.textures_upload_time, info.stats.draw_exec_time, info.stats.flip_time,
				num_dirty_textures, texture_memory_size, tmp_texture_memory_size,
				num_flushes, num_misses, cache_miss_ratio, num_unavoidable, num_mispredict, num_speculate,
				num_texture_upload, num_texture_upload_miss, texture_upload_miss_ratio, texture_copies_ellided,
				vertex_cache_hit_count, info.stats.vertex_cache_request_count, vertex_cache_hit_ratio,
				program_cache_ellided, program_cache_lookups, program_cache_ellision_rate)
			);
		}

		direct_fbo->release();
	}

	if (target_layout != present_layout)
	{
		vk::change_image_layout(*m_current_command_buffer, target_image, target_layout, present_layout, subresource_range);
	}

	queue_swap_request();

	m_frame_stats.flip_time = m_profiler.duration();

	m_frame->flip(m_context);
	rsx::thread::flip(info);

	// Data sync
	const rsx::surface_scaling_config_t active_res_scaling_config =
	{
		.scale_percent = static_cast<u16>(g_cfg.video.resolution_scale_percent),
		.min_scalable_dimension = static_cast<u16>(g_cfg.video.min_scalable_dimension),
	};

	if (active_res_scaling_config != this->resolution_scaling_config)
	{
		// First, try to reclaim any memory since the res scale upgrade is so memory intensive
		if (const auto severity = vk::vmm_determine_memory_load_severity();
			severity > rsx::problem_severity::low && m_rtts.handle_memory_pressure(*m_current_command_buffer, severity))
		{
			flush_command_queue(true);
		}

		// Then apply the change
		m_rtts.sync_scaling_config(*m_current_command_buffer, active_res_scaling_config);
		this->resolution_scaling_config = active_res_scaling_config;

		// Finally reclaim any unused resources
		if (const auto severity = vk::vmm_determine_memory_load_severity();
			severity > rsx::problem_severity::low && m_rtts.handle_memory_pressure(*m_current_command_buffer, severity))
		{
			flush_command_queue(true);
		}
	}
}
