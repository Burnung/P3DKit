#version 410
layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 attri1;
layout(location = 2) in vec3 attri2;
layout(location = 3) in vec3 attri3;

uniform float orthWorldWidth;
uniform float orthWorldHeight;
uniform vec3 camePos;
uniform vec3 camDir;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 proMat;
uniform mat4 mvp;
layout(location = 0) out vec3 oattri1;
layout(location = 1) out vec3 oattri2;
layout(location = 2) out vec3 oattri3;
void main(){
    
    vec4 temPos =  mvp* modelMat * vec4(InPos.x,InPos.y,InPos.z,1.0);
    gl_Position = vec4(temPos.x/temPos.w ,temPos.y/temPos.w,0.5,1.0);
    oattri1 = attri3;
    oattri2 = attri2;
    oattri3 = attri1;
   //gl_Position = vec4(InPos.x,(InPos.y-1.55),0.0,1.0);

} 