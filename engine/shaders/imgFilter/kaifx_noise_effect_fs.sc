$input v_texcoord0

/*
* Created by ZhengYi on 9/19/20.
* Copyright © 2020 Kwai Inc. All rights reserved.
*/

#include "../common/common.sh"

uniform vec4 NoiseProperty;

SAMPLER2D(s_texColor, 0);

float hash(vec3 p3)
{
    vec3 result = fract(p3 * 0.1031);
    result += dot(result,result.yzx + 19.19);
    return fract((result.x + result.y) * result.z);
}

float noise(vec3 x)
{
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    return mix(mix(mix(hash(i+vec3(0.0, 0.0, 0.0)), 
                       hash(i+vec3(1.0, 0.0, 0.0)),f.x),
                   mix(hash(i+vec3(0.0, 1.0, 0.0)), 
                       hash(i+vec3(1.0, 1.0, 0.0)),f.x),f.y),
               mix(mix(hash(i+vec3(0.0, 0.0, 1.0)), 
                       hash(i+vec3(1.0, 0.0, 1.0)),f.x),
                   mix(hash(i+vec3(0.0, 1.0, 1.0)), 
                       hash(i+vec3(1.0, 1.0, 1.0)),f.x),f.y),f.z);
}

float grain_source(vec3 x, float strength, float pitch)
{
    float center = noise(x);
    float v1 = center - noise(vec3( -1.0, -1.0, 0.0)/pitch + x) + 0.5;
    float v2 = center - noise(vec3( 1.0, 1.0, 0.0)/pitch + x) + 0.5;
    float total = (v1 + v2) / 2.0;
    return mix(1.0, 0.5 + total, strength);
}

void main()
{
    vec4 rgb = texture2D(s_texColor, v_texcoord0);
    float grain_lift_ratio = NoiseProperty.x;
    float grain_strength = NoiseProperty.y;
    float grain_rate = NoiseProperty.z;
    float grain_pitch = NoiseProperty.w;
    float noise = grain_source(vec3(v_texcoord0*vec2(2048.0,2048.0), floor(grain_rate*100.0)), grain_strength, grain_pitch);
    vec3 grain = vec3_splat(noise);
	
    rgb.rgb = max(mix(rgb.rgb*grain, rgb.rgb+(grain-1.0), grain_lift_ratio), 0.0);
    gl_FragColor = rgb;
}
