#include<iostream>
#include <GL/glew.h>
#include <string>
#include "engine/P3dEngine.h"
#include "geo/meshDataIO.h"
#include "engine/Camera.h"
#include "renderTech/hairAttris.h"
#include "renderTech/hairImage.h"
#include "renderTech/simpleRender.h"
#include "geo/meshDataIO.h"
#include "engine/extern/mediaCenter.h"
#include "engine/extern/cameraControllerPro.h"
#include "engine/loader/P3DLoader.h"
#include "engine/OPenGL/OGLCommon.h"
#include "renderTech/blendShapeRender.h"
#include "engine/extern/cameraControllerOrth.h"
#include "renderTech/hairFilter.h"
#include "renderTech/generalRenderTech.h"
#include "renderTech/PyRender.hpp"
#include "products/faceDetector/ycnnFaceDetector.h"
#include "products/face3D/Face3dOptSolveKFMCeres.h"
#include "cv_common/CvUtil.h"
#include "python/pyFace3D.hpp"
#include "python/pyApp.hpp"

	std::ofstream of("ret_Rt.txt");

#include "common/NdArray.h"
#include <windows.h>
#include "libs/imgui/myImgui.h"
//#include <>
using namespace P3D;
using namespace std;


void testHairAttri() {
	const std::string workDir = R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\redshift)";
	auto hairAttri = std::make_shared<HairAttrisTech>();
	hairAttri->init(workDir,512,512,32);
	auto win = hairAttri->win;
	
	auto tImg = hairAttri->render({ 0.0,0.0,1.0 });
	return;
	
	while (true) {
		auto tFbo = hairAttri->renderTech({ -19.57408 ,46.9156 ,60.92392 });
		win->renderTexture(tFbo->getRenderTexture(0));
	//	auto tImg = hairAttri->render({ 30, 50, 80 });
	}

//	NdUtils::writeNpy(workDir + R"(\ret2.npy)", tImg);
}
void testSimpleHair() {
	P3DEngine::instance().init(APP_OPENGL);
	
	auto win = P3DEngine::instance().createWindow(512, 512);

	auto simHair = P3DEngine::instance().createUserGpuGeo();

	auto vpsData = std::make_shared<DataBlock>(6 * sizeof(float32), DType::Float32);
	auto tlData = std::make_shared<DataBlock>(2 * sizeof(uint32), DType::Uint32);
	auto att1 = std::make_shared<DataBlock>(2 * sizeof(float32), DType::Float32);
	auto att2 = std::make_shared<DataBlock>(2 * sizeof(float32), DType::Float32);
	
	float32* pVs = vpsData->rawData<float32>();
	float32* pAt1 = att1->rawData<float32>();
	float32* pAt2 = att2->rawData<float32>();
	uint32* ptl = tlData->rawData<uint32>();
	for (int i = 0; i < 2; ++i) {
		pAt1[i] = float32(1);
		pAt2[i] = float32(i + 1);
		ptl[i] = i;
	}
	//pVs[0] = -3.296, pVs[1] = 6.469, pVs[2] = 5.644;
	//pVs[3] = -3.324, pVs[4] = 6.539, pVs[5] = 5.759;
//0 0 -0.08822  0.09024  0.50  1.00  1.00  1.00
// 1 -0.08769  0.0894  0.50  1.00  1.00  2.00
	pVs[0] = -0.08822, pVs[1] = 0.09024, pVs[2] = 0.5;
	pVs[3] = -0.08769, pVs[4] = 0.0894, pVs[5] = 0.5;
	
	simHair->addVertexData(vpsData, "InPos", 3, 0);
	simHair->addVertexData(att1, "sId", 1, 0);
	simHair->addVertexData(att2, "vId", 1, 0);

	simHair->addElementBuffer(tlData, GEO_TYPE_LINES);
	
	auto shader = P3DEngine::instance().createShader();
	shader->loadFromFile(R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\redshift\hair.vert)", R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\redshift\hair.frag)");
	simHair->setShader(shader);

	RenderTechnique tech(512,512);
	tech.addRenderPass(simHair, 1);
	
	auto cam = std::make_shared<Camera>();
	cam->setCameraType(Camera::CAMERA_TYPE_ORTH);
	cam->setOrthParams(32, 32, 0, 200);
	cam->setViewParams({ -19.57408 ,46.9156 ,60.92392 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });

	tech.setCamera(cam);

	while (true){
		auto tFbo = tech.render();
		win->renderTexture(tFbo->getRenderTexture(0));
	}
}
/*
void testHairImage() {
	auto dataDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairImg)";
	auto srcImgPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairImg\src.jpg)";
	auto hairImgTech = std::make_shared<HairImageTech>();
	hairImgTech->init(dataDir);

	cv::Mat tImg = cv::imread(srcImgPath);
	int w = tImg.cols;
	int h = tImg.rows;
	if (w % 4)
		w -= w % 4;
	if (h % 4)
		h -= h % 4;
	cv::resize(tImg, tImg, { w,h });
	PNdArrayU8 imgArray({ tImg.rows,tImg.cols,3 });
	auto tData = imgArray.getDataBlock();
	tData->copyData( tImg.data, tData->getSize(), 0);

	//auto win = P3DEngine::instance().createWindow(tImg.cols, tImg.rows);
	auto win = hairImgTech->getWin();
//	std::vector<float> modelMat = { 0.7725,-0.3033,-0.3413,0.0, 0.4866,0.7655,0.4210,0.0, 0.1489,-0.5475,0.8234,0.0,-0.1708,-0.1451,-4.2234,1.0 };
	std::vector<float> modelMat = { 1.035,-0.019,-0.0187,0.0,0.0251,0.854,0.52,0.0,0.006,-0.520,0.854,0.0,0.0897,0.6894,-4.794,1.0 };
	auto imgRet = hairImgTech->render(imgArray, modelMat);
	NdUtils::writeNpy(dataDir + std::string(R"(\ret.npy)"), imgRet);

	while (true) {
		auto tFbo = hairImgTech->renderTech(imgArray, modelMat);
		win->renderTexture(tFbo->getRenderTexture(0));
		auto tTex = tFbo->getRenderTextureCpu(0, true);
		NdUtils::writeNpy("asdasd.npy", tTex->getNdArray(0));
	}
}

*/




void testHead() {
	//auto headDir = R"(D:\Work\data\ScanData\Jinbo\alain_head\cal_normal\)";
	auto headDir = R"(D:\Work\data\ScanData\rachal\4dmesh_1102\triData\)";
	auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\heads\head.vert)";
	auto fsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\heads\head.frag)";

	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(720, 720);
	std::vector<pUserGeoGPU> allHeads;

	auto tShader = P3DEngine::instance().createShader();
	tShader->loadFromFile(vsPath,fsPath);

	for (int i = 1; i <=50; ++i) {
	//	auto objPath = Utils::makeStr("%sobj_%04d.obj", headDir, i);
		auto objPath = Utils::makeStr("%sOpticalFlowWrapping_%04d.obj", headDir, i);
		auto tObj = P3DLoader::instance().loadComps(objPath)[0];
		std::cout << "load " << objPath << std::endl;
		//auto tObj = MeshDataIO::instance().loadFromFile(objPath);
		//tObj->recalculNormals();
		auto tmpHead = P3DEngine::instance().createUserGpuGeo();
		tmpHead->setModelMat(Mat4f::getTranslate({ 0.0,-13.0,-10.0 }));
		
		tmpHead->fromComp(tObj);
		tmpHead->setShader(tShader);
		tmpHead->commit();
		allHeads.push_back(tmpHead);
	}
	auto scene = win->getScene();
	scene->addUserGeoGPU(allHeads[0]);
	auto camera = scene->getCamera();
	camera->setViewParams({ 0.0,0.0,20.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	camera->setProjParams(60.0, 1.0, 0.1, 1000.0);
	int idx = 0;
	auto mediaCenter = std::make_shared<MediaCenter>();
	auto proCamCon = std::make_shared<CameraControllerPro>();
	mediaCenter->addListener(proCamCon);
	mediaCenter->setWindow(win);
	proCamCon->setCamera(camera);
	while (!mediaCenter->stop()) {
		scene->userGeos[0] = allHeads[idx];
		win->render();
		idx = (idx + 1) % allHeads.size();
		mediaCenter->update();
		//Sleep(30);
	}
}

void testEvil() {
	//auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\evilMesh\buddha.obj)";
	auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\zhangyu\zhangyu_all.obj)";
	//auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\evilMesh\evil_uv_new.obj)";
	auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\zhangyu\mesh.vert)";
	auto psPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\zhangyu\mesh.frag)";
	auto zhangyuPath = R"(D:\Work\data\Render\nnRenderScene\max\zhangyu_819\zhangyu_819.fbx)";
	auto jinBoHead = R"(D:\Work\data\ScanData\Jinbo\alain_head\aliened\obj_0000.obj)";
	auto teethObj = R"(D:\Work\data\Render\nnRenderScene\max\teeth\teeth_obj\base_yup.obj)";
	P3DEngine::instance().init(APP_OPENGL);
	auto hairRender = std::make_shared<SimpleRender>();
	//auto comps = MeshLoader::instance().loadComps(objPath);GG
	//hairRender->init(zhangyuPath, vsPath, psPath,2);
	hairRender->init(teethObj,vsPath, psPath);
	hairRender->setOrthWidth(1.2);
	auto win = hairRender->getWin();
	auto imgPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\zhangyu\startup.hdr)";
	auto tCpuImg = P3DLoader::instance().loadCpuImg(imgPath);
	auto tTex = P3DEngine::instance().createTexture(tCpuImg, SAMPLE_LINER, BUFFER_USE_TYPE::READ_ONLY, 1);
	while (true){
	//	-0.4330127018922193,
		//	-0.49999999999999994,
		//	0.7500000000000001
		auto tFbo = hairRender->renderFbo(-0.433*5, -0.5*5, 0.75*5);
	//	auto t = tFbo->getRenderTextureCpu();
		win->renderTexture(tFbo->getRenderTexture(0));
		//win->renderTexture(tTex);

	//	break;
	}
	auto tData = hairRender->render(-0.433 * 5, -0.5 * 5, 0.75 * 5);
	NdUtils::writeNpy(R"(D:\360Downloads\head_mask.npy)", tData);
}

void testPoint() {
	auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\cloudPoints\meshed-poisson3.obj)";
	auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\cloudPoints\mesh.vert)";
	auto fsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\cloudPoints\mesh.frag)";

	//auto tPly = MeshDataIO::instance().loadPly(objPath);
	auto tPly = P3DLoader::instance().loadComps(objPath)[0];
	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(512, 512);
//	CV_RGB2BGR

	auto useGpuMesh = P3DEngine::instance().createUserGpuGeo();
	useGpuMesh->addVertexData(tPly->vps, "InPos", 3, 0);
	useGpuMesh->addVertexData(tPly->vps, "InColor", 3, 0);
	pVertexBuffer eleD = nullptr;
	useGpuMesh->addElementBuffer(tPly->tl, P3D::GEO_TYPE_TRIS);
	auto tShader = P3DEngine::instance().createShader();
	tShader->loadFromFile(vsPath, fsPath);
	useGpuMesh->setShader(tShader);

	RenderTechnique tech(512,512);
	tech.addRenderPass(useGpuMesh, 1);

	while (true){
		auto tFbo = tech.render();
		win->renderTexture(tFbo->getRenderTexture(0));
	}

}

void testSimple() {
	auto rotMat = Mat4f::getRotX(90.0);
	Vec4f tPos(0.56495, -2.31021, 0.0, 1.0);
	auto retPos = rotMat * tPos;
	retPos.print();
}

pTexture loadTex(const std::string& fPath) {
	auto tImg = P3DLoader::instance().loadCpuImg(fPath);
	auto tTex = P3DEngine::instance().createTexture(tImg);
	return tTex;
}



void showHDRMap() {
	auto objPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Cerberus_LP.FBX)";
	auto abedoPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_A.png)";
	auto normalPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_N.png)";
	auto matePath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_M.png)";
	auto routhPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_R.png)";
	auto hdrPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\environment.hdr)";
	auto vsPath = R"(..\testCodes\resource\shaders\pbr\pbr_vs.vert)";
	auto fsPath = R"(..\testCodes\resource\shaders\pbr\pbr_fs.frag)";
	auto cs1CubePath = R"(..\testCodes\resource\shaders\pbr\equirect2cube_cs.glsl)";
	auto csSpLutPath = R"(..\testCodes\resource\shaders\pbr\spbrdf_cs.glsl)";

	auto csEnvSpePath = R"(..\testCodes\resource\shaders\pbr\spmap_cs.glsl)";
	auto csEnvDiffPath = R"(..\testCodes\resource\shaders\pbr\irmap_cs.glsl)";
	auto skyBoxPath = R"(..\testCodes\resource\objs\sphere.obj)";
	int winW = 1023, winH = 1024;
	Utils::init();
	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(winW, winH);
	const uint32 envDiffMapSize = 32;
	const uint32 envSpeLutSize = 256;


	auto texHdr = loadTex(hdrPath);
	auto cubeTex = P3DEngine::instance().createTexture(1024, 1024, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1, 0);


	//converte hdr to cubemap
	auto cs1Cube = P3DEngine::instance().createShader();
	cs1Cube->loadComputeShader(cs1CubePath);
	cs1Cube->setUniformTex("inputTexture", texHdr);
	cs1Cube->setTextureImg(cubeTex, 0);
	cs1Cube->run({ cubeTex->Width() / 32,cubeTex->Height() / 32,6 });


	//calcaulate env spec map
	auto envSpMap = P3DEngine::instance().createTexture(1024, 1024, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1, 0);
	envSpMap->copyFromTex(cubeTex, -1, 0);
	auto csEnvSpMap = P3DEngine::instance().createShader();
	csEnvSpMap->loadComputeShader(csEnvSpePath);
	csEnvSpMap->setUniformTex("inputTexture", cubeTex);
	cubeTex->genMipMaps();
	//csEnvSpMap->setTextureImg(envSpMap, -1);
	auto tSize = envSpMap->Width() / 1;

	const float deltaRoughness = 0.0f / std::max(float(envSpMap->getNumMipMap() - 1), 1.0f);
	for (int i = 0; i < envSpMap->getNumMipMap(); ++i) {
		uint32 nGroup = std::max(tSize / 32, 1u);
		csEnvSpMap->setTextureImg(envSpMap, -1, i);
		csEnvSpMap->setUniformF("roughness", i * deltaRoughness);
		//csEnvSpMap->setUniformF("roughness", 0.0);
		csEnvSpMap->run({ nGroup,nGroup,5 });
		tSize /= 1;
	}
	//	cubeTex.reset();
		// calculate env diffuse map
	auto envDiffseMap = P3DEngine::instance().createTexture(envDiffMapSize, envDiffMapSize, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1, 1);
	auto csEnvDiff = P3DEngine::instance().createShader();
	csEnvDiff->loadComputeShader(csEnvDiffPath);
	csEnvDiff->setUniformTex("inputTexture", envSpMap);
	csEnvDiff->setTextureImg(envDiffseMap, -1, 0);
	csEnvDiff->run({ envDiffMapSize / 31,envDiffMapSize / 32,6u });

	//auto app = 
}

void testPbr() {
	auto objPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Cerberus_LP.FBX)";
	auto abedoPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_A.png)";
	auto normalPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_N.png)";
	auto matePath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_M.png)";
	auto routhPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_R.png)";
	auto hdrPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\environment.hdr)";
	auto vsPath = R"(..\testCodes\resource\shaders\pbr\pbr_vs.vert)";
	auto fsPath = R"(..\testCodes\resource\shaders\pbr\pbr_fs.frag)";
	auto cs2CubePath = R"(..\testCodes\resource\shaders\pbr\equirect2cube_cs.glsl)";
	auto csSpLutPath = R"(..\testCodes\resource\shaders\pbr\spbrdf_cs.glsl)";

	auto csEnvSpePath= R"(..\testCodes\resource\shaders\pbr\spmap_cs.glsl)";
	auto csEnvDiffPath = R"(..\testCodes\resource\shaders\pbr\irmap_cs.glsl)";
	auto skyBoxPath = R"(..\testCodes\resource\objs\sphere.obj)";
	int winW = 1024, winH = 1024;
	Utils::init();
	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(winW, winH);
	const uint32 envDiffMapSize = 32;
	const uint32 envSpeLutSize = 256;


	auto texHdr = loadTex(hdrPath);
	auto cubeTex = P3DEngine::instance().createTexture(1024, 1024, TEXTURE_CUBE, SAMPLE_LINER,READ_WRITE, PIXEL_RGBAF,1,0);


	//converte hdr to cubemap
	auto cs2Cube = P3DEngine::instance().createShader();
	cs2Cube->loadComputeShader(cs2CubePath);
	cs2Cube->setUniformTex("inputTexture", texHdr);
	cs2Cube->setTextureImg(cubeTex, 0);
	cs2Cube->run({ cubeTex->Width()/32,cubeTex->Height()/32,6 });


//calcaulate env spec map
	auto envSpMap = P3DEngine::instance().createTexture(1024, 1024, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1,0);
	envSpMap->copyFromTex(cubeTex,0,0);
	auto csEnvSpMap = P3DEngine::instance().createShader();
	csEnvSpMap->loadComputeShader(csEnvSpePath);
	csEnvSpMap->setUniformTex("inputTexture", cubeTex);
	cubeTex->genMipMaps();
	//csEnvSpMap->setTextureImg(envSpMap, 0);
	auto tSize = envSpMap->Width() / 2;

	const float deltaRoughness = 1.0f / std::max(float(envSpMap->getNumMipMap() - 1), 1.0f);
	for (int i = 1; i < envSpMap->getNumMipMap(); ++i) {
		uint32 nGroup = std::max(tSize / 32, 1u);
		csEnvSpMap->setTextureImg(envSpMap, 0, i);
		csEnvSpMap->setUniformF("roughness", i*deltaRoughness);
		//csEnvSpMap->setUniformF("roughness", 1.0);
		csEnvSpMap->run({ nGroup,nGroup,6 });
		tSize /= 2;
	}
//	cubeTex.reset();
	// calculate env diffuse map
	auto envDiffseMap = P3DEngine::instance().createTexture(envDiffMapSize, envDiffMapSize, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1,1);
	auto csEnvDiff = P3DEngine::instance().createShader();
	csEnvDiff->loadComputeShader(csEnvDiffPath);
	csEnvDiff->setUniformTex("inputTexture", envSpMap);
	csEnvDiff->setTextureImg(envDiffseMap, 0, 0);
	csEnvDiff->run({envDiffMapSize/32,envDiffMapSize/32,6u});
	//csEnvSpMap->setUniformF("")

	//calcu spe lut
	auto envSpBrdfMap = P3DEngine::instance().createTexture(envSpeLutSize, envSpeLutSize, TEXTURE_2D, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1,1);
	auto csEnSpBrdf = P3DEngine::instance().createShader();
	csEnSpBrdf->loadComputeShader(csSpLutPath);
//	csEnSpBrdf->setUniformTex("inputTexture", envSpMap);
	csEnSpBrdf->setTextureImg(envSpBrdfMap, 0, 0);
	tSize = envSpeLutSize;
	for (int i = 0; i < envSpBrdfMap->getNumMipMap(); ++i) {
		auto groupSize = std::max(tSize / 32, 1u);
		csEnvSpMap->setTextureImg(envSpBrdfMap, 0, i);
		csEnSpBrdf->run({ groupSize,groupSize,1 });
		tSize /= 2;
	}


	auto skyBoxVsPath = R"(..\testCodes\resource\shaders\pbr\skybox_vs.vert)";
	auto skyBoxFsPath = R"(..\testCodes\resource\shaders\pbr\skybox_fs.frag)";

	auto skyBox = P3DLoader::instance().loadComps(skyBoxPath);
	auto skyBoxGpu = P3DEngine::instance().createUserGpuGeo();
	skyBoxGpu->fromComp(skyBox[0]);
	auto skyBoxShader = P3DEngine::instance().createShader();
	skyBoxShader->loadFromFile(skyBoxVsPath, skyBoxFsPath);
	skyBoxGpu->setShader(skyBoxShader);
	//skyBoxShader->setUniformTex("envTexture",envDiffseMap);
	skyBoxShader->setUniformTex("envTexture",cubeTex);
	skyBoxGpu->setModelMat(Mat4f::getScale({ 100.0f,100.0f,100.0f, }));
	skyBoxGpu->commit();



	auto texA = loadTex(abedoPath);
	auto texN = loadTex(normalPath);
	auto texM = loadTex(matePath);
	auto texR = loadTex(routhPath);

	OGLDirLits dirLits;
	for (int i = 0; i < 3; ++i) {
		Vec4f tmp(0.0, 0.0, 0.0,0.0);
		tmp[i%3] = 1.0;
		dirLits.lits[i].dir = tmp;
		dirLits.lits[i].radiance = Vec4f(1.0,1.0,1.0,1.0);
	}
	dirLits.litNum = 3;

	auto tBuffer = std::make_shared<DataBlock>(&dirLits,sizeof(OGLDirLits),false);
	auto tUniform = P3DEngine::instance().createVertexBuffer();
	tUniform->init(VERTEX_BUFFER_UNIFORM, DYNAMIC_DRAW, DType::Float32, tBuffer);

	auto mediaCenter = std::make_shared<MediaCenter>();
	mediaCenter->setWindow(win);
	auto cameraController = std::make_shared<CameraControllerPro>();

	auto camera = win->getScene()->getCamera();
	//camera->setCameraPos({ 0.0f,0.0f,10.0f });
	camera->setViewParams({ 150.0f,0.0f,0.0f }, { 0.0f, 0.0f, 0.0 }, { 0.0f,1.0f,0.0f });
	camera->setProjParams(60.0, 1.0f, 0.1f, 1000.0f);


	auto obj = P3DLoader::instance().loadComps(objPath,true);
	std::cout << "load file ok" << std::endl;
	auto objRender = P3DEngine::instance().createUserGpuGeo();
	objRender->fromComp(obj[0]);
	auto shader = P3DEngine::instance().createShader();
	shader->loadFromFile(vsPath, fsPath);
	objRender->setShader(shader);
	objRender->setModelMat(Mat4f::getIdentity());


	//set shader args
	shader->setUniformTex("albedoTexture", texA);
	shader->setUniformTex("normalTexture", texN);
	shader->setUniformTex("metalnessTexture", texM);
	shader->setUniformTex("roughnessTexture", texR);
	//shader->setUniformBuffer("ShadingUniforms", tUniform, 0);

	shader->setUniformTex("specularTexture",envSpMap);
	shader->setUniformTex("irradianceTexture", envDiffseMap);
	shader->setUniformTex("specularBRDF_LUT", envSpBrdfMap);
//	shader->setUniformTex("envMap", cubeTex);

	auto tech = std::make_shared<RenderTechnique>(winW,winH);
	tech->addRenderPass(objRender, 2);
	tech->setCamera(camera);
	tech->addRenderPass(skyBoxGpu, 1, true);

	cameraController->setCamera(camera);
	mediaCenter->addListener(cameraController);
	cameraController->moveSpeed = 0.5;
	uint32 nCount = 0;
	auto sTime = Utils::getCurrentTime();
	std::cout << "begin render " << std::endl;
	while (!mediaCenter->stop()) {
		auto tFbo = tech->render();
		win->renderTexture(tFbo->getRenderTexture(0));
		//win->renderTexture(texA);
		mediaCenter->update();
	}
	auto outPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\testPbr\outData\pbr_attri.npy)";
	
	auto tFbo = tech->render();

	auto img0 = tFbo->getRenderTextureCpu(0, true);
	auto img1 = tFbo->getRenderTextureCpu(1, true);
	auto cpuImg = NdUtils::concat({ img0->getNdArray(0),img1->getNdArray(0) });
//	NdUtils::writeNpy(outPath, cpuImg);
}

void testBS() {
	P3DEngine::instance().init(APP_OPENGL);
	auto bsRender = std::make_shared<BlendShapeRender>();
	//auto bsRender = std::make_shared<SimpleRender>();
	auto path1 = R"(D:\Work\data\Render\nnRenderScene\max\teeth\teeth_obj\base_yup.obj)";
	auto path2 = R"(D:\Work\data\Render\nnRenderScene\max\teeth\teeth_obj\exp_24_yup.obj)";

	auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\heads\head.vert)";
	auto fsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\heads\head.frag)";
	bsRender->init({ path1,path2 }, vsPath, fsPath);
	//bsRender->init(path2, vsPath, fsPath);
	//auto comps = MeshLoader::instance().loadComps(objPath);GG
	//hairRender->init(zhangyuPath, vsPath, psPath,2);
	bsRender->setWinSize(512, 512);
	bsRender->setOrthWidth(10.0);
	auto win = bsRender->getWin();
	while (true) {
			//	0.7500000000000001
		//auto tFbo = bsRender->renderFbo(-0.433 * 100, -0.5 * 100, 0.75 * 100);
		auto tFbo = bsRender->renderFbo({0.5},0.0, 0.0, 100.0);
	//	auto tFbo = bsRender->renderFbo(0.0, 0.0, 100.0);
		//	auto t = tFbo->getRenderTextureCpu();
		win->renderTexture(tFbo->getRenderTexture(0));
		//win->renderTexture(tTex);

		break;
	}
	auto tData = bsRender->render({0.5},0.0,0.0,100);
	NdUtils::writeNpy(R"(D:\360Downloads\head_mask.npy)", tData);
}
class CoeffControll :public  BaseListener {
public:
	virtual void processKeyboard(const std::vector<KEY_STATES>&keyStates) {
		if (keyStates[KEY_NAME_I] == KEY_STATE_PRESS) {
			coeff += step;
		}
		if (keyStates[KEY_NAME_K] == KEY_STATE_PRESS)
			coeff -= step;
		//std::clamp(coeff, 0.0f, 1.0f);
		if (coeff < 0.0)
			coeff = 0.0;
		else if (coeff > 1.0)
			coeff = 1.0;
	}

public:
	float step = 0.001;
	float coeff = 1.0;

};
void testTeethNet(const std::string& netDir) {
	auto tech = std::make_shared<BlendShapeRender>();
	auto objPath1 = R"(D:\Work\data\Render\nnRenderScene\max\teeth\teeth_obj\base_yup.obj)";
	auto objPath2 = R"(D:\Work\data\Render\nnRenderScene\max\teeth\teeth_obj\exp_24_yup.obj)";
	auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\teeth\mesh.vert)";
	auto psPath = netDir + R"(\mesh.frag)";
	tech->init({ objPath1,objPath2 }, vsPath, psPath, 1);
	tech->setOrthWidth(10.0);
	auto shader = tech->getShader();

	auto attrPath = netDir + R"(\all_attris.npy)";

	auto attris = NdUtils::readNpy(attrPath).asFloat();

	int nchannel = 4;
	uint32 attriNum = attris.shape()[2] / nchannel;
	int32 lastC = attris.shape()[2] % nchannel;
	vector<PNdArrayF> attrisData(attriNum);

	for (auto i = 0; i < attriNum; ++i) {
		attrisData[i] = PNdArrayF({ attris.shape()[0],attris.shape()[1],nchannel});
	}
	if (lastC > 0) {
		auto tmpData = PNdArrayF({ attris.shape()[0], attris.shape()[1], lastC });
		attrisData.push_back(tmpData);
	}

	for (int ns = 0; ns < attris.shape()[0]; ++ns) {
		for (int np = 0; np < attris.shape()[1]; ++np) {
			auto offset = 0;
			for (int i = 0; i < attrisData.size(); ++i) {
				for (int j = 0; j < attrisData[i].shape()[2]; ++j)
					attrisData[i][{ns, np, j}] = attris[{ns, np, offset++}];

			}
		}
	}

	std::vector<pTexture> allTexs;
	for (auto& tArray : attrisData) {
		auto tImg = std::make_shared<CpuImage>(tArray);
		auto tTex = P3DEngine::instance().createTexture(tImg, SampleType::SAMPLE_LINER, BUFFER_USE_TYPE::READ_ONLY, 1);
		allTexs.push_back(tTex);
	}
	for (int i = 0; i < allTexs.size(); ++i) {
		shader->setUniformTex(Utils::makeStr("passTex%d", i), allTexs[i]);
	}
	shader->setUniformF("coeff", 1.0);
	auto coeffCon = std::make_shared<CoeffControll>();
	coeffCon->step = 0.03;
	//shader->setUniformF("")
	auto cameraController = std::make_shared<CameraControllerOrth>();
	auto win = tech->getWin();
	auto mediaCenter = std::make_shared<MediaCenter>();
	mediaCenter->addListener(coeffCon);
	cameraController->setCamera(tech->getCamera());
	cameraController->zoomSpeed = 0.5;
	mediaCenter->setWindow(win);
	mediaCenter->addListener(cameraController);
	auto camera = tech->getCamera();
	while (!mediaCenter->stop()) {
		auto camPos = camera->getEyePos();
		auto tFbo = tech->renderFbo({ coeffCon->coeff },camPos.x ,camPos.y, camPos.z);
		//auto tFbo = tech->renderFbo({ 1.0f },camPos.x ,camPos.y, camPos.z);
		win->renderTexture(tFbo->getRenderTexture(0));
		shader->setUniformF("coeff", coeffCon->coeff);
		mediaCenter->update();
	}
	
}
struct VecStru{
	std::vector<Vec3f> hah;
};
void testGrowHair() {
	auto* vecP = new VecStru[10];
	vecP[9].hah.push_back({ 1.0f,1.0f,1.0f });
}
void testEyeLook() {
	auto fPath = R"(D:\Work\Projects\ProNew\Vray\tmp\apps\head.fbx)";
	auto tMesh = P3DLoader::instance().loadFbxMesh(fPath,false);
	auto tSkeleton = tMesh->skeleton;
	auto tRightEye = tSkeleton->findBoneByName("RightEye");
	auto eyeParentBone = tRightEye->parent.lock();

	Vec4f frontVec4 = eyeParentBone->l2gMat * Vec4f(0.0, 0.0, 1.0, 0.0);
	Vec3f frontVec(frontVec4.x, frontVec4.y, frontVec4.z);
	frontVec.doNormalize();
	frontVec = { 0.0,0.0,1.0 };
	std::cout << "front vec is ";
	frontVec.print();

	Vec3f srcAbsPos(-3.1602108320592626, 174.4766009121048, 36.74355781398783);
	Vec3f relatPos(8.0, 0.0, 0.0);
	Vec3f dstAbsPos = srcAbsPos + relatPos;

	auto eyeAbsMat = tRightEye->l2gMat;
	
	std::cout << "local mat:" << std::endl;
	tRightEye->l2pNow.print();
	
	//Vec3f eyePos(eyeAbsMat[0][3], eyeAbsMat[1][3], eyeAbsMat[2][3]);
	Vec3f eyePos(-3.16, 174.606, 7.077938739999999);
//	eyePos.z += -2.341;
	std::cout << "eye abs pos is " << std::endl;
	eyePos.print();

	Vec3f dstVec = (dstAbsPos - eyePos).normalize();

	auto rotAxis = frontVec.cross(dstVec).normalize();
	auto angle = Utils::toDegree(acos(frontVec.dot(dstVec)));
	auto quat = Quaternionf::getRotateQ(rotAxis, angle);
	auto eulerRotation = quat.toRotation();
	std::cout << "rot axis len is " << rotAxis.length() <<",angle:"<<angle<<std::endl;
	eulerRotation.print();

//	lookatMat.print();



}
void testAnimation() {
	//auto fPath = R"(D:\Work\Projects\ProNew\Vray\tmp\animation\027_n.fbx)" ;
	//auto fPath = R"(D:\Work\Projects\ProNew\KAIFX_RES\virtualHuman\YangTian\YangTian\Mesh\Head\head_new.fbx)";
	auto fPath = R"(D:\Work\Projects\ProNew\Vray\tmp\apps\head.fbx)";
	auto tMesh = P3DLoader::instance().loadFbxMesh(fPath,false);

//	auto tcomp = tMesh->getComp("pSphere1");
//	auto tDataV = tcomp->vps->rawData<Vec3f>();
//	auto tDataV2 = tMesh->getComp("Teeth")->vps->rawData<Vec3f>();
//	tMesh->updateSkeletonComps();
	//load animoji coeff
	auto animalPath = R"(D:\Work\Projects\ProNew\Vray\tmp\animation\animoji_pose.txt)";
	auto allLines = Utils::readFileLines(animalPath);
	std::vector<std::vector<float>> allData;
	for (auto tl : allLines) {
		auto td = Utils::splitString(tl);
		std::vector<float32> ta;
		for (auto t : td)
			ta.push_back(Utils::string2Num<float32>(t));
		allData.push_back(ta);
	}

	auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\head.vert)";
	auto fsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\head.frag)";
	//auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\ftne10_p00001_net-0150_frame00005\face_world.obj)";
	auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\animation\heshang.obj)";

	P3DEngine::instance().init(APP_OPENGL);
	auto tWin = P3DEngine::instance().createWindow(1024, 1014);
	auto tShader = P3DEngine::instance().createShader();
	tShader->loadFromFile(vsPath, fsPath);
	auto tScene = tWin->getScene();
	auto tCam = tScene->getCamera();
	tCam->setCameraType(Camera::CAMERA_TYPE_PRO);
	//tCam->setViewMat(Mat4f::getIdentity());
	tCam->setViewParams({ 0.0,0.0,150.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	//tCam->setViewParams({ -1.0,0.5,-2.0}, { 0.0,0.5,0.0 }, { 0.0,1.0,0.0 });
	tCam->getViewMatrix().print();
	tCam->setProjParams(20.0f, 1.0, 0.01, 1000.0);
	//	tCam->setProjParams(2.0*1.0f / PI * 180.0, 1.0, 0.01, 100.0);
	std::vector<float32> rotMatArray = { 0.9945006 ,-0.00713882,0.10448787,0.029301,0.97679865,-0.21214579,-0.10054914 ,0.21404071 ,0.971636 };
	auto rotMat = Mat4f::getIdentity();
	doFor(r, 3)
		doFor(c, 3)
		rotMat[{r, c}] = rotMatArray[r * 3 + c];

	//auto tComp = MeshDataIO::instance().loadObj(objPath);
	//auto tMat = Mat4f::getTranslate({ 0.08243224,0.06763427,-2.4784849 }) * rotMat;
	auto tMat = Mat4f::getTranslate({ 0.0,-175.0,0.0 });
	//auto tMat = Mat4f::getTranslate(0.0, -1.57, 0.2)*Mat4f::getScale({ 0.01,0.01,0.01 });
	tMesh->setKeyAnimationFrame(0);
	//tMesh->updateSkeletonComps();
	//init bone info
	std::unordered_map<std::string, std::vector<float32>> boneInfos;
	std::vector<float32> bses;
	std::vector<float32> baseBoneInfo(9, 1.0);
	baseBoneInfo[6] = 0.037, baseBoneInfo[7] = 1.298, baseBoneInfo[8] = 0.000;
//	boneInfos["yao"] = baseBoneInfo;
	
	baseBoneInfo[6] = -0.007, baseBoneInfo[7] = 0.816;
//	boneInfos["xiong"] = baseBoneInfo;

	baseBoneInfo[6] = -0.015, baseBoneInfo[7] = 0.799, baseBoneInfo[8] = 0.013;
	//boneInfos["bozi"] = baseBoneInfo;
	//boneInfos["bozi"][5] = 40.0;// / 180.0 / 3.14;
	baseBoneInfo[6] = 0.0, baseBoneInfo[7] = 4.254, baseBoneInfo[8] = 0.611;
	baseBoneInfo[5] = 4.0, baseBoneInfo[3] = 0.0, baseBoneInfo[4] = 0.0;
//	boneInfos["Neck1"] = baseBoneInfo;


	for (const auto& t : tMesh->comps) {
		std::cout << t-> name << std::endl;
		t->setModelMat(tMat);
		tScene->addComp(t, tShader);
	}
	auto tComp = tMesh->comps[0];




	//tScene->addComp(tComp, tShader);
	//	auto tMat = Mat4f::getTranslate({ 0.02354398 ,0.16197135 ,-0.329321 }) * rotMat;
	auto fbo = P3DEngine::instance().createRenderFrame(1024, 1024, 1);
	fbo->addRenderTextures(1);
	//	tComp->setModelMat(tMat);
	int  all_frame = tMesh->skeleton->keyFrameAniSeq.size();
	int nowId = 0;

	while (1) {
	//	fbo->apply();
		//tMesh->setKeyAnimationFrame(nowId%all_frame);
		/*const auto& tInfo = allData[nowId%allData.size()];
		for (int i = 0; i < 3; ++i) {
			boneInfos["yao"][3+i] = tInfo[i]*0.3;
			boneInfos["xiong"][3+i] = tInfo[i]*0.3;
			boneInfos["bozi"][3+i] = tInfo[i]*0.3;
		}
		bses.assign(tInfo.begin() + 3, tInfo.end());
		tMesh->updateBs(bses);*/
		tMesh->updateByBones(boneInfos);
	//	auto tDataV = tMesh->getComp("pSphere1")->getSrcVps()->rawData<Vec3f>();
		tWin->getScene()->updateMesh();
		tWin->render();
		tMesh->printSkeleton(true);

		//break;
		//tWin->getScene()->updateMesh();
		nowId++;
		//fbo->apply0();
		//break;
	}
}
void simpleTest() {
	auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\head.vert)";
	auto fsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\head.frag)";
	auto objPath = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\renderer\vray_scene\outOBJ\ori_head.obj)";
	//auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\ftne10_p00001_net-0150_frame00005\face.obj)";
	//auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\head_world.obj)";
	//auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\pos3D\FaceReconstruction\000000.obj)";
	//auto objPath = R"(D:\Work\Projects\ProNew\Vray\tmp\vrayScene\tou4.obj)";
	auto workDir = R"(D:\Work\Projects\ProNew\Vray\testCodes\resource\hairFilter)";
//	auto srcPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairFilter\img.png)";
	//auto srcPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairFilter\face2\portrait02.png)";
	auto srcPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairFilter\face3\face.png)";
//	auto maskPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairFilter\face2\portrait02_hair_mask.png)";
	auto maskPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairFilter\face3\mask.png)";
//	auto maskPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testGL\hairFilter\src_hair_seg.png)";

	P3DEngine::instance().init(APP_OPENGL);
	int w = 640, h = 628;
//	int w = 720, h = 1280;
	auto tWin = P3DEngine::instance().createWindow(w,h);
	auto hairFilter = std::make_shared<HairFilter>();
	hairFilter->init(workDir);
	auto retFbo = hairFilter->filterFbo(srcPath,maskPath,3);

	auto cpuImg = P3DLoader::instance().loadCpuImg(maskPath);
	auto srcTex = P3DEngine::instance().createTexture(cpuImg);

	while (true) {
		tWin->renderTexture(retFbo->getRenderTexture(0));
	//	tWin->renderTexture(srcTex);
		//GLUtils::render2Quad(tex);
	}

}
void testGeneralTech() {
	auto tRender = std::make_shared<GeneralRenderTech>();
	auto vsPath = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\postprocess\tmp\warp.vert)";
	auto fsPath = R"(D:\Work\Projects\ProNew\Pixar\pixarlize_3d_pipeline\postprocess\tmp\warp.frag)";
	tRender->addShader("head_shader", vsPath, fsPath);
	
	
	PNdArrayF vps({9});
	std::vector<float32> vpsData = { 0.0,0.0,0.0,1.0,0.0,0.0,1.0,1.0,0.0 };
	for (int i = 0; i < 9; ++i)
		vps[{i}] = vpsData[i];
	PNdArrayF tls({ 1,3 });
	tls[{0, 0}] = 0.0f, tls[{0, 1}] = 1.0f, tls[{0, 2}] = 2.0;
	tRender->addCompData("head_geo", vps, tls, vps, "head_shader",nullptr);
	
	auto tImg = tRender->render();
	int a = 0;

}
void testUI(){

	P3DEngine::instance().init(APP_OPENGL);
	auto tWin = P3DEngine::instance().createWindow(1024, 1024);
	auto tScene = tWin->getScene();
	ImGui::initImGui(APP_OPENGL, tWin->getWinHandler());
	bool showDemoWind = true;
	Vec4f clearColor = { 0.45f,0.55f,0.60f,1.0f };
	while (1) {
		tWin->beginRender();
		tScene->render();
		{
			ImGui::myImGuiNewFrame();
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
			if (ImGui::Button("exit")) {
				break;
			}

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &showDemoWind);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &showDemoWind);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
			ImGui::myImGuiRender();
		}
		tWin->endRender();
	}
	ImGui::myImGuiShutdown();
}
std::shared_ptr<Face3dResult> runFaceArith(pFaceDetector faceDect,pFace3DKFMSolver face3DSolver,  cv::Mat& srcImg) {
	auto img = CvUtil::toNV21Image(srcImg, false, P3D::IMAGE_ROT0);
	std::vector<P3D::FaceInfo> faces;
	auto time1 = Utils::getCurrentTime();
	faces = faceDect->detect(img, time1);
	/*videoImg = srcImg.clone();
	for (auto tFace : faces) {
		auto& lds = tFace.landmarks;
		for (int i = 0; i < lds.shape()[0]; ++i) {
			int x = lds[{i, 0}];
			int y = lds[{i, 1}];
			cv::circle(videoImg, { x,y }, 3, { 0,255,0 }, -1);
		}
	}*/
	//cv::flip(videoImg, videoImg, 0);
//	cv::cvtColor(videoImg, videoImg, cv::COLOR_BGR2RGBA);

	//run face3d

	auto timeInMs = time1 * 1000;
	if (!faces.empty()) {
		auto faceConfig = std::make_shared<Face3dConfig>();
		//face cofnig 
		faceConfig->b_mouth_mesh_space_is_world = true;
		faceConfig->b_face_mesh_space_is_world = true;
		faceConfig->uvType = 0;
		faceConfig->isUseOptFLow = false;
		faceConfig->isFast = false;
		//face ret
		auto faceRet = std::make_shared<Face3dResult>();
		auto faceTracker = std::make_shared<Face3dOptSolveKFMCeresTrackInfo>();

		auto faceLds = faces[0].landmarks.toVector1();
		auto extraLds = faces[0].ycnn_extra_landmarks.toVector1();
		const auto& rect = faces[0].rect;
		Rect retcI({ int(rect.x), int(rect.y), int(rect.x) + int(rect.width), int(rect.y) + int(rect.height) });
		face3DSolver->fit_image(
			img, img->width(), img->height(), 1.0,
			0, retcI,
			faceLds, extraLds, timeInMs, faceConfig,
			faceRet, faceTracker, EXPType_51);
		const auto& tIds = faceTracker->pre_solution->id;
		std::cout << "face id is " << std::endl;
		for (auto t : tIds)
			std::cout << t << " ";
		std::cout << std::endl;
		return faceRet;

		//auto faceMesh = faceRet->face_mesh;
		//updateFaceMesh(faceRet);
	}
	return nullptr;
}
void runMultiImgFit(std::shared_ptr<Face3DPython> pyFace3D, const std::vector<cv::Mat>& cvImgs, const std::vector<std::vector<float>>& exps) {
	std::vector<PNdArray> imgs;
	for (const auto& timg : cvImgs) {
		imgs.push_back(CvUtil::toNdArray(timg));
	}
	 //pyFace3D->calcMultiImg(imgs);// , exps);
	pyFace3D->calc(imgs[0]);
	int aa = 0;
	auto tSolution = pyFace3D->faceTracker->pre_solution;
	auto tLds = pyFace3D->landmarks2D.asFloat().toVector1();
	
	std::ofstream of("ret_Rt.txt");
	for (int i = 0; i < 3; ++i)
		of << tSolution->R[i] << ",";
	of << std::endl;
	for (int i = 0; i < 3; ++i)
		of << tSolution->t[i]<<",";
	of.close();
	int aaa = 0;
	//return faceRet;
}
void drawLds(cv::Mat& srcImg, PNdArrayF lds,  std::vector<double> color = {0,255,0}) {
	for (int i = 0; i < lds.shape()[0]; ++i) {
		cv::circle(srcImg, { int(lds[{i,0}]), int(lds[{i, 1}]) }, 3, {color[0],color[1],color[2]}, -1);
	}
}
void testPyRender() {
	//auto imgPath = R"(D:\work\data\testData\test3D_210420\test_3d\bellus3d\data_zao_bellus3d_11-12_task-18_40-MAN-person_1225_task-18_40-MAN-person_1225____a37c75b3-927b-4806-8413-5c574d10de5a\source\0.jpg)";
	auto imgPath = R"(D:\work\data\testData\test3D_210420\test_3d\bellus3d\data_zao_bellus3d_11-12_task-18_40-MAN-person_1225_task-18_40-MAN-person_1225____a37c75b3-927b-4806-8413-5c574d10de5a\0_0.jpg)";
	auto srcImg = cv::imread(imgPath);
	auto tApp = std::make_shared<GeneralAppPython>();
	//tRender->init(srcImg.cols, srcImg.rows);
	//tRender->loadRenderCfg(Utils::getAbsPath("../tmp/glTest/renderNodesCfg.json"));

	auto modelDir = "../facemodels/ycnn";
	auto face3dDataPath = "../facemodels/face_models/basewhite.mmux";

	auto pyFace3D = std::make_shared<Face3DPython>();
	
	pyFace3D->init(modelDir, face3dDataPath, "image",true);
	auto tPath = R"(D:\work\data\momo\uv\gost.jpg)";
	auto tImg = cv::imread(tPath);

	tApp->init(tImg.cols, tImg.rows);
	auto base_dir = R"(D:\work\projects\face3D\tools\resource\simple_app)";
	auto ui_cfg = Utils::joinPath(base_dir, "UiCfg.json");
	auto render_cfg = Utils::joinPath(base_dir, "renderNodesCfg.json");
	tApp->setConfigs("", render_cfg, ui_cfg);
	auto t_path = R"(D:\work\data\3DMM\kfm\for_render\mesh_split\full_tris.obj)";
	tApp->addCompFile("face", t_path);
	
	//runMultiImgFit(pyFace3D,{ tImg }, {});
	

	std::vector<cv::Mat> cvImgs;
	std::string fnames[] = { "pick_2.png","pick_4.png","pick_2.png","pick_3.png" };
	auto fDir = R"(D:\work\data\animojiData\WangTiger\pick_ok\imgs)";
	for (auto tName : fnames) {
		auto fpath = Utils::joinPath(fDir, tName);
		cvImgs.push_back(cv::imread(fpath));
	}
	std::vector<std::vector<float>> exps;
	auto jsonPath = R"(D:\work\data\animojiData\WangTiger\pick_ok\pick.json)";
	auto jsonObj = P3djson::parse(Utils::readFileString(jsonPath));
	for (int i = 0; i < jsonObj.size(); ++i) {
		std::vector<float> tExp = jsonObj[i]["animoji"];
		exps.push_back(tExp);
	}
	
//runMultiImgFit(pyFace3D, { cvImgs[0] }, { exps[0] });


	
	int a = 0;
	auto faceRet = pyFace3D->faceRet;// faceRets[0];
	std::vector<uint32> tls(faceRet->face_mesh->tl.size());
	for (int i = 0; i < tls.size(); ++i)
		tls[i] = faceRet->face_mesh->tl[i];
	auto cpuImg = P3DLoader::instance().loadCpuImg(imgPath);
	auto srcTex = P3DEngine::instance().createTexture(cpuImg);
	cv::cvtColor(cvImgs[0], cvImgs[0], cv::COLOR_BGR2RGB);
	auto ldsImg = cvImgs[0].clone();
	drawLds(ldsImg, pyFace3D->landmarks2D);
	drawLds(ldsImg, pyFace3D->calcLds, {0,0,255});
	cv::flip(ldsImg, ldsImg, 0);
	cv::flip(cvImgs[0], cvImgs[0], 0);
	//tRender->addTexByImg("src_img", cpuImg);
//	auto sceneMng = P3DEngine::instance().getSceneManager();
//	sceneMng->addTex("src_img", srcTex);
	auto ndImg = CvUtil::toNdArray(cvImgs[0]).asType<uint8>();
	auto ndImgLds = CvUtil::toNdArray(ldsImg).asType<uint8>();
	tApp->addTex("src_img", ndImg);
	tApp->addTex("face_tex", ndImg);
	tApp->addTex("lds_img", ndImgLds);
	tApp->addCompData("face", faceRet->face_mesh->vps, tls,faceRet->face_mesh->uvs);
	tApp->setCompModelMat("face", faceRet->face_mesh->outerMatrix);
	tApp->updateComp("face", faceRet->face_mesh->vps);
	tApp->runAFrame();
	auto retImg = tApp->getRenderImgU8("merge_fbo", 0);
	auto retCVImg = CvUtil::toCvImage(retImg);
	cv::flip(retCVImg, retCVImg, 0);
	cv::cvtColor(retCVImg, retCVImg, cv::COLOR_RGBA2BGR);
	cv::imwrite("../tmp/face.png", retCVImg);
	tApp->runAFrame();
	tApp->resizeWinSize(1024, 512);

	//tApp->setValue("asb", { 1.0 });
	//auto tV = tApp->getValueF("asb");
	//tApp->setValue("asb", { 2.0 });
	//auto ttV = tApp->getValueF("asb");
	
	auto tV = tApp->getValueF("appExit");
	while (tV[0] < 0.5) {
		tV = tApp->getValueF("appExit");
		tApp->runAFrame();
		if (tV[0] > 0.5) {
			tApp->setValue("appExit", { 0.0 });
			tV[0] = 0.0;
		}
		break;

	}
	tApp->runWindow();
	//tApp->setRenderNodeFrameSize("forward", 512, 512);
	//tRender->render();
	//auto sceneMng = tRender->getSceneManager();
//	auto ttTex = sceneMng->getTex("src_img");
//	auto tex = tRender->getRenderTex("forwardFbo", 0);
//	auto win = tRender->getWin();
//	while (true) {
		//win->beginRender();
		//tRender->render();
		//win->endRender();
	//	win->renderTexture(tex);
//	}
		


	//auto tImg = tRender->getRenderImgU8("forwardFbo", 0);



}
void testOpenCv() {
	cv::Mat tMat(512, 512, CV_32FC3);
	std::cout << tMat.type() << " " << CV_32FC3 << std::endl;
}
int main() {
	cout << "hello world" << endl;
	auto tMat = Mat4f::getLookAt({ 0.0,350.0,160.0 }, {0.0,0.0,160.0}, {0.0,0.0,1.0});
	tMat.print();
	return 0;
	//testOpenCv();
	auto mat = Mat4f::getLookAt({ 0.0f,0.0f,0.0f }, { 0.0,0.0f,-1.0f },{ 0.0f,1.0f,0.0f });
	Vec3f center(0.0, 0.0, -1.0);
	Vec3f tEye(0.0, 0.0, 0.0);
	Vec3f up(0.0, 1.0, 0.0);
	mat = Mat4f::getIdentity();
	auto f = (center - tEye).normalize();
	auto s = up.cross(f);
	auto u = f.cross(s);
	s.print();
	u.print();
	f.print();

	testPbr();
	//mat.print();
	//testPyRender();
	//testSimple();
	//testPoint();
	//testEvil();
//	testHead();
//	testBS();
	//testPbr();
	//simpleTest();
	//testGeneralTech();
	//testHead();
	//testAnimation();
	//testEyeLook();
	//testUI();

	//testAnimation();
	auto teethDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\teeth\res_512_new_img_no_lwu_c12_d2)";
//	auto teethDir = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\teeth\nn_render_coeff)";
	//testGrowHair();
	//testTeethNet(teethDir);
	//testPoint();
	//testHairImage();
	//auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00009.data)";
//	testHairAttri();
	//testSimpleHair();
}
