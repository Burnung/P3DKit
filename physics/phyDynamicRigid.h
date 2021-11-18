#pragma once
#include "PhyRigid.h"
#include "common/mathMatrix.hpp"

BEGIN_P3D_NS
class PhyDynamicRigid : public PhyRigid {
public:
	PhyDynamicRigid(physx::PxPhysics*,PhyGeoType getType,const PhyMaterial& maetrial, const Mat4f& mat,const std::vector<float>& externData,float32 density);
	virtual ~PhyDynamicRigid() {}
public:
	virtual Mat4f updatePostion();
	virtual void addForce(const Vec3f& force, PhyForceType fType);

public:
	Mat4f inverseMat;

};


END_P3D_NS
