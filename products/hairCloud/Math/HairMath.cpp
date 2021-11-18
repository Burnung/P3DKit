// Chongyang Ma - 2013-10-08
// Implementation about various math routines
// --------------------------------------------------------------

#include "HairMath.h"

namespace hair_math
{
	Scalar QuatDifference(const Vec4f& q1, const Vec4f& q2)
	{
		Scalar dotProduct = dot(q1, q2);
		dotProduct = abs(dotProduct);
		// NOTE: the clamps below seem to be necessary due to round-off error
		dotProduct = min( 1.f, dotProduct);
		Scalar dist = (Scalar)acos(dotProduct);

		return dist;
	}

	Scalar QuatDifferenceFromZero(const Vec4f& q)
	{
		return QuatDifference(q, Vec4f(1.0f, 0.0f, 0.0f, 0.0f));
	}
    
    Vec4f QuatConjugate(const Vec4f& q0)
	{
		Vec4f q;
		q[0] = q0[0];
		q[1] = -q0[1];
		q[2] = -q0[2];
		q[3] = -q0[3];
        
		return q;
	}

	Vec4f QuatProduct(const Vec4f& q1, const Vec4f& q2)
	{
		Vec4f q;

		q[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
		q[1] = q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2];
		q[2] = q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1];
		q[3] = q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0];

		return q;
	}

	Vec3f QuatMultiplyVec(const Vec4f& q0, const Vec3f& v0)
	{
		// From Wiki (Quaternions and spatial rotation)
		Vec3f v;

		Scalar t2 =  q0[0] * q0[1];
		Scalar t3 =  q0[0] * q0[2];
		Scalar t4 =  q0[0] * q0[3];
		Scalar t5 = -q0[1] * q0[1];
		Scalar t6 =  q0[1] * q0[2];
		Scalar t7 =  q0[1] * q0[3];
		Scalar t8 = -q0[2] * q0[2];
		Scalar t9 =  q0[2] * q0[3];
		Scalar t10= -q0[3] * q0[3];

		v[0] = 2.f * ( (t8 + t10) * v0[0] + (t6 - t4 ) * v0[1] + (t3 + t7) * v0[2] ) + v0[0];
		v[1] = 2.f * ( (t4 + t6 ) * v0[0] + (t5 + t10) * v0[1] + (t9 - t2) * v0[2] ) + v0[1];
		v[2] = 2.f * ( (t7 - t3 ) * v0[0] + (t2 + t9 ) * v0[1] + (t5 + t8) * v0[2] ) + v0[2];

		return v;
	}

	void SetMatrixFromQuat(Scalar* ptrMat, Vec4f quat)
	{
		for ( int i=0; i<16; i++ )
		{
			ptrMat[i] = 0.f;
		}
		ptrMat[15] = 1.f;
		normalize(quat);
		Scalar s = quat[0];
		Scalar vx = quat[1];
		Scalar vy = quat[2];
		Scalar vz = quat[3];
		ptrMat[0] = 1.f - 2.f * vy * vy - 2.f * vz * vz;
		ptrMat[4] = 2.f * vx * vy - 2.f * s * vz;
		ptrMat[8] = 2.f * vx * vz + 2.f * s * vy;
		ptrMat[1] = 2.f * vx * vy + 2.f * s * vz;
		ptrMat[5] = 1.f - 2.f * vx * vx - 2.f * vz * vz;
		ptrMat[9] = 2.f * vy * vz - 2.f * s * vx;
		ptrMat[2] = 2.f * vx * vz - 2.f * s * vy;
		ptrMat[6] = 2.f * vy * vz + 2.f * s * vx;
		ptrMat[10] = 1.f - 2.f * vx * vx - 2.f * vy * vy;
	}

	void SetQuatFromMatrix(Scalar* ptrMat, Vec4f& quat)
	{
		Scalar tr, s;
		tr = ptrMat[0] + ptrMat[5] + ptrMat[10];

		if ( tr >= 0 )
		{
			s = sqrt(tr + Scalar(1));
			quat[0] = 0.5f * s;
			s = 0.5f / s;
			quat[1] = (ptrMat[6] - ptrMat[9]) * s;
			quat[2] = (ptrMat[8] - ptrMat[2]) * s;
			quat[3] = (ptrMat[1] - ptrMat[4]) * s;
		}
		else
		{
			int i = 0;
			if ( ptrMat[5] > ptrMat[0] )
			{
				i = 1;
			}
			if ( ptrMat[10] > ptrMat[5 * i] )
			{
				i = 2;
			}

			switch(i)
			{
			case 0:
				s = sqrt((ptrMat[0] - (ptrMat[5] + ptrMat[10])) + Scalar(1));
				quat[1] = 0.5f * s;
				s = 0.5f / s;
				quat[2] = (ptrMat[4] + ptrMat[1]) * s;
				quat[3] = (ptrMat[2] + ptrMat[8]) * s;
				quat[0] = (ptrMat[6] - ptrMat[9]) * s;
				break;
			case 1:
				s = sqrt((ptrMat[5] - (ptrMat[10] + ptrMat[0])) + Scalar(1));
				quat[2] = 0.5f * s;
				s = 0.5f / s;
				quat[3] = (ptrMat[9] + ptrMat[6]) * s;
				quat[1] = (ptrMat[4] + ptrMat[1]) * s;
				quat[0] = (ptrMat[8] - ptrMat[2]) * s;
				break;
			case 2:
				s = sqrt((ptrMat[10] - (ptrMat[0] + ptrMat[5])) + Scalar(1));
				quat[3] = 0.5f * s;
				s = 0.5f / s;
				quat[1] = (ptrMat[2] + ptrMat[8]) * s;
				quat[2] = (ptrMat[9] + ptrMat[6]) * s;
				quat[0] = (ptrMat[1] - ptrMat[4]) * s;
				break;
			}
		}
		normalize(quat);
	}
    
    void SetQuatFromMatrix3x3(Matrix3x3f& mat, Vec4f& quat)
    {
        float trace = mat[0][0] + mat[1][1] + mat[2][2];
        float u;
        if (trace>=0.0) {
            u = (float)sqrt(trace+1.0);
            quat[0] = 0.5f*u;
            u = 0.5f/u;
            quat[1] = (mat[2][1]-mat[1][2])*u;
            quat[2] = (mat[0][2]-mat[2][0])*u;
            quat[3] = (mat[1][0]-mat[0][1])*u;
        } else {
            int i = 0;
            if(mat[1][1] > mat[0][0])
                i = 1;
            
            if(mat[2][2] > mat[i][i])
                i = 2;
            
            switch (i)
            {
                case 0:
                    u = sqrt((mat[0][0] - (mat[1][1] + mat[2][2])) + 1.0f);
                    quat[1] = 0.5f * u;
                    u = 0.5f / u;
                    quat[2] = (mat[1][0] + mat[0][1]) * u;
                    quat[3] = (mat[0][2] + mat[2][0]) * u;
                    quat[0] = (mat[2][1] - mat[1][2]) * u;
                    break;
                    
                case 1:
                    u = sqrt((mat[1][1] - (mat[2][2] + mat[0][0])) + 1.0f);
                    quat[2] = 0.5f * u;
                    u = 0.5f / u;
                    quat[3] = (mat[2][1] + mat[1][2]) * u;
                    quat[1] = (mat[1][0] + mat[0][1]) * u;
                    quat[0] = (mat[0][2] - mat[2][0]) * u;
                    break;
                    
                case 2:
                    u = sqrt((mat[2][2] - (mat[0][0] + mat[1][1])) + 1.0f);
                    quat[3] = 0.5f * u;
                    u = 0.5f / u;
                    quat[1] = (mat[0][2] + mat[2][0]) * u;
                    quat[2] = (mat[2][1] + mat[1][2]) * u;
                    quat[0] = (mat[1][0] - mat[0][1]) * u;
                    break;
            }
        }
        normalize(quat);
    }
    
    void ComputeRotationMat(Vec3f vecBefore, Vec3f vecAfter, Matrix3x3f& mat)
    {
        Vec3f rotationAxis = cross(vecBefore, vecAfter);
        float rotationAngle = acosf(dot(vecBefore, vecAfter));
        normalize(rotationAxis);
        ComputeRotationMat(rotationAngle, rotationAxis, mat);
    }
    
    void ComputeRotationMat(float rotationAngle, Vec3f rotationAxis, Matrix3x3f& mat)
    {
        mat[0][0] = cos(rotationAngle) + rotationAxis[0] * rotationAxis[0] * (1 - cos(rotationAngle));
        mat[0][1] = rotationAxis[0] * rotationAxis[1] * (1 - cos(rotationAngle) - rotationAxis[2] * sin(rotationAngle));
        mat[0][2] = rotationAxis[1] * sin(rotationAngle) + rotationAxis[0] * rotationAxis[2] * (1 - cos(rotationAngle));
        
        mat[1][0] = rotationAxis[2] * sin(rotationAngle) + rotationAxis[0] * rotationAxis[1] * (1 - cos(rotationAngle));
        mat[1][1] = cos(rotationAngle) + rotationAxis[1] * rotationAxis[1] * (1 - cos(rotationAngle));
        mat[1][2] = -rotationAxis[0] * sin(rotationAngle) + rotationAxis[1] * rotationAxis[2] * (1 - cos(rotationAngle));
        
        mat[2][0] = -rotationAxis[1] * sin(rotationAngle) + rotationAxis[0] * rotationAxis[2] * (1 - cos(rotationAngle));
        mat[2][1] = rotationAxis[0] * sin(rotationAngle) + rotationAxis[1] * rotationAxis[2] * (1 - cos(rotationAngle));
        mat[2][2] = cos(rotationAngle) + rotationAxis[2] * rotationAxis[2] * (1 - cos(rotationAngle));
    }
    
    void SetAxisRotFromQuat(Vec3f& axisRot, const Vec4f& quat)
    {
        Vec4f quatNew = quat;
        if ( quatNew[0] < 0.f )
        {
            quatNew *= -1.f;
        }
        Scalar theta = (float)acos(quatNew[0]) * 2.f;
        if ( theta < 1e-10 ) // Numerical tolerance
        {
            axisRot = Vec3f(0.f, 0.f, 0.f);
        }
        else
        {
            Vec3f axis = Vec3f(quatNew[1], quatNew[2], quatNew[3]);
            normalize(axis);
            axisRot = axis * theta;
        }
    }
    
    void SetQuatFromAxisRot(const Vec3f& axisRot, Vec4f& quat)
    {
        // According to http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
        if ( mag2(axisRot) < 1e-10 ) // Numerical tolerance
        {
            quat = Vec4f(1.f, 0.f, 0.f, 0.f);
        }
        else
        {
            Scalar theta = mag(axisRot);
            Vec3f axis = axisRot * (1.f / theta);
            Scalar thetaHalf = theta * 0.5f;
            Scalar cv = cos(thetaHalf);
            Scalar sv = sin(thetaHalf);
            axis *= sv;
            quat = Vec4f(cv, axis[0], axis[1], axis[2]);
        }
    }
    
    bool InsideTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& p0)
    {
        Scalar a1, a2, a3;
        Scalar U[4], V[4];

        Vec3f norm = cross(v2 - v1, v3 - v1);
        a1 = abs(norm[0]);
        a2 = abs(norm[1]);
        a3 = abs(norm[2]);
        int MaxIndex = a1 > a2 ? (a1 > a3 ? 0 : 2) : (a2 > a3 ? 1 : 2);
        
        if ( MaxIndex == 0 )
        {
            U[0] = v1[1];
            U[1] = v2[1];
            U[2] = v3[1];
            U[3] = p0[1];
            V[0] = v1[2];
            V[1] = v2[2];
            V[2] = v3[2];
            V[3] = p0[2];
        }
        else if ( MaxIndex == 1 )
        {
            U[0] = v1[0];
            U[1] = v2[0];
            U[2] = v3[0];
            U[3] = p0[0];
            V[0] = v1[2];
            V[1] = v2[2];
            V[2] = v3[2];
            V[3] = p0[2];
        }
        else
        {
            U[0] = v1[1];
            U[1] = v2[1];
            U[2] = v3[1];
            U[3] = p0[1];
            V[0] = v1[0];
            V[1] = v2[0];
            V[2] = v3[0];
            V[3] = p0[0];
        }
        
        for ( int i=0; i<3; i++ )
        {
            U[i] -= U[3];
            V[i] -= V[3];
        }
        
        int NC = 0;
        int SH, NSH, j;
        SH = MySign(V[0]);
        for ( int i=0; i<3; i++ )
        {
            j = (i+1) % 3;
            NSH = MySign(V[j]);
            if ( SH != NSH )
            {
                if ( U[i] > 0 && U[j] > 0 ) // the line must cross +U'
                {
                    NC++;
                }
                else if ( U[i] > 0 || U[j] > 0 )
                {
                    if ( (U[i] - V[i] * (U[j] - U[i]) / (V[j] - V[i])) > 0 ) // the line must cross +U'
                    {
                        NC++;
                    }
                }
            }
            SH = NSH;
        }
        if ( NC % 2 == 1 )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    Scalar GetPointToTriangleDistance(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& p0, Vec3f& pNearest)
    {
        Vec3f e1 = v2 - v1;
        Vec3f e2 = v3 - v1;
        Vec3f n = cross(e1, e2);
        normalize(n);
        Scalar distToPlane = dot(n, (p0 - v1));
        Vec3f pc = p0 - distToPlane * n;
        distToPlane = abs(distToPlane);
        if ( InsideTriangle(v1, v2, v3, pc) == true )
        {
            pNearest = pc;
            return distToPlane;
        }
        else
        {
            Scalar distSqr1 = dist2(v1, p0);
            Scalar distSqr2 = dist2(v2, p0);
            Scalar distSqr3 = dist2(v3, p0);
            if ( distSqr1 < distSqr2 && distSqr1 < distSqr3 )
            {
                pNearest = v1;
                return sqrt(distSqr1);
            }
            else if ( distSqr2 < distSqr3 )
            {
                pNearest = v2;
                return sqrt(distSqr2);
            }
            else
            {
                pNearest = v3;
                return sqrt(distSqr3);
            }
        }
    }
    
    Scalar GetPointToTriangleDistance(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3,
                                      const Vec3f& n1, const Vec3f& n2, const Vec3f& n3,
                                      const Vec3f& p0, Vec3f& pNearest, Vec3f& nNearest)
    {
        Vec3f e1 = v2 - v1;
        Vec3f e2 = v3 - v1;
        Vec3f n = cross(e1, e2);
        normalize(n);
        Scalar distToPlane = dot(n, (p0 - v1));
        Vec3f pc = p0 - distToPlane * n;
        distToPlane = abs(distToPlane);
        if ( InsideTriangle(v1, v2, v3, pc) == true )
        {
            pNearest = pc;
            Scalar fd = dot(e1, e2);
            Scalar f1 = mag2(e1);
            Scalar f2 = mag2(e2);
            Scalar ft = 1.f / (f1 * f2 - fd * fd);
            Vec3f ubeta = f2 * e1 - fd * e2;
            ubeta = ft * ubeta;
            Vec3f ueta = f1 * e2 - fd * e1;
            ueta = ft * ueta;
            Vec3f vf = pc - v1;
            Scalar beta = dot(ubeta, vf);
            Scalar eta = dot(ueta, vf);
            Scalar alpa = 1.f - beta - eta;
            nNearest = alpa * n1 + beta * n2 + eta * n3;
            return distToPlane;
        }
        else
        {
            Scalar distSqr1 = dist2(v1, p0);
            Scalar distSqr2 = dist2(v2, p0);
            Scalar distSqr3 = dist2(v3, p0);
            if ( distSqr1 < distSqr2 && distSqr1 < distSqr3 )
            {
                pNearest = v1;
                nNearest = n1;
                return sqrt(distSqr1);
            }
            else if ( distSqr2 < distSqr3 )
            {
                pNearest = v2;
                nNearest = n2;
                return sqrt(distSqr2);
            }
            else
            {
                pNearest = v3;
                nNearest = n3;
                return sqrt(distSqr3);
            }
        }
    }
    
    void GetNearesetPointFromPointToTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& p0, Vec3f& pNearest)
    {
        GetPointToTriangleDistance(v1, v2, v3, p0, pNearest);
    }
    
    void GetNearesetPointFromPointToTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3,
                                             const Vec3f& n1, const Vec3f& n2, const Vec3f& n3,
                                             const Vec3f& p0, Vec3f& pNearest, Vec3f& nNearest)
    {
        GetPointToTriangleDistance(v1, v2, v3, n1, n2, n3, p0, pNearest, nNearest);
    }
    
    Vec3f Matrix3x3MultiplyVec(Matrix3x3f& mat, Vec3f& vec)
    {
        Vector3f vector(vec[0], vec[1], vec[2]);
        Vector3f result = mat*vector;
        return Vec3f(result.x, result.y, result.z);
    }
    
    Vec3f HSVtoRGB(Vec3f hsv)
    {
        // From FvD
        float H = hsv[0];
        float S = hsv[1];
        float V = hsv[2];
        if ( S <= 0.0f )
        {
            return Vec3f(V, V, V);
        }
        H = std::fmod(H, TWO_PI);
        if ( H < 0.0f )
        {
            H += TWO_PI;
        }
        H *= 6.0f / TWO_PI;
        int i = int(std::floor(H));
        float f = H - i;
        float p = V * (1.0f - S);
        float q = V * (1.0f - (S * f));
        float t = V * (1.0f - (S * (1.0f - f)));
        switch (i)
        {
            case 0:
                return Vec3f(V, t, p);
            case 1:
                return Vec3f(q, V, p);
            case 2:
                return Vec3f(p, V, t);
            case 3:
                return Vec3f(p, q, V);
            case 4:
                return Vec3f(t, p, V);
            default:
                return Vec3f(V, p, q);
        }
    }
    
    Vec3f ProjectPointOnPlane(Vec3f p, Vec3f planeNormal, float d)
    {
        float t = (dot(planeNormal, p) + d) / mag2(planeNormal);
        return p - t * planeNormal;
    }
    
    bool LineIntersectWithPlane(Vec3f p0, Vec3f p1, Vec3f planeNormal, float d, Vec3f& p)
    {
        float flag0 = dot(planeNormal, p0) + d;
        float flag1 = dot(planeNormal, p1) + d;
        if (flag0*flag1>0.0f) {
            return false;
        }
        float t0 = flag0 / mag2(planeNormal);
        float t1 = flag1 / mag2(planeNormal);
        p = p0+(p1-p0)*(float)fabs(t0)/(float)(fabs(t0)+fabs(t1));
        return true;
    }
    
    float RayIntersectionWithTriangle(Vec3f start, Vec3f orientation, Vec3f v0, Vec3f v1, Vec3f v2)
    {
        Vec3f e1 = v1-v0;
        Vec3f e2 = v2-v0;
        Vec3f q = cross(orientation, e2);
        float tmp = dot(q, e1);
        if (fabsf(tmp)<1e-10)
            return -1000.0f;
        tmp = 1.0f/tmp;
        Vec3f s = start-v0;
        float u = tmp*dot(s, q);
        if (u<0.0f || u>1.0f)
            return -1000.0f;
        q = cross(s, e1);
        float vv = tmp*dot(orientation, q);
        if (vv<0.0 || vv+u>1.0)
            return -1000.0f;
        float dist = tmp*dot(e2, q);
        return dist;
    }
    
    float RayIntersectionWithPlane(Vec3f start, Vec3f orientation, Vec3f v0, Vec3f v1, Vec3f v2)
    {
        Vec3f e1 = v1-v0;
        Vec3f e2 = v2-v0;
        Vec3f q = cross(orientation, e2);
        float tmp = dot(q, e1);
        if (fabsf(tmp)<1e-10)
            return -1000.0f;
        tmp = 1.0f/tmp;
        Vec3f s = start-v0;
        float u = tmp*dot(s, q);
        q = cross(s, e1);
        float vv = tmp*dot(orientation, q);
        float dist = tmp*dot(e2, q);
        return dist;
    }
    
    float Area(const Vec3f v1, const Vec3f v2, const Vec3f v3)
    {
        return mag(cross(v1-v2,v1-v3));
    }
    
    bool GetBarycentricCoordinates(const Vec3f p, const Vec3f v0, const Vec3f v1, const Vec3f v2, float& a0, float &a1, float &a2)
    {
        float a = Area(v0, v1, v2);
        a0 = Area(p, v1, v2)/a;
        a1 = Area(p, v0, v2)/a;
        a2 = 1.0f-a0-a1;
        if (a0>=0.0f && a1>=0.0f && a2>0.0f) {
            return true;
        } else {
            return false;
        }
    }
    
    float ProjPointOnLine(Vec3f pos, Vec3f v0, Vec3f v1)
    {
        float v = ((v1[0]-v0[0])*(pos[0]-v0[0])+(v1[1]-v0[1])*(pos[1]-v0[1])+(v1[2]-v0[2])*(pos[2]-v0[2]))/
        ((v1[0]-v0[0])*(v1[0]-v0[0])+(v1[1]-v0[1])*(v1[1]-v0[1])+(v1[2]-v0[2])*(v1[2]-v0[2]));
        return v;
    }
}
