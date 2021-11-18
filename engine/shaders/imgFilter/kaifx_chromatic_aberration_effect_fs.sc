$input v_texcoord0

/*
* Created by ZhengYi on 9/19/20.
* Copyright © 2020 Kwai Inc. All rights reserved.
*/

#include "../common/common.sh"

uniform vec4 CAProperty;

SAMPLER2D(s_texColor, 0);

void main()
{
    vec4 rgb = texture2D(s_texColor, v_texcoord0);
    vec2 uvBias = normalize(v_texcoord0-vec2(0.5,0.5));
    float centerDist = max(0.0,length(v_texcoord0-vec2(0.5,0.5))-CAProperty.w);
    float R_CA = centerDist * CAProperty.x;
    float G_CA = centerDist * CAProperty.y;
    float B_CA = centerDist * CAProperty.z;
    if(R_CA>0.0)
    {
        vec2 uvBias = vec2_splat(R_CA*0.001);
        float _r = 0.0;
        for(int i=-3;i<=3;i+=2)
        {
            _r += texture2D(s_texColor, max(vec2_splat(0.0),min(vec2_splat(1.0),v_texcoord0+uvBias*vec2_splat(i)))).r;
        }
        _r = _r*0.25;
        rgb.r = mix(rgb.r,_r,min(1.0,R_CA));
    }
    if(G_CA>0.0)
    {
        vec2 uvBias = vec2_splat(G_CA*0.001);
        float _g = 0.0;
        for(int i=-3;i<=3;i+=2)
        {
            _g += texture2D(s_texColor, max(vec2_splat(0.0),min(vec2_splat(1.0),v_texcoord0+uvBias*vec2_splat(i)))).g;
        }
        _g = _g*0.25;
        rgb.g = mix(rgb.g,_g,min(1.0,G_CA));
    }
    if(B_CA>0.0)
    {
        vec2 uvBias = vec2_splat(B_CA*0.001);
        float _b = 0.0;
        for(int i=-3;i<=3;i+=2)
        {
            _b += texture2D(s_texColor, max(vec2_splat(0.0),min(vec2_splat(1.0),v_texcoord0+uvBias*vec2_splat(i)))).b;
        }
        _b = _b*0.25;
        rgb.b = mix(rgb.b,_b,min(1.0,B_CA));
    }
    gl_FragColor = rgb;
}
