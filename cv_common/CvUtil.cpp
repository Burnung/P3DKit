#include "CvUtil.h"
#include "../common/json_inc.h"
#include <algorithm>
using namespace std;

BEGIN_P3D_NS

cv::Mat CvUtil::toCvMat(const pImage& img) {
	PNdArray arr = img->cropToArray(RectF(Rect(img->size())), img->size(), IMAGE_FORMAT_BGR);
	return toCvMat(arr);
}
cv::Mat CvUtil::toCvMat(const PNdArray& arr) { //cv::Mat
	auto dtype = arr.dtype();
	if (dtype != DType::UInt8 && dtype != DType::Int32
		&& dtype != DType::Float32 && dtype != DType::Float64)
		P3D_THROW_RUNTIME("Invalid dtype = %d", dtype);
	int width, height;
	auto ndim = arr.ndim();
	const auto& shape = arr.shape();
	cv::Mat rlt;
	if (ndim < 2)
		P3D_THROW_RUNTIME("Invalid ndim %d", ndim);
	height = shape[0];
	width = shape[1];
	if (ndim == 2 || (ndim == 3 && shape[2] == 1)) {
		switch (dtype) {
			case DType::UInt8:
				rlt = cv::Mat1b::zeros(height, width);
				break;
			case DType::Int32:
				rlt = cv::Mat1i::zeros(height, width);
				break;
			case DType::Float32:
				rlt = cv::Mat1f::zeros(height, width);
				break;
			case DType::Float64:
				rlt = cv::Mat1d::zeros(height, width);
				break;
			default: break;
		}
	}
	else if (ndim == 3 && shape[2] == 2) {
		switch (dtype) {
			case DType::UInt8:
				rlt = cv::Mat2b::zeros(height, width);
				break;
			case DType::Int32:
				rlt = cv::Mat2i::zeros(height, width);
				break;
			case DType::Float32:
				rlt = cv::Mat2f::zeros(height, width);
				break;
			case DType::Float64:
				rlt = cv::Mat2d::zeros(height, width);
				break;
			default: break;
		}
	}
	else if (ndim == 3 && shape[2] == 3) {
		switch (dtype) {
			case DType::UInt8:
				rlt = cv::Mat3b::zeros(height, width);
				break;
			case DType::Int32:
				rlt = cv::Mat3i::zeros(height, width);
				break;
			case DType::Float32:
				rlt = cv::Mat3f::zeros(height, width);
				break;
			case DType::Float64:
				rlt = cv::Mat3d::zeros(height, width);
				break;
			default: break;
		}
	}
	else if (ndim == 3 && shape[2] == 4) {
		switch (dtype) {
		case DType::UInt8:
			rlt = cv::Mat4b::zeros(height, width);
			break;
		case DType::Int32:
			rlt = cv::Mat4i::zeros(height, width);
			break;
		case DType::Float32:
			rlt = cv::Mat4f::zeros(height, width);
			break;
		case DType::Float64:
			rlt = cv::Mat4d::zeros(height, width);
			break;
		default: break;
		}
	}
	else {
		P3D_THROW_RUNTIME("Invalid arr ndim=%d, nchannel=%d", ndim, ndim<=2?1:shape[2]);
	}
	int srcStride = arr.strides()[0];
	const char* psrc = arr.rawData();
	for (int i = 0; i < height; i++) {
		memcpy(rlt.ptr(i), psrc, srcStride);
		psrc += srcStride;
	}
	return rlt;
}

pImage CvUtil::toNV21Image(const cv::Mat& img, bool useMemPool, IMAGE_ROTATION rotation, IMAGE_MIRROR mirror) {
    int height = img.rows;
    int width = img.cols;
    if (height == 0 || width == 0) return nullptr;
    if ((width & 1) != 0 || (height & 1) != 0)
        P3D_THROW_RUNTIME("Invalid width=%d, height=%d", width, height);
    cv::Mat yuvI420(height + height / 2, width, CV_8UC1);
    cv::cvtColor(img, yuvI420, cv::COLOR_BGR2YUV_I420);
    if (useMemPool) {
        int dataSize = calcNv21DataSize(width, height);
        auto block = std::make_shared<DataBlock>(dataSize);
        CvUtil::cvtI420ToNv21(width, height, (const char*)yuvI420.data, block->rawData<char>());
        return std::make_shared<Image>(block->rawData(), width, height,
                                        IMAGE_FORMAT_NV21, rotation, mirror, block);
    }
    else {
        auto bytes = CvUtil::cvtI420ToNv21(width, height, (const char*)yuvI420.data);
        auto dataHolder = make_shared<DataHolder<ByteBuffer>>(bytes);
        return std::make_shared<Image>(dataHolder->d.data(), width, height,
                                        IMAGE_FORMAT_NV21, rotation, mirror, dataHolder);
    }
}

pImage CvUtil::toGrayImage(const cv::Mat& img, bool useMemPool, IMAGE_ROTATION rotation, IMAGE_MIRROR mirror) {
    int height = img.rows;
    int width = img.cols;
    if (height == 0 || width == 0) return nullptr;
    if ((width & 1) != 0 || (height & 1) != 0)
        P3D_THROW_RUNTIME("Invalid width=%d, height=%d", width, height);
    cv::Mat gray(height, width, CV_8UC1);
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    if (useMemPool) {
        int dataSize = calcGrayDataSize(width, height);
        auto block = std::make_shared<DataBlock>(dataSize);
        memcpy(block->rawData(), gray.data, dataSize);
        return std::make_shared<Image>(block->rawData(), width, height,
                                        IMAGE_FORMAT_GREY, rotation, mirror, block);
    }
    else {
        int dataSize = calcGrayDataSize(width, height);
        auto d = make_shared<DataHolder<ByteBuffer>>();
        d->d.resize(dataSize);
        memcpy(d->d.data(), gray.data, dataSize);
        return std::make_shared<Image>(d->d.data(), width, height,
                                        IMAGE_FORMAT_GREY, rotation, mirror, d);
    }
}

int	CvUtil::calcNv21DataSize(int width, int height) {
    if ((width & 1) != 0 || (height & 1) != 0)
        P3D_THROW_RUNTIME("Invalid width=%d, height=%d", width, height);
    int nPixels = width * height;
    return nPixels * 3 / 2;
}

int	CvUtil::calcGrayDataSize(int width, int height) {
    if ((width & 1) != 0 || (height & 1) != 0)
        P3D_THROW_RUNTIME("Invalid width=%d, height=%d", width, height);
    return width * height;
}

void CvUtil::cvtI420ToNv21(int width, int height, const char* data, char* dst) {
    if ((width & 1) != 0 || (height & 1) != 0)
        P3D_THROW_RUNTIME("Invalid width=%d, height=%d", width, height);
    const char* p0 = data;
    int nPixels = width * height;
    int hw = width / 2, hh = height / 2;
    char* p1 = dst;
    memcpy(p1, p0, nPixels);
    p0 += nPixels;
    p1 += nPixels;
    auto p1a = p1 + 1;
    for (int i = 0; i < hh; i++) for (int j = 0; j < hw; j++) {
            *p1a = *(p0++);
            p1a += 2;
        }
    p1a = p1;
    for (int i = 0; i < hh; i++) for (int j = 0; j < hw; j++) {
            *p1a = *(p0++);
            p1a += 2;
        }
}

ByteBuffer CvUtil::cvtI420ToNv21(int width, int height, const char* data) {
    ByteBuffer qa(calcNv21DataSize(width, height), 0);
    cvtI420ToNv21(width, height, data, qa.data());
    return qa;
}


#ifdef IS_DESKTOP
#ifndef CVUTILS_BUILD_PUBS_FOR_RELEASE
PNdArray CvUtil::readNdArrayImage(cstring fpath, IMAGE_FORMAT format, bool useCHWLayout) { //todo: really need it?
	cv::Mat m = cv::imread(fpath);
	PNdArray arr;
	int lineWidth = (int)m.step[0];
	uchar* sdata = m.data;
	int height = m.rows;
	int width = m.cols;
	if (m.channels() == 1) { //grey
		switch (format) {
		case IMAGE_FORMAT_GREY_NORM_FLOAT: {
			break;
		}
		default:
			throw runtime_error(Utils::makeStr("unsupported mmu image format %d", format));
		}
	}
	else if(m.channels() == 3) { //bgr
		switch (format) {
		case IMAGE_FORMAT_BGR_NORM_FLOAT: {
			auto arr1 = PNdArrayF({ height,width,3 });
			auto data1 = arr1.data();
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					for (int i = 0; i < 3; i++)
						*(data1++) = (sdata[3*x+i] - 127.0f) / 128.0f;
				}
				sdata += lineWidth;
			}
			arr = arr1;
			break;
		}
		case IMAGE_FORMAT_RGB_NORM_FLOAT: {
			if (useCHWLayout) {
				auto arr1 = PNdArrayF({ 3,height,width });
				auto data1 = arr1.data();
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						float bgr[3];
						for (int i = 0; i < 3; i++)
							bgr[i] = (sdata[3 * x + i] - 127.0f) / 128.0f;
						arr1[{0, y, x}] = bgr[2];
						arr1[{1, y, x}] = bgr[1];
						arr1[{2, y, x}] = bgr[0];
					}
					sdata += lineWidth;
				}
				arr = arr1;
			}
			break;
		}
		default:
			throw runtime_error(Utils::makeStr("unsupported mmu image format %d", format));
		}
	}
	else {
		throw runtime_error("Invalid image channels");
	}
	return arr;
}

void CvUtil::writeNdArrayImage(cstring fpath, const PNdArray& img, IMAGE_FORMAT format, bool useCHWLayout) {
	PNdArray img1 = img.shallowCopy();
	auto f2i = [](float f) {return (uint8_t)MathUtil::toRange(roundi(f*128.0f + 127.0f),0,255); };
	if (useCHWLayout) {
		if (img1.ndim() == 4) {
			if (img1.dimSize(0) != 1)
				P3D_THROW_RUNTIME("Invalid shape");
			img1.doSqueeze(0);
		}
		if (img1.ndim() != 3 || img1.dimSize(0) != 3)
			P3D_THROW_RUNTIME("Invalid shape");
		if (format == IMAGE_FORMAT_RGB_NORM_FLOAT) {
			if(img1.dtype() != DType::Float32)
				P3D_THROW_RUNTIME("Invalid format");
			int height = img1.dimSize(1), width = img1.dimSize(2);
			cv::Mat3b rlt = cv::Mat3b::zeros(height, width);
			PNdArrayF img2(img1);
			for (int y = 0; y < height; y++) for (int x = 0; x < width; x++) {
				rlt(y, x) = cv::Vec3b(f2i(img2[{2, y, x}]), f2i(img2[{1, y, x}]), f2i(img2[{0, y, x}]));
			}
			cv::imwrite(fpath, rlt);
		}
		else {
			P3D_THROW_RUNTIME("Unsupported format");
		}
	}
	else {
		P3D_THROW_RUNTIME("not implemented");
	}
}

pImage CvUtil::readNV21Image(cstring fname, bool useMemPool, IMAGE_ROTATION rotation, IMAGE_MIRROR mirror) {
	cv::Mat img = cv::imread(fname);
	return toNV21Image(img, useMemPool, rotation, mirror);
}

pImage CvUtil::readGrayImage(cstring fname, bool useMemPool, IMAGE_ROTATION rotation, IMAGE_MIRROR mirror) {
	cv::Mat img = cv::imread(fname);
	return toGrayImage(img, useMemPool, rotation, mirror);
}

void CvUtil::showWait(std::string name, const cv::Mat& img)
{
	cv::imshow(name, img);
	cv::waitKey();
}

cv::Mat CvUtil::toCvImage(const PNdArray& _arr) {
	PNdArray arr = _arr;
	if (arr.dtype() == DType::Float32) {
		arr = NdUtils::toUInt8Image(_arr.get());
	}
	if (arr.dtype() != DType::UInt8)
		P3D_THROW_RUNTIME("Invalid image dtype = %d", arr.dtype());
	return CvUtil::toCvMat(arr);
}

PNdArray CvUtil::toNdArray(const cv::Mat& arr) {
	auto cvtype = CV_MAT_DEPTH(arr.type());
	DType dtype = DType::NoType;
	switch (cvtype) {
	case CV_8U:
		dtype = DType::UInt8;
		break;
	case CV_32S:
		dtype = DType::Int32;
		break;
	case CV_32F:
		dtype = DType::Float32;
		break;
	case CV_64F:
		dtype = DType::Float64;
		break;
	default:
		P3D_THROW_RUNTIME("Unsupported cv type %d", cvtype);
	}
	auto eleSize = DTypeUtil::getSize(dtype);
	PNdArray newarr(dtype, { arr.rows, arr.cols, arr.channels() }, false);
	const uchar* psrc = arr.data;
	char* pdst = newarr.rawData();
	int srcStep = (int)arr.step[0];
	int dstStep = newarr.strides()[0];
	for (int i = 0; i < arr.rows; i++) {
		memcpy(pdst, psrc, dstStep);
		pdst += dstStep;
		psrc += srcStep;
	}
	return newarr;
}

void CvUtil::showLines(const std::string&winName, Size winSize, std::vector<std::string>& lines) {
	auto font = cv::FONT_HERSHEY_SIMPLEX;
	cv::Mat img = cv::Mat3b::zeros(winSize.height, winSize.width);
	int fontScale = 1;
	auto fontColor = cv::Scalar(255, 255, 255);
	int lineType = 2;
	int c = 0;
	for (const auto&line : lines) {
		c++;
		cv::putText(img, line,
			{ 0, c * 30 },
			font,
			fontScale,
			fontColor,
			lineType);
	}
	imshow(winName, img);
}

void CvUtil::imshow(const std::string& name, const PNdArray& img, float scale) {
	auto cvImg = CvUtil::toCvMat(img);
	imshow(name, cvImg, scale);
}

cv::Mat CvUtil::scaleImage(const cv::Mat&img, float scale) {
	if (scale == 1) return img;
	cv::Mat img1;
	int height = img.rows;
	int width = img.cols;
	int height1 = max(int(height*scale), 1);
	int width1 = max(int(width*scale), 1);
	cv::resize(img, img1, cv::Size(width1, height1));
	return img1;
}

void CvUtil::imshow(const std::string& name, const cv::Mat& img, float scale)
{
	auto cvImg = scaleImage(img, scale);
    int width = cvImg.size().width;
    int height = cvImg.size().height;
    
    int length = std::min(cvImg.size().width, cvImg.size().height);
    const double max_length_size = 1024.0;
    if (length > (int)max_length_size) {
        width = cvImg.size().width * max_length_size / length;
        height = cvImg.size().height * max_length_size / length;
        cv::Mat img1;
        cv::resize(cvImg, img1, cv::Size(width, height));
        cvImg = img1;
    }
    
    cv::namedWindow(name, cv::WINDOW_NORMAL);
	cv::resizeWindow(name, width, height);
	cv::imshow(name, cvImg);
}

void CvUtil::imwrite(const std::string& fname, const PNdArray& img) {
	auto cvImg = CvUtil::toCvMat(img);
	cv::imwrite(fname, cvImg);
}


CvGreyImage CvUtil::toGrey(const CvColorImage& img)
{
	CvGreyImage img1 = CvGreyImage::zeros(img.size());
	cv::cvtColor(img, img1, cv::COLOR_BGR2GRAY);
	return img1;
}

cv::Rect CvUtil::toCv(const Rect& rect) {
	return cv::Rect(rect.x, rect.y, rect.width, rect.height);
}

cv::Rect CvUtil::toCv(const RectF& rect) {
	return toCv(Rect(rect));
}

Rect CvUtil::toRect(const cv::Rect& rect) {
	return Rect(rect.x, rect.y, rect.width, rect.height);
}

void CvUtil::drawLandmarks(cv::Mat&img, const PNdArrayI& landmarks, int radius, cv::Scalar color, bool drawIndices) {
	auto& shape = landmarks.shape();
	if (shape.size() != 2 || shape[1] != 2) {
		P3D_THROW_ARGUMENT("Invalid landmark array size");
	}
	int nlandmarks = shape[0];
	float fontScale = 0.2f;
	cv::Scalar fontColor = cv::Scalar(0, 0, 255);
	int lineType = 1;
	for (int i = 0; i < nlandmarks; i++) {
		int x = landmarks[{i, 0}];
		int y = landmarks[{i, 1}];
		cv::circle(img, { x,y }, radius, color);
		if (drawIndices) {
			cv::putText(img, to_string(i),
				{ x, y },
				cv::FONT_HERSHEY_SIMPLEX,
				fontScale,
				fontColor,
				lineType
			);
		}
	}
}

void CvUtil::drawLandmarks(cv::Mat&img, const PNdArrayF& landmarks, int radius, cv::Scalar color, bool drawIndices) {
	auto& shape = landmarks.shape();
	if (shape.size() != 2 || shape[1] != 2) {
		P3D_THROW_ARGUMENT("Invalid landmark array size");
	}
	int nlandmarks = shape[0];
	for (int i = 0; i < nlandmarks; i++) {
		cv::circle(img, { roundi(landmarks[{i,0}]),roundi(landmarks[{i,1}]) }, radius, color, drawIndices);
	}
}

void CvUtil::drawRect(cv::Mat&img, const RMRect& rect, cv::Scalar color) {
	auto info = rect.calcInfo();
	vector<Vec2f> pts = { info.p0, info.p0 + info.lvx, info.p0 + info.lvx + info.lvy, info.p0 + info.lvy };
	for (int i = 0; i < 4; i++) {
		int j = (i + 1) % 4;
		cv::line(img, { roundi(pts[i].x), roundi(pts[i].y) }, { roundi(pts[j].x), roundi(pts[j].y) }, color);
	}
}

void CvUtil::drawRect(cv::Mat&img, const Rect& rect, cv::Scalar color) {
	cv::rectangle(img, toCv(rect), color);
}

void CvUtil::drawRect(cv::Mat&img, const RectF& rect, cv::Scalar color) {
	cv::rectangle(img, toCv(rect), color);
}

cv::Mat_<double> CvUtil::readJsonMatrix(cstring fname) {
	auto jdoc = P3djson::parse(Utils::readFileString(fname));
	if (jdoc.type() != P3djson::value_t::array)
		P3D_THROW_ARGUMENT("Invalid array");
	auto arr = jdoc;
	int m = arr.size();
	if (m == 0)
		return cv::Mat_<double>();
	int n = arr[0].size();
	cv::Mat_<double> r = cv::Mat_<double>::zeros(m, n);
	for (int i = 0; i < m; i++) {
		auto sa = arr[i];
		for (int j = 0; j < n; j++) {
			r(i, j) = double(sa[j]);
		}
	}
	return r;
}
/*
void CvUtil::drawCenterText(cv::Mat& img, const std::string& text, RectF rect, int fontScale, cv::Scalar fontColor, int lineType) {
	int x = rect.x + roundi(rect.width / 4);
	int y = rect.y + roundi(rect.height / 2);
	auto lines = StrUtil::splitLines(text);
	int c = 0;
	for (auto line : lines) {
		drawText(img, line, { x,y+(c++)*30*fontScale }, fontScale, fontColor, lineType);
	}
}
*/
//mask image helper
struct RMRectRasHelper {
	typedef array<int, 2> Seg;
	typedef array<int, 3> Line;
	typedef array<Vec2i, 4> Quad;
	typedef array<Line, 4> QuadLine;

	static const int mini = numeric_limits<int>::min();
	static const int maxi = numeric_limits<int>::max();

	static Seg interSeg(const Seg& s1, const Seg& s2) {
		int a=max(s1[0],s2[0]);
		int b=min(s1[1],s2[1]);
		if(b<=a) return {0,0};
		else return {a,b};
	}

	//Ax+By+C>=0
	static Seg solve(int A, int B, int C, int y) {
		//Ax>=p=-By-C
		int p = -B*y-C;
		if(A==0)
			return p<=0?Seg({mini,maxi}):Seg({0,0});
		else if(A>0) {
			//x>=p/A
			return {MathUtil::divCeil(p,A), maxi};
		}else{ //A<0
			//x<=p/A
			return {mini, MathUtil::divUp(p,A)};
		}
	}

	static Seg solve(const vector<array<int, 4>>& constraints) {
		Seg r = {mini, maxi};
		for(auto& c:constraints) {
			Seg s = solve(c[0],c[1],c[2],c[3]);
			r = interSeg(r, s);
		}
		return r;
	}

	// (x0,y0) (x1,y1) -> upway x,y->-y,x
	// (y0-y1)*x+(x1-x0)*y+(x0y1-y0x1) >= 0
	static Line buildLine(const Vec2i& p0, const Vec2i& p1) {
		if (p0.x == p1.x && p0.y == p1.y) return {0, 0, 0};
		int x0 = p0.x, y0 = p0.y;
		int x1 = p1.x, y1 = p1.y;
		int A = y0 - y1, B = x1 - x0, C = x0 * y1 - y0 * x1;
		return {A, B, C};
	}

	static QuadLine buildQuadLine(const Quad& quad) {
		return {
				buildLine(quad[0],quad[1]),
				buildLine(quad[1],quad[2]),
				buildLine(quad[2],quad[3]),
				buildLine(quad[3],quad[0])
		};
	}

	static Seg solve(const QuadLine& quadLine, int y) {
		Seg r = {mini, maxi};
		for(int i=0;i<4;i++) {
			auto& line = quadLine[i];
			Seg s = solve(line[0],line[1],line[2],y);
			r = interSeg(r, s);
		}
		return r;
	}
};

//lx,ly: 0-1
struct GetValue {
	const float* data;
	float defVal = 0;
	int width = 0, height = 0;
	explicit GetValue(const PNdArrayF& arr) {
		if(!(arr.ndim() == 2 || (arr.ndim() == 3 && arr.shape()[2] == 1))) {
			P3D_THROW_ARGUMENT("Invalid shape for mask image");
		}
		this->data = arr.data();
		this->height = arr.shape()[0];
		this->width = arr.shape()[1];
	}

	float pick(int x, int y) {
		if(x<0||x>=width||y<0||y>=height)
			return defVal;
		return data[y*width+x];
	}

	float get(float lx, float ly) {
		float _ix = floor(lx);
		int ix = (int)_ix;
		float _iy = floor(ly);
		int iy = (int)_iy;
		float c00 = pick(ix, iy);
		float c10 = pick(ix+1, iy);
		float c01 = pick(ix, iy+1);
		float c11 = pick(ix+1, iy+1);
		float a = lx - _ix;
		float b = ly - _iy;
		float a1 = 1 - a;
		float b1 = 1 - b;
		float val = c00*a1*b1 + c10*a*b1 + c01*a1*b + c11*a*b;
		return val;
	}
};

void CvUtil::drawMaskImage(cv::Mat3b&img, RMRect rect, const PNdArrayF& mask, cv::Scalar _color) {
	const int scale = 4;
	cv::Vec3f color = {(float)_color.val[0],(float)_color.val[1],(float)_color.val[2]};
	array<Vec2i, 4> quadvs;
	{
		array<Vec2f, 4> rfpoints;
		RMRect rect1 = rect;
		rect1.mirror = IMAGE_MIRROR_NO;
		auto rectInfo = rect1.calcInfo();
		rfpoints[0] = rectInfo.p0;
		rfpoints[1] = rectInfo.p0 + rectInfo.lvx;
		rfpoints[2] = rectInfo.p1;
		rfpoints[3] = rectInfo.p0 + rectInfo.lvy;
		for(int i=0;i<4;i++) {
			rfpoints[i]*=(float)scale;
			quadvs[i] = Vec2i(roundi(rfpoints[i].x), roundi(rfpoints[i].y));
		}
	}
	auto quadLine = RMRectRasHelper::buildQuadLine(quadvs);
	int miny = std::min({quadvs[0].y,quadvs[1].y,quadvs[2].y,quadvs[3].y})/scale;
	int maxy = (std::max({quadvs[0].y,quadvs[1].y,quadvs[2].y,quadvs[3].y})+3)/scale;
	int imgWidth = img.cols;
	int imgHeight = img.rows;
	miny = MathUtil::toRange(miny, 0, imgHeight-1);
	maxy = MathUtil::toRange(maxy, 0, imgHeight-1);
	auto rectInfo = rect.calcInfo();
	float lenX2 = rectInfo.lvx.sqrLenght();
	float lenY2 = rectInfo.lvy.sqrLenght();
	GetValue getValue(mask);
	for(int y=miny;y<=maxy;y++) {
		int xbegin, xend; 
		{
			auto seg = RMRectRasHelper::solve(quadLine, y*scale);
			xbegin = seg[0];
			xend = seg[1];
		}
		int c=0;
		Vec2f first, last, delta;
		int _xbegin = MathUtil::toRange(MathUtil::divCeil(xbegin, 4),0,imgWidth-1); //x>=xbegin/4
		int _xend = MathUtil::toRange(MathUtil::divUp(xend, 4),0,imgWidth); //x<xend/4
		for(int x=_xbegin; x<_xend; x++) {
			c++;
			if(c == 1 || c == 2) {
				Vec2f v(x,y);
				auto lv = v-rectInfo.p0;
				float lx = rectInfo.lvx.dot(lv)/lenX2;
				float ly = rectInfo.lvy.dot(lv)/lenY2;
				last = Vec2f(lx, ly);
				if(c==1)
					first = last;
				else //c==2
					delta = last-first;
			} else
				last += delta;
			float alpha = getValue.get(last.x, last.y);
			if (alpha > 0) {
				cv::Vec3b& dstV = img(y, x);
				cv::Vec3f dstV1(dstV[0], dstV[1], dstV[2]);
				dstV1 = (1 - alpha)*dstV1 + alpha * color;
				dstV[0] = (uint8_t)dstV1[0];
				dstV[1] = (uint8_t)dstV1[1];
				dstV[2] = (uint8_t)dstV1[2];
			}
		}
	}
}
#endif
#endif


bool CvPnpSolver::solve(const PNdArrayF& objectPoints, const PNdArrayF& imagePoints,
	const PNdArrayD& cameraMatrix, const PNdArrayD& distCoeffs,
	/*out*/PNdArrayF& rvec, /*out*/PNdArrayF& tvec, bool useExtrinsicGuess) {
	cv::Mat _objectPoints = CvUtil::toCvMat(objectPoints);
	cv::Mat _imagePoints = CvUtil::toCvMat(imagePoints);
	cv::Mat _cameraMatrix = CvUtil::toCvMat(cameraMatrix);
	cv::Mat _distCoeffs = CvUtil::toCvMat(distCoeffs);
	cv::Mat _rvec, _tvec;

	if (useExtrinsicGuess)
	{
		_rvec = CvUtil::toCvMat(rvec);
		_tvec = CvUtil::toCvMat(tvec);
	}

	if (!cv::solvePnP(_objectPoints, _imagePoints, _cameraMatrix, _distCoeffs, _rvec, _tvec, useExtrinsicGuess))
		return false;

	cv::Mat Rvec;
	cv::Mat_<float> Tvec;
	_rvec.convertTo(Rvec, CV_32F);   //旋转向量
	_tvec.convertTo(Tvec, CV_32F);   //平移向量

	rvec.data()[0] = Rvec.at<float>(0, 0);
	rvec.data()[1] = Rvec.at<float>(1, 0);
	rvec.data()[2] = Rvec.at<float>(2, 0);

	tvec.data()[0] = Tvec.at<float>(0, 0);
	tvec.data()[1] = Tvec.at<float>(1, 0);
	tvec.data()[2] = Tvec.at<float>(2, 0);

	return true;
};

END_P3D_NS