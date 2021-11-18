#include "vrayRenderController.h" 
#include "common/json_inc.h"
#include "P3DLoader.h"

BEGIN_P3D_NS

void VrayRenderController::initFromConfig(const std::string& cfgPath) {
	auto fileStr = Utils::readFileString(cfgPath);
	auto rootObj = p3djson::parse(fileStr);
	std::string vrayPath = rootObj["vrayPath"];
	std::string fbxPath = rootObj["fbxScene"];
	
	auto jsonNodeMap = rootObj["nodeMaps"];
	for (auto&it : jsonNodeMap) {
		nodeMap[it["nodeName"]] = it["compName"];
	}

	if (mRender == nullptr) {
		mRender = std::make_shared<VrayRender>();
		mRender->init(720, 1280, false);
	}
	
	mRender->loadScene(vrayPath);

	//if(mMesh == nullptr)
	mMesh = P3DLoader::instance().loadFbxMesh(fbxPath, false);
}

void VrayRenderController::setAnimation(const std::unordered_map<std::string,std::vector<float32> >&bonePos, const std::vector<float32>& bs) {
	std::cout << "inter set Animation" << std::endl;
	for (const auto& it : bonePos) {
		std::cout << it.first << std::endl;
	}
	mMesh->updateBs(bs);
	std::cout << "update bs ok" << std::endl;
	mMesh->updateByBones(bonePos);
	std::cout << "update bone ok" << std::endl;
	for (const auto& it : nodeMap) {
		std::cout << it.first << " " << it.second << std::endl;
		auto tComp = mMesh->getComp(it.second);
		if (tComp == nullptr) {
			std::cout << "no comp" << std::endl;
		}
		mRender->addMeshNode(it.first, mMesh->getComp(it.second));
	}
}
void VrayRenderController::renderImg(const std::string& imgPath,float32 gamma) {
	mRender->renderImg(imgPath, -1,gamma);
}

PNdArrayF VrayRenderController::renderRaw() {
	return mRender->renderRaw(-1);
}


std::vector<float32> VrayRenderController::getBoneBaseEuler(std::string& boneName) {
	return mMesh->getBoneBaseEuler(boneName);
}
END_P3D_NS