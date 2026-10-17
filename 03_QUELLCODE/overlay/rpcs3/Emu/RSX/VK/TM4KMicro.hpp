#pragma once

// Opt-in live implementation of remove_glow.py variant E, in final-display
// coordinates. No encoded-RGB detail or broad/mesoscale lighting transfer.
namespace tm4k_micro
{
constexpr const char* crop = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1,rgba8) uniform writeonly image2D OutputTexture;
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    float scale=float(sz.x)/1280.0;
    vec2 uv=(vec2(39,24)*scale+(vec2(p)+0.5)/vec2(sz)*vec2(1202,672)*scale)/vec2(sz);
    imageStore(OutputTexture,p,texture(Input0,uv));
}
)glsl";
constexpr const char* decode = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1,rgba8) uniform writeonly image2D OutputTexture;
vec3 sample_decoded(ivec2 p) {
    vec4 raw=texelFetch(Input0,p,0);
    vec3 x=raw.rgb*(raw.a*255.0)*0.5;
    x=x/(1.0+x);
    vec3 s=mix(1.055*pow(x,vec3(1.0/2.4))-0.055,12.92*x,lessThanEqual(x,vec3(0.0031308)));
    // Offline PIL input was truncated to RGB8 before filtering, not RGBA.
#ifdef TM4K_MASTER_REFERENCE
    return round(clamp(s,0.0,1.0)*255.0)/255.0;
#else
    return floor(clamp(s,0.0,1.0)*255.0)/255.0;
#endif
}
float lanczos(float x) {
    x=abs(x);
    if(x<1e-6) return 1.0;
    if(x>=3.0) return 0.0;
    float t=3.141592653589793*x;
    return sin(t)/t*sin(t/3.0)/(t/3.0);
}
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture),src=textureSize(Input0,0);
    if(any(greaterThanEqual(p,sz))) return;
    // Equal horizontal dimensions. Vertical 704 -> 720 resize, Pillow pixel centers.
    float sy=(float(p.y)+0.5)*float(src.y)/float(sz.y)-0.5;
    int start=int(floor(sy))-2;
    vec3 total=vec3(0); float sum=0.0;
    for(int i=0;i<6;++i) {
        int y=start+i;
        if(y<0 || y>=src.y) continue;
        float w=lanczos(float(y)-sy);
        total+=sample_decoded(ivec2(p.x,y))*w; sum+=w;
    }
    imageStore(OutputTexture,p,vec4(floor(clamp(total/sum,0.0,1.0)*255.0+0.5)/255.0,1));
}
)glsl";
constexpr const char* logs = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // cropped authoritative base
layout(set=0,binding=1) uniform sampler2D Input1; // decoded/resized guide
layout(set=0,binding=2,rgba32f) uniform writeonly image2D OutputTexture;
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    vec3 d=texelFetch(Input0,p,0).rgb,g=texelFetch(Input1,p,0).rgb;
    vec3 weights=vec3(0.2126,0.7152,0.0722);
    float yd=max(dot(d,weights),1e-4),yg=max(dot(g,weights),1e-4);
    // The decoded lighting attachment omits sky/additive atmosphere. Its
    // dark residual is not a texture edge in the fogged presentation. Frame
    // 4060 proves that transferring it creates bright contour lines.
    bool source_valid=yg>0.015;
    float confidence=smoothstep(0.12,0.65,yg/yd);
    if(yg<=0.015) yg=yd;
    vec2 v=(vec2(p)+0.5)*vec2(3840,2160)/vec2(sz);
    bool ui=v.y>=1740.0 || (v.x<560.0 && v.y<580.0) ||
        (v.x>=40.0 && v.x<480.0 && v.y>=40.0 && v.y<480.0);
    vec3 delta=abs(d-g);
    ui=ui || max(max(delta.r,delta.g),delta.b)>0.65;
    // W: -1=UI, 0=missing geometry, 1..2=valid geometry plus confidence.
    imageStore(OutputTexture,p,vec4(log2(yd),log2(yg),yg,ui?-1.0:(source_valid?1.0+confidence:0.0)));
}
)glsl";
constexpr const char* fit = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // logI/logP/guide luma/UI
layout(set=0,binding=1,rgba32f) uniform writeonly image2D OutputTexture;
shared vec3 tile[20][20];
void main() {
    ivec2 sz=imageSize(OutputTexture),local=ivec2(gl_LocalInvocationID.xy);
    ivec2 origin=ivec2(gl_WorkGroupID.xy)*16-2;
    for(uint i=gl_LocalInvocationIndex;i<400;i+=256) {
        ivec2 q=ivec2(int(i)%20,int(i)/20);
        vec4 v=texelFetch(Input0,clamp(origin+q,ivec2(0),sz-1),0);
        tile[q.y][q.x]=vec3(v.rg,step(1.0,v.a));
    }
    barrier();
    ivec2 p=ivec2(gl_GlobalInvocationID.xy);
    if(any(greaterThanEqual(p,sz))) return;
    vec4 sums=vec4(0);
    float valid=1.0;
    for(int y=0;y<5;++y) for(int x=0;x<5;++x) {
        vec3 v=tile[local.y+y][local.x+x]; sums+=vec4(v.rg,v.x*v.x,v.x*v.y);
        valid=min(valid,v.z);
    }
    sums/=25.0;
    float variance=max(sums.z-sums.x*sums.x,0.0);
    float a=(sums.w-sums.x*sums.y)/(variance+0.015);
    imageStore(OutputTexture,p,vec4(a,sums.y-a*sums.x,valid,0));
}
)glsl";
constexpr const char* merge = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // base
layout(set=0,binding=1) uniform sampler2D Input1; // logs
layout(set=0,binding=2) uniform sampler2D Input2; // guided coefficients
layout(set=0,binding=3) uniform sampler2D Input3; // opaque-logo sentinel
layout(set=0,binding=4,rgba8) uniform writeonly image2D OutputTexture;
shared vec3 tile[20][20];
float guide_luma(ivec2 p,ivec2 sz) { return texelFetch(Input1,clamp(p,ivec2(0),sz-1),0).b; }
void main() {
    ivec2 sz=imageSize(OutputTexture),local=ivec2(gl_LocalInvocationID.xy);
    ivec2 origin=ivec2(gl_WorkGroupID.xy)*16-2;
    for(uint i=gl_LocalInvocationIndex;i<400;i+=256) {
        ivec2 q=ivec2(int(i)%20,int(i)/20);
        tile[q.y][q.x]=texelFetch(Input2,clamp(origin+q,ivec2(0),sz-1),0).rgb;
    }
    barrier();
    ivec2 p=ivec2(gl_GlobalInvocationID.xy);
    if(any(greaterThanEqual(p,sz))) return;
    vec4 base=texelFetch(Input0,p,0),v=texelFetch(Input1,p,0);
    // Preserve the startup/menu sentinel; decoded hidden world is not a logo.
    if(v.a<0.0 || texelFetch(Input3,ivec2(0),0).a<0.0) {
        imageStore(OutputTexture,p,base); return;
    }
    vec2 mean=vec2(0);
    float valid=1.0;
    for(int y=0;y<5;++y) for(int x=0;x<5;++x) {
        vec3 c=tile[local.y+y][local.x+x];
        mean+=c.rg; valid=min(valid,c.z);
    }
    mean/=25.0;
    float bf=mean.x*v.r+mean.y;
    float dx=(guide_luma(p+ivec2(1,0),sz)-guide_luma(p-ivec2(1,0),sz))*
        ((p.x==0 || p.x==sz.x-1)?1.0:0.5);
    float dy=(guide_luma(p+ivec2(0,1),sz)-guide_luma(p-ivec2(0,1),sz))*
        ((p.y==0 || p.y==sz.y-1)?1.0:0.5);
    float planar=clamp(1.0-length(vec2(dx,dy))*10.0,0.0,1.0);
    // Two radius-2 filters depend on a radius-4 neighborhood. If any guide
    // pixel there is missing, do not turn that discontinuity into detail.
    float confidence=clamp(v.a-1.0,0.0,1.0)*valid;
    if(confidence<=0.0) { imageStore(OutputTexture,p,base); return; }
    float micro=(v.g-bf)*(1.0-0.4*planar)*confidence;
    float y=exp2(v.r+clamp(micro*1.35,-1.8,1.8));
    float yc=y<=0.85?y:0.85+0.15*tanh((y-0.85)/0.25);
    float peak=max(max(base.r,base.g),base.b);
    float ratio=min(yc/exp2(v.r),1.0/max(peak,1e-4));
    vec3 out_rgb=floor(clamp(base.rgb*ratio,0.0,1.0)*255.0)/255.0;
    imageStore(OutputTexture,p,vec4(out_rgb,base.a));
}
)glsl";
}
