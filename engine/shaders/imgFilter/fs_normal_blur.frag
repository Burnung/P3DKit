#version 430
layout(location=0) in vec2 a_uv;

uniform sampler2D s_texColor;

uniform vec4 u_step;

layout(location = 0) out vec4 outColor;

void main()
{
    //outColor = vec4(a_uv.xy,0.0,1.0);
    //return;
    vec4 baseNormal = texture(s_texColor,a_uv);
    float depth = mix(1.0,0.0,baseNormal.w);
    vec2 step = u_step.xy * vec2(depth*2);
    vec2 uv = vec2(0.0,0.0);
    vec4 color = vec4(0.0);
    vec4 wSum = vec4(0.0f);
    vec3 w = vec3(0.0f);
    vec3 sigma = vec3(4.0) / vec3(3.0);
    vec3 ratio = vec3(10.0) / (sigma * sigma) * pow(1-depth,4.0);
    for (int i = -3; i <= 3; ++i)
    {
        w = vec3(exp(-float(i) * float(i) * ratio.x));
        uv = clamp(a_uv + step * float(i), 0.0, 1.0);
        vec4 nd = texture(s_texColor, uv);
        if(nd.x>0.0 || nd.y>0.0)
        {
            color.xy += w.xy * nd.xy;
            wSum.xy += w.xy;
        }
    }
    color.w = baseNormal.w;//Depth
    color.z = baseNormal.z;//ID
    wSum.w = 1.0;
    wSum.z = 1.0;

    outColor = color / wSum;
}
