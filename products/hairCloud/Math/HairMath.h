// Chongyang Ma - 2013-10-08
// Declaration about various math routines
// --------------------------------------------------------------

#ifndef HAIRMATH_H
#define HAIRMATH_H

#include "main.h"
#include "vec.h"
#include "Matrix3x3f.h"

#define MySign(A) ( (A) >= 0 ? 1 : -1 )

namespace hair_math
{
	Scalar QuatDifference(const Vec4f& q1, const Vec4f& q2);

	Scalar QuatDifferenceFromZero(const Vec4f& q);
    
    Vec4f QuatConjugate(const Vec4f& q0);

	Vec4f QuatProduct(const Vec4f& q1, const Vec4f& q2);

	Vec3f QuatMultiplyVec(const Vec4f& q0, const Vec3f& v0);

	void SetMatrixFromQuat(Scalar* ptrMat, Vec4f quat);

	void SetQuatFromMatrix(Scalar* ptrMat, Vec4f& quat);
    
    void SetQuatFromMatrix3x3(Matrix3x3f& mat, Vec4f& quat);
    
    void SetAxisRotFromQuat(Vec3f& axisRot, const Vec4f& quat);
    
    void SetQuatFromAxisRot(const Vec3f& axisRot, Vec4f& quat);
    
    void ComputeRotationMat(Vec3f vecBefore, Vec3f vecAfter, Matrix3x3f& mat);
    
    void ComputeRotationMat(float rotationAngle, Vec3f rotationAxis, Matrix3x3f& mat);
    
    bool InsideTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& p0);
    
    Scalar GetPointToTriangleDistance(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& p0, Vec3f& pNearest);
    
    Scalar GetPointToTriangleDistance(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3,
                                      const Vec3f& n1, const Vec3f& n2, const Vec3f& n3,
                                      const Vec3f& p0, Vec3f& pNearest, Vec3f& nNearest);
    
    void GetNearesetPointFromPointToTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& p0, Vec3f& pNearest);
    
    void GetNearesetPointFromPointToTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3,
                                             const Vec3f& n1, const Vec3f& n2, const Vec3f& n3,
                                             const Vec3f& p0, Vec3f& pNearest, Vec3f& nNearest);
    
    Vec3f Matrix3x3MultiplyVec(Matrix3x3f& mat, Vec3f& vec);
    
    Vec3f HSVtoRGB(Vec3f hsv);
    
    Vec3f ProjectPointOnPlane(Vec3f p, Vec3f planeNormal, float d);
    
    Vec3f PointDisttoLine(Vec3f start, Vec3f end, Vec3f p);
    
    bool LineIntersectWithPlane(Vec3f p0, Vec3f p1, Vec3f planeNormal, float d, Vec3f& p);
    
    float RayIntersectionWithTriangle(Vec3f start, Vec3f orientation, Vec3f v0, Vec3f v1, Vec3f v2);
    
    float RayIntersectionWithPlane(Vec3f start, Vec3f orientation, Vec3f v0, Vec3f v1, Vec3f v2);
    
    float Area(const Vec3f v1, const Vec3f v2, const Vec3f v3);
    
    bool GetBarycentricCoordinates(const Vec3f p, const Vec3f v0, const Vec3f v1, const Vec3f v2, float& a0, float &a1, float &a2);
    
    float ProjPointOnLine(Vec3f pos, Vec3f v0, Vec3f v1);
}

#endif // HAIRMATH_H
