#version 430
layout(location = 0) in vec3 InPos;
layout(location = 1) in vec2 InUv;
layout(location = 2) in vec3 InVn;
uniform float orthWorldWidth;
uniform float orthWorldHeight;

uniform vec3 camDir;
uniform vec3 camPos;
uniform vec4 u_viewRect;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 proMat;
uniform mat4 mvp;

uniform mat4 u_mtx;

layout(location=0) out vec3 v_dir;

void main()
{
	gl_Position = mvp * vec4(a_position, 1.0);

	float fov = radians(45.0);
	float height = tan(fov*0.5);
	float aspect = height*(u_viewRect.z / u_viewRect.w);
	vec2 tex = (2.0*InUv-1.0) * vec2(aspect, height);
	v_dir = (u_mtx*vec4(tex, 1.0, 0.0)).xyz;
}
