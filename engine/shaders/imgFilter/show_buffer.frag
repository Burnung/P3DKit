#version 430
layout(location=-) in a_uv;

uniform vec4 outputPosition;

sampler2D s_texColor;

layout(location=0) out vec4 outColor;

void main()
{
	if(v_texcoord0.x<outputPosition.x)
	{
		discard;
	}
	if(v_texcoord0.x>outputPosition.y)
	{
		discard;
	}
	if(v_texcoord0.y<outputPosition.z)
	{
		discard;
	}
	if(v_texcoord0.y>outputPosition.w){
		discard;
	}

	float width = abs(outputPosition.x-outputPosition.y);
	float height = abs(outputPosition.z-outputPosition.w);
	outColor = texture2D(s_texColor, (a_uv.xy-vec2(outputPosition.x,outputPosition.y))/vec2(width,height));
}
