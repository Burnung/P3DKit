#include "hairImage.h"
#include "geo/hairModel.h"
#include "geo/meshDataIO.h"
#include "common/json_inc.h"
BEGIN_P3D_NS
using namespace std;


HairImageTech::HairImageTech() {
	P3DEngine::instance().init(APP_OPENGL);
}

void HairImageTech::init(const std::string& dataDir) {

	win = P3DEngine::instance().createWindow(816, 1024);

	auto hairModel = std::make_shared<HairModel>();
	hairModel->loadFromFile(dataDir + "/hair.data");
	hairModel->loadJointInfo(dataDir + "/hair_joints.txt");
	auto hairData = hairModel->getJointGPUData(true);


	auto attris = NdUtils::readNpy(dataDir + "/all_attris.npy").asFloat();
	//auto attris = NdUtils::readNpy(fDir + R"(\all_attris_smooth2.npy)").asFloat();


	uint32 attriNum = attris.shape()[2] / 4;
	int32 lastC = attris.shape()[2] % 4;
	vector<PNdArrayF> attrisData(attriNum);

	for (auto i = 0; i < attriNum; ++i) {
		attrisData[i] = PNdArrayF({ attris.shape()[0],attris.shape()[1],4 });
	}
	if (lastC > 0) {
		auto tmpData = PNdArrayF({ attris.shape()[0], attris.shape()[1], lastC });
		attrisData.push_back(tmpData);
	}

	//	auto attrData2 = std::make_shared<P3D::DataBlock>(hairData[0]->getSize(),P3D::DATA_TYPE_FLOAT32);

	size_t offset = 0;
	//	size_t tStride = 3 * 50 * sizeof(float);

	for (int ns = 0; ns < attris.shape()[0]; ++ns) {
		for (int np = 0; np < attris.shape()[1]; ++np) {
			auto offset = 0;
			for (int i = 0; i < attrisData.size(); ++i) {
				for (int j = 0; j < attrisData[i].shape()[2]; ++j)
					attrisData[i][{ns, np, j}] = attris[{ns, np, offset++}];
			}
		}
	}


	pHair = P3DEngine::instance().createUserGpuGeo();

	pHair->addVertexData(hairData["vps"], "InPos", 3, 0);
	pHair->addElementBuffer(hairData["tl"], GeoTYPE::GEO_TYPE_LINES);

	int nAttr = 1;
	for (auto& arr : attrisData) {
		auto dataBlock = arr.getDataBlock();
		dataBlock->setDataType(DType::Float32);
		auto attName = Utils::makeStr("attri%d", nAttr++);
		pHair->addVertexData(dataBlock, attName, arr.shape()[2], 0);
	}
	pHair->addVertexData(hairData["jointId"], "jointID", 4, 0);
	pHair->addVertexData(hairData["jointWeight"], "jointWeight", 4, 0);

	pHair->addVertexData(hairData["disInfo"], "disInfo", 1, 0);


	pHairShader = P3D::P3DEngine::instance().createShader();
	pHairShader->loadFromFile(dataDir + "/hair.vert", dataDir + "/hair.frag");
	pHair->setShader(pHairShader);


	auto headMesh = MeshDataIO::instance().loadObj(dataDir + "/head.obj");
	pHead = P3DEngine::instance().createMeshGPU();

	pHeadShader = P3DEngine::instance().createShader();
	pHeadShader->loadFromFile(dataDir + "/head.vert", dataDir + "/head.frag");
	pHead->setShader(pHeadShader);

	pHead->init(headMesh->vps, headMesh->tl);


	std::vector<float32> quadVps = { -1.0,-1.0,0.5, 1.0,-1.0,0.5, 1.0,1.0,0.5, -1.0,1.0,0.5 };
	std::vector<uint32> quadTl = { 0,1,2, 0,2,3 };
	auto quadDataVps = std::make_shared<DataBlock>(quadVps.size() * sizeof(float32), DType::Float32);
	quadDataVps->copyData((uint8*)quadVps.data(), quadDataVps->getSize(), 0);
	auto quadDataTl = std::make_shared<DataBlock>(quadTl.size() * sizeof(uint32), DType::Uint32);
	quadDataTl->copyData((uint8*)quadTl.data(), quadDataTl->getSize(), 0);

	pQuadShader = P3DEngine::instance().createShader();
	pQuadShader->loadFromFile(dataDir + "/quad.vert", dataDir + "/quad.frag");

	pQuad = P3DEngine::instance().createUserGpuGeo();
	pQuad->addVertexData(quadDataVps, "InPos", 3, 0);
	pQuad->addElementBuffer(quadDataTl, GeoTYPE::GEO_TYPE_TRIS);
	pQuad->setShader(pQuadShader);
	pQuad->commit();


	auto jsonStr = Utils::readFileString(dataDir + "/key_frames.json");
	auto jsonV = P3djson::parse(jsonStr);
	for (auto& t : jsonV) {
		std::vector<Vec3f> tFrame;
		tFrame.reserve(t.size() * 3);
		for (auto& tp : t) {
			tFrame.push_back(Vec3f(tp[0],tp[1],tp[2]));
		}
		animalFrame.push_back(tFrame);
	}
}


pTechnique HairImageTech::setupScene(PNdArray srcImg, const std::vector<float>& modelMat) {
	auto w = srcImg.shape()[1], h = srcImg.shape()[0];


	auto proMat = Mat4f::getPerspective(1.0 / PI * 180.0,w*1.0 / h,0.01,1000);
	//auto proMat = Mat4f::getKSPerspective2(1.0 / PI * 180.0, 1.0*w / h);
	std::vector<float32> preMatData = { 0.1,0.0,0.0,0.0, 0.0,0.1,0.0,0.5, 0.0,0.0,0.1,0.0, 0.0,0.0,0.0,1.0 };
	Mat4f preMat(preMatData.data());

	Mat4f headMat((float*)(modelMat.data()));
	headMat = headMat.getTranspose();


	auto  theta = 1.0 * rand() / RAND_MAX * 180.0 - 90.0;
	auto phi = 1.0 * rand() / RAND_MAX * 360.0;
	auto camPos = Vec3f::getSpPos(theta, phi);
	pHairShader->setUniformF3("camDir", camPos);

	pHeadShader->setUniformMat("mvp", proMat*headMat);
	pHairShader->setUniformMat("mvp", proMat*headMat*preMat);
	
	auto aniId = rand() % animalFrame.size();
	pHairShader->setUniformF3s("deltaDis", animalFrame[aniId]);

	auto srcTex = std::make_shared<CpuImage>(srcImg);

	auto srcGpuTex = P3DEngine::instance().createTexture(srcTex, SampleType::SAMPLE_LINER, BUFFER_USE_TYPE::READ_ONLY, 1);


	pQuadShader->setUniformTex("srcTex", srcGpuTex);

	auto tRenderTech = std::make_shared<RenderTechnique>(w,h);

	tRenderTech->addRenderPass({ pHead,pHair }, 1);
	tRenderTech->addRenderPass(pQuad,1);

	return tRenderTech;
}

PNdArrayF HairImageTech::render(PNdArray srcImg, const std::vector<float>& modelMat) {
	auto tRenderTech = setupScene(srcImg, modelMat);

	auto tFbo = tRenderTech->render();

	std::vector<PNdArray> rets;
	auto tmpCpu = tFbo->getRenderTextureCpu(0, true);
	auto ret = tmpCpu->getNdArray();

	return ret.asFloat();

}

pRenderFrame HairImageTech::renderTech(PNdArray srcImg, const std::vector<float>& modelMat) {
	auto tRenderTech = setupScene(srcImg, modelMat);
	
	return tRenderTech->render();
}
END_P3D_NS