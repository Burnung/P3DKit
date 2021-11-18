#pragma once
#include "common/P3D_Utils.h"
#include "common/mathIncludes.h"
#include "phyCommon.h"
#include "PhyScene.h"
#include "PhyRigid.h"
BEGIN_P3D_NS

class PhyEngine :public SingletonBase<PhyEngine>, P3DObject {
	P3D_SINGLETON_DECLEAR(PhyEngine)
public:
	virtual ~PhyEngine();
public:
	void init();
	void release();
	inline	physx::PxPhysics* getPxPhysics() { return pxPhysics; }

public:
	pPhyScene createPhyScene(uint32 numWorks, Vec3f gravity = {0.0,-9.8,0.0});
	pPhyRigid createRigid(PhyRigidType rigidType, PhyGeoType geoType, const PhyMaterial& material,const Mat4f& modelMat, const std::vector<float>& externData,float32 density=1.0);
	pPhyJointChain createJointChain(PhyJointType jointType,const Vec3f& defDelta);
	
	PhyMaterial createMaterial(float32 staticFraiction ,float32 dynamicFraction,float32 retitution);

protected:
	physx::PxPhysics* pxPhysics = nullptr;
	physx::PxFoundation* pxFoundation = nullptr;
	physx::PxPvd* pxPvd = nullptr;
	physx::PxPvdTransport* pxPvdTrannport = nullptr;

	physx::PxDefaultAllocator		pxAllocator;
	physx::PxDefaultErrorCallback	pxErrorCallback;
	std::vector<pPhyScene> allScenes;
};




END_P3D_NS
