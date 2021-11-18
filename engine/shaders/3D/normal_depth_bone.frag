#version 430
#include"../common/f_attri.frag"


layout(location =0) out vec4 outColor;

void main(){
    float dis = length(v_view.xyz)/100.0;
    outColor = vec4(normalize(v_normal.xyz).xy*0.5+0.5,1.0,dis);
    //outColor = vec4(dis*10,dis,dis,1.0);
   // outColor = vec4(1.0,0.0,0.0,0.5);
}
