#include"mathVector.hpp"
#include "mathMatrix.hpp"
#pragma once

BEGIN_P3D_NS
template<typename T>
class Quaternion :public P3DZeroObject{
public:
	Quaternion<T>() : x(0), y(0), z(0), w(0) {};
    Quaternion<T>(T x, T y, T z,T w):x(x),y(y),z(z),w(w){};
	Quaternion<T>(Vec3<T> v) : v(v), w(0) {};
    Quaternion<T>(Vec3<T> v,T w): v(v), w(w){};
    Quaternion<T>(const Quaternion<T>& q): v(q.v),w(q.w){};
public:
	Quaternion<T> operator+ (const Quaternion<T>& r) const {
		Quaternion<T> ret;
		ret.v = this->v + r.v;
		ret.w = this->w + r.w;
		return ret;
		//return Quaternion<T>(v + r.v, w + r.w);
	}
	Quaternion<T> operator- (const Quaternion<T>& r) const {
		return Quaternion<T>(v - r.v, w - r.w);
	}
	Quaternion<T> operator* (const Quaternion<T>& r) const {
		Quaternion<T> ret;
		ret.w = static_cast<T>(x * r.w - v.dot(r.v));
		ret.x = static_cast<T>(w * r.x + r.w * x + y * r.z - z * r.y);
		ret.y = static_cast<T>(w * r.y + r.w * y + z * r.x - r.z * x);
		ret.z = static_cast<T>(w * r.z + r.w * z + x * r.y - r.x * y);
		return ret;
	}
public:
	Mat4f toMatrix() const {
		Mat4f ret = Mat4f::getIdentity();
		auto y2 = static_cast<float32>(y*y);
		auto x2 = static_cast<float32>(x*x);
		auto z2 = static_cast<float32>(z*z);
		ret[{0, 0}] = 1 - 2 * y2 - 2 * z2;
		ret[{0, 1}] = 2 * x*y - 2 * z *w;
		ret[{0, 2}] = 2 * x*z + 2 * y * w;
		
		ret[{1, 0}] = 2*x*y + 2*z*w;
		ret[{1, 1}] = 1 - 2*x2 - 2*z2;
		//ret[{1, 2}] = 2*y*z ¡ª 2*x*w;
		
		ret[{2, 0}] = 2 *x*z - 2 *y*w;
		ret[{2, 1}] = 2 *y*z + 2 *x*w;
		ret[{2, 2}] = 1 - 2*x2 - 2*y2;
		
		return ret;
	}
	Vec3f toRotation()const {
		Vec3f ret;
		float32 xx = x*x, yy = y*y, zz = z*z, ww = w*w;
		ret.x = Utils::toDegree(atan2(2.0f*(x*w - y * z), 1.0 - 2.0*(xx + zz)));
		ret.y = Utils::toDegree(atan2(2.0f*(y*w + x * z), 1.0f - 2.0f*(yy + zz)));
		ret.z = Utils::toDegree(asin(2.0f*(x*y + z * w)));
		return ret;
	}
public:
     static Quaternion<T> getRotateQ(const Vec3<T>& axis,T angle){
        const T halfAngle = (angle /(static_cast<T>(2))) * static_cast<T>(PI) / static_cast<T>(180);
        Quaternion<T> ret;
        ret.w = static_cast<T>(cos(halfAngle));
        ret.v = axis * static_cast<T>(sin(halfAngle));
        return ret; 
    }
     Quaternion<T> getFromEualerAngles(const Vec3<T>& angles){
        auto halfAngles = (angles / static_cast<T>(2)) * static_cast<T>(PI) / static_cast<T>(180);

        T cx = static_cast<T>(cos(halfAngles.x));
        T cy = static_cast<T>(cos(halfAngles.y));
        T cz = static_cast<T>(cos(halfAngles.z));

        T sx = static_cast<T>(sin(halfAngles.x));
        T sy = static_cast<T>(sin(halfAngles.y));
        T sz = static_cast<T>(sin(halfAngles.z));

	    w = static_cast<T>(cx * cy * cz - sx * sy * sz);
		x = static_cast<T>(sx*cy*cz - cx * sy*sz);
	   // x = static_cast<T>(cx * sy * sz + sx * cy * cz);
	    y = static_cast<T>(cx*sy*cz+sx*cy*sz);
	    z = static_cast<T>(cx*cy*sz - sx * sy * cz);

    }
     static Vec3<T> rotateVec(const Vec3<T>& src,const Vec3<T> &axis,T angle){
        auto quatern = getRotateQ(axis,angle);
        Quaternion<T> srcQua(src);
        auto retQua = quatern * srcQua * quatern.conjugate();
        return  retQua.v;
    }
public:
    Quaternion<T> conjugate(){
        return Quaternion<T>(-x,-y,-z,w);
    }
    T lengthSqaure(){
        return static_cast<T>(x * x + y * y + z* z + w * w);
    }
    T length(){
        return static_cast<T>(sqrt(lengthSqaure()));
    }
    T dot(const Quaternion<T> r){
        return static_cast<T>(w * w + v.dot(r.v));
    }
    Quaternion<T> Inverse(){
        return conjugate() / length();
    }



public:
    union{
        struct{
            T x,y,z;
        };
        Vec3<T> v;
    };
    T w;
};
using Quaternionf = Quaternion<float32>;
using QuaternionSd = Quaternion<float64>;
/*
template<typename T>
Quaternion<T> matrix2Qaut(const Mat4<T>& m) {
}
*/
END_P3D_NS