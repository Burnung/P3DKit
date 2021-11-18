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
#pragma once
#include "Vector3f.h"
#include <memory.h>
#include <string>

// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

/** A 3x3 matrix which can represent rotations around axes.
    @note
        <b>All the code is adapted from the Wild Magic 0.2 Matrix
        library (http://www.geometrictools.com/).</b>
    @par
        The coordinate system is assumed to be <b>right-handed</b>.
*/

class Matrix3x3f;
void MultiplyWithTranspose(Matrix3x3f &dest, const Vector3f &a, const Vector3f& b);
Matrix3x3f MultiplyWithTranspose(const Vector3f &a, const Vector3f& b);

class Matrix3x3f
{
public:
    /** Default constructor.
        @note
            It does <b>NOT</b> initialize the matrix for efficiency.
    */
	inline Matrix3x3f () {};
    inline explicit Matrix3x3f (const float arr[3][3])
	{
		memcpy(m,arr,9*sizeof(float));
	}
    inline Matrix3x3f (const Matrix3x3f& rkMatrix)
	{
		memcpy(m,rkMatrix.m,9*sizeof(float));
	}
    Matrix3x3f (float fEntry00, float fEntry01, float fEntry02,
                float fEntry10, float fEntry11, float fEntry12,
                float fEntry20, float fEntry21, float fEntry22)
	{
		m[0][0] = fEntry00;
		m[0][1] = fEntry01;
		m[0][2] = fEntry02;
		m[1][0] = fEntry10;
		m[1][1] = fEntry11;
		m[1][2] = fEntry12;
		m[2][0] = fEntry20;
		m[2][1] = fEntry21;
		m[2][2] = fEntry22;
	}
    Matrix3x3f (Vector3f x,
                Vector3f y,
                Vector3f z)
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

    // member access, allows use of construct mat[r][c]
    inline float* operator[] (size_t iRow) const
	{
		return (float*)m[iRow];
	}
    /*inline operator float* ()
	{
		return (float*)m[0];
	}*/
    Vector3f GetColumn (size_t iCol) const;
    void SetColumn(size_t iCol, const Vector3f& vec);
    void SetColumns(const Vector3f& x, const Vector3f& y, const Vector3f& z);
    void FromAxes(const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& zAxis);

    // assignment and comparison
    inline Matrix3x3f& operator= (const Matrix3x3f& rkMatrix)
	{
		memcpy(m,rkMatrix.m,9*sizeof(float));
		return *this;
	}
    bool operator== (const Matrix3x3f& rkMatrix) const;
    inline bool operator!= (const Matrix3x3f& rkMatrix) const
	{
		return !operator==(rkMatrix);
	}

    // arithmetic operations
    Matrix3x3f operator+ (const Matrix3x3f& rkMatrix) const;
    Matrix3x3f &operator+= (const Matrix3x3f& rkMatrix);
    Matrix3x3f operator- (const Matrix3x3f& rkMatrix) const;
    Matrix3x3f operator* (const Matrix3x3f& rkMatrix) const;
    Matrix3x3f operator- () const;

    // matrix * vector [3x3 * 3x1 = 3x1]
    Vector3f operator* (const Vector3f& rkVector) const;

    // vector * matrix [1x3 * 3x3 = 1x3]
    friend Vector3f operator* (const Vector3f& rkVector,
        const Matrix3x3f& rkMatrix);

    // matrix * scalar
    Matrix3x3f operator* (float fScalar) const;
    void operator*= (float fScalar);

    // scalar * matrix
    friend Matrix3x3f operator* (float fScalar, const Matrix3x3f& rkMatrix);

    // utilities
    Matrix3x3f Transpose () const;
    bool Inverse (Matrix3x3f& rkInverse, float fTolerance = 1e-06) const;
    Matrix3x3f Inverse (float fTolerance = 1e-06) const;
    float Determinant () const;

    // singular value decomposition
    void SingularValueDecomposition (Matrix3x3f& rkL, Vector3f& rkS,
        Matrix3x3f& rkR) const;
    void SingularValueComposition (const Matrix3x3f& rkL,
        const Vector3f& rkS, const Matrix3x3f& rkR);

    // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
    void Orthonormalize ();

    // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
    void QDUDecomposition (Matrix3x3f& rkQ, Vector3f& rkD,
        Vector3f& rkU) const;

    float SpectralNorm () const;
    // eigensolver, matrix must be symmetric
    void EigenSolveSymmetric (float afEigenvalue[3],
        Vector3f akEigenvector[3]) const;

    static void TensorProduct (const Vector3f& rkU, const Vector3f& rkV,
        Matrix3x3f& rkProduct);

    static const float EPSILON;
    static const Matrix3x3f ZERO;
    static const Matrix3x3f IDENTITY;

	void Rotate(float radians, const Vector3f &axis);

	friend void MultiplyWithTranspose(Matrix3x3f &dest, const Vector3f &a, const Vector3f& b);

	union
	{
		float m[3][3];
		struct
		{
			float m00, m01, m02, m10, m11, m12, m20, m21, m22;
		};
	};
protected:
    // support for eigensolver
    void Tridiagonal (float afDiag[3], float afSubDiag[3]);
    bool QLAlgorithm (float afDiag[3], float afSubDiag[3]);

    // support for singular value decomposition
    static const float ms_fSvdEpsilon;
    static const unsigned int ms_iSvdMaxIterations;
    static void Bidiagonalize (Matrix3x3f& kA, Matrix3x3f& kL,
        Matrix3x3f& kR);
    static void GolubKahanStep (Matrix3x3f& kA, Matrix3x3f& kL,
        Matrix3x3f& kR);

    // support for spectral norm
    static float MaxCubicRoot (float afCoeff[3]);

};

//std::string ToString(Matrix3x3f &M);
