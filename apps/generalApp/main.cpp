#include "baseApp/baseApp.h"
#include "common/P3D_Utils.h"
#include "engine/mgs/renderNodeManager.hpp"
#include "libs/imgui/imgui.h"
#include "loader/P3DLoader.h"
#include "products/faceDetector/ycnnFaceDetector.h"
#include "products/faceDetector/faceBase.h"
#include <opencv2/opencv.hpp>
#include "products/face3D/Face3dOptSolveKFMCeres.h"
#include "ceres/ceres.h"
#include "baseApp/generalApp.h"

using namespace P3D;
std::string videoPath;

class testApp :public BaseApp {
public:
	//BaseApp(uint32 w, uint32 h, const std::string&tittle = "Demo");
	testApp(uint32 w,uint h,const std::string&tittle="Demo"):BaseApp(w,h,tittle) {
	}
	virtual void onInit() {
		faceDect = std::make_shared<YCNNFaceDetector>();
		auto modelDir = "../facemodels/ycnn";
 		faceDect->loadModel(modelDir);
		faceDect->setMode(P3D::IFaceDetector::Mode::Video);
		faceRet = std::make_shared<Face3dResult>();

		face3DSolver = std::make_shared<Face3dOptSolveKFMCeres>();
		auto face3dDataPath = "../facemodels/face_models/basewhite.mmux";
		auto memDataStore = std::make_shared<MemDataStore>(face3dDataPath);
		face3DSolver->init(memDataStore,EXPType_51);

		videoCap.open(0);
		//videoCap.open(videoPath);
		videoCap.read(videoImg);
		// CpuImage(TextureType t,PixelFormat format, uint32 w,uint32 h,uint32 d,bool useuint8 = true);
		sceneManager = P3DEngine::instance().getSceneManager();
		auto cpuImg = std::make_shared<P3D::CpuImage>(P3D::TEXTURE_2D, P3D::PixelFormat::PIXEL_RGBA8, videoImg.cols, videoImg.rows,1);
		auto tTex = P3D::P3DEngine::instance().createTexture(cpuImg);
		sceneManager->addTex("video_tex", tTex);


		auto renderNoderManger = P3DEngine::instance().getRenderNodeManager();
		auto vsPath = Utils::getAbsPath("../tmp/apps/faceApp/shaders/head.vert");
		auto fsPath = Utils::getAbsPath("../tmp/apps/faceApp/shaders/head.frag");
		renderNode1 = renderNoderManger->createRenderNode(P3D::RENDERNODE_CUSTOM, "test", vsPath, fsPath, "$screen", { 1.0f,1.0f });
	//	auto skinMesh = P3DLoader::instance().loadFbxMesh(R"(D:\Work\Projects\ProNew\Vray\tmp\apps\head.fbx)",true);
	//	skinMesh->setKeyAnimationFrame(0);
	//	sceneManager->addSkinMesh("human",skinMesh);
	//	renderNode1->addRenderable("head");
		renderNode2 = renderNoderManger->createRenderNode(P3D::RENDERNODE_IMG_FILTER, "copy", "", "copy_fs.frag", "$screen");
		renderNode2->addUniform(std::make_shared<P3D::ShaderUniform>("s_tex", sceneManager->getTex("video_tex")));
		auto tCam = std::make_shared<Camera>();
		tCam->setCameraType(P3D::Camera::CAMERA_TYPE_PRO);
		//tCam->setViewParams({ 0.0,0.0,150.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });
		tCam->setViewMat(Mat4f::getIdentity());
		float fovDegree = P3D::Utils::toDegree(1.0f);
		float imgAsp = videoImg.cols * 1.0f / videoImg.rows;
		tCam->setProjParams(fovDegree, imgAsp, 0.01, 1000.0);
		//auto tMat = Mat4f::getTranslate({ 0.0,-175.0,0.0 });
		//sceneManager->setSkinModelMat("human", tMat);
		sceneManager->addCamera("cam0", tCam);
		renderNode1->setCamera("cam0");
	};
	void initUniforms() {
	//	auto 
	}
	virtual void onUpdateRender(float deltTime) {
		//renderNode1->addUniform(std::make_shared<P3D::ShaderUniform>("u_color", uColor));
		cv::Mat tImg;
		if (!videoCap.read(tImg)) {
			videoCap.release();
			//videoCap.open(videoPath);
			videoCap.open(0);
			videoCap.read(tImg);
		}
		cv::flip(tImg, tImg, 1);
		runFaceArith(tImg);
		auto tTex = sceneManager->getTex("video_tex");
		auto cpuImg = std::make_shared<P3D::CpuImage>(P3D::TEXTURE_2D, P3D::PixelFormat::PIXEL_RGBA8, videoImg.cols, videoImg.rows,1);
		cpuImg->setFromBuffer(videoImg.data);
		tTex->updateSource(cpuImg);
		//sceneManager->updateSkins(deltTime);
		sceneManager->updateRenderables();
		renderNode2->render();
		renderNode1->render();
	}
	virtual void onUpdateUI() {	
		ImGui::Begin("shader Uniforms");
		ImGui::ColorEdit4("clear color", (float*)&uColor); // Edit 3 floats representing a color
		if (ImGui::Button("reload shader"))
			reloadShader();
		ImGui::End();
	}
	void reloadShader() {
		auto allShaders = sceneManager->getAllShaders();
		for (auto t : allShaders)
			t.second->reload();
	}
	void runFaceArith(cv::Mat& srcImg) {
		auto img = P3D::FaceExtUtils::toNV21Image(srcImg, false, P3D::IMAGE_ROT0);
		std::vector<P3D::FaceInfo> faces;
		auto time1 = Utils::getCurrentTime();
		faces = faceDect->detect(img, time1);
		videoImg = srcImg.clone();
		for (auto tFace : faces) {
			auto& lds = tFace.landmarks;
			for (int i = 0; i < lds.shape()[0]; ++i) {
				int x = lds[{i, 0}];
				int y = lds[{i, 1}];
				cv::circle(videoImg, { x,y }, 3, { 0,255,0 }, -1);
			}
		}
		cv::flip(videoImg, videoImg, 0);
		cv::cvtColor(videoImg, videoImg, cv::COLOR_BGR2RGBA);

		//run face3d
		
		auto timeInMs = time1 * 1000;
		if (!faces.empty()){
			auto faceConfig = std::make_shared<Face3dConfig>();
			//face cofnig 
			faceConfig->b_mouth_mesh_space_is_world = true;
			faceConfig->b_face_mesh_space_is_world = true;
			faceConfig->uvType = 0;
			faceConfig->isUseOptFLow = false;
			faceConfig->isFast = false;
			//face ret
			//face tracker
			if (faceTracker == nullptr)
				faceTracker = std::make_shared<Face3dOptSolveKFMCeresTrackInfo>();

			auto faceLds = faces[0].landmarks.toVector1();
			auto extraLds = faces[0].ycnn_extra_landmarks.toVector1();
			const auto&rect = faces[0].rect;
			Rect retcI({ int(rect.x), int(rect.y), int(rect.x) + int(rect.width), int(rect.y) + int(rect.height) });
			face3DSolver->fit(
				img, img->width(), img->height(), 1.0,
				0, retcI,
				faceLds, extraLds, timeInMs, faceConfig,
				faceRet, faceTracker, EXPType_51);
			const auto& tIds = faceTracker->pre_solution->id;
			std::cout << "face id is " << std::endl;
			for (auto t : tIds)
				std::cout << t << " ";
			std::cout << std::endl;
			
			//auto faceMesh = faceRet->face_mesh;
			updateFaceMesh(faceRet);
			int aaa = 0;
		}
	}
	void updateFaceMesh(std::shared_ptr<Face3dResult> tResult) {
		auto tFace = tResult->face_mesh;
		if (tFace) {
			auto vpsData = std::make_shared<DataBlock>(tFace->vps.data(), tFace->vps.size() * sizeof(float), false, DType::Float32);
			auto uvsData = std::make_shared<DataBlock>(tFace->uvs.data(), tFace->uvs.size() * sizeof(float), false, DType::Float32);
			auto tlData = std::make_shared<DataBlock>(tFace->tl.size() * sizeof(uint32), DType::Uint32);
			auto tlPtr = tlData->rawData<uint32>();
			for (int i = 0; i < tFace->tl.size(); ++i)
				tlPtr[i] = tFace->tl[i];
			if (faceComp == nullptr) {
				faceComp = std::make_shared<P3D::P3DComp>(vpsData,tlData,uvsData,nullptr);
				faceComp->recalculNormals();
				sceneManager->addComp("face", faceComp);
				renderNode1->addRenderable("face");
			}
			else {

				//faceComp->cache_vps = std::make_shared<DataBlock>(tFace->vps.data(), tFace->vps.size() * sizeof(float), true, DType::Float32);
				faceComp->cache_vps->copyData(tFace->vps.data(), faceComp->cache_vps->getSize(), 0);
				sceneManager->updateRenderables();
			}
			//if(faceComp)
			//	faceComp->recalculNormals();
			Mat4f faceMat(tFace->outerMatrix);
		//	std::cout << "face Mat is " << std::endl;
			//faceMat.print();
			sceneManager->setCompModelMat("face", faceMat);
		}
	}
	void threadVideo() {
		while (!threadStop) {
			if (!videoCap.isOpened())
				videoCap.open(0);
			cv::Mat tImg;
			videoCap.read(tImg);
			runFaceArith(tImg);
		}
	}
	virtual void onShuwdown() {
		threadStop = true;
	};

private:
	P3D::pRenderNode renderNode1;
	P3D::pRenderNode renderNode2;
	P3D::pSceneManager sceneManager;
	P3D::pFaceDetector faceDect;
	P3D::pComp faceComp = nullptr;
	P3D::Vec4f uColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	cv::Mat videoImg;
	cv::VideoCapture videoCap;
	
//	std::string videoPath;
	bool threadStop = false;
	pFace3DKFMSolver face3DSolver;
	std::shared_ptr<P3D::Face3dOptSolveKFMCeresTrackInfo> faceTracker = nullptr;
	std::shared_ptr<P3D::Face3dResult> faceRet=nullptr;
};
struct CostFunctor {
   template <typename T>
   bool operator()(const T* const x, T* residual) const {
     residual[0] = 10.0 - x[0];
     return true;
   }
};

void testCeres(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);

  // The variable to solve for with its initial value.
  double initial_x = 5.0;
  double x = initial_x;
  using namespace ceres;

  // Build the problem.
  Problem problem;

  // Set up the only cost function (also known as residual). This uses
  // auto-differentiation to obtain the derivative (jacobian).
  CostFunction* cost_function =
      new AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
  problem.AddResidualBlock(cost_function, nullptr, &x);

  // Run the solver!
  ceres::Solver::Options options;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = true;
  Solver::Summary summary2;
  ceres::Solve(options, &problem, &summary2);

  std::cout << summary2.BriefReport() << "\n";
  std::cout << "x : " << initial_x
            << " -> " << x << "\n";
}


class TApp :public GeneralApp {
public:

	TApp(int w, int h, const std::string& tName = "demo") :GeneralApp(w, h, tName) {}
public:
	virtual void onUpdateUI() {
		GeneralApp::onUpdateUI();
		auto ttLit = sceneManager->getAllLights()["pointLight3"];
		auto tLit = std::dynamic_pointer_cast<PointLight>(ttLit);
	// tLit->litColor
		ImGui::Begin("pbr");
		ImGui::SliderFloat3("litColor", (float*)&(tLit->litPos),0.0,100.0);
		auto material = std::dynamic_pointer_cast<P3DMaterialPbr>(sceneManager->getMaterial("pbr_base"));
		ImGui::SliderFloat("roughness", (float*)(material->roughness.rawData->rawData()), 0.0, 1.0);
		ImGui::SliderFloat("metalic", (float*)(material->matallic.rawData->rawData()), 0.0, 1.0);
		ImGui::End();
	}
};
int main(int argc, char** argv) {

//	testCeres(argc,argv);
	/*videoPath = "../tmp/data/videos/test_video.mp4";
	cv::VideoCapture cap;
	cap.open(0);
	//cap.open(videoPath);
	auto tt = cap.isOpened();
	cv::Mat tImg;
	cap.read(tImg);
	cap.release();
	auto app = std::make_shared<testApp>(tImg.cols, tImg.rows, "Demo");
	app->run();
	*/
	//std::cout<<"shader dir is "<< P3D::P3DEngine::instance().getShaderRootDir() << std::endl;
	auto myApp = std::make_shared<TApp>(1280,720,"uiTest");
	myApp->init();
	auto baseDir = R"(D:\work\projects\P3DKit\tmp\apps\pbr)";
	//auto uiCfg = Utils::joinPath(baseDir, "UiCfg.json");
	//auto renderCfg = Utils::joinPath(baseDir, "renderNodesCfg.json");
	auto sceneCfg = Utils::joinPath(baseDir, "scenPro.json");
	myApp->initFromCfgs(sceneCfg);
	myApp->runFrames();


	return 0;
}