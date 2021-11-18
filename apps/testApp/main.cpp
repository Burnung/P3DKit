#include "baseApp/baseApp.h"
#include "common/P3D_Utils.h"
#include "engine/mgs/renderNodeManager.hpp"
#include "libs/imgui/imgui.h"
#include "loader/P3DLoader.h"
#include "libs/glm/glm/gtc/matrix_transform.hpp"
#include "engine/OPenGL/OGLCommon.h"
#include "cv_common/CvUtil.h"

using namespace P3D;



pTexture loadTex(const std::string& fPath) {
	auto tImg = P3DLoader::instance().loadCpuImg(fPath);
	auto tTex = P3DEngine::instance().createTexture(tImg);
	return tTex;
}

class testApp :public BaseApp {
public:
	//BaseApp(uint32 w, uint32 h, const std::string&tittle = "Demo");
	testApp(uint32 w,uint h,const std::string&tittle="Demo"):BaseApp(w,h,tittle) {
	}
	void addRenderNode(const std::string& nodeName, const std::string& shaderName,pTexture tex,float roughNess) {

		auto renderNode = renderNodeManager->createRenderNode(RENDERNODE_CUSTOM, nodeName, shaderName, nodeName+"_fbo");
		renderNode->addUniform(std::make_shared<ShaderUniform>("envTexture",tex));
		renderNode->addRenderable("quad");
		auto tUniform = std::make_shared<ShaderUniform>("lod", roughNess);
		renderNode->addUniform(tUniform);
		//renderNode->addRenderable("sphere");
		renderNode->setCamera("mainCam");
		renderNode->addRenderStateSetting("cull_face", { "disable" });
		renderNodeManager->setRenderSort(1,nodeName);
	}
	virtual void onInit() {
		sceneManager = P3DEngine::instance().getSceneManager();
		renderNodeManager = P3DEngine::instance().getRenderNodeManager();
		sceneManager->addValue("lod", Value(0.0f));
		initHDR();
		auto camera = std::make_shared<P3D::Camera>();
		camera->setProjParams(60, winW * 1.0f / winH, 0.01, 100);
		camera->setViewParams({ 0.0,0.0,50.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
		
		auto vsPath = R"(D:\work\projects\P3DKit\testCodes\resource\shaders\test\show_HDR.vert)";
		auto fsPath = R"(D:\work\projects\P3DKit\testCodes\resource\shaders\test\show_HDR.frag)";

		auto vsPath2 = R"(D:\work\projects\P3DKit\testCodes\resource\shaders\test\quad_show.vert)";
		auto fsPath2 = R"(D:\work\projects\P3DKit\testCodes\resource\shaders\test\quad_show.frag)";

	//	pRenderNode createRenderNode(const RENDERNODE_TYPE& tType, const std::string& tName, const std::string& vsPath, const std::string& fsPath, const std::string&renderFrameName, const std::vector<float>& winRatio = { 1.0,1.0 }) {
		auto renderNode = renderNodeManager->createRenderNode(RENDERNODE_CUSTOM, "base", vsPath2,fsPath2, "forwardFbo");
		//auto renderNode = renderNodeManager->createRenderNode(RENDERNODE_CUSTOM, "base", vsPath,fsPath, "forwardFbo");
		renderNode->addUniform(std::make_shared<ShaderUniform>("envTexture", cubeTex));
		renderNodeManager->setRenderSort(0,"base");
		auto pherePaht = R"(D:\work\projects\P3DKit\testCodes\resource\objs\sphere.obj)";
		auto quadPath = R"(D:\work\projects\P3DKit\testCodes\resource\objs\quad.obj)";
		auto sphere = P3D::P3DLoader::instance().loadComps(pherePaht)[0];
		auto quad = P3D::P3DLoader::instance().loadComps(quadPath)[0];
		//sphere->setModelMat()
		sceneManager->addComp("sphere", sphere);
		sceneManager->addComp("quad", quad);
		sceneManager->setCompModelMat("sphere", Mat4f::getIdentity());
		sceneManager->addCamera("mainCam", camera);

		renderNode->addRenderable("quad");
		auto tUniform = std::make_shared<ShaderUniform>("lod", 0.0f);
		Value tv;
		sceneManager->getValue("lod", tv);
		//tUniform->setValue(tv);
		renderNode->addUniform(tUniform);
		//renderNode->addRenderable("sphere");
		renderNode->setCamera("mainCam");
		renderNode->addRenderStateSetting("cull_face", { "disable" });

		auto shaderName = renderNode->getDefaultShaderName();
		addRenderNode("irraMap", shaderName, envDiffseMap,0.0f);
		nodeNames.push_back("irraMap");

		float tDelta = 0.5 / 4;
	//	auto renderNode = renderNodeManager->getRenderNode("specMap");
		for (int i = 1; i <= 4; ++i) {
			char tName[256];
			sprintf(tName, "specMap_%d", i);
			addRenderNode(tName, shaderName, envSpMap, i * tDelta);
			nodeNames.push_back(tName);
		}
	//	addRenderNode("specMap", shaderName, envSpMap);


		
		//renderNodeManager->createRenderNode()
	};
	void initHDR() {
		auto objPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Cerberus_LP.FBX)";
		auto abedoPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_A.png)";
		auto normalPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_N.png)";
		auto matePath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_M.png)";
		auto routhPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\Textures\Cerberus_R.png)";
		//auto hdrPath = R"(..\testCodes\resource\Cerberus_by_Andrew_Maximov\environment.hdr)";
		auto hdrDir = R"(D:\work\data\momo\scanData\comps\hdri)";
		auto hdrPath = winTittle;// Utils::joinPath(hdrDir, winTittle);
		auto vsPath = R"(..\testCodes\resource\shaders\pbr\pbr_vs.vert)";
		auto fsPath = R"(..\testCodes\resource\shaders\pbr\pbr_fs.frag)";
		auto cs2CubePath = R"(..\testCodes\resource\shaders\pbr\equirect2cube_cs.glsl)";
		auto csSpLutPath = R"(..\testCodes\resource\shaders\pbr\spbrdf_cs.glsl)";

		auto csEnvSpePath = R"(..\testCodes\resource\shaders\pbr\spmap_cs.glsl)";
		auto csEnvDiffPath = R"(..\testCodes\resource\shaders\pbr\irmap_cs.glsl)";
		auto skyBoxPath = R"(..\testCodes\resource\objs\sphere.obj)";
		int winW = 1024, winH = 1024;
	//	Utils::init();
	//	P3DEngine::instance().init(APP_OPENGL);
		//auto win = P3DEngine::instance().createWindow(winW, winH);
		const uint32 envDiffMapSize = 32;
		const uint32 envSpeLutSize = 256;


		auto texHdr = loadTex(hdrPath);
		cubeTex = P3DEngine::instance().createTexture(1024, 1024, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1, 0);


		//converte hdr to cubemap
		auto cs2Cube = P3DEngine::instance().createShader();
		cs2Cube->loadComputeShader(cs2CubePath);
		cs2Cube->setUniformTex("inputTexture", texHdr);
		cs2Cube->setTextureImg(cubeTex, 0);
		cs2Cube->run({ cubeTex->Width() / 32,cubeTex->Height() / 32,6 });


		//calcaulate env spec map
		envSpMap = P3DEngine::instance().createTexture(1024, 1024, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1, 0);
		envSpMap->copyFromTex(cubeTex, 0, 0);
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
			csEnvSpMap->setUniformF("roughness", i * deltaRoughness);
			//csEnvSpMap->setUniformF("roughness", 1.0);
			csEnvSpMap->run({ nGroup,nGroup,6 });
			tSize /= 2;
		}
		//	cubeTex.reset();
			// calculate env diffuse map
		envDiffseMap = P3DEngine::instance().createTexture(envDiffMapSize, envDiffMapSize, TEXTURE_CUBE, SAMPLE_LINER, READ_WRITE, PIXEL_RGBAF, 1, 1);
		auto csEnvDiff = P3DEngine::instance().createShader();
		csEnvDiff->loadComputeShader(csEnvDiffPath);
		csEnvDiff->setUniformTex("inputTexture", envSpMap);
		csEnvDiff->setTextureImg(envDiffseMap, 0, 0);
		csEnvDiff->run({ envDiffMapSize / 32,envDiffMapSize / 32,6u });
	}
	void initUniforms() {
	//	auto 
	}
	virtual void onUpdateRender(float deltTime) {
		//renderNode1->addUniform(std::make_shared<P3D::ShaderUniform>("u_color", uColor));
	//	sceneManager->updateSkins(deltTime);
	//	sceneManager->updateRenderables();
	//	renderNode1->render();
		//renderNode2->render();
		//std::cout << "innter render" << std::endl;
	//	renderNodeManager->renderAll();
		auto camera = sceneManager->getCamera("mainCam");
		
		camera->setViewParams(camPos, { 0.0,camPos.y,0.0 }, { 0.0,1.0,0.0 });
		camera->setProjParams(fov, winW*1.0f / winH, 0.1f, 100.0f);
		renderNodeManager->renderAll();
		
		auto diffuseFbo = sceneManager->getRenderFrame("irraMap_fbo");
		auto diffuseImg = diffuseFbo->getRenderTextureCpu(0, true, PIXEL_RGBF);
		auto cvDiffuse = CvUtil::toCvMat(diffuseImg->buffers[0]);
		cv::Mat diffuseN;
		cv::flip(cvDiffuse, diffuseN, 0);
	//	cv::imwrite("diffuse.exr", diffuseN);

		auto fbo = sceneManager->getRenderFrame("forwardFbo");
		GLUtils::render2Quad(fbo->getRenderTexture(0));
		auto t_img = fbo->getRenderTextureCpu(0, true,PIXEL_RGBF);
		auto cvImg = CvUtil::toCvMat(t_img->buffers[0]);
		cv::Mat cvImgN;
		cv::flip(cvImg, cvImgN, 0);
		//cv::imwrite("src.exr", cvImgN);
		auto dstDir = R"(D:\work\projects\P3DKit\tmp\HDR_preFilter_new)";
		for (auto tName:nodeNames) {
			auto tNode = renderNodeManager->getRenderNode(tName);
			auto tFbo = sceneManager->getRenderFrame(tNode->getRenderFrameName());
			auto tImg = tFbo->getRenderTextureCpu(0, true, PIXEL_RGBF);
			cvImg = CvUtil::toCvMat(tImg->buffers[0]);
			cv::Mat cvImg2;
			cv::flip(cvImg, cvImg2, 0);
			auto ttName = Utils::getBaseName(winTittle);
			auto fileName = Utils::splitString(ttName,'.')[0];
			cv::imwrite(Utils::joinPath(dstDir, fileName +"_"+ tName + ".exr"), cvImg2);
		}
		//win->renderTexture(fbo->getRenderTexture(0));
	}
	virtual void onUpdateUI() {	     
		return;
		ImGui::Begin("shader Uniforms");
		ImGui::ColorEdit4("clear color", (float*)&uColor); // Edit 3 floats representing a color
		if (ImGui::Button("reload shader"))
			reloadShader();
		auto renderNode = renderNodeManager->getRenderNode("base");
		if(ImGui::Button("env"))
			renderNode->addUniform(std::make_shared<ShaderUniform>("envTexture", cubeTex));
		else if(ImGui::Button("dif"))
			renderNode->addUniform(std::make_shared<ShaderUniform>("envTexture", envDiffseMap));
		else if(ImGui::Button("spe"))
			renderNode->addUniform(std::make_shared<ShaderUniform>("envTexture", envSpMap));
	//	if (ImGui::Button("reload_renderNodes"))
//			loadNodeCfg();
		ImGui::SliderFloat("CamPosZ", &(camPos.z),0.5,20);
		ImGui::SliderFloat("CamPosY", &(camPos.y),-0.5,0.5);
		ImGui::SliderFloat("fov", &fov, 5.0f, 60.0f);

		//Value tv;
		//sceneManager->getValue("lod",tv);
		//ImGui::SliderFloat("lod_ralate", tv.getDataBlock()->rawData<float32>(), 0.0, 1.0);

		ImGui::End();

	}
	void loadNodeCfg() {
		renderNodeManager->paraseFromeFile(renderNodeCfg);
	}
	void reloadShader() {
		auto allShaders = sceneManager->getAllShaders();
		for (auto t : allShaders)
			t.second->reload();
	}
	virtual void onShuwdown() {};
private:
	P3D::pRenderNode renderNode1;
	P3D::pRenderNode renderNode2;
	P3D::pSceneManager sceneManager;
	P3D::pRenderNodeManager renderNodeManager;
	P3D::Vec4f uColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	std::string renderNodeCfg;
	P3D::Vec3f camPos = { 0.0,0.1,1.2 };
	float32 fov = 20.0f;

	pTexture cubeTex, envSpMap, envDiffseMap;
	std::vector<std::string> nodeNames;
		
};
int main(int argc,char* argv[]) {

	auto app = std::make_shared<testApp>(1024, 512, argv[1]);
	app->init();
	app->runAFrame();
	//app->run();
	//cv::Mat t(512, 512,CV_32FC3);
	//cv::imwrite("test.exr", t);
	//std::cout<<"shader dir is "<< P3D::P3DEngine::instance().getShaderRootDir() << std::endl;
	return 0;
}