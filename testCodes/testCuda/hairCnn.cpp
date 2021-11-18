#include "hairCnn.h"
#include "common/NdArray.h"
#include "engine/OPenGL/OGLUserMeshGpu.h"
#include "engine/OPenGL/OGLVertexBuffer.h"

void HairCNNRender::init(const std::string& fDir) {
	net = std::make_shared<CudnnNet>();
	auto conv1 = std::make_shared<CudnnConvOpt>();
	auto conv2 = std::make_shared<CudnnConvOpt>();
	auto conv3 = std::make_shared<CudnnConvOpt>();
	auto conv4 = std::make_shared<CudnnConvOpt>();
	conv1->init(14, 32, 1, 0, 1, 1, true);
	conv2->init(32, 64, 1, 0, 1, 1, true);
	conv3->init(64, 32, 1, 0, 1, 1, true);
	conv4->init(32, 4, 1, 0, 1, 1, true);
	loadConvFromFile(conv1, fDir + R"(\net.0.weight.npy)");
	loadConvFromFile(conv2, fDir + R"(\net.2.weight.npy)");
	loadConvFromFile(conv3, fDir + R"(\net.4.weight.npy)");
	loadConvFromFile(conv4, fDir + R"(\net.6.weight.npy)");
	auto relue1 = std::make_shared<CudnnActiveOpt>();
	auto relue2 = std::make_shared<CudnnActiveOpt>();
	auto relue3 = std::make_shared<CudnnActiveOpt>();
	auto tanh4 = std::make_shared<CudnnActiveOpt>();

	relue1->init(CUDNN_ACTIVATION_RELU,false, 0);
	relue2->init(CUDNN_ACTIVATION_RELU,false, 0);
	relue3->init(CUDNN_ACTIVATION_RELU,false, 0);
	tanh4->init(CUDNN_ACTIVATION_TANH, false, 0);
	net->addOpts({ conv1,relue1,conv2,relue2,conv3,relue3,conv4,tanh4 });

}

void HairCNNRender::loadConvFromFile(pConvOpt conv, const std::string& fpath) {
	auto pArray = ksu::NdUtils::readNpy(fpath).asFloat();
	conv->setValue(pArray);
}

P3D::pUserGeoGPU HairCNNRender::loadHairData(const std::string& fDir) {
	auto hair = std::make_shared<P3D::HairModel>();
	hair->loadFromFile(fDir + R"(\hair.data)");
	auto hairData = hair->getGPUData();

	auto a1 = ksu::NdUtils::readNpy(fDir + R"(\a1.npy)").asFloat();
	auto a2 = ksu::NdUtils::readNpy(fDir + R"(\a2.npy)").asFloat();
	//auto a3 = ksu::NdUtils::readNpy(fDir + R"(\a3.npy)");

	auto attrData1 = std::make_shared<P3D::DataBlock>(hairData[0]->getSize(),P3D::DATA_TYPE_FLOAT32);
	auto attrData2 = std::make_shared<P3D::DataBlock>(hairData[0]->getSize(),P3D::DATA_TYPE_FLOAT32);
	//auto attrData3 = std::make_shared<P3D::DataBlock>(hairData[0]->getSize(),P3D::DATA_TYPE_FLOAT32);

	size_t offset = 0;
	size_t tStride = 3 * 50 * sizeof(float);
	float* attr1Raw = attrData1->rawData<float>();
	float* attr2Raw = attrData2->rawData<float>();
	for (int ns = 0; ns < a1.shape()[0]; ++ns) {
		for (int np = 0; np < a1.shape()[1]; ++np) {
			for (int i = 0; i < a1.shape()[2]; ++i) {
				auto idx = a1.shape()[1] * a1.shape()[2] * ns + np * a1.shape()[2] + i;
				attr1Raw[idx] = a1[{ns, np, i}];
				attr2Raw[idx] = a2[{ns, np, i}];
			}
		}
	}
	/*for (int i = 0; i < hair->srcStrands.size();++i) {
		if(hair->srcStrands.size() < 2)
			continue;
		size_t tSize = hair->srcStrands[i]->pos.size() * 3 * sizeof(float);
		attrData1->copyData((P3D::uint8*)(a1.rawData() + tStride * i) ,tSize,offset);
		attrData2->copyData((P3D::uint8*)(a2.rawData() + tStride * i) ,tSize,offset);
	//	attrData3->copyData((P3D::uint8*)(a3.rawData() + tStride * i) ,tSize,offset);
		offset += tSize;
	}*/
	auto attrBuffer1 = std::make_shared<P3D::OGLVertexBuffer>();
	auto attrBuffer2 = std::make_shared<P3D::OGLVertexBuffer>();
//	auto attrBuffer3 = std::make_shared<P3D::OGLVertexBuffer>();
	auto posBuffer = std::make_shared<P3D::OGLVertexBuffer>();
	auto tlBuffer = std::make_shared<P3D::OGLVertexBuffer>();

	attrBuffer1->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_FLOAT32, attrData1);
	attrBuffer2->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_FLOAT32, attrData2);
	//attrBuffer3->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_FLOAT32, attrData3);

	posBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ARRAY, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_FLOAT32, hairData[0]);
	tlBuffer->init(P3D::VertexBufferType::VERTEX_BUFFER_ELEMETN, P3D::VertexBufferUsage::STATIC_DRAW, P3D::DATA_TYPE_UINT32, hairData[1]);

	bigHairGpu = std::make_shared<P3D::OGLUserMeshGPU>();
	bigHairGpu->addVertexData(posBuffer, "InPos", 3, 0);

	bigHairGpu->addVertexData(attrBuffer1, "attri1", 3, 0);
	bigHairGpu->addVertexData(attrBuffer2, "attri2", 3, 0);
	//bigHairGpu->addVertexData(attrBuffer3, "attri3", 3, 0);
	bigHairGpu->addElementBuffer(tlBuffer, P3D::GEO_TYPE_LINES);

	auto shader = P3D::P3DEngine::instance().createShader();
	shader->loadFromFile(fDir + R"(\hair.vert)", fDir + R"(\hair.frag)");

	bigHairGpu->setShader(shader);
	
	return bigHairGpu;
	
}

void HairCNNRender::setInputTexs(const std::vector<P3D::pTexture>& texs) {
	srcInputs.reserve(texs.size());
	for (int i = 0; i < texs.size();++i) {
		auto ti = std::make_shared<CudnnTensor>();
		if (i < texs.size() - 2)
			ti->initfromGLTex(texs[i], 4, true);
		else
			ti->initfromGLTex(texs[i], 3, true);
		srcInputs.push_back(ti);
	}
	useInput = CudnnTensor::concat(srcInputs);
	net->preRun(useInput);
}

pTensor HairCNNRender::forward() {
/*	for (auto t : srcInputs)
		t->update();
	CudnnTensor::concat(srcInputs, useInput);*/
	return net->forward(useInput);
}

