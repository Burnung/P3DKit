#include "phyCommon.h"


BEGIN_P3D_NS


physx::PxTransform convertMat42PhyxTransform(const Mat4f& mat) {
	physx::PxTransform ret;
	ret.p = physx::PxVec3(mat[0][3], mat[1][3], mat[2][3]);

	physx::PxVec3 col1(mat[0][0], mat[1][0], mat[2][0]);
	physx::PxVec3 col2(mat[0][1], mat[1][1], mat[2][1]);
	physx::PxVec3 col3(mat[0][2], mat[1][2], mat[2][2]);
	ret.q = physx::PxQuat(physx::PxMat33(col1, col2, col3));
	return ret;
}

Mat4f convertTransform2Mat4(const physx::PxTransform& trans) {
	physx::PxMat33 rot(trans.q);
	physx::PxMat44 tMat(trans);
	Mat4f ret = Mat4f::getIdentity();

	for (int i = 0; i < 3; ++i) {
		ret[i] = Vec4f(tMat.column0[i],tMat.column1[i],tMat.column2[i],tMat.column3[i]);
		//ret[{(uint32)i,3}] = trans.p[i];
	}
	return ret;
}



END_P3D_NS