#version 430

layout(location=0) out vec3 v_dir;

uniform samplerCube s_texCube;
uniform samplerCube s_texCubeIrr;
uniform vec4 skySphereProperty;

layout(location=0) out vec4 outColor;

vec4 toLinear(vec4 _rgb){
	return vec4(pow(abs(_rgb.xyz), vec3(2.2)),_rgb.w);
}
vec3 toFilmic(vec4 _rgb){
	_rgb.xyz = max(vec3(0.0), _rgb.xyz - 0.004);
	_rgb.xyz = (_rgb.xyz*(6.2*_rgb.xyz + 0.5) ) / (_rgb.xyz*(6.2*_rgb.xyz + 1.7) + 0.06);
	return _rgb;
}

void main(){
	float bgType = skySphereProperty.x;
	float exposure = skySphereProperty.y;

	vec3 dir = normalize(v_dir);
	vec4 bgColor = textureCube(s_texCubeIrr, dir);
	vec4 color;
	if (bgType > 5.0)
	{
		color = toLinear(bgColor);
	}
	else
	{
		float lod = bgType;
		dir = fixCubeLookup(dir, lod, 256.0);
		vec4 lodColor = textureCubeLod(s_texCube, dir, lod);
		color = toLinear(mix(lodColor,bgColor,max(0.0,min(1.0,(bgType-3.0)/2.0))));
	}
	color *= exp2(exposure);

	outColor = toFilmic(color);
}
