#include "phyStaticRigid.h"
#include "phyCommon.h"
using namespace physx;
BEGIN_P3D_NS

PhyStaticRigid::PhyStaticRigid(PxPhysics* phy, PhyGeoType geoType,const PhyMaterial& material, const Mat4f& modelMat ,const std::vector<float>& externData) {
	rigidType = PHY_RIGIDTYPE_STATIC;
	geoType = geoType;
	this->modelMat = modelMat;
	this->srcMat = modelMat;
	auto trans = convertMat42PhyxTransform(modelMat);
	switch (geoType)
	{
	case PHY_GEOTYPE_BOX:
	{
		auto geo = PxBoxGeometry(externData[0], externData[1], externData[2]);
		pxRigid =  (PxRigidBody*)PxCreateStatic(*phy, trans,geo,*(material.mat));;
		break;
	}
	case PHY_GEOTYPE_SPHERE:
	{
		auto geo = PxSphereGeometry(externData[0]);
		pxRigid =  (PxRigidBody*)PxCreateStatic(*phy, trans,geo,*(material.mat));;
		break;
	}
	case PHY_GEOTYPE_PLANE:
	{
		auto geo = PxPlane(externData[0], externData[1], externData[2], externData[3]);
		pxRigid = (PxRigidBody*)PxCreatePlane(*phy, geo,*(material.mat));
		break;
	}
	case PHY_GEOTYPE_CAPSULE:
	{
		auto geo = PxCapsuleGeometry(externData[0], externData[1]);
		pxRigid = (PxRigidBody*)PxCreateStatic(*phy, trans, geo, *(material.mat));
		break;
	}
	case PHY_GEOTYPE_MESH:
	{
		//auto geo = PxTriangleMesh();
		break;
	}
	default:
		P3D_THROW_ARGUMENT("PhyStaticRight:: invalid GEO_TYPE");
		break;
	}
}



END_P3D_NS