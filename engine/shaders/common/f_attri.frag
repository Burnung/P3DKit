
layout(location = 0) in vec2 fuv;
layout(location = 1) in vec3 fpos;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_tangent;
layout(location=4) in vec3 v_front;
layout(location = 5) in vec4 v_worldpos;
layout(location=6) in vec3 v_view;

#define saturate(_x) clamp(_x, 0.0, 1.0)
#define atan2(_x,_y) tan(_x,_y)
#define SAMPLER2D(_name, _reg)       uniform sampler2D _name
#define SAMPLER2DMS(_name, _reg)     uniform sampler2DMS _name
#define SAMPLER3D(_name, _reg)       uniform sampler3D _name
#define SAMPLERCUBE(_name, _reg)     uniform samplerCube _name
#define SAMPLER2DSHADOW(_name, _reg) uniform sampler2DShadow _name
#define mul(_a, _b) ( (_a) * (_b) )