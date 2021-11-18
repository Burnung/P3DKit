
#version 430
layout(location = 0) in vec3 InPos;
layout(location = 1) in vec2 InUv;
uniform float orthWorldWidth;
uniform float orthWorldHeight;
uniform vec3 camePos;
uniform vec3 camDir;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 proMat;
uniform mat4 mvp;
layout(location = 0) out vec2 fuv;
void main(){
    
    vec4 temPos =  mvp* modelMat * vec4(InPos.x,InPos.y,InPos.z,1.0);
    gl_Position = vec4(temPos.x/temPos.w ,temPos.y/temPos.w,temPos.z / temPos.w,1.0);
    //vec4 temPos = mvp * modelMat * vec4(InPos.x,InPos.y,InPos.z,1.0);
    //gl_Position = vec4(temPos.x/temPos.w,temPos.y/temPos.w,0.0,1.0);
    fuv = InUv;
   //gl_Position = vec4(InPos.x,(InPos.y-1.55),0.0,1.0);

} 