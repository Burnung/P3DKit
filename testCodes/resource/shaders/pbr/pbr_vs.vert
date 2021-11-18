#version 450 core
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Physically Based shading model: Vertex program.
#include"../common/v_attri.vert"


layout(std140, binding=0) uniform TransformUniforms

{
	mat4 viewProjectionMatrix;
	mat4 skyProjectionMatrix;
	mat4 sceneRotationMatrix;
};

layout(location=8) out mat3 tangentBasis;


void main()
{
	fpos = vec3(modelMat * vec4(InPos, 1.0));
	fuv = vec2(InUv.x, 1.0-InUv.y);

	// Pass tangent space basis vectors (for normal mapping).
	tangentBasis = mat3(modelMat) * mat3(InTangent, bitangent, InVn);

	gl_Position = mvp * modelMat * vec4(InPos, 1.0);
}
