#version 430
layout(location=0) in vec2 a_uv;

uniform sampler2D s_tex;

layout(location=0) out vec4 outColor;

void main(){
	
	vec4 tColor = texture(s_tex,a_uv).xyzw;
	//outColor = vec4(tColor.xyz,tColor.w);
	outColor = vec4(tColor.xyz,tColor.w);
	//outColor = vec4(a_uv.xy,0.0,0.0);
}
