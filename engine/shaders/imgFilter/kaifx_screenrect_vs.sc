$input a_position, a_texcoord0
$output v_texcoord0

/*
* Created by ZhengYi on 9/19/20.
* Copyright © 2020 Kwai Inc. All rights reserved.
*/

#include "../common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_texcoord0 = a_texcoord0;
}
