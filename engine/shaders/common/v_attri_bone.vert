layout(location = 0) in vec3 InPos;
layout(location = 1) in vec4 InBoneIds;
layout(location = 2) in vec4 InBoneWeights; 

layout(location = 3) in vec2 InUv;
layout(location = 4) in vec3 InVn;
layout(location = 5) in vec3 InTangent;


uniform vec3 camPos;
uniform vec3 camDir;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 proMat;
uniform mat4 mvp;

uniform mat4 u_boneMats[128];

layout(location = 0) out vec2 v_uv;
layout(location = 1) out vec3 v_pos;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_tangent;
layout(location=4) out vec3 v_front;
layout(location = 5) out vec4 v_worldpos;
layout(location=6) out vec3 v_view;