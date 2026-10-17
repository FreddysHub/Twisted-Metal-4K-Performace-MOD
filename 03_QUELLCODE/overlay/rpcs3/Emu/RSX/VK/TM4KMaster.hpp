#pragma once

// Process-local experiment: use the actual decoded high-resolution lighting
// attachment, as in fire-analysis/build_fire_master.py. Not a texture pack,
// sharpening filter, or a recovered native particle pass.
namespace tm4k_master
{
constexpr const char* merge = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // existing validated full-canvas base
layout(set=0,binding=1) uniform sampler2D Input1; // decoded high-resolution scene
layout(set=0,binding=2) uniform sampler2D Input2; // existing geometry/UI validity
layout(set=0,binding=3) uniform sampler2D Input3; // opaque startup sentinel
layout(set=0,binding=4,rgba8) uniform writeonly image2D OutputTexture;
void main()
{
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),size=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,size))) return;
    vec4 base=texelFetch(Input0,p,0);
    vec4 validity=texelFetch(Input2,p,0);
    // Do not expose a hidden menu behind opaque logos; do not reconstruct sky
    // from a lighting attachment which does not contain sky. Preserve the
    // existing HUD/late-UI protections, even at the expense of scene coverage.
    if(validity.a<1.0 || texelFetch(Input3,ivec2(0),0).a<0.0)
    { imageStore(OutputTexture,p,base); return; }
    vec3 scene=texelFetch(Input1,p,0).rgb;
    // No low-resolution palette fit, luma cap, CAS, or temporal averaging.
    // This intentionally tests the reference's actual RGB details/grading.
    imageStore(OutputTexture,p,vec4(scene,base.a));
}
)glsl";

// Full scene experiment: direct decoded geometry, with independent native UI.
// Colour/display inputs are UI/effect evidence only, not a low-resolution
// replacement for the high-resolution material RGB. Missing atmosphere keeps
// the validated base. This is not a per-object pixel count or asset replacement.
constexpr const char* protected_merge = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // validated full-canvas base
layout(set=0,binding=1) uniform sampler2D Input1; // decoded high-res scene
layout(set=0,binding=2) uniform sampler2D Input2; // geometry/UI validity
layout(set=0,binding=3) uniform sampler2D Input3; // opaque startup sentinel
layout(set=0,binding=4) uniform sampler2D Input4; // native scene colour
layout(set=0,binding=5) uniform sampler2D Input5; // native display, including UI
layout(set=0,binding=6,rgba8) uniform writeonly image2D OutputTexture;
void main()
{
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),size=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,size))) return;
    vec4 base=texelFetch(Input0,p,0);
    vec4 validity=texelFetch(Input2,p,0);
    vec3 scene=texelFetch(Input1,p,0).rgb;
    if(validity.a<0.0 || texelFetch(Input3,ivec2(0),0).a<0.0 ||
        max(max(scene.r,scene.g),scene.b)<=0.001)
    { imageStore(OutputTexture,p,base);return; }
    vec2 uv=(vec2(p)+0.5)/vec2(size);
    vec2 display_uv=(vec2(117,72)+uv*vec2(3606,2016))/vec2(size);
    vec3 shown=texture(Input5,display_uv).rgb;
    // Native colour is repeated from a lower-resolution transfer. Compare
    // neighbouring native texels to avoid misclassifying its sampling phase
    // at a world edge as an overlay. The scene itself is NEVER UV-shifted.
    vec2 native_texel=1.0/vec2(textureSize(Input4,0));
    float difference=1.0;
    for(int y=-1;y<=1;++y) for(int x=-1;x<=1;++x)
    {
        vec3 delta=abs(shown-texture(Input4,uv+vec2(x,y)*native_texel*3.0).rgb);
        difference=min(difference,max(max(delta.r,delta.g),delta.b));
    }
    float late_ui=smoothstep(0.04,0.12,difference);
    imageStore(OutputTexture,p,vec4(mix(scene,base.rgb,late_ui),base.a));
}
)glsl";

// Mode 3: calibrate only low-frequency colour/shading, retaining the actual
// high-resolution guide at every scene sample. No replacement RGB texture,
// sharpening, historical frame blend, or geometry coordinate offset.
constexpr const char* colour_fit = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // box-filtered decoded guide
layout(set=0,binding=1) uniform sampler2D Input1; // authoritative native tone
layout(set=0,binding=2,rgba16f) uniform writeonly image2D OutputTexture;
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    vec3 center=texelFetch(Input0,p,0).rgb,sg=vec3(0),sc=vec3(0);
    float sw=0.0;
    for(int y=-4;y<=4;y+=2) for(int x=-4;x<=4;x+=2) {
        ivec2 q=clamp(p+ivec2(x,y),ivec2(0),sz-1);
        vec3 g=texelFetch(Input0,q,0).rgb,c=texelFetch(Input1,q,0).rgb;
        vec3 delta=g-center;
        float w=exp(-dot(delta,delta)/0.02-float(x*x+y*y)/32.0);
        if(max(max(g.r,g.g),g.b)<=0.001) w=0.0;
        sg+=g*w;sc+=c*w;sw+=w;
    }
    vec3 gain=sw>1e-6?clamp((sc+vec3(0.0001))/(sg+vec3(0.0001)),vec3(0),vec3(4)):vec3(1);
    imageStore(OutputTexture,p,vec4(gain,sw>1e-6?1.0:0.0));
}
)glsl";

constexpr const char* colour_merge = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // legacy base, not scene source
layout(set=0,binding=1) uniform sampler2D Input1; // actual decoded 4K scene
layout(set=0,binding=2) uniform sampler2D Input2; // legacy validity, not UI boxes
layout(set=0,binding=3) uniform sampler2D Input3; // opaque startup sentinel
layout(set=0,binding=4) uniform sampler2D Input4; // native colour
layout(set=0,binding=5) uniform sampler2D Input5; // native display incl. late UI
layout(set=0,binding=6) uniform sampler2D Input6; // smooth RGB calibration field
layout(set=0,binding=7,rgba8) uniform writeonly image2D OutputTexture;
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    vec2 uv=(vec2(p)+0.5)/vec2(sz);
    float scale=float(sz.x)/1280.0;
    vec2 display_uv=(vec2(39,24)*scale+uv*vec2(1202,672)*scale)/vec2(sz);
    vec4 shown=texture(Input5,display_uv);
    vec3 scene=texelFetch(Input1,p,0).rgb;
    // Use actual display for logos/missing atmosphere, not the already-fused
    // base. This also protects black pixels within partially covering logos.
    if(texelFetch(Input3,ivec2(0),0).a<0.0 ||
        max(max(scene.r,scene.g),scene.b)<=0.001 ||
        max(max(shown.r,shown.g),shown.b)<(0.5/255.0)) {
        imageStore(OutputTexture,p,shown);return;
    }
    vec3 mapped=clamp(scene*texture(Input6,uv).rgb,vec3(0),vec3(1));
    // NativeColor is a lower-resolution transfer while NativeDisplay has a
    // different sampling phase. A centre-only difference falsely classifies
    // world/material edges as UI and replaces real 4K detail with that transfer.
    // Match neighbouring native samples ONLY for overlay classification; never
    // move, resample or blur the high-resolution scene or its calibrated colour.
    vec2 native_texel=1.0/vec2(textureSize(Input4,0));
    float difference=1.0;
    for(int y=-1;y<=1;++y) for(int x=-1;x<=1;++x) {
        vec3 delta=abs(shown.rgb-texture(Input4,uv+vec2(x,y)*native_texel*3.0).rgb);
        difference=min(difference,max(max(delta.r,delta.g),delta.b));
    }
    // Keep real late UI from NativeDisplay, not the pre-fused base. Opaque
    // startup/black-display and missing-atmosphere guards above are unchanged.
    float ui=smoothstep(0.04,0.12,difference);
    imageStore(OutputTexture,p,vec4(mix(mapped,shown.rgb,ui),shown.a));
}
)glsl";
}
