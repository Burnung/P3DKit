#include "hairAttris.h"
#include "geo/hairModel.h"

BEGIN_P3D_NS

HairAttrisTech::HairAttrisTech() {
	P3DEngine::instance().init(APP_OPENGL);
}

void HairAttrisTech::init(const std::string& workDir, float32 w, float32 h,float32 orthW,int32 nFbo) {
	win = P3DEngine::instance().createWindow(w, h);
	auto hair = std::make_shared<HairModel>();
	hair->loadFromFile(workDir + "/hair.data");
	auto userGpu = P3DEngine::instance().createUserGpuGeo();
	auto hairData = hair->getGPUData();

	userGpu->addVertexData(hairData[0], "InPos", 3, 0);
	userGpu->addElementBuffer(hairData[1], GEO_TYPE_LINES);
	//auto tlBuffer = P3DEngine::instance().createVertexBuffer();
	//tlBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ELEMETN, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_UINT32, hairData[1]);
	//userGpu->addElementBuffer(tlBuffer, GEO_TYPE_LINES);

	cam = std::make_shared<Camera>();
	cam->setCameraType(Camera::CAMERA_TYPE_ORTH);
	cam->setOrthParams(orthW, orthW, 0.0, 300.0);
	cam->setViewParams({ 0.0,0.0,1.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });

	
	//create attris
	auto attriData = std::make_shared<DataBlock>(hair->allVex * 2 * sizeof(float32), DType::Float32);
	float32* pData = attriData->rawData<float32>();
	int idx = 0;
	for (int i = 0; i < hair->srcStrands.size();++i) {
		auto sd = hair->srcStrands[i];
		for (int j = 0; j < sd->pos.size();++j) {
			pData[idx * 2] = i + 1;
			pData[idx * 2 + 1] = j + 1;
			idx++;
		}
	}
	for (int j = 0; j < hair->allVex; ++j) {
		if (pData[2*j+1] < 0) {
			std::cout << j << std::endl;
		}
	}
	
	//////create attris single
	/*
	auto strandIds = std::make_shared<DataBlock>(hair->allVex * sizeof(float32), DATA_TYPE_FLOAT32);
	auto vexIds = std::make_shared<DataBlock>(hair->allVex * sizeof(float32), DATA_TYPE_FLOAT32);
	float* pData1 = strandIds->rawData<float32>();
	float* pData2 = vexIds->rawData<float32>();
	uint32 idx = 0;
	for (int i = 0; i < hair->srcStrands.size();++i) {
		auto sd = hair->srcStrands[i];
		for (int j = 0; j < sd->pos.size();++j) {
			pData1[idx] = float32(i + 1);
			pData2[idx] = float32(j +1);// j + 1;
			idx++;
		}
	}
	for (int i = 0; i < hair->allVex; ++i) {
		if (pData2[i] < 0) {
			int aaaaa = 0;
			std::cout << i << std::endl;
		}
	}


	userGpu->addVertexData(strandIds, "sId", 1, 0);
	userGpu->addVertexData(vexIds, "vId", 1, 0);
	*/
	userGpu->addVertexData(attriData, "Ids", 2, 0);
	auto shader = P3DEngine::instance().createShader();
	shader->loadFromFile(workDir + "/hair.vert", workDir + "/hair.frag");
	userGpu->setShader(shader);
	//userGpu->commit();
//	userGpu->setModelMat(Mat4f::getTranslate({ 0.0f,.0f }));
	userGpu->setModelMat(Mat4f::getIdentity());

	pTech = std::make_shared<RenderTechnique>(w,h);
	pTech->addRenderPass(userGpu, nFbo);
	pTech->setCamera(cam);

	this->w = w, this->h = h;
	this->nFbo = nFbo;
}

PNdArrayF HairAttrisTech::render(std::vector<float32> camPos) {
	cam->setViewParams({ camPos[0],camPos[1],camPos[2] }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	auto tFbo = pTech->render();

	std::vector<PNdArray> rets;
	for (int i = 0; i < nFbo; ++i) {
		auto tmpCpu = tFbo->getRenderTextureCpu(i, true);
		auto tNd = tmpCpu->getNdArray();
		rets.push_back(tNd);
	}
	auto ret = NdUtils::concat(rets);
	return ret.asFloat();
}


pRenderFrame HairAttrisTech::renderTech(const std::vector<float>& camPos) {

	cam->setViewParams({ camPos[0],camPos[1],camPos[2] }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	auto tFbo = pTech->render();
	return tFbo;

}
END_P3D_NS