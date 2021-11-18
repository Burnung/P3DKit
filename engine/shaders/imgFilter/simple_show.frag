#version 430

uniform vec4 srcColor;
uniform float srcAlpha;
layout(location=0) out vec4 outColor;

void main(){
	
	//outColor = vec4(tColor.xyz,tColor.w);
	outColor = vec4(srcColor.rgb,srcAlpha);
	//outColor = vec4(1.0,0.0,1.0,srcAlpha);
	//outColor = vec4(a_uv.xy,0.0,0.0);
}
