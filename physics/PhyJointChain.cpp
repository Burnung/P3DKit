#include "phyJointChain.h"
#include "PhyEngine.h"

BEGIN_P3D_NS

using namespace physx;
using namespace std;


std::vector<physx::PxJoint*> PhyJointChain::addJoints(std::vector<pPhyRigid> rigids_, const std::vector<float>& externData){
	std::vector<PxJoint*> ret;
	for (auto r : rigids_) {
		auto* j = addJoint(r, externData);
		if (j)
			ret.push_back(j);
	}
	return ret;
}
/*
std::vector<physx::PxJoint*> PhyJointChain::addJoints(std::vector<Mat4f>& mats, PhyGeoType geoType,const PhyMaterial& material, const std::vector<float>& geoData, std::vector<float> externData) {
	if (mats.size() != geoData.size()) {
		P3D_THROW_ARGUMENT("PhyJointChain::addJoints error ,invalid size of mats add geoData");
	}
	vector<PxJoint*> ret;
	for (uint32 i = 0; i < mats.size(); ++i) {
		auto r = PhyEngine::instance().createRigid(PHY_RIGIDTYPE_DYNAMIC, geoType, material, mats[i], geoData);
		auto*j = addJoint(r, externData);
		if (j)
			ret.push_back(j);
	}
	return j;
}
*/
physx::PxJoint* PhyJointChain::addJoint(pPhyRigid rigid,const std::vector<float32>& externData) {
	PxRigidActor* pre = nullptr;
	PxJoint* ret;

	Vec4f prePos = rigid->modelMat * Vec4f(0.0, 0.0, 0.0, 1.0);
	Vec4f nowPos(0.0,0.0,0.0,1.0);
	if (!rigids.empty()) {
		pre = rigids.back()->pxRigid;
		auto prePost = rigids.back()->modelMat * Vec4f(0.0, 0.0, 0.0, 1.0);
		auto nowPost = rigid->modelMat * Vec4f(0.0, 0.0, 0.0, 1.0);

		auto deltaPos = nowPost - prePost;// +Vec3f(0.0, 1.0, 0.0);
		prePos = (deltaPos * 0.5);
		nowPos = deltaPos * (-0.5);
	}
	auto* phy = PhyEngine::instance().getPxPhysics();
	switch (jointType) {
	case PHY_JOINTTYPE_SPHERE:
	{	
		auto* joint = PxSphericalJointCreate(*phy, pre, PxTransform({ prePos.x,prePos.y,prePos.z }), rigid->pxRigid, PxTransform({nowPos.x,nowPos.y,nowPos.z}));
		//auto* joint = PxDistanceJointCreate(*phy, pre, PxTransform({ prePos.x,prePos.y,prePos.z }), rigid->pxRigid, PxTransform({nowPos.x,nowPos.y,nowPos.z}));
		if (!externData.empty()) {
			//joint->setLimitCone(PxJointLimitCone(externData[0], externData[1],externData[2]));
			joint->setLimitCone(PxJointLimitCone(0.01,0.01 ,0.5));
			joint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		}
		ret = joint;
		break;
	}
	case PHY_JOINTTYPE_DISTANCE:
	{
		auto* joint = PxDistanceJointCreate(*phy, pre, PxTransform({ prePos.x, prePos.y, prePos.z }), rigid->pxRigid, PxTransform({nowPos.x,nowPos.y,nowPos.z}));
		if (pre) {
			auto dis = (nowPos - prePos).length();
			auto tDis = joint->getDistance();
			//joint->setMinDistance(dis);
			//joint->setMaxDistance(dis);
			//joint->setDistanceJointFlag(PxDistanceJointFlag::eMIN_DISTANCE_ENABLED,true);
			//joint->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED,true);
			joint->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED,false);
		}
		/*if (!externData.empty()) {
			joint->

		}*/
		ret = joint;
		break;
	}
	case PHY_JOINTTYPE_FIXED:
	{
		auto* joint = PxFixedJointCreate(*phy, pre, PxTransform({ prePos.x, prePos.y, prePos.z }), rigid->pxRigid, PxTransform({nowPos.x,nowPos.y,nowPos.z}));
		ret = joint;
		break;
	}
	default:
		break;
	}
	if (ret) {
		joints.push_back(ret);
		rigids.push_back(rigid);
	}
	return ret;

}



END_P3D_NS