#version 450 core

in VERTEX_OUT 
{
	vec3 myOrigin;
	vec3 myDirection;
	vec2 myTexcoord;
} VertexIn;

layout (location = 0) out vec4 oColor;

uniform sampler2D uDepth;

layout (std140, binding = 0) uniform GLOBAL_BUFFER
{ 
	mat4 myProjectionMatrix;
	mat4 myPrevProjectionMatrix;
	mat4 myViewMatrix;
	mat4 myPrevViewMatrix;
	mat4 myInvProjViewMatrix;
	vec4 myResolutionAndTime;
} CameraBuffer;

// ------------------------------------------------------------------------------------
mat2 mm2(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}
mat2 m2 = mat2(0.95534, 0.29552, -0.29552, 0.95534);
float tri(in float x){return clamp(abs(fract(x)-.5),0.01,0.49);}
vec2 tri2(in vec2 p){return vec2(tri(p.x)+tri(p.y),tri(p.y+tri(p.x)));}

float triNoise2d(in vec2 p, float spd)
{
    float z=1.8;
    float z2=2.5;
	float rz = 0.;
    p *= mm2(p.x*0.06);
    vec2 bp = p;
	for (float i=0.; i<5.; i++ )
	{
        vec2 dg = tri2(bp*1.85)*.75;
        dg *= mm2(CameraBuffer.myResolutionAndTime.z*spd);
        p -= dg/z2;

        bp *= 1.3;
        z2 *= .45;
        z *= .42;
		p *= 1.21 + (rz-1.0)*.02;
        
        rz += tri(p.x+tri(p.y))*z;
        p*= -m2;
	}
    return clamp(1./pow(rz*29., 1.3),0.,.55);
}

float hash21(in vec2 n){ return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453); }
vec4 aurora(vec3 ro, vec3 rd)
{
    vec4 col = vec4(0);
    vec4 avgCol = vec4(0);
    
    for(float i=0.;i<30.;i++)
    {
        float of = 0.006*hash21(gl_FragCoord.xy)*smoothstep(0.,15., i);
        float pt = ((.8+pow(i,1.4)*.002)-ro.y)/(rd.y);
        pt -= of;
    	vec3 bpos = ro + pt*rd;
        vec2 p = bpos.zx * .4;
        float rzt = triNoise2d(p, 0.1);
        vec4 col2 = vec4(0,0,0, rzt);
        col2.rgb = (sin(1.-vec3(2.15,-.5, 1.2)+i*0.043)*0.5+0.5)*rzt;
        avgCol =  mix(avgCol, col2, .5);
        col += avgCol*exp2(-i*0.065 - 2.5)*smoothstep(0.,5., i);
        
    }
    
    col *= (clamp(rd.y*15.+.4,0.,1.));
    
    
    //return clamp(pow(col,vec4(1.3))*1.5,0.,1.);
    //return clamp(pow(col,vec4(1.7))*2.,0.,1.);
    //return clamp(pow(col,vec4(1.5))*2.5,0.,1.);
    //return clamp(pow(col,vec4(1.8))*1.5,0.,1.);
    
    //return smoothstep(0.,1.1,pow(col,vec4(1.))*1.5);
    return col*1.8;
    //return pow(col,vec4(1.))*2.
}


//-------------------Background and Stars--------------------

//From Dave_Hoskins (https://www.shadertoy.com/view/4djSRW)
vec3 hash33(vec3 p)
{
    p = fract(p * vec3(443.8975,397.2973, 491.1871));
    p += dot(p.zxy, p.yxz+19.27);
    return fract(vec3(p.x * p.y, p.z*p.x, p.y*p.z));
}

vec3 nmzHash33(vec3 q)
{
    uvec3 p = uvec3(ivec3(q));
    p = p*uvec3(374761393U, 1103515245U, 668265263U) + p.zxy + p.yzx;
    p = p.yzx*(p.zxy^(p >> 3U));
    return vec3(p^(p >> 16U))*(1.0/vec3(0xffffffffU));
}

vec3 stars(in vec3 p)
{
    vec3 c = vec3(0.);
    float res = CameraBuffer.myResolutionAndTime.x*1.;
    
	for (float i=0.;i<4.;i++)
    {
        vec3 q = fract(p*(.15*res))-0.5;
        vec3 id = floor(p*(.15*res));
        vec2 rn = nmzHash33(id).xy;
        float c2 = 1.-smoothstep(0.,.6,length(q));
        c2 *= step(rn.x,.0005+i*i*0.001);
        c += c2*(mix(vec3(1.0,0.49,0.1),vec3(0.75,0.9,1.),rn.y)*0.1+0.9);
        p *= 1.3;
    }
    return c*c*.8;
}

vec3 bg(in vec3 rd)
{
    float sd = dot(normalize(vec3(-0.5, -0.6, 0.9)), rd)*0.5+0.5;
    sd = pow(sd, 5.);
    vec3 col = mix(vec3(0.05,0.1,0.2), vec3(0.1,0.05,0.2), 1.0f);
    return col*.63;
}

//-----------------------------------------------------------

void main()
{	
	if (texture(uDepth, VertexIn.myTexcoord).x < 0.99999)
	{
		discard;
	}
	
	vec2 q = VertexIn.myTexcoord;
    vec2 p = q - 0.5;
	p.x*= CameraBuffer.myResolutionAndTime.x / CameraBuffer.myResolutionAndTime.y;
    
    vec3 ro = VertexIn.myOrigin;
	ro.xyz *= 0.005;
	ro.y = 0.4;
	vec3 rd = VertexIn.myDirection;
    
    vec3 col = vec3(0.);
    vec3 brd = rd;
    float fade = smoothstep(0.,0.01,abs(brd.y))*0.1+0.9;
    
    col = bg(rd);    

	vec4 aur = smoothstep(0.,1.5,aurora(ro,rd))*fade;
	// col += stars(rd);
	col = col*(1.-aur.a) + aur.rgb; 
	oColor = vec4(col, 1.);
}