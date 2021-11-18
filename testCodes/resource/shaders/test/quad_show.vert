#version 450 core
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Environment skybox: Vertex program.
#include"../common/v_attri.vert"
layout(std140, binding=0) uniform TransformUniforms
{
	mat4 viewProjectionMatrix;
	mat4 skyProjectionMatrix;
	mat4 sceneRotationMatrix;
};

void main()
{
	fpos = InPos.xyz;//*0.5+0.5;
	gl_Position  =vec4(InPos.xyz,1.0);
	//gl_Position   = mvp * modelMat*vec4(InPos.xy,0.5, 1.0);
}
