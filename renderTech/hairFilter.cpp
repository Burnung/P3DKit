#include "hairFilter.h"
#include "products/loader/P3DLoader.h"

BEGIN_P3D_NS
void HairFilter::init(const std::string& workDir) {
	
	P3DEngine::instance().init(APP_OPENGL);

	win = P3DEngine::instance().getWindow();

	auto cpuDir = P3DLoader::instance().loadCpuImg(workDir + "/dir.png");
	detailTex = P3DEngine::instance().createTexture(cpuDir);


	auto shaderDir = workDir + "/shader/";

	gradientShader = P3DEngine::instance().createShader();
	gradientShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "gradient.frag");
	//gradientShader->setUniformF2()
	gaussianShader = P3DEngine::instance().createShader();
	gaussianShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "gaussian1D.frag");

	smoothShader = P3DEngine::instance().createShader();
	smoothShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "smooth.frag");

	rgb2LabShader = P3DEngine::instance().createShader();
	rgb2LabShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "rgb2lab.frag");

	lab2RgbShader = P3DEngine::instance().createShader();
	lab2RgbShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "lab2rgb.frag");

	quantizationShader = P3DEngine::instance().createShader();
	quantizationShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "quantization.frag");
	
	blendShader = P3DEngine::instance().createShader();
	blendShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "blend.frag");

	composeShader = P3DEngine::instance().createShader();
	composeShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "compose.frag");

	detailShader = P3DEngine::instance().createShader();
	detailShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "addDetail.frag");
	detailShader->setUniformTex("hairDetailTexture", detailTex);


	basicShader = P3DEngine::instance().createShader();
	basicShader->loadFromFile(shaderDir + "basic.vert", shaderDir + "basic.frag");



	//init vao
	quad = P3DEngine::instance().createUserGpuGeo();
	std::vector<Vec3f> vps = { {-1.0f,-1.0f,0.5f},{1.0f,-1.0f,0.5f},{1.0f,1.0f,0.5f},{-1.0f,1.0f,0.5f} };
	std::vector<Vec2f> uvs = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };
	std::vector<Vec3<uint32>> tls = { {0,1,3},{1,3,2} };
	auto vpsData = std::make_shared<DataBlock>(vps.data(), vps.size() * sizeof(Vec3f), false, DType::Float32);
	auto uvsData = std::make_shared<DataBlock>(uvs.data(), uvs.size() * sizeof(Vec2f), false, DType::Float32);
	auto tlData = std::make_shared<DataBlock>(tls.data(), tls.size() * sizeof(Vec3<uint32>), false, DType::Uint32);

	quad->addVertexData(vpsData, 0, 3, 0);
	quad->addVertexData(uvsData, 1, 2, 0);
	quad->addElementBuffer(tlData,GeoTYPE::GEO_TYPE_TRIS);
	//quad->setShader(basicShader);
	quad->commitPure();


}
pRenderFrame HairFilter::filterFbo(const std::string& srcPath, const std::string& maskPath, int mode) {
	auto srcImg = P3DLoader::instance().loadCpuImg(srcPath);
	auto maskImg = P3DLoader::instance().loadCpuImg(maskPath);
	return filterFbo(srcImg, maskImg, mode);
}
pRenderFrame HairFilter::filterFbo(PNdArrayU8 srcImg, PNdArrayU8 maskImg, int mode) {

	auto srcCpu = std::make_shared<CpuImage>(srcImg);
	auto maskCpu = std::make_shared<CpuImage>(maskImg);
	return 	filterFbo(srcCpu, maskCpu, mode);
}
void HairFilter::addFbo(pRenderFrame& tFbo) {
	tFbo = P3DEngine::instance().createRenderFrame(mWidth, mHeight, 1);
	tFbo->addRenderTextures(1);
}

void HairFilter::setupFbo() {
	addFbo(gradinetFbo);
	addFbo(guassionFbo);
	addFbo(LabFbo);
	addFbo(rgbFbo);
	addFbo(quantFbo);
	addFbo(tmpFbo);
	addFbo(tmpFbo2);
}

PNdArrayU8 HairFilter::filter(PNdArrayU8 srcImg, PNdArrayU8 maskImg,int mode) {
	auto ret = filterFbo(srcImg, maskImg,mode);
	auto imgCpu = ret->getRenderTextureCpu(0);
	return imgCpu->getNdArray(0).asType<uint8>();
}

pRenderFrame HairFilter::filterFbo(pCPuImage srcCpu, pCPuImage maskCpu,int mode) {

	if (srcCpu->getWidth() != mWidth || srcCpu->getHeight() != mHeight) {
		mWidth = srcCpu->getWidth();
		mHeight = srcCpu->getHeight();
		setupFbo();
	}
	srcTex = P3DEngine::instance().createTexture(srcCpu);
	maskTex = P3DEngine::instance().createTexture(maskCpu);

	gradientShader->setUniformF2("dimensions", {float32(mWidth),float32(mHeight)});

	gaussianShader->setUniformF2("dimensions", {float32(mWidth),float32(mHeight)});
	
	smoothShader->setUniformF2("dimensions", {float32(mWidth),float32(mHeight)});
	smoothShader->setUniformI("steps", int(mSteps));

	detailShader->setUniformF2("dimensions", {float32(mWidth),float32(mHeight)});
	detailShader->setUniformI("steps", int(mSteps));

	composeShader->setUniformF2("dimensions", {float32(mWidth),float32(mHeight)});

	quantizationShader->setUniformI("u_numBins", 8);
	quantizationShader->setUniformF("u_softness", 1.5f);

	//pass1 compute image gradient
	drawScene(gradinetFbo, gradientShader, srcTex);
	//drawScene(gradinetFbo, basicShader, detailTex);
	return gradinetFbo;
	//x-axis

	//pass 2 smooth image gradient to get oritation map
	renderGaussianPass1D(mode);
	//return guassionFbo;


	if (mode == 1) {
		// smooth render
		renderSmoothPass(tmpFbo,srcTex, mode);
		return tmpFbo;
	}
	else if (mode == 2) {
		// pass 3-1 smooth the color
		renderSmoothPass(tmpFbo,srcTex, mode);
		// pass 3-2 apply quantization to the luminance channel
		drawScene(LabFbo, rgb2LabShader, tmpFbo->getRenderTexture(0));
		drawScene(quantFbo, quantizationShader, LabFbo->getRenderTexture(0));
		drawScene(rgbFbo, lab2RgbShader, quantFbo->getRenderTexture(0));
		//return rgbFbo;
		// pass 3-3 smooth the color again
		renderSmoothPass(tmpFbo,rgbFbo->getRenderTexture(0), mode);
		// pass 3-4 blend with the input
		renderBlendingPass(tmpFbo2, srcTex, tmpFbo->getRenderTexture(0));
		//drawScene(tmpFbo2, basicShader, srcTex);
		return tmpFbo2;
	}
	else if (mode == 3) {
		//pass 3-2 apply quantization to luminacne channel
		drawScene(LabFbo, rgb2LabShader, srcTex);
		drawScene(quantFbo, quantizationShader, LabFbo->getRenderTexture(0));
		drawScene(rgbFbo, lab2RgbShader, quantFbo->getRenderTexture(0));

		// pass 3-3 add detail
		renderDetailPass(tmpFbo, rgbFbo->getRenderTexture(0), mode);
		//return tmpFbo;
		//pass 3-4 smooth the color again
		renderSmoothPass(tmpFbo2, tmpFbo->getRenderTexture(0),mode);
		//pass 3-5 blend with input
		
		renderBlendingPass(tmpFbo, srcTex, tmpFbo2->getRenderTexture(0));

		return tmpFbo;

	}

	return guassionFbo;



}

void HairFilter::renderDetailPass(pRenderFrame tFbo, pTexture tex,int mode) {
	detailShader->setUniformTex("inputTexture", tex);
	detailShader->setUniformTex("inputMask", maskTex); 
	detailShader->setUniformTex("gradientSampler", guassionFbo->getRenderTexture(0));

	if (mode == 3) {
		detailShader->setUniformF2("sampleCenter", { 0.5f, 0.8f });
		//diffuse:
		detailShader->setUniformF("diffuseDetailScale", 2.2f);
		detailShader->setUniformF("diffuseDetailPower", 1.2f);
		detailShader->setUniformF("diffuseMix", 0.7f);
		detailShader->setUniformF("diffuseSaturate", 0.9f);
	}
	detailShader->setUniformF("strength", 4.0f);
	tFbo->apply();
	P3DEngine::instance().clearBuffers(0.0f, 0.0f, 0.0f, 0.0f);
	detailShader->applay();
	quad->drawPure();
	detailShader->applay0();
	tFbo->apply0();
}
void HairFilter::renderBlendingPass(pRenderFrame tFbo, pTexture tex1, pTexture tex2) {
	blendShader->setUniformTex("inputTexture1", tex1);
	blendShader->setUniformTex("inputTexture2", tex2);
	blendShader->setUniformTex("inputMask", maskTex);

	tFbo->apply();
	P3DEngine::instance().clearBuffers(0.0, 0.0, 0.0, 0.0);
	blendShader->applay();
	quad->drawPure();
	blendShader->applay0();
	tFbo->apply0();
}

void HairFilter::renderGaussianPass1D(int mode) {

	gaussianShader->setUniformF2("direction", { 1.0f,0.0f });
	gaussianShader->setUniformF("sigma", mSigma);

	switch (mode)
	{
	case 0:
	case 1:
	case 2:
	{
		gaussianShader->setUniformF("gaussionScale", 1.0f);
		gaussianShader->setUniformF("strand_tangent", 0.0f);
		gaussianShader->setUniformF("strand_fluence", 0.0f);
		break;
	}
	case 3:
	{
		gaussianShader->setUniformF("gaussionScale", 1.0f);
		gaussianShader->setUniformF("strand_tangent", 1.0f);
		gaussianShader->setUniformF("strand_fluence", 1.0f);
		break;

	}
	default:
		break;
	}
	drawScene(guassionFbo, gaussianShader, gradinetFbo->getRenderTexture(0));
}
void HairFilter::drawScene(pRenderFrame tFbo, pShader tShader, pTexture tex) {
	tShader->setUniformTex("inputTexture", tex);
	tFbo->apply();
	tShader->applay();
	P3DEngine::instance().clearBuffers(0.0f, 0.0f, 0.0f, 0.0f);
	quad->drawPure();
	tShader->applay0();
	tFbo->apply0();
}
void HairFilter::renderSmoothPass(pRenderFrame tFbo, pTexture tex,int mode) {
	smoothShader->setUniformTex("inputTexture", tex);
	smoothShader->setUniformTex("inputMask", maskTex);
	smoothShader->setUniformTex("gradientSampler", guassionFbo->getRenderTexture(0));
	smoothShader->setUniformTex("hairDetailTexture", detailTex);
	smoothShader->setUniformF("strength", 4.0f);
	switch (mode)
	{
	case 0:
	case 1:
	case 2:
	{
		smoothShader->setUniformF2("sampleCenter_", { 0.5f, 0.5f });
		//diffuse:
		smoothShader->setUniformF("diffuseDetailScale", 2.2f);
		smoothShader->setUniformF("diffuseDetailPower", 1.2f);
		smoothShader->setUniformF("diffuseMix", 0.0f);
		smoothShader->setUniformF("diffuseSaturate", 1.0f);
		//area-specualr:
		smoothShader->setUniformF("specular1Value", 0.0f);
		smoothShader->setUniformF("specular1LumiFallOff", 0.5f);
		smoothShader->setUniformF("specular1LumiCut", 0.0f);
		//strand-specualr:
		smoothShader->setUniformF("specular2Value", 0.0f);
		smoothShader->setUniformF("specular2LumiFallOff", 1.5f);
		smoothShader->setUniformF("specular2LumiCut", 0.0f);
		//point-specualr:
		smoothShader->setUniformF("specular3Value", 0.0f);
		smoothShader->setUniformF("specular3LumiFallOff", 4.0f);
		smoothShader->setUniformF("specular3LumiCut", 0.03f);
	}break;
	case 3:
	{
		smoothShader->setUniformF2("sampleCenter_", { 0.5f, 0.8f });
		//diffuse:
		smoothShader->setUniformF("diffuseDetailScale", 2.2f);
		smoothShader->setUniformF("diffuseDetailPower", 1.2f);
		smoothShader->setUniformF("diffuseMix", 0.7f);
		smoothShader->setUniformF("diffuseSaturate", 0.9f);
		//area-specualr:
		smoothShader->setUniformF("specular1Value", 0.25f);
		smoothShader->setUniformF("specular1LumiFallOff", 0.5f);
		smoothShader->setUniformF("specular1LumiCut", 0.0f);
		//strand-specualr:
		smoothShader->setUniformF("specular2Value", 1.0f);
		smoothShader->setUniformF("specular2LumiFallOff", 1.5f);
		smoothShader->setUniformF("specular2LumiCut", 0.0f);
		//point-specualr:
		smoothShader->setUniformF("specular3Value", 10.0f);
		smoothShader->setUniformF("specular3LumiFallOff", 4.0f);
		smoothShader->setUniformF("specular3LumiCut", 0.03f);
	}break;
	}
	tFbo->apply();
	P3DEngine::instance().clearBuffers(0.0f, 0.0f, 0.0f, 0.0f);
	smoothShader->applay();
	quad->drawPure();
	smoothShader->applay0();
	tFbo->apply0();
}

END_P3D_NS