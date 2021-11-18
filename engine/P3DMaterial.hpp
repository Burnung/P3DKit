#pragma once
#include "common/P3D_Utils.h"
#include "common/mathVector.hpp"
#include "shaderUniform.hpp"
#include "common/RenderCommon.h"
#include "shaderUniform.hpp"
BEGIN_P3D_NS
class P3DMaterialBase :public P3DObject {
public:
	P3DMaterialBase() = default;
	~P3DMaterialBase() = default;
	virtual std::vector<pShaderUniform> apply() { return {}; }
};

class P3DMaterialPbr :public P3DMaterialBase {

public:

	virtual std::vector<pShaderUniform> apply() {
		std::vector<pShaderUniform> ret;
		using namespace SHADER_KEY_WORDS;
		auto albedoUni = std::make_shared < ShaderUniform>();

		if (albedo.mType == Value::VALUE_FLOAT3) {
			albedoUni->dataType = ShaderUniform::UniformType::TYPE_VEC3;
			albedoUni->locName = PBR_ALBEDO;
			albedoUni->dataValue = albedo;
		}
		else {
			albedoUni->dataType = ShaderUniform::UniformType::TYPE_TEX;
			albedoUni->locName = PBR_ALBEDO_TEX;
			albedoUni->dataValue = albedo;
		}
		auto matalicUni = std::make_shared<ShaderUniform>();
		if (matallic.mType == Value::VALUE_FLOAT) {
			matalicUni->dataType = ShaderUniform::UniformType::TYPE_FLOAT;
			matalicUni->locName = PBR_MATALLIC;
			matalicUni->dataValue = matallic;
		}
		else {
			matalicUni->dataType = ShaderUniform::UniformType::TYPE_TEX;
			matalicUni->locName = PBR_MATALLIC_TEX;
			matalicUni->dataValue = matallic;
		}

		auto roughnessUni = std::make_shared<ShaderUniform>();
		if (roughness.mType == Value::VALUE_FLOAT) {
			roughnessUni->dataType = ShaderUniform::UniformType::TYPE_FLOAT;
			roughnessUni->locName = PBR_ROUGHNESS;
			roughnessUni->dataValue = roughness;
		}
		else {
			roughnessUni->dataType = ShaderUniform::UniformType::TYPE_TEX;
			roughnessUni->locName = PBR_ROUGHNESS_TEX;
			roughnessUni->dataValue = roughness;
		}

		auto aoUni = std::make_shared<ShaderUniform>();
		if (ao.mType == Value::VALUE_FLOAT) {
			aoUni->dataType = ShaderUniform::UniformType::TYPE_FLOAT;
			aoUni->locName = PBR_AO;
			aoUni->dataValue = ao;
		}
		else {
			aoUni->dataType = ShaderUniform::UniformType::TYPE_TEX;
			aoUni->locName = PBR_AO_TEX;
			aoUni->dataValue = ao;
		}
		return { albedoUni,matalicUni,roughnessUni,aoUni };
	}

	Value albedo;
	Value matallic;
	Value roughness;
	Value ao;

};
typedef std::shared_ptr<P3DMaterialBase> pMaterial;

END_P3D_NS
