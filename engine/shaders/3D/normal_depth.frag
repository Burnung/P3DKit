#version 430
#include"../common/f_attri.frag"


layout(location =0) out vec4 outColor;

void main(){
    float dis = length(v_view.xyz)/100.0;
    outColor = vec4(normalize(v_normal.xyz)*0.5+0.5,1.0);
}
