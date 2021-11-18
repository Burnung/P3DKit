#version 430

#include"common/v_attri.vert"

void main(){
    vec4 temPos =  mvp * modelMat* vec4(InPos.x,InPos.y,InPos.z,1.0);
    float tTan = tan(0.5);
    vec4 retPos = vec4(-InPos.x/(InPos.z*tTan),-InPos.y/(InPos.z*tTan),0.5,1.0);
    //gl_Position = retPos;
    //gl_Position = vec4(temPos.x/temPos.w ,temPos.y/temPos.w,temPos.z / temPos.w,1.0);
    gl_Position = temPos;
    
    //vec4 temPos = mvp * modelMat * vec4(InPos.x,InPos.y,InPos.z,1.0);
    //gl_Position = vec4(temPos.x/temPos.w,temPos.y/temPos.w,0.0,1.0);
    fuv = vec2(InUv.x,1.0-InUv.y);
    fpos = InPos;
    fnormal = normalize(InVn);
    fnormal = fnormal*0.5+0.5;
    ftangent = 0.5*normalize(InTangent)+0.5;
    //fnormal = normalize((modelMat * vec4(InVn,0.0)).xyz);
   //gl_Position = vec4(InPos.x,(InPos.y-1.55),0.0,1.0);

} 