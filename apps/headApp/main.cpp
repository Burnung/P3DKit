#include "baseApp/baseApp.h"
#include "common/P3D_Utils.h"
#include "engine/mgs/renderNodeManager.hpp"
#include "libs/imgui/imgui.h"
#include "loader/P3DLoader.h"
#include "libs/glm/glm/gtc/matrix_transform.hpp"
using namespace P3D;
class EyeRotattionCtrl {
public:
	EyeRotattionCtrl() = default;
	~EyeRotattionCtrl() = default;
	Vec3f getRot(const Vec3f& srcVec, const Vec3f&dstVec) {
		auto rotAxis = srcVec.cross(dstVec).normalize();
		if (rotAxis.length() < 1e-4)
			return { 0,0,0 };
		auto angle = Utils::toDegree(acos(srcVec.dot(dstVec)));
		auto quat = Quaternionf::getRotateQ(rotAxis, angle);
		auto eulerRotation = quat.toRotation();
		return eulerRotation;

	}
	std::vector<Vec3f> getBoneRotation() {
		Vec3f targetPos;
		auto rPhi = Utils::toRadian(phi);
		auto rTheta = Utils::toRadian(theta);

		targetPos.y = radius * sin(rPhi);
		targetPos.x = radius * cos(rPhi)*sin(rTheta);
		targetPos.z = radius * cos(rPhi)*cos(rTheta);

		auto leftVec = (targetPos - leftEyePos).normalize();
		auto leftRot = getRot(srcVec, leftVec);
	
		auto rightVec = (targetPos - rightEyePos).normalize();
		auto rightRot = getRot(srcVec, rightVec);

		return {leftRot, rightRot};
	}
public:
	float32 theta = 0.0f;
	float32 phi = 0.00f;
	float32 radius = 10000;
	Vec3f srcVec = { 0.0f, 0.0f, 1.0f };
	Vec3f leftEyePos = { 3.16f,0.0f,0.0f };
	Vec3f rightEyePos = { -3.16f,0.0f,0.0f };

};
class testApp :public BaseApp {
public:
	//BaseApp(uint32 w, uint32 h, const std::string&tittle = "Demo");
	testApp(uint32 w,uint h,const std::string&tittle="Demo"):BaseApp(w,h,tittle) {
	}
	virtual void onInit() {
		sceneManager = P3DEngine::instance().getSceneManager();
		renderNodeManager = P3DEngine::instance().getRenderNodeManager();
		auto t = eyeCtrl.getBoneRotation();
		
		renderNodeCfg = R"(D:\Work\Projects\ProNew\Vray\tmp\apps\renderNodesCfg.json)";
		auto scenePath = R"(D:\Work\Projects\ProNew\Vray\tmp\apps\headApp\scenProHead.json)";
		sceneManager->paraseFromFile(scenePath);
		auto glmMat = glm::lookAt({ camPos.x,camPos.y,camPos.z }, glm::vec3(0.0f, camPos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		std::cout << "glm is " << std::endl;
		for (int r = 0; r < 4; ++r) {
			for (int c = 0; c < 4; ++c)
				std::cout << glmMat[r][c] << " ";
			std::cout << std::endl;
		}
		
		auto mMat = Mat4f::getLookAt(camPos, { 0.0,camPos.y,0.0 }, { 0.0,1.0,0.0 });
		std::cout << "my mat is " << std::endl;
		mMat.print();
		/*renderNodeManager->paraseFromeFile(renderNodeCfg);
		auto vsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\apps\shaders\skin.vert)";
		auto fsPath = R"(D:\Work\Projects\ProNew\Vray\tmp\apps\shaders\skin.frag)";
		//renderNode1 = renderNodeManager->createRenderNode(P3D::RENDERNODE_CUSTOM,"test",vsPath,fsPath,"pass1",1.0);
		auto skinMesh = P3DLoader::instance().loadFbxMesh(R"(D:\Work\Projects\ProNew\Vray\tmp\apps\head.fbx)",true);
		skinMesh->setKeyAnimationFrame(0);
		sceneManager->addSkinMesh("human",skinMesh);
		auto tMat = Mat4f::getTranslate({ 0.0,-175.0,0.0 });
		sceneManager->setSkinModelMat("human", tMat);
		std::cout << "init scene ok" << std::endl;*/
		//renderNode1->addRenderable("head");
		//renderNode2 = renderNodeManager->createRenderNode(P3D::RENDERNODE_IMG_FILTER, "copy", "", "copy_fs.frag", "$screen");
		//renderNode2->addUniform(std::make_shared<P3D::ShaderUniform>("s_tex", sceneManager->getTex("head_diffuse")));
		/*auto tCam = std::make_shared<Camera>();
		tCam->setCameraType(P3D::Camera::CAMERA_TYPE_PRO);
		tCam->setViewParams({ 0.0,0.0,150.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
		tCam->setProjParams(20.0f, 1.0, 0.01, 1000.0);
		sceneManager->addCamera("cam0", tCam);
		renderNode1->setCamera("cam0");*/
	};
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
		auto camera = sceneManager->getCamera("mainCam");
		camera->setViewParams(camPos, { 0.0,camPos.y,0.0 }, { 0.0,1.0,0.0 });
		camera->setProjParams(fov, winW*1.0f / winH, 0.1f, 100.0f);

		std::unordered_map<std::string, std::vector<float32>> boneInfos;
		boneInfos["LeftEye"] = { 1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,3.160 ,-1.372,9.413 };
		boneInfos["RightEye"] = { 1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,-3.160 ,-1.372,9.413 };
		auto eyeRotations = eyeCtrl.getBoneRotation();
		for (int i = 0; i < 3; ++i) {
			boneInfos["LeftEye"][i + 3] = eyeRotations[0][i];
			boneInfos["RightEye"][i + 3] = eyeRotations[1][i];
		}
		
		sceneManager->setBoneData("skin1", boneInfos);
		sceneManager->updateSkins(deltTime);
		renderNodeManager->renderAll();
	}
	virtual void onUpdateUI() {	     
		ImGui::Begin("shader Uniforms");
		ImGui::ColorEdit4("clear color", (float*)&uColor); // Edit 3 floats representing a color
		if (ImGui::Button("reload shader"))
			reloadShader();
	//	if (ImGui::Button("reload_renderNodes"))
//			loadNodeCfg();
		ImGui::SliderFloat("CamPosZ", &(camPos.z),0.5,20);
		ImGui::SliderFloat("CamPosY", &(camPos.y),-0.5,0.5);
		ImGui::SliderFloat("fov", &fov, 5.0f, 60.0f);

		ImGui::SliderFloat("Dst_Phi", &(eyeCtrl.phi),-20,20);
		ImGui::SliderFloat("Dst_Theta", &(eyeCtrl.theta),-20,20);
		ImGui::SliderFloat("Dst_Raduis", &(eyeCtrl.radius),50,1000);
		
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
	EyeRotattionCtrl eyeCtrl;
};
int main() {
	auto app = std::make_shared<testApp>(1280, 720, "Demo");
	app->run();
	//std::cout<<"shader dir is "<< P3D::P3DEngine::instance().getShaderRootDir() << std::endl;
	return 0;
}