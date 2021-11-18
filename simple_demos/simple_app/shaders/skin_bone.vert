#version 430
#include"common/v_attri_bone.vert"

void main(){
	mat4 boneMat = u_boneMats[int(InBoneIds.x)]*InBoneWeights.x;
	boneMat += u_boneMats[int(InBoneIds.y)]*InBoneWeights.y;
	boneMat += u_boneMats[int(InBoneIds.z)]*InBoneWeights.z;
	boneMat += u_boneMats[int(InBoneIds.w)]*InBoneWeights.w;

	vec4 wPos = modelMat * boneMat* vec4(InPos.xyz,1.0);
	wPos.xyz /=wPos.w;
	wPos.w = 1.0;
	gl_Position = mvp*wPos;

	mat4 modelViewMat = viewMat*modelMat;

	v_normal = normalize((modelViewMat*boneMat*vec4(InVn.xyz, 0.0)).xyz);
	v_front = normalize((modelViewMat*vec4(0.0, 0.0, 1.0, 0.0)).xyz);
	v_tangent = normalize((modelViewMat*boneMat*vec4(InTangent.xyz, 0.0)).xyz);

	v_view = (viewMat*wPos).xyz;

	v_worldpos = boneMat * vec4(InPos.xyz, 1.0);
	v_worldpos.xyz /= v_worldpos.w;
	v_worldpos.w = 1.0;
	fuv = InUv.xy;//*vec2(1.0,-1.0);
} 
