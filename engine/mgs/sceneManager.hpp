#pragma once
#include "../renderFrame.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../userGeoGpu.h"
#include "../P3dEngine.h"
#include "geo/comp.h"
#include "geo/P3DSkinMesh.h"
#include "../P3DLight.hpp"
#include "../P3DMaterial.hpp"
BEGIN_P3D_NS

class SceneManager :public P3DObject {
private:
	pRenderable getNormalQuad() {
		auto userGpu = P3DEngine::instance().createUserGpuGeo();
		std::vector<Vec3f> vps = { {-1.0f,-1.0f,0.0f},{1.0f,-1.0f,0.0f},{1.0f,1.0f,0.0f},{-1.0f,1.0f,0.0f} };
		std::vector<uint8> tl = { 0,1,2,2,3,0 };
		auto vpsBlock = std::make_shared<DataBlock>(sizeof(Vec3f) * vps.size(), DType::Float32);
		vpsBlock->setFromMem((uint8*)vps.data(), vpsBlock->getSize());
		auto tlBlock = std::make_shared<DataBlock>(tl.size() * sizeof(uint8), DType::UInt8);
		tlBlock->setFromMem((uint8*)tl.data(), tlBlock->getSize());
		userGpu->addVertexData(vpsBlock, 0, 3);
		userGpu->addElementBuffer(tlBlock);
		userGpu->commitPure();
		return userGpu;
		//quadVbo->init(vpsBlock, tlBlock, nullptr, nullptr);
	}
public:
	SceneManager() {
		//default render frame
		addRenderFrame("$screen", P3DEngine::instance().getDefaultRenderFrame());
		//default quad
		renderAbles["$quad"] = getNormalQuad();
		//default camera
		auto camera = std::make_shared<Camera>();
		camera->setCameraType(Camera::CAMERA_TYPE_ORTH);
		camera->setViewMat(Mat4f::getIdentity());
		camera->setOrthMat(Mat4f::getIdentity());
		cameras["$normCam"] = camera;
	}
	virtual ~SceneManager() = default;
public:
	//for camera
	bool addCamera(const std::string& tName, pCamera cam) {
		cameras[tName] = cam;
		return true;
	}
	pCamera getCamera(const std::string& tName) {
		return cameras[tName];
	}
	// for comp
	bool addComp(const std::string& tName, pComp tComp) {
		comps[tName] = tComp;
		tComp->name = tName;
		auto tUserGpu = P3DEngine::instance().createUserGpuGeo();
		tUserGpu->fromComp(tComp);
		tUserGpu->commitPure();
		renderAbles[tName] = tUserGpu; 
	
		return true;
	}
	bool getValue(cstring vName,Value& v) {
		if (allValues.count(vName)) {
			v = allValues[vName];
			return true;
		}
		return false;
	}
	void addValue(cstring vName, const Value& v) {
		allValues[vName] = v;
	}
	pComp getComp(const std::string& tName) {
		return comps[tName];
	}
	pRenderable getRenderable(const std::string& tName) {
		return renderAbles[tName];
	}
	bool setCompModelMat(const std::string&tName,const Mat4f& modelMat) {
		auto pComp = getComp(tName);
		if(pComp)
			pComp->setModelMat(modelMat);
		auto tRen = getRenderable(tName);
		if(tRen)
			tRen->setModelMat(modelMat);
		//std::cout << tName << tRen.get() << std::endl;
		return true;
	}
	bool setSkinModelMat(const std::string& tName, const Mat4f& modelMat) {
		auto tSkin = skinMeshes[tName];
		if (tSkin == nullptr)
			return false;
		for (auto tComp: tSkin->comps) {
			setCompModelMat(tComp->name, modelMat);
		}
		return true;
	}
	bool addSkinMesh(const std::string& tName, pSkinMesh pSkin) {
		pSkin->name = tName;
		skinMeshes[tName] = pSkin;
		pSkin->updateSkeletonComps();
		for (auto tComp : pSkin->comps) {
			addComp(tName+"_"+tComp->name, tComp);
		}
		return true;
	}
// shader
	bool addShader(const std::string& tName, pShader tShader) {
		shaders[tName] = tShader;
		return true;
	}
	pShader getShader(const std::string& tName) {
		return shaders[tName];
	}

	bool setRenderableShader(const std::string& tName, const std::string& sName) {
		if (renderAbles.find(tName) == renderAbles.end() || shaders.find(sName) == shaders.end())
			return false;
		renderAbles[tName]->setShader(shaders[sName]);
		return true;
	}
	const std::unordered_map<std::string, pShader>& getAllShaders() {
		return shaders;
	}
	// render framebuff
	bool addRenderFrame(const std::string& tName, pRenderFrame tFrame) {
		renderFrames[tName] = tFrame;
		if (tName == "$screen")
			return true;
		if (tFrame == nullptr)
			return true;
		tFrame->name = tName;
		//add tex
		for (int i = 0; i < tFrame->getRenderCount(); ++i) {
			char ID[20];
			itoa(i, ID,10);
			auto texName = "$"+tName + "_"+ID;
			texs[texName] = tFrame->getRenderTexture(i);
		}
		return true;
		//texs[tName+"depth"] = tFrame->
	}
	pRenderFrame getRenderFrame(const std::string& tName) {
		if (renderFrames.find(tName) == renderFrames.end())
			return nullptr;
		return renderFrames[tName];
	}
	const std::unordered_map<std::string, pRenderFrame>& getAllRenderFrame()const {
		return renderFrames;
	}

	//for tex
	pTexture getTex(const std::string&texName) {
		if (texs.find(texName) == texs.end())
			return nullptr;
		return texs[texName];
	}
	bool addTex(const std::string&texName, pTexture tex) {
		texs[texName] = tex;
		return true;
	}
	void addMaterial(const std::string&matName,pMaterial tm){
		materials[matName] = tm;
	}
	pMaterial getMaterial(cstring tName) {
		return materials[tName];
	}
	void addLight(cstring litName, pLight tLit) {
		lights[litName] = tLit;
	}
	std::unordered_map<std::string, pLight> getAllLights() {
		return lights;
	}
	pLight getLight(cstring litName) {
		return lights[litName];
	}
	//scene update
	void setBsData(std::string& skinName, const std::vector<float>& vs) {
		bsInfos[skinName] = vs;
	}
	void setBoneData(const std::string& skinName, const std::unordered_map<std::string, std::vector<float>> boneInfo) {
		boneInfos[skinName] = boneInfo;
	}
	void updateSkins(float deltaTime) {
		for (auto& tk : bsInfos) {
			skinMeshes[tk.first]->updateBs(tk.second);
		}
		for (auto& tk : boneInfos) {
			skinMeshes[tk.first]->updateByBones(tk.second);
		}
		for (auto &tk : skinMeshes)
			tk.second->updateSkeletonComps();
	}
	void updateRenderables() {
		for (auto tk : comps) {
			updateRenderableByComp(renderAbles[tk.first], comps[tk.first]);
		}
	}

	void paraseFromFile(const std::string& fPath);
private:
	void updateRenderableByComp(pRenderable ren, pComp tComp) {
		//
		if (ren == nullptr || tComp == nullptr)
			return;
		using namespace SHADER_KEY_WORDS;
		ren->update(tComp->cache_vps, IN_VERT_POS);
		ren->update(tComp->cache_vns, IN_VERT_NORMAL);
		//ren->u)
	}
private:
	//data info
	std::unordered_map<std::string, pTexture> texs;
	std::unordered_map<std::string, pComp> comps;
	std::unordered_map<std::string, pRenderable> renderAbles;
	std::unordered_map<std::string, pShader> shaders;
	std::unordered_map<std::string, pRenderFrame> renderFrames;
	std::unordered_map<std::string, pSkinMesh> skinMeshes;
	std::unordered_map<std::string, pCamera> cameras;
	std::unordered_map<std::string, pLight> lights;
	std::unordered_map<std::string, pMaterial> materials;
	//for update
	std::unordered_map<std::string, std::vector<float>> bsInfos;
	std::unordered_map<std::string, std::unordered_map<std::string,std::vector<float>>> boneInfos;
	//´æ´¢ËùÓÐvalue
	std::unordered_map<std::string, Value> allValues;
};

typedef std::shared_ptr<SceneManager> pSceneManager;


END_P3D_NS