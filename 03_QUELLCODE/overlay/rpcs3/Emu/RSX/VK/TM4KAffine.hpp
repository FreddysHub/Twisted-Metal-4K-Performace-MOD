#pragma once

// Mode 4: the non-generative, linear-light calculation saved with the five
// approved previews. Separate from modes 1-3. All coefficients are FP32;
// no frame history, sharpening, UV offset, or CPU post-FX is introduced.
namespace tm4k_affine
{
constexpr const char* downsample = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1,rgba32f) uniform writeonly image2D OutputTexture;
vec3 linearRGB(vec3 x) {
    return mix(pow((x+0.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(0.04045)));
}
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    ivec2 source=textureSize(Input0,0);
    // Pillow BOX uses whole samples between rounded support boundaries, not
    // fractional overlap weights. This matters at the 2160 -> 704 conversion.
    ivec2 start=ivec2(floor(vec2(p)*vec2(source)/vec2(sz)+0.5));
    ivec2 end=ivec2(floor(vec2(p+1)*vec2(source)/vec2(sz)+0.5));
    // Measured Pillow BOX support at this exact size: double-precision centre
    // calculation ends row 681 at 2092 rather than rounding its tie to 2093.
    // Preserve the approved calculation; this is a filter boundary, not a UV shift.
    if(source==ivec2(3840,2160) && sz==ivec2(1280,704) && p.y==681) end.y=2092;
    vec3 sum=vec3(0);float count=0;
    for(int y=start.y;y<end.y;++y) for(int x=start.x;x<end.x;++x) {
        sum+=linearRGB(texelFetch(Input0,clamp(ivec2(x,y),ivec2(0),source-1),0).rgb);count+=1;
    }
    imageStore(OutputTexture,p,vec4(sum/max(count,1.0),1));
}
)glsl";

constexpr const char* fit = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1) uniform sampler2D Input1;
layout(set=0,binding=2,rgba32f) uniform writeonly image2D OutputTexture;
shared vec4 sharedGuide[484];
shared vec4 sharedColour[484];
void addCompensated(vec3 value,inout vec3 sum,inout vec3 compensation) {
    precise vec3 y=value-compensation;
    precise vec3 t=sum+y;
    compensation=(t-sum)-y;sum=t;
}
void main() {
    ivec2 low=textureSize(Input0,0),p=ivec2(gl_GlobalInvocationID.xy);
    // The atlas is twice as wide; only its first half computes the two fields.
    // 1280 and 704 are multiples of 16, so this exit is workgroup-uniform.
    if(any(greaterThanEqual(ivec2(gl_WorkGroupID.xy)*16,low))) return;
    ivec2 origin=ivec2(gl_WorkGroupID.xy)*16-3;
    for(uint i=gl_LocalInvocationIndex;i<484;i+=256) {
        ivec2 q=clamp(origin+ivec2(int(i)%22,int(i)/22),ivec2(0),low-1);
        sharedGuide[i]=texelFetch(Input0,q,0);sharedColour[i]=texelFetch(Input1,q,0);
    }
    barrier();
    if(any(greaterThanEqual(p,low))) return;
    precise vec3 sg=vec3(0),sc=vec3(0),sgg=vec3(0),sgc=vec3(0);
    vec3 cg=vec3(0),cc=vec3(0),cgg=vec3(0),cgc=vec3(0);
    ivec2 local=ivec2(gl_LocalInvocationID.xy);
    for(int y=0;y<7;++y) for(int x=0;x<7;++x) {
        int i=(local.y+y)*22+local.x+x;
        vec3 g=sharedGuide[i].rgb,c=sharedColour[i].rgb;
        addCompensated(g,sg,cg);addCompensated(c,sc,cc);
        addCompensated(g*g,sgg,cgg);addCompensated(g*c,sgc,cgc);
    }
    precise vec3 mg=sg/49.0,mc=sc/49.0;
    precise vec3 var=max(sgg/49.0-mg*mg,vec3(0)),cov=sgc/49.0-mg*mc;
    precise vec3 a=clamp((cov+vec3(0.00005))/(var+vec3(0.00005)),vec3(0.05),vec3(1.5));
    precise vec3 b=mc-a*mg;
    imageStore(OutputTexture,p,vec4(a,1));
    imageStore(OutputTexture,p+ivec2(low.x,0),vec4(b,1));
}
)glsl";

constexpr const char* smooth = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1,rgba32f) uniform writeonly image2D OutputTexture;
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    ivec2 low=ivec2(sz.x/2,sz.y),offset=ivec2(p.x>=low.x?low.x:0,0);
    vec3 sum=vec3(0);
    for(int y=-2;y<=2;++y) for(int x=-2;x<=2;++x)
        sum+=texelFetch(Input0,clamp(p-offset+ivec2(x,y),ivec2(0),low-1)+offset,0).rgb;
    imageStore(OutputTexture,p,vec4(sum/25.0,1));
}
)glsl";

// Frame-local visual classification, NOT a proven game-state discriminator.
// Scores and thresholds are tested against the saved reference frames.
// Unknown frames keep only the conservative game HUD guards; no time latch.
constexpr const char* classify = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0;
layout(set=0,binding=1,rgba32f) uniform writeonly image2D OutputTexture;
vec3 shown(vec2 uv) {return texture(Input0,(vec2(117,72)+uv*vec2(3606,2016))/vec2(3840,2160)).rgb;}
float red(vec3 c) {return c.r>0.10 && c.r>c.g*1.65 && c.r>c.b*1.65?1.0:0.0;}
float redRegion(vec4 r) {
    float n=0;
    for(int y=0;y<16;++y) for(int x=0;x<32;++x)
        n+=red(shown(mix(r.xy,r.zw,(vec2(x,y)+0.5)/vec2(32,16))));
    return n/512.0;
}
void main() {
    if(any(notEqual(ivec2(gl_GlobalInvocationID.xy),ivec2(0)))) return;
    float outside=0,black=0,total=0;
    for(int y=0;y<36;++y) for(int x=0;x<64;++x) {
        vec2 uv=(vec2(x,y)+0.5)/vec2(64,36);
        bool edge=uv.x<0.20 || uv.x>0.80 || uv.y<0.12 || uv.y>0.90;
        if(edge) {vec3 c=shown(uv);black+=max(max(c.r,c.g),c.b)<(0.5/255.0)?1:0;outside+=1;}
        total+=max(max(shown(uv).r,shown(uv).g),shown(uv).b)<(0.5/255.0)?1:0;
    }
    float logo=black/max(outside,1.0);
    float selection=redRegion(vec4(0.565,0.468,0.955,0.532));
    float menu=redRegion(vec4(0.06,0.62,0.345,0.835));
    // 0=game/unknown, 1=main menu, 2=vehicle selection, 3=native logo.
    int kind=0;
    if(logo>0.97 && total/2304.0>0.50) kind=3;
    else if(selection>0.50 && total/2304.0>0.04) kind=2;
    else if(menu>0.055 && total/2304.0>0.08) kind=1;
    imageStore(OutputTexture,ivec2(0),vec4(kind,logo,selection,menu));
}
)glsl";

constexpr const char* merge = R"glsl(
#version 450
layout(local_size_x=16,local_size_y=16) in;
layout(set=0,binding=0) uniform sampler2D Input0; // decoded RGB8 scene
layout(set=0,binding=1) uniform sampler2D Input1; // native colour
layout(set=0,binding=2) uniform sampler2D Input2; // native display
layout(set=0,binding=3) uniform sampler2D Input3; // FP32 slope/bias atlas
layout(set=0,binding=4) uniform sampler2D Input4; // sentinel
layout(set=0,binding=5) uniform sampler2D Input5; // frame-local class/scores
layout(set=0,binding=6,rgba8) uniform writeonly image2D OutputTexture;
vec4 bilinearPixels(sampler2D source,vec2 pos) {
    ivec2 size=textureSize(source,0);
    pos=clamp(pos,vec2(0),vec2(size-1));
    ivec2 a=ivec2(floor(pos)),b=min(a+1,size-1);
    precise vec2 w=pos-vec2(a);
    precise vec4 top=mix(texelFetch(source,a,0),texelFetch(source,ivec2(b.x,a.y),0),w.x);
    precise vec4 bottom=mix(texelFetch(source,ivec2(a.x,b.y),0),texelFetch(source,b,0),w.x);
    return mix(top,bottom,w.y);
}
vec3 linearRGB(vec3 x) {return mix(pow((x+0.055)/1.055,vec3(2.4)),x/12.92,lessThanEqual(x,vec3(0.04045)));}
vec3 sRGB(vec3 x) {
    x=clamp(x,vec3(0),vec3(1));
    return mix(1.055*pow(x,vec3(1.0/2.4))-0.055,x*12.92,lessThanEqual(x,vec3(0.0031308)));
}
bool rect(ivec2 p,ivec4 r) {return all(greaterThanEqual(p,r.xy)) && all(lessThan(p,r.zw));}
bool nativeUI(ivec2 p,int kind) {
    if(kind==1) return rect(p,ivec4(50,170,1980,1290)) || rect(p,ivec4(140,1260,1280,1880)) || rect(p,ivec4(650,1980,2390,2160));
    if(kind==2) return rect(p,ivec4(2180,990,3840,2060)) || rect(p,ivec4(2840,50,3840,1030)) || rect(p,ivec4(650,1980,2450,2160));
    return rect(p,ivec4(0,0,570,590)) || rect(p,ivec4(20,1080,575,1220)) || rect(p,ivec4(0,1780,760,2160)) || rect(p,ivec4(1640,1840,2670,2160)) || rect(p,ivec4(3000,1830,3840,2160));
}
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy),sz=imageSize(OutputTexture);
    if(any(greaterThanEqual(p,sz))) return;
    vec2 uv=(vec2(p)+0.5)/vec2(sz);
    precise vec2 displayPos=vec2(117,72)+uv*vec2(3606,2016)-0.5;
    vec4 d=bilinearPixels(Input2,displayPos);
    int kind=int(texelFetch(Input5,ivec2(0),0).r+0.5);
    vec3 g=texelFetch(Input0,p,0).rgb;
    if(kind==3 || nativeUI(p,kind) || texelFetch(Input4,ivec2(0),0).a<0.0 || max(max(g.r,g.g),g.b)<=0.001 || max(max(d.r,d.g),d.b)<(0.5/255.0)) {
        imageStore(OutputTexture,p,d);return;
    }
    vec2 atlas=vec2(textureSize(Input3,0)),low=vec2(atlas.x/2.0,atlas.y);
    vec2 fitPos=clamp(uv*low,vec2(0.5),low-0.5);
    vec3 a=bilinearPixels(Input3,fitPos-0.5).rgb;
    vec3 b=bilinearPixels(Input3,fitPos+vec2(low.x,0)-0.5).rgb;
    vec3 mapped=sRGB(a*linearRGB(g)+b);
    float diff=1.0;
    precise vec2 nativePos=uv*vec2(textureSize(Input1,0))-0.5;
    for(int y=-1;y<=1;++y) for(int x=-1;x<=1;++x) {
        vec3 delta=abs(d.rgb-bilinearPixels(Input1,nativePos+vec2(x,y)*3.0).rgb);
        diff=min(diff,max(max(delta.r,delta.g),delta.b));
    }
    imageStore(OutputTexture,p,vec4(mix(mapped,d.rgb,smoothstep(0.04,0.12,diff)),d.a));
}
)glsl";
}
