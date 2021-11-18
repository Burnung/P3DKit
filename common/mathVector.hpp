#pragma once

#include<array>
#include <math.h>
#include <type_traits>
#include"P3D_Utils.h"
#include "Template_Utils.h"
#include<iostream>
BEGIN_P3D_NS


template <typename T>//, typename  = typename std::enable_if<IsBaseType<T>::Value>::type>
class Vec2 :public P3DZeroObject{
public:
	Vec2<T>() { x = T(0), y = T(0.0); }
   // ~Vec2<T>() = default;
	Vec2<T>(const Vec2<T>& t) {
		x = t.x;
		y = t.y;
	}
    Vec2<T>(T* pdata){
        data[0] = pdata[0];
        data[1] = pdata[1];
    };
    Vec2<T>(T x,T y){
        this->x = x;
        this->y = y;
    };
public:
//operator
Vec2<T>& operator =(const Vec2<T>& t) {
	x = t.x;
	y = t.y;
	return *this;
}
T& operator [](uint32 index){
    P3D_ENSURE(index < 2,"Vec2::[] Invalid index");
    return data[index];
}
const T& operator [](uint32 index) const{
    P3D_ENSURE(index < 2,"Vec2::[] Invalid index");
    return data[index];
}
Vec2<T> operator *(T f)const{
    Vec2<T> ret(*this);
    doFor(i,2)
        ret.data[i] *= f;
    return ret;
}
Vec2<T> operator /(T f) const{
    P3D_ENSURE(abs(f) > 1e-5,"invlid devide");
    Vec2<T> ret(*this);
    doFor(i,2)
        ret.data[i] /= f;
    return ret;
}
Vec2<T> operator *(const Vec2<T>& l) const {
    Vec2<T> ret(*this);
    doFor(i,2)
        ret[i] *= l[i];
    return ret;
}
Vec2<T> operator /(const Vec2<T>& l) const{
    Vec2<T> ret(*this);
    doFor(i,2)
        ret[i] /= l[i];
    return ret;
}
Vec2<T> operator -(const Vec2<T>& l)const{
    Vec2<T> ret(*this);
    doFor(i,2)
        ret[i] -= l[i];
    return ret;
}
Vec2<T> operator +(const Vec2<T>& l) const{
    Vec2<T> ret(*this);
    doFor(i,2)
        ret[i] += l[i];
    return ret;
}

Vec2<T>& operator -=(const Vec2<T>& l){
    doFor(i,2)
        data[i] -= l[i];
    return *this;
}
Vec2<T>& operator +=(const Vec2<T>& l){
    doFor(i,2)
        data[i] += l[i];
    return *this;
}
Vec2<T>& operator *=(const Vec2<T>& l){
    doFor(i,2)
        data[i] *= l[i];
    return *this;
}

Vec2<T>& operator /=(float l){
    doFor(i,2)
        data[i] /= l;
    return *this;
}

Vec2<T>& operator *=(T t){
    doFor(i,2)
        data[i] *= t;
    return *this;
}

public:
inline Vec2<T> rotate(float angle) const {
		float _cos = cos(angle);
		float _sin = sin(angle);
		return {
			static_cast<T>(x*_cos - y * _sin),
			static_cast<T>(x*_sin + y * _cos)
		};
	}
T dot(const Vec2<T>& r)const{
    float64 ret = 0;
    doFor(i,2)
        ret += data[i] * r[i];
    return T(ret);
}
Vec2<T> cross(const Vec2<T>& r)const{
    Vec2<T> ret;
    ret.x = r.x;
    ret.y = r.y;
    return ret;
}
float64 length() const{
    float64 ret = 0;
    doFor(i,2)
        ret += data[i] * data[i];
    return sqrt(ret);
}
float64 sqrLenght()const {
	return x * x + y * y;
}
Vec2<T> normalize()const{
    Vec2<T> ret(*this);
    auto l = ret.length();
    return ret / l;
}
void doNormalize(){
    auto l = length();
    doFor(i,2)
        data[i] /= l;
}
T* ptr(){
    return data.data();
}

void print(){
    std::cout<<x<<" "<<y<<" "<<std::endl;
}
public:

union{
    struct{T x,y;};
    std::array<T,2> data;
};
};
typedef Vec2<float32> Vec2f;
typedef Vec2<float64> Vec2d;
typedef Vec2<int32> Vec2i;

template<typename T>
class Vec3 :public P3DZeroObject{
public:
    Vec3<T>():Vec3<T>(0,0,0){};
    ~Vec3<T>() = default;
    Vec3<T>(T* pdata){
        doFor(i,3)
            data[i] = pdata[i];
    }
    Vec3<T>(T x,T y,T z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
	Vec3<T>(const std::vector<T>& tv) : Vec3<T>(tv[0], tv[1], tv[2]) {};
public:
	static Vec3<T> getSpPos(double theta, double phi) {
		theta = theta / 180.0 * PI;
		phi = phi / 180.0 * PI;
		auto y = static_cast<T>(sin(theta));
		auto r = static_cast<T>(cos(theta));

		auto x = static_cast<T>(r*sin(phi));
		auto z = static_cast<T>(r*cos(phi));
		return Vec3<T>(x, y, z);
	}
public:
//operator
T& operator [](uint32 index){
    P3D_ENSURE(index < 3,"Vec3::[] Invalid index");
    return data[index];
}
const T& operator [](uint32 index) const{
    P3D_ENSURE(index < 3,"Vec3::[] Invalid index");
    return data[index];
}
Vec3<T> operator *(T f)const{
    Vec3<T> ret(*this);
    doFor(i,3)
        ret.data[i] *= f;
    return ret;
}
Vec3<T> operator /(T f) const{
    P3D_ENSURE(abs(f) > 1e-5,"invlid devide");
    Vec3<T> ret(*this);
    doFor(i,3)
        ret.data[i] /= f;
    return ret;
}
Vec3<T> operator *(const Vec3<T>& l) const {
    Vec3<T> ret(*this);
    doFor(i,3)
        ret[i] *= l[i];
    return ret;
}
Vec3<T> operator /(const Vec3<T>& l) const{
    Vec3<T> ret(*this);
    doFor(i,3)
        ret[i] /= l[i];
    return ret;
}
Vec3<T> operator -(const Vec3<T>& l)const{
    Vec3<T> ret(*this);
    doFor(i,3)
        ret[i] -= l[i];
    return ret;
}
Vec3<T> operator +(const Vec3<T>& l) const{
    Vec3<T> ret(*this);
    doFor(i,3)
        ret[i] += l[i];
    return ret;
}

Vec3<T>& operator -=(const Vec3<T>& l){
    doFor(i,3)
        data[i] -= l[i];
    return *this;
}
Vec3<T>& operator +=(const Vec3<T>& l){
    doFor(i,3)
        data[i] += l[i];
    return *this;
}
Vec3<T>& operator *=(const Vec3<T>& l){
    doFor(i,3)
        data[i] *= l[i];
    return *this;
}
Vec3<T>& operator *=(T t){
    doFor(i,4)
        data[i] *= t;
    return *this;
}

public:
T dot(const Vec3<T>& r)const{
    float64 ret = 0;
    doFor(i,3)
        ret += data[i] * r[i];
    return T(ret);
}
Vec3<T> cross(const Vec3<T>& r)const{
    Vec3<T> ret;
   /* doFor(i,3){
        uint32 li = (i + 1) % 3;
        uint32 ri = (i + 2) % 3;
        ret[i] = data[li] * r[ri] - data[ri] * r[li];
    }*/
	ret[0] = data[1] * r[2] - data[2] * r[1];
	ret[1] = data[2] * r[0] - data[0] * r[2];
	ret[2] = data[0] * r[1] - data[1] * r[0];
    return ret;
}
T min()const {
	return std::min(data[0], std::min(data[1], data[2]));
}
T max() const {
	return std::max(data[0], std::max(data[1], data[2]));
}
float64 length() const{
    float64 ret = 0;
    doFor(i,3)
        ret += data[i] * data[i];
    return sqrt(ret);
}
Vec3<T> normalize()const{
    Vec3<T> ret(*this);
    auto l = (ret.length()+1e-5);
    return ret / l;
}
void doNormalize(){
    auto l = length();
    doFor(i,3)
        data[i] /= (l+1e-6);
}
void print(){
    std::cout<<x<<" "<<y<<" "<<z<<std::endl;
}
std::vector<T> toVector() {
	return { data[0],data[1],data[2] };
}
T* ptr(){
    return data.data();
}
public:

union{
    struct{T x,y,z;};
    struct{T r,g,b;};
    std::array<T,3> data;
};
};

typedef Vec3<float32> Vec3f;
typedef Vec3<float64> Vec3d;
typedef Vec3<int32> Vec3i;

template<typename T>
class Vec4 :public P3DZeroObject{
public:
    Vec4<T>():Vec4<T>(0,0,0,0){};
    ~Vec4<T>() = default;
    Vec4<T>(T* pdata){
        doFor(i,4)
            data[i] = pdata[i];
    }
    Vec4<T>(T x,T y,T z,T w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    Vec4<T>(const Vec3<T>& v3):Vec4<T>(v3.x,v3.y,v3.z,1.0f){};
	Vec4<T>(const Vec3<T>& v3, T a) : Vec4<T>(v3.x, v3.y, v3.z, a) {};
	Vec4<T>(const std::vector<T>& tv) : Vec4<T>(tv[0], tv[1], tv[2], tv[3]) {};
public:
//operator
T& operator [](uint32 index){
    P3D_ENSURE(index < 4,"Vec4::[] Invalid index");
    return data[index];
}
const T& operator [](uint32 index) const{
    P3D_ENSURE(index < 4,"Vec4::[] Invalid index");
    return data[index];
}
Vec4<T> operator *(T f) const{
    Vec4<T> ret(*this);
    doFor(i,4)
        ret.data[i] *= f;
    return ret;
}
Vec4<T> operator /(T f) const{
    P3D_ENSURE(abs(f) > 1e-5,"invlid devide");
    Vec4<T> ret(*this);
    doFor(i,4)
        ret.data[i] /= f;
    return ret;
}
Vec4<T> operator *(const Vec4<T>& l) const{
    Vec4<T> ret(*this);
    doFor(i,4)
        ret[i] *= l[i];
    return ret;
}
Vec4<T> operator /(const Vec4<T>& l)const{
    Vec4<T> ret(*this);
    doFor(i,4)
        ret[i] /= l[i];
    return ret;
}
Vec4<T> operator -(const Vec4<T>& l)const{
    Vec4<T> ret(*this);
    doFor(i,4)
        ret[i] -= l[i];
    return ret;
}
Vec4<T> operator +(const Vec4<T>& l)const{
    Vec4<T> ret(*this);
    doFor(i,4)
        ret[i] += l[i];
    return ret;
}

Vec4<T>& operator -=(const Vec4<T>& l){
    doFor(i,4)
        data[i] -= l[i];
    return *this;
}
Vec4<T>& operator +=(const Vec4<T>& l){
    doFor(i,4)
        data[i] += l[i];
    return *this;
}
Vec4<T>& operator *=(const Vec4<T>& l){
    doFor(i,4)
        data[i] *= l[i];
    return *this;
}
Vec4<T>& operator *=(T t){
    doFor(i,4)
        data[i] *= t;
    return *this;
}
const Vec4<T>& operator =(const Vec3<T>& t) {
	doFor(i, 3)
		data[i] = t[i];
		return *this;
}
public:
T dot(const Vec4<T>& r)const{
    float64 ret = 0;
    doFor(i,4)
        ret += data[i] * r[i];
    return T(ret);
}
T cross(const Vec4<T>& r)const{
    Vec4<T> ret;
    doFor(i,4){
        uint32 li = (i + 1) % 4;
        uint32 ri = (i + 2) % 4;
        ret[i] = data[li] * r[li] - data[ri] * r[li];
    }
    return ret;
}
float64 length()const{
    float64 ret = 0;
    doFor(i,4)
        ret += data[i] * data[i];
    return sqrt(ret);
}
Vec4<T> normalize()const{
    Vec4<T> ret(*this);
    auto l = ret.length();
    return ret / l;
}
T sum() const {
	T r = 0;
	doFor(i, 4)
		r += data[i];
	return r;
}

void doNormalize(){
    auto l = length();
    doFor(i,4)
        data[i] /= l;
}
T* ptr(){
    return data.data();
}

void print(){
    std::cout<<x<<" "<<y<<" "<<z<<" "<<w<<std::endl;
}

std::vector<T> toVector()const {
	std::vector<T> ret(data.begin(), data.end());
	return ret;
}

public:

union{
    struct{T x,y,z,w;};
    struct{T r,g,b,a;};
    std::array<T,4> data;
};
};


typedef Vec4<float32> Vec4f;
typedef Vec4<float64> Vec4d;
typedef Vec4<int32> Vec4i;
typedef Vec4<uint16> Vec4u16;
typedef Vec4<int16> Vec4i16;

END_P3D_NS
