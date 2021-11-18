#version 450 core
#include"../common/f_attri.frag"
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Environment skybox: Fragment program.

layout(location=0) out vec4 color;


layout(binding=0) uniform samplerCube envTexture;


void main()
{
	vec3 envVector = normalize(fpos);
	color = vec4(1.0);
	//color = textureLod(envTexture, envVector, 0);
}
