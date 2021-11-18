#include "PxFoundation.h"
#include "pvd/PxPvd.h"
#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "common/mathMatrix.hpp"
#include "foundation/PxMat33.h"
#include "foundation/PxMat44.h"
#include "foundation/PxTransform.h"
#include "foundation/PxVec3.h"

#pragma once

#define PX_RELEASE(x)	if(x)	{ x->release(); x = nullptr;	}

BEGIN_P3D_NS

enum PhyGeoType {
	PHY_GEOTYPE_INVALID = -1,
	PHY_GEOTYPE_PLANE,
	PHY_GEOTYPE_SPHERE,
	PHY_GEOTYPE_BOX,
	PHY_GEOTYPE_CAPSULE,
	PHY_GEOTYPE_MESH,
	PHY_GEOTYPE_NUM,
};

enum PhyRigidType {
	PHY_RIGIDTYPE_INVALID = -1,
	PHY_RIGIDTYPE_STATIC,
	PHY_RIGIDTYPE_DYNAMIC,
	PHY_RIGIDTYPE_NUM,
};

enum PhyForceType{
	PHY_FORCETYPE_FORCE = physx::PxForceMode::eFORCE,
	PHY_FORCETYPE_IMPUSE,
	PHY_FORCETYPE_CHANGE,
	PHY_FORCETYPE_ACCELERATION,
};

enum PhyJointType{
	PHY_JOINTTYPE_SPHERE,
	PHY_JOINTTYPE_FIXED,
	PHY_JOINTTYPE_REVOLUTE,
	PHY_JOINTTYPE_DISTANCE,
	PHY_JOINTTYPE_PRISMATIC,
};

struct PhyMaterial{
	physx::PxMaterial* mat = nullptr;
};

typedef std::shared_ptr<PhyMaterial> pPhyMaterial;

physx::PxTransform convertMat42PhyxTransform(const Mat4f& mat);
Mat4f convertTransform2Mat4(const physx::PxTransform& trans);

END_P3D_NS
