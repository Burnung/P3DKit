#version 430
#include"common/v_attri.vert"

void main(){
	vec4 wPos = modelMat *vec4(InPos.xyz,1.0);
	fpos = wPos.xyz/wPos.w;
	gl_Position = mvp*wPos;

	mat4 modelViewMat = viewMat*modelMat;

	v_normal = normalize((modelViewMat*vec4(InVn.xyz, 0.0)).xyz);
	v_front = normalize((modelViewMat*vec4(0.0, 0.0, 1.0, 0.0)).xyz);
	//v_tangent = normalize((modelViewMat*vec4(InTangent.xyz, 0.0)).xyz);

	v_view = (viewMat*wPos).xyz;

	v_worldpos = vec4(InPos.xyz, 1.0);
	fuv = InUv.xy;//*vec2(1.0,-1.0);
    //fuv = vec2(InUv.x,1.0-InUv.y);
} 
