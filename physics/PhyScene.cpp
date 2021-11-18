#include "PhyScene.h"

BEGIN_P3D_NS
using namespace physx;

PhyScene::~PhyScene() {
	release();
}

PhyScene::PhyScene(PxPhysics* phy,uint32 numWorks,Vec3f gravity) {
	P3D_ENSURE(phy, "error PhyScene Pxphysics should not be null!");

	PxSceneDesc sceneDesc(phy->getTolerancesScale());
	sceneDesc.gravity = PxVec3(gravity.x, gravity.y, gravity.z);
	pxDispatcher = PxDefaultCpuDispatcherCreate(numWorks);
	sceneDesc.cpuDispatcher = pxDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	pxScene = phy->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
	if (pvdClient){
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

}


void PhyScene::release() {
	PX_RELEASE(pxScene);
	PX_RELEASE(pxDispatcher);
}

void PhyScene::addRigid(pPhyRigid rigid) {
	for (const auto& t : rigids)
		if (t == rigid)
			return;
	rigids.push_back(rigid);
	pxScene->addActor(*(rigid->pxRigid));

}

void PhyScene::addForce(const Vec3f& force, PhyForceType forceType) {
	for (auto t : rigids)
		t->addForce(force, forceType);
}


void PhyScene::addJointChain(pPhyJointChain joints) {
	auto rs = joints->getRigids();
	for (auto t : rs) {
		addRigid(t);
	}
}

END_P3D_NS