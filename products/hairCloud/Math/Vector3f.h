#pragma once
#include <math.h>
#include "vec.h"
#include <assert.h>

class Vector3f
{
public:
    union {
        struct {
            float x, y, z;
        };
        float val[3];
    };

public:
    inline Vector3f()
		: x( 0 ), y( 0 ), z( 0 )
    {
    }

    inline Vector3f( const float fX, const float fY, const float fZ )
        : x( fX ), y( fY ), z( fZ )
    {
    }

	inline Vector3f(Vec3f vec)
		: x(vec[0]), y(vec[1]), z(vec[2])
	{
	}

    inline explicit Vector3f( const float afCoordinate[3] )
        : x( afCoordinate[0] ),
          y( afCoordinate[1] ),
          z( afCoordinate[2] )
    {
    }

    //inline explicit Vector3f( const int afCoordinate[3] )
    //{
    //    x = (float)afCoordinate[0];
    //    y = (float)afCoordinate[1];
    //    z = (float)afCoordinate[2];
    //}

    inline explicit Vector3f( float* const r )
        : x( r[0] ), y( r[1] ), z( r[2] )
    {
    }

    inline explicit Vector3f( const float scalar )
        : x( scalar )
        , y( scalar )
        , z( scalar )
    {
    }


    inline Vector3f( const Vector3f& rkVector )
        : x( rkVector.x ), y( rkVector.y ), z( rkVector.z )
    {
    }

	inline float operator [] ( const size_t i ) const
    {
        assert( i < 3 );

        return *(&x+i);
    }

	inline float& operator [] ( const size_t i )
    {
        assert( i < 3 );

        return *(&x+i);
    }

    /** Assigns the value of the other vector.
        @param
            rkVector The other vector
    */
    inline Vector3f& operator = ( const Vector3f& rkVector )
    {
        x = rkVector.x;
        y = rkVector.y;
        z = rkVector.z;

        return *this;
    }

    inline Vector3f& operator = ( const float fScalar )
    {
        x = fScalar;
        y = fScalar;
        z = fScalar;

        return *this;
    }

    inline bool operator == ( const Vector3f& rkVector ) const
    {
        return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
    }

    inline bool operator != ( const Vector3f& rkVector ) const
    {
        return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
    }

    // arithmetic operations
    inline Vector3f operator + ( const Vector3f& rkVector ) const
    {
        Vector3f kSum;

        kSum.x = x + rkVector.x;
        kSum.y = y + rkVector.y;
        kSum.z = z + rkVector.z;

        return kSum;
    }

    inline Vector3f operator - ( const Vector3f& rkVector ) const
    {
        Vector3f kDiff;

        kDiff.x = x - rkVector.x;
        kDiff.y = y - rkVector.y;
        kDiff.z = z - rkVector.z;

        return kDiff;
    }

    inline Vector3f operator * ( const float fScalar ) const
    {
        Vector3f kProd;

        kProd.x = fScalar*x;
        kProd.y = fScalar*y;
        kProd.z = fScalar*z;

        return kProd;
    }

    inline Vector3f operator * ( const Vector3f& rhs) const
    {
        Vector3f kProd;

        kProd.x = rhs.x * x;
        kProd.y = rhs.y * y;
        kProd.z = rhs.z * z;

        return kProd;
    }

    inline Vector3f operator / ( const float fScalar ) const
    {
        assert( fScalar != 0.0f );

        Vector3f kDiv;

        float fInv = 1.0f / fScalar;
        kDiv.x = x * fInv;
        kDiv.y = y * fInv;
        kDiv.z = z * fInv;

        return kDiv;
    }

    inline Vector3f operator / ( const Vector3f& rhs) const
    {
        Vector3f kDiv;

        kDiv.x = x / rhs.x;
        kDiv.y = y / rhs.y;
        kDiv.z = z / rhs.z;

        return kDiv;
    }


    inline Vector3f operator - () const
    {
        Vector3f kNeg;

        kNeg.x = -x;
        kNeg.y = -y;
        kNeg.z = -z;

        return kNeg;
    }

    // overloaded operators to help Vector3f
    inline friend Vector3f operator * ( const float fScalar, const Vector3f& rkVector )
    {
        Vector3f kProd;

        kProd.x = fScalar * rkVector.x;
        kProd.y = fScalar * rkVector.y;
        kProd.z = fScalar * rkVector.z;

        return kProd;
    }

    inline friend Vector3f operator + (const Vector3f& lhs, const float rhs)
    {
        Vector3f ret(rhs);
        return ret += lhs;
    }

    inline friend Vector3f operator + (const float lhs, const Vector3f& rhs)
    {
        Vector3f ret(lhs);
        return ret += rhs;
    }

    inline friend Vector3f operator - (const Vector3f& lhs, const float rhs)
    {
        return lhs - Vector3f(rhs);
    }

    inline friend Vector3f operator - (const float lhs, const Vector3f& rhs)
    {
        Vector3f ret(lhs);
        return ret -= rhs;
    }

    // arithmetic updates
    inline Vector3f& operator += ( const Vector3f& rkVector )
    {
        x += rkVector.x;
        y += rkVector.y;
        z += rkVector.z;

        return *this;
    }

    inline Vector3f& operator += ( const float fScalar )
    {
        x += fScalar;
        y += fScalar;
        z += fScalar;
        return *this;
    }

    inline Vector3f& operator -= ( const Vector3f& rkVector )
    {
        x -= rkVector.x;
        y -= rkVector.y;
        z -= rkVector.z;

        return *this;
    }

    inline Vector3f& operator -= ( const float fScalar )
    {
        x -= fScalar;
        y -= fScalar;
        z -= fScalar;
        return *this;
    }

    inline Vector3f& operator *= ( const float fScalar )
    {
        x *= fScalar;
        y *= fScalar;
        z *= fScalar;
        return *this;
    }

    inline Vector3f& operator *= ( const Vector3f& rkVector )
    {
        x *= rkVector.x;
        y *= rkVector.y;
        z *= rkVector.z;

        return *this;
    }

    inline Vector3f& operator /= ( const float fScalar )
    {
        assert( fScalar != 0.0f );

        float fInv = 1.0f / fScalar;

        x *= fInv;
        y *= fInv;
        z *= fInv;

        return *this;
    }

    inline Vector3f& operator /= ( const Vector3f& rkVector )
    {
        x /= rkVector.x;
        y /= rkVector.y;
        z /= rkVector.z;

        return *this;
    }


    /** Returns the length (magnitude) of the vector.
        @warning
            This operation requires a square root and is expensive in
            terms of CPU operations. If you don't need to know the exact
            length (e.g. for just comparing lengths) use squaredLength()
            instead.
    */
    inline float Length () const
    {
        return (float)sqrt( x * x + y * y + z * z );
    }
    inline float Magnitude () const
    {
        return (float)sqrt( x * x + y * y + z * z );
    }
    /** Returns the square of the length(magnitude) of the vector.
        @remarks
            This  method is for efficiency - calculating the actual
            length of a vector requires a square root, which is expensive
            in terms of the operations required. This method returns the
            square of the length of the vector, i.e. the same as the
            length but before the square root is taken. Use this if you
            want to find the longest / shortest vector without incurring
            the square root.
    */
    inline float SquaredLength () const
    {
        return x * x + y * y + z * z;
    }

    /** Calculates the dot (scalar) product of this vector with another.
        @remarks
            The dot product can be used to calculate the angle between 2
            vectors. If both are unit vectors, the dot product is the
            cosine of the angle; otherwise the dot product must be
            divided by the product of the lengths of both vectors to get
            the cosine of the angle. This result can further be used to
            calculate the distance of a point from a plane.
        @param
            vec Vector with which to calculate the dot product (together
            with this one).
        @returns
            A float representing the dot product value.
    */
    inline float DotProduct(const Vector3f& vec) const
    {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    inline float AngleBetween(const Vector3f& vec) const
    {
		float dot = DotProduct(vec);
		float len = Length() + vec.Length();
		float cos = dot / len;
		float angle = (float)acos(cos);
        return angle;
    }

    /** Normalizes the vector.
        @remarks
            This method Normalizes the vector such that it's
            length / magnitude is 1. The result is called a unit vector.
        @note
            This function will not crash for zero-sized vectors, but there
            will be no changes made to their components.
        @returns The previous length of the vector.
    */
    inline float Normalize()
    {
        float fLength = (float)sqrt( x * x + y * y + z * z );

        // Will also work for zero-sized vectors, but will change nothing
        if ( fLength > 1e-08 )
        {
            float fInvLength = 1.0f / fLength;
            x *= fInvLength;
            y *= fInvLength;
            z *= fInvLength;
        }

        return fLength;
    }

	inline Vector3f RotatedCopy(float angle, const Vector3f &axis)
	{
		Vector3f ret = *this;
		ret.Rotate(angle, axis);
		return ret;
	}

	inline void Rotate(float angle, const Vector3f &axisP)
	{
		//
		//  Compute the length of the rotation axis.
		//
		Vector3f axis = axisP;
		if(axis.SquaredLength() != 1)
			axis.Normalize();

		//
		//  Compute the dot product of the vector and the rotation axis.
		//
		float dot = DotProduct(axis);
		//
		//  Compute the parallel component of the vector.
		//
		Vector3f xp = dot * axis;
		//
		//  Compute the normal component of the vector.
		//
		Vector3f xn = *this - xp;
		float normn = xn.Length();
		xn.Normalize();

		//
		//  Compute a second vector, lying in the plane, perpendicular
		//  to (X1,Y1,Z1), and forming a right-handed system...
		//
		Vector3f xn2 = axis.CrossProduct(*this);
		xn2.Normalize();

		//
		//  Rotate the normal component by the angle.
		//
		Vector3f xr = normn * ((float)cos(angle) * xn + (float)sin(angle) * xn2);
		//
		//  The rotated vector is the parallel component plus the rotated
		//  component.
		//
		*this = xp + xr;
	}

    /** Calculates the cross-product of 2 vectors, i.e. the vector that
        lies perpendicular to them both.
        @remarks
            The cross-product is normally used to calculate the normal
            vector of a plane, by calculating the cross-product of 2
            non-equivalent vectors which lie on the plane (e.g. 2 edges
            of a triangle).
        @param
            vec Vector which, together with this one, will be used to
            calculate the cross-product.
        @returns
            A vector which is the result of the cross-product. This
            vector will <b>NOT</b> be Normalized, to maximise efficiency
            - call Vector3f::Normalize on the result if you wish this to
            be done. As for which side the resultant vector will be on, the
            returned vector will be on the side from which the arc from 'this'
            to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
            = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
			This is because OGRE uses a right-handed coordinate system.
        @par
            For a clearer explanation, look a the left and the bottom edges
            of your monitor's screen. Assume that the first vector is the
            left edge and the second vector is the bottom edge, both of
            them starting from the lower-left corner of the screen. The
            resulting vector is going to be perpendicular to both of them
            and will go <i>inside</i> the screen, towards the cathode tube
            (assuming you're using a CRT monitor, of course).
    */
    inline Vector3f CrossProduct( const Vector3f& rkVector ) const
    {
        Vector3f kCross;

        kCross.x = y * rkVector.z - z * rkVector.y;
        kCross.y = z * rkVector.x - x * rkVector.z;
        kCross.z = x * rkVector.y - y * rkVector.x;

        return kCross;
    }

    /** Returns true if the vector's scalar components are all greater
        that the ones of the vector it is compared against.
    */
    inline bool operator < ( const Vector3f& rhs ) const
    {
        if( x < rhs.x && y < rhs.y && z < rhs.z )
            return true;
        return false;
    }

    /** Returns true if the vector's scalar components are all smaller
        that the ones of the vector it is compared against.
    */
    inline bool operator > ( const Vector3f& rhs ) const
    {
        if( x > rhs.x && y > rhs.y && z > rhs.z )
            return true;
        return false;
    }

    /** As Normalize, except that this vector is unaffected and the
        Normalized vector is returned as a copy. */
    inline Vector3f NormalizedCopy(void) const
    {
        Vector3f ret = *this;
        ret.Normalize();
        return ret;
    }

    /** Calculates a reflection vector to the plane with the given normal .
    @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
    */
    inline Vector3f Reflect(const Vector3f& normal) const
    {
        return Vector3f( *this - ( 2 * this->DotProduct(normal) * normal ) );
    }
	
    static const Vector3f ZERO;
    static const Vector3f UNIT_X;
    static const Vector3f UNIT_Y;
    static const Vector3f UNIT_Z;
};
