#pragma once
//#include "common/P3D_Utils.h"
#include "../common/faceArithBase.h"
#include "../common/Image.h"
#include <opencv2/opencv.hpp>

BEGIN_P3D_NS
typedef cv::Mat_<cv::Vec3b> CvColorImage;
typedef cv::Mat_<uint8_t> CvGreyImage;

struct CvUtil {

	static cv::Mat toCvMat(const PNdArray& arr);//cv::Mat
	static cv::Mat toCvMat(const pImage& arr);

    static pImage toNV21Image(const cv::Mat& img,
                              bool useMemPool = false,
                              IMAGE_ROTATION rotation = IMAGE_ROT0,
                              IMAGE_MIRROR mirror = IMAGE_MIRROR_NO);
    static pImage toGrayImage(const cv::Mat& img,
                              bool useMemPool = false,
                              IMAGE_ROTATION rotation = IMAGE_ROT0,
                              IMAGE_MIRROR mirror = IMAGE_MIRROR_NO);

    static int calcNv21DataSize(int width, int height);
    static int calcGrayDataSize(int width, int height);
    static ByteBuffer cvtI420ToNv21(int width, int height, const char* data);
    static void cvtI420ToNv21(int width, int height, const char* data, char* dst);

#ifdef IS_DESKTOP
#ifndef CVUTILS_BUILD_PUBS_FOR_RELEASE
	static PNdArray readNdArrayImage(cstring fpath, IMAGE_FORMAT format, bool useCHWLayout = false);
	static void writeNdArrayImage(cstring fpath, const PNdArray& img, IMAGE_FORMAT format, bool useCHWLayout = false);
	static pImage readNV21Image(cstring fname,
		bool useMemPool = false,
		IMAGE_ROTATION rotation = IMAGE_ROT0,
		IMAGE_MIRROR mirror = IMAGE_MIRROR_NO);
	static pImage readGrayImage(cstring fname,
		bool useMemPool = false,
		IMAGE_ROTATION rotation = IMAGE_ROT0,
		IMAGE_MIRROR mirror = IMAGE_MIRROR_NO);
	static void showWait(std::string name, const cv::Mat& img);
	static cv::Mat toCvImage(const PNdArray& arr);
	static PNdArray toNdArray(const cv::Mat& arr);
	static void showLines(const std::string&winName, Size winSize, std::vector<std::string>& lines);
	static void imshow(const std::string& name, const PNdArray& img, float scale = 1);
	static void imshow(const std::string& name, const cv::Mat& img, float scale = 1);
	static void imwrite(const std::string& fname, const PNdArray& img);
//	static void imwrite(const std::string& fname, pImage img);
	static CvGreyImage toGrey(const CvColorImage& img);
	static cv::Rect toCv(const Rect& rect);
	static cv::Rect toCv(const RectF& rect);
	static Rect toRect(const cv::Rect& rect);
	static void drawLandmarks(cv::Mat&img, const PNdArrayI& landmarks, int radius, cv::Scalar color, bool drawIndices = false);
	static void drawLandmarks(cv::Mat&img, const PNdArrayF& landmarks, int radius, cv::Scalar color, bool drawIndices = false);
	static void drawRect(cv::Mat&img, const RMRect& rect, cv::Scalar color);
	static void drawRect(cv::Mat&img, const Rect& rect, cv::Scalar color);
	static void drawRect(cv::Mat&img, const RectF& rect, cv::Scalar color);
	static void drawMaskImage(cv::Mat3b&img, RMRect rect, const PNdArrayF& mask, cv::Scalar color);
	static cv::Mat_<double> readJsonMatrix(cstring fname);
	//static void drawText(cv::Mat& img, const std::string& text, Point2I p, int fontScale=1, cv::Scalar fontColor=cv::Scalar(0,0,255), int lineType = 2);
	static void drawCenterText(cv::Mat& img, const std::string& text, RectF rect, int fontScale=1, cv::Scalar fontColor=cv::Scalar(0, 0, 255), int lineType=2);
	static cv::Mat scaleImage(const cv::Mat&img, float scale);
#endif
#endif

	static void registerCvImpls();
};

class CvPnpSolver {
public:
	bool solve(const PNdArrayF& objectPoints, const PNdArrayF& imagePoints,
		const PNdArrayD& cameraMatrix, const PNdArrayD& distCoeffs,
		/*out*/PNdArrayF& rvec, /*out*/PNdArrayF& tvec, bool useExtrinsicGuess);
};
END_P3D_NS
