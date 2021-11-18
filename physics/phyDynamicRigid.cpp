#include "phyDynamicRigid.h"
#include <memory>

BEGIN_P3D_NS
using namespace physx;

PhyDynamicRigid::PhyDynamicRigid(PxPhysics* phy, PhyGeoType geoType, const PhyMaterial& material, const Mat4f& modelMat, const std::vector<float>& externData,float32 density) {
	rigidType = PHY_RIGIDTYPE_DYNAMIC;
	geoType = geoType;
	this->modelMat = modelMat;
	this->srcMat = modelMat;
	inverseMat = modelMat.simpleInvers();
	//auto testMat = inverseMat * modelMat;
	auto trans = convertMat42PhyxTransform(modelMat);
	switch (geoType)
	{
	case PHY_GEOTYPE_BOX:
	{
		auto geo_ = PxBoxGeometry(externData[0], externData[1], externData[2]);
		pxRigid = (PxRigidBody*)PxCreateDynamic(*phy, trans, geo_, *(material.mat),density);;
		break;
	}
	case PHY_GEOTYPE_SPHERE:
	{
		auto geo_ = PxSphereGeometry(externData[0]);
		pxRigid = (PxRigidBody*)PxCreateDynamic(*phy, trans, geo_, *(material.mat),density);
		break;
	}
	case PHY_GEOTYPE_CAPSULE:
	{
		auto geo_ = PxCapsuleGeometry(externData[0], externData[1]);
		pxRigid = (PxRigidBody*)PxCreateDynamic(*phy, trans, geo_, *(material.mat),density);
		break;
	}
	default:
		P3D_THROW_ARGUMENT("PhyDynamicRight:: invalid GEO_TYPE");
		break;
	}
}

Mat4f PhyDynamicRigid::updatePostion() {
	auto trans = pxRigid->getGlobalPose();
	auto tMat = convertTransform2Mat4(trans);
	auto tp = tMat * Vec3f(0.0, 0.0, 0.0);
	modelMat = Mat4f::getTranslate(tp);
	
	//pxRigid->getCMassLocalPose(
	relativeMat = modelMat * inverseMat;
	relativePos = Vec3f(relativeMat[0].w, relativeMat[1].w, relativeMat[2].w);
	return modelMat;
}

void PhyDynamicRigid::addForce(const Vec3f& force, PhyForceType fType){
	pxRigid->addForce({ force[0],force[1],force[2] }, static_cast<PxForceMode::Enum>(fType));

}
END_P3D_NS
