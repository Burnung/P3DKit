/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/
#include "Matrix3x3f.h"

// Adapted from Matrix math by Wild Magic http://www.geometrictools.com/

const float Matrix3x3f::EPSILON = 1e-06f;
const Matrix3x3f Matrix3x3f::ZERO(0,0,0,0,0,0,0,0,0);
const Matrix3x3f Matrix3x3f::IDENTITY(1,0,0,0,1,0,0,0,1);
const float Matrix3x3f::ms_fSvdEpsilon = 1e-04f;
const unsigned int Matrix3x3f::ms_iSvdMaxIterations = 32;

//-----------------------------------------------------------------------
Vector3f Matrix3x3f::GetColumn (size_t iCol) const
{
    assert( 0 <= iCol && iCol < 3 );
    return Vector3f(m[0][iCol],m[1][iCol],
        m[2][iCol]);
}
//-----------------------------------------------------------------------
void Matrix3x3f::SetColumn(size_t iCol, const Vector3f& vec)
{
    assert( 0 <= iCol && iCol < 3 );
    m[0][iCol] = vec.x;
    m[1][iCol] = vec.y;
    m[2][iCol] = vec.z;
}
//-----------------------------------------------------------------------
void Matrix3x3f::SetColumns(const Vector3f& x, const Vector3f& y, const Vector3f& z)
{
    m[0][0] = x.x;
    m[1][0] = x.y;
    m[2][0] = x.z;
    m[0][1] = y.x;
    m[1][1] = y.y;
    m[2][1] = y.z;
    m[0][2] = z.x;
    m[1][2] = z.y;
    m[2][2] = z.z;
}
//-----------------------------------------------------------------------
void Matrix3x3f::FromAxes(const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& zAxis)
{
    SetColumn(0,xAxis);
    SetColumn(1,yAxis);
    SetColumn(2,zAxis);
}

//-----------------------------------------------------------------------
bool Matrix3x3f::operator== (const Matrix3x3f& rkMatrix) const
{
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            if ( m[iRow][iCol] != rkMatrix.m[iRow][iCol] )
                return false;
        }
    }

    return true;
}
//-----------------------------------------------------------------------
Matrix3x3f Matrix3x3f::operator+ (const Matrix3x3f& rkMatrix) const
{
    Matrix3x3f kSum;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            kSum.m[iRow][iCol] = m[iRow][iCol] +
                rkMatrix.m[iRow][iCol];
        }
    }
    return kSum;
}

//-----------------------------------------------------------------------
Matrix3x3f &Matrix3x3f::operator+= (const Matrix3x3f& rkMatrix)
{
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            m[iRow][iCol] += rkMatrix.m[iRow][iCol];
        }
    }
    return *this;
}
//-----------------------------------------------------------------------
Matrix3x3f Matrix3x3f::operator- (const Matrix3x3f& rkMatrix) const
{
    Matrix3x3f kDiff;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            kDiff.m[iRow][iCol] = m[iRow][iCol] -
                rkMatrix.m[iRow][iCol];
        }
    }
    return kDiff;
}
//-----------------------------------------------------------------------
Matrix3x3f Matrix3x3f::operator* (const Matrix3x3f& rkMatrix) const
{
    Matrix3x3f kProd;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            kProd.m[iRow][iCol] =
                m[iRow][0]*rkMatrix.m[0][iCol] +
                m[iRow][1]*rkMatrix.m[1][iCol] +
                m[iRow][2]*rkMatrix.m[2][iCol];
        }
    }
    return kProd;
}
//-----------------------------------------------------------------------
Vector3f Matrix3x3f::operator* (const Vector3f& rkPoint) const
{
    Vector3f kProd;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] =
            m[iRow][0]*rkPoint[0] +
            m[iRow][1]*rkPoint[1] +
            m[iRow][2]*rkPoint[2];
    }
    return kProd;
}
//-----------------------------------------------------------------------
Vector3f operator* (const Vector3f& rkPoint, const Matrix3x3f& rkMatrix)
{
    Vector3f kProd;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] =
            rkPoint[0]*rkMatrix.m[0][iRow] +
            rkPoint[1]*rkMatrix.m[1][iRow] +
            rkPoint[2]*rkMatrix.m[2][iRow];
    }
    return kProd;
}
//-----------------------------------------------------------------------
Matrix3x3f Matrix3x3f::operator- () const
{
    Matrix3x3f kNeg;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
            kNeg[iRow][iCol] = -m[iRow][iCol];
    }
    return kNeg;
}
//-----------------------------------------------------------------------
Matrix3x3f Matrix3x3f::operator* (float fScalar) const
{
    Matrix3x3f kProd;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
            kProd[iRow][iCol] = fScalar*m[iRow][iCol];
    }
    return kProd;
}
//-----------------------------------------------------------------------
void Matrix3x3f::operator*= (float fScalar)
{
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
            m[iRow][iCol] *= fScalar;
    }
}
//-----------------------------------------------------------------------
Matrix3x3f operator* (float fScalar, const Matrix3x3f& rkMatrix)
{
    Matrix3x3f kProd;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
            kProd[iRow][iCol] = fScalar*rkMatrix.m[iRow][iCol];
    }
    return kProd;
}
//-----------------------------------------------------------------------
Matrix3x3f Matrix3x3f::Transpose () const
{
    Matrix3x3f kTranspose;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
            kTranspose[iRow][iCol] = m[iCol][iRow];
    }
    return kTranspose;
}
//-----------------------------------------------------------------------
bool Matrix3x3f::Inverse (Matrix3x3f& rkInverse, float fTolerance) const
{
    // Invert a 3x3 using cofactors.  This is about 8 times faster than
    // the Numerical Recipes code which uses Gaussian elimination.

    rkInverse[0][0] = m[1][1]*m[2][2] -
        m[1][2]*m[2][1];
    rkInverse[0][1] = m[0][2]*m[2][1] -
        m[0][1]*m[2][2];
    rkInverse[0][2] = m[0][1]*m[1][2] -
        m[0][2]*m[1][1];
    rkInverse[1][0] = m[1][2]*m[2][0] -
        m[1][0]*m[2][2];
    rkInverse[1][1] = m[0][0]*m[2][2] -
        m[0][2]*m[2][0];
    rkInverse[1][2] = m[0][2]*m[1][0] -
        m[0][0]*m[1][2];
    rkInverse[2][0] = m[1][0]*m[2][1] -
        m[1][1]*m[2][0];
    rkInverse[2][1] = m[0][1]*m[2][0] -
        m[0][0]*m[2][1];
    rkInverse[2][2] = m[0][0]*m[1][1] -
        m[0][1]*m[1][0];

    float fDet =
        m[0][0]*rkInverse[0][0] +
        m[0][1]*rkInverse[1][0]+
        m[0][2]*rkInverse[2][0];

    if ( fabs(fDet) <= fTolerance )
        return false;

    float fInvDet = 1.0f/fDet;
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
            rkInverse[iRow][iCol] *= fInvDet;
    }

    return true;
}
//-----------------------------------------------------------------------
Matrix3x3f Matrix3x3f::Inverse (float fTolerance) const
{
    Matrix3x3f kInverse = Matrix3x3f::ZERO;
    Inverse(kInverse,fTolerance);
    return kInverse;
}
//-----------------------------------------------------------------------
float Matrix3x3f::Determinant () const
{
    float fCofactor00 = m[1][1]*m[2][2] -
        m[1][2]*m[2][1];
    float fCofactor10 = m[1][2]*m[2][0] -
        m[1][0]*m[2][2];
    float fCofactor20 = m[1][0]*m[2][1] -
        m[1][1]*m[2][0];

    float fDet =
        m[0][0]*fCofactor00 +
        m[0][1]*fCofactor10 +
        m[0][2]*fCofactor20;

    return fDet;
}
//-----------------------------------------------------------------------
void Matrix3x3f::Bidiagonalize (Matrix3x3f& kA, Matrix3x3f& kL,
    Matrix3x3f& kR)
{
    float afV[3], afW[3];
    float fLength, fSign, fT1, fInvT1, fT2;
    bool bIdentity;

    // map first column to (*,0,0)
    fLength = (float)sqrt(kA[0][0]*kA[0][0] + kA[1][0]*kA[1][0] +
        kA[2][0]*kA[2][0]);
    if ( fLength > 0.0 )
    {
        fSign = (kA[0][0] > 0.0f ? 1.0f : -1.0f);
        fT1 = kA[0][0] + fSign*fLength;
        fInvT1 = 1.0f/fT1;
        afV[1] = kA[1][0]*fInvT1;
        afV[2] = kA[2][0]*fInvT1;

        fT2 = -2.0f/(1.0f+afV[1]*afV[1]+afV[2]*afV[2]);
        afW[0] = fT2*(kA[0][0]+kA[1][0]*afV[1]+kA[2][0]*afV[2]);
        afW[1] = fT2*(kA[0][1]+kA[1][1]*afV[1]+kA[2][1]*afV[2]);
        afW[2] = fT2*(kA[0][2]+kA[1][2]*afV[1]+kA[2][2]*afV[2]);
        kA[0][0] += afW[0];
        kA[0][1] += afW[1];
        kA[0][2] += afW[2];
        kA[1][1] += afV[1]*afW[1];
        kA[1][2] += afV[1]*afW[2];
        kA[2][1] += afV[2]*afW[1];
        kA[2][2] += afV[2]*afW[2];

        kL[0][0] = 1.0f+fT2;
        kL[0][1] = kL[1][0] = fT2*afV[1];
        kL[0][2] = kL[2][0] = fT2*afV[2];
        kL[1][1] = 1.0f+fT2*afV[1]*afV[1];
        kL[1][2] = kL[2][1] = fT2*afV[1]*afV[2];
        kL[2][2] = 1.0f+fT2*afV[2]*afV[2];
        bIdentity = false;
    }
    else
    {
        kL = Matrix3x3f::IDENTITY;
        bIdentity = true;
    }

    // map first row to (*,*,0)
    fLength = (float)sqrt(kA[0][1]*kA[0][1]+kA[0][2]*kA[0][2]);
    if ( fLength > 0.0 )
    {
        fSign = (kA[0][1] > 0.0f ? 1.0f : -1.0f);
        fT1 = kA[0][1] + fSign*fLength;
        afV[2] = kA[0][2]/fT1;

        fT2 = -2.0f/(1.0f+afV[2]*afV[2]);
        afW[0] = fT2*(kA[0][1]+kA[0][2]*afV[2]);
        afW[1] = fT2*(kA[1][1]+kA[1][2]*afV[2]);
        afW[2] = fT2*(kA[2][1]+kA[2][2]*afV[2]);
        kA[0][1] += afW[0];
        kA[1][1] += afW[1];
        kA[1][2] += afW[1]*afV[2];
        kA[2][1] += afW[2];
        kA[2][2] += afW[2]*afV[2];

        kR[0][0] = 1.0f;
        kR[0][1] = kR[1][0] = 0.0f;
        kR[0][2] = kR[2][0] = 0.0f;
        kR[1][1] = 1.0f+fT2;
        kR[1][2] = kR[2][1] = fT2*afV[2];
        kR[2][2] = 1.0f+fT2*afV[2]*afV[2];
    }
    else
    {
        kR = Matrix3x3f::IDENTITY;
    }

    // map second column to (*,*,0)
    fLength = (float)sqrt(kA[1][1]*kA[1][1]+kA[2][1]*kA[2][1]);
    if ( fLength > 0.0f )
    {
        fSign = (kA[1][1] > 0.0f ? 1.0f : -1.0f);
        fT1 = kA[1][1] + fSign*fLength;
        afV[2] = kA[2][1]/fT1;

        fT2 = -2.0f/(1.0f+afV[2]*afV[2]);
        afW[1] = fT2*(kA[1][1]+kA[2][1]*afV[2]);
        afW[2] = fT2*(kA[1][2]+kA[2][2]*afV[2]);
        kA[1][1] += afW[1];
        kA[1][2] += afW[2];
        kA[2][2] += afV[2]*afW[2];

        float fA = 1.0f+fT2;
        float fB = fT2*afV[2];
        float fC = 1.0f+fB*afV[2];

        if ( bIdentity )
        {
            kL[0][0] = 1.0f;
            kL[0][1] = kL[1][0] = 0.0f;
            kL[0][2] = kL[2][0] = 0.0f;
            kL[1][1] = fA;
            kL[1][2] = kL[2][1] = fB;
            kL[2][2] = fC;
        }
        else
        {
            for (int iRow = 0; iRow < 3; iRow++)
            {
                float fTmp0 = kL[iRow][1];
                float fTmp1 = kL[iRow][2];
                kL[iRow][1] = fA*fTmp0+fB*fTmp1;
                kL[iRow][2] = fB*fTmp0+fC*fTmp1;
            }
        }
    }
}
//-----------------------------------------------------------------------
void Matrix3x3f::GolubKahanStep (Matrix3x3f& kA, Matrix3x3f& kL,
    Matrix3x3f& kR)
{
    float fT11 = kA[0][1]*kA[0][1]+kA[1][1]*kA[1][1];
    float fT22 = kA[1][2]*kA[1][2]+kA[2][2]*kA[2][2];
    float fT12 = kA[1][1]*kA[1][2];
    float fTrace = fT11+fT22;
    float fDiff = fT11-fT22;
    float fDiscr = (float)sqrt(fDiff*fDiff+4.0f*fT12*fT12);
    float fRoot1 = 0.5f*(fTrace+fDiscr);
    float fRoot2 = 0.5f*(fTrace-fDiscr);

    // adjust right
    float fY = kA[0][0] - (fabs(fRoot1-fT22) <=
        fabs(fRoot2-fT22) ? fRoot1 : fRoot2);
    float fZ = kA[0][1];
	float fInvLength = 1.0f/(float)sqrt(fY*fY+fZ*fZ);
    float fSin = fZ*fInvLength;
    float fCos = -fY*fInvLength;

    float fTmp0 = kA[0][0];
    float fTmp1 = kA[0][1];
    kA[0][0] = fCos*fTmp0-fSin*fTmp1;
    kA[0][1] = fSin*fTmp0+fCos*fTmp1;
    kA[1][0] = -fSin*kA[1][1];
    kA[1][1] *= fCos;

    size_t iRow;
    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = kR[0][iRow];
        fTmp1 = kR[1][iRow];
        kR[0][iRow] = fCos*fTmp0-fSin*fTmp1;
        kR[1][iRow] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust left
    fY = kA[0][0];
    fZ = kA[1][0];
    fInvLength = 1.0f/(float)sqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    kA[0][0] = fCos*kA[0][0]-fSin*kA[1][0];
    fTmp0 = kA[0][1];
    fTmp1 = kA[1][1];
    kA[0][1] = fCos*fTmp0-fSin*fTmp1;
    kA[1][1] = fSin*fTmp0+fCos*fTmp1;
    kA[0][2] = -fSin*kA[1][2];
    kA[1][2] *= fCos;

    size_t iCol;
    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = kL[iCol][0];
        fTmp1 = kL[iCol][1];
        kL[iCol][0] = fCos*fTmp0-fSin*fTmp1;
        kL[iCol][1] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust right
    fY = kA[0][1];
    fZ = kA[0][2];
    fInvLength = 1.0f/(float)sqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    kA[0][1] = fCos*kA[0][1]-fSin*kA[0][2];
    fTmp0 = kA[1][1];
    fTmp1 = kA[1][2];
    kA[1][1] = fCos*fTmp0-fSin*fTmp1;
    kA[1][2] = fSin*fTmp0+fCos*fTmp1;
    kA[2][1] = -fSin*kA[2][2];
    kA[2][2] *= fCos;

    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = kR[1][iRow];
        fTmp1 = kR[2][iRow];
        kR[1][iRow] = fCos*fTmp0-fSin*fTmp1;
        kR[2][iRow] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust left
    fY = kA[1][1];
    fZ = kA[2][1];
    fInvLength = 1.0f/(float)sqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    kA[1][1] = fCos*kA[1][1]-fSin*kA[2][1];
    fTmp0 = kA[1][2];
    fTmp1 = kA[2][2];
    kA[1][2] = fCos*fTmp0-fSin*fTmp1;
    kA[2][2] = fSin*fTmp0+fCos*fTmp1;

    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = kL[iCol][1];
        fTmp1 = kL[iCol][2];
        kL[iCol][1] = fCos*fTmp0-fSin*fTmp1;
        kL[iCol][2] = fSin*fTmp0+fCos*fTmp1;
    }
}
//-----------------------------------------------------------------------
void Matrix3x3f::SingularValueDecomposition (Matrix3x3f& kL, Vector3f& kS,
    Matrix3x3f& kR) const
{
    // temas: currently unused
    //const int iMax = 16;
	size_t iRow, iCol;

    Matrix3x3f kA = *this;
    Bidiagonalize(kA,kL,kR);

    for (unsigned int i = 0; i < ms_iSvdMaxIterations; i++)
    {
        float fTmp, fTmp0, fTmp1;
        float fSin0, fCos0, fTan0;
        float fSin1, fCos1, fTan1;

        bool bTest1 = (fabs(kA[0][1]) <=
            ms_fSvdEpsilon*(fabs(kA[0][0])+fabs(kA[1][1])));
        bool bTest2 = (fabs(kA[1][2]) <=
            ms_fSvdEpsilon*(fabs(kA[1][1])+fabs(kA[2][2])));
        if ( bTest1 )
        {
            if ( bTest2 )
            {
                kS[0] = kA[0][0];
                kS[1] = kA[1][1];
                kS[2] = kA[2][2];
                break;
            }
            else
            {
                // 2x2 closed form factorization
                fTmp = (kA[1][1]*kA[1][1] - kA[2][2]*kA[2][2] +
                    kA[1][2]*kA[1][2])/(kA[1][2]*kA[2][2]);
                fTan0 = 0.5f*(fTmp+(float)sqrt(fTmp*fTmp + 4.0f));
                fCos0 = 1.0f/(float)sqrt(1.0f+fTan0*fTan0);
                fSin0 = fTan0*fCos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = kL[iCol][1];
                    fTmp1 = kL[iCol][2];
                    kL[iCol][1] = fCos0*fTmp0-fSin0*fTmp1;
                    kL[iCol][2] = fSin0*fTmp0+fCos0*fTmp1;
                }

                fTan1 = (kA[1][2]-kA[2][2]*fTan0)/kA[1][1];
                fCos1 = 1.0f/(float)sqrt(1.0f+fTan1*fTan1);
                fSin1 = -fTan1*fCos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = kR[1][iRow];
                    fTmp1 = kR[2][iRow];
                    kR[1][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                    kR[2][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                }

                kS[0] = kA[0][0];
                kS[1] = fCos0*fCos1*kA[1][1] -
                    fSin1*(fCos0*kA[1][2]-fSin0*kA[2][2]);
                kS[2] = fSin0*fSin1*kA[1][1] +
                    fCos1*(fSin0*kA[1][2]+fCos0*kA[2][2]);
                break;
            }
        }
        else
        {
            if ( bTest2 )
            {
                // 2x2 closed form factorization
                fTmp = (kA[0][0]*kA[0][0] + kA[1][1]*kA[1][1] -
                    kA[0][1]*kA[0][1])/(kA[0][1]*kA[1][1]);
                fTan0 = 0.5f*(-fTmp+(float)sqrt(fTmp*fTmp + 4.0f));
                fCos0 = 1.0f/(float)sqrt(1.0f+fTan0*fTan0);
                fSin0 = fTan0*fCos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = kL[iCol][0];
                    fTmp1 = kL[iCol][1];
                    kL[iCol][0] = fCos0*fTmp0-fSin0*fTmp1;
                    kL[iCol][1] = fSin0*fTmp0+fCos0*fTmp1;
                }

                fTan1 = (kA[0][1]-kA[1][1]*fTan0)/kA[0][0];
                fCos1 = 1.0f/(float)sqrt(1.0f+fTan1*fTan1);
                fSin1 = -fTan1*fCos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = kR[0][iRow];
                    fTmp1 = kR[1][iRow];
                    kR[0][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                    kR[1][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                }

                kS[0] = fCos0*fCos1*kA[0][0] -
                    fSin1*(fCos0*kA[0][1]-fSin0*kA[1][1]);
                kS[1] = fSin0*fSin1*kA[0][0] +
                    fCos1*(fSin0*kA[0][1]+fCos0*kA[1][1]);
                kS[2] = kA[2][2];
                break;
            }
            else
            {
                GolubKahanStep(kA,kL,kR);
            }
        }
    }

    // positize diagonal
    for (iRow = 0; iRow < 3; iRow++)
    {
        if ( kS[iRow] < 0.0 )
        {
            kS[iRow] = -kS[iRow];
            for (iCol = 0; iCol < 3; iCol++)
                kR[iRow][iCol] = -kR[iRow][iCol];
        }
    }
}
//-----------------------------------------------------------------------
void Matrix3x3f::SingularValueComposition (const Matrix3x3f& kL,
    const Vector3f& kS, const Matrix3x3f& kR)
{
    size_t iRow, iCol;
    Matrix3x3f kTmp;

    // product S*R
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
            kTmp[iRow][iCol] = kS[iRow]*kR[iRow][iCol];
    }

    // product L*S*R
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
        {
            m[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 3; iMid++)
                m[iRow][iCol] += kL[iRow][iMid]*kTmp[iMid][iCol];
        }
    }
}
//-----------------------------------------------------------------------
void Matrix3x3f::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    float fInvLength = 1.0f/(float)sqrt(m[0][0]*m[0][0]
        + m[1][0]*m[1][0] +
        m[2][0]*m[2][0]);

    m[0][0] *= fInvLength;
    m[1][0] *= fInvLength;
    m[2][0] *= fInvLength;

    // compute q1
    float fDot0 =
        m[0][0]*m[0][1] +
        m[1][0]*m[1][1] +
        m[2][0]*m[2][1];

    m[0][1] -= fDot0*m[0][0];
    m[1][1] -= fDot0*m[1][0];
    m[2][1] -= fDot0*m[2][0];

    fInvLength = 1.0f/(float)sqrt(m[0][1]*m[0][1] +
        m[1][1]*m[1][1] +
        m[2][1]*m[2][1]);

    m[0][1] *= fInvLength;
    m[1][1] *= fInvLength;
    m[2][1] *= fInvLength;

    // compute q2
    float fDot1 =
        m[0][1]*m[0][2] +
        m[1][1]*m[1][2] +
        m[2][1]*m[2][2];

    fDot0 =
        m[0][0]*m[0][2] +
        m[1][0]*m[1][2] +
        m[2][0]*m[2][2];

    m[0][2] -= fDot0*m[0][0] + fDot1*m[0][1];
    m[1][2] -= fDot0*m[1][0] + fDot1*m[1][1];
    m[2][2] -= fDot0*m[2][0] + fDot1*m[2][1];

    fInvLength = 1.0f/(float)sqrt(m[0][2]*m[0][2] +
        m[1][2]*m[1][2] +
        m[2][2]*m[2][2]);

    m[0][2] *= fInvLength;
    m[1][2] *= fInvLength;
    m[2][2] *= fInvLength;
}
//-----------------------------------------------------------------------
void Matrix3x3f::QDUDecomposition (Matrix3x3f& kQ,
    Vector3f& kD, Vector3f& kU) const
{
    // Factor M = QR = QDU where Q is orthogonal, D is diagonal,
    // and U is upper triangular with ones on its diagonal.  Algorithm uses
    // Gram-Schmidt orthogonalization (the QR algorithm).
    //
    // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
    //   r10 = 0      r11 = q1*m1  r12 = q1*m2
    //   r20 = 0      r21 = 0      r22 = q2*m2
    //
    // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
    // u02 = r02/r00, and u12 = r12/r11.

    // Q = rotation
    // D = scaling
    // U = shear

    // D stores the three diagonal entries r00, r11, r22
    // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

    // build orthogonal matrix Q
    float fInvLength = 1.0f/(float)sqrt(m[0][0]*m[0][0]
        + m[1][0]*m[1][0] +
        m[2][0]*m[2][0]);
    kQ[0][0] = m[0][0]*fInvLength;
    kQ[1][0] = m[1][0]*fInvLength;
    kQ[2][0] = m[2][0]*fInvLength;

    float fDot = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] +
        kQ[2][0]*m[2][1];
    kQ[0][1] = m[0][1]-fDot*kQ[0][0];
    kQ[1][1] = m[1][1]-fDot*kQ[1][0];
    kQ[2][1] = m[2][1]-fDot*kQ[2][0];
    fInvLength = 1.0f/(float)sqrt(kQ[0][1]*kQ[0][1] + kQ[1][1]*kQ[1][1] +
        kQ[2][1]*kQ[2][1]);
    kQ[0][1] *= fInvLength;
    kQ[1][1] *= fInvLength;
    kQ[2][1] *= fInvLength;

    fDot = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] +
        kQ[2][0]*m[2][2];
    kQ[0][2] = m[0][2]-fDot*kQ[0][0];
    kQ[1][2] = m[1][2]-fDot*kQ[1][0];
    kQ[2][2] = m[2][2]-fDot*kQ[2][0];
    fDot = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] +
        kQ[2][1]*m[2][2];
    kQ[0][2] -= fDot*kQ[0][1];
    kQ[1][2] -= fDot*kQ[1][1];
    kQ[2][2] -= fDot*kQ[2][1];
    fInvLength = 1.0f/(float)sqrt(kQ[0][2]*kQ[0][2] + kQ[1][2]*kQ[1][2] +
        kQ[2][2]*kQ[2][2]);
    kQ[0][2] *= fInvLength;
    kQ[1][2] *= fInvLength;
    kQ[2][2] *= fInvLength;

    // guarantee that orthogonal matrix has determinant 1 (no reflections)
    float fDet = kQ[0][0]*kQ[1][1]*kQ[2][2] + kQ[0][1]*kQ[1][2]*kQ[2][0] +
        kQ[0][2]*kQ[1][0]*kQ[2][1] - kQ[0][2]*kQ[1][1]*kQ[2][0] -
        kQ[0][1]*kQ[1][0]*kQ[2][2] - kQ[0][0]*kQ[1][2]*kQ[2][1];

    if ( fDet < 0.0 )
    {
        for (size_t iRow = 0; iRow < 3; iRow++)
            for (size_t iCol = 0; iCol < 3; iCol++)
                kQ[iRow][iCol] = -kQ[iRow][iCol];
    }

    // build "right" matrix R
    Matrix3x3f kR;
    kR[0][0] = kQ[0][0]*m[0][0] + kQ[1][0]*m[1][0] +
        kQ[2][0]*m[2][0];
    kR[0][1] = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] +
        kQ[2][0]*m[2][1];
    kR[1][1] = kQ[0][1]*m[0][1] + kQ[1][1]*m[1][1] +
        kQ[2][1]*m[2][1];
    kR[0][2] = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] +
        kQ[2][0]*m[2][2];
    kR[1][2] = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] +
        kQ[2][1]*m[2][2];
    kR[2][2] = kQ[0][2]*m[0][2] + kQ[1][2]*m[1][2] +
        kQ[2][2]*m[2][2];

    // the scaling component
    kD[0] = kR[0][0];
    kD[1] = kR[1][1];
    kD[2] = kR[2][2];

    // the shear component
    float fInvD0 = 1.0f/kD[0];
    kU[0] = kR[0][1]*fInvD0;
    kU[1] = kR[0][2]*fInvD0;
    kU[2] = kR[1][2]/kD[1];
}
//-----------------------------------------------------------------------
float Matrix3x3f::MaxCubicRoot (float afCoeff[3])
{
    // Spectral norm is for A^T*A, so characteristic polynomial
    // P(x) = c[0]+c[1]*x+c[2]*x^2+x^3 has three positive float roots.
    // This yields the assertions c[0] < 0 and c[2]*c[2] >= 3*c[1].

    // quick out for uniform scale (triple root)
    const float fOneThird = 1.0f/3.0f;
    const float fEpsilon = 1e-06f;
    float fDiscr = afCoeff[2]*afCoeff[2] - 3.0f*afCoeff[1];
    if ( fDiscr <= fEpsilon )
        return -fOneThird*afCoeff[2];

    // Compute an upper bound on roots of P(x).  This assumes that A^T*A
    // has been scaled by its largest entry.
    float fX = 1.0f;
    float fPoly = afCoeff[0]+fX*(afCoeff[1]+fX*(afCoeff[2]+fX));
    if ( fPoly < 0.0 )
    {
        // uses a matrix norm to find an upper bound on maximum root
        fX = (float)fabs(afCoeff[0]);
        float fTmp = 1.0f+(float)fabs(afCoeff[1]);
        if ( fTmp > fX )
            fX = fTmp;
        fTmp = 1.0f+(float)fabs(afCoeff[2]);
        if ( fTmp > fX )
            fX = fTmp;
    }

    // Newton's method to find root
    float fTwoC2 = 2.0f*afCoeff[2];
    for (int i = 0; i < 16; i++)
    {
        fPoly = afCoeff[0]+fX*(afCoeff[1]+fX*(afCoeff[2]+fX));
        if ( fabs(fPoly) <= fEpsilon )
            return fX;

        float fDeriv = afCoeff[1]+fX*(fTwoC2+3.0f*fX);
        fX -= fPoly/fDeriv;
    }

    return fX;
}
//-----------------------------------------------------------------------
float Matrix3x3f::SpectralNorm () const
{
    Matrix3x3f kP;
    size_t iRow, iCol;
    float fPmax = 0.0f;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
        {
            kP[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 3; iMid++)
            {
                kP[iRow][iCol] +=
                    m[iMid][iRow]*m[iMid][iCol];
            }
            if ( kP[iRow][iCol] > fPmax )
                fPmax = kP[iRow][iCol];
        }
    }

    float fInvPmax = 1.0f/fPmax;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
            kP[iRow][iCol] *= fInvPmax;
    }

    float afCoeff[3];
    afCoeff[0] = -(kP[0][0]*(kP[1][1]*kP[2][2]-kP[1][2]*kP[2][1]) +
        kP[0][1]*(kP[2][0]*kP[1][2]-kP[1][0]*kP[2][2]) +
        kP[0][2]*(kP[1][0]*kP[2][1]-kP[2][0]*kP[1][1]));
    afCoeff[1] = kP[0][0]*kP[1][1]-kP[0][1]*kP[1][0] +
        kP[0][0]*kP[2][2]-kP[0][2]*kP[2][0] +
        kP[1][1]*kP[2][2]-kP[1][2]*kP[2][1];
    afCoeff[2] = -(kP[0][0]+kP[1][1]+kP[2][2]);

    float fRoot = MaxCubicRoot(afCoeff);
    float fNorm = (float)sqrt(fPmax*fRoot);
    return fNorm;
}

//-----------------------------------------------------------------------
void Matrix3x3f::Tridiagonal (float afDiag[3], float afSubDiag[3])
{
    // Householder reduction T = Q^t M Q
    //   Input:
    //     mat, symmetric 3x3 matrix M
    //   Output:
    //     mat, orthogonal matrix Q
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    float fA = m[0][0];
    float fB = m[0][1];
    float fC = m[0][2];
    float fD = m[1][1];
    float fE = m[1][2];
    float fF = m[2][2];

    afDiag[0] = fA;
    afSubDiag[2] = 0.0f;
    if ( fabs(fC) >= EPSILON )
    {
        float fLength = (float)sqrt(fB*fB+fC*fC);
        float fInvLength = 1.0f/fLength;
        fB *= fInvLength;
        fC *= fInvLength;
        float fQ = 2.0f*fB*fE+fC*(fF-fD);
        afDiag[1] = fD+fC*fQ;
        afDiag[2] = fF-fC*fQ;
        afSubDiag[0] = fLength;
        afSubDiag[1] = fE-fB*fQ;
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = fB;
        m[1][2] = fC;
        m[2][0] = 0.0f;
        m[2][1] = fC;
        m[2][2] = -fB;
    }
    else
    {
        afDiag[1] = fD;
        afDiag[2] = fF;
        afSubDiag[0] = fB;
        afSubDiag[1] = fE;
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[2][0] = 0.0f;
        m[2][1] = 0.0f;
        m[2][2] = 1.0f;
    }
}
//-----------------------------------------------------------------------
bool Matrix3x3f::QLAlgorithm (float afDiag[3], float afSubDiag[3])
{
    // QL iteration with implicit shifting to reduce matrix from tridiagonal
    // to diagonal

    for (int i0 = 0; i0 < 3; i0++)
    {
        const unsigned int iMaxIter = 32;
        unsigned int iIter;
        for (iIter = 0; iIter < iMaxIter; iIter++)
        {
            int i1;
            for (i1 = i0; i1 <= 1; i1++)
            {
                float fSum = (float)fabs(afDiag[i1]) +
                    (float)fabs(afDiag[i1+1]);
                if ( fabs(afSubDiag[i1]) + fSum == fSum )
                    break;
            }
            if ( i1 == i0 )
                break;

            float fTmp0 = (afDiag[i0+1]-afDiag[i0])/(2.0f*afSubDiag[i0]);
            float fTmp1 = (float)sqrt(fTmp0*fTmp0+1.0f);
            if ( fTmp0 < 0.0 )
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0-fTmp1);
            else
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0+fTmp1);
            float fSin = 1.0f;
            float fCos = 1.0f;
            float fTmp2 = 0.0f;
            for (int i2 = i1-1; i2 >= i0; i2--)
            {
                float fTmp3 = fSin*afSubDiag[i2];
                float fTmp4 = fCos*afSubDiag[i2];
                if ( fabs(fTmp3) >= fabs(fTmp0) )
                {
                    fCos = fTmp0/fTmp3;
                    fTmp1 = (float)sqrt(fCos*fCos+1.0f);
                    afSubDiag[i2+1] = fTmp3*fTmp1;
                    fSin = 1.0f/fTmp1;
                    fCos *= fSin;
                }
                else
                {
                    fSin = fTmp3/fTmp0;
                    fTmp1 = (float)sqrt(fSin*fSin+1.0f);
                    afSubDiag[i2+1] = fTmp0*fTmp1;
                    fCos = 1.0f/fTmp1;
                    fSin *= fCos;
                }
                fTmp0 = afDiag[i2+1]-fTmp2;
                fTmp1 = (afDiag[i2]-fTmp0)*fSin+2.0f*fTmp4*fCos;
                fTmp2 = fSin*fTmp1;
                afDiag[i2+1] = fTmp0+fTmp2;
                fTmp0 = fCos*fTmp1-fTmp4;

                for (int iRow = 0; iRow < 3; iRow++)
                {
                    fTmp3 = m[iRow][i2+1];
                    m[iRow][i2+1] = fSin*m[iRow][i2] +
                        fCos*fTmp3;
                    m[iRow][i2] = fCos*m[iRow][i2] -
                        fSin*fTmp3;
                }
            }
            afDiag[i0] -= fTmp2;
            afSubDiag[i0] = fTmp0;
            afSubDiag[i1] = 0.0f;
        }

        if ( iIter == iMaxIter )
        {
            // should not get here under normal circumstances
            return false;
        }
    }

    return true;
}
//-----------------------------------------------------------------------
void Matrix3x3f::EigenSolveSymmetric (float afEigenvalue[3],
    Vector3f akEigenvector[3]) const
{
    Matrix3x3f kMatrix = *this;
    float afSubDiag[3];
    kMatrix.Tridiagonal(afEigenvalue,afSubDiag);
    kMatrix.QLAlgorithm(afEigenvalue,afSubDiag);

    for (size_t i = 0; i < 3; i++)
    {
        akEigenvector[i][0] = kMatrix[0][i];
        akEigenvector[i][1] = kMatrix[1][i];
        akEigenvector[i][2] = kMatrix[2][i];
    }

    // make eigenvectors form a right--handed system
    Vector3f kCross = akEigenvector[1].CrossProduct(akEigenvector[2]);
    float fDet = akEigenvector[0].DotProduct(kCross);
    if ( fDet < 0.0 )
    {
        akEigenvector[2][0] = - akEigenvector[2][0];
        akEigenvector[2][1] = - akEigenvector[2][1];
        akEigenvector[2][2] = - akEigenvector[2][2];
    }
}
//-----------------------------------------------------------------------
void Matrix3x3f::TensorProduct (const Vector3f& rkU, const Vector3f& rkV,
    Matrix3x3f& rkProduct)
{
    for (size_t iRow = 0; iRow < 3; iRow++)
    {
        for (size_t iCol = 0; iCol < 3; iCol++)
            rkProduct[iRow][iCol] = rkU[iRow]*rkV[iCol];
    }
}
//-----------------------------------------------------------------------

void MultiplyWithTranspose(Matrix3x3f &dest, const Vector3f &a, const Vector3f& b)
{
	dest.m[0][0] = a.x * b.x;
	dest.m[0][1] = a.x * b.y;
	dest.m[0][2] = a.x * b.z;
	dest.m[1][0] = a.y * b.x;
	dest.m[1][1] = a.y * b.y;
	dest.m[1][2] = a.y * b.z;
	dest.m[2][0] = a.z * b.x;
	dest.m[2][1] = a.z * b.y;
	dest.m[2][2] = a.z * b.z;
}

Matrix3x3f MultiplyWithTranspose(const Vector3f &a, const Vector3f& b)
{
	return Matrix3x3f(	a.x * b.x,	a.x * b.y,	a.x * b.z,
						a.y * b.x,	a.y * b.y,	a.y * b.z,
						a.z * b.x,	a.z * b.y,	a.z * b.z);
}

/*
std::string ToString(Matrix3x3f &M)
{
	return		std::string("[") + StringConverter::ToString(M.m00) + std::string(", ") + StringConverter::ToString(M.m01) + std::string(", ") + StringConverter::ToString(M.m02) + std::string(";\n") +
				std::string(" ") + StringConverter::ToString(M.m10) + std::string(", ") + StringConverter::ToString(M.m11) + std::string(", ") + StringConverter::ToString(M.m12) + std::string(";\n") +
				std::string(" ") + StringConverter::ToString(M.m20) + std::string(", ") + StringConverter::ToString(M.m21) + std::string(", ") + StringConverter::ToString(M.m22) + std::string("]\n");
}
*/

void Matrix3x3f::Rotate(float radians, const Vector3f &axis)
{
	SetColumn(0, GetColumn(0).RotatedCopy(radians, axis));
	SetColumn(1, GetColumn(1).RotatedCopy(radians, axis));
	SetColumn(2, GetColumn(2).RotatedCopy(radians, axis));
}
