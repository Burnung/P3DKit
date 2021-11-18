#version 430
#include"../common/v_attri_bone.vert"

void main(){
    vec4 tPos = vec4(0.0);
    tPos += u_boneMats[int(InBoneIds.x)]*vec4(InPos.xyz,1.0)*InBoneWeights.x;
    tPos += u_boneMats[int(InBoneIds.y)]*vec4(InPos.xyz,1.0)*InBoneWeights.y;
    tPos += u_boneMats[int(InBoneIds.z)]*vec4(InPos.xyz,1.0)*InBoneWeights.z;
    tPos += u_boneMats[int(InBoneIds.w)]*vec4(InPos.xyz,1.0)*InBoneWeights.w;
	
    mat4 boneMat = u_boneMats[int(InBoneIds.x)]*InBoneWeights.x;
	boneMat += u_boneMats[int(InBoneIds.y)]*InBoneWeights.y;
	boneMat += u_boneMats[int(InBoneIds.z)]*InBoneWeights.z;
	boneMat += u_boneMats[int(InBoneIds.w)]*InBoneWeights.w;
    boneMat += u_boneMats[int(InBoneIds.y)]*InBoneWeights.y;
    tPos = boneMat*vec4(InPos.xyz,1.0);

    vec3 wPos = (modelMat*vec4(tPos.xyz/tPos.w,1.0)).xyz;

    gl_Position = mvp*vec4(wPos.xyz,1.0);

    vec3 wNormal = (viewMat*modelMat*boneMat*vec4(InVn,0.0)).xyz;

    v_normal =  normalize(wNormal);
    //v_normal = normalize(boneMat[0].xyz);
    v_view = camPos - wPos;
} 