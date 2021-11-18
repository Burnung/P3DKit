#version 430
uniform vec4 u_screenSize;
uniform vec4 u_lightPos0;
uniform vec4 u_lightColor0;
uniform vec4 u_lightPos1;
uniform vec4 u_lightColor1;
uniform vec4 u_lightPos2;
uniform vec4 u_lightColor2;
uniform vec4 u_mouthAO;
uniform vec4 u_material;
uniform vec4 u_material2;
uniform vec4 u_material3;
uniform vec4 u_material4;
uniform vec4 u_material5;
uniform vec4 u_BS1;
uniform vec4 u_BS2;
uniform vec4 u_BS3;
uniform vec4 u_BS4;
uniform vec4 u_BS5;
uniform vec4 u_BS6;
uniform vec4 u_BS7;
uniform vec4 u_BS8;
uniform vec4 u_BS9;
uniform vec4 u_BS10;
uniform vec4 u_BS11;
uniform vec4 u_BS12;
uniform vec4 u_BS13;
uniform mat4 u_iblMtx;
uniform mat4 u_lightMtx0;

//////////// 皱纹混合强度（法线） ////////////
const float normal_blend_pow = 0.2;

//////////// 皱纹混合强度（漫反射） ////////////
const float albedo_blend_pow = 0.2;

//////////// 皱纹高光遮罩强度 ////////////
const float spec_mask = 0.2;
/*
#if SHADOW_PACKED_DEPTH
uniform sampler2D  s_shadowMap, 0);
#	define Sampler sampler2D
#else
SAMPLER2DSHADOW(s_shadowMap, 0);
#	define Sampler sampler2DShadow
#endif // SHADOW_PACKED_DEPTH
*/
uniform sampler2D s_shadowMap;
uniform samplerCube s_rttCube;
uniform samplerCube s_rttCubeIrr;

uniform sampler2D s_rttNormalDepth;
uniform sampler2D  s_texAlbedo;
uniform sampler2D  s_texAO;
uniform sampler2D  s_texGloss;
uniform sampler2D  s_texNormal;
uniform sampler2D  s_texSSSPower;
uniform sampler2D  s_texDetailNormal;
uniform sampler2D  s_texNormalWrinkle1;
uniform sampler2D  s_texNormalWrinkleMask;
uniform sampler2D  s_texAlbedoWrinkle1;
uniform sampler2D  s_texSpecWrinkle1;

layout(location = 0) in vec2 fuv;
layout(location = 1) in vec3 fpos;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in  vec3 v_tangent;
layout(location=4) in  vec3 v_front;
layout(location = 5) in vec4 v_worldpos;
layout(location=6) in vec3 v_view;

vec4 mul(Mat4 m,vec4 v){
	return m*v;
}

float hardShadow(Sampler _sampler, vec4 _shadowCoord, float _bias)
{
	return step(texCoord.z-_bias, unpackRgbaToFloat(texture2D(_sampler, texCoord.xy) ) );
}

vec2 hash(vec2 v0)
{
    vec2 v = vec2( v0.x * 94.55 + v0.y * -69.38, v0.x * -89.27 + v0.y * 78.69 );
	return normalize(vec2(fract(cos(v.x)),fract(cos(v.y)))*vec2(825.79)-vec2(0.5));
}

float PCF(Sampler _sampler, vec4 _shadowCoord, float _bias, vec2 _texelSize)
{
	vec2 texCoord = _shadowCoord.xy/_shadowCoord.w;

	bool outside = any(greaterThan(texCoord, vec2(1.0)))
				|| any(lessThan   (texCoord, vec2(0.0)))
				 ;

	if (outside)
	{
		return 1.0;
	}

	float result = 0.0;
	vec2 offset = _texelSize * _shadowCoord.w;
	vec2 rot = hash(_shadowCoord.xy*vec2(100.0));
	float finalBias = _bias * mix(0.5,1.5,fract(texCoord.y*2048.0));
	for(int i=0;i<16;i++)
	{
		result += hardShadow(_sampler, _shadowCoord + vec4(reflect(Poisson2D_16[i],rot) * offset, 0.0, 0.0), finalBias );
	}
	return min(1.0,result / 15.0);
}

vec3 calcLambert(vec3 _cdiff, float _ndotl)
{
	return _cdiff * vec3(max(0.0,_ndotl));
}

vec3 calcBlinn(float _cspec, float _ndoth, float _ndotl, float _specPwr)
{
	float brdf = pow(_ndoth, _specPwr)*_ndotl;
	return vec3(_cspec*brdf);
}

vec3 decodeNormal(vec2 inputNormal)
{
    vec3 n;
    n.xy = inputNormal.xy*vec2(2.0)-vec2(1.0);
    n.z = -sqrt(1.0-n.x*n.x-n.y*n.y);
    n = normalize(n);
    return n;
}

float calEnvFresnel(float NDotV, float Strength, float F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - NDotV, 5.0) * Strength;
}

vec3 blend_rnm(vec4 n1, vec4 n2)
{
    vec3 t = n1.xyz*vec3( 2.0,  2.0, 2.0) + vec3(-1.0, -1.0,  0.0);
    vec3 u = n2.xyz*vec3(-2.0, -2.0, 2.0) + vec3( 1.0,  1.0, -1.0);
    vec3 r = t*dot(t, u) - u*t.z;
    return normalize(r);
}

float remap(float origin, float min, float max){
    return ((1.0-min)/max)*origin + min;
}

vec4 expression_normal(vec2 NormalMapDiffuse, vec2 NormalMapSpecular, vec2 tc, vec4 brow_mask1, vec4 brow_mask2, vec4 cheek_mask, vec4 nose_mask, vec4 mouth_mask1, vec4 mouth_mask2)
{
	// parameters
	float brow_up_l = u_BS1.w;//04
    float brow_up_r = u_BS2.x;//05
    float brow_up_c = remap(u_BS1.z,0.0,0.65);//03

    float brow_down_l = u_BS1.x;//01
    float brow_down_r = u_BS1.y;//02

    float eyeblink_l = u_BS3.x;//09
    float eyeblink_r = u_BS3.y;//10
    float eyesquint_l = u_BS5.z;//19
    float eyesquint_r = u_BS5.w;//20

    float cheeksquin_l = u_BS2.z;//07
    float cheeksquin_r = u_BS2.w;//08

    float noseup_l = u_BS13.z;//51
    float noseup_r = u_BS13.z;//51

    float smile_l = u_BS11.w;//44
    float smile_r = u_BS12.x;//45
    float dimple_l = u_BS7.w;//28
    float dimple_r = u_BS8.x;//29

    float mouth_l = u_BS9.x;//33
    float mouth_r = u_BS10.z;//39
    float mouth_puc = u_BS10.y;//38

    float mouthdown_l = u_BS8.y;//30
    float mouthdown_r = u_BS8.z;//31
    float mouthsad =  u_BS11.y;//42
	
	float brow_down_weights = max(brow_down_r*brow_mask2.r,brow_down_l*brow_mask2.g);
	
	// wrinkle_normals 
	vec2 wrinkle1 = texture2D(s_texNormalWrinkle1, tc.xy).xy*2.0-1.0;
	wrinkle1.y = -wrinkle1.y;

	// texture_blend
	NormalMapDiffuse.xy += wrinkle1*brow_down_weights*normal_blend_pow;
	NormalMapSpecular.xy += wrinkle1*brow_down_weights*normal_blend_pow;
 
	return vec4(NormalMapDiffuse.x,NormalMapDiffuse.y,NormalMapSpecular.x,NormalMapSpecular.y);
}

vec3 expression_albedo(vec3 OriginAlbedo, vec2 tc, vec4 brow_mask1, vec4 brow_mask2, vec4 cheek_mask, vec4 nose_mask, vec4 mouth_mask1, vec4 mouth_mask2)
{
	// parameters
	float brow_up_l = u_BS1.w;//04
    float brow_up_r = u_BS2.x;//05
    float brow_up_c = remap(u_BS1.z,0.0,0.65);//03

    float brow_down_l = u_BS1.x;//01
    float brow_down_r = u_BS1.y;//02

    float eyeblink_l = u_BS3.x;//09
    float eyeblink_r = u_BS3.y;//10
    float eyesquint_l = u_BS5.z;//19
    float eyesquint_r = u_BS5.w;//20

    float cheeksquin_l = u_BS2.z;//07
    float cheeksquin_r = u_BS2.w;//08

    float noseup_l = u_BS13.z;//51
    float noseup_r = u_BS13.z;//51

    float smile_l = u_BS11.w;//44
    float smile_r = u_BS12.x;//45
    float dimple_l = u_BS7.w;//28
    float dimple_r = u_BS8.x;//29

    float mouth_l = u_BS9.x;//33
    float mouth_r = u_BS10.z;//39
    float mouth_puc = u_BS10.y;//38

    float mouthdown_l = u_BS8.y;//30
    float mouthdown_r = u_BS8.z;//31
    float mouthsad =  u_BS11.y;//42

	// weights
	float brow_down_weights = max(brow_down_r*brow_mask2.r,brow_down_l*brow_mask2.g);

	// wrinkle_albedos
	vec3 wrinkle_albedo1 = texture2D(s_texAlbedoWrinkle1, tc).xyz;

	// mix
	OriginAlbedo = mix(OriginAlbedo,wrinkle_albedo1,brow_down_weights*albedo_blend_pow);

	return OriginAlbedo;
}

float expression_spec(float OriginSpecPow, vec2 tc, vec4 brow_mask1, vec4 brow_mask2, vec4 cheek_mask, vec4 nose_mask, vec4 mouth_mask1, vec4 mouth_mask2)
{
	// parameters
	float brow_up_l = u_BS1.w;//04
    float brow_up_r = u_BS2.x;//05
    float brow_up_c = remap(u_BS1.z,0.0,0.65);//03

    float brow_down_l = u_BS1.x;//01
    float brow_down_r = u_BS1.y;//02

    float eyeblink_l = u_BS3.x;//09
    float eyeblink_r = u_BS3.y;//10
    float eyesquint_l = u_BS5.z;//19
    float eyesquint_r = u_BS5.w;//20

    float cheeksquin_l = u_BS2.z;//07
    float cheeksquin_r = u_BS2.w;//08

    float noseup_l = u_BS13.z;//51
    float noseup_r = u_BS13.z;//51

    float smile_l = u_BS11.w;//44
    float smile_r = u_BS12.x;//45
    float dimple_l = u_BS7.w;//28
    float dimple_r = u_BS8.x;//29

    float mouth_l = u_BS9.x;//33
    float mouth_r = u_BS10.z;//39
    float mouth_puc = u_BS10.y;//38

    float mouthdown_l = u_BS8.y;//30
    float mouthdown_r = u_BS8.z;//31
    float mouthsad =  u_BS11.y;//42

	// weights
	float brow_down_weights = max(brow_down_r*brow_mask2.r,brow_down_l*brow_mask2.g);

	// wrinkle_spec
	float wrinkle_spec = texture2D(s_texSpecWrinkle1, tc).x;
	wrinkle_spec = mix(1.0,wrinkle_spec,brow_down_weights*spec_mask);

	//mix
	OriginSpecPow *= wrinkle_spec;
	
	return OriginSpecPow;
}

// Cellular noise ("Worley noise") in 3D in GLSL.
// Copyright (c) Stefan Gustavson 2011-04-19. All rights reserved.
// This code is released under the conditions of the MIT license.
// See LICENSE file for details.

// Permutation polynomial: (34x^2 + x) mod 289
vec4 permute(vec4 x) 
{
  return mod((34.0 * x + 1.0) * x, 289.0);
}
vec3 permute(vec3 x) 
{
  return mod((34.0 * x + 1.0) * x, 289.0);
}

// Cellular noise, returning F1 and F2 in a vec2.
// Speeded up by using 2x2x2 search window instead of 3x3x3,
// at the expense of some pattern artifacts.
// F2 is often wrong and has sharp discontinuities.
// If you need a good F2, use the slower 3x3x3 version.
vec2 cellular2x2x2(vec3 P) 
{
	#define K 0.142857142857 // 1/7
	#define Ko 0.428571428571 // 1/2-K/2
	#define K2 0.020408163265306 // 1/(7*7)
	#define Kz 0.166666666667 // 1/6
	#define Kzo 0.416666666667 // 1/2-1/6*2
	#define jitter 0.8 // smaller jitter gives less errors in F2
	vec3 Pi = mod(floor(P), 289.0);
 	vec3 Pf = fract(P);
	vec4 Pfx = Pf.x + vec4(0.0, -1.0, 0.0, -1.0);
	vec4 Pfy = Pf.y + vec4(0.0, 0.0, -1.0, -1.0);
	vec4 p = permute(Pi.x + vec4(0.0, 1.0, 0.0, 1.0));
	p = permute(p + Pi.y + vec4(0.0, 0.0, 1.0, 1.0));
	vec4 p1 = permute(p + vec4(Pi.z)); // z+0
	vec4 p2 = permute(p + vec4(Pi.z) + vec4(1.0)); // z+1
	vec4 ox1 = fract(p1*K) - Ko;
	vec4 oy1 = mod(floor(p1*K), 7.0)*K - Ko;
	vec4 oz1 = floor(p1*K2)*Kz - Kzo; // p1 < 289 guaranteed
	vec4 ox2 = fract(p2*K) - Ko;
	vec4 oy2 = mod(floor(p2*K), 7.0)*K - Ko;
	vec4 oz2 = floor(p2*K2)*Kz - Kzo;
	vec4 dx1 = Pfx + jitter*ox1;
	vec4 dy1 = Pfy + jitter*oy1;
	vec4 dz1 = vec4(Pf.z) + jitter*oz1;
	vec4 dx2 = Pfx + jitter*ox2;
	vec4 dy2 = Pfy + jitter*oy2;
	vec4 dz2 = vec4(Pf.z - 1.0) + jitter*oz2;
	vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1; // z+0
	vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2; // z+1

	// Sort out the two smallest distances (F1, F2)
#if 0
	// Cheat and sort out only F1
	d1 = min(d1, d2);
	d1.xy = min(d1.xy, d1.wz);
	d1.x = min(d1.x, d1.y);
	return sqrt(d1.xx);
#else
	// Do it right and sort out both F1 and F2
	vec4 d = min(d1,d2); // F1 is now in d
	d2 = max(d1,d2); // Make sure we keep all candidates for F2
	d.xy = (d.x < d.y) ? d.xy : d.yx; // Swap smallest to d.x
	d.xz = (d.x < d.z) ? d.xz : d.zx;
	d.xw = (d.x < d.w) ? d.xw : d.wx; // F1 is now in d.x
	d.yzw = min(d.yzw, d2.yzw); // F2 now not in d2.yzw
	d.y = min(d.y, d.z); // nor in d.z
	d.y = min(d.y, d.w); // nor in d.w
	d.y = min(d.y, d2.x); // F2 is now in d.y
	return sqrt(d.xy); // F1 and F2
#endif
}
layout(location = 0) vec4 outColor;
void main(){
	outColor = vec4(v_normal,1.0);
	/*
	
	float Specular1Power = u_material.x;
	float Specular1Gloss = u_material.y;
	float Specular2Power = u_material.z;
	float Specular2Gloss = u_material.w;

	float SpecularMix = u_material2.x;
	float F0 = u_material2.y;
	float AOPower = u_material2.z;
	float BeautiPower = u_material2.w;

	float NormalMapPower = u_material3.x;
	float DetailTextureScale = u_material3.y;
	float DetailTexturePower = u_material3.z;
	float SSSPower = u_material3.w;

	float EdgePower = u_material4.x;
	float EdgeGloss = u_material4.y;

	float GIPower = u_material5.x;
	float Exposure = u_material5.w;

	vec2 ScreenPos = gl_FragCoord.xy/u_screenSize.xy;
	vec4 rttNormalDepth = texture2D(s_rttNormalDepth,ScreenPos);
	vec3 SSSPowerMap = texture2D(s_texSSSPower, v_texcoord0).xyz;

	vec3 AlbedoMap = texture2D(s_texAlbedo, v_texcoord0).xyz;
	
	vec3 AOMap = texture2D(s_texAO, v_texcoord0).xyz;
	float AO = pow(AOMap.x,AOPower);
	vec3 BaseNormalMap = texture2D(s_texNormal, v_texcoord0).xyz;
	vec3 HightLightMap = texture2D(s_texGloss, v_texcoord0).xyz;
	float SpecularPower = HightLightMap.x;
	float Gloss = HightLightMap.y;
	float Wet = HightLightMap.z;

	//Beauti:
	vec3 skinSpot = vec3(SpecularPower);
	vec3 ifFlag= step(skinSpot,vec3(0.5));
	skinSpot = ifFlag*(AlbedoMap*skinSpot*vec3(2.0)+AlbedoMap*AlbedoMap*(vec3(1.0)-skinSpot*vec3(2.0)))+(vec3(1.0)-ifFlag)*(AlbedoMap*(vec3(1.0)-skinSpot)*vec3(2.0)+sqrt(AlbedoMap)*(vec3(2.0)*skinSpot-vec3(1.0)));
	AlbedoMap = mix(skinSpot,AlbedoMap,min(1.0,BeautiPower*0.5+0.5));

	vec3 NormalMapSpecular = mix(vec3(0.5,0.5,1.0),BaseNormalMap,min(NormalMapPower,1.0));
	vec3 NormalMapDiffuse = mix(vec3(0.5,0.5,1.0),BaseNormalMap,min(NormalMapPower*0.25,1.0));
	vec3 DetailNormalMap1 = texture2D(s_texDetailNormal, v_texcoord0*DetailTextureScale).xyz;
	vec3 DetailNormalMap2 = texture2D(s_texDetailNormal, v_texcoord0*DetailTextureScale*0.5).xyz;

	float DetailCave1 = DetailNormalMap1.z;
	DetailNormalMap1.z = 1.0;
	DetailNormalMap1 = mix(DetailNormalMap1,vec3(0.5,0.5,1.0),min(1.0,rttNormalDepth.w/500.0));
	DetailNormalMap1 = DetailNormalMap1*vec3(2.0)-vec3(1.0);
	DetailNormalMap1.y = -DetailNormalMap1.y;

	float DetailCave2 = DetailNormalMap2.z;
	DetailNormalMap2.z = 1.0;
	DetailNormalMap2 = mix(DetailNormalMap2,vec3(0.5,0.5,1.0),min(1.0,rttNormalDepth.w/500.0));
	DetailNormalMap2 = DetailNormalMap2*vec3(2.0)-vec3(1.0);
	DetailNormalMap2.y = -DetailNormalMap2.y;

	NormalMapDiffuse = NormalMapDiffuse * vec3(2.0) - vec3(1.0);
	NormalMapDiffuse.y = -NormalMapDiffuse.y;

	NormalMapSpecular = NormalMapSpecular * vec3(2.0) - vec3(1.0);
	NormalMapSpecular.y = -NormalMapSpecular.y;
	
	{//expression
		//expression_masks
		vec2 tc = v_texcoord0;
		vec2 uv1 = vec2((tc.x)*0.25,(tc.y+1.0)*0.5);
		vec2 uv2 = vec2((tc.x+1.0)*0.25,(tc.y+1.0)*0.5);
		vec2 uv3 = vec2((tc.x+2.0)*0.25,(tc.y+1.0)*0.5);
		vec2 uv4 = vec2((tc.x+3.0)*0.25,(tc.y+1.0)*0.5);
		vec2 uv5 = vec2((tc.x)*0.25,(tc.y)*0.5);
		vec2 uv6 = vec2((tc.x+1.0)*0.25,(tc.y)*0.5);

		vec4 brow_mask1 = texture2D(s_texNormalWrinkleMask, uv1);
		vec4 brow_mask2 = texture2D(s_texNormalWrinkleMask, uv2);
		vec4 cheek_mask = texture2D(s_texNormalWrinkleMask, uv3);
		vec4 nose_mask = texture2D(s_texNormalWrinkleMask, uv4);
		vec4 mouth_mask1 = texture2D(s_texNormalWrinkleMask, uv5);
		vec4 mouth_mask2 = texture2D(s_texNormalWrinkleMask, uv6);

		//expression_albedo
		AlbedoMap = expression_albedo(AlbedoMap, v_texcoord0, brow_mask1, brow_mask2, cheek_mask, nose_mask, mouth_mask1, mouth_mask2);

		//expression_spec
		SpecularPower = expression_spec(SpecularPower, v_texcoord0, brow_mask1, brow_mask2, cheek_mask, nose_mask, mouth_mask1, mouth_mask2);

		//expression_normal
		vec4 outResult = expression_normal(NormalMapDiffuse.xy,NormalMapSpecular.xy, v_texcoord0, brow_mask1, brow_mask2, cheek_mask, nose_mask, mouth_mask1, mouth_mask2);
		NormalMapDiffuse.xy = outResult.xy;
		NormalMapSpecular.xy = outResult.zw;
	}
	
	NormalMapDiffuse = normalize(NormalMapDiffuse);
	NormalMapSpecular = normalize(mix(NormalMapSpecular,DetailNormalMap1 + DetailNormalMap2,DetailTexturePower));

	vec3 LightDir0 = normalize(u_lightPos0.xyz-v_view.xyz);
	vec3 LightDir1 = normalize(u_lightPos1.xyz-v_view.xyz);
	vec3 LightDir2 = normalize(u_lightPos2.xyz-v_view.xyz);
	vec3 ViewDir  = normalize(v_view);

	vec3 DNormal  = normalize(v_normal);
	vec3 DTangent  = normalize(v_tangent);
	vec3 DBinormal  = cross(DNormal, DTangent);
	vec3 _DTangent = vec3(DTangent.x,DBinormal.x,DNormal.x);
	vec3 _DBinormal = vec3(DTangent.y,DBinormal.y,DNormal.y);
	vec3 _DNormal = vec3(DTangent.z,DBinormal.z,DNormal.z);

	vec3 SNormal  = normalize(decodeNormal(rttNormalDepth.xy));
	vec3 STangent  = normalize(v_tangent);
	vec3 SBinormal  = cross(SNormal, STangent);
	vec3 _STangent = vec3(STangent.x,SBinormal.x,SNormal.x);
	vec3 _SBinormal = vec3(STangent.y,SBinormal.y,SNormal.y);
	vec3 _SNormal = vec3(STangent.z,SBinormal.z,SNormal.z);

	vec2 cellLumi = cellular2x2x2(vec3(v_texcoord0*1000.0,0.0));
	float cellAlbedo = mix(1.0,1.0-cellLumi.x,SSSPowerMap.y);
	float cellSpecular = mix(1.0,cellLumi.y-cellLumi.x,SSSPowerMap.z);

	vec3 NormalDiffuse;
	NormalDiffuse.x = dot(NormalMapDiffuse,_DTangent);
	NormalDiffuse.y = dot(NormalMapDiffuse,_DBinormal);
	NormalDiffuse.z = dot(NormalMapDiffuse,_DNormal);

	vec3 NormalSpecular;
	NormalSpecular.x = dot(NormalMapSpecular,_STangent);
	NormalSpecular.y = dot(NormalMapSpecular,_SBinormal);
	NormalSpecular.z = dot(NormalMapSpecular,_SNormal);

	vec3 HDir  = normalize(-ViewDir + LightDir0);
	vec3 HDir1  = normalize(-ViewDir + LightDir1);
	vec3 HDir2  = normalize(-ViewDir + LightDir2);
	float NdotV = clamp(dot(NormalDiffuse, -ViewDir), 0.01, 0.99);
	float NS1dotV = clamp(dot(NormalSpecular, -ViewDir), 0.01, 0.99);
	
	float NdotLDiffuse = clamp(dot(NormalDiffuse, LightDir0), 0.01, 0.99);
	float NdotLSpecular = clamp(dot(NormalSpecular, LightDir0), 0.01, 0.99);
	
	float NdotLDiffuse1 = clamp(dot(NormalDiffuse, LightDir1), 0.01, 0.99);
	float NdotLSpecular1 = clamp(dot(NormalSpecular, LightDir1), 0.01, 0.99);
	
	float NdotLDiffuse2 = clamp(dot(NormalDiffuse, LightDir2), 0.01, 0.99);
	float NdotLSpecular2 = clamp(dot(NormalSpecular, LightDir2), 0.01, 0.99);
	
	float NdotH = clamp(dot(NormalDiffuse, HDir), 0.01, 0.99);
	float NdotH1 = clamp(dot(NormalDiffuse, HDir1), 0.01, 0.99);
	float NdotH2= clamp(dot(NormalDiffuse, HDir2), 0.01, 0.99);

    vec3 Albedo = toLinear(AlbedoMap) * mix(vec3(1.0,0.8,0.8),vec3(1.0,1.0,1.0),DetailCave1);
	float Fresnel = calEnvFresnel(NS1dotV,Gloss,F0);
	SpecularPower = Fresnel * SpecularPower;

	float ShadowMapBias = 0.000025;// * mix(5.0, 0.5, NdotLDiffuse);
	float Visibility = PCF(s_shadowMap, mul(u_lightMtx0,v_worldpos), ShadowMapBias, vec2(1.0/256.0));
	
	//Direct Light
	vec3 DirectLightColor = u_lightColor0.xyz * vec3(u_lightColor0.w);
	vec3 Lambert = calcLambert(Albedo, NdotLDiffuse) * vec3(max(0.0,1.0 - SpecularPower*(Specular1Power+Specular2Power)));
	vec3 DirectDiffuse = DirectLightColor * Lambert * vec3(Visibility);
	float areaPower = 0.2;
	vec3 DirectSpecular1 = min(vec3(1.0), calcBlinn(Specular1Power, NdotH, pow(NdotLSpecular,areaPower), Specular1Gloss*Gloss*256.0));
	vec3 DirectSpecular2 = min(vec3(1.0), calcBlinn(min(1.0,Specular2Power+Wet), NdotH, pow(NdotLDiffuse,areaPower), Specular2Gloss*Gloss*256.0));
	vec3 DirectSpecular  = DirectLightColor * vec3(SpecularPower) * (DirectSpecular1+DirectSpecular2*6.28);

	DirectLightColor = u_lightColor1.xyz * vec3(u_lightColor1.w);
	Lambert = calcLambert(Albedo, NdotLDiffuse1) * vec3(max(0.0,1.0 - SpecularPower*(Specular1Power+Specular2Power)));
	DirectDiffuse += DirectLightColor * Lambert;

	DirectLightColor = u_lightColor2.xyz * vec3(u_lightColor2.w);
	Lambert = calcLambert(Albedo, NdotLDiffuse2) * vec3(max(0.0,1.0 - SpecularPower*(Specular1Power+Specular2Power)));
	DirectDiffuse += DirectLightColor * Lambert;

	//Indirect Light
	float MipLevel1 = mix(5.0,0.0,Specular1Gloss*Gloss);
	float MipLevel2 = mix(5.0,0.0,Specular2Gloss*Gloss);
	vec3 RefDirection = reflect(ViewDir,mix(NormalSpecular,decodeNormal(rttNormalDepth.xy),min(1.0,Wet*2.0)));
	vec4 RadianceDirection = mul(u_iblMtx,vec4(RefDirection,0));
	vec3 Radiance1    = (textureCubeLod(s_rttCube, RadianceDirection.xyz, MipLevel1).xyz);
	vec3 Radiance2    = (textureCubeLod(s_rttCube, RadianceDirection.xyz, MipLevel2).xyz);
	vec3 RadianceWet  = (textureCubeLod(s_rttCube, RadianceDirection.xyz, 2.0).xyz);
	vec3 Radiance	  = mix(Radiance1 * Specular1Power * DetailCave1, Radiance2 * Specular2Power * DetailCave2, SpecularMix);
	Radiance += RadianceWet*Wet;

	vec3 Irradiance  = vec3(0.0,0.0,0.0);

	//ScreenSpaceAO & ScreenSpaceGI
	{
		float ao = 32.0;
		float viewDist = length(v_view)/100.0;
		float depthScale = pow(max(0.0,1.0-viewDist*10.0),4.0);
		float dist = 40.0*depthScale;
		float depthOffset = dist*500.0;
		for(int j=1;j<=2;j++)
        {
			for(int i=0;i<16;i++)
			{
				vec2 samplePosBias = (Poisson2D_16[i]+vec2(v_normal.x,-v_normal.y)*0.1)*dist/u_screenSize.xy;
				float sampleDist = length(samplePosBias);
				vec2 newSamplePos = samplePosBias + ScreenPos;
				if(newSamplePos.x>0.0 && newSamplePos.x<1.0 && newSamplePos.y>0.0 && newSamplePos.y<1.0)
				{
					vec4 _rttNormalDepth = texture2D(s_rttNormalDepth,newSamplePos);
					if(_rttNormalDepth.w<1.0)
					{
						float depthBias = min(1.0,max(0.0,viewDist-_rttNormalDepth.w)*depthOffset);
						ao -= depthBias * (1.0-sampleDist/dist);
					}
				}
			}
		}
		AO *= pow(ao/32.0,AOPower*mix(5.0,1.0,AOMap.y));
	}

	//Fur
	vec3 FurEffect = vec3(0.0);
	{
		FurEffect = vec3(pow(1.0-NdotV,EdgeGloss*10.0)*EdgePower*AOMap.z*HightLightMap.x);
	}

	vec4 IrrDirection = mul(u_iblMtx,vec4(NormalDiffuse,0));
	Irradiance += (textureCube(s_rttCubeIrr, IrrDirection.xyz).xyz);

	Albedo = Albedo * mix(vec3(1.0,0.8,0.5),vec3(1.0,1.0,1.0),cellAlbedo);
	
	SpecularPower = SpecularPower * AO * cellSpecular;

	vec3 EnvDiffuse  = ((Albedo * mix(vec3(1.0,0.7,0.7),vec3(1.0,1.0,1.0),SSSPowerMap.xxx) + FurEffect) * Irradiance) * vec3(1.0-SpecularPower);
	vec3 EnvSpecular = Radiance * vec3(SpecularPower);
	
	gl_FragData[0].xyz = (DirectDiffuse + EnvDiffuse * GIPower) * vec3(AO);
	gl_FragData[0].w = max(0.01,1.0-(SSSPowerMap.x*SSSPower));

	gl_FragData[1].xyz = (DirectSpecular + EnvSpecular * GIPower);
	gl_FragData[1].w = 1.0;
	*/
}