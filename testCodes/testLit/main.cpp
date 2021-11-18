
#include<iostream>
#include <string>
#include "engine/P3dEngine.h"
#include <opencv2/opencv.hpp>
#include "geo/meshDataIO.h"
#include "engine/Camera.h"
#include "common/NdArray.h"
#include <GL/glew.h>
#ifdef _WIN32
#	define WINDOWS_LEAN_AND_MEAN
#	define NOMINMAX
#endif

#include "engine/OPenGL/OGLVertexBuffer.h"
#include "engine/OPenGL/OGLUserMeshGpu.h"
#include "engine/renderTechnique.h"
#include "engine/extern/mediaCenter.h"
#include "engine/extern/cameraControllerOrth.h"

using namespace P3D;
using namespace std;

class BigController :public CameraControllerOrth {
public:
	BigController() = default;
	
	virtual void processKeyboard(const std::vector<KEY_STATES>&keyStates) {
		CameraControllerOrth::processKeyboard(keyStates);
		
	if (keyStates[KEY_NAME_I] == KEY_STATE_PRESS)
		litPos.y += moveSpeed;
	if (keyStates[KEY_NAME_K] == KEY_STATE_PRESS)
		litPos.y -= moveSpeed;
	if (keyStates[KEY_NAME_J] == KEY_STATE_PRESS)
		litPos.x -= moveSpeed;
	if (keyStates[KEY_NAME_L] == KEY_STATE_PRESS)
		litPos.x += moveSpeed;
	if (keyStates[KEY_NAME_SPACE] == KEY_STATE_PRESS)
		saveImg = true;
	
	litPos.doNormalize();

	}
public:
	Vec3f litPos = Vec3f(0.0, 0.0, 1.0);
	bool saveImg = false;
};

P3D::pUserGeoGPU loadHairData(const std::string& fDir) {
	auto hair = std::make_shared<P3D::HairModel>();
	hair->loadFromFile(fDir + R"(\hair.data)");
	auto hairData = hair->getGPUData();

	//auto attris = NdUtils::readNpy(fDir + R"(\all_attris.npy)").asFloat();
	auto attris = NdUtils::readNpy(fDir + R"(\all_attris.npy)").asFloat();

	
	uint32 attriNum = attris.shape()[2] / 4;
	int32 lastC = attris.shape()[2] % 4;
	vector<PNdArrayF> attrisData(attriNum);

	for (auto i = 0; i < attriNum; ++i) {
		attrisData[i] = PNdArrayF({ attris.shape()[0],attris.shape()[1],4 });
	}
	if (lastC > 0) {
		auto tmpData = PNdArrayF({ attris.shape()[0], attris.shape()[1], lastC});
		attrisData.push_back(tmpData);
	}

	auto attrData2 = std::make_shared<P3D::DataBlock>(hairData[0]->getSize(),P3D::DATA_TYPE_FLOAT32);

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

	auto bigHairGpu = std::make_shared<P3D::OGLUserMeshGPU>();
	auto posBuffer = std::make_shared<P3D::OGLVertexBuffer>();
	auto tlBuffer = std::make_shared<P3D::OGLVertexBuffer>();

	tlBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ELEMETN, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_UINT32, hairData[1]);
	posBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_FLOAT32, hairData[0]);
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


	bigHairGpu->addElementBuffer(tlBuffer, P3D::GEO_TYPE_LINES);

	auto shader = P3D::P3DEngine::instance().createShader();
	shader->loadFromFile(fDir + R"(\hair.vert)", fDir + R"(\hair.frag)");

	bigHairGpu->setShader(shader);
	
	return bigHairGpu;
	
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
void runInWindow(const RenderTechnique& technique, pWindow win,pShader hairShader) {
	auto mediaCenter = std::make_shared<MediaCenter>();
	mediaCenter->setWindow(win);
	auto cameraController = std::make_shared<BigController>();
	auto camera = technique.getCamera();
	cameraController->setCamera(camera);
	mediaCenter->addListener(cameraController);
	uint32 nCount = 0;
	uint32 frameCount = 0;
	string dstDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testLit\technique\retData\)";
	auto sTime = Utils::getCurrentTime();
	while (!mediaCenter->stop()) {
		hairShader->setUniformF3("litDir",cameraController->litPos);
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
		if (cameraController->saveImg) {
			auto filePath = dstDir + Utils::makeStr("frame_%05d.npy", frameCount++);
			auto tImg = fbo->getRenderTextureCpu(0, true);
			NdUtils::writeNpy(filePath, tImg->getNdArray(0));
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

int main(int argc, char* argv[]) {
	cout << "hello world" << endl;
	//	auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00009.data)";
	string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testLit\technique)";
	const string quadPath = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\objs\quad.obj)";
	Utils::init();
	uint32 winSize = 512;// 1024;
	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(winSize,winSize);

	auto scene = win->getScene();

	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_ORTH);
	camera->setOrthParams(0.6, 0.6, -100.0, 100.0);

	auto modelMat = Mat4f::getTranslate({ 0.0f,-1.55f,0.0f });

	auto bigHairModel = loadHairData(netDir);
	bigHairModel->setModelMat(modelMat);
	auto hairShader = bigHairModel->getShader();

	//	scene->addHair(srcHair,tShader);
	//camera->setViewParams({ -0.360796,0.888975,0.267063 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	camera->setViewParams({ 0.0,0.0,1.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	//camera->setViewParams({ 0.940322,0.0,-0.305529 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	

	//init quad 
	auto quadMesh = MeshDataIO::instance().loadObj(quadPath);
	auto renderQuad = P3DEngine::instance().createMeshGPU();
	auto quadShader = P3DEngine::instance().createShader();
	quadShader->loadFromFile(netDir + R"(\quad.vert)", netDir + R"(\test_fc.frag)");
	renderQuad->setShader(quadShader);
	renderQuad->init(quadMesh->vps, quadMesh->tl);
	scene->addComp(quadMesh, quadShader);
	auto fbo = P3DEngine::instance().createRenderFrame(512, 512, 1);
	fbo->addRenderTextures(1);

	auto technique = RenderTechnique(winSize, winSize);
	technique.setCamera(camera);
	technique.addRenderPass(bigHairModel, 4);
	technique.addRenderPass(renderQuad, 1);

	runInWindow(technique, win,hairShader);
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
	NdUtils::writeNpy(netDir + R"(\retData\img.npy)",img1->getNdArray(0));
	auto attrFbo = technique.getPassFrame(0);
	auto imgArr = getFbo2NdArray(attrFbo);
	NdUtils::writeNpy(netDir + R"(\retData\all_attris.npy)", imgArr);
//	NdUtils::writeNpy(netDir + R"(\retData\ret_img.npy)", imgArr);

	//fbo->apply();
	return 0;
}
