#pragma once
#include "common/P3D_Utils.h"
#include "phyCommon.h"
#include "common/mathIncludes.h"
BEGIN_P3D_NS

class PhyRigid :public P3DObject {
public:
	//static std::shared_ptr<PhyRigid> make()
public:
	PhyRigid() {};
	
	virtual void addForce(const Vec3f& force, PhyForceType fType) {};
	virtual Mat4f updatePostion() { return modelMat; }
public:
	PhyRigidType rigidType;
	Mat4f modelMat;
	Mat4f srcMat;
	Mat4f relativeMat = Mat4f::getIdentity();
	Vec3f relativePos;
	PhyGeoType geoType;
	physx::PxRigidBody* pxRigid = nullptr;
};
typedef std::shared_ptr<PhyRigid> pPhyRigid;
END_P3D_NS
