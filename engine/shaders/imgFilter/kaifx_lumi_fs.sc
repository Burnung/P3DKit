$input v_texcoord0

/*
* Created by ZhengYi on 9/19/20.
* Copyright © 2020 Kwai Inc. All rights reserved.
*/

#include "../common/common.sh"

uniform vec4 lumiProperty;

SAMPLER2D(s_texColor, 0);

void main()
{
	vec4 rgb = texture2D(s_texColor, v_texcoord0) * lumiProperty.w;
	float lumi = rgb.x*lumiProperty.x+rgb.y*lumiProperty.y+rgb.z*lumiProperty.z;
	gl_FragColor = toGamma(vec4(lumi,lumi,lumi, 1.0) );
}
