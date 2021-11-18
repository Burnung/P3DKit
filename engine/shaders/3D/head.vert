#version 430
layout(location = 0) in vec3 InPos;
layout(location = 1) in vec2 InUv;
layout(location = 2) in vec3 InVn;
uniform float orthWorldWidth;
uniform float orthWorldHeight;


uniform vec3 camDir;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 proMat;
uniform mat4 mvp;
layout(location = 0) out vec2 fuv;
layout(location = 1) out vec3 fpos;
layout(location = 2) out vec3 fnormal;
void main(){
    vec4 temPos =  mvp * modelMat* vec4(InPos.x,InPos.y,InPos.z,1.0);
    float tTan = tan(0.5);
    vec4 retPos = vec4(-InPos.x/(InPos.z*tTan),-InPos.y/(InPos.z*tTan),0.5,1.0);
    //gl_Position = retPos;
    //gl_Position = vec4(temPos.x/temPos.w ,temPos.y/temPos.w,temPos.z / temPos.w,1.0);
    gl_Position = temPos;
    
    //vec4 temPos = mvp * modelMat * vec4(InPos.x,InPos.y,InPos.z,1.0);
    //gl_Position = vec4(temPos.x/temPos.w,temPos.y/temPos.w,0.0,1.0);
    fuv = InUv;
    fpos = InPos;
    fnormal = normalize(InVn);
    fnormal = fnormal*0.5+0.5;
    //fnormal = normalize((modelMat * vec4(InVn,0.0)).xyz);
   //gl_Position = vec4(InPos.x,(InPos.y-1.55),0.0,1.0);

} 