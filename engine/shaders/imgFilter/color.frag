#version 430
layout(location=0) in vec2 a_uv;

layout(location=0) out vec4 outColor;
uniform vec4 u_color;
void main(){
	outColor = vec4(u_color);
}
