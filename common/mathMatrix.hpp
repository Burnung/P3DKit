#pragma once
#include"P3D_Utils.h"
#include"mathVector.hpp"
#include<array>
#include <type_traits>
#include<limits>
BEGIN_P3D_NS
template<typename T>
class Mat3 :public P3DZeroObject {
public:
	Mat3<T>() {};
	explicit Mat3<T>(T*t) {
		doFor(i, 9)
			data[i] = t[i];
	}
	Mat3<T>(const Mat3<T>& m) {
		doFor(i, 3)
			row[i] = m[i];
	}
	Mat3<T>& operator =(const Mat3<T>&m) {
		doFor(i, 3)
			row[i] = m[i];
		return *this;
	}
public:
	Vec3<T> getCol(uint32 index) {
		return { row[0][index],row[1][index],row[2][index] };
	}
public:
	static Mat3<T> getIdentity() {
		auto ret = Mat3<T>();
		doFor(r, 3) {
			doFor(c, 3)
				ret[{r, c}] = 0.0;
			ret[{r, r}] = 1.0;
		}
		return ret;
	}
public:
	Vec3<T>& operator [](uint32 index){
        P3D_ENSURE(index < 3,"Mat3::[] INValide index");
        return row[index];
    }
    const Vec3<T>& operator [](uint32 index) const{
        P3D_ENSURE(index < 3,"Mat3::[] INValide index");
        return row[index];
    }
    T& operator [](const std::array<uint32,2>& index){
        P3D_ENSURE((index[0] < 3 && index[1] < 3),"Mat4::[{}] INValide index");
        return data[index[0] * 3 + index[1]];
    }
    const T& operator [](const std::array<uint32,2>& index) const{
        P3D_ENSURE((index[0] < 3 && index[1] < 3),"Mat4::[{}] INValide index");
        return data[index[0] * 3 + index[1]];
    }
    Mat3<T> operator*(const Mat3<T>& m) const{
        Mat3<T> ret;
        doFor(r,3){
            doFor(c,3){
                ret[r][c] = this->row[r].dot(m.getCol(c));
            }
        }
        return ret;
    }
    Mat3<T> operator +(const Mat3<T>& m)const{
        Mat3<T> ret;
        doFor(r,3)
            ret[r] = this->row[r] + m[r];
        return ret;
    }
    Mat3<T> operator -(const Mat3<T>& m)const{
        Mat3<T> ret;
        doFor(r,4)
            ret[r] = this->row[r] - m[r];
        return ret;
    }
    template<typename U>
    Vec3<U> operator *(const Vec3<U>& v){
        Vec3<U> ret;
        doFor(i,3){
            ret[i] = U(0);
            doFor(j,3){
                ret[i] += U(row[i][j]) * v[j];
            }
        }
        return ret;
    }

	std::vector<T> toVector()const {
		std::vector<T> ret(data.begin(), data.end());
		return ret;
	}
private:
	union {
		std::array<Vec3<T>, 3> row;
		std::array<T, 9> data = { 0 };
	};
};
typedef Mat3<float32> Mat3f;
typedef Mat3<float64> Mat3d;

template<typename T>
class Mat4:public P3DZeroObject{
public:
    Mat4<T>(){};
	explicit Mat4(const std::vector<T>& t) {
		doFor(i, 16)
			data[i] = t[i];
	}
    explicit Mat4(T* t){
        doFor(i,16)
            data[i] = t[i];
    }
    Mat4<T>(const Mat4<T>& m){
        doFor(i,4)
            row[i] = m[i];
    }
    Mat4<T>& operator =(const Mat4<T>&m){
        doFor(i,4)
            row[i] = m[i];
        return *this;
    }
public:
    static Mat4<T> getIdentity(){
        Mat4<T> ret;
        ret[0][0] = static_cast<T>(1);
        ret[1][1] = static_cast<T>(1);
        ret[2][2] = static_cast<T>(1);
        ret[3][3] = static_cast<T>(1);
        return ret;
    }
    static Mat4<T> getRotX(float Pitch_){
		auto Pitch = Utils::toRadian(Pitch_);
        auto ret = getIdentity();
        ret[1][1] = T(cos(Pitch));
        ret[1][2] = T(sin(Pitch));
        ret[2][1] = T(-sin(Pitch));
        ret[2][2] = T(cos(Pitch));
        return ret;

    }
    static Mat4<T> getRotY(float32 Yaw_){
		auto Yaw = Utils::toRadian(Yaw_);
        auto ret = getIdentity();
        ret[0][0] = T(cos(Yaw));
        ret[0][2] = T(sin(Yaw));
        ret[2][0] = T(-sin(Yaw));
        ret[2][2] = T(cos(Yaw));
        return ret;
    }
    static Mat4<T> getRotZ(float32 Roll_){
		auto Roll = Utils::toRadian(Roll_);
        auto ret = getIdentity();
        ret[0][0] = T(cos(Roll));
        ret[0][1] = T(-sin(Roll));
        ret[1][0] = T(sin(Roll));
        ret[1][1] = T(cos(Roll));
        return ret;
    }
	static Mat4<T> getRotation(const Vec3f& tRot) {
		return getRotZ(tRot.z)*getRotY(tRot.y)*getRotX(tRot.x);
	}
    static Mat4<T> getScale(T s){
        auto ret = getIdentity();
        doFor(i,3)
            ret[i][i] = s;
        return ret;
    }
    static Mat4<T> getScale(T x,T y, T z){
        return getScale({x,y,z});
    }
    static Mat4<T> getScale(const Vec3<T>& s){
        auto ret = getIdentity();
        doFor(i,3)
            ret[i][i] = s[i];
        return ret;
    }
    static Mat4<T> getTranslate(const Vec3<T>& t){
        auto ret = getIdentity();
        doFor(i,3)
            ret[i][3] = t[i];
        return ret;
    }
    static Mat4<T> getTranslate(T x, T y, T z){
        return getTranslate({x,y,z});
    }
    static Mat4<T> getLookAt(const Vec3<T>& eye,const Vec3<T>& center,const Vec3<T>& up){
        auto ret = getIdentity();
        auto f = (center - eye).normalize();
		auto s = f.cross(up).normalize();
		auto u = s.cross(f).normalize();

		/*
		ret[0][0] = s.x;
		ret[1][0] = s.y;
		ret[2][0] = s.z;
		ret[0][1] = u.x;
		ret[1][1] = u.y;
		ret[2][1] = u.z;
		ret[0][2] =-f.x;
		ret[1][2] =-f.y;
		ret[2][2] =-f.z;
		*/

		ret[0] = s;//{ -s[0],-s[1],-s[2] };
		ret[1] = u;
		
		//ret[2] = f;
		ret[2] = { -f[0],-f[1],-f[2] };
		ret[0][3] = -s.dot(eye);
		ret[1][3] = -u.dot(eye);
		ret[2][3] = f.dot(eye);
		ret[3][3] = 1.0;
        return ret;
    }
    static Mat4<T> getPerspective(T fovy,T aspect,T zNear,T zFar){
        Mat4<T> ret;

        //P3D_ENSURE(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0),\
        //"Mat4::getPerspective error aspect");
        const T halfFovy = (fovy /(static_cast<T>(2))) * static_cast<T>(PI) / static_cast<T>(180);
        const T tanFov = tan(halfFovy);
        ret[0][0] = static_cast<T>(1) / (aspect * tanFov);
        ret[1][1] = static_cast<T>(1) / (tanFov);
        ret[2][2] = -(zFar + zNear) / (zFar- zNear);
        ret[2][3] = -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
        ret[3][2] = -static_cast<T>(1);

        return ret;
    }
	//static Mat4<T> getKSPerspective2()
	static Mat4<T> getKSPerspective2(T fov, float aspect) {
		Mat4<T> ret;
        const T halfFovy = (fov /(static_cast<T>(2))) * static_cast<T>(PI) / static_cast<T>(180);
		double dist = static_cast<T>(1.0 / tan(halfFovy));
		ret[0][0] = static_cast<T>(dist * aspect);
		ret[1][1] = static_cast<T>(dist);
		ret[2][2] = static_cast<T>(0);
		ret[3][3] = static_cast<T>(0);
		ret[2][3] = static_cast<T>(1);
		ret[3][2] = static_cast<T>(-1);
		return ret;
	}
	static Mat4<T> getOrthognal(T xMin, T xMax, T yMin, T yMax, T zMin, T zMax) {
		Mat4<T> ret;
		ret[0][0] = static_cast<T>(2 / (xMax - xMin));
		ret[1][1] = static_cast<T>(2 / (yMax - yMin));
		ret[2][2] = static_cast<T>(-2 / (zMax - zMin));
		ret[3][3] = static_cast<T>(1);
		
		ret[0][3] = static_cast<T>(-(xMax + xMin) / (xMax - xMin));
		ret[1][3] = static_cast<T>(-(yMax + yMin) / (xMax - xMin));
		ret[2][3] = static_cast<T>(-(zMax + zMin) / (zMax - zMin));
		return ret;
	}
public:
    Mat4<T>& rotate(T pitch,T yaw,T roll){
        auto Rotx = getRotX(pitch);
        auto Roty = getRotY(yaw);
        auto Rotz = getRotZ(roll);
        *this = Rotz * Roty * Rotx * (*this);
        return *this;
    }
   /* Mat4<T>& rotate(Vec3<T> rot){
        auto Rotx = getRotX(Vec3[0]);
        auto Roty = getRotY(Vec3[1]);
        auto RotZ = getRotX(Vec3[2]);
        *this = Rotz * Roty * Rotx * (*this);
        return *this;
    }*/
    Mat4<T>& translate(T x, T y,T z){
        row[0][3] += x;
        row[1][3] += y;
        row[2][3] += z;
        return *this;
    }
    Mat4<T>& translate(Vec3<T> v){
        doFor(i,3)
            row[i][3] = v[i];
        return *this;
    }
    Mat4<T>& scale(T s){
        *this = getScale(s) * (*this);
        return *this;
    }
    Mat4<T>& scale(T x,T y,T z){
        return scale({x,y,z});
    }
    Mat4<T>& scale(const Vec3<T>& s){
        *this = getScale(s) * (*this);
        return *this;
    }
	Mat4<T> getTranspose()const {
		Mat4<T> ret;
		doFor(i, 4)
			doFor(j, 4)
			ret[i][j] = this->row[j][i];
		return ret;
	}
	Mat3<T> getRoteMat3() const {
		Mat3<T> ret;
		doFor(i, 3)
			doFor(j, 3)
			ret[i][j] = row[i][j];
		return ret;
	}
	Mat4<T> simpleInvers()const {
		Mat4<T> ret;
		doFor(i, 3)
			doFor(j, 3)
			ret[i][j] = this->row[j][i];
		doFor(i, 3)
			ret[3][i] = 0;
		ret[3][3] = 1;

		Vec3<T> posV(row[0][3], row[1][3], row[2][3]);
		auto mat3 = getRoteMat3();
		auto tPos = mat3 * posV;
		doFor(i, 3)
			ret[i][3] = -tPos[i];
		return ret;
	}
public:
    Vec4<T>& operator [](uint32 index){
        P3D_ENSURE(index < 4,"Mat4::[] INValide index");
        return row[index];
    }
    const Vec4<T>& operator [](uint32 index) const{
        P3D_ENSURE(index < 4,"Mat4::[] INValide index");
        return row[index];
    }
    T& operator [](const std::array<uint32,2>& index){
        P3D_ENSURE((index[0] < 4 && index[1] < 4),"Mat4::[{}] INValide index");
        return data[index[0] * 4 + index[1]];
    }
	//bool operator ==（）
	//只适用于变换矩阵
	Mat4<T>  inv()const  {
		T sx = static_cast<T>(sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]));
		T sy = sqrt(data[4] * data[4] + data[5] * data[5] + data[6] * data[6]);
		T sz = sqrt(data[8] * data[8] + data[9] * data[9] + data[10] * data[10]);

		auto invS = Mat4<T>::getScale({ T(1.0/sx),T(1.0/sy),T(1.0/sz) });
		auto invT = Mat4<T>::getTranslate({ -data[3],-data[7],-data[11] });

		Mat4<T> invR;
		invR[0] = row[0] / sx;
		invR[1] = row[1] / sy;
		invR[2] = row[2] / sz;
		for (int i = 0; i < 3; ++i)
			invR[i][3] = 0.0;
		invR[3][3] = 1.0;
		invR = invR.getTranspose();
		return invS*invR*invT;
	}
	Vec3f getEluerRotation()const {
		float32 sx = static_cast<float32>(sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]));
		float32 sy = sqrt(data[4] * data[4] + data[5] * data[5] + data[6] * data[6]);
		float32 sz = sqrt(data[8] * data[8] + data[9] * data[9] + data[10] * data[10]);
		float dd00 = data[0] / sx, dd01 = data[1] / sx, dd02 = data[2] / sx;
		float dd10 = data[4] / sy, dd11 = data[5] / sy, dd12 = data[6] / sy;
		float dd20 = data[8] / sz, dd21 = data[9] / sz, dd22 = data[10] / sz;
		// 判断非正交阵, 这里出过一个bug，这里的阈值设的太小，导致不能正确解析出rot
		float dotv = dd00 * dd10 + dd01 * dd11 + dd02 * dd12;
		Vec3f ret;
		if (fabs(dotv) > 0.3) {
			return ret;
		}
		dotv = dd00 * dd20 + dd01 * dd21 + dd02 * dd22;
		if (fabs(dotv) > 0.3) {
			return ret;
		}
		dotv = dd20 * dd10 + dd21 * dd11 + dd22 * dd12;
		if (fabs(dotv) > 0.3) {
			return ret;
		}
		// 得到欧拉角
		float a = -atan2(dd21, dd22);
		float b = atan2(-dd20, sqrt(dd21*dd21 + dd22 * dd22));
		float c = atan2(dd10, dd00);
		ret = { a, b, c };
		return ret;
	}

    const T& operator [](const std::array<uint32,2>& index) const{
        P3D_ENSURE((index[0] < 4 && index[1] < 4),"Mat4::[{}] INValide index");
        return data[index[0] * 4 + index[1]];
    }
    Mat4<T> operator*(const Mat4<T>& m) const{
        Mat4<T> ret;
        doFor(r,4){
            doFor(c,4){
                ret[r][c] = this->row[r].dot(m.getCol(c));
            }
        }
        return ret;
    }
    Mat4<T> operator +(const Mat4<T>& m)const{
        Mat4<T> ret;
        doFor(r,4)
            ret[r] = this->row[r] + m[r];
        return ret;
    }
    Mat4<T> operator -(const Mat4<T>& m)const{
        Mat4<T> ret;
        doFor(r,4)
            ret[r] = this->row[r] - m[r];
        return ret;
    }
    template<typename U>
    Vec4<U> operator *(const Vec4<U>& v)const {
        Vec4<U> ret;
        doFor(i,4){
            ret[i] = U(0);
            doFor(j,4){
                ret[i] += U(row[i][j]) * v[j];
            }
        }
        return ret;
    }
    template<typename U>
    Vec3<U> operator *(const Vec3<U>  v)const {
        auto ret = (*this) * Vec4<U>(v);
        return {ret.x,ret.y,ret.z};
    }
	T* rawData() {
		return static_cast<T*>(data.data());
	}
	Vec4<T> getCol(uint8 index)const {
		P3D_ENSURE(index < 4,  "Mat4::getCol INValide index");
		Vec4<T> ret;
		doFor(i, 4) {
			ret[i] = row[i][index];
		}
		return ret;
	}
public:
    void print(){
        doFor(i,4)
            row[i].print();
    }
	std::vector<T> toVector() const {
		std::vector<T> ret(data.begin(), data.end());
		return ret;
	}
	std::vector<T> toVec() {
		return std::vector<T>(data.begin(), data.end());
	}
private:
    union{
        std::array<Vec4<T>,4> row;
        std::array<T,16> data = {0};
    };
};
typedef Mat4<float32> Mat4f;
typedef Mat4<float64> Mat4d;

END_P3D_NS
