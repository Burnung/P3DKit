#version 450 core
#include"../common/f_attri.frag"
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Environment skybox: Fragment program.

layout(location=0) out vec4 color;


layout(binding=0) uniform samplerCube envTexture;

const float PI = 3.1415927;
const float TwoPI = 2 * PI;
uniform float lod;
void main()
{
	//vec3 envVector = normalize(fpos);
	//envVector.y = -envVector.y;
	vec2 tpos = fpos.xy*0.5+0.5;
	float phi = tpos.x * TwoPI-PI;
	float theta = tpos.y * PI;
	float y = cos(theta);
	float sinT = sin(theta);
	vec3 tv = vec3(sinT*cos(phi),y,sinT*sin(phi));
	tv = normalize(tv);
	color = vec4(tpos.xy,0.0,1.0);
	color = vec4(tv,1.0);
	int lev = textureQueryLevels(envTexture);
	color = textureLod(envTexture, tv,lod*lev);
}
