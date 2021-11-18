#pragma once
#include "P3D_Utils.h"
#include "mathVector.hpp"
#include "NdArray.h"
#include "OneEuro.h"
#include "json_inc.h"

BEGIN_P3D_NS

enum IMAGE_FORMAT {
	IMAGE_FORMAT_NONE = -1,
	IMAGE_FORMAT_RGBA = 0,
	IMAGE_FORMAT_NV21 = 1,
	IMAGE_FORMAT_NV12 = 2,
	IMAGE_FORMAT_RGB = 3,
	IMAGE_FORMAT_BGRA = 4,
	IMAGE_FORMAT_BGR = 5,
	IMAGE_FORMAT_GREY = 6,
	IMAGE_FORMAT_GREY_NORM_FLOAT = 7,
	IMAGE_FORMAT_RGB_NORM_FLOAT = 8,
	IMAGE_FORMAT_BGR_NORM_FLOAT = 9,
	IMAGE_FORMAT_YUV0 = 10,
};


enum IMAGE_INTERPOLATION {
	IMAGE_INTERP_BILINEAR,
	IMAGE_INTERP_NN,
};

typedef Vec2f Point2F;

template<typename T>
struct SizeT {
	T width;
	T height;
	SizeT() : width(0), height(0)
	{}
	SizeT(T width, T height) : width(width), height(height)
	{}
};
struct SizeF;
struct Size : SizeT<int> {
	Size() {}
	Size(int width, int height) : SizeT(width, height)
	{}
	Size t() const {
		return Size(height, width);
	}
	Size t(IMAGE_ROTATION rot) const {
		if (rot == IMAGE_ROT90_CCW || rot == IMAGE_ROT90_CW)
			return Size(height, width);
		else
			return Size(width, height);
	}
	bool operator != (const Size& r) {
		return width != r.width || height != r.height;
	}
	SizeF toF1() const;
};

struct SizeF : SizeT<float> {
	SizeF() {}
	SizeF(float width, float height) : SizeT(width, height)
	{}
	SizeF(int width, int height) : SizeT(float(width), float(height))
	{}
	SizeF t() const {
		return { height, width };
	}
	SizeF t(IMAGE_ROTATION rot) const {
		if (rot == IMAGE_ROT90_CCW || rot == IMAGE_ROT90_CW)
			return { height, width };
		else
			return { width, height };
	}
};

template<typename T>
struct RectT {
	T x; // left: x = 0, right: x > 0
	T y; // top: y = 0, bottom: y > 0
	T width; // right - left
	T height; // bottom - top
	RectT() :x(0), y(0), width(0), height(0)
	{}
	RectT(T x, T y, T width, T height) : x(x), y(y), width(width), height(height)
	{}
	RectT(const Size& size) : x(0), y(0), width((T)size.width), height((T)size.height)
	{}
	T area() const {
		return width * height;
	}
	bool isEmpty() const {
		return width <= 0 && height <= 0;
	}
};

inline SizeF Size::toF1() const {
	return SizeF(width - 1, height - 1);
}

struct RectF;
struct Rect;
struct RMRect;

struct Rect : RectT<int> {
	Rect() {}
	Rect(int x, int y, int width, int height) : RectT<int>(x, y, width, height)
	{}
	explicit Rect(const Size& size) : RectT<int>(size)
	{}
	explicit Rect(const RectF& rect);
};

struct RectF : RectT<float> {
	RectF() {}
	RectF(float x, float y, float width, float height) : RectT<float>(x, y, width, height)
	{}
	explicit RectF(const Size& size) : RectT<float>(size)
	{}
	explicit RectF(const Rect& rect);
};

//floori(x)+1=roundi(x+0.5)
inline Rect::Rect(const RectF& rect) : RectT<int>(floori(rect.x) + 1, floori(rect.y) + 1, roundi(rect.width), roundi(rect.height))
{}

inline RectF::RectF(const Rect& rect) : RectT<float>(float(rect.x) - 0.5f, float(rect.y) - 0.5f, (float)rect.width, (float)rect.height)
{}

struct Parallelogram
{
	Vec2f startPos;
	Vec2f lvx, lvy;

	Parallelogram() {}
	Parallelogram(Vec2f startPos, Vec2f lvx, Vec2f lvy)
		: startPos(startPos), lvx(lvx), lvy(lvy)
	{}
	Vec2f localToGlobal(float x, float y) const {
		return startPos + lvx * x + lvy * y;
	}
	Vec2f localToGlobal(Vec2f p) const {
		return startPos + lvx * p.x + lvy * p.y;
	}
	Parallelogram localToGlobal(RectF local) const {
		Vec2f sp1 = localToGlobal(Vec2f(local.x, local.y));
		return Parallelogram(sp1, lvx*local.width, lvy*local.height);
	}
};

struct RMRect {
	Point2F center;
	SizeF 	size;
	float angle = 0;
	IMAGE_MIRROR mirror = IMAGE_MIRROR_NO; //1.mirror 2.rotate
	struct Info {
		Vec2f lvx, lvy; //length vector x, y
		Vec2f p0, p1; //upper left point, lower right point
	};

	explicit RMRect() {
	}

	RMRect(Point2F center, SizeF size, float angle = 0, IMAGE_MIRROR mirror = IMAGE_MIRROR_NO)
		: center(center), size(size), angle(angle), mirror(mirror)
	{}

	explicit RMRect(const Rect& rect) {
		center = { float(rect.x) - 0.5f + float(rect.width) / 2.0f,
			float(rect.y) - 0.5f + float(rect.height) / 2.0f };
		size = { (float)rect.width, (float)rect.height };
	}

	explicit RMRect(const RectF& rect) {
		center = { rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f };
		size = { rect.width, rect.height };
	}

	bool isEmpty() const {
		return size.height <= 0 || size.width <= 0;
	}

	float area() const {
		return size.width*size.height;
	}

	Rect calcAppRect() const {
		float m = std::max(size.width, size.height);
		float x0 = center.x - m / 2;
		float y0 = center.y - m / 2;
		return Rect(RectF(x0, y0, m, m));
	}

	RectF calcAppRectF() const {
		float m = std::max(size.width, size.height);
		float x0 = center.x - m / 2;
		float y0 = center.y - m / 2;
		return { x0,y0,m,m };
	}

	Info calcInfo() const {
		auto lvx = Vec2f(mirror == IMAGE_MIRROR_NO ? size.width : -size.width, 0).rotate(angle);
		auto lvy = Vec2f(0, size.height).rotate(angle);
		auto p0 = Vec2f(center) - lvx * 0.5f - lvy * 0.5f;
		auto p1 = Vec2f(center) + lvx * 0.5f + lvy * 0.5f;
		return { lvx, lvy, p0, p1 };
	}

	Parallelogram toParallelogram() const {
		auto info = calcInfo();
		return { info.p0,info.lvx,info.lvy };
	}

	RMRect calcParaRect() const {
		auto A = float(M_PI / 2);
		float newAngle = roundi(angle / A)*A;
		return { center, size, newAngle, mirror };
	}

	Vec2f calcAbsPoint(Vec2f p) const {
		auto info = calcInfo();
		return info.p0 + info.lvx*p.x + info.lvy*p.y;
	}

	std::vector<Vec2f> calcAbsPoints(const std::vector<Vec2f>& points) const {
		auto info = calcInfo();
		std::vector<Vec2f> apoints(points.size());
		for (size_t i = 0; i < points.size(); i++) {
			auto p = points[i];
			apoints[i] = info.p0 + info.lvx*p.x + info.lvy*p.y;
		}
		return apoints;
	}

	template<size_t size>
	std::array<Vec2f, size> calcAbsPoints(const std::array<Vec2f, size>& points) const {
		auto info = calcInfo();
		std::array<Vec2f, size> apoints;
		for (size_t i = 0; i < points.size(); i++) {
			auto p = points[i];
			apoints[i] = info.p0 + info.lvx*p.x + info.lvy*p.y;
		}
		return apoints;
	}

	RMRect localToGlobal(RectF local) const {
		Vec2f center1 = calcAbsPoint({ local.x + local.width / 2,local.y + local.height / 2 });
		float width1 = local.width*size.width;
		float height1 = local.height*size.height;
		return RMRect(center1, SizeF(width1, height1), angle, mirror);
	}
};

struct FaceInfo {
	int trackId = -1;
	RectF rect;
	RMRect rmRect;
	float prob = 0;
	PNdArrayF landmarks;
	std::array<float, 3> pitch_yaw_roll = { 0 };
	PNdArrayF tongueLandmarks;
	float tongueConfidence = -1;
	PNdArrayF ycnn_extra_landmarks; // x, y
	PNdArrayF ycnn_extra_landmarks_info; // z, valid, visible
};


struct ImageRM {
	IMAGE_ROTATION rot;
	IMAGE_MIRROR mirror;
	static inline IMAGE_MIRROR time(IMAGE_MIRROR m1, IMAGE_MIRROR m2) {
		return IMAGE_MIRROR(int(m1) ^ int(m2));
	}
	static inline IMAGE_ROTATION time(IMAGE_ROTATION r1, IMAGE_ROTATION r2) {
		return IMAGE_ROTATION((r1 + r2) & 3);
	}
	static inline IMAGE_ROTATION inv(IMAGE_ROTATION r) {
		return IMAGE_ROTATION((4 - r) & 3);
	}
	static inline ImageRM time(ImageRM rm1, ImageRM rm2) {
		IMAGE_ROTATION r1 = rm2.mirror == IMAGE_MIRROR_YES ? inv(rm1.rot) : rm1.rot;
		return { time(r1, rm2.rot), time(rm1.mirror, rm2.mirror) };
	}
	static inline IMAGE_ROTATION fromToRot(IMAGE_ROTATION from, IMAGE_ROTATION to) {
		return IMAGE_ROTATION(int(to) - int(from) + 4);
	}
	friend inline ImageRM operator * (ImageRM r1, ImageRM r2) {
		return time(r1, r2);
	}
	ImageRM inv() const {
		return { mirror == IMAGE_MIRROR_YES ? rot : inv(rot), mirror };
	}
	bool isRot90() const {
		return rot == IMAGE_ROT90_CCW || rot == IMAGE_ROT90_CW;
	}
	ImageRM() : rot(IMAGE_ROT0), mirror(IMAGE_MIRROR_NO)
	{}
	ImageRM(IMAGE_ROTATION rot, IMAGE_MIRROR mirror = IMAGE_MIRROR_NO) : rot(rot), mirror(mirror)
	{}
	Size apply(Size size) const {
		return size.t(rot);
	}
	Rect apply(Size size, Rect rect) const { //rect in size
		Rect r;
		switch (rot) {
		case IMAGE_ROT0:
			r = { rect.x, rect.y, rect.width, rect.height };
			break;
		case IMAGE_ROT90_CCW:
			r = { rect.y, size.width - rect.width - rect.x, rect.height, rect.width };
			break;
		case IMAGE_ROT180:
			r = { size.width - rect.width - rect.x, size.height - rect.height - rect.y, rect.width, rect.height };
			break;
		case IMAGE_ROT90_CW:
			r = { size.height - rect.height - rect.y, rect.x, rect.height, rect.width };
			break;
		}
		if (mirror == IMAGE_MIRROR_YES) {
			r.x = size.t(rot).width - (r.x + r.width);
		}
		return r;
	}
	RectF apply(Size size, RectF rect) const { //rect in size
		RectF r;
		SizeF sizef = size.toF1();
		switch (rot) {
		case IMAGE_ROT0:
			r = { rect.x, rect.y, rect.width, rect.height };
			break;
		case IMAGE_ROT90_CCW:
			r = { rect.y, sizef.width - rect.width - rect.x, rect.height, rect.width };
			break;
		case IMAGE_ROT180:
			r = { sizef.width - rect.width - rect.x, sizef.height - rect.height - rect.y, rect.width, rect.height };
			break;
		case IMAGE_ROT90_CW:
			r = { sizef.height - rect.height - rect.y, rect.x, rect.height, rect.width };
			break;
		}
		if (mirror == IMAGE_MIRROR_YES) {
			r.x = sizef.t(rot).width - (r.x + r.width);
		}
		return r;
	}

	Parallelogram apply(Size size, Parallelogram rect) const {
		return apply(size.toF1(), rect);
	}

	//what is the rect like in transformed image, keeping crop not changed
	Parallelogram apply(SizeF size, Parallelogram rect) const {
		auto startPos1 = apply(size, rect.startPos);
		auto x1 = apply(size, rect.startPos + rect.lvx);
		auto y1 = apply(size, rect.startPos + rect.lvy);
		return Parallelogram(startPos1, x1 - startPos1, y1 - startPos1);
	}

	//what is the rect like in transformed image, keeping crop not changed
	RMRect apply(Size size, RMRect rect) const { //rect in size
		SizeF sizef = size.toF1();
		RMRect nr;
		nr.angle = rect.angle;
		nr.mirror = rect.mirror;
		switch (rot) {
		case IMAGE_ROT0:
			nr.center = rect.center;
			nr.size = rect.size;
			break;
		case IMAGE_ROT90_CCW:
			nr.center = { rect.center.y, sizef.width - rect.center.x };
			nr.size = rect.size; //{ rect.size.height, rect.size.width };
			nr.angle -= C_PI / 2.0f;
			break;
		case IMAGE_ROT180:
			nr.center = { sizef.width - rect.center.x, sizef.height - rect.center.y };
			nr.size = rect.size;
			nr.angle -= C_PI / 2.0f * 2;
			break;
		case IMAGE_ROT90_CW:
			nr.center = { sizef.height - rect.center.y, rect.center.x };
			nr.size = rect.size; //{ rect.size.height, rect.size.width };
			nr.angle -= C_PI / 2.0f * 3;
			break;
		}
		if (mirror == IMAGE_MIRROR_YES) {
			nr.center.x = (float)sizef.t(rot).width - nr.center.x;
			nr.angle *= -1;
			nr.mirror = (IMAGE_MIRROR)(1 - nr.mirror);
		}
		return nr;
	}
	Vec2<int> apply(Size size, const Vec2<int>& p) const { //rect in size
		Vec2<int> np;
		switch (rot) {
		case IMAGE_ROT0:
			np = p;
			break;
		case IMAGE_ROT90_CCW:
			np = { p.y, size.width - 1 - p.x };
			break;
		case IMAGE_ROT180:
			np = { size.width - 1 - p.x, size.height - 1 - p.y };
			break;
		case IMAGE_ROT90_CW:
			np = { size.height - 1 - p.y, p.x };
			break;
		}
		if (mirror == IMAGE_MIRROR_YES) {
			np.x = size.t(rot).width - 1 - p.x;
		}
		return np;
	}
	Vec2f apply(Size size, const Vec2f& p) const { //rect in size
		return apply(size.toF1(), p);
	}
	//Vec2f apply(Size size, const Vec2f& p) const { //rect in size
	//	return apply(size.toF1(), (const Vec2<float>&)p);
	//}
//	Vec2f apply(SizeF size, const Vec2f& p) const { //rect in size
//		return apply(size, (const Vec2<float>&)p);
//	}
	Vec2<float> apply(SizeF size, const Vec2<float>& p) const { //rect in size
		Vec2<float> np;
		switch (rot) {
		case IMAGE_ROT0:
			np = p;
			break;
		case IMAGE_ROT90_CCW:
			np = { p.y, size.width - p.x };
			break;
		case IMAGE_ROT180:
			np = { size.width - p.x, size.height - p.y };
			break;
		case IMAGE_ROT90_CW:
			np = { size.height - p.y, p.x };
			break;
		}
		if (mirror == IMAGE_MIRROR_YES) {
			np.x = size.t(rot).width - p.x;
		}
		return np;
	}
};
class FaceUtils {
public:
	static std::vector<int> getEyeIndices(int leftOrRight) { //0:left, 1:right
		if (leftOrRight == 0) { //0:left
			std::vector<int> left_indices;
			left_indices.push_back(95);
			for (int i = 39; i < 51; i++) left_indices.push_back(i);
			return left_indices;
		}
		else { //1:right
			std::vector<int> right_indices;
			right_indices.push_back(96);
			for (int i = 51; i < 63; i++) right_indices.push_back(i);
			return right_indices;
		}
	}

	static Vec2f calcAvePoint(const std::vector<int>& indices, const PNdArrayF& arr) {
		Vec2f v;
		for (int i : indices) {
			v.x += arr[{i, 0}];
			v.y += arr[{i, 1}];
		}
		v = v /(float)indices.size();
		return v;
	}

	static RMRect calcLandmarkRMRect_failsafe(const PNdArrayF& landmarks) {
		int n = landmarks.shape()[0];
		float minx = landmarks[{0, 0}], maxx = minx;
		float miny = landmarks[{0, 1}], maxy = miny;
		float sx = minx, sy = miny;
		for (int i = 1; i < n; i++) {
			float x = landmarks[{i, 0}], y = landmarks[{i, 1}];
			sx += x;
			sy += y;
			minx = std::min(minx, x);
			maxx = std::max(maxx, x);
			miny = std::min(miny, y);
			maxy = std::max(maxy, y);
		}
		sx /= n;
		sy /= n;
		return RMRect({ sx,sy }, SizeF(maxx - minx, maxy - miny));
	}
	static	RMRect calcLandmarkRMRect(const PNdArrayF& landmarks) { //based on eye point
		int n = landmarks.shape()[0];
		std::vector<int> left_indices = getEyeIndices(0);//{63};
		std::vector<int> right_indices = getEyeIndices(1);//{74};
		//L:95,R:96
		Vec2f pL = calcAvePoint(left_indices, landmarks);
		Vec2f pR = calcAvePoint(right_indices, landmarks);
		//x vec
		Vec2f vx = pR - pL;
		float d = (pR - pL).length();
		if (d < 1e-4f) return calcLandmarkRMRect_failsafe(landmarks); //do not fix
		vx = vx/ d;
		float angle = acos(vx.x);
		if (vx.y < 0)
			angle *= -1;
		if (!(angle > -10 && angle < 10))
			return calcLandmarkRMRect_failsafe(landmarks);
		//y vec
		Vec2f vy = vx.rotate(C_PI / 2);
		float left = 1e5f, right = -1e5f, top = 1e5f, bottom = -1e5f;
		for (int i = 0; i < n; i++) {
			Vec2f v(landmarks[{i, 0}], landmarks[{i, 1}]);
			Vec2f v1 = v - pL;
			float lx = v1.dot(vx);
			float ly = v1.dot(vy);
			left = std::min(left, lx);
			right = std::max(right, lx);
			top = std::min(top, ly);
			bottom = std::max(bottom, ly);
		}
		Vec2f lcenter((left + right) / 2, (top + bottom) / 2);
		float length = std::max(right - left, bottom - top);
		Vec2f center = pL + vx * lcenter.x + vy * lcenter.y;
		return RMRect(center, { length,length }, angle);
	}
};
class AnimojiFilter1 {
public:
	void addPoint(int index, float x0, float alpha0, float alpha1, float cb, float c0, float c1, float cut0 = 0.999f);
	void doFilter(PNdArrayF& arr);
	float doFilter(int index, float val);
	float doFilter(int index, float val, float v);
	float doFilter2(int index, float val, float stdDiff);
	float getLast(int index);
	bool hasLast(int index);
	void loadFromJson(cstring scfg);
	void loadFromJson(P3djson& cfg);
	bool hasIndex(int index) const { return items.count(index) > 0; }
private:
	struct Item {
		int index;
		float x0, alpha0, alpha1;
		float cb, c0, c1; //for curve fitting
		float cut0;
		float v0 = 0;
		float last = 0;
		bool hasLast = false;
		void init() {
			v0 = exp(-x0 / alpha0);
		}

		Item()
		{
		}
		Item(int index_, float x0_, float alpha0_, float alpha1_, float cb_, float c0_, float c1_, float cut0_)
		{
			index = index_;
			x0 = x0_;
			alpha0 = alpha0_;
			alpha1 = alpha1_;
			cb = cb_;
			c0 = c0_;
			c1 = c1_;
			cut0 = cut0_;

			init();
		}
		float calcCoff(float p, float diff) const {
			float stdDiff = p < cb ? c0 : c0 + (p - cb) * c1;
			float x = diff / stdDiff;
			if (x < x0) {
				float val = exp(-x / alpha0);
				if (val > cut0) val = 1;
				return val;
			}
			else
				return v0 * exp(-(x - x0) / alpha1);
		}
		float calcCoff2(float stdDiff, float diff) const {
			float x = diff / stdDiff;
			if (x < x0) {
				float val = exp(-x / alpha0);
				if (val > cut0) val = 1;
				return val;
			}
			else
				return v0 * exp(-(x - x0) / alpha1);
		}
		float filter(float p) {
			if (!hasLast) {
				last = p;
				hasLast = true;
				return p;
			}
			float baseP = last; //expected value
			float diffP = abs(p - baseP);
			float coff = calcCoff(p, diffP);
			float outP = baseP * coff + p * (1 - coff);
			last = outP;
			return outP;
		}

		float filterWithValue(float p, float stdDiff) {
			if (!hasLast) {
				last = p;
				hasLast = true;
				return p;
			}
			float baseP = last; //expected value
			float diffP = abs(p - baseP);
			float coff = calcCoff2(stdDiff, diffP);
			float outP = baseP * coff + p * (1 - coff);
			last = outP;
			return outP;
		}
	};
	std::unordered_map<int, Item> items;
	int maxIndex = 0;
};

class AnimojiFilter2 {
public:
	void init();
	void addPoint(int index, float frequency, float mincutoff, float beta, float dcutoff);
	float doFilter(int index, float val);
	bool hasIndex(int index) const { return items.count(index) > 0; }
	void setFreq(int index, float freq);
	float getLast(int index);
private:
	std::unordered_map<int, std::shared_ptr<OneEuro::OneEuroFilter> > items;
};

class AnimojiFilter3 {
public:
	void init();
	void addPoint(int index, float accMax, float clampmin = 0, float clampmax = 1);
	float doFilter(int index, float val);
	bool hasIndex(int index) const { return items.count(index) > 0; }
private:

	struct MomentumItem {
		int index;
		float maxAcc;
		float oldVal;
		float oldSpeed;

		float clampMin;
		float clampMax;

		bool hasOld = false;

		MomentumItem()
		{
		}
		MomentumItem(int index_, float accMax_, float clampMin_ = 0.0, float clampMax_ = 1.0)
		{
			index = index_;
			maxAcc = accMax_;

			clampMin = clampMin_;
			clampMax = clampMax_;
		}

		float filter(float curVal) {
			if (!hasOld) {
				oldVal = curVal;
				oldSpeed = 0;
				hasOld = true;
				return curVal;
			}
			float delta = curVal - oldVal;

			// 计算当前的速度
			float curSpeed = delta;
			if (curSpeed > oldSpeed && curSpeed > oldSpeed + maxAcc) // 防止速度增加过快
				curSpeed = oldSpeed + maxAcc;
			if (curSpeed < oldSpeed && curSpeed < oldSpeed - maxAcc) // 防止速度降低过快
				curSpeed = oldSpeed - maxAcc;
			if ((curSpeed + oldVal - curVal)*(curVal - oldVal) > 0) // 防止运动超过真实运动（弹簧效应）
				curSpeed = curVal - oldVal;
			// 设置回去
			curVal = oldVal + curSpeed;
			if (curVal > clampMax) curVal = clampMax;
			if (curVal < clampMin) curVal = clampMin;
			oldSpeed = curVal - oldVal;
			oldVal = curVal;
			return curVal;
		}
	};

	std::unordered_map<int, MomentumItem > items;
};

class AnimojiFilter4 {
public:
	void init();
	float doFilter(float val);
private:

	struct ConfidenceItem {
		int frameNum;
		std::vector<bool> values;
		bool status;

		ConfidenceItem()
		{
		}
		ConfidenceItem(int frameNum_)
		{
			frameNum = frameNum_;
			status = false;
		}

		float filter(bool curVal) {
			if (values.size() >= frameNum) {
				values.erase(values.begin());
			}
			values.push_back(curVal);

			if (values.size() == frameNum && status != curVal)
			{
				bool equal = true;
				for (int i = 1; i < values.size(); i++)
				{
					if (values[i - 1] != values[i])
					{
						equal = false;
						break;
					}
				}
				if (equal)
					status = curVal;
			}

			return status;
		}
	};

	ConfidenceItem item;
};

class LandmarkFilter {
public:
	struct FaceData {
		AnimojiFilter1 filter1;
	};
private:
	std::map<int, FaceData> faces;
public:
	void clear() { faces.clear(); }
	void filterOne(int faceid, float* in_out_landmark, int landmark_num, float width, float height, float yaw, float pitch, float filterRatio = 1.0, float filterRatioBigAngle = 1.0);
};

class PoseFilter {
public:
	struct FaceData {
		AnimojiFilter1 filter1;
		AnimojiFilter2 filter2;
	};
private:
	std::map<int, FaceData> faces;
public:
	void clear() { faces.clear(); }
	void filterOne(int faceid, std::vector<float> & in_out_pose, float width, float height);
	void filterOneForPnp(int faceid, std::vector<float> & in_out_pose, float width, float height, bool isR, bool isStable, float filterRatioNorm = 1.0, float filterRatioBigAngle = 1.0);
};

class EarFilter {
public:
	struct FaceData {
		AnimojiFilter1 filter1;
	};
private:
	std::map<int, FaceData> faces;
public:
	void clear() { faces.clear(); }
	void filterOne(int faceid, std::vector<float> & in_out_pose);
};

class StableDeterminationForPnp {
public:
	struct FaceData {
		std::vector<std::vector<float> > list;
		std::vector<float> min;
		std::vector<float> max;
	};
private:
	std::map<int, FaceData> faces;
public:
	void clear() { faces.clear(); }
	bool add(int faceId, float w, float h, const PNdArrayF& landmarks);
};

END_P3D_NS
