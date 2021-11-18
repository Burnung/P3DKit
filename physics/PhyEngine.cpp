#include "PhyEngine.h"
#include "PhyStaticRigid.h"
#include "phyDynamicRigid.h"
BEGIN_P3D_NS

using namespace physx;

PhyEngine::~PhyEngine() {
	release();
}
void PhyEngine::init() {
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocator, pxErrorCallback);
	pxPvd = PxCreatePvd(*pxFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), true, pxPvd);
	PxInitExtensions(*pxPhysics, pxPvd);

}


PhyMaterial PhyEngine::createMaterial(float32 staticFriction, float32 dynamicFriction, float32 restitution) {
	auto *mat = pxPhysics->createMaterial(staticFriction,dynamicFriction,restitution);
	PhyMaterial ret;
	ret.mat = mat;
	return ret;
}

pPhyScene PhyEngine::createPhyScene(uint32 numWorks,Vec3f gravity) {
	auto ret = std::make_shared<PhyScene>(pxPhysics,numWorks,gravity);
	allScenes.push_back(ret);
	return ret;
}

void PhyEngine::release() {
	for (auto& t : allScenes)
		t->release();

	PxCloseExtensions();
	PX_RELEASE(pxPhysics);
	if (pxPvd)
	{
		PxPvdTransport* transport = pxPvd->getTransport();
		PX_RELEASE(pxPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(pxFoundation);
}

pPhyRigid PhyEngine::createRigid(PhyRigidType rigidType, PhyGeoType geoType,const PhyMaterial& material, const Mat4f& modelMat, const std::vector<float> &externData,float32 density) {
	pPhyRigid ret = nullptr;
	switch (rigidType)
	{
	case P3D::PHY_RIGIDTYPE_STATIC:
		ret = std::make_shared<PhyStaticRigid>(pxPhysics,geoType, material, modelMat, externData);
		break;
	case P3D::PHY_RIGIDTYPE_DYNAMIC:
		ret = std::make_shared<PhyDynamicRigid>(pxPhysics, geoType, material, modelMat, externData, density);
		break;
	}
	return ret;
}



pPhyJointChain PhyEngine::createJointChain(PhyJointType jointType,const Vec3f& defDelta) {
	auto ret = std::make_shared<PhyJointChain>(pxPhysics, jointType,defDelta);
	return ret;
}

END_P3D_NS