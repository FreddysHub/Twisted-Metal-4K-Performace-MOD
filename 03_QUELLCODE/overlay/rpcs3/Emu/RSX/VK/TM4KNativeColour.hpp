#pragma once
// Mode 6: two-pass native RGB matching; mode 7 adds the selected bounded third pass.
// All fields are frame-local. No sharpening, image history or UV displacement.
namespace tm4k_native_colour
{
constexpr const char* base = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1) uniform sampler2D Input1;
layout(binding=2) uniform sampler2D Input2;
layout(binding=3) uniform sampler2D Input3;
layout(binding=4) uniform sampler2D Input4;
layout(binding=5,rgba32f) uniform writeonly image2D OutputTexture;
vec3 lin(vec3 x){return mix(pow((x+.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(.04045)));}
vec3 srg(vec3 x){x=clamp(x,0,1);return mix(1.055*pow(x,vec3(1/2.4))-.055,x*12.92,lessThanEqual(x,vec3(.0031308)));}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);if(any(greaterThanEqual(p,sz)))return;
vec3 v=texelFetch(Input0,p,0).rgb,g=texelFetch(Input1,p,0).rgb;
if(texelFetch(Input4,ivec2(0),0).g>.5 && max(max(g.r,g.g),g.b)>0){
vec3 hi=vec3(0);for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++)hi+=lin(texelFetch(Input1,clamp(p+ivec2(x,y),ivec2(0),sz-1),0).rgb);hi=srg(hi/9);
ivec2 ip=(p+2)/3-1,low=textureSize(Input2,0);vec2 pos=vec2(ip)+vec2((p+2)%3)/3.0;vec3 res=vec3(0);float n=0;
for(int y=-1;y<=2;y++)for(int x=-1;x<=2;x++){ivec2 tap=ip+ivec2(x,y),q=clamp(tap,ivec2(0),low-1);vec4 l=texelFetch(Input2,q,0);vec3 dg=hi-srg(l.rgb);vec2 ds=pos-vec2(tap);
float w=max(exp(-dot(ds,ds)/(2*.85*.85))*exp(-dot(dg,dg)/(2*.10*.10))*pow(max(l.a,.02),2),1e-20);res+=w*texelFetch(Input3,q,0).rgb;n+=w;}
v=max(v+res/n,0);}
imageStore(OutputTexture,p,vec4(v,1));}
)glsl";
constexpr const char* mean = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1,rgba32f) uniform writeonly image2D OutputTexture;
vec3 lin(vec3 x){return mix(pow((x+.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(.04045)));}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy);if(any(greaterThanEqual(p,imageSize(OutputTexture))))return;
vec3 v=vec3(0);for(int y=0;y<3;y++)for(int x=0;x<3;x++){vec3 a=texelFetch(Input0,p*3+ivec2(x,y),0).rgb;
#ifdef NATIVE_SRGB
a=lin(a);
#endif
v+=a;}imageStore(OutputTexture,p,vec4(v/9,1));}
)glsl";
constexpr const char* gain = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1) uniform sampler2D Input1;
layout(binding=2,rgba32f) uniform writeonly image2D OutputTexture;
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy);if(any(greaterThanEqual(p,imageSize(OutputTexture))))return;
#ifdef CONSISTENCY_PASS
const float lim=1.5;
#elif defined(SECOND_PASS)
const float lim=2.0;
#else
const float lim=3.0;
#endif
vec3 ratio=(texelFetch(Input0,p,0).rgb+vec3(.0005))/(texelFetch(Input1,p,0).rgb+vec3(.0005));
imageStore(OutputTexture,p,vec4(log(clamp(ratio,vec3(1/lim),vec3(lim))),1));}
)glsl";
constexpr const char* apply = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0; // value to correct
layout(binding=1) uniform sampler2D Input1; // fixed base guidance, both passes
layout(binding=2) uniform sampler2D Input2; // fixed 3x3 base mean
layout(binding=3) uniform sampler2D Input3; // low-grid log gain
layout(binding=4,rgba32f) uniform writeonly image2D OutputTexture;
vec3 srg(vec3 x){x=clamp(x,0,1);return mix(1.055*pow(x,vec3(1/2.4))-.055,x*12.92,lessThanEqual(x,vec3(.0031308)));}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);if(any(greaterThanEqual(p,sz)))return;
vec3 hi=vec3(0);for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++)hi+=texelFetch(Input1,clamp(p+ivec2(x,y),ivec2(0),sz-1),0).rgb;hi=srg(hi/9);
ivec2 ip=(p+2)/3-1,low=textureSize(Input2,0);vec2 pos=vec2(ip)+vec2((p+2)%3)/3.0;vec3 a=vec3(0);float n=0;
for(int y=-1;y<=2;y++)for(int x=-1;x<=2;x++){ivec2 tap=ip+ivec2(x,y),q=clamp(tap,ivec2(0),low-1);vec3 dg=hi-srg(texelFetch(Input2,q,0).rgb);vec2 ds=pos-vec2(tap);
float w=max(exp(-dot(ds,ds)/(2*.85*.85))*exp(-dot(dg,dg)/(2*.12*.12)),1e-20);a+=w*texelFetch(Input3,q,0).rgb;n+=w;}
imageStore(OutputTexture,p,vec4(max(texelFetch(Input0,p,0).rgb*exp(a/n),0),1));}
)glsl";
constexpr const char* merge = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0; // corrected linear scene
layout(binding=1) uniform sampler2D Input1; // native colour
layout(binding=2) uniform sampler2D Input2; // native display / true UI alpha
layout(binding=3) uniform sampler2D Input3; // route/logo
layout(binding=4) uniform sampler2D Input4; // micro_guide (decoded 4K scene)
layout(binding=5,rgba8) uniform writeonly image2D OutputTexture;
vec3 srg(vec3 x){x=clamp(x,0,1);return mix(1.055*pow(x,vec3(1/2.4))-.055,x*12.92,lessThanEqual(x,vec3(.0031308)));}
float cubic(float x){x=abs(x);if(x<1)return (1.5*x-2.5)*x*x+1;if(x<2)return ((-.5*x+2.5)*x-4)*x+2;return 0;}
vec4 display(vec2 pos){ivec2 b=ivec2(floor(pos)),sz=textureSize(Input2,0);vec4 v=vec4(0);float n=0;for(int y=-1;y<=2;y++)for(int x=-1;x<=2;x++){ivec2 q=b+ivec2(x,y);float w=cubic(pos.x-q.x)*cubic(pos.y-q.y);v+=w*texelFetch(Input2,clamp(q,ivec2(0),sz-1),0);n+=w;}return v/n;}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);if(any(greaterThanEqual(p,sz)))return;vec2 uv=(vec2(p)+.5)/vec2(sz);
vec4 d=display(vec2(118,73.5)+uv*vec2(3606,2017)-.5);if(texelFetch(Input3,ivec2(0),0).r>2.5){imageStore(OutputTexture,p,vec4(clamp(d.rgb,0,1),d.a));return;}
float a=clamp(d.a,0,1);if(a<16.0/255.0)a=0.0;vec3 c=texelFetch(Input1,p,0).rgb,U=clamp(d.rgb-(1-a)*c,vec3(0),vec3(a));
vec3 sc=max(srg(texelFetch(Input0,p,0).rgb),d.rgb*0.75);
vec3 g=texelFetch(Input4,p,0).rgb;
float lg=dot(g,vec3(0.299,0.587,0.114));
float blur=0.0;
for(int y=-1;y<=1;y++){for(int x=-1;x<=1;x++){
    float w=(x==0&&y==0)?4.0:((x==0||y==0)?2.0:1.0);
    blur+=dot(texelFetch(Input4,clamp(p+ivec2(x,y),ivec2(0),sz-1),0).rgb,vec3(0.299,0.587,0.114))*w;
}}
blur*=0.0625;
float detail=lg-blur;
float geo_mask=clamp((lg-0.005)/0.02,0.0,1.0);
float d_clamp=clamp(detail,-0.08,0.08)*0.45*geo_mask;
sc=clamp(sc+vec3(d_clamp),0.0,1.0);
imageStore(OutputTexture,p,vec4(U+(1-a)*sc,a));}
)glsl";
}
