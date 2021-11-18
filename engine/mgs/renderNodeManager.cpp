#include "renderNodeManager.hpp"
#include "common/json_inc.h"
#include "../loader/P3DLoader.h"
BEGIN_P3D_NS
using namespace std;

pRenderNode getRenderNodeBase(const P3djson& obj, const std::string& tDir) {
	//	std::string vsPath = JsonUtil::get()
	P3djson shaderObj = obj["shader"];
	std::string sName = JsonUtil::get<std::string>(shaderObj, "name","");
	sName = JsonUtil::get(obj, "type", "custom") + "_" + sName;
	std::string vsPath = JsonUtil::get(shaderObj, "vs_path", "");
	std::string fsPath = JsonUtil::get(shaderObj, "fs_path", "");
	std::string fboName = obj["fbo_name"];
	RENDERNODE_TYPE tType = obj["type"];
	std::string nodeName = obj["name"];
	float priority = obj["priority"];

	auto  fboRatio = JsonUtil::get(obj, "fbo_ratio", {1.0,1.0});

	if (!Utils::getFileDir(vsPath).empty())
		vsPath = Utils::joinPath(tDir, vsPath);
	if (!Utils::getFileDir(fsPath).empty())
		fsPath = Utils::joinPath(tDir, fsPath);

	auto tScenen = P3DEngine::instance().getSceneManager();
	auto trenderMng = P3DEngine::instance().getRenderNodeManager();
	pRenderNode ret = nullptr;
	if (tScenen->getShader(sName)) {
		ret = trenderMng->createRenderNode(tType, JsonUtil::ensureGet<string>(obj, "name"), sName, fboName, fboRatio);
	}
	else
		ret = trenderMng->createRenderNode(tType, nodeName, vsPath, fsPath, fboName, fboRatio);
	if (ret == nullptr)
		return nullptr;

	trenderMng->setRenderSort(int(priority), nodeName);
	//renderables
	if (obj.count("renderables")) {
		for (std::string renderableName : obj["renderables"])
			ret->addRenderable(renderableName);
	}
	//camera
	if (obj.count("camera")) {
		P3djson camObj = obj["camera"];
		std::string camName = camObj["name"];
		auto tCam = tScenen->getCamera(camName);
		if (tCam == nullptr) {
			std::vector<float> viewParam;
			//= camObj["view_params"];
			viewParam = JsonUtil::get(camObj, "view_params", std::vector<float>());
			auto camera = std::make_shared<Camera>();
			if (!viewParam.empty()) {
				camera->setViewParams({ viewParam[0],viewParam[1],viewParam[2] }, { viewParam[3],viewParam[4],viewParam[5] }, \
				{viewParam[6], viewParam[7], viewParam[8]});
			}
			else
				camera->setViewMat(Mat4f::getIdentity());
			float fov = Utils::toDegree(1.0f);
			fov	= JsonUtil::get(camObj,"fov",fov);
			camera->setProjParams(fov, P3DEngine::instance().getWidth()*fboRatio[0] / P3DEngine::instance().getHeight()*fboRatio[1], 0.1f, 1000.0f);
			tScenen->addCamera(camObj["name"], camera);
		}
		ret->setCamera(camObj["name"]);
	}
	//renderstate setting 
	if (obj.count("render_state")) {
		const auto& renderStateObj = obj["render_state"];
		for (auto& it=renderStateObj.begin();it!=renderStateObj.end();++it) {
			std::string tName = it.key();
			std::vector<std::string> tParms = it.value();
			ret->addRenderStateSetting(tName,tParms);
		}
	}
	return ret;
	
}
template<typename T>
void getUniformInvers(T& v) {
	for (int i = 0; i < v.data.size(); ++i) {
		if (abs(v[i]) > 1e-5)
			v[i] = 1.0 / v[i];
	}
}

pShaderUniform json2Uniform(const std::string tName,const P3djson& obj) {
	std::string type = obj["Type"][0];
	pShaderUniform ret = nullptr;
	std::string vName = JsonUtil::get(obj, "valueName", "");
	if (type == "Vec4") {
		std::vector<float> tData(4, 0.0f);
		tData = JsonUtil::get(obj,"Data",tData);
		Vec4f tVec(tData);
		//(tData);
		float invers = JsonUtil::get(obj, "invers", 0.0);
		if (invers > 0.5) {
			getUniformInvers(tVec);
		}
		ret = std::make_shared<ShaderUniform>(tName, tVec);
	}
	else if (type == "Float") {
		float v = JsonUtil::get(obj, "Data", 0.0f);
		float invers = JsonUtil::get(obj, "invers", 0.0);
		if (invers > 0.5)
			v = 1.0 / (v+1e-5);
		ret = std::make_shared<ShaderUniform>(tName, v);
	}
	if (ret && !vName.empty()) {
		auto tScene = P3DEngine::instance().getSceneManager();
		if (!tScene->getValue(vName, ret->dataValue)) {
			tScene->addValue(vName, ret->dataValue);
		}
	}
	
	return ret;
}
std::list<pShaderUniform> json2Uniforms(const P3djson& obj) {
	std::list<pShaderUniform> ret;
	for (auto tObj=obj.begin(); tObj != obj.end();++tObj) {
		std::string tName = tObj.key();
		auto tUniform = json2Uniform(tName, tObj.value());
		if (tUniform)
			ret.push_back(tUniform);
	}
	return ret;
}
std::list<pShaderUniform> json2UniformsTex(const P3djson& obj,const std::string& tDir) {
	auto tScene = P3DEngine::instance().getSceneManager();
	std::list<pShaderUniform> ret;
	for (const P3djson& tobj : obj) {
		std::string tName = tobj["name"];
		auto pTex = tScene->getTex(tName);
		std::string locName = tobj["sampler"];
		// Fbo tex 实时更新
		if (tName._Starts_with("$")) {
			ret.push_back(std::make_shared<ShaderUniform>(locName, tName));
		}
		else if (pTex)//如果存在 直接用
			ret.push_back(std::make_shared<ShaderUniform>(locName, pTex));
		else {// 不存在 从文件创建
			if (tobj.count("path")) {
				auto imgPath = Utils::joinPath(tDir, tobj["path"]);
				auto cpuImg = P3DLoader::instance().loadCpuImg(imgPath);
				auto tTex = P3DEngine::instance().createTexture(cpuImg);
				tScene->addTex(tName, tTex);
				ret.push_back(std::make_shared<ShaderUniform>(locName, tTex));
			}
			else
				ret.push_back(std::make_shared<ShaderUniform>(locName, tName));
		}
	}
	return ret;
}

void paraseUniforms(pRenderNode renderNode, const P3djson& obj, const std::string& tDir) {
	//
	if (obj.count("uniforms")) {
		auto unifoms = json2Uniforms(obj["uniforms"]);
		for (auto t : unifoms)
			renderNode->addUniform(t);
	}
	if (obj.count("tex_uniforms")) {
		auto unifoms = json2UniformsTex(obj["tex_uniforms"],tDir);
		for (const auto& t : unifoms) {
			renderNode->addUniform(t);
		}
	}
	//renderable uniform
	if (obj.count("renderbale_uniforms")) {
		for (const P3djson tObj : obj["renderbale_uniforms"]) {
			std::string tName = tObj["model_name"];
			if (tObj.count("uniforms")) {
				auto uniforms = json2Uniforms(tObj["uniforms"]);
				for (auto t : uniforms)
					renderNode->addRenderableUniform(tName, t);
			}
			if (tObj.count("texs")) {
				auto uniforms = json2UniformsTex(tObj["texs"],tDir);
				for (const auto& t : uniforms) {
					renderNode->addRenderableUniform(tName, t);
				}
			}
		}
	}
}
void RenderNodeManager::paraseFromeFile(const std::string& fPath) {
	auto jsonStr = Utils::readFileString(fPath);
	P3djson rootJson = P3djson::parse(jsonStr);
	auto relativeDir = Utils::getAbsPath(Utils::getFileDir(fPath));
//	auto tDir = Utils::getFileDir("");
	for (const auto& obj : rootJson) {
		if(JsonUtil::get(obj,"is_use",1.0) < 0.5)
			continue;
		auto renderNode = getRenderNodeBase(obj,relativeDir);
		if (renderNode == nullptr)
			return;
		paraseUniforms(renderNode,obj,relativeDir);
		int a =0 ;
	}
}
END_P3D_NS