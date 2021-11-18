#pragma once
#include "common/NdArray.h"
#include "products/face3D/Face3dOptSolveKFMCeres.h"
#include "products/faceDetector/ycnnFaceDetector.h"
#include "cv_common/CvUtil.h"
BEGIN_P3D_NS

class Face3DPython :public P3DObject {
public:
	Face3DPython() = default;
	~Face3DPython() = default;
public:
	void init(cstring ycnnDir, cstring face3dPath, cstring mode = "video",bool useLightFlow=true) {
		Utils::init();
		faceDetector = std::make_shared<YCNNFaceDetector>();
		faceDetector->loadModel(ycnnDir);

		face3DSolver = std::make_shared<Face3dOptSolveKFMCeres>();
		auto memDataStore = std::make_shared<MemDataStore>(face3dPath);
		face3DSolver->init(memDataStore,EXPType_51);
		if (mode == "image")
			isImage = true;
		if (isImage) {
			faceDetector->setMode(IFaceDetector::Mode::Image);
		}
		else {
			faceDetector->setMode(IFaceDetector::Mode::Video);
		}
		faceConfig = std::make_shared<Face3dConfig>();
		faceConfig->b_mouth_mesh_space_is_world = true;
		faceConfig->b_face_mesh_space_is_world = true;
		faceConfig->b_eye_mesh_space_is_world = true;
		faceConfig->b_eyebrow_mesh_space_is_world = true;
		faceConfig->uvType = 0;
		faceConfig->isUseOptFLow = useLightFlow;
		faceConfig->isFast = false;
		faceTracker = std::make_shared<Face3dOptSolveKFMCeresTrackInfo>();
		faceRet = std::make_shared<Face3dResult>();
	}
	bool calcLandmarks(PNdArray srcImg) {
		auto cvImg = CvUtil::toCvImage(srcImg);

		auto img = CvUtil::toNV21Image(cvImg);
		//	std::cout << "convert to nv21 img" << std::endl;
		auto time1 = Utils::getCurrentTime();
		auto timeInMs = time1 * 1000;
		//std::cout << "begin detect face" << std::endl;
		auto faces = faceDetector->detect(img, time1);

		if (faces.empty())
			return false;
		//取得人脸关键点
		landmarks2D = faces[0].landmarks;
	}
	bool calc(PNdArray srcImg) {
	//	std::cout << "inter calc" << srcImg.shape()[0]<<std::endl;
		auto cvImg = CvUtil::toCvImage(srcImg);
		
		auto img = CvUtil::toNV21Image(cvImg);
	//	std::cout << "convert to nv21 img" << std::endl;
		auto time1 = Utils::getCurrentTime();
		auto timeInMs = time1 * 1000;
		//std::cout << "begin detect face" << std::endl;
		auto faces = faceDetector->detect(img, time1);

		if (faces.empty())
			return false;
		//取得人脸关键点
		landmarks2D = faces[0].landmarks;

		auto faceLds = faces[0].landmarks.toVector1();
		auto extraLds = faces[0].ycnn_extra_landmarks.toVector1();
		const auto& rect = faces[0].rect;
		//Rect retcI({ int(rect.x), int(rect.y), int(rect.x) + int(rect.width), int(rect.y) + int(rect.height) });
		Rect retcI({ int(rect.x), int(rect.y), int(rect.width),int(rect.height) });
		if (!isImage) {
			face3DSolver->fit(
				img, img->width(), img->height(), fov,
				0, retcI,
				faceLds, extraLds, timeInMs, faceConfig,
				faceRet, faceTracker, EXPType_51);
		}
		else {
			face3DSolver->fit_image(img, img->width(), img->height(),fov, 0, retcI,
				faceLds, extraLds, timeInMs, faceConfig, faceRet, faceTracker, EXPType_51);
		}
		calcLds = faceRet->calcLds;
		return true;
	}
	bool calcMultiImg(std::vector<PNdArray> srcImgs, const std::vector<std::vector<float>>& exps = {}) {
		std::vector<pImage> imgs(srcImgs.size());
		std::vector<PNdArrayF> faceLds(srcImgs.size());
		std::vector<PNdArrayF> extraLds(srcImgs.size());
		std::vector<Rect> rects(srcImgs.size());
		for (int i = 0; i < srcImgs.size(); ++i) {
			auto tmp = CvUtil::toCvImage(srcImgs[i]);
			imgs[i] = CvUtil::toNV21Image(tmp);
			auto tFaces = faceDetector->detect(imgs[i]);
			if (tFaces.empty())
				return false;
			faceLds[i] = tFaces[0].landmarks;
			extraLds[i] = tFaces[0].ycnn_extra_landmarks;
			const auto& rect = tFaces[0].rect;
			Rect rectI({ int(rect.x), int(rect.y), int(rect.x) + int(rect.width), int(rect.y) + int(rect.height) });
			rects[i] = rectI;
		}
		allLandmars = faceLds;
		int imgW = imgs[0]->width(), imgH = imgs[0]->height();
		faceRets.clear();
		if (exps.size() != imgs.size())
			face3DSolver->fitMultiImages(imgs, imgW, imgH, 1.0, 0, rects, faceLds, \
				extraLds, 0.0, faceConfig, faceRets, faceTracker, KFM_EXPType::EXPType_51, false);
		else
			face3DSolver->fitMulImgsWithExp(imgs, exps, imgW, imgH, 1.0, rects, faceLds, faceConfig, faceRets, faceTracker, KFM_EXPType::EXPType_51);

		for (int i = 0; i < faceRets.size(); ++i)
			allcalcLandmars.push_back(faceRets[i]->calcLds);
		faceRet = faceRets[0];
	}
	
public:
	float fov = 1.0;
	PNdArrayF landmarks2D, calcLds;
	std::vector<PNdArrayF> allLandmars;
	std::vector<PNdArrayF> allcalcLandmars;
	std::shared_ptr<Face3dResult> faceRet;
	std::shared_ptr<Face3dConfig> faceConfig;
	std::vector<std::shared_ptr<Face3dResult>> faceRets;
	std::shared_ptr<Face3dOptSolveKFMCeresTrackInfo> faceTracker;
private:
	bool isImage = false;
	pFaceDetector faceDetector;
	pFace3DKFMSolver face3DSolver;
	
};


END_P3D_NS
