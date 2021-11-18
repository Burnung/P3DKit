
#include<iostream>
#include <string>
#include "engine/P3dEngine.h"
#include <opencv2/opencv.hpp>
#include "geo/meshDataIO.h"
#include "engine/Camera.h"

#include <GL/glew.h>
#ifdef _WIN32
#	define WINDOWS_LEAN_AND_MEAN
#	define NOMINMAX
#endif

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <helper/helper_cuda.h>
#include <helper/helper_gl.h>
#include <helper/helper_timer.h>
#include <cudaGL.h>
#include <cudnn.h>
#include "simpleCuda.h"
#include "hairCnn.h"
#include "common/json_inc.h"

using namespace P3D;
using namespace std;


void testCuda(int argc,char** argv,pTexture glTex) {
	auto devId = findCudaDevice(argc, (const char**)argv);
	checkCudaErrors(cudaGLSetGLDevice(devId));

	std::cout << "cuda devId is " << devId << std::endl;
	cudaGraphicsResource_t cudaTex;// = nullptr;
	checkCudaErrors(cudaGraphicsGLRegisterImage(&cudaTex, glTex->getHandler(), GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone));

	cudaArray_t texture_ptr;

	checkCudaErrors(cudaGraphicsMapResources(1, &cudaTex, 0));
	checkCudaErrors(cudaGraphicsSubResourceGetMappedArray(&texture_ptr, cudaTex, 0, 0));

	size_t texSize = glTex->Width() * glTex->Height() * 3 * sizeof(float32);
	auto dataBlock = P3D::DataBlock(texSize, P3D::DATA_TYPE_FLOAT32);
	checkCudaErrors(cudaMemcpyFromArray(dataBlock.rawData(),texture_ptr,0,0,texSize,cudaMemcpyDeviceToHost));
	auto cvImg4 = cv::Mat(glTex->Height(), glTex->Width(),CV_32FC3, dataBlock.rawData());
	//retImg *= 255;
	cv::Mat useMat(cvImg4.size(),CV_8U);
	for (auto r = 0; r < cvImg4.rows; ++r) {
		for (auto c = 0; c < cvImg4.cols; ++c) {
			auto tmp = cvImg4.at<cv::Vec3f>(r, c);
			useMat.at<uint8>(r, c) = (uint8)(tmp[0] * 255);
		}
	}
	cv::imwrite("cudaTex.png", useMat);
	int a = 0;

}

void testCudnn(int argc,char** argv,pTexture inputTex) {
	initCudaGL(argc, argv);
	char data[1024];
	float* tmpDev = nullptr;

	auto iTensor = std::make_shared<CudnnTensor>();
	iTensor->initfromGLTex(inputTex,3,true);

	auto tConv = std::make_shared<CudnnConvOpt>();
	tConv->init(3,3,1,0,1,1,true);
	auto dataBlock = std::make_shared<DataBlock>(9*sizeof(float32));
	float *dataPtr = dataBlock->rawData<float>();
	for (int i = 0; i < 9; ++i)
		dataPtr[i] = 0;
	dataPtr[0] = 1.0, dataPtr[4] = 1.0, dataPtr[8] = 1.0;

	//tConv->setValue(dataBlock);
	

	//relue opt
	auto relu = std::make_shared<CudnnActiveOpt>();
	relu->init(CUDNN_ACTIVATION_RELU, false, 0);

	auto net = std::make_shared<CudnnNet>();
	net->addOpt(tConv);
	net->addOpt(relu);

	net->preRun(iTensor);
	
	auto ret = net->forward(iTensor);

	ret->transform(false);


	auto cpuData = ret->dump();
	auto cvImg4 = cv::Mat(iTensor->h, iTensor->w,CV_32FC3, cpuData->rawData());
	//retImg *= 255;
	cv::Mat useMat(cvImg4.size(),CV_8U);
	for (auto r = 0; r < cvImg4.rows; ++r) {
		for (auto c = 0; c < cvImg4.cols; ++c) {
			auto tmp = cvImg4.at<cv::Vec3f>(r, c);
			useMat.at<uint8>(r, c) = (uint8)(tmp[0] * 255);
		}
	}
	cv::imwrite("cudnnTex.png", useMat);


}

auto testHair(int argc,char** argv) {
	initCudaGL(argc, argv);
	std::shared_ptr<HairCNNRender> hairRender = std::make_shared<HairCNNRender>();
	std::string modelDir = R"(D:\Work\Projects\ProNew\Vray\tmp)";
	//hairRender->init(modelDir+R"(\testNet)");
	auto hairModel =  hairRender->loadHairData(modelDir + R"(\testNet\dump)");
	std::cout << "init ok" << endl;
	return hairRender;

}
ksu::PNdArrayF readRenderData(pRenderFrame pFbo, int num) {
	std::vector<ksu::PNdArray> allAttris;
	for (int i = 0; i < num; ++i) {
		auto tImg = pFbo->getRenderTextureCpu(i, true);
		ksu::PNdArrayF tArray_({(int)tImg->getHeight(),(int)tImg->getWidth(),4 });
		memcpy(tArray_.rawData(), tImg->getRawData(), tImg->getBufferSize());
		auto tArray = tArray_.flip(0);
		allAttris.push_back(tArray);
	}
	auto allRet = ksu::NdUtils::concat(allAttris);
	return allRet.asFloat();
}
void renderVideos(pRenderFrame pFbo, pScene scene) {
	auto camera = scene->getCamera();
	std::string camPosPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\renderRet\cam_pos.json)";
	std::string saveDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\renderRet\allAttris\)";
	auto jsonStr = Utils::readFileString(camPosPath);
	auto allCamPos = ksjson::parse(jsonStr);
	int a = allCamPos.size();
	for (int i = 0; i < allCamPos.size(); ++i) {
		auto tPos = allCamPos[i];
		camera->setViewParams({ tPos[0],tPos[1],tPos[2] }, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
		pFbo->applyMulti(3);
		scene->render();
		pFbo->apply0();
		auto attris = readRenderData(pFbo, 3);
		auto fileName = Utils::makeStr("frame_%04d.npy", i);
		ksu::NdUtils::writeNpy(saveDir + fileName, attris);
	}
}
int main(int argc,char* argv[]){
    cout<<"hello world"<<endl;
//	auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00009.data)";
	auto hairPath = R"(D:\Work\Projects\ProNew\render_research\tmp\simple_hair_5000_500.data)";

	auto vs = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\shaders\hair\hair.vert)";
	auto ps = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\shaders\hair\hair.frag)";
	auto texPath = R"(D:\TMPDATA\DataSwaps\jinlong_fov_50.png)";
	auto quadPath = R"(D:\Work\Projects\ProNew\Vray\testCodes\shaders\objs\quad.obj)";

	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(512, 512);

	auto scene = win->getScene();

	auto fbo = P3DEngine::instance().createRenderFrame(512, 512, 1);

	fbo->addRenderTextures(3);
	//	scene->addComp(quad, shader);
	auto modelMat = Mat4f::getTranslate({ 0.0f,-1.55f,0.0f });
	//auto modelRot = Mat4f::getRotY(45.0f);
	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_ORTH);
	camera->setOrthParams(0.6, 0.6, -100.0, 100.0);

	auto srcHair = std::make_shared<HairModel>();
	srcHair->loadFromFile(hairPath);
	srcHair->setModelMat(modelMat);
	auto tShader = P3DEngine::instance().createShader();
	tShader->loadFromFile(vs, ps);

	//testCuda(argc, argv,retTex);
	//testCudnn(argc, argv, retTex);
	auto cnnHair = testHair(argc, argv);

	auto bigHairModel = cnnHair->bigHairGpu;
	bigHairModel->setModelMat(modelMat);
	scene->addUserGeoGPU(bigHairModel);
//	scene->addHair(srcHair,tShader);
	camera->setViewParams({ -0.360796,0.888975,0.267063 },  { 0.0,0.0,0.0 }, {0.0,1.0,0.0});
	fbo->applyMulti(3);
	//fbo->apply();
	ksu::PNdArrayF tmpArray({ 512,512,4});
	scene->render();
	fbo->apply0();
	auto tCPU = fbo->getRenderTextureCpu(0, true);
	//renderVideos(fbo, scene);
	//return 0;
	std::vector<ksu::PNdArray> allAttris;
	std::string  dst_dir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\renderRet\)";
	for (int i = 0; i < 3; i++) {
		tCPU = fbo->getRenderTextureCpu(i, true);
		ksu::PNdArrayF tmpArray_({ int(tCPU->height),int(tCPU->width),4 });
		memcpy(tmpArray_.rawData(), tCPU->getRawData(), tCPU->getBufferSize());
		tmpArray = tmpArray_.flip(0);
		allAttris.push_back(tmpArray);
	}
	auto bigRet = ksu::NdUtils::concat(allAttris);
	ksu::NdUtils::writeNpy(dst_dir + R"(\all_attris.npy)", bigRet);
	return 0;

//	auto cvImg4 = cv::Mat(tCPU->width, tCPU->height, tCPU->nchannel==3 ? CV_32FC3:CV_32FC4, tCPU->getRawData());
//	cnnHair->setInputTexs({ fbo->getRenderTexture(0),fbo->getRenderTexture(2),fbo->getRenderTexture(3),fbo->getRenderTexture(4) });

//	auto retTensor = cnnHair->forward();
//	auto retTensor = cnnHair->srcInputs[0];
	//retTensor->transform(false);
//	auto cpuTensor = retTensor->dump();
	auto cvImg4 = cv::Mat(tCPU->height,tCPU->width, CV_32FC4, tmpArray.rawData());
	cv::Mat useMat(tCPU->height,tCPU->width,CV_8U);
	for (auto r = 0; r < tCPU->height; ++r) {
		for (auto c = 0; c < tCPU->width; ++c) {
			auto tmp = cvImg4.at<cv::Vec4f>(r, c);
			useMat.at<uint8>(r, c) = (uint8)((tmp[3]) * 255);
		}
	}
	
	cv::imwrite("test_multi.png", useMat);
	return 0;
	while (true) {
		win->render();
	}
	//render->exportScene(R"(tmp\scene_1.vrscene)");
	
	return 0;
}
