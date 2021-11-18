#version 430
#include"../common/v_attri.vert"

void main(){
    vec3 wPos = (modelMat*vec4(InPos.xyz,1.0)).xyz;

    gl_Position = mvp*vec4(wPos.xyz,1.0);

    vec3 wNormal = (viewMat*modelMat *vec4(InVn,0.0)).xyz;

    v_normal = normalize(wNormal);
    v_view = camPos - wPos;
} 