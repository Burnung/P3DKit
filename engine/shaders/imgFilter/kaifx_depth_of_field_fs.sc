$input v_texcoord0

/*
* Created by sunjinda on 1/14/21.
* Copyright © 2021 Kwai Inc. All rights reserved.
*/

#include "../common/common.sh"
#define Quality 8.0 //Bokeh quality (reduces noise) [2. 4. 6. 8. 16.]
#define Range 1.0   //Focus range [.1 .5 1. 2. 5.]
#define Bias 0.0    //Bias towards brightness [0. 1. 3. 5. 8.]

SAMPLER2D(s_texColor, 0);
SAMPLER2D(s_normalDepth, 1);
uniform vec4 u_BlurLevel; //x is blur level, z is clear radius

float blurSize(float depth, float focusPoint, float focusScale){
    float clearRadius = u_BlurLevel.z;
    float coc = 0.0;
    float distance_far = focusPoint + clearRadius;
    if (depth >= distance_far){
        coc = clamp((100.0 / distance_far - 100.0 / depth) * focusScale ,1.0,30.0)/4.0;
    }
    return coc;
}

vec2 hash(vec2 v0){
    vec2 v = vec2( v0.x * 94.55 + v0.y * -69.38, v0.x * -89.27 + v0.y * 78.69 );
	return normalize(vec2(fract(cos(v.x)),fract(cos(v.y)))*vec2_splat(825.79)-vec2_splat(0.5));
}

vec4 pack_color(vec4 col){
    float gray = dot(col,vec4(.299,.587,.114,0));
    return col*pow(gray,Bias);
}

vec4 unpack_color(vec4 col){
    float gray = pow(dot(col,vec4(.299,.587,.114,0)),Bias/(1.+Bias));
    return col/gray;
}

void main(){
    if (u_BlurLevel.x == 0.0){  
        gl_FragColor = texture2D(s_texColor, v_texcoord0);
        return;
    }
    vec2 step = vec2(0.001,0.001);
    float autoFocus = texture2D(s_normalDepth, vec2(0.5,0.5)).w*100.0;
    float depth = texture2D(s_normalDepth, v_texcoord0).w * 100.0;
    float far = autoFocus + u_BlurLevel.z;;
    float radius = blurSize(depth, autoFocus, u_BlurLevel.x);

    float weight = 0.0;
    vec4 col = vec4_splat(0.0);
    vec4 tmp_tex;
    float tmp_depth;
    vec2 uv;
    float d = 1.0;
    vec2 samp = hash(v_texcoord0)*radius/Quality;
    for(int i = 0;i<int(Quality*Quality/4.0);i++){
        d += 1.0/d;
        samp = vec2( samp.x * -0.73736882209777832 + samp.y * 0.67549037933349609, samp.x * -0.67549037933349609 + samp.y * -0.73736882209777832 );
        uv = v_texcoord0 + samp*vec2(d-1.0,d-1.0)*step;
        tmp_tex = texture2D(s_texColor,uv);
        col += pack_color(tmp_tex);
        weight = weight + 1.0;
	}
    gl_FragColor =  unpack_color(col / weight);
}
