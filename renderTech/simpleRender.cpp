#include "simpleRender.h"
#include "geo/meshDataIO.h"
using namespace std;

BEGIN_P3D_NS
void SimpleRender::init(const std::string& hairPath, const std::string& vsPath, const std::string& psPath, P3D::uint32 mrt) {

	uint32 width = 512, height = 512;
	P3DEngine::instance().init(APP_OPENGL);
	mWindow = P3DEngine::instance().createWindow(width, height);
	auto shader = P3DEngine::instance().createShader();
	shader->loadFromFile(vsPath, psPath);
	mScene = mWindow->getScene();
	mComp = MeshDataIO::instance().loadFromFile(hairPath);
	//auto comps = P3DLoader::instance().loadComps(hairPath);
	//mComp = comps[0];
	auto modelMat = Mat4f::getIdentity();
	auto rotMat = Mat4f::getRotX(-90);
	mComp->setModelMat(modelMat);
	mScene->addComp(mComp, shader);
	mCamera = mScene->getCamera();
	mCamera->setCameraType(Camera::CAMERA_TYPE_ORTH);
	mCamera->setOrthParams(0.6, 0.6, -20, 20);
	mCamera->setViewParams({ 0.0,0.0,1.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });

	mFbo = P3DEngine::instance().createRenderFrame(width, height, 1);
	mFbo->addRenderTextures(mrt);
	this->mrt = mrt;
}

void SimpleRender::setWinSize(int w, int h) {
	if (winH == h && winW == w)
		return;
	mFbo = P3DEngine::instance().createRenderFrame(w, h, 1);
	mFbo->addRenderTextures(mrt);
	winH = h;
	winW = w;
}
pRenderFrame SimpleRender::renderFbo(float32 camX, float32 camY, float32 camZ) {
	mCamera->setViewParams({ camX,camY,camZ }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
	if (mrt > 1)
		mFbo->applyMulti(mrt);
	else
		mFbo->apply();
	mScene->render();
	mFbo->apply0();
	return mFbo;
}

P3D::PNdArrayF SimpleRender::render(float camX, float camY, float camZ) {
	mCamera->setViewParams({ camX,camY,camZ }, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
	if (mrt > 1)
		mFbo->applyMulti(mrt);
	else
		mFbo->apply();
	mScene->render();
	mFbo->apply0();

	std::vector<P3D::PNdArray> ret;
	for (int i = 0; i < mrt; ++i) {
		auto cpuImg = mFbo->getRenderTextureCpu(i, true);
		P3D::PNdArray tmp(P3D::DType::Float32, { (int)(cpuImg->width),int(cpuImg->height),4 }, true);
		memcpy(tmp.rawData(), cpuImg->getRawData(), cpuImg->getBufferSize());
		ret.push_back(tmp);
	}
	auto bigRet = P3D::NdUtils::concat(ret);
	auto allRet = bigRet.asFloat();
	return allRet;
}

void SimpleRender::setOrthWidth(float w) {
	mCamera->setOrthParams(w, w, -200, 500);
}
void SimpleRender::setModelEuler(float px, float py, float pz, float rotx, float roty, float rotz) {
	auto rotModel = P3D::Mat4f::getRotZ(rotz)*P3D::Mat4f::getRotY(roty)*P3D::Mat4f::getRotZ(rotz);
	auto transModel = P3D::Mat4f::getTranslate({ px,py,pz });
	auto modelMat = transModel * rotModel;
	mComp->setModelMat(modelMat);

}
END_P3D_NS
