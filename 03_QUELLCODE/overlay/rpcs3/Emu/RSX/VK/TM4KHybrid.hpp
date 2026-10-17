#pragma once
// Mode5: real native 720-grid; approved palette weights for other scenes,
// Gemini gain-prior fit + consistency for sepia gameplay. No image history.
namespace tm4k_hybrid
{
constexpr const char* down_guide = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1,rgba32f) uniform writeonly image2D OutputTexture;
vec3 lin(vec3 x){return mix(pow((x+.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(.04045)));}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy);if(any(greaterThanEqual(p,imageSize(OutputTexture))))return;vec3 g=vec3(0);float n=0;
for(int y=0;y<3;y++)for(int x=0;x<3;x++){vec3 a=texelFetch(Input0,p*3+ivec2(x,y),0).rgb;float v=max(max(a.r,a.g),a.b)>0?1:0;g+=lin(a)*v;n+=v;}
imageStore(OutputTexture,p,vec4(g/max(n,1),n/9));}
)glsl";
constexpr const char* down_colour = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1,rgba32f) uniform writeonly image2D OutputTexture;
vec3 lin(vec3 x){return mix(pow((x+.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(.04045)));}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy);if(any(greaterThanEqual(p,imageSize(OutputTexture))))return;imageStore(OutputTexture,p,vec4(lin(texelFetch(Input0,p*3+1,0).rgb),1));}
)glsl";
constexpr const char* classify = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1) uniform sampler2D Input1;
layout(binding=2,rgba32f) uniform writeonly image2D OutputTexture;
void main(){if(any(notEqual(ivec2(gl_GlobalInvocationID.xy),ivec2(0))))return;
float black=0,edges=0,total=0,coverage=0;vec3 colour=vec3(0);
for(int y=0;y<36;y++)for(int x=0;x<64;x++){vec2 uv=(vec2(x,y)+.5)/vec2(64,36);vec4 d=texture(Input0,(vec2(118,73.5)+uv*vec2(3606,2017))/vec2(3840,2160));
bool edge=uv.x<.20||uv.x>.80||uv.y<.12||uv.y>.90;float b=max(max(d.r,d.g),d.b)<.5/255?1:0;if(edge){black+=b;edges++;}total+=b;coverage+=d.a;colour+=texture(Input1,uv).rgb;}
bool logo=black/edges>.97&&total/2304>.5;
// Broad colour family, not per-pixel branch, no latch. Alpha excludes menu panels.
bool sepia=colour.r>1.15*colour.g && colour.r>1.5*colour.b && coverage/2304<.065;
imageStore(OutputTexture,ivec2(0),vec4(logo?3:0,sepia?1:0,coverage/2304,0));}
)glsl";
constexpr const char* fit = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1) uniform sampler2D Input1;
layout(binding=2) uniform sampler2D Input2;
layout(binding=3,rgba32f) uniform writeonly image2D OutputTexture;
vec3 srg(vec3 x){x=clamp(x,0,1);return mix(1.055*pow(x,vec3(1/2.4))-.055,x*12.92,lessThanEqual(x,vec3(.0031308)));}
vec3 colour(ivec2 p){return texelFetch(Input1,clamp(p,ivec2(0),textureSize(Input1,0)-1),0).rgb;}
vec3 palette(ivec2 p){vec3 s=vec3(0);for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++)s+=colour(p+ivec2(x,y));return s/9;}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy),low=textureSize(Input0,0);if(any(greaterThanEqual(p,low)))return;
bool gem=texelFetch(Input2,ivec2(0),0).g>.5;vec3 center=texelFetch(Input0,p,0).rgb,pc=palette(p),sg=vec3(0),sc=vec3(0),sgg=vec3(0),sgc=vec3(0);float sw=0;int r=gem?1:3;
for(int y=-r;y<=r;y++)for(int x=-r;x<=r;x++){ivec2 q=clamp(p+ivec2(x,y),ivec2(0),low-1);vec4 gg=texelFetch(Input0,q,0);vec3 g=gg.rgb,c=colour(q);float w=gem?(gg.a>.999?1:0):exp(-dot(center-g,center-g)/(2*.06*.06)-dot(pc-palette(q),pc-palette(q))/(2*.075*.075)-float(x*x+y*y)/(2*1.5*1.5));
if(gem){g=srg(g);c=srg(c);}sw+=w;sg+=w*g;sc+=w*c;sgg+=w*g*g;sgc+=w*g*c;}
vec3 mg=sg/max(sw,1e-9),mc=sc/max(sw,1e-9),v=max(sgg/max(sw,1e-9)-mg*mg,0),cov=sgc/max(sw,1e-9)-mg*mc;
vec3 e=gem?vec3(.003)+.01*mg*mg:vec3(.001),prior=gem?clamp(mc/max(mg,vec3(.001)),.2,5):vec3(1);
vec3 a=clamp((cov+e*prior)/(v+e),vec3(.15),vec3(1.0));vec3 b=mc-a*mg;
imageStore(OutputTexture,p,vec4(a,1));imageStore(OutputTexture,p+ivec2(low.x,0),vec4(b,1));}
)glsl";
constexpr const char* smooth = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1) uniform sampler2D Input1;
layout(binding=2) uniform sampler2D Input2;
layout(binding=3) uniform sampler2D Input3;
layout(binding=4,rgba32f) uniform writeonly image2D OutputTexture;
vec3 colour(ivec2 p){return texelFetch(Input2,clamp(p,ivec2(0),textureSize(Input2,0)-1),0).rgb;}
vec3 palette(ivec2 p){vec3 s=vec3(0);for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++)s+=colour(p+ivec2(x,y));return s/9;}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);if(any(greaterThanEqual(p,sz)))return;ivec2 low=textureSize(Input1,0),offset=ivec2(p.x>=low.x?low.x:0,0),base=p-offset;
bool gem=texelFetch(Input3,ivec2(0),0).g>.5;vec3 center=texelFetch(Input1,base,0).rgb,pc=palette(base),s=vec3(0);float n=0;int r=gem?1:2;
for(int y=-r;y<=r;y++)for(int x=-r;x<=r;x++){ivec2 q=clamp(base+ivec2(x,y),ivec2(0),low-1);vec3 dg=center-texelFetch(Input1,q,0).rgb,dc=pc-palette(q);float w=gem?1:exp(-dot(dg,dg)/(2*.06*.06)-dot(dc,dc)/(2*.075*.075)-float(x*x+y*y)/4);s+=w*texelFetch(Input0,q+offset,0).rgb;n+=w;}
imageStore(OutputTexture,p,vec4(s/max(n,1e-20),1));}
)glsl";
constexpr const char* predict = R"glsl(
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
vec3 palette(ivec2 p){vec3 s=vec3(0);for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++)s+=texelFetch(Input2,clamp(p+ivec2(x,y),ivec2(0),textureSize(Input2,0)-1),0).rgb;return s/9;}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);if(any(greaterThanEqual(p,sz)))return;
// Integer 3:1 decomposition: float uv*low can fall below an exact integer
// and shift the entire 4x4 tap window. That caused coloured neon speckles.
ivec2 low=textureSize(Input1,0),ip=(p+2)/3-1;vec2 pos=vec2(ip)+vec2((p+2)%3)/3.0;bool gem=texelFetch(Input4,ivec2(0),0).g>.5;
vec3 g=texelFetch(Input0,p,0).rgb,gl=lin(g),hi=gl;
if(gem){hi=vec3(0);for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++)hi+=lin(texelFetch(Input0,clamp(p+ivec2(x,y),ivec2(0),sz-1),0).rgb);hi=srg(hi/9);}
vec3 target=vec3(0);vec2 f=fract(pos);for(int y=0;y<2;y++)for(int x=0;x<2;x++)target+=palette(clamp(ip+ivec2(x,y),ivec2(0),low-1))*(x==0?1-f.x:f.x)*(y==0?1-f.y:f.y);
vec3 a=vec3(0),b=vec3(0),sky=vec3(0);float n=0,ns=0;
for(int y=-1;y<=2;y++)for(int x=-1;x<=2;x++){ivec2 tap=ip+ivec2(x,y),q=clamp(tap,ivec2(0),low-1);vec4 l=texelFetch(Input1,q,0);vec3 dg=hi-(gem?srg(l.rgb):l.rgb),dc=target-palette(q);vec2 ds=pos-vec2(tap);
float space=exp(-dot(ds,ds)/(gem?(2*.85*.85):1.5));float w=gem?space*exp(-dot(dg,dg)/(2*.05*.05))*pow(max(l.a,.02),2):space*exp(-dot(dg,dg)/(2*.06*.06)-dot(dc,dc)/(2*.075*.075));w=max(w,1e-20);
a+=w*texelFetch(Input3,q,0).rgb;b+=w*texelFetch(Input3,q+ivec2(low.x,0),0).rgb;n+=w;
float ws=space*(pow(max(1-l.a,0),2)+.0001);sky+=ws*texelFetch(Input2,q,0).rgb;ns+=ws;}
vec3 pred=gem?(a/n*g+b/n):(a/n*gl+b/n);
vec3 p_out=max(mix(pred,target,0.35),target*0.75);
vec3 outp=gem?lin(clamp(p_out,0,1)):p_out;
if(max(max(g.r,g.g),g.b)==0)outp=sky/max(ns,1e-20);
imageStore(OutputTexture,p,vec4(outp,1));}
)glsl";
constexpr const char* residual = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0;
layout(binding=1) uniform sampler2D Input1;
layout(binding=2) uniform sampler2D Input2;
layout(binding=3,rgba32f) uniform writeonly image2D OutputTexture;
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy);if(any(greaterThanEqual(p,imageSize(OutputTexture))))return;vec3 v=vec3(0);for(int y=0;y<3;y++)for(int x=0;x<3;x++)v+=texelFetch(Input0,p*3+ivec2(x,y),0).rgb;
imageStore(OutputTexture,p,vec4(texelFetch(Input2,p,0).a>.999?texelFetch(Input1,p,0).rgb-v/9:vec3(0),1));}
)glsl";
constexpr const char* merge = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(binding=0) uniform sampler2D Input0; // raw linear reconstructed scene
layout(binding=1) uniform sampler2D Input1; // guide RGB8
layout(binding=2) uniform sampler2D Input2; // guide linear low + validity
layout(binding=3) uniform sampler2D Input3; // consistency residual
layout(binding=4) uniform sampler2D Input4; // native colour
layout(binding=5) uniform sampler2D Input5; // native display incl true alpha
layout(binding=6) uniform sampler2D Input6; // frame-local route/logo
layout(binding=7,rgba8) uniform writeonly image2D OutputTexture;
vec3 lin(vec3 x){return mix(pow((x+.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(.04045)));}
vec3 srg(vec3 x){x=clamp(x,0,1);return mix(1.055*pow(x,vec3(1/2.4))-.055,x*12.92,lessThanEqual(x,vec3(.0031308)));}
float cubic(float x){x=abs(x);if(x<1)return (1.5*x-2.5)*x*x+1;if(x<2)return ((-.5*x+2.5)*x-4)*x+2;return 0;}
vec4 display(vec2 pos){ivec2 b=ivec2(floor(pos)),sz=textureSize(Input5,0);vec4 v=vec4(0);float n=0;for(int y=-1;y<=2;y++)for(int x=-1;x<=2;x++){ivec2 q=b+ivec2(x,y);float w=cubic(pos.x-q.x)*cubic(pos.y-q.y);v+=w*texelFetch(Input5,clamp(q,ivec2(0),sz-1),0);n+=w;}return v/n;}
void main(){ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);if(any(greaterThanEqual(p,sz)))return;vec2 uv=(vec2(p)+.5)/vec2(sz);
vec4 cls=texelFetch(Input6,ivec2(0),0),d=display(vec2(118,73.5)+uv*vec2(3606,2017)-.5);if(cls.r>2.5){imageStore(OutputTexture,p,vec4(clamp(d.rgb,0,1),d.a));return;}
vec3 outp=texelFetch(Input0,p,0).rgb,g=texelFetch(Input1,p,0).rgb;
if(cls.g>.5 && max(max(g.r,g.g),g.b)>0){vec3 hi=vec3(0);for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++)hi+=lin(texelFetch(Input1,clamp(p+ivec2(x,y),ivec2(0),sz-1),0).rgb);hi=srg(hi/9);
ivec2 ip=(p+2)/3-1,low=textureSize(Input2,0);vec2 pos=vec2(ip)+vec2((p+2)%3)/3.0;vec3 res=vec3(0);float n=0;
for(int y=-1;y<=2;y++)for(int x=-1;x<=2;x++){ivec2 tap=ip+ivec2(x,y),q=clamp(tap,ivec2(0),low-1);vec4 l=texelFetch(Input2,q,0);vec3 dg=hi-srg(l.rgb);vec2 ds=pos-vec2(tap);float w=max(exp(-dot(ds,ds)/(2*.85*.85)-dot(dg,dg)/(2*.05*.05))*pow(max(l.a,.02),2),1e-20);res+=w*texelFetch(Input3,q,0).rgb;n+=w;}outp=max(outp+res/n,0);}
vec3 fs=srg(outp);float alpha=clamp(d.a,0,1);if(alpha<1.5/255)alpha=0;vec3 c=texelFetch(Input4,p,0).rgb,U=clamp(d.rgb-(1-alpha)*c,vec3(0),vec3(alpha));
imageStore(OutputTexture,p,vec4(U+(1-alpha)*fs,alpha));}
)glsl";
}
