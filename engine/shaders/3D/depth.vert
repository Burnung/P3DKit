#version 430
layout(location = 0) in vec3 InPos;
layout(location = 1) in vec2 InUv;
layout(location = 2) in vec3 InVn;
uniform float orthWorldWidth;
uniform float orthWorldHeight;

uniform vec3 camDir;
uniform vec3 camPos;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 proMat;
uniform mat4 mvp;

layout(location = 0) out float fdepth;

/*
* Created by ZhengYi on 9/16/20.
* Copyright © 2020 Kwai Inc. All rights reserved.
*/


void main()
{
	gl_Position = mvp * vec4(InPos.xyz,1.0);
	fdepth = gl_Position.z / gl_Position.w;

}
