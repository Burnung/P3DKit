#pragma once
#include "common/P3D_Utils.h"
#include "common/mathVector.hpp"
#include "shaderUniform.hpp"
#include "common/RenderCommon.h"

BEGIN_P3D_NS

class BaseLight :public P3DObject {
public:
	BaseLight() = default;
	~BaseLight() = default;
	virtual  std::vector<pShaderUniform> apply(int litID) {
		return {};
	}

};

class PointLight :public  BaseLight {
public:
	PointLight(const Vec3f& pos, const Vec3f& litColor) :litPos(pos), litColor(litColor) {};
	~PointLight() = default;

	virtual std::vector<pShaderUniform> apply(int litId) {
		char tPosStr[256];
		char tColorStr[256];
		sprintf(tPosStr, SHADER_KEY_WORDS::POINT_LIT_POS_STRING.c_str(), litId);
		sprintf(tColorStr, SHADER_KEY_WORDS::POINT_LIT_COLOR_STRING.c_str(), litId);
		auto tUniform1 = std::make_shared<ShaderUniform>(tPosStr, litPos);
		auto tUniform2 = std::make_shared<ShaderUniform>(tColorStr, litColor);
		return { tUniform1,tUniform2 };
	}

public:
	Vec3f litPos;
	Vec3f litColor;
};
typedef std::shared_ptr<BaseLight> pLight;
END_P3D_NS