#pragma once

#include "common/P3D_Utils.h"
#include "phyCommon.h"
#include "PhyRigid.h"
#include "phyJointChain.h"
BEGIN_P3D_NS

class PhyScene : public P3DObject {
public:
	explicit PhyScene(physx::PxPhysics* phy, uint32 numWorks = 8, Vec3f gravity = {0.0,-9.8,0.0});
	virtual ~PhyScene();
public:
	void release();
	inline physx::PxScene* getPxScene() { return pxScene;}
public:
	void addRigid(pPhyRigid rigid);
	void addJointChain(pPhyJointChain joints);

	void addForce(const Vec3f& force,PhyForceType forceType);

	inline 	void update(float32 esp) {
		pxScene->simulate(esp);
		pxScene->fetchResults(true);
	}

public:
	
private:
	physx::PxDefaultCpuDispatcher* pxDispatcher;
	physx::PxScene* pxScene;
	std::vector<pPhyRigid> rigids;
};
typedef std::shared_ptr<PhyScene> pPhyScene;


END_P3D_NS