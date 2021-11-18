#pragma once
#include "PhyRigid.h"
#include "phyCommon.h"
BEGIN_P3D_NS
class PhyStaticRigid : public PhyRigid {
public:
	PhyStaticRigid(physx::PxPhysics* phy, PhyGeoType geoType,const PhyMaterial& material, const Mat4f& modelMat, const std::vector<float>& externData);
	virtual ~PhyStaticRigid() {};

public:

};


END_P3D_NS
