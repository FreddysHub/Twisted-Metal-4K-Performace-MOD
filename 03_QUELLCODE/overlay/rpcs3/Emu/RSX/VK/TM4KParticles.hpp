#pragma once

// Restoration of the native half-resolution particle and effect pass (fire, smoke, nitro, sparks).
// Keeps the game draw/depth lookup unchanged; extracts only its actual blend and composites cleanly onto 4K.
namespace tm4k_particles
{
constexpr const char* extract = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // before
layout(set=0,binding=1) uniform sampler2D Input1; // after
layout(set=0,binding=2) uniform sampler2D Input2; // previous layer
layout(set=0,binding=3,rgba16f) uniform writeonly image2D OutputTexture;
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy);
    if(any(greaterThanEqual(p,imageSize(OutputTexture)))) return;
    vec4 b=texelFetch(Input0,p,0),a=texelFetch(Input1,p,0),old=texelFetch(Input2,p,0);
    if(any(isnan(a)) || any(isinf(a)) || any(isnan(b)) || any(isinf(b))) {
        imageStore(OutputTexture,p,old);
        return;
    }
    // Check alpha delta for alpha-blended particles
    float d_alpha = max(a.a - b.a, 0.0);
    float alpha = clamp(d_alpha / max(1.0 - b.a, 1e-4), 0.0, 1.0);
    vec3 premul = vec3(0.0);
    if (alpha > 1e-4 && b.a < 0.999) {
        // Standard alpha-blended particle (e.g. smoke, Axel fire)
        premul = max(a.rgb - b.rgb * (1.0 - alpha), vec3(0.0));
    } else {
        // Pure additive particle (e.g. Nitro exhaust flame, sparks, muzzle flashes)
        vec3 added = max(a.rgb - b.rgb, vec3(0.0));
        premul = added;
        alpha = 0.0;
    }
    // Porter-Duff Over accumulation in premultiplied space
    vec3 acc_rgb = premul + old.rgb * (1.0 - alpha);
    float acc_a = alpha + old.a * (1.0 - alpha);
    imageStore(OutputTexture, p, vec4(acc_rgb, acc_a));
}
)glsl";
constexpr const char* compose = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // base (4K reconstructed scene)
layout(set=0,binding=1) uniform sampler2D Input1; // particles (1920x1080 accumulated particle layer)
layout(set=0,binding=2) uniform sampler2D Input2; // sentinel
layout(set=0,binding=3) uniform sampler2D Input3; // native color
layout(set=0,binding=4) uniform sampler2D Input4; // native display
layout(set=0,binding=5,rgba8) uniform writeonly image2D OutputTexture;
vec3 linear(vec3 x) {
    return mix(pow((x+0.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(0.04045)));
}
vec3 srgb(vec3 x) {
    return mix(1.055*pow(max(x,vec3(0.0)),vec3(1.0/2.4))-0.055,12.92*x,lessThanEqual(x,vec3(0.0031308)));
}
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    vec4 base=texelFetch(Input0,p,0);
    if(texelFetch(Input2,ivec2(0),0).a<0.0) {imageStore(OutputTexture,p,base);return;}
    vec2 uv=(vec2(p)+0.5)/vec2(sz);
    vec4 fire=texture(Input1,uv);
    if(fire.a<1e-4 && max(max(fire.r,fire.g),fire.b)<1e-4) {imageStore(OutputTexture,p,base);return;}
    // Protect late UI using true display alpha (never paste over HUD/minimap)
    vec2 display_uv=(vec2(117,72)+uv*vec2(3606,2016))/vec2(sz);
    float ui_alpha=texture(Input4,display_uv).a;
    if(ui_alpha>0.05) {
        imageStore(OutputTexture,p,base);return;
    }
    vec3 base_lin = linear(base.rgb);
    float alpha = clamp(fire.a, 0.0, 1.0);
    vec3 flame_lin = max(fire.rgb, vec3(0.0));
    // Soft tonemap high HDR bursts to prevent harsh clamping
    flame_lin = flame_lin / (1.0 + flame_lin);
    vec3 blended_lin = base_lin * (1.0 - alpha) + flame_lin;
    vec3 color = srgb(blended_lin);
    imageStore(OutputTexture,p,vec4(clamp(color,0.0,1.0),base.a));
}
)glsl";
}
