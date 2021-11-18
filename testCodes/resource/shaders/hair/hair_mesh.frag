#version 430
layout(location = 0) in vec2 fuv;
layout(location =  0) out vec4 outColor;

void main(){
    //outColor = vec4(1.0,1.0,1.0,1.0);
    outColor = vec4(fuv,fuv.x,1.0);
} 
