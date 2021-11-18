// Chongyang Ma - 2013-10-08
// Class implementation of hair sample
// --------------------------------------------------------------

#include "HairSample.h"
#include "HairMath.h"

CHairSample::CHairSample()
{
	m_pos = Vec3f(0.0f, 0.0f, 0.0f);
	m_norm = Vec3f(1.0f, 0.0f, 0.0f);
}

void CHairSample::RotateSample(const Vec4f& rot, const Vec3f& posRef)
{
    Vec3f prOld = m_pos - posRef;
	Vec3f prNew = hair_math::QuatMultiplyVec(rot, prOld);
	m_pos = posRef + prNew;
	m_norm = hair_math::QuatMultiplyVec(rot, m_norm);
	m_quat = hair_math::QuatProduct(rot, m_quat);
}