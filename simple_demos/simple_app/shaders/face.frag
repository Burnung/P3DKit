#version 430
layout(location = 0) in vec2 fuv;

layout(location = 1) in vec3 fpos;

layout(location = 2) in vec3 fnormal;

layout(location = 3) in vec3 ftangent;
layout(location =  0) out vec4 outColor1;

uniform sampler2D face_tex;
uniform float t_alpha;

uniform float camPos[3];

void main(){

    //outColor1  = vec4(fuv.xy,0.0,1.0);
    vec3 litV = normalize(vec3(0.0,0.3,1.0));
    vec3 cam_dir = normalize(-fpos);
    vec3 hDir = normalize((litV+cam_dir)*0.5);

    float spec = pow(dot(hDir,fnormal),20) * 0.2;
    
    float shading = dot(litV,fnormal);
    //outColor1 = vec4(fnormal,1.0);
    vec4 tColor = texture(face_tex,fuv);
    //outColor1 = vec4(clamp(vec3(spec+ shading*0.5+0.2),0.0,1.0),t_alpha);
    outColor1 = vec4(fnormal*0.5+0.5,1.0);
    //outColor1 = vec4(vec3(shading*0.5+0.2),1.0);
  //  outColor1 =vec4(tColor.bgr*shading,1.0);
    //outColor1 =vec4(fnormal*0.5+0.5,1.0);
    
   // outColor1 = vec4(ftangent,1.0);
    //outColor1 = vec4(1.0);

} 