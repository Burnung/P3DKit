// Chongyang Ma - 2013-10-08
// Class declaration of hair sample
// --------------------------------------------------------------

#ifndef HAIRSAMPLE_H
#define HAIRSAMPLE_H

#include "vec.h"
#include <vector>
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//#include <GLUT/glut.h>
#include "HairConfig.h"

using namespace std;

class CHairSample
{
public:
    CHairSample();
    inline Vec3f GetPos() const { return m_pos; }
	inline void SetPos(Vec3f pos) { m_pos = pos; }
    
	inline void SetNorm(Vec3f norm) { m_norm = norm; }
	inline Vec3f GetNorm() { return m_norm; }

    void TranslateSample(const Vec3f& trans) { m_pos += trans; }
	void RotateSample(const Vec4f& rot, const Vec3f& posRef);
private:
	Vec3f m_pos;
	Vec3f m_norm;
    Vec4f m_quat; // Local coordinate frame
};

#endif // HAIRSAMPLE_H
