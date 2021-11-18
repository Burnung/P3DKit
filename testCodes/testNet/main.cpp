
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

/*
P3D::pUserGeoGPU loadHairData(const std::string& fDir) {
	auto hair = std::make_shared<P3D::HairModel>();
	hair->loadFromFile(fDir + R"(\hair.data)");
	auto hairData = hair->getGPUData();

	auto attris = NdUtils::readNpy(fDir + R"(\all_attris.npy)").asFloat();
	//auto attris = NdUtils::readNpy(fDir + R"(\all_attris_smooth2.npy)").asFloat();

	
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
	}

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
*/
class AttriChanger :public BaseListener {
public:
	virtual void processKeyboard(const std::vector<KEY_STATES>&keyStates) {
		for (auto i = (int)KEY_NAME_0; i <= (int)KEY_NAME_9; ++i) {
			if (keyStates[i] == KEY_STATE_PRESS) {
				tIndex = i - KEY_NAME_0;
			}
		}
		if (keyStates[KEY_NAME_U] == KEY_STATE_PRESS) {
			valueA = std::vector<float32>(9, 1.0);
			valueB = std::vector<float32>(9, 0.0);
		}
		if (keyStates[KEY_NAME_J] == KEY_STATE_PRESS)
			useA = (!useA);
		if (keyStates[KEY_NAME_SPACE]== KEY_STATE_PRESS) {
			cout << "A:" << endl;
			for (auto t : valueA)
				cout << t << " ";
			cout << endl << "B:" << endl;
			for (auto t : valueB)
				cout << t << " ";
			cout << endl;
		}
		if (tIndex >= valueA.size())
			return;
		if (keyStates[KEY_NAME_I] == KEY_STATE_PRESS) {
			if (useA)
				valueA[tIndex] += delta;
			else
				valueB[tIndex] += delta;
		}
		else if (keyStates[KEY_NAME_K] == KEY_STATE_PRESS) {
			if (useA)
				valueA[tIndex] -= delta;
			else
				valueB[tIndex] -= delta;
		}

	}

	virtual void processMouse(const std::vector<MOUSE_BUTTON_STATES>& keyStates,Vec2f mousePos) {}
	std::vector<std::vector<float>> getValue() const { return { valueA,valueB }; }

private:
	std::vector<float32> valueA = std::vector<float32>(9, 1.0);
	std::vector<float32> valueB = std::vector<float32>(9, 0.0);
	float delta = 0.01;
	int tIndex = 0;
	bool useA = true;
};
/*
pTechnique getTeethTech(const std::string& dDir) {
	auto pTech = std::make_shared<BlendShapeRender>();

}*/

pTechnique getZhangyuTech(const std::string& fDir){
	auto meshPath = fDir + R"(\obj.obj)";
	//auto meshPath2 = fDir + R"(\zhangyu_ps.obj)";
	
	auto tComp = MeshDataIO::instance().loadObj(meshPath);
	auto gpuMesh = P3DEngine::instance().createUserGpuGeo();
	gpuMesh->addVertexData(tComp->vps, "InPos", 3, 0);
	gpuMesh->addVertexData(tComp->vns, "InVn", 3, 0);
	gpuMesh->addVertexData(tComp->uvs, "InUv", 2, 0);
	gpuMesh->addElementBuffer(tComp->tl, GeoTYPE::GEO_TYPE_TRIS);
	

	auto attrPath = fDir + R"(\all_attris.npy)";
	auto attris = NdUtils::readNpy(attrPath).asFloat();	

	uint32 attriNum = attris.shape()[2] / 3;
	int32 lastC = attris.shape()[2] % 3;
	vector<PNdArrayF> attrisData(attriNum);

	for (auto i = 0; i < attriNum; ++i) {
		attrisData[i] = PNdArrayF({ attris.shape()[0],attris.shape()[1],3 });
	}
	if (lastC > 0) {
		auto tmpData = PNdArrayF({ attris.shape()[0], attris.shape()[1], lastC});
		attrisData.push_back(tmpData);
	}

	size_t offset = 0;
	size_t tStride = 3 * 50 * sizeof(float);

	for (int ns = 0; ns < attris.shape()[0]; ++ns) {
		for (int np = 0; np < attris.shape()[1]; ++np) {
			auto offset = 0;
			for (int i = 0; i < attrisData.size(); ++i) {
				for (int j = 0; j < attrisData[i].shape()[2]; ++j)
					attrisData[i][{ns, np, j}] =  attris[{ns, np, offset++}];
					
			}
		}
	}
	std::vector<pTexture> allTexs;
	for (auto& tArray : attrisData) {
		auto tImg = std::make_shared<CpuImage>(tArray);
		auto tTex = P3DEngine::instance().createTexture(tImg, SampleType::SAMPLE_LINER, BUFFER_USE_TYPE::READ_ONLY, 1);
		allTexs.push_back(tTex);
	}


	//Shader
	auto tShader = P3DEngine::instance().createShader();
	//tShader->loadFromFile(fDir + R"(\mesh.vert)", fDir + R"(\test_fc_es.frag)");
	tShader->loadFromFile(fDir + R"(\mesh.vert)", fDir + R"(\mesh.frag)");
	for (int i = 0; i < allTexs.size(); ++i) {
		tShader->setUniformTex(Utils::makeStr("passTex%d", i), allTexs[i]);
	}

	gpuMesh->setShader(tShader);
	gpuMesh->commit();

	auto retTech = std::make_shared<RenderTechnique>(512, 512);
	retTech->addRenderPass(gpuMesh, 1);

	auto psGpuGeo = P3DEngine::instance().createUserGpuGeo();
	//auto psGeo = MeshDataIO::instance().loadObj(meshPath2);
//	psGpuGeo->addVertexData(psGeo->vps, "InPos", 3);
//	psGpuGeo->addVertexData(psGeo->uvs, "InUv", 2);
//	psGpuGeo->addVertexData(psGeo->vns, "InVn", 3);
//	psGpuGeo->addElementBuffer(psGeo->tl, GeoTYPE::GEO_TYPE_TRIS);
//	string psImgPath = fDir + R"(\02L_Diffuse.A2.png)";
//	auto cvImg = cv::imread(psImgPath);

//	PNdArrayU8 imgArray1({ cvImg.rows,cvImg.cols,3 });
//	auto tData = imgArray1.getDataBlock();
//	tData->copyData( cvImg.data, tData->getSize(), 0);
//	auto psCpuImg = std::make_shared<CpuImage>(imgArray1);
//	auto psTex = P3DEngine::instance().createTexture(psCpuImg, SAMPLE_LINER, READ_ONLY, 1);

//	auto psShader = P3DEngine::instance().createShader();
//	psShader->loadFromFile(fDir + R"(\mesh.vert)", fDir + R"(\mesh_2.frag)");

//	psShader->setUniformTex("passTex0", psTex);

//	psGpuGeo->setShader(psShader);
	//psGpuGeo->commit();
//	retTech->addRenderPass(psGpuGeo, 1, true);
	return  retTech;
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

void runInWindow(RenderTechnique& technique, pWindow win,pShader tShader = nullptr) {
	auto mediaCenter = std::make_shared<MediaCenter>();
	mediaCenter->setWindow(win);
	auto valueChanger = std::make_shared<AttriChanger>();
	mediaCenter->addListener(valueChanger);
	auto cameraController = std::make_shared<CameraControllerOrth>();

							   
	auto camera = technique.getCamera();
	//camera->setOrthDis(300.0);
	camera->setOrthDis(1.2);
	cameraController->setCamera(camera);
	mediaCenter->addListener(cameraController);
	cameraController->zoomSpeed = 0.05;
	uint32 nCount = 0;
	auto sTime = Utils::getCurrentTime();
	auto rotZ = 0.0f;
	while (!mediaCenter->stop()) {
		auto fbo = technique.render();
		win->renderTexture(fbo->getRenderTexture(0));
		mediaCenter->update();
		nCount++;
		if (tShader) {
			auto values = valueChanger->getValue();
			tShader->setUniformFv("valueA", values[0]);
			tShader->setUniformFv("valueB", values[1]);
		}
		//auto matRot = Mat4f::getRotZ()
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
void saveImg(RenderTechnique& technique, const std::string& dstDir,pWindow win) {
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
	//string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\technique_seg)";
	//string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\technique)";
	//string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\menglu_yellow_pos_v2)";
	string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\zhangyu3)";
	//string netDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\buddha)";

	const string quadPath = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\objs\quad.obj)";
	Utils::init();
	uint32 winSize = 512;// 1024;
	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(winSize,winSize);

	auto scene = win->getScene();

	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_ORTH);
	//camera->setOrthParams(300, 300, 0, 1000.0);
	camera->setOrthParams(1.2, 1.2, 0, 1000.0);

	camera->setViewParams({ 0.0,0.0,300 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	//camera->setViewParams({ -9.14531,11.1473,37.3108 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	
	//init quad 
	auto quadMesh = MeshDataIO::instance().loadObj(quadPath);
	auto renderQuad = P3DEngine::instance().createMeshGPU(); 
	auto quadShader = P3DEngine::instance().createShader();
	//quadShader->loadFromFile(netDir + R"(\quad.vert)", netDir + R"(\test_fc.frag)");
	//renderQuad->setShader(quadShader);
//	renderQuad->init(quadMesh->vps, quadMesh->tl);

	auto technique = getZhangyuTech(netDir);
	technique->setCamera(camera);
	auto tShader = technique->getShader(0);

	runInWindow(*technique, win,tShader);
	//saveImg(technique, netDir + R"(\retData\videos\)", win);

	return 0;

	uint32 nCount = 1;
	auto sTime = Utils::getCurrentTime();
	while (nCount > 0) {
		auto fbo1 = technique->render();
		nCount--;
		//fbo = fbo1;
	}

	glFinish();
	/*
	auto eTime = Utils::getCurrentTime();
	cout << endl << "render all cost " << eTime - sTime << " render one frame cost " << (eTime - sTime) / 1000 << endl;
	
	auto img1 = fbo->getRenderTextureCpu(0, true);
	NdUtils::writeNpy(netDir + R"(\retData\img.npy)",img1->getNdArray(0));
	auto attrFbo = technique.getPassFrame(0);
	auto imgArr = getFbo2NdArray(attrFbo);
	NdUtils::writeNpy(netDir + R"(\retData\all_attris.npy)", imgArr);
//	NdUtils::writeNpy(netDir + R"(\retData\ret_img.npy)", imgArr);
*/
	//fbo->apply();
	return 0;
}
