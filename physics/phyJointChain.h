#pragma once
#include "phyCommon.h"
#include "PhyRigid.h"
#include <vector>
BEGIN_P3D_NS

class PhyJointChain : public P3DObject {
public:
	PhyJointChain(physx::PxPhysics* phy, PhyJointType jointType,const Vec3f& defDelta) :jointType(jointType), defDelta(defDelta){}
	virtual ~PhyJointChain() {
		rigids.clear();
		joints.clear();
	}
public:
	std::vector<physx::PxJoint*> addJoints(std::vector<pPhyRigid> rigids,const std::vector<float>& externData = {});
	//std::vector<physx::PxJoint*> addJoints(std::vector<Mat4f>& rigidMats, PhyGeoType geoType, const std::vector<float>& geoData, std::vector<float> externData);
	physx::PxJoint* addJoint(pPhyRigid rigids, const std::vector<float>& extData = {});
	std::vector<pPhyRigid> getRigids() { return rigids; }

protected:
	PhyJointType jointType;
	Vec3f defDelta;

	std::vector<pPhyRigid> rigids;
	std::vector<physx::PxJoint*> joints;

};
typedef std::shared_ptr<PhyJointChain> pPhyJointChain;


END_P3D_NS