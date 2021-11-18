
#define  VRAY_RUNTIME_LOAD_PRIMARY
//#define VRAY_RUNTIME_LOAD_SECONDARY
#include "vrayUtils.h"
#include "vrayplugins.hpp"
#include <fstream>
#include<iostream>

#include "vrayRender.h"
#include "geo/meshDataIO.h"
#include "common/NdArray.h"
#include "geo/meshDataIO.h"
#ifdef _WIN32
#include <experimental/filesystem>
#endif

BEGIN_P3D_NS
using namespace VRay;
using namespace std;
using namespace VRay::Plugins;

//string model_path = "D:/Work/Projects/KSFuture/materials/face_data_enhancement/test/raw/vray_use.fbx";
namespace VRAY_NODE_NAMES {

	const string HAIR_MATERIAL_NAME = "VRayHair3Mtl2@mtlsingle";
	const string HAIR_VOLUME_NAME = "VRayHair3Mtl2";
	const string HAIR_NODE_NAME = "HairShape@Node";
	const string HAIR_GEO_NAME = "HairShape@Geo";

	const string ENVIROMENT_BITMAP_NAME = "Enviroment_bit_buffer";

	const string ENVIROMENT_TEX_NAME = "Environment_tex_name";
	const string ENVIROMENT_UVW_GEN_NAME = "Enviroment_UVW_Gen";

	const string DEMO_LIGHT_BITBUFFER_NAME = "bitmapBuffer_0_g1_0";
	const string DEMO_LIGTH_TEX_NAME = "file12@VRayPlaceEnvTex2";
	const string DEMO_LIGHT_UVW_GEN_NAME = "VRayPlaceEnvTex2";
	const string DEMO_LIGHT_TEXCOLOR_NAME = "file12@VRayPlaceEnvTex2@lightTexAlpha";
	const string DEMO_LIGHT_NAME = "VRayLightDomeShape1";


	const string EMILY_HEAD_NAME = "Emily_headShape@node";

};
using namespace VRAY_NODE_NAMES;

//const string HAIR_GEO_NAME = "HairShape@Geo";

VrayRender::~VrayRender() {
	//vrayRender->removeHosts
	vrayRender = nullptr;
	vrayIniter = nullptr;

}

void VrayRender::clearLights() {
	auto allLights = vrayRender->getPlugins("LightRectangle");
	cout << "light size " << allLights.size() << endl;
	for (auto t : allLights) {
		cout << t.getName() << endl;
		vrayRender->deletePlugin(t.getName());
	}
}
void VrayRender::addRenderChannel(const std::string& cName) {
	auto renderElement = vrayRender->getRenderElements();
	if (cName == "nodeId") {
		auto allType = renderElement.getAll(VRay::RenderElement::NODEID);
		if (allType.empty())
			renderElement.add(VRay::RenderElement::NODEID, cName.c_str(), cName.c_str());
		renderChannels.push_back(cName);
	}
	else if(cName == "specular"){
		auto allType = renderElement.getAll(VRay::RenderElement::SPECULAR);
		if (allType.empty())
			renderElement.add(VRay::RenderElement::SPECULAR, cName.c_str(), cName.c_str());
		renderChannels.push_back(cName);
	}
	else if (cName == "SSS") {
		auto allType = renderElement.getAll(VRay::RenderElement::SSS);
		if (allType.empty())
			renderElement.add(VRay::RenderElement::SSS, cName.c_str(), cName.c_str());
		renderChannels.push_back(cName);
	}
	else if (cName == "alpha") {
		auto allType = renderElement.getAll(VRay::RenderElement::ALPHA);
		if (allType.empty())
			renderElement.add(VRay::RenderElement::ALPHA, cName.c_str(), cName.c_str());
		renderChannels.push_back(cName);
	}
	else if (cName == "normal") {
		auto allType = renderElement.getAll(VRay::RenderElement::NORMALS);
		if (allType.empty())
			renderElement.add(VRay::RenderElement::NORMALS, cName.c_str(), cName.c_str());
		renderChannels.push_back(cName);
	}
	else if (cName == "diffuse") {
		auto allType = renderElement.getAll(VRay::RenderElement::DIFFUSE);
		if (allType.empty())
			renderElement.add(VRay::RenderElement::DIFFUSE, cName.c_str(), cName.c_str());
		renderChannels.push_back(cName);
	}
}

void VrayRender::setImgSize(uint32 w, uint32 h) {
	imgW = w;
	imgH = h;
	vrayRender->setImageSize(w, h);
}

VRay::Plugins::MtlSingleBRDF VrayRender::getVrayMtl(const std::string&mtlName, const std::vector<float>& color) {

	auto mtl = vrayRender->getPlugin<Plugins::MtlSingleBRDF>(mtlName);
	if (!mtl.isEmpty()) {
		return mtl;
	}
	mtl = vrayRender->newPlugin<Plugins::MtlSingleBRDF>(mtlName);

	auto brdf = vrayRender->newPlugin<Plugins::BRDFDiffuse>(mtlName + "_brdf");
	brdf.set_color(VRay::Color(color[0], color[1], color[2]));
	brdf.set_roughness(1.0);
	//brdf.set_diffuse(VRay::AColor(color[0],color[1],color[2],1.0));
//	brdf.s
	mtl.set_brdf(brdf);
	return mtl;

}
void VrayRender::initHairNode() {
	auto hairNode = vrayRender->getPlugin<Node>(HAIR_NODE_NAME);
	if (hairNode.isEmpty()) {
		hairNode = vrayRender->newPlugin<Node>(HAIR_NODE_NAME);
	}
	auto hairMtl = vrayRender->getPlugin<Plugins::MtlSingleBRDF>(HAIR_MATERIAL_NAME);
	if (hairMtl.isEmpty())
		int a = 0;
	auto hairVolume = vrayRender->getPlugin<Plugins::BRDFVRayMtl>(HAIR_VOLUME_NAME);
	if (hairVolume.isEmpty())
		int a = 0;
	hairNode.set_material(hairMtl);
	hairNode.set_volume(hairVolume);
	hairNode.set_objectID(1);

	auto renderElement = vrayRender->getRenderElements();
	renderElement.add(VRay::RenderElement::NODEID,nullptr,nullptr);
	/*
		auto brdf = vrayRender->newPlugin<VRay::Plugins::BRDFHair3>();
		//dfet color for the diffuse component of the shader.
		brdf.set_overall_color(AColor(0.1f, 0.1, 0.1));

		brdf.set_diffuse_color(AColor(0.0, 0.0f, 0.0));

		brdf.set_primary_specular(AColor(0.6, 0.6, 0.6, 1));
		brdf.set_primary_glossiness(0.9);
		brdf.set_secondary_lock_to_transmission(false);

		brdf.set_secondary_specular(AColor(0.3, 0.3, 0.3));
		brdf.set_secondary_glossiness(0.6);

		brdf.set_transparency(AColor(0.1, 0.1, 0.1));

		brdf.set_transmission(AColor(0.1, 0.1, 0.1, 1));
		brdf.set_transmission_glossiness_length(0.02);
		brdf.set_transmission_glossiness_width(0.0001);

		auto material = vrayRender->newPlugin<VRay::Plugins::MtlSingleBRDF>();
		material.set_brdf(brdf);

		material.setName(HAIR_MATERIAL_NAME);
		hairNode.set_material(material);
		*/


}

void VrayRender::initRenderSetting() {

	auto sampler = vrayRender->getInstanceOrCreate<SettingsImageSampler>();
	auto type = sampler.get_type();
	sampler.set_type(1);
	// for buckt
	sampler.set_subdivision_normals(true);
	sampler.set_subdivision_normals_threshold(0.005);
	sampler.set_dmc_minSubdivs(8);
	sampler.set_dmc_maxSubdivs(512);
	sampler.set_dmc_threshold(0.005);
	sampler.set_subdivision_minRate(-1);
	sampler.set_subdivision_maxRate(2);
	sampler.set_subdivision_jitter(true);
	sampler.set_subdivision_threshold(0.1);
	sampler.set_subdivision_edges(true);
	sampler.set_progressive_maxTime(30.0);
	//for progressive
	sampler.set_progressive_maxSubdivs(1000);
	sampler.set_progressive_progNoiseThreshold(0.005);
	sampler.set_progressive_threshold(0.001);
	sampler.set_progressive_bundleSize(128);
	sampler.set_progressive_showMask(false);
	sampler.set_render_mask_mode(0);
	sampler.set_render_mask_clear(1);
		//gi
	auto gi = vrayRender->getInstanceOrCreate<SettingsGI>();
	gi.set_on(true);
	gi.set_primary_engine(2);
	gi.set_secondary_engine(3);
	gi.set_primary_multiplier(1.0);
	gi.set_refract_caustics(true);
	gi.set_reflect_caustics(false);
	gi.set_saturation(true);

}

void VrayRender::setCameraType(uint32 t,float32 orthW) {

	cameraType = t;
	if (t >= CAMERA_TYPE_NUM)
		return;

	auto camera = vrayRender->getInstanceOrCreate<SettingsCamera>();
	auto view = vrayRender->getInstanceOrCreate<RenderView>();
	if (t == CAMERA_TYPE_ORTHOGONAL) {
		view.set_orthographic(true);
		view.set_orthographicWidth(orthW);
	}
	else if (t == CAMERA_TYPE_PERSPECTIVE) {
		view.set_orthographic(false);
	}
}
void VrayRender::setOrthoWidth(float32 w) {
	if (this->cameraType == CAMERA_TYPE_ORTHOGONAL) {
		auto view = vrayRender->getInstanceOrCreate<RenderView>();
		view.set_orthographicWidth(w);
	}
}
bool VrayRender::init(uint w,uint h,bool use_window) {
	cout << "vrayRender init" << endl;
	Utils::init();
	imgW = h;
	imgH = h;
	try {
		vrayIniter = std::make_shared<VRayInit>(nullptr, use_window);
	}
	catch (VRayException ex) {
		cout << "vary init error " << endl;
		cout << ex.what() << endl;
	}
	cout << "vray init ok" << endl;

	try {
		vrayRender = std::make_shared<VRayRenderer>();
	}
	catch (VRayException ex) {

		printf("VRayException: %s\n", ex.what());
		return false;
	}
	if (!vrayRender)
		return false;

	try {
		vrayRender->setRenderMode(VRayRenderer::RENDER_MODE_PRODUCTION);

		//if (use_window)
		vrayRender->setOnLogMessage(logMessage);

		//init camera
		auto renderView = vrayRender->getInstanceOrCreate<RenderView>();
		auto transform = renderView.get_transform();
		transform.matrix = VRay::Matrix({ 1.0f,0.0,0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,1.0 });
		transform.offset = {camPos.x,camPos.y,camPos.z};
		renderView.set_transform(transform);
	//	renderView.set_orthographic
		auto camera = vrayRender->getInstanceOrCreate<SettingsCamera>();
		cout << "camera type is " << camera.get_type() << endl;
		//set Image size/
		vrayRender->setImageSize(w, h);
		//render setting
		//samplder.set_progressive_maxTime
		setCameraType(cameraType);
		initRenderSetting();

		//auto t = vrayRender->getInstanceOrCreate<SettingsImageSampler>();
		//t.


		//initHairNode();
		vrayRender->commit();
		vrayRender->setAutoCommit(true);
	}
	catch(VRayException ex){
		cout << "vray render setting  error" << endl;
		printf("VRayException: %s\n", ex.what());
		return false;
	};

	//add objId

	modelMat = Mat4f::getIdentity();
	return true;

}

bool VrayRender::loadHairMtl(const std::string& matPath) {
	auto hairMt1 = vrayRender->getPlugin<Plugins::MtlSingleBRDF>(HAIR_MATERIAL_NAME);
	if (hairMt1.isNotEmpty())
		vrayRender->deletePlugin(HAIR_MATERIAL_NAME);
	auto hairVolume = vrayRender->getPlugin<Plugins::BRDFHair3>(HAIR_VOLUME_NAME);
	if (hairVolume.isNotEmpty())
		vrayRender->deletePlugin(HAIR_VOLUME_NAME);
	if (vrayRender->load(matPath) != 0) {
		auto errorP = vrayRender->getLastParserError();
		std::cout << Utils::makeStr("error is in %d of %s ,%s", errorP.fileLine, errorP.fileName, errorP.errorText);
	}

	//auto t = vrayRender->getOrCreatePlugin<LightDome>
	vrayRender->setRenderMode(VRayRenderer::RENDER_MODE_PRODUCTION);

		//if (use_window)
	vrayRender->setOnLogMessage(logMessage);

		//init camera
	auto renderView = vrayRender->getInstanceOrCreate<RenderView>();
	auto transform = renderView.get_transform();
	transform.matrix = VRay::Matrix({ 1.0f,0.0,0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,1.0 });
	transform.offset = {camPos.x,camPos.y,camPos.z};
	renderView.set_transform(transform);
		//set Image size/
	vrayRender->setImageSize(imgW, imgH);
		//render setting
		//samplder.set_progressive_maxTime
	initRenderSetting();
	initHairNode();
	setCameraType(cameraType);
	return true;
}

void VrayRender::clearScene() {
	vrayRender->clearScene();
}
void VrayRender::loadScene(const std::string& vrayScenePath) {
	std::cout << "loading " << vrayScenePath << endl;

	vrayRender->clearScene();

	vrayRender->setRenderMode(VRayRenderer::RENDER_MODE_PRODUCTION);

	vrayRender->setOnLogMessage(logMessage);

	if (vrayRender->load(vrayScenePath)) {
		auto loadError = vrayRender->getLastParserError();
		std::cout << "load_file " << loadError.fileName << "error in line: " << loadError.fileLine << endl;
		std::cout << loadError.errorText << endl;
	}
	else {
		std::cout << "load no error" << endl;
	}

	auto renderView = vrayRender->getInstanceOrCreate<RenderView>();
	auto transform = renderView.get_transform();
	transform.matrix = VRay::Matrix({ 1.0f,0.0,0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,1.0 });

	camPos = {transform.offset[0],transform.offset[1],transform.offset[2]};
	std::cout << "scene Cam pos is " << camPos[0] << "," << camPos[1] << "," << camPos[2] << std::endl;
	vrayRender->getImageSize(imgW,imgH);

}
PNdArrayF VrayRender::renderRaw(int timeMax) {
	auto numThr = vrayRender->getNumThreads();
	cout << "render threads is" << numThr << endl;
	vrayRender->startSync();
	auto t1 = Utils::getCurrentTime();

	auto okEnd = false;
	if (timeMax < 0)
		okEnd = vrayRender->waitForRenderEnd();
	else
		okEnd = vrayRender->waitForRenderEnd(timeMax);

	if (!okEnd) {
		cout << "render don't commeplete!!!" << endl;
	}
	vrayRender->stop();
	auto t2 = Utils::getCurrentTime();

	cout << "render cost time " << (t2 - t1) << endl;

	LocalVRayImage image(vrayRender->getImage());

	auto pixelData = image->getPixelData();
	PNdArrayF ret({ image->getHeight(),image->getWidth(),4 });

	ret.getDataBlock()->setFromMem((uint8*)pixelData, ret.getDataBlock()->getSize());
	return ret;
}

bool VrayRender::renderAll() {
	std::cout << "begin render Channals:" << std::endl;
	std::cout << "RGB," << endl;
	for (auto t : renderChannels) {
		std::cout << t << ",";
	}
	std::cout << endl;
	auto t1 = Utils::getCurrentTime();
	vrayRender->startSync();
	vrayRender->waitForRenderEnd();
	auto t2 = Utils::getCurrentTime();
	std::cout << "render ok,cost time " << t2 - t1 << endl;
	vrayRender->stop();
	return true;
}

PNdArray VrayRender::getChannel(const std::string& cName) {
	bool find = false;
	for (auto t : renderChannels) {
		if (t == cName) {
			find = true;
			break;
		}
	}
	if (!find) {
		std::cout << "channel name " << cName << " is error" << endl;
		return PNdArray(P3D::DType::UInt8, { 1,1 }, true);
	}
	VRay::RenderElement::Type dstType = VRay::RenderElement::NONE;
	if (cName == "nodeId") {
		dstType = VRay::RenderElement::NODEID;
	}
	else if(cName == "specular"){
		dstType = VRay::RenderElement::SPECULAR;
	}
	else if (cName == "SSS") {
		dstType = VRay::RenderElement::SSS;
	}
	else if (cName == "alpha") {
		dstType = VRay::RenderElement::ALPHA;
	}
	else if (cName == "normal") {
		dstType = VRay::RenderElement::NORMALS;
	}

	auto dstEle = vrayRender->getRenderElements().get(dstType);
	uint8* pData = nullptr;

	RenderElement::GetDataOptions dataOption;
	auto pxFmt = dstEle.getDefaultPixelFormat();

	dataOption.format = pxFmt;
	dataOption.rgbOrder = true;

	uint32 dataSize = dstEle.getData((void**)&pData,dataOption);

	if (pxFmt == RenderElement::PF_BW_BIT8 || pxFmt == RenderElement::PF_RGB_BIT8 || pxFmt == RenderElement::PF_RGBA_BIT8) {

		auto mArray = P3D::PNdArrayU8({ imgH,imgW,int(dataSize / (imgH*imgW)) });
		memcpy(mArray.rawData(), pData, dataSize);
		return mArray;
	}
	if (pxFmt == RenderElement::PF_BW_FLOAT || pxFmt == RenderElement::PF_RGB_FLOAT || pxFmt == RenderElement::PF_RGBA_FLOAT) {
		auto mArray = P3D::PNdArrayF({ imgH,imgW,int(dataSize / (imgH*imgW*sizeof(float))) });
		memcpy(mArray.rawData(), pData, dataSize);
		return mArray;
	}
	if (pxFmt == RenderElement::PF_BW_BIT32) {
		std::cout << "img size is " << imgH << "x" << imgW << "dataPtr is"<<pData<<std::endl;
		auto mArray = P3D::PNdArrayT<int>({ imgH,imgW });
		std::cout << "init ndarray ok" << std::endl;
		memcpy(mArray.rawData(), pData, dataSize);
		std::cout << "copy data ok" << std::endl;
		return mArray;
	}
}

bool VrayRender::renderImg(const std::string& imgPath, int32 timeMax, float32 gamma) {

	auto okEnd = false;
	auto t1 = Utils::getCurrentTime();
	vrayRender->startSync();
	if (timeMax < 0)
		okEnd = vrayRender->waitForRenderEnd();
	else
		okEnd = vrayRender->waitForRenderEnd(timeMax);

	if (!okEnd) {
		cout << "render don't commeplete!!!" << endl;
	}
	vrayRender->stop();
	auto t2 = Utils::getCurrentTime();

	LocalVRayImage image(vrayRender->getImage());

	image->setGamma(gamma);
	image->saveToPng(imgPath,true);
	cout << "render cost time " << (t2 - t1) << endl;
	return true;
}
bool VrayRender::render(const std::string& imgFilePath, int timeMax,const std::string& idPath) {
		//vrayRender->commit();
	std::cout << "begin to render img " << imgFilePath << endl;

	auto numThr = vrayRender->getNumThreads();
	cout << "render threads is" << numThr << endl;
	auto t1 = Utils::getCurrentTime();

	vrayRender->startSync();

	auto okEnd = false;
	if (timeMax < 0)
		okEnd = vrayRender->waitForRenderEnd();
	else
		okEnd = vrayRender->waitForRenderEnd(timeMax);

	if (!okEnd) {
		cout << "render don't commeplete!!!" << endl;
	}
	vrayRender->stop();
	auto t2 = Utils::getCurrentTime();

	cout << "render cost time " << (t2 - t1) << endl;
	if ((t2 - t1 < 10) && !okEnd)  //has error for render
		return false;


	LocalVRayImage image(vrayRender->getImage());

	image->setGamma(2.2);
	image->saveToPng(imgFilePath,true);

	return true;

}

void VrayRender::setModelsTransform(const VRay::Transform& transform) {

	//Node hairNode = plugin_cast<Node>(vrayRender->getOrCreatePlugin(HAIR_NODE_NAME, Node::getType()));
	//hairNode.set_transform(transform);

	auto allNodes = vrayRender->getPlugins("Node");

	for (auto tNode : allNodes) {
		auto ttNode = plugin_cast<Node>(tNode);
		ttNode.set_transform(transform);
		//auto tNode = vrayRender->getPlugin<Plugins::Node>(tName);
		//tNode.set_transform(transform);
	}

}
void VrayRender::setModelEuler(float pitch, float yaw, float roll, float x, float y, float z) {
	auto newModelMat = Mat4f::getIdentity();
	newModelMat.rotate(pitch, yaw, roll);

	this->modelMat = Mat4f::getTranslate({x,y,z}) * newModelMat;

	VRay::Transform transform;
	transform.matrix = convertMat2VrayMatrix(newModelMat);
	transform.offset = { x,y,z };


	setModelsTransform(transform);


	cout << "pitch: " << pitch << "yaw: " << yaw << "roll: " << roll << endl;
	//Hair

}

void VrayRender::setSceneEuler(const std::vector<float32>& euler) {
	auto transMat = Mat4f::getTranslate({ euler[3],euler[4],euler[5] });
	auto rotMat = Mat4f::getIdentity();
	rotMat.rotate(euler[0], euler[1], euler[2]);
	auto modelMat = transMat * rotMat;
	VRay::Transform transform;
	transform.matrix = convertMat2VrayMatrix(modelMat);
	transform.offset = { euler[3],euler[4],euler[5]};
	auto allNodes = vrayRender->getPlugins("Node");
	for (auto& t : allNodes) {
		auto tNode = plugin_cast<Node>(t);
		//auto preTrans = tNode.get_transform();
		//auto retTrans = transform * preTrans;
		tNode.set_transform(transform);
	}
}

bool VrayRender::exportScene(const std::string& filePath) {
	return vrayRender->exportScene(filePath) == 0;
}


void VrayRender::setCameraPos(float x, float y, float z,float ux,float uy,float uz) {

	camPos = Vec3f(x, y, z);
	auto dis = camPos.length();
	auto camMat = Mat4f::getLookAt({ x,y,z }, { 0.0f,0.0f,0.0f }, { ux,uy,uz });
	auto transMat = camMat.getTranspose();
	auto vrayMat = convertMat2VrayMatrix(transMat);

	if (this->cameraType == CAMERA_TYPE_PERSPECTIVE) {
		auto renderView = vrayRender->getInstanceOrCreate<RenderView>();
		auto transform = renderView.get_transform();
		transform.matrix = vrayMat;
		//transform.offset = { x,y,z };
		transform.offset = { x,y,z };
		renderView.set_transform(transform);
	}
	else if (this->cameraType == CAMERA_TYPE_ORTHOGONAL) {
		auto renderView = vrayRender->getInstanceOrCreate<RenderView>();
		auto transform = renderView.get_transform();
		//transform.matrix = vrayMat;
		transform.offset = { x,y,z };
		renderView.set_transform(transform);
	}

}

void VrayRender::addMotionSqueeze(const std::vector<std::vector<float>>& eulers) {
	vrayRender->useAnimatedValues(true);
	for (int i = 0; i < eulers.size(); ++i) {
		//vrayRender->setCurrentFrame(i);
		vrayRender->setCurrentTime(i);
		this->setSceneEuler(eulers[i]);
	}
	nFrame = eulers.size();
}

void VrayRender::renderSqueeze(const std::string& dirPath, int start, int end, int step, bool mBlur ) {
	end = std::min(end, nFrame - 1);

	SubSequenceDesc subSequenceDescriptions[1];
	subSequenceDescriptions[0].start = start;
	subSequenceDescriptions[0].end = end * 20;
	subSequenceDescriptions[0].step = step;
	//auto setting = vrayRender->
	auto phyCam = vrayRender->getInstanceOrCreate<CameraPhysical>();
	phyCam.set_use_moblur(mBlur);


	auto outSetting = vrayRender->getInstanceOrCreate<SettingsOutput>();

	outSetting.set_img_width(imgW);
	outSetting.set_img_height(imgH);
	outSetting.set_img_dir(dirPath);

	auto tName = outSetting.get_img_file();
	//outSetting.set_img_file("frame_h.png");

	outSetting.set_frame_start(0);
	outSetting.set_frames_per_second(20);

	vrayRender->setCurrentFrame(start);

	vrayRender->renderSequence(subSequenceDescriptions, 1);
//	vrayRender->renderSequence();
	for (int i = start; i <= 10;++i) {
		cout << "now is frame " << i << endl;

		auto allNodes = vrayRender->getPlugins<Node>();
		for (auto t : allNodes) {
			auto tNode = plugin_cast<Node>(t);
			auto tTrans = tNode.get_transform();
			int aaa = 10;

		}

		vrayRender->waitForRenderEnd();

		LocalVRayImage image(vrayRender->getImage());

		image->setGamma(2.2);
		image->saveToPng(dirPath + Utils::makeStr("\\frame_%03d.png",i),true);
		vrayRender->continueSequence();
	}
	//vrayRender->renderSequence();
	//vrayRender->waitForSequenceEnd();

}

void VrayRender::setCameraFov(float fov) {
	auto renderView = vrayRender->getInstanceOrCreate<RenderView>();
	renderView.set_fov(fov * PI / 180.0f);
	//renderView.
	//renderView.
}

void VrayRender::setCamMat(const std::vector<float32>& rotMat, const std::vector<float32>& camPos) {
	Mat4f tMat;
	doFor(r, 3) {
		doFor(c, 3)
			tMat[{c, r}] = rotMat[r * 3 + c];
	}
	auto vrayMat = convertMat2VrayMatrix(tMat);

	auto renderView = vrayRender->getInstanceOrCreate<RenderView>();
	auto transform = renderView.get_transform();
	transform.matrix = vrayMat;
	transform.offset = { camPos[0],camPos[2],camPos[2] };
	renderView.set_transform(transform);
}

void VrayRender::setNodeTranslation(const std::string& nodeName, const std::vector<float32>& rotScale,const std::vector<float32>& trans) {
	Mat4f tMat;
	doFor(r, 3) {
		doFor(c, 3)
			tMat[{r, c}] = rotScale[r * 3 + c];
	}
	tMat[3][3] = 1.0;
	auto transMat = Mat4f::getTranslate({ trans[0],trans[1],trans[2] });
	tMat =  transMat *tMat;

	VRay::Transform transform;
	transform.matrix = convertMat2VrayMatrix(tMat);
    //transform.offset = { tMat[0][3],tMat[1][3],tMat[2][3] };

	transform.offset = { trans[0],trans[1],trans[2] };
	std::cout << "offset is " << trans[0] << " " << trans[1] << " " << trans[2] << std::endl;

	auto tNode = vrayRender->getPlugin<Node>(nodeName.c_str());
	if (!tNode.isEmpty()) {
		std::cout << "node name is " << tNode.getName() << std::endl;
		std::cout << "mesh name is " << tNode.get_geometry().getName() << std::endl;
	}
	tNode.set_transform(transform);
}
VRay::Matrix VrayRender::convertMat2VrayMatrix(const Mat4f& mat) {

	VRay::Matrix matrix = VRay::Matrix(
		VRay::Vector(mat[{0, 0}], mat[{1, 0}], mat[{2, 0}]),
		VRay::Vector(mat[{0, 1}], mat[{1, 1}], mat[{2, 1}]),
		VRay::Vector(mat[{0, 2}], mat[{1, 2}], mat[{2, 2}]));

	return matrix;
}

void VrayRender::updateHair(const std::string& nodeName,std::shared_ptr<HairModel> commonHair,float32 srcWidth) {
	if (!commonHair)
		return;

	//initHairNode();

	auto hairNode = vrayRender->getPlugin<Node>(nodeName);
	if (hairNode.isEmpty()) {
		std::cout << nodeName << " is empty" << std::endl;
		hairNode = vrayRender->newPlugin<Node>(nodeName);
		auto hairMtl = vrayRender->getPlugin<Plugins::MtlSingleBRDF>(HAIR_MATERIAL_NAME);
		hairNode.set_material(hairMtl);
		//return;
	}

	auto oldeHairGeo = hairNode.get_geometry();
	if(oldeHairGeo.isNotEmpty())
		vrayRender->deletePlugin(oldeHairGeo);


	auto newHairGeo = vrayRender->newPlugin<GeomMayaHair>();

	VectorList vps;
	vps.reserve(commonHair->vps.size());
	for (const auto& p : commonHair->vps) {
		vps.push_back({ p.x,p.y,p.z });
	}
	newHairGeo.set_hair_vertices(vps);
	newHairGeo.set_num_hair_vertices(commonHair->ncs.data(), commonHair->ncs.size());

	FloatList widths(vps.size(), srcWidth);
	int an = 0;
    /*
	for (auto tn : commonHair->ncs) {
		float w = srcWidth;
		for (int i = 0; i < 0.1 * tn && i < tn; ++i)
			widths[i+an] = w;

		float w2 = srcWidth * 0.6;
		float step = (w - w2) / (tn*0.2);
		w -= step;
		for (int i = 0.1 * tn; i < 0.3 * tn && i < tn; ++i) {
			widths[i+an] = w;
			//widths[i+an] = srcWidth;
			w -= step;
		}

		for (int i = 0.3 * tn; i < 0.8 * tn; ++i)
			widths[i] = w2;

		auto w3 = srcWidth * 0.1;
		step = ( w2 - w3) / (tn * 0.2);
		for (int i = 0.8 * tn; i < tn && i >=0 ; i++) {
			widths[i+an] = w2;
		//	widths[i+an] = srcWidth;
			w2 -= step;
		}
		an += tn;
	}
    */
	newHairGeo.set_widths(widths);// { return setValue("widths", floatList); }

	newHairGeo.setName(nodeName+"_Geo");
	hairNode.set_geometry(newHairGeo);

	if (!hairNode.get_visible()) {
		hairNode.set_visible(true);
	}

	auto hairGeo2 = vrayRender->newPlugin<VRay::Plugins::GeomMayaHair>();
	hairGeo2.set_num_hair_vertices(IntList({ 4 }));
	hairGeo2.set_hair_vertices({ {-1,-1,0},{-1,1,0},{1,1,0},{1,-1,0} });
	float width = 0.01f;
	hairGeo2.set_widths({ width, width, width, width });

	//hairNode.set_geometry(hairGeo2);

	auto trans = hairNode.get_transform();
	trans.matrix.makeDiagonal({ 1.0,1.0,1.0 });
	trans.offset = { 0.0,0.0,0.0 };
	hairNode.set_transform(trans);
	int a = 10;
}
/*
void VrayRender::setHairNodeData(const std::string& nodeName, PNdArrayF hairData, float32 srcWidth) {
}*/
void VrayRender::setMeshNodeData(const std::string& nodeName, P3D::PNdArrayF vps, P3D::PNdArrayF vns, P3D::PNdArrayF uvs, P3D::PNdArrayF tls,P3D::PNdArrayF utTls,bool calN) {
	std::cout << "inter setMeshNodeData" << std::endl;
	auto tComp = std::make_shared<P3DComp>();

	tComp->cache_vps = vps.getDataBlock();
	tComp->cache_vns = vns.getDataBlock();
	tComp->uvs = uvs.getDataBlock();

	tComp->tl = std::make_shared<DataBlock>(tls.shape()[0] * 3 * sizeof(uint32));
	auto tlData = tComp->tl->rawData<uint32>();


	for (int r = 0; r < tls.shape()[0]; ++r) {
		for (int c = 0; c < tls.shape()[1]; ++c) {
			tlData[r * 3 + c] = uint32(tls[{r, c}]);
		}
	}
	tComp->uvtl = std::make_shared<DataBlock>(utTls.shape()[0] * 3 * sizeof(uint32));
	auto tlData2 = tComp->uvtl->rawData<uint32>();

	for (int r = 0; r < utTls.shape()[0]; ++r) {
		for (int c = 0; c < utTls.shape()[1]; ++c) {
			tlData2[r * 3 + c] = uint32(utTls[{r, c}]);
		}
	}
	if (calN) {
		tComp->cache_vns->setSize(tComp->cache_vps->getSize());
		tComp->caclNormals(tComp->cache_vps, tComp->tl, tComp->cache_vns);
	}
	addMeshNode(nodeName, tComp);

}

void VrayRender::setNodeMaterial(const std::string& nodeName, const std::string& mtlName) {

	auto tNode = vrayRender->getPlugin<Node>(nodeName.c_str());
	if (tNode.isEmpty()) {
		std::cout << "node name " << nodeName << " error!" << std::endl;
	}

	auto tMtl = vrayRender->getPlugin(mtlName);
	if (tMtl.isEmpty()) {
		std::cout << "materail " << mtlName << "not exit,create a defualt material" << std::endl;
		auto tMtl2= getVrayMtl(mtlName, { 1.0,1.0,1.0 });
		tNode.set_material(tMtl2);
	}
	tNode.set_material(tMtl);
}

void  VrayRender::setNodeVisiable(const std::string& nodeName, bool visiable) {
	auto tNode = vrayRender->getPlugin<Node>(nodeName.c_str());
	if (tNode.isEmpty()) {
		std::cout << "node " << nodeName << " is empty" << std::endl;
		return;
	}
	tNode.set_visible(visiable);
}

void VrayRender::addMeshNode(const std::string& nodeName, pComp tComp) {
	auto tNode = vrayRender->getPlugin<Node>(nodeName.c_str());
	if (tNode.isEmpty()) {
		std::cout << "node " << nodeName << " is empty" << std::endl;
		tNode = vrayRender->newPlugin<Node>(nodeName);
	}

	auto newMesh = convertComp2Vray(tComp);
	std::cout << "update Mesh " << tComp->name << endl;

	auto oldMesh = plugin_cast<Plugins::GeomStaticMesh>(tNode.get_geometry());
	/*
		// update position
		auto vpsData = tComp->cache_vps->rawData<float32>();
		auto nVt = oldMesh.get_vertices();
		for (int i = 0; i < nVt.size(); ++i) {
			nVt[i] = { vpsData[i * 3],vpsData[i * 3 + 1],vpsData[i * 3 + 2] };
		}
		oldMesh.set_vertices(nVt);
		auto vns = oldMesh.get_normals();
		auto vnsData = tComp->cache_vns->rawData<float32>();
		for (int i = 0; i < vns.size(); ++i)
			vns[i] = { vnsData[i * 3],vnsData[i * 3 + 1],vnsData[i * 3 + 2] };
		oldMesh.set_normals(vns);
	*/
	if (!oldMesh.isEmpty())
		vrayRender->deletePlugin(oldMesh);
	tNode.set_geometry(newMesh);

	auto tTrans = Transform(1);
	tNode.set_transform(tTrans);


	tNode.set_visible(true);
}

void VrayRender::setMeshNode(const std::string& nodeName, const std::string& meshPath) {
	auto tComp = MeshDataIO::instance().loadObjSrc(meshPath);
	tComp->cache_vns = std::make_shared<DataBlock>(tComp->cache_vps->getSize(), DType::Float32);
	tComp->caclNormals(tComp->cache_vps, tComp->tl, tComp->cache_vns);
	//tNode.set_transform();
	addMeshNode(nodeName, tComp);
}
void VrayRender::setHairNode(const std::string& nodeName, const std::string& hairPath, float32 srcWidth) {
	auto hairModel = std::make_shared<HairModel>();
	hairModel->loadFromFile(hairPath);

	hairModel->convertData();
	updateHair(nodeName, hairModel, srcWidth);
}
void VrayRender::setHair(const std::string& hairPath,float32 srcWidth) {

	auto hairModel = std::make_shared<HairModel>();
	hairModel->loadFromFile(hairPath);

	hairModel->convertData();

	updateHair(HAIR_NODE_NAME,hairModel,srcWidth);

/*

	auto light = vrayRender->newPlugin<Plugins::LightDirect>();
	auto brdf = vrayRender->newPlugin<Plugins::BRDFVRayMtl>();
	brdf.set_diffuse(AColor(1, 1, 1, 1.0));
	brdf.set_reflect(AColor(0.8, 0.8, 0.8, 1.0));
	brdf.set_fresnel(true);
	auto material = vrayRender->newPlugin<Plugins::MtlSingleBRDF>();
	material.set_brdf(brdf);
	//geometry
	auto hairGeo2 = vrayRender->newPlugin<VRay::Plugins::GeomMayaHair>();
	hairGeo2.set_num_hair_vertices(IntList({ 4 }));
	hairGeo2.set_hair_vertices({ {-1,-1,0},{-1,1,0},{1,1,0},{1,-1,0} });
	float width = 0.01f;
	hairGeo2.set_widths({ width, width, width, width });
	//node
	auto node = vrayRender->newPlugin<Node>();
	//node.set_transform(makeTransform(0, 0, 0));
	node.set_geometry(hairGeo2);
	node.set_material(material);

	auto transform = node.get_transform();
	transform.matrix.makeDiagonal({1.0,1.0,1.0});
	transform.offset = { 0.0,0.0,0.0 };
	node.set_transform(transform);
	*/

}

void VrayRender::setEnvironmentColor(const std::vector<float32> & color) {
	auto env = vrayRender->getInstanceOrCreate<VRay::Plugins::SettingsEnvironment>();
	//env.set_bg_color({ 1.0,1.0,1.0 });
	env.set_gi_tex({ color[0],color[1],color[2]});
    //env.set_reflect_tex({ color[0],color[1],color[2] });
	env.set_reflect_tex({ 0.0,0.0,0.0 });
    env.set_refract_tex({0.0,0.0,0.0});
	//env.set_gi
	//env.set_refract_tex({ color[0],color[1],color[2] });
//	env.set_gi_tex_mult(0.0);
//	env.set_reflect_tex_mult(0.0);
//	env.set_refract_tex_mult(0.0);
	env.set_bg_tex_mult(1.0);

	vrayRender->commit();
	//env.set_bg_tex({ 1.0,1;.0,1.0,1.0 });
}

void VrayRender::setTexture(const std::string& img_path, const std::string bitMapName,const std::string& texNodeName,int colorSpace) {
	auto bitMapBuffer = vrayRender->getPlugin<Plugins::BitmapBuffer>(bitMapName);
	if (bitMapBuffer.isNotEmpty())
		vrayRender->deletePlugin(bitMapBuffer);

	auto newBitMapBuffer = vrayRender->newPlugin<VRay::Plugins::BitmapBuffer>();


	if (newBitMapBuffer.setName(ENVIROMENT_BITMAP_NAME))
		cout << "set bit map name successfully" << endl;

	newBitMapBuffer.set_load_file(true);
	newBitMapBuffer.set_file(img_path);
	//newBitMapBuffer.set_maya_compatible(true);
	newBitMapBuffer.set_color_space(colorSpace);
	newBitMapBuffer.set_filter_type(1);
	newBitMapBuffer.set_filter_blur(1);


	auto texNode = vrayRender->getPlugin<VRay::Plugins::TexBitmap>(texNodeName);
	if (texNode.isEmpty()) {
		std::cout << "tex " << texNodeName << " is empty" << std::endl;
		return;
	}
	texNode.set_bitmap(newBitMapBuffer);
	vrayRender->commit();


}
void VrayRender::setEnvironmentTex(const string& img_path, float intensity, const std::vector<float> &rot,bool noBack,bool asLight) {
	auto bitMap_buffer = vrayRender->getPlugin<VRay::Plugins::BitmapBuffer>(ENVIROMENT_BITMAP_NAME);

	if(bitMap_buffer.isNotEmpty())
		vrayRender->deletePlugin(bitMap_buffer);

	auto newBitMapBuffer = vrayRender->newPlugin<VRay::Plugins::BitmapBuffer>();


	if (newBitMapBuffer.setName(ENVIROMENT_BITMAP_NAME))
		cout << "set bit map name successfully" << endl;

	newBitMapBuffer.set_load_file(true);
	newBitMapBuffer.set_file(img_path);
	newBitMapBuffer.set_maya_compatible(true);
	newBitMapBuffer.set_color_space(COLOR_SPACE_LINEAR);
	newBitMapBuffer.set_filter_type(0);
	newBitMapBuffer.set_filter_blur(1);

	auto texNode = vrayRender->getPlugin<VRay::Plugins::TexBitmap>(ENVIROMENT_TEX_NAME);
	if (texNode.isEmpty()) {
		texNode = vrayRender->newPlugin<VRay::Plugins::TexBitmap>(ENVIROMENT_TEX_NAME);
		auto uvwGen = vrayRender->newPlugin<VRay::Plugins::UVWGenEnvironment>(ENVIROMENT_UVW_GEN_NAME);
		uvwGen.set_mapping_type("spherical");

		// Specify transformation of the input directions.
		uvwGen.set_uvw_matrix(
			Matrix(
				Vector(1.0, 0.0, 0.0),
				Vector(0.0, 0.0, 1.0),
				Vector(0.0, -1.0, 0.0)));
		uvwGen.set_ground_on(0);
	/*	compatibility_with = 1;
		alpha_from_intensity = 0;
		invert = 0;
		color_mult = AColor(1, 1, 1, 1);
		color_offset = AColor(0, 0, 0, 1);
		alpha_mult = 1;
		alpha_offset = 0;
		nouvw_color = AColor(0.5, 0.5, 0.5, 1);
		uvwgen = VRayPlaceEnvTex;
		bitmap = bitmapBuffer_5_g1_0*/
		texNode.set_compatibility_with(1);
		texNode.set_alpha_from_intensity(0);
		texNode.set_invert(0.0);
		texNode.set_color_mult({ 1.0,1.0,1.0,1.0 });
		texNode.set_color_offset({ 0.0, 0.0, 0.0, 1.0 });
		texNode.set_alpha_mult(1.0);
		texNode.set_alpha_offset(0);
		texNode.set_nouvw_color({ 0.5,0.5,0.5,1.0 });
		texNode.set_uvwgen(uvwGen);
	}

	texNode.set_bitmap(newBitMapBuffer);

	auto environment = vrayRender->getInstanceOrCreate<VRay::Plugins::SettingsEnvironment>();

//	environment.set_bg_color({1.0,1.0,1.0});

	if (!noBack) {
		environment.set_bg_color({ 0.0,0.0,0.0 });
		environment.set_bg_tex(texNode);
	}
	if (asLight) {
		environment.set_gi_tex(texNode);
		environment.set_gi_tex_mult(intensity);
		environment.set_reflect_tex(texNode);
		environment.set_reflect_tex_mult(intensity);
		environment.set_refract_tex(texNode);
		environment.set_refract_tex_mult(intensity);
	}
}

void VrayRender::setVrayTexByName(const string& bitMapName, const string& texName, const string& imgPath, int colorSpace) {
	auto oldBuffer = vrayRender->getPlugin<VRay::Plugins::BitmapBuffer>(bitMapName);
	vrayRender->deletePlugin(oldBuffer);

	auto bitMapBuffer = vrayRender->newPlugin<VRay::Plugins::BitmapBuffer>();
	bitMapBuffer.setName(bitMapName);
	bitMapBuffer.set_gamma(1.0f);
	bitMapBuffer.set_allow_negative_colors(true);
	bitMapBuffer.set_load_file(true);
	bitMapBuffer.set_filter_type(0);//1
	bitMapBuffer.set_filter_blur(0.1); //1`
	bitMapBuffer.set_ifl_playback_rate(1);
	bitMapBuffer.set_ifl_start_frame(0); //0
	bitMapBuffer.set_ifl_end_condition(0);
	bitMapBuffer.set_allow_negative_colors(1);

	bitMapBuffer.set_file(imgPath);
	bitMapBuffer.set_maya_compatible(true);

	bitMapBuffer.set_color_space(colorSpace);

	auto textNode = vrayRender->getPlugin<VRay::Plugins::TexBitmap>(texName);

	textNode.set_bitmap(bitMapBuffer);
}


void VrayRender::setLight(uint32 type, const std::string& name, const std::vector<float32>& color,const std::vector<float32>& pos,float32 intensity,const std::vector<float32>& extraData) {
	switch (type)
	{
	case 1://direct light
	{
		auto light = vrayRender->getOrCreatePlugin<LightDirect>(name.c_str());
		Vec3f litPos(pos[0], pos[1], pos[2]);
		litPos.doNormalize();
		Mat4f tmpMat;
		if (abs(litPos.y) > 0.99) {
			tmpMat = Mat4f::getLookAt(litPos, { 0.0,0.0,0.0 }, { 0.0,0.0,1.0 });
		}
		else {
			tmpMat = Mat4f::getLookAt(litPos, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
		}
		//auto tmpMat = Mat4f::getLookAt({pos[0],pos[1],pos[2]}, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
		auto transMat = tmpMat.getTranspose();
		auto vrayMat = convertMat2VrayMatrix(transMat);
		light.set_transform({ vrayMat,{0.0,0.0,0.0} });
		light.set_color({ color[0],color[1],color[2] });
		light.set_intensity(intensity);
		light.set_diffuse_contribution(extraData[0]);
		light.set_specular_contribution(extraData[1]);
        light.set_affectSpecular(true);

		if (!light.get_enabled())
			light.set_enabled(true);
		break;
	}
	case 2://rect light
	{
		auto light = vrayRender->getOrCreatePlugin<Plugins::LightRectangle>(name.c_str());
		
		// dst_pos

		auto tmpMat = Mat4f::getLookAt({pos[0],pos[1],pos[2]}, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
		auto transMat = tmpMat.getTranspose();
		auto vrayMat = convertMat2VrayMatrix(transMat);
		auto srcTrans = light.get_transform();

		Vec3f dstPos(extraData[0], extraData[1], extraData[2]);
		float32 litDis = extraData[3];
		Vec3f litDir(pos[0], pos[1], pos[2]);
		auto litPos = dstPos + litDir * litDis;

		srcTrans.offset = { litPos.x,litPos.y,litPos.z };
		light.set_transform(srcTrans);
		//light.set_transform({ vrayMat,{pos[0],pos[1],pos[2]} });
		//break;
		light.set_color({ color[0],color[1],color[2] });
		light.set_intensity(intensity);

		light.set_u_size(extraData[4]);
		light.set_v_size(extraData[4]);
		
		light.set_affectDiffuse(true);
		light.set_affectSpecular(false);
		light.set_affectReflections(true);
		light.set_ignoreLightNormals(true);
		//light.set_cutoffThreshold()

		if (!light.get_enabled())
			light.set_enabled(true);

	}
	default:
		break;
	}
}
void VrayRender::addLight(uint32 type, const string& name, const std::vector<float>& pos, const std::vector<float>&rot, const std::vector<float>& color, float32 intensity, const std::vector<float>& extraData) {
	switch (type)
	{
	case 1:
	{
		auto lit = vrayRender->newPlugin<LightOmni>();
		lit.set_transform(Transform(
			Matrix(
				Vector(1.0, 0.0, 0.0),
				Vector(0.0, 1.0, 0.0),
				Vector(0.0, 0.0, 1.0)),
			Vector(pos[0], pos[1], pos[2])));

		// Specify the light intensity based on the 'units' parameter.

		lit.set_color({ color[0], color[1], color[2] });

		lit.set_intensity(intensity);

		// Specify the size of the light for shadows. The light is not directly
		// visible to the camera.
		// This parameter controls the softness of the shadows. A value of 0.0
		// produces a point light. Larger values result in soft (area) shadows.
		lit.set_shadowRadius(1.2f);

		// Specify shadow offset from the surface. Helps to prevent polygonal shadow
		// artifacts on low-poly surfaces.
		lit.set_shadowBias(0.02f);


		// Specify a threshold for the light intensity, below which the light will
		// not be computed. This can be useful in scenes with many lights, where you
		// want to limit the effect of the lights to some distance around them.
		// Larger values cut away more from the light; lower values make the light
		// range larger. If you specify 0.0, the light will be calculated for all
		// surfaces.
		lit.set_cutoffThreshold(0.005f);

		// Specify that the bumped normal should be used to check if the light
		// direction is below the surface.
		lit.set_bumped_below_surface_check(true);
		break;
	}
	//light
	case 2:
	{
		auto light = vrayRender->newPlugin<LightDirect>();
		auto rotMat = Mat4f::getIdentity();
		rotMat.rotate(rot[0], rot[1], rot[2]);
		auto trans = convertMat2VrayMatrix(rotMat);
		light.set_transform({ trans,{0.0f,0.0f,0.0f} });
		light.set_color({ color[0],color[1],color[2] });
		light.set_intensity(intensity);
		light.setName(name);
		if (!light.get_enabled())
			light.set_enabled(true);


		break;
	}
}
}

void VrayRender::exportHair(const std::string& filePath) {
	auto hairNode = vrayRender->getPlugin<GeomMayaHair>(HAIR_GEO_NAME);

	auto pos = hairNode.get_hair_vertices();
	auto width = hairNode.get_widths();
	auto nums = hairNode.get_num_hair_vertices();

	ofstream of(filePath,ios::binary);
	int pos_size = pos.size();
	of.write((char*)&pos_size, sizeof(int));
	of.write((char*)&(pos[0][0]), sizeof(float) * pos_size * 3);
	of.write((char*)(width.data()), sizeof(int) * pos_size);

	auto strandNum = nums.size();
	of.write((char*)&strandNum, sizeof(int));
	of.write((char*)(nums.data()), strandNum * sizeof(int));

	of.close();
}

string VrayRender::addObject(const std::string& fPath){
	auto obj = MeshDataIO::instance().loadObj(fPath);
	auto tNode = vrayRender->newPlugin<VRay::Plugins::Node>();
	auto tGeo = vrayRender->newPlugin<VRay::Plugins::GeomStaticMesh>();
	tGeo.set_vertices(obj->vps->rawData<VRay::Vector>(), obj->nVtex);
	tGeo.set_faces(obj->tl->rawData<int32>(), obj->nTri*3);

	tNode.set_geometry(tGeo);

	string geoName = tGeo.getName();
	auto mtl = getVrayMtl(geoName + "_diffuse", {0.9176,0.7686,0.6157});

	tNode.set_material(mtl);

	auto hairNode = vrayRender->getPlugin<Plugins::Node>(HAIR_NODE_NAME);
	auto transForm = hairNode.get_transform();

	tNode.set_transform(transForm);
	tNode.set_visible(true);
	tNode.set_objectID(2);
	//hairNode.set_visible(false);

	auto nodeName = tNode.getName();
	objNames.push_back(nodeName);

	return nodeName;
}

void VrayRender::setDomeLightTex(const std::string& img_path,bool useSpecular) {

	auto bitMap_buffer = vrayRender->getPlugin<VRay::Plugins::BitmapBuffer>(DEMO_LIGHT_BITBUFFER_NAME);

	if(bitMap_buffer.isNotEmpty())
		vrayRender->deletePlugin(bitMap_buffer);

	auto newBitMapBuffer = vrayRender->newPlugin<VRay::Plugins::BitmapBuffer>();


	if (newBitMapBuffer.setName(ENVIROMENT_BITMAP_NAME))
		cout << "set bit map name successfully" << endl;

	newBitMapBuffer.set_load_file(true);
	newBitMapBuffer.set_file(img_path);
	newBitMapBuffer.set_maya_compatible(true);
	newBitMapBuffer.set_color_space(COLOR_SPACE_LINEAR);
	newBitMapBuffer.set_filter_type(0);
	newBitMapBuffer.set_filter_blur(1);

	auto texNode = vrayRender->getPlugin<VRay::Plugins::TexBitmap>(ENVIROMENT_TEX_NAME);
	if (texNode.isEmpty()) {
		texNode = vrayRender->newPlugin<VRay::Plugins::TexBitmap>(ENVIROMENT_TEX_NAME);
		auto uvwGen = vrayRender->getPlugin<VRay::Plugins::UVWGenEnvironment>(ENVIROMENT_UVW_GEN_NAME);
		if (uvwGen.isEmpty()) {
			uvwGen = vrayRender->newPlugin<VRay::Plugins::UVWGenEnvironment>(ENVIROMENT_UVW_GEN_NAME);
			uvwGen.set_mapping_type("spherical");

			// Specify transformation of the input directions.
			uvwGen.set_uvw_matrix(
				Matrix(
					Vector(1.0, 0.0, 0.0),
					Vector(0.0, 0.0, 1.0),
					Vector(0.0, -1.0, 0.0)));
			uvwGen.set_ground_on(0);
		}
		texNode.set_uvwgen(uvwGen);
		texNode.set_compatibility_with(1);
		texNode.set_alpha_from_intensity(0);
		texNode.set_invert(0.0);
		texNode.set_color_mult({ 1.0,1.0,1.0,1.0 });
		texNode.set_color_offset({ 0.0, 0.0, 0.0, 1.0 });
		texNode.set_alpha_mult(1.0);
		texNode.set_alpha_offset(0);
		texNode.set_nouvw_color({ 0.5,0.5,0.5,1.0 });
		texNode.set_uvwgen(uvwGen);
	}
	texNode.set_bitmap(newBitMapBuffer);
	auto colorOp = vrayRender->getPlugin<Plugins::TexAColorOp>(DEMO_LIGHT_TEXCOLOR_NAME);
	colorOp.set_color_a(texNode);
	auto demoLight = vrayRender->getPlugin<Plugins::LightDome>(DEMO_LIGHT_NAME);
	demoLight.set_dome_tex(colorOp);
	demoLight.set_affectSpecular(useSpecular);
}

void VrayRender::setDomeLightEnable(bool enable) {
	auto domeLight = vrayRender->getPlugin<Plugins::LightDome>(DEMO_LIGHT_NAME);
	if (!domeLight.isEmpty())
		domeLight.set_enabled(enable);
}

void VrayRender::dumpHair(const std::string& dstPath,const std::vector<std::string>& hairNames){
	auto hair1 = vrayRender->getPlugin<GeomMayaHair>("mesh_description5Shape@collection1@description5@xgen_patch");
	auto hairVps1 = hair1.get_hair_vertices();
	auto hairN1 = hair1.get_num_hair_vertices();
//	mesh_description8Shape@collection1@description8@xgen_patch
//	auto hair2 = vrayRender->getPlugin<GeomMayaHair>("mesh_description8Shape@collection1@description8@xgen_patch");
	auto hair2 = vrayRender->getPlugin<GeomMayaHair>("mesh_description8Shape@collection1@description8@xgen_patch");
	auto hairVps2 = hair2.get_hair_vertices();
	auto hairN2 = hair2.get_num_hair_vertices();

	auto allHairN = hairN2.size() + hairN1.size();

	auto hairGeo = std::make_shared<HairModel>();
	hairGeo->srcStrands.reserve(allHairN);
	uint32 vIdx = 0;
	uint32 preIdx = 0;
	uint32 maxD = 0;
	for (auto& tName : hairNames) {
		auto tHair = vrayRender->getPlugin<GeomMayaHair>(tName);
		auto hairVps = tHair.get_hair_vertices();
		auto hairN = tHair.get_num_hair_vertices();
		uint32 vIdx = 0;
		uint32 preIdx = 0;
		for (int i = 0; i < hairN.size(); ++i) {
			auto tmpS = std::make_shared<CommonSingleStrand>();
			tmpS->pos.reserve(hairN[i]);
			vIdx = preIdx;
			for (int j = 0; j < hairN[i]; j += 1) {
				tmpS->pos.push_back({ hairVps[vIdx][0],hairVps[vIdx][1],hairVps[vIdx][2] });
				++vIdx;
			}
			preIdx += hairN[i];
			hairGeo->srcStrands.push_back(tmpS);
		}
	}

	hairGeo->dumpHair(dstPath);

	int aa = 0;
}
void VrayRender::debugJust() {

}

void VrayRender::loadComp(const std::vector<std::string > &pathes) {
	mComp = MeshDataIO::instance().loadObjSrc(pathes[0]);
	for (int i = 1; i < pathes.size(); ++i) {
		auto tmpC = MeshDataIO::instance().loadObjSrc(pathes[i]);
		mComp->addBS(tmpC->vps);
	}
}

void VrayRender::updateModel(const std::string& nodeName, const std::vector<float32>& bsCoeff) {
	if (!mComp)
		return;
	auto tNode = vrayRender->getPlugin<Node>(nodeName.c_str());
	if (tNode.isEmpty())
		return;
	mComp->applayBS(bsCoeff);
	auto newMesh = convertComp2Vray(mComp);
	std::cout << "update Mesh " << mComp->name << endl;

	auto oldMesh = plugin_cast<Plugins::GeomStaticMesh>(tNode.get_geometry());
	//auto tNormals = oldMesh.get_normals();
	//auto tVertex = oldMesh.get_vertices();
	//auto tvTl = oldMesh.get_faces();
	//auto tnTl = oldMesh.get_faceNormals();
	//newMesh.set_normals(tNormals);
	//newMesh.set_faceNormals(tnTl);
	if (!oldMesh.isEmpty())
		vrayRender->deletePlugin(oldMesh);
	tNode.set_geometry(newMesh);
}

VRay::Plugins::GeomStaticMesh VrayRender::convertComp2Vray(pComp tComp){
	auto vrayMesh = vrayRender->newPlugin<GeomStaticMesh>();
	int np = tComp->cache_vps->getSize() / sizeof(float32) / 3;
	auto vpsData = tComp->cache_vps->rawData<float32>();
	auto vnsData = tComp->cache_vns->rawData<float32>();
	auto uvsData = tComp->uvs->rawData<float32>();
	VRay::VectorList psV, nsV, uvV;
	for (int i = 0; i < np; ++i) {
		psV.push_back({ vpsData[i * 3],vpsData[i * 3 + 1],vpsData[i * 3 + 2] });
	}
	int vnp = tComp->cache_vns->getSize() / sizeof(float32) / 3;
	for (int i = 0; i < vnp; ++i) {
		nsV.push_back({ vnsData[i * 3],vnsData[i * 3 + 1],vnsData[i * 3 + 2] });
	}
	int uvN = tComp->uvs->getSize() / sizeof(float32) / 2;
	for (int i = 0; i < uvN; ++i)
		uvV.push_back({ uvsData[i * 2],uvsData[i * 2 + 1],0.0f });

	vrayMesh.set_vertices(psV);
	vrayMesh.set_normals(nsV);

	int nTri = tComp->tl->getSize() / sizeof(uint32) / 3;
	std::vector<int> tl(nTri * 3);
	auto tlData = tComp->tl->rawData<uint32>();
	for (int i = 0; i < nTri * 3; ++i) {
		tl[i] = tlData[i];
	}
	vrayMesh.set_faces(tl);
	if(tComp->vntl == nullptr)
		vrayMesh.set_faceNormals(tl);
	else {
		std::vector<int> tlvn(nTri * 3);
		auto tlData = tComp->vntl->rawData<uint32>();
		for (int i = 0; i < nTri * 3; ++i) {
			tlvn[i] = tlData[i];
			if (tlvn[i] >= nsV.size())
				int aaaa = 0;
		}
		vrayMesh.set_faceNormals(tlvn);
	}

	std::vector<int> uvTri(nTri*3);
	uint32* uvtlData = tComp->uvtl->rawData<uint32>();
	for (int i = 0; i < nTri * 3; ++i) {
		uvTri[i] = uvtlData[i];
	}

	ValueList channel;
	channel.push_back(Value(0));
	channel.push_back(Value(uvV));
	channel.push_back(Value(uvTri));

	ValueList mapChannels;
	mapChannels.push_back(Value(channel));
	vrayMesh.set_map_channels(mapChannels);
	return vrayMesh;
}

END_P3D_NS
