
#pragma once
#include "common/mathIncludes.h"
#include"common/P3D_Utils.h"

BEGIN_P3D_NS
class Camera :public P3DObject {
public:
	enum CAMERA_TYPE {
		CAMERA_TYPE_ORTH,
		CAMERA_TYPE_PRO,
	};
public:
	Camera();
	//Camera(const Vec3f& e,const Vec3f& u,const Vec3f& d);
public:
	void setViewParams(const Vec3f& e, const Vec3f& d, const Vec3f& u = Vec3f(0.0f, 1.0f, 0.0f));
	void setViewMat(const Mat4f& tMat) { viewMat = tMat; }
	void setOrthMat(const Mat4f& tMat) { orthMat = tMat; };
	void setProMat(const Mat4f& tMat) { projMat = tMat; };
	void setCameraPos(const Vec3f& pos);
	void setProjParams(float fov, float asp, float nearP, float fraP);
	void setOrthParams(float32 w, float32 h, float32 nearZ, float farZ);
	void setOrthWidth(float32 w);
	void setCameraType(uint8 t) { type = t; }
	void setOrthDis(float32 d) { orthDis = d; }
	float32 getOrthDis() { return orthDis; }
	uint8 getType() { return type; }

public:
	Vec3f getEyePos() const { return eyePos; }
	Vec3f getDstPos() const { return dstPos; }
    Vec3f getUpDir() const {return upVec;}
	Vec3f getRightDir()const { return rightVec; }
    Vec3f getLookVec(){return forwardVec;}
    Mat4f getViewMatrix(){return viewMat;}
    Mat4f getProjMatrix(){return projMat;}
	Mat4f getOrthMatrix() { return orthMat; }
    Mat4f getInverseViewMatrix(){return inverseViewMat;}
    Mat4f getInverseProjMatris(){return inverseProjMat;}
    Mat4f getViewProjMatrix(){return projMat * viewMat;}
	Mat4f getViewOrthMatrx() { return orthMat * viewMat;}
	float32 getOrthWidth() { return orthW; }
	float32 getOrthHeight() { return orthH; }
	void print();
public:
    void move(float right,float forward);
    void rotate(float pitch,float yaw);
    

public:

	float fov;
    float asp;
    float nearPlane;
    float farPlane;
private:
	uint8 type = CAMERA_TYPE_PRO;
	float32 orthW = 1.0f;
	float32 orthH = 1.0f;
	float32 orthNearZ = 0.1f;
	float32 orthFarZ = 100.0f;

	float orthDis = 1.0;

    Vec3f eyePos;
    Vec3f upVec;
    Vec3f forwardVec;
    Vec3f rightVec;
	Vec3f dstPos;


    Mat4f viewMat;
    Mat4f projMat;
	Mat4f orthMat;
    Mat4f inverseViewMat;
    Mat4f inverseProjMat;
};
typedef std::shared_ptr<Camera> pCamera;
END_P3D_NS