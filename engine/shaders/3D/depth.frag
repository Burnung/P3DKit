#version 430
layout(location=0) in float fdepth;

layout(location=1) out vec4 outColor;

void main(){
	outColor = vec4(fdepth);
}
