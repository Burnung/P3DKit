#version 430
layout(location = 0) in vec2 fuv;

layout(location = 1) in vec3 fpos;

layout(location = 2) in vec3 fnormal;

layout(location =  0) out vec4 outColor1;


uniform float camPos[3];

void main(){

    //outColor1  = vec4(fuv.xy,0.0,1.0);
    outColor1 = vec4(fnormal,1.0);
    //outColor1 = vec4(1.0);

} 