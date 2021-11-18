#include "sceneManager.hpp"
#include "common/json_inc.h"
#include "../loader/P3DLoader.h"
#include "../P3dEngine.h"
#include "renderNodeManager.hpp"
#include "../P3DLight.hpp"
BEGIN_P3D_NS

Mat4f json2Mat(const P3djson& obj) {
	std::vector<float> tScale = JsonUtil::get(obj, "scale", { 1.0,1.0,1.0 });
	std::vector<float> trans = JsonUtil::get(obj, "translate", { 0.0,0.0,0.0 });
	std::vector<float> tRotate = JsonUtil::get(obj, "rotate", { 0.0,0.0,0.0 });
	auto retMat = Mat4f::getScale({ tScale[0],tScale[1],tScale[2] });
	retMat = Mat4f::getRotation({ tRotate[0],tRotate[1],tRotate[2] })*retMat;
	retMat = Mat4f::getTranslate({ trans[0],trans[1],trans[2] })*retMat;
	return retMat;
}
void paraseSceneNodes(const P3djson& obj, const std::string& tDir) {
	auto tScene = P3DEngine::instance().getSceneManager();
	for (const auto& tObj : obj) {
		std::string tName = tObj["name"];
		std::string tType = JsonUtil::get(tObj, "type", "comp");
		auto fPath = Utils::joinPath(tDir, tObj["path"]);
		auto tMat = Mat4f::getIdentity();
		if (tObj.count("transform"))
			tMat = json2Mat(tObj["transform"]);
		//tMat.print();
		if (tType == "comp") {
			auto tComps = P3DLoader::instance().loadComps(fPath);
			for (auto tComp : tComps) {
				tScene->addComp(tName, tComp);
				tScene->setCompModelMat(tName, tMat);
			}
		}
		else if (tType == "skinMesh") {
			auto tMesh = P3DLoader::instance().loadFbxMesh(fPath,true);
			tScene->addSkinMesh(tName,tMesh);
			tScene->setSkinModelMat(tName, tMat);
		}
		if (tObj.count("material")) {
			auto tMat = tScene->getMaterial(tObj["material"]);
			auto tdata = tScene->getRenderable(tName);
			tdata->setMaterial(tMat);
		}
	}
}

void paraseSceneLights(const P3djson& litObjs) {
	auto tScene = P3DEngine::instance().getSceneManager();
	for (const auto& tObj : litObjs.items()) {
		std::string litName = tObj.key();
	//	std::cout << tObj.key() << std::endl;
		const auto& tv = tObj.value();
		if (tv["type"] == "point") {
			std::vector<float> tPos = tv["pos"];
			std::vector<float> tColor = tv["color"];
			auto tLit = std::make_shared<PointLight>(Vec3f(tPos[0],tPos[1],tPos[2]) , Vec3f(tColor[0],tColor[1],tColor[2]));
			tScene->addLight(litName, tLit);
		}
	}

}

void paraseMaterials(const P3djson& objs, cstring tDir) {
	auto tScene = P3DEngine::instance().getSceneManager();
	for (const auto& tobj : objs.items()) {
		std::string tName = tobj.key();
		const auto& tv = tobj.value();
		if (tv["type"] == "pbr"){
			auto tPbr = std::make_shared<P3DMaterialPbr>();
			if (tv.count("albedo")) {
				std::vector<float> albedoV = tv["albedo"];
				tPbr->albedo = Value(Vec3f(albedoV[0],albedoV[1],albedoV[2]));
			}
			if (tv.count("matallic")) {
				float mv = tv["matallic"];
				tPbr->matallic = Value(mv);
			}
			if (tv.count("roughness")) {
				float rv = tv["roughness"];
				tPbr->roughness = Value(rv);
			}
			if (tv.count("ao")) {
				float av = tv["ao"];
				tPbr->ao = Value(av);
			}
			tScene->addMaterial(tName, tPbr);
			//tPbr->apply();
		}
	}
}

void SceneManager::paraseFromFile(const std::string& fPath) {
	auto jsonStr = Utils::readFileString(fPath);
	auto rootObj = P3djson::parse(jsonStr);
	auto tDir = Utils::getFileDir(fPath);
	// parase material
	if (rootObj.count("materials")) {
		paraseMaterials(rootObj["materials"], tDir);
	}
	//parase rendernodos
	if (rootObj.count("renderCfg")) {
		auto cfgPath = Utils::joinPath(tDir,rootObj["renderCfg"]);
		P3DEngine::instance().getRenderNodeManager()->paraseFromeFile(cfgPath);
	}
	//parase node
	if (rootObj.count("nodes")) {
		 paraseSceneNodes(rootObj["nodes"], tDir);
	}
	if (rootObj.count("lights")) {
		paraseSceneLights(rootObj["lights"]);
	}
	
}


END_P3D_NS