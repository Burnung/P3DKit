#version 430
layout(location=0) in vec2 fuv;

uniform vec2 u_step;
uniform int u_radius;
uniform sampler2D s_tex;

layout vec4 outColor;
void main()
{
    vec2 step = u_step.xy;
    vec2 uv = vec2(0.0);
    int radius = u_radius;
    vec3 color = vec3(0.0);
    float wSum = 0.0f, w = 0.0f;
    float sigma = float(radius) / 3.0;
    float ratio = 1.0 / (2.0 * sigma * sigma);
    for (int i = -radius; i <= radius; ++i){
        w = exp(-float(i) * float(i) * ratio);
        uv = clamp(fuv + step * float(i), step, 1.0 - step);
        color += w * texture(s_tex, fuv).rgb;
        wSum += w;
    }
	outColor = vec4(color / wSum, 1.0);
}
