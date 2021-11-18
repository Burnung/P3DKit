#version 410
layout(location =  0) out vec4 outColor;
//layout(location =  1) out vec4 outAttri1;
//layout(location =  2) out vec4 outAttri2;
//layout(location =  3) out vec4 outAttri3;

//layout(location = 0) in vec3 attri1;
//layout(location = 1) in vec3 attri2;
//layout(location = 2) in vec3 attri3;

uniform float camPos[3];
//uniform float tmp[3];;
uniform vec3 camDir;

void main(){
    //outColor = vec4(1.0,1.0,0.0,1.0);
    outColor = vec4(camPos[1],camPos[1],camPos[2],1.0);
    //outColor = vec4(1.0,0.0,0.0,1.0);
    //outAttri1 = vec4(attri1,1.0);
    //outAttri2 = vec4(attri2,1.0);
    //outAttri1 = vec4(attri3,1.0);
} 
