#version 430
layout(location=0) in vec2 a_uv;

uniform sampler2D s_tex;
uniform vec4 rect;

layout(location=0) out vec4 outColor;

void main(){
	vec2 src_pos = a_uv * 2.0 -1.0;
	if (src_pos.x < rect.x || src_pos.x > rect.y || src_pos.y < rect.z || src_pos.y > rect.w)
		discard;
	float n_width = (rect.y-rect.x);
	float n_height = (rect.w-rect.z);
	float new_x = (src_pos.x - rect.x) / n_width;
	float new_y = (src_pos.y - rect.z) / n_height;
	vec2 n_uv = vec2(new_x,new_y);

	vec4 tColor = texture(s_tex,n_uv).xyzw;
	//outColor = vec4(tColor.xyz,tColor.w);
	outColor = vec4(tColor.xyz,tColor.w);
	//outColor = vec4(a_uv.xy,0.0,0.0);
}
