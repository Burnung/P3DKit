
#include<iostream>
#include <string>
#include "engine/P3dEngine.h"
#include <opencv2/opencv.hpp>
#include "geo/meshDataIO.h"
#include "engine/Camera.h"
#include "common/NdArray.h"
#include <GL/glew.h>
#include <random>

#ifdef _WIN32
#	define WINDOWS_LEAN_AND_MEAN
#	define NOMINMAX
#endif

#include "engine/OPenGL/OGLVertexBuffer.h"
#include "engine/OPenGL/OGLUserMeshGpu.h"
#include "engine/renderTechnique.h"
#include "engine/extern/mediaCenter.h"
#include "engine/extern/cameraControllerOrth.h"
#include "engine/extern/cameraControllerPro.h"
#include "physics/phyCommon.h"
#include "physics/PhyEngine.h"
#include "physics/phyJointChain.h"
#include "engine/extern/listener.h"
#include "common/json_inc.h"

using namespace P3D;
using namespace std;


class PhyController :public BaseListener {
public:
	PhyController() = default;
	~PhyController() = default;
public:
	
	virtual void processKeyboard(const std::vector<KEY_STATES>&keyStates) {
		if (!phyScene)
			return;
		if (keyStates[KEY_NAME_J] == KEY_STATE_PRESS) {
			phyScene->addForce({ -5,0,0 }, P3D::PhyForceType::PHY_FORCETYPE_ACCELERATION);
		}
		if (keyStates[KEY_NAME_L] == KEY_STATE_PRESS)
			phyScene->addForce({ 5,0,0 }, P3D::PhyForceType::PHY_FORCETYPE_ACCELERATION);

		if (keyStates[KEY_NAME_I] == KEY_STATE_PRESS)
			phyScene->addForce({ 0.0,0.0,-3.0 }, P3D::PhyForceType::PHY_FORCETYPE_ACCELERATION);
		if (keyStates[KEY_NAME_K] == KEY_STATE_PRESS)
			phyScene->addForce({ 0.0,0.0,5.0 }, P3D::PhyForceType::PHY_FORCETYPE_ACCELERATION);
		if (keyStates[KEY_NAME_U] == KEY_STATE_PRESS) {
			auto tTime = Utils::getCurrentTime();
			if (tTime - lastUpdate > 0.3) {
				update = true;
				lastUpdate = tTime;
			}
		}
		if (keyStates[KEY_NAME_SPACE] == KEY_STATE_PRESS) {
			saveImg = true;
		}

	}
	virtual void processMouse(const std::vector<MOUSE_BUTTON_STATES>&keyStates, Vec2f mousePos) {
	if(sp==nullptr)
		return;
	float32 speed = 0.01;
	if (keyStates[MOUSE_BUTTON_RIGHT] == MOUSE_STATE_PRESS) {
		if (mouseStateLast == MOUSE_STATE_PRESS) {
			auto posDelta = (mousePos - mousePositionLast)*speed;
			auto transMat = Mat4f::getTranslate({ posDelta.x,-posDelta.y,0.0 });
			auto modeMat = sp->modelMat * transMat;
			sp->setModelMat(modeMat);
			std::cout << "now pos is " << modeMat[0].w << " " << modeMat[1].w << " " << modeMat[2].w << endl;
		 }
	}
	mouseStateLast = keyStates[MOUSE_BUTTON_RIGHT];
	mousePositionLast = mousePos;
	}
public:
	MOUSE_BUTTON_STATES mouseStateLast;
	Vec2f mousePositionLast;
	pPhyScene phyScene;
	pComp sp;
	bool update = true;
	float32 lastUpdate = 0.0;
	bool saveImg = false;
};


std::pair<pHairModel, P3D::pUserGeoGPU> loadHairData(const std::string& fDir,const std::string& jointInfoPath) {
	auto hair = std::make_shared<P3D::HairModel>();
	hair->loadFromFile(fDir + R"(\hair.data)");
	hair->loadJointInfo(jointInfoPath);
	//auto hairData = hair->getGPUData();
	auto hairData = hair->getJointGPUData();

	auto attris = NdUtils::readNpy(fDir + R"(\all_attris.npy)").asFloat();

	
	uint32 attriNum = attris.shape()[2] / 4;
	int32 lastC = attris.shape()[2] % 4;
	vector<PNdArrayF> attrisData(attriNum);

	for (uint32 i = 0; i < attriNum; ++i) {
		attrisData[i] = PNdArrayF({ attris.shape()[0],attris.shape()[1],4 });
	}
	if (lastC > 0) {
		auto tmpData = PNdArrayF({ attris.shape()[0], attris.shape()[1], lastC});
		attrisData.push_back(tmpData);
	}

	auto attrData2 = std::make_shared<P3D::DataBlock>(hairData["vps"]->getSize(),P3D::DATA_TYPE_FLOAT32);

	size_t offset = 0;
	size_t tStride = 3 * 50 * sizeof(float);

	for (int ns = 0; ns < attris.shape()[0]; ++ns) {
		for (int np = 0; np < attris.shape()[1]; ++np) {
			auto offset = 0;
			for (int i = 0; i < attrisData.size(); ++i) {
				for (int j = 0; j < attrisData[i].shape()[2]; ++j)
					attrisData[i][{ns, np, j}] = attris[{ns, np, offset++}];
			}
		}
	}
	/*
	for (auto i = 0; i < attrisData.size(); ++i) {
		for (auto j = 0; j < attrisData[i].shape()[2]; ++j)
			cout << attrisData[i][{0, 0, j}] << " ";
		cout << endl;
	}*/

	auto bigHairGpu = P3DEngine::instance().createUserGpuGeo();// std::make_shared<P3D::OGLUserMeshGPU>();
	auto posBuffer = std::make_shared<P3D::OGLVertexBuffer>();
	auto tlBuffer = std::make_shared<P3D::OGLVertexBuffer>();

	tlBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ELEMETN, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_UINT32, hairData["tl"]);
	posBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::DYNAMIC_DRAW, P3D::DATA_TYPE_FLOAT32, hairData["vps"]);
	bigHairGpu->addVertexData(posBuffer, "InPos", 3, 0);

	int nAttr = 1;

	for (auto& arr : attrisData) {
		auto dataBlock = arr.getDataBlock();
		dataBlock->setDataType(DATA_TYPE_FLOAT32);
		auto tmpAttrBuffer = std::make_shared<P3D::OGLVertexBuffer>();
		tmpAttrBuffer->init(VertexBufferType::VERTEX_BUFFER_ARRAY, VertexBufferUsage::STATIC_DRAW, DATA_TYPE_FLOAT32, dataBlock);
		auto attName = Utils::makeStr("attri%d", nAttr++);
		bigHairGpu->addVertexData(tmpAttrBuffer, attName, arr.shape()[2], 0);
	}

	bigHairGpu->addVertexData(hairData["jointId"], "jointID", 4, 0);
	bigHairGpu->addVertexData(hairData["jointWeight"], "jointWeight", 4, 0);


	bigHairGpu->addElementBuffer(tlBuffer, P3D::GEO_TYPE_LINES);

	auto shader = P3D::P3DEngine::instance().createShader();
	shader->loadFromFile(fDir + R"(\hair_mat.vert)", fDir + R"(\hair.frag)");

	bigHairGpu->setShader(shader);
	
	return { hair,bigHairGpu };
	
}

PNdArray getFbo2NdArray(pRenderFrame fbo) {
	std::vector<PNdArray> rets;
	for (int i = 0; i < fbo->getRenderCount(); ++i) {
		auto tmpCpu = fbo->getRenderTextureCpu(i, true);
		auto tNd = tmpCpu->getNdArray();
		rets.push_back(tNd);
	}
	auto ret = NdUtils::concat(rets);
	return ret;
}

void runInWindow(const RenderTechnique& technique, pWindow win) {
	auto mediaCenter = std::make_shared<MediaCenter>();
	mediaCenter->setWindow(win);
	auto cameraController = std::make_shared<CameraControllerOrth>();
	auto camera = technique.getCamera();
	cameraController->setCamera(camera);
	mediaCenter->addListener(cameraController);
	uint32 nCount = 0;
	auto sTime = Utils::getCurrentTime();
	while (!mediaCenter->stop()) {
		auto fbo = technique.render();
		win->renderTexture(fbo->getRenderTexture(0));
		mediaCenter->update();
		nCount++;
		if (nCount == 10) {
			auto eTime = Utils::getCurrentTime();
			auto renderFps = 10 / (eTime - sTime);
			auto fpsStr = Utils::makeStr("Fps:%.3f,frame cost:%.3fms", renderFps,1000.0 / renderFps);
			win->setWinTitle(fpsStr);
			nCount = 0;
			sTime = Utils::getCurrentTime();
		}
	}
}

void saveImg(const RenderTechnique& technique, const std::string& dstDir,pWindow win) {
	float theta = 0.0;
	float phi = 0.0;
	auto camera = technique.getCamera();
	int nFrame = 0;
	while (phi < PI - 0.1) {
		auto y = sin(theta);
		auto x = cos(theta) * sin(phi);
		auto z = cos(theta) * cos(phi);
		camera->setViewParams({ x,y,z }, { 0.0,0.0,0.0 }, {0.0,1.0,0.0});
		auto fbo = technique.render();
		auto filePath = dstDir + Utils::makeStr("frame_%05d.npy", nFrame++);
		auto tImg = fbo->getRenderTextureCpu(0);
		NdUtils::writeNpy(filePath, tImg->getNdArray(0));
		//win->renderTexture(fbo->getRenderTexture(0));
		phi += 0.01;
	}
	while (theta < 0.5 * PI - 0.1) {
		auto y = sin(theta);
		auto x = cos(theta) * sin(phi);
		auto z = cos(theta) * cos(phi);
		camera->setViewParams({ x,y,z }, { 0.0,0.0,0.0 }, {0.0,1.0,0.0});
		auto fbo = technique.render();
		auto filePath = dstDir + Utils::makeStr("frame_%05d.npy", nFrame++);
		auto tImg = fbo->getRenderTextureCpu(0);
		NdUtils::writeNpy(filePath, tImg->getNdArray(0));
		theta += 0.01;
	}
	auto w = 0.6;
	while (w > 0.15) {
		camera->setOrthWidth(w);
		auto fbo = technique.render();
		auto filePath = dstDir + Utils::makeStr("frame_%05d.npy",nFrame++);
		auto tImg = fbo->getRenderTextureCpu(0);
		NdUtils::writeNpy(filePath, tImg->getNdArray(0));
		w -= 0.003;
	}

}

std::vector<pPhyJointChain> getChainsFromHair(pHairModel hair,float32 raduis = 0.1) {
	std::vector<pPhyJointChain> ret;
	auto material = PhyEngine::instance().createMaterial(50.0f,50,0.6 );
	auto modelMat = Mat4f::getTranslate({ 0.0,-15.5,0.0 }) * Mat4f::getScale({ 10.0f,10.0f,10.0f });
	auto nCount = 0;
	for (auto st : hair->srcStrands) {
		pPhyJointChain tJoint = nullptr;

		if (nCount > 8)
			tJoint = PhyEngine::instance().createJointChain(PHY_JOINTTYPE_SPHERE, { 0.0,0.0,0.0 });
		else
			tJoint = PhyEngine::instance().createJointChain(PHY_JOINTTYPE_FIXED, { 0.0,0.0,0.0 });


		for (auto tp : st->pos) {
			auto rigid = PhyEngine::instance().createRigid(PHY_RIGIDTYPE_DYNAMIC, PHY_GEOTYPE_BOX, material, Mat4f::getTranslate(tp), {raduis,raduis,raduis},20.0);
			tJoint->addJoint(rigid, {0});
		}
		ret.push_back(tJoint);
	}
	return ret;
}


void upDataHairByJointGL(pShader tShader,const std::vector<pPhyJointChain>& joints) {
	std::vector<Mat4f> relativeMats;
	relativeMats.reserve(joints.size()* 5);
	std::vector<Vec3f> relativePoses;
	relativePoses.reserve(joints.size() * 5);
	//relativeMats.push_back(Mat4f::getIdentity());
	auto preMat = Mat4f::getIdentity();// Mat4f::getTranslate({ 0.0,-15.5,0.0 }) * Mat4f::getScale({ 10.0,10,10 });
	//update joints
	for (const auto& jt : joints) {
		const auto& rigids = jt->getRigids();
		for (const auto& rd : rigids) {
			rd->updatePostion();
			//relativeMats.push_back(rd->relativeMat);
			relativePoses.push_back(rd->relativePos);
			//relativeMats.push_back(preMat);
			//tmpMap.push_back(rd->modelMat);
		}
	}
	//tShader->setUniformMats("modelMats", relativeMats);
	tShader->setUniformF3s("deltaDis", relativePoses);

	//applay to hair
	/*
	for (int i = 0; i < hair->srcStrands.size();++i){
		const auto& srcPos = hair->srcStrands[i]->pos;
		const auto& jointInfo = hair->srcStrands[i]->jointInfos;
		auto& dstPos = hair->dstStrands[i]->pos;
		for (int j = 0; j < srcPos.size(); ++j) {
			//dstPos[j] = preMat * srcPos[j];
			//continue;
			auto tp1 = relativeMats[jointInfo[0].id][jointInfo[0].jointId[j]] * preMat * srcPos[j];
			if (jointInfo[0].jointWeights[j] < 1 - 1e-4) {
				auto tp12 = relativeMats[jointInfo[0].id][jointInfo[0].jointId[j]+1] * preMat * srcPos[j];
				tp1 = tp1 * jointInfo[0].jointWeights[j] + tp12 * (1.0 - jointInfo[0].jointWeights[j]);
			}
			auto tp2 = relativeMats[jointInfo[1].id][jointInfo[1].jointId[j]] * preMat * srcPos[j];
			if (jointInfo[1].jointWeights[j] < 1 - 1e-4) {
				auto tp22 = relativeMats[jointInfo[1].id][jointInfo[1].jointId[j]+1] * preMat * srcPos[j];
				tp2 = tp2 * jointInfo[1].jointWeights[j] + tp22 * (1.0 - jointInfo[1].jointWeights[j]);
			}
			dstPos[j] = tp1 * jointInfo[0].weight + tp2 * jointInfo[1].weight;
		}
	}
	*/
	//auto newHairData = hair->getGPUData();
	//return newHairData[0];
}

/*
pDataBlock upDataHairByJoint(pHairModel hair, const std::vector<pPhyJointChain>& joints) {
	std::vector<std::vector<Mat4f>> relativeMats;
	relativeMats.reserve(joints.size());
	//update joints
	for (const auto& jt : joints) {
		const auto& rigids = jt->getRigids();
		std::vector<Mat4f> tmpMap;
		tmpMap.reserve(rigids.size());
		for (const auto& rd : rigids) {
			rd->updatePostion();
			tmpMap.push_back(rd->relativeMat);
			//tmpMap.push_back(rd->modelMat);
		}
		relativeMats.push_back(tmpMap);
	}
	//applay to hair
	auto preMat = Mat4f::getTranslate({ 0.0,-15.5,0.0 }) * Mat4f::getScale({ 10.0,10,10 });
	for (int i = 0; i < hair->srcStrands.size();++i){
		const auto& srcPos = hair->srcStrands[i]->pos;
		const auto& jointInfo = hair->srcStrands[i]->jointInfos;
		auto& dstPos = hair->dstStrands[i]->pos;
		for (int j = 0; j < srcPos.size(); ++j) {
			//dstPos[j] = preMat * srcPos[j];
			//continue;
			auto tp1 = relativeMats[jointInfo[0].id][jointInfo[0].jointId[j]] * preMat * srcPos[j];
			if (jointInfo[0].jointWeights[j] < 1 - 1e-4) {
				auto tp12 = relativeMats[jointInfo[0].id][jointInfo[0].jointId[j]+1] * preMat * srcPos[j];
				tp1 = tp1 * jointInfo[0].jointWeights[j] + tp12 * (1.0 - jointInfo[0].jointWeights[j]);
			}
			auto tp2 = relativeMats[jointInfo[1].id][jointInfo[1].jointId[j]] * preMat * srcPos[j];
			if (jointInfo[1].jointWeights[j] < 1 - 1e-4) {
				auto tp22 = relativeMats[jointInfo[1].id][jointInfo[1].jointId[j]+1] * preMat * srcPos[j];
				tp2 = tp2 * jointInfo[1].jointWeights[j] + tp22 * (1.0 - jointInfo[1].jointWeights[j]);
			}
			dstPos[j] = tp1 * jointInfo[0].weight + tp2 * jointInfo[1].weight;
		}
	}
	auto newHairData = hair->getGPUData();
	return newHairData[0];
}


pDataBlock upDataHair(pHairModel hair, std::vector<pPhyJointChain> joints) {
	std::vector<std::vector<Mat4f>> relativeMats;
	relativeMats.reserve(joints.size());
	//update joints
	for (const auto& jt : joints) {
		const auto& rigids = jt->getRigids();
		std::vector<Mat4f> tmpMap;
		tmpMap.reserve(rigids.size());
		for (const auto& rd : rigids) {
			rd->updatePostion();
			tmpMap.push_back(rd->relativeMat);
		}
		relativeMats.push_back(tmpMap);
	}

	auto preMat = Mat4f::getTranslate({ 0.0,-15.5,0.0 }) * Mat4f::getScale({ 10.0,10,10 });
	for (int i = 0;i < hair->srcStrands.size();++i) {
		const auto& srcPos = hair->srcStrands[i]->pos;
		auto& dstPos = hair->dstStrands[i]->pos;
		for (int j = 0; j < dstPos.size(); ++j) {
			dstPos[j] = relativeMats[i][j] * preMat*srcPos[j];
		}
	}
	auto newHairData = hair->getGPUData();
	return newHairData[0];
}
*/
/*
void addWind(pPhyScene pscene, float32 delta) {
	auto nowTime = Utils::getCurrentTime();
	static auto lastTime = nowTime;
	if(nowTime)
}*/
void logRigidPos(const std::vector<pPhyJointChain>& hairChains,std::vector<std::vector<std::vector<float> > > &keyPoints) {
	std::vector<std::vector<float> > framPos;
	for (auto tj : hairChains) {
		for (auto& rigid : tj->getRigids()) {
			framPos.push_back(rigid->relativePos.toVector());
		}
	}
	keyPoints.push_back(framPos);

}

void testSp() {
	string phyDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testPhy)";
	const string spherePath = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\objs\sphere.obj)";
	const string quadPath = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\objs\quad.obj)";
	string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift)";
	auto jointPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift\bone\joint_pos.json)";
	auto jointInfoPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift\hair_joints.txt)";
	//Utils::init();

	auto modelMat = Mat4f::getTranslate({ 0.0f,25.0f,0.0f });
	uint32 width = 512, height = 512;
	uint32 jointNum = 2;
	

	//Physic
	PhyEngine::instance().init();
	auto phyScene = PhyEngine::instance().createPhyScene(4, { 0.0,-1,0.0 });
	auto phyMaterial = PhyEngine::instance().createMaterial(0.5, 0.5, 0.6);
//	auto spRigid = PhyEngine::instance().createRigid(PHY_RIGIDTYPE_DYNAMIC, PHY_GEOTYPE_SPHERE, phyMaterial, modelMat, {1.0},1.0);
//	phyScene->addRigid(spRigid);
	//auto phyPlane = PhyEngine::instance().createRigid(PHY_RIGIDTYPE_STATIC, PHY_GEOTYPE_PLANE, phyMaterial, Mat4f::getIdentity(), { 0.0,1.0,0.0 ,20.0});
	//phyScene->addRigid(phyPlane);
	
	//auto phyChain = PhyEngine::instance().createJointChain(P3D::PhyJointType::PHY_JOINTTYPE_SPHERE,{0.0,-3.0,0.0});
	//auto phyChain = PhyEngine::instance().createJointChain(P3D::PhyJointType::PHY_JOINTTYPE_DISTANCE, {0.0,-3.0,0.0});
	Vec3f tPos(0.0f, 25.0f, 0.0f);
	
	auto phyController = std::make_shared<PhyController>();
	phyController->phyScene = phyScene;

	auto win = P3DEngine::instance().createWindow(width, height);

	auto scene = win->getScene();

	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_PRO);
	//camera->setOrthParams(0.6, 0.6, -100.0, 100.0);


	//	scene->addHair(srcHair,tShader);
	//camera->setViewParams({ 0.0,50.0,50.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	camera->setViewParams({ 0.0,0.0,60.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	camera->setProjParams(40, 1.0, 1, 1000.0);

	//init quad 
	//auto spShader = P3DEngine::instance().createShader();
	//spShader->loadFromFile(netDir + R"(\sp.vert)", netDir + R"(\sp.frag)");
	
	auto phyHair = std::make_shared<HairModel>();
	phyHair->loadFromJson(jointPath);
	auto hairChains = getChainsFromHair(phyHair,0.5);


	std::vector<std::vector<pComp>> allComps;
//	auto capModel = Mat4f::getTranslate({ 0.0,1.21,0.0 }) * Mat4f::getRotZ(90);
//	auto capRigid = PhyEngine::instance().createRigid(PHY_RIGIDTYPE_STATIC, PHY_GEOTYPE_CAPSULE, phyMaterial, capModel, {1.0,1.0},1.0);
//	phyScene->addRigid(capRigid);
	for (auto js : hairChains) {
		phyScene->addJointChain(js);
	}

	//hair->loadFromFile(R"(D:\Work\Projects\ProNew\Vray\tmp\testPhy\simple_hair_10_2.data)");
	auto hairData = loadHairData(netDir,jointInfoPath);
	auto hair = std::get<0>(hairData);
	auto hairGeo = std::get<1>(hairData);
	hairGeo->setModelMat(Mat4f::getIdentity());
	//scene->addHair(phyHair, spShader);
	//hair->setModelMat(modelMat);	//init quad 
	auto quadMesh = MeshDataIO::instance().loadObj(quadPath);
	auto renderQuad = P3DEngine::instance().createMeshGPU();
	auto quadShader = P3DEngine::instance().createShader();
	quadShader->loadFromFile(netDir + R"(\quad.vert)", netDir + R"(\test_fc.frag)");
	renderQuad->setShader(quadShader);
	renderQuad->init(quadMesh->vps, quadMesh->tl);
	
	auto technique = RenderTechnique(width, height);
	technique.setCamera(camera);
	technique.addRenderPass(hairGeo, 3);
	technique.addRenderPass(renderQuad, 1);

	auto mediaCenter = std::make_shared<MediaCenter>();
	mediaCenter->setWindow(win);
	auto cameraController = std::make_shared<CameraControllerPro>();
	cameraController->setCamera(camera);
	//phyController->sp = spMesh;

	mediaCenter->addListener(cameraController);
	mediaCenter->addListener(phyController);

	auto lastT = Utils::getCurrentTime();
//	auto spRigs = phyChain->getRigids();
	//auto hairVps = upDataHairByJoint(hair, hairChains);
	//auto hairVps = upDataHair(phyHair, hairChains);
	auto hairShader = hairGeo->getShader();
	upDataHairByJointGL(hairShader, hairChains);
	uint32 nCount = 1;
	std::default_random_engine rdGen;
	std::uniform_real_distribution<float32> rdUniform(-10, 5);
	std::uniform_int<int32> rdIntUniform(0, 4);
	const string dstDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testPhy\ret1\)";
	int32 nFrame = 0;

	std::vector<std::vector<std::vector<float> > > allFrames;

	std::cout << "begin render..." << std::endl;
	while (!mediaCenter->stop()) {
		//auto modelMat = spRigid->updatePostion();
		//spMesh->setModelMat(modelMat);
		hair->setModelMat(Mat4f::getIdentity());
		phyHair->setModelMat(Mat4f::getIdentity());
		if (phyController->update) {
			upDataHairByJointGL(hairShader, hairChains);
		}
		auto tFbo = technique.render();
		win->renderTexture(tFbo->getRenderTexture(0));
		
		if (phyController->saveImg) {
			std::cout << "logging.." << std::endl;
			logRigidPos(hairChains, allFrames);
			nFrame += 1;
			//auto filePath = netDir + R"(\retData\videos\)" + Utils::makeStr("frame_%04d.npy", nFrame++);
			//auto tImg = tFbo->getRenderTextureCpu(0);
			//NdUtils::writeNpy(filePath, tImg->getNdArray(0));
		}
		//scene->updateHair(phyHair, hairVps);
		if (nCount % 5 == 0) {
			//phyScene->addForce({ 0.0,0.0,(float32)(-5.0+rdUniform(rdGen))}, P3D::PhyForceType::PHY_FORCETYPE_ACCELERATION);
			nCount = rdIntUniform(rdGen);
		}
		//win->render();
		mediaCenter->update();
		auto nowT = Utils::getCurrentTime();
		phyScene->update(min(nowT - lastT,1.0/60.0f));
		lastT = nowT;
		nCount++;
		if(nFrame > 150)
			break;
	}
	if (phyController->saveImg) {
		auto root = p3djson(allFrames);
		auto jsonStr = root.dump(4);
		auto keyPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift\key_frames.json)";
		ofstream of(keyPath);
		of << jsonStr;
		of.close();
	}

	
}

void simpleSp() {
	string phyDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testPhy\)";
	const string spherePath = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\objs\sphere.obj)";
	const string quadPath = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\objs\quad.obj)";
	string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\technique_seg)";
	auto jointPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift\bone\joint_pos.json)";
	auto jointInfoPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift\hair_joints.txt)";
	auto headPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift\bone\head2.obj)";

	uint32 width = 512, height = 512;

	PhyEngine::instance().init();
	auto phyScene = PhyEngine::instance().createPhyScene(4, { 0.0,0.0,0.0 });

	auto joinHair = std::make_shared<HairModel>();
	joinHair->loadFromJson(jointPath);
	auto hairChains = getChainsFromHair(joinHair,0.5);


	for (auto js : hairChains) {
		phyScene->addJointChain(js);
	}
	

	auto win = P3DEngine::instance().createWindow(width, height);
	auto scene = win->getScene();
	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_PRO);
	auto spShader = P3DEngine::instance().createShader();
	spShader->loadFromFile(phyDir + R"(sp.vert)", phyDir + R"(sp.frag)");
	camera->setProjParams(60, 1.0, 1.0, 500.0);
	camera->setViewParams({ 0.0,0.0,60.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });

	std::vector<pComp> phySp;
	auto nCount = 0;
	for (auto tChain : hairChains) {
		for (auto rigid : tChain->getRigids()) {
			auto tCom = MeshDataIO::instance().loadObj(spherePath);
			tCom->setModelMat(rigid->modelMat);
			phySp.push_back(tCom);
			scene->addComp(tCom,spShader);
		}
		nCount++;
	}
	auto head = MeshDataIO::instance().loadObj(headPath);
//	scene->addComp(head, spShader);

	auto mediaCenter = std::make_shared<MediaCenter>();
	mediaCenter->setWindow(win);
	auto cameraController = std::make_shared<CameraControllerPro>();
	
	cameraController->setCamera(camera);
	//phyController->sp = spMesh;

	mediaCenter->addListener(cameraController);
	//mediaCenter->addListener(phyController);
	while (!mediaCenter->stop()) {
		win->render();
		mediaCenter->update();
		phyScene->update(1.0 / 60.0);
		auto nCount = 0;
		for (auto tChain : hairChains) {
			for (auto rigid : tChain->getRigids()) {
				auto tMat = rigid->updatePostion();
				phySp[nCount]->setModelMat(tMat);
				nCount++;
			}
		}
	}

}


int main(int argc, char* argv[]) {
	cout << "hello world" << endl;
	//	auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00009.data)";
	string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testPhy)";
	const string spherePath = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\objs\sphere.obj)";
	Utils::init();
	P3DEngine::instance().init(APP_OPENGL);
	testSp();
	//simpleSp();
	return 0;
/*
	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(512,512);

	auto scene = win->getScene();

	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_PRO);
	camera->setProjParams(45, 1.0, 0.1, 100.0);
	//camera->setOrthParams(0.6, 0.6, -100.0, 100.0);

	auto modelMat = Mat4f::getTranslate({ 0.0f,0.0f,0.0f });


	//	scene->addHair(srcHair,tShader);
	camera->setViewParams({ 0.0,0.0,1.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });

	//init quad 
	auto spMesh = MeshDataIO::instance().loadObj(spherePath);
//	auto renderSp = P3DEngine::instance().createMeshGPU();
	auto spShader = P3DEngine::instance().createShader();
	spShader->loadFromFile(netDir + R"(\sp.vert)", netDir + R"(\sp.frag)");
	//renderSp->setShader(spShader);
//	renderSp->init(spMesh->vps, spMesh->tl);
	scene->addComp(spMesh, spShader);

	
	auto fbo = P3DEngine::instance().createRenderFrame(512, 512, 1);
	fbo->addRenderTextures(1);

	auto technique = RenderTechnique(512, 512);
	technique.setCamera(camera);
	technique.addRenderPass(, 3);
	//technique.addRenderPass(renderQuad, 1);

	runInWindow(technique, win);
	//saveImg(technique, netDir + R"(\retData\videos\)", win);
	return 0;

	uint32 nCount = 1;
	auto sTime = Utils::getCurrentTime();
	while (nCount > 0) {
		auto fbo1 = technique.render();
		nCount--;
		fbo = fbo1;
	}

	glFinish();
	auto eTime = Utils::getCurrentTime();
	cout << endl << "render all cost " << eTime - sTime << " render one frame cost " << (eTime - sTime) / 1000 << endl;
	
	auto img1 = fbo->getRenderTextureCpu(0, true);
	auto imgArr = getFbo2NdArray(fbo);
	NdUtils::writeNpy(netDir + R"(\retData\all_attris.npy)", imgArr);
//	NdUtils::writeNpy(netDir + R"(\retData\ret_img.npy)", imgArr);

	//fbo->apply();
	return 0;*/
}
