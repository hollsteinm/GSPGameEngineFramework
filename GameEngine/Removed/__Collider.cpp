#include "Collider.h"

///////////////////////////////////////////////////////////
//Helpers (XNACollision Demo 2010)
///////////////////////////////////////////////////////////
static const XMVECTOR unitQuaternionEpsilon =
{
    1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f
};

static const XMVECTOR unitVectorEpsilon =
{
    1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f
};

static const XMVECTOR unitPlaneEpsilon =
{
    1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f
};


//-----------------------------------------------------------------------------
// Return TRUE if any of the elements of a 3 vector are equal to 0xffffffff.
// Slightly more efficient than using XMVector3EqualInt.
//-----------------------------------------------------------------------------
static inline BOOL XMVector3AnyTrue( FXMVECTOR V )
{
    XMVECTOR C;

    // Duplicate the fourth element from the first element.
    C = XMVectorSwizzle( V, 0, 1, 2, 0 );

    return XMComparisonAnyTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );
}



//-----------------------------------------------------------------------------
// Return TRUE if all of the elements of a 3 vector are equal to 0xffffffff.
// Slightly more efficient than using XMVector3EqualInt.
//-----------------------------------------------------------------------------
static inline BOOL XMVector3AllTrue( FXMVECTOR V )
{
    XMVECTOR C;

    // Duplicate the fourth element from the first element.
    C = XMVectorSwizzle( V, 0, 1, 2, 0 );

    return XMComparisonAllTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );
}



//-----------------------------------------------------------------------------
// Return TRUE if the vector is a unit vector (length == 1).
//-----------------------------------------------------------------------------
static inline BOOL XMVector3IsUnit( FXMVECTOR V )
{
    XMVECTOR Difference = XMVector3Length( V ) - XMVectorSplatOne();

    return XMVector4Less( XMVectorAbs( Difference ), unitVectorEpsilon );
}



//-----------------------------------------------------------------------------
// Return TRUE if the quaterion is a unit quaternion.
//-----------------------------------------------------------------------------
static inline BOOL XMQuaternionIsUnit( FXMVECTOR Q )
{
    XMVECTOR Difference = XMVector4Length( Q ) - XMVectorSplatOne();

    return XMVector4Less( XMVectorAbs( Difference ), unitQuaternionEpsilon );
}



//-----------------------------------------------------------------------------
// Return TRUE if the plane is a unit plane.
//-----------------------------------------------------------------------------
static inline BOOL XMPlaneIsUnit( FXMVECTOR Plane )
{
    XMVECTOR Difference = XMVector3Length( Plane ) - XMVectorSplatOne();

    return XMVector4Less( XMVectorAbs( Difference ), unitPlaneEpsilon );
}



//-----------------------------------------------------------------------------
// Transform a plane by a rotation and translation.
//-----------------------------------------------------------------------------
static inline XMVECTOR TransformPlane( FXMVECTOR Plane, FXMVECTOR Rotation, FXMVECTOR Translation )
{
    XMVECTOR Normal = XMVector3Rotate( Plane, Rotation );
    XMVECTOR D = XMVectorSplatW( Plane ) - XMVector3Dot( Normal, Translation );

    return XMVectorInsert( Normal, D, 0, 0, 0, 0, 1 );
}



//-----------------------------------------------------------------------------
// Return the point on the line segement (S1, S2) nearest the point P.
//-----------------------------------------------------------------------------
static inline XMVECTOR PointOnLineSegmentNearestPoint( FXMVECTOR S1, FXMVECTOR S2, FXMVECTOR P )
{
    XMVECTOR Dir = S2 - S1;
    XMVECTOR Projection = ( XMVector3Dot( P, Dir ) - XMVector3Dot( S1, Dir ) );
    XMVECTOR LengthSq = XMVector3Dot( Dir, Dir );

    XMVECTOR t = Projection * XMVectorReciprocal( LengthSq );
    XMVECTOR Point = S1 + t * Dir;

    // t < 0
    XMVECTOR SelectS1 = XMVectorLess( Projection, XMVectorZero() );
    Point = XMVectorSelect( Point, S1, SelectS1 );

    // t > 1
    XMVECTOR SelectS2 = XMVectorGreater( Projection, LengthSq );
    Point = XMVectorSelect( Point, S2, SelectS2 );

    return Point;
}


//-----------------------------------------------------------------------------
// Test if the point (P) on the plane of the triangle is inside the triangle 
// (V0, V1, V2).
//-----------------------------------------------------------------------------
static inline XMVECTOR PointOnPlaneInsideTriangle( FXMVECTOR P, FXMVECTOR V0, FXMVECTOR V1, CXMVECTOR V2 )
{
    // Compute the triangle normal.
    XMVECTOR N = XMVector3Cross( V2 - V0, V1 - V0 );

    // Compute the cross products of the vector from the base of each edge to 
    // the point with each edge vector.
    XMVECTOR C0 = XMVector3Cross( P - V0, V1 - V0 );
    XMVECTOR C1 = XMVector3Cross( P - V1, V2 - V1 );
    XMVECTOR C2 = XMVector3Cross( P - V2, V0 - V2 );

    // If the cross product points in the same direction as the normal the the
    // point is inside the edge (it is zero if is on the edge).
    XMVECTOR Zero = XMVectorZero();
    XMVECTOR Inside0 = XMVectorGreaterOrEqual( XMVector3Dot( C0, N ), Zero );
    XMVECTOR Inside1 = XMVectorGreaterOrEqual( XMVector3Dot( C1, N ), Zero );
    XMVECTOR Inside2 = XMVectorGreaterOrEqual( XMVector3Dot( C2, N ), Zero );

    // If the point inside all of the edges it is inside.
    return XMVectorAndInt( XMVectorAndInt( Inside0, Inside1 ), Inside2 );
}

static inline BOOL SolveCubic( FLOAT e, FLOAT f, FLOAT g, FLOAT* t, FLOAT* u, FLOAT* v )
{
    FLOAT p, q, h, rc, d, theta, costh3, sinth3;

    p = f - e * e / 3.0f;
    q = g - e * f / 3.0f + e * e * e * 2.0f / 27.0f;
    h = q * q / 4.0f + p * p * p / 27.0f;

    if( h > 0.0 )
    {
        return FALSE; // only one real root
    }

    if( ( h == 0.0 ) && ( q == 0.0 ) ) // all the same root
    {
        *t = - e / 3;
        *u = - e / 3;
        *v = - e / 3;

        return TRUE;
    }

    d = sqrtf( q * q / 4.0f - h );
    if( d < 0 )
        rc = -powf( -d, 1.0f / 3.0f );
    else
        rc = powf( d, 1.0f / 3.0f );

    theta = acosf( -q / ( 2.0f * d ) );
    costh3 = cosf( theta / 3.0f );
    sinth3 = sqrtf( 3.0f ) * sinf( theta / 3.0f );
    *t = 2.0f * rc * costh3 - e / 3.0f;
    *u = -rc * ( costh3 + sinth3 ) - e / 3.0f;
    *v = -rc * ( costh3 - sinth3 ) - e / 3.0f;

    return TRUE;
}

static inline XMVECTOR CalculateEigenVector( FLOAT m11, FLOAT m12, FLOAT m13,
                                             FLOAT m22, FLOAT m23, FLOAT m33, FLOAT e )
{
    FLOAT f1, f2, f3;

    FLOAT fTmp[3];
    fTmp[0] = ( FLOAT )( m12 * m23 - m13 * ( m22 - e ) );
    fTmp[1] = ( FLOAT )( m13 * m12 - m23 * ( m11 - e ) );
    fTmp[2] = ( FLOAT )( ( m11 - e ) * ( m22 - e ) - m12 * m12 );

    XMVECTOR vTmp = XMLoadFloat3( (XMFLOAT3*)fTmp );

    if( XMVector3Equal( vTmp, XMVectorZero() ) ) // planar or linear
    {
        // we only have one equation - find a valid one
        if( ( m11 - e != 0.0 ) || ( m12 != 0.0 ) || ( m13 != 0.0 ) )
        {
            f1 = m11 - e; f2 = m12; f3 = m13;
        }
        else if( ( m12 != 0.0 ) || ( m22 - e != 0.0 ) || ( m23 != 0.0 ) )
        {
            f1 = m12; f2 = m22 - e; f3 = m23;
        }
        else if( ( m13 != 0.0 ) || ( m23 != 0.0 ) || ( m33 - e != 0.0 ) )
        {
            f1 = m13; f2 = m23; f3 = m33 - e;
        }
        else
        {
            // error, we'll just make something up - we have NO context
            f1 = 1.0; f2 = 0.0; f3 = 0.0;
        }

        if( f1 == 0.0 )
            vTmp = XMVectorSetX( vTmp, 0.0f );
        else
            vTmp = XMVectorSetX( vTmp, 1.0f );

        if( f2 == 0.0 )
            vTmp = XMVectorSetY( vTmp, 0.0f );
        else
            vTmp = XMVectorSetY( vTmp, 1.0f );

        if( f3 == 0.0 )
        {
            vTmp = XMVectorSetZ( vTmp, 0.0f );
            // recalculate y to make equation work
            if( m12 != 0.0 )
                vTmp = XMVectorSetY( vTmp, ( FLOAT )( -f1 / f2 ) );
        }
        else
        {
            vTmp = XMVectorSetZ( vTmp, ( FLOAT )( ( f2 - f1 ) / f3 ) );
        }
    }

    if( XMVectorGetX( XMVector3LengthSq( vTmp ) ) > 1e-5f )
    {
        return XMVector3Normalize( vTmp );
    }
    else
    {
        // Multiply by a value large enough to make the vector non-zero.
        vTmp *= 1e5f;
        return XMVector3Normalize( vTmp );
    }
}



//-----------------------------------------------------------------------------
static inline BOOL CalculateEigenVectors( FLOAT m11, FLOAT m12, FLOAT m13,
                                          FLOAT m22, FLOAT m23, FLOAT m33,
                                          FLOAT e1, FLOAT e2, FLOAT e3,
                                          XMVECTOR* pV1, XMVECTOR* pV2, XMVECTOR* pV3 )
{
    XMVECTOR vTmp, vUp, vRight;

    BOOL v1z, v2z, v3z, e12, e13, e23;

    vUp = XMVectorSetBinaryConstant( 0, 1, 0, 0 );
    vRight = XMVectorSetBinaryConstant( 1, 0, 0, 0 );

    *pV1 = CalculateEigenVector( m11, m12, m13, m22, m23, m33, e1 );
    *pV2 = CalculateEigenVector( m11, m12, m13, m22, m23, m33, e2 );
    *pV3 = CalculateEigenVector( m11, m12, m13, m22, m23, m33, e3 );

    v1z = v2z = v3z = FALSE;

    XMVECTOR Zero = XMVectorZero();

    if ( XMVector3Equal( *pV1, Zero ) )
        v1z = TRUE;

    if ( XMVector3Equal( *pV2, Zero ) )
        v2z = TRUE;

    if ( XMVector3Equal( *pV3, Zero ))
        v3z = TRUE;

    e12 = ( fabsf( XMVectorGetX( XMVector3Dot( *pV1, *pV2 ) ) ) > 0.1f ); // check for non-orthogonal vectors
    e13 = ( fabsf( XMVectorGetX( XMVector3Dot( *pV1, *pV3 ) ) ) > 0.1f );
    e23 = ( fabsf( XMVectorGetX( XMVector3Dot( *pV2, *pV3 ) ) ) > 0.1f );

    if( ( v1z && v2z && v3z ) || ( e12 && e13 && e23 ) ||
        ( e12 && v3z ) || ( e13 && v2z ) || ( e23 && v1z ) ) // all eigenvectors are 0- any basis set
    {
        *pV1 = XMVectorSetBinaryConstant( 1, 0, 0, 0 );
        *pV2 = XMVectorSetBinaryConstant( 0, 1, 0, 0 );
        *pV3 = XMVectorSetBinaryConstant( 0, 0, 1, 0 );
        return TRUE;
    }

    if( v1z && v2z )
    {
        vTmp = XMVector3Cross( vUp, *pV3 );
        if( XMVectorGetX( XMVector3LengthSq( vTmp ) ) < 1e-5f )
        {
            vTmp = XMVector3Cross( vRight, *pV3 );
        }
        *pV1 = XMVector3Normalize( vTmp );
        *pV2 = XMVector3Cross( *pV3, *pV1 );
        return TRUE;
    }

    if( v3z && v1z )
    {
        vTmp = XMVector3Cross( vUp, *pV2 );
        if( XMVectorGetX( XMVector3LengthSq( vTmp ) ) < 1e-5f )
        {
            vTmp = XMVector3Cross( vRight, *pV2 );
        }
        *pV3 = XMVector3Normalize( vTmp );
        *pV1 = XMVector3Cross( *pV2, *pV3 );
        return TRUE;
    }

    if( v2z && v3z )
    {
        vTmp = XMVector3Cross( vUp, *pV1 );
        if( XMVectorGetX( XMVector3LengthSq( vTmp ) ) < 1e-5f )
        {
            vTmp = XMVector3Cross( vRight, *pV1 );
        }
        *pV2 = XMVector3Normalize( vTmp );
        *pV3 = XMVector3Cross( *pV1, *pV2 );
        return TRUE;
    }

    if( ( v1z ) || e12 )
    {
        *pV1 = XMVector3Cross( *pV2, *pV3 );
        return TRUE;
    }

    if( ( v2z ) || e23 )
    {
        *pV2 = XMVector3Cross( *pV3, *pV1 );
        return TRUE;
    }

    if( ( v3z ) || e13 )
    {
        *pV3 = XMVector3Cross( *pV1, *pV2 );
        return TRUE;
    }

    return TRUE;
}



//-----------------------------------------------------------------------------
static inline BOOL CalculateEigenVectorsFromCovarianceMatrix( FLOAT Cxx, FLOAT Cyy, FLOAT Czz,
                                                              FLOAT Cxy, FLOAT Cxz, FLOAT Cyz,
                                                              XMVECTOR* pV1, XMVECTOR* pV2, XMVECTOR* pV3 )
{
    FLOAT e, f, g, ev1, ev2, ev3;

    // Calculate the eigenvalues by solving a cubic equation.
    e = -( Cxx + Cyy + Czz );
    f = Cxx * Cyy + Cyy * Czz + Czz * Cxx - Cxy * Cxy - Cxz * Cxz - Cyz * Cyz;
    g = Cxy * Cxy * Czz + Cxz * Cxz * Cyy + Cyz * Cyz * Cxx - Cxy * Cyz * Cxz * 2.0f - Cxx * Cyy * Czz;

    if( !SolveCubic( e, f, g, &ev1, &ev2, &ev3 ) )
    {
        // set them to arbitrary orthonormal basis set
        *pV1 = XMVectorSetBinaryConstant( 1, 0, 0, 0 );
        *pV2 = XMVectorSetBinaryConstant( 0, 1, 0, 0 );
        *pV3 = XMVectorSetBinaryConstant( 0, 0, 1, 0 );
        return FALSE;
    }

    return CalculateEigenVectors( Cxx, Cxy, Cxz, Cyy, Cyz, Czz, ev1, ev2, ev3, pV1, pV2, pV3 );
}

///////////////////////////////////////////////////////
//End Helpers
///////////////////////////////////////////////////////

void ConstructBoundingSphere(Sphere* pOutSphere, unsigned int count, const XMFLOAT3* pPoints, unsigned int stride)
{
	XMVECTOR minX, maxX, minY, maxY, minZ, maxZ;

	minX = maxX = minY = maxY = minZ = maxZ = XMLoadFloat3(pPoints);

	for(unsigned int i = 1; i < count; ++i)
	{
		XMVECTOR point = XMLoadFloat3((XMFLOAT3*)((unsigned char*)pPoints + i * stride));

		float px = XMVectorGetX(point);
		float py = XMVectorGetY(point);
		float pz = XMVectorGetZ(point);

		if(px < XMVectorGetX(minX))
			minX = point;

		if(px > XMVectorGetX(maxX))
			maxX = point;

		if(py < XMVectorGetY(minY))
			minY = point;

		if(py > XMVectorGetY(maxY))
			maxY = point;

		if(pz < XMVectorGetZ(minZ))
			minZ = point;

		if(pz > XMVectorGetZ(maxZ))
			maxZ = point;
	}

	//use the min/max pair with greatest distance to form the sphere
	XMVECTOR deltaX = maxX - minX;
	XMVECTOR distanceX = XMVector3Length(deltaX);

	XMVECTOR deltaY = maxY - minY;
	XMVECTOR distanceY = XMVector3Length(deltaY);

	XMVECTOR deltaZ = maxZ - minZ;
	XMVECTOR distanceZ = XMVector3Length(deltaZ);

	XMVECTOR center;
	XMVECTOR radius;

	if(XMVector3Greater(distanceX, distanceY))
	{
		if(XMVector3Greater(distanceX, distanceZ))
		{
			center = (maxX + minX) * 0.5f;
			radius = distanceX * 0.5f;
		}
		else
		{
			center = (maxZ + minZ) * 0.05f;
			radius = distanceZ * 0.5f;
		}
	}

	else
	{
		if(XMVector3Greater(distanceY, distanceZ))
		{
			center = (maxY + minY) * 0.5f;
			radius = distanceY * 0.5f;
		}
		else
		{
			center = (maxZ + minZ) * 0.5f;
			radius = distanceZ * 0.5f;
		}
	}

	for(unsigned int i = 0; i < count; ++i)
	{
		XMVECTOR point = XMLoadFloat3((XMFLOAT3*)((unsigned char*)pPoints + i * stride));
		XMVECTOR delta = point - center;
		XMVECTOR distance= XMVector3Length(delta);

		if(XMVector3Greater(distance, radius))
		{
			radius = (radius + distance) * 0.5f;
			center += (XMVectorReplicate(1.0f) - radius * XMVectorReciprocal(distance)) * delta;
		}
	}

	XMStoreFloat3(&pOutSphere->mPosition, center);
	XMStoreFloat(&pOutSphere->mRadius, radius);
	Log::Inform("Created Bounding Sphere", "Sphere", 0);
}

void ConstructAABB(AABB* pOutAABB, unsigned int count, const XMFLOAT3* pPoints, unsigned int stride)
{
	XMVECTOR vMin, vMax;

	vMin = vMax = XMLoadFloat3(pPoints);

	for(unsigned int i = 1; i < count; ++i)
	{
		XMVECTOR point = XMLoadFloat3((XMFLOAT3*)((unsigned char*)pPoints + i * stride));

		vMin = XMVectorMin(vMin, point);
		vMax = XMVectorMax(vMax, point);
	}

	XMStoreFloat3(&pOutAABB->mCenter, (vMin + vMax) * 0.5f);
	XMStoreFloat3(&pOutAABB->mExtents, (vMax - vMin) * 0.5f);
	Log::Inform("Create AABB", "AABB", 0);
}

void ConstructOABB(OABB* pOutOABB, unsigned int count, const XMFLOAT3* pPoints, unsigned stride)
{
	static const XMVECTORI32 PermuteXXY = 
	{
		XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W
	};

	static const XMVECTORI32 PermuteYZZ =
	{
		XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0W
	};

	XMVECTOR centerOfMass = XMVectorZero();

	for(unsigned int i = 0; i < count; ++i)
	{
		XMVECTOR point = XMLoadFloat3((XMFLOAT3*)((unsigned char*)pPoints + i * stride));
		centerOfMass += point;
	}

	centerOfMass *= XMVectorReciprocal(XMVectorReplicate(float(count)));

	XMVECTOR XX_YY_ZZ = XMVectorZero();
	XMVECTOR XY_XZ_YZ = XMVectorZero();

	for(unsigned int i = 0; i < count; ++i)
	{
		XMVECTOR point = XMLoadFloat3((XMFLOAT3*)((unsigned char*)pPoints + i * stride)) - centerOfMass;

		XX_YY_ZZ += point * point;

		XMVECTOR XXY = XMVectorPermute(point, point, PermuteXXY);
		XMVECTOR YZZ = XMVectorPermute(point, point, PermuteYZZ);

		XY_XZ_YZ += XXY * YZZ;
	}

	XMVECTOR v1, v2, v3;

	CalculateEigenVectorsFromCovarianceMatrix(XMVectorGetX(XX_YY_ZZ), XMVectorGetY(XX_YY_ZZ),
		XMVectorGetZ(XX_YY_ZZ),
		XMVectorGetX(XY_XZ_YZ), XMVectorGetY(XY_XZ_YZ),
		XMVectorGetZ(XY_XZ_YZ),
		&v1, &v2, &v3);

	XMMATRIX R;
	R.r[0] = XMVectorSetW(v1, 0.0f);
	R.r[1] = XMVectorSetW(v2, 0.0f);
	R.r[2] = XMVectorSetW(v3, 0.0f);
	R.r[3] = XMVectorSetBinaryConstant(0, 0, 0, 1);

	//convert to right handed coordinate system
	XMVECTOR determinant = XMMatrixDeterminant(R);

	if(XMVector4Less(determinant, XMVectorZero()))
	{
		const XMVECTORF32 vectorNegativeOne = 
		{
			-1.0f, -1.0f, -1.0f, -1.0f
		};

		R.r[0] *= vectorNegativeOne;
		R.r[1] *= vectorNegativeOne;
		R.r[2] *= vectorNegativeOne;
	}

	//rotation quaternion
	XMVECTOR orientation = XMQuaternionRotationMatrix(R);

	orientation = XMQuaternionNormalize(orientation);

	R = XMMatrixRotationQuaternion(orientation);

	XMMATRIX inverseR = XMMatrixTranspose(R);

	XMVECTOR vMin, vMax;

	vMin = vMax = XMVector3TransformNormal(XMLoadFloat3(pPoints), inverseR);

	for(unsigned int i = 1; i < count; ++i)
	{
		XMVECTOR point = XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)((unsigned char*)pPoints + i * stride)),
			inverseR);

		vMin = XMVectorMin(vMin, point);
		vMax = XMVectorMax(vMax, point);
	}

	XMVECTOR center = (vMin + vMax) * 0.05f;
	center = XMVector3TransformNormal(center, R);

	XMStoreFloat3(&pOutOABB->mCenter, center);
	XMStoreFloat3(&pOutOABB->mExtents, (vMax - vMin) * 0.05f);
	XMStoreFloat4(&pOutOABB->mOrientation, orientation);
}

void ConstructFrustumFromProjection(Frustum* pOutFrustum, XMFLOAT4X4* pProjection)
{
	//coners of projection frustum in homogenouse space
	static XMVECTOR HomogenousPoints[6] = 
	{
		{1.0f, 0.0f, 1.0f, 1.0f},
		{-1.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f}
	};

	XMMATRIX projection = XMLoadFloat4x4(pProjection);

	XMVECTOR determinant;
	XMMATRIX matrixInverse = XMMatrixInverse(&determinant, projection);

	XMVECTOR points[6];

	for(int i = 0; i < 6; ++i)
	{
		points[i] = XMVector4Transform(HomogenousPoints[i], matrixInverse);
	}

	pOutFrustum->mOrigin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pOutFrustum->mOrientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	points[0] = points[0] * XMVectorReciprocal(XMVectorSplatZ(points[0]));
	points[1] = points[1] * XMVectorReciprocal(XMVectorSplatZ(points[1]));
	points[2] = points[2] * XMVectorReciprocal(XMVectorSplatZ(points[2]));
	points[3] = points[3] * XMVectorReciprocal(XMVectorSplatZ(points[3]));

	pOutFrustum->mRightSlope = XMVectorGetX( points[0] );
    pOutFrustum->mLeftSlope = XMVectorGetX( points[1] );
    pOutFrustum->mTopSlope = XMVectorGetY( points[2] );
    pOutFrustum->mBottomSlope = XMVectorGetY( points[3] );

    // Compute near and far.
    points[4] = points[4] * XMVectorReciprocal( XMVectorSplatW( points[4] ) );
    points[5] = points[5] * XMVectorReciprocal( XMVectorSplatW( points[5] ) );

    pOutFrustum->mNear = XMVectorGetZ( points[4] );
    pOutFrustum->mFar = XMVectorGetZ( points[5] );
}

//collision detection
//point collisions
bool IntersectPointSphere(const XMFLOAT3 pPoint, const Sphere* pVolume)
{
	XMVECTOR center = XMLoadFloat3(&pVolume->mPosition);
	XMVECTOR radius = XMVectorReplicatePtr(&pVolume->mRadius);
	XMVECTOR point = XMLoadFloat3(&pPoint);

	XMVECTOR distanceSquared = XMVector3LengthSq(point - center);
	XMVECTOR radiusSquared = radius * radius;

	return XMVector4LessOrEqual(distanceSquared, radiusSquared) == TRUE;
}

bool IntersectPointAABB(const XMFLOAT3 pPoint, const AABB* pVolume)
{
	XMVECTOR center = XMLoadFloat3(&pVolume->mCenter);
	XMVECTOR extents = XMLoadFloat3(&pVolume->mExtents);
	XMVECTOR point = XMLoadFloat3(&pPoint);

	return XMVector3InBounds(point = center, extents) == TRUE;
}

bool IntersectPointOABB(const XMFLOAT3 pPoint, const OABB* pVolume)
{
	XMVECTOR center = XMLoadFloat3(&pVolume->mCenter);
	XMVECTOR extents = XMLoadFloat3(&pVolume->mExtents);
	XMVECTOR orientation = XMLoadFloat4(&pVolume->mOrientation);
	XMVECTOR point = XMLoadFloat3(&pPoint);

	XMVECTOR tPoint = XMVector3InverseRotate(point = center, orientation);
	return XMVector3InBounds(tPoint, extents) == TRUE;
}

bool IntersectPointFrustum(const XMFLOAT3 pPoint, const Frustum* pVolume)
{
	static const XMVECTORU32 selectW = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_0, XM_SELECT_1};
	static const XMVECTORU32 selectZ = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};

	static const XMVECTOR basePlanes[6] = 
	{
		{0.0f, 0.0f, -1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, -1.0f, 0.0f, 0.0f},
	};

	XMVECTOR planes[6];
	planes[0] = XMVectorSelect(basePlanes[0], XMVectorSplatX(XMLoadFloat(&pVolume->mNear)), selectW);
	planes[1] = XMVectorSelect(basePlanes[1], XMVectorSplatX(XMLoadFloat(&pVolume->mFar)), selectW);
	planes[2] = XMVectorSelect(basePlanes[2], XMVectorSplatX(XMLoadFloat(&pVolume->mRightSlope)), selectZ);
	planes[3] = XMVectorSelect(basePlanes[3], XMVectorSplatX(XMLoadFloat(&pVolume->mLeftSlope)), selectZ);
	planes[4] = XMVectorSelect(basePlanes[4], XMVectorSplatX(XMLoadFloat(&pVolume->mTopSlope)), selectZ);
	planes[5] = XMVectorSelect(basePlanes[5], XMVectorSplatX(XMLoadFloat(&pVolume->mBottomSlope)), selectZ);

	XMVECTOR origin = XMLoadFloat3(&pVolume->mOrigin);
	XMVECTOR orientation = XMLoadFloat4(&pVolume->mOrientation);
	XMVECTOR point = XMLoadFloat3(&pPoint);

	XMVECTOR tPoint = XMVector3InverseRotate(point - origin, orientation);

	tPoint = XMVectorInsert(tPoint, XMVectorSplatOne(), 0, 0, 0, 0, 1);

	XMVECTOR zero = XMVectorZero();
	XMVECTOR outside = zero;

	for(int i = 0; i < 6; ++i)
	{
		XMVECTOR dot = XMVector4Dot(tPoint, planes[i]);
		outside = XMVectorOrInt(outside, XMVectorGreater(dot, zero));
	}

	return XMVector4NotEqualInt(outside, XMVectorTrueInt()) == TRUE;
}

//ray collisions
bool IntersectRayTriangle(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, const XMFLOAT3 & v0, 
	const XMFLOAT3 & v1, XMFLOAT3 & v2, float* pDistance)
{
	static const XMVECTOR epsilon = 
	{
		1e-20f, 1e-20f, 1e-20f, 1e-20f,
	};

	XMVECTOR zero = XMVectorZero();
	XMVECTOR V0, V1, V2;
	V0 = XMLoadFloat3(&v0);
	V1 = XMLoadFloat3(&v1);
	V2 = XMLoadFloat3(&v2);
	XMVECTOR origin = XMLoadFloat3(&pOrigin);
	XMVECTOR direction = XMLoadFloat3(&pDirection);

	XMVECTOR e1 = V1 - V0;
	XMVECTOR e2 = V2 - V0;

	XMVECTOR p = XMVector3Cross(direction, e2);
	XMVECTOR det = XMVector3Dot(e1, p);

	XMVECTOR u, v, t;

	if(XMVector3GreaterOrEqual(det, epsilon))
	{
		XMVECTOR s = origin - V0;
		u = XMVector3Dot(s, p);

		XMVECTOR noIntersection = XMVectorLess(u, zero);
		noIntersection = XMVectorOrInt(noIntersection, XMVectorGreater(u, det));

		XMVECTOR q = XMVector3Cross(s, e1);

		v = XMVector3Dot(direction, q);

		noIntersection = XMVectorOrInt(noIntersection, XMVectorLess(v, zero));
		noIntersection = XMVectorOrInt(noIntersection, XMVectorGreater(u + v, det));

		t = XMVector3Dot(e2, q);

		noIntersection = XMVectorOrInt(noIntersection, XMVectorLess(t, zero));

		if(XMVector4EqualInt(noIntersection, XMVectorTrueInt()))
			return false;
	}

	else if(XMVector3LessOrEqual(det, -epsilon))
	{
		XMVECTOR s = origin - V0;

		u = XMVector3Dot(s, p);

		XMVECTOR noIntersection = XMVectorGreater(u, zero);
		noIntersection = XMVectorOrInt(noIntersection, XMVectorLess(u, det));

		XMVECTOR q = XMVector3Cross(s, e1);

		v = XMVector3Dot(direction, q);

		noIntersection = XMVectorOrInt(noIntersection, XMVectorGreater(v, zero));
		noIntersection = XMVectorOrInt(noIntersection, XMVectorLess(u + v, det));

		t = XMVector3Dot(e2, q);

		noIntersection = XMVectorOrInt(noIntersection, XMVectorGreater(t, zero));

		if(XMVector4EqualInt(noIntersection, XMVectorTrueInt()))
			return false;
	}
	else
	{
		return false;
	}

	XMVECTOR inv_det = XMVectorReciprocal(det);

	t *= inv_det;

	XMStoreFloat(pDistance, t);

	return true;
}

bool IntersectRayAABB(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, AABB* pVolume, float* pDistance)
{
	  static const XMVECTOR Epsilon =
    {
        1e-20f, 1e-20f, 1e-20f, 1e-20f
    };
    static const XMVECTOR FltMin =
    {
        -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX
    };
    static const XMVECTOR FltMax =
    {
        FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX
    };

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->mCenter );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->mExtents );
	XMVECTOR Origin = XMLoadFloat3(&pOrigin);
	XMVECTOR Direction = XMLoadFloat3(&pDirection);

    // Adjust ray origin to be relative to center of the box.
    XMVECTOR TOrigin = Center - Origin;

    // Compute the dot product againt each axis of the box.
    // Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
    XMVECTOR AxisDotOrigin = TOrigin;
    XMVECTOR AxisDotDirection = Direction;

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual( XMVectorAbs( AxisDotDirection ), Epsilon );

    // Test against all three axii simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal( AxisDotDirection );
    XMVECTOR t1 = ( AxisDotOrigin - Extents ) * InverseAxisDotDirection;
    XMVECTOR t2 = ( AxisDotOrigin + Extents ) * InverseAxisDotDirection;

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect( XMVectorMin( t1, t2 ), FltMin, IsParallel );
    XMVECTOR t_max = XMVectorSelect( XMVectorMax( t1, t2 ), FltMax, IsParallel );

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    t_min = XMVectorMax( t_min, XMVectorSplatY( t_min ) );  // x = max(x,y)
    t_min = XMVectorMax( t_min, XMVectorSplatZ( t_min ) );  // x = max(max(x,y),z)
    t_max = XMVectorMin( t_max, XMVectorSplatY( t_max ) );  // x = min(x,y)
    t_max = XMVectorMin( t_max, XMVectorSplatZ( t_max ) );  // x = min(min(x,y),z)

    // if ( t_min > t_max ) return FALSE;
    XMVECTOR NoIntersection = XMVectorGreater( XMVectorSplatX( t_min ), XMVectorSplatX( t_max ) );

    // if ( t_max < 0.0f ) return FALSE;
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( XMVectorSplatX( t_max ), XMVectorZero() ) );

    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return FALSE;
    XMVECTOR ParallelOverlap = XMVectorInBounds( AxisDotOrigin, Extents );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorAndCInt( IsParallel, ParallelOverlap ) );

    if( !XMVector3AnyTrue( NoIntersection ) )
    {
        // Store the x-component to *pDist
        XMStoreFloat( pDistance, t_min );
        return true;
    }

    return false;
}

bool IntersectRayOABB(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, OABB* pVolume, float* pDistance)
{
	    static const XMVECTOR Epsilon =
    {
        1e-20f, 1e-20f, 1e-20f, 1e-20f
    };
    static const XMVECTOR FltMin =
    {
        -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX
    };
    static const XMVECTOR FltMax =
    {
        FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX
    };
    static const XMVECTORI32 SelectY =
    {
        XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0
    };
    static const XMVECTORI32 SelectZ =
    {
        XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0
    };

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->mCenter );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->mExtents );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->mOrientation );
	XMVECTOR Origin = XMLoadFloat3(&pOrigin);
	XMVECTOR Direction = XMLoadFloat3(&pDirection);

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Get the boxes normalized side directions.
    XMMATRIX R = XMMatrixRotationQuaternion( Orientation );

    // Adjust ray origin to be relative to center of the box.
    XMVECTOR TOrigin = Center - Origin;

    // Compute the dot product againt each axis of the box.
    XMVECTOR AxisDotOrigin = XMVector3Dot( R.r[0], TOrigin );
    AxisDotOrigin = XMVectorSelect( AxisDotOrigin, XMVector3Dot( R.r[1], TOrigin ), SelectY );
    AxisDotOrigin = XMVectorSelect( AxisDotOrigin, XMVector3Dot( R.r[2], TOrigin ), SelectZ );

    XMVECTOR AxisDotDirection = XMVector3Dot( R.r[0], Direction );
    AxisDotDirection = XMVectorSelect( AxisDotDirection, XMVector3Dot( R.r[1], Direction ), SelectY );
    AxisDotDirection = XMVectorSelect( AxisDotDirection, XMVector3Dot( R.r[2], Direction ), SelectZ );

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual( XMVectorAbs( AxisDotDirection ), Epsilon );

    // Test against all three axes simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal( AxisDotDirection );
    XMVECTOR t1 = ( AxisDotOrigin - Extents ) * InverseAxisDotDirection;
    XMVECTOR t2 = ( AxisDotOrigin + Extents ) * InverseAxisDotDirection;

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect( XMVectorMin( t1, t2 ), FltMin, IsParallel );
    XMVECTOR t_max = XMVectorSelect( XMVectorMax( t1, t2 ), FltMax, IsParallel );

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    t_min = XMVectorMax( t_min, XMVectorSplatY( t_min ) );  // x = max(x,y)
    t_min = XMVectorMax( t_min, XMVectorSplatZ( t_min ) );  // x = max(max(x,y),z)
    t_max = XMVectorMin( t_max, XMVectorSplatY( t_max ) );  // x = min(x,y)
    t_max = XMVectorMin( t_max, XMVectorSplatZ( t_max ) );  // x = min(min(x,y),z)

    // if ( t_min > t_max ) return FALSE;
    XMVECTOR NoIntersection = XMVectorGreater( XMVectorSplatX( t_min ), XMVectorSplatX( t_max ) );

    // if ( t_max < 0.0f ) return FALSE;
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( XMVectorSplatX( t_max ), XMVectorZero() ) );

    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return FALSE;
    XMVECTOR ParallelOverlap = XMVectorInBounds( AxisDotOrigin, Extents );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorAndCInt( IsParallel, ParallelOverlap ) );

    if( !XMVector3AnyTrue( NoIntersection ) )
    {
        // Store the x-component to *pDist
        XMStoreFloat( pDistance, t_min );
        return true;
    }

    return false;
}

bool IntersectRaySphere(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, Sphere* pVolume, float* pDistance)
{
	XMVECTOR center = XMLoadFloat3(&pVolume->mPosition);
	XMVECTOR radius = XMVectorReplicatePtr(&pVolume->mRadius);
	XMVECTOR origin = XMLoadFloat3(&pOrigin);
	XMVECTOR direction = XMLoadFloat3(&pDirection);

	XMVECTOR l = center - origin;

	XMVECTOR s = XMVector3Dot(l, direction);
	XMVECTOR l2 = XMVector3Dot(l, l);
	XMVECTOR r2 = radius * radius;

	XMVECTOR m2 = l2 - s * s;

	XMVECTOR noIntersection;

	noIntersection = XMVectorAndInt(XMVectorLess(s, XMVectorZero()), XMVectorGreater(l2, r2));
	noIntersection = XMVectorOrInt(noIntersection, XMVectorGreater(m2, r2));

	XMVECTOR q = XMVectorSqrt(r2 - m2);
	XMVECTOR t1 = s - q;
	XMVECTOR t2 = s + q;

	XMVECTOR originInside = XMVectorLessOrEqual(l2, r2);
	XMVECTOR t = XMVectorSelect(t1, t2, originInside);

	if(XMVector4NotEqualInt(noIntersection, XMVectorTrueInt()))
	{
		XMStoreFloat(pDistance, t);
		return true;
	}
	return false;
}

//triangle collision detection (vertices)
bool IntersectTriangleTriangle(const XMFLOAT3 a0, const XMFLOAT3 a1, const XMFLOAT3 a2, const XMFLOAT3 & b0, const XMFLOAT3 & b1, const XMFLOAT3 & b2 )
{
	 static const XMVECTOR Epsilon =
    {
        1e-20f, 1e-20f, 1e-20f, 1e-20f
    };
    static const XMVECTORI32 SelectY =
    {
        XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0
    };
    static const XMVECTORI32 SelectZ =
    {
        XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0
    };
    static const XMVECTORI32 Select0111 =
    {
        XM_SELECT_0, XM_SELECT_1, XM_SELECT_1, XM_SELECT_1
    };
    static const XMVECTORI32 Select1011 =
    {
        XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_1
    };
    static const XMVECTORI32 Select1101 =
    {
        XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1
    };

    XMVECTOR Zero = XMVectorZero();
	XMVECTOR A1, A0, A2;
	XMVECTOR B0, B1, B2;
	A1 = XMLoadFloat3(&a1);
	A0 = XMLoadFloat3(&a0);
	A2 = XMLoadFloat3(&a2);

	B1 = XMLoadFloat3(&b1);
	B0 = XMLoadFloat3(&b0);
	B2 = XMLoadFloat3(&b2);

    // Compute the normal of triangle A.
    XMVECTOR N1 = XMVector3Cross( A1 - A0, A2 - A0 );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( N1, Zero ) );

    // Test points of B against the plane of A.
    XMVECTOR BDist = XMVector3Dot( N1, B0 - A0 );
    BDist = XMVectorSelect( BDist, XMVector3Dot( N1, B1 - A0 ), SelectY );
    BDist = XMVectorSelect( BDist, XMVector3Dot( N1, B2 - A0 ), SelectZ );

    // Ensure robustness with co-planar triangles by zeroing small distances.
    UINT BDistIsZeroCR;
    XMVECTOR BDistIsZero = XMVectorGreaterR( &BDistIsZeroCR, Epsilon, XMVectorAbs( BDist ) );
    BDist = XMVectorSelect( BDist, Zero, BDistIsZero );

    UINT BDistIsLessCR;
    XMVECTOR BDistIsLess = XMVectorGreaterR( &BDistIsLessCR, Zero, BDist );

    UINT BDistIsGreaterCR;
    XMVECTOR BDistIsGreater = XMVectorGreaterR( &BDistIsGreaterCR, BDist, Zero );

    // If all the points are on the same side we don't intersect.
    if( XMComparisonAllTrue( BDistIsLessCR ) || XMComparisonAllTrue( BDistIsGreaterCR ) )
        return FALSE;

    // Compute the normal of triangle B.
    XMVECTOR N2 = XMVector3Cross( B1 - B0, B2 - B0 );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( N2, Zero ) );

    // Test points of A against the plane of B.
    XMVECTOR ADist = XMVector3Dot( N2, A0 - B0 );
    ADist = XMVectorSelect( ADist, XMVector3Dot( N2, A1 - B0 ), SelectY );
    ADist = XMVectorSelect( ADist, XMVector3Dot( N2, A2 - B0 ), SelectZ );

    // Ensure robustness with co-planar triangles by zeroing small distances.
    UINT ADistIsZeroCR;
    XMVECTOR ADistIsZero = XMVectorGreaterR( &ADistIsZeroCR, Epsilon, XMVectorAbs( BDist ) );
    ADist = XMVectorSelect( ADist, Zero, ADistIsZero );

    UINT ADistIsLessCR;
    XMVECTOR ADistIsLess = XMVectorGreaterR( &ADistIsLessCR, Zero, ADist );

    UINT ADistIsGreaterCR;
    XMVECTOR ADistIsGreater = XMVectorGreaterR( &ADistIsGreaterCR, ADist, Zero );

    // If all the points are on the same side we don't intersect.
    if( XMComparisonAllTrue( ADistIsLessCR ) || XMComparisonAllTrue( ADistIsGreaterCR ) )
        return FALSE;

    // Special case for co-planar triangles.
    if( XMComparisonAllTrue( ADistIsZeroCR ) || XMComparisonAllTrue( BDistIsZeroCR ) )
    {
        XMVECTOR Axis, Dist, MinDist;

        // Compute an axis perpindicular to the edge (points out).
        Axis = XMVector3Cross( N1, A1 - A0 );
        Dist = XMVector3Dot( Axis, A0 );

        // Test points of B against the axis.
        MinDist = XMVector3Dot( B0, Axis );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( B1, Axis ) );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( B2, Axis ) );
        if( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return FALSE;

        // Edge (A1, A2)
        Axis = XMVector3Cross( N1, A2 - A1 );
        Dist = XMVector3Dot( Axis, A1 );

        MinDist = XMVector3Dot( B0, Axis );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( B1, Axis ) );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( B2, Axis ) );
        if( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return false;

        // Edge (A2, A0)
        Axis = XMVector3Cross( N1, A0 - A2 );
        Dist = XMVector3Dot( Axis, A2 );

        MinDist = XMVector3Dot( B0, Axis );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( B1, Axis ) );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( B2, Axis ) );
        if( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return false;

        // Edge (B0, B1)
        Axis = XMVector3Cross( N2, B1 - B0 );
        Dist = XMVector3Dot( Axis, B0 );

        MinDist = XMVector3Dot( A0, Axis );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( A1, Axis ) );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( A2, Axis ) );
        if( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return false;

        // Edge (B1, B2)
        Axis = XMVector3Cross( N2, B2 - B1 );
        Dist = XMVector3Dot( Axis, B1 );

        MinDist = XMVector3Dot( A0, Axis );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( A1, Axis ) );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( A2, Axis ) );
        if( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return false;

        // Edge (B2,B0)
        Axis = XMVector3Cross( N2, B0 - B2 );
        Dist = XMVector3Dot( Axis, B2 );

        MinDist = XMVector3Dot( A0, Axis );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( A1, Axis ) );
        MinDist = XMVectorMin( MinDist, XMVector3Dot( A2, Axis ) );
        if( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return false;

        return true;
    }

    //
    // Find the single vertex of A and B (ie the vertex on the opposite side
    // of the plane from the other two) and reorder the edges so we can compute 
    // the signed edge/edge distances.
    //
    // if ( (V0 >= 0 && V1 <  0 && V2 <  0) ||
    //      (V0 >  0 && V1 <= 0 && V2 <= 0) ||
    //      (V0 <= 0 && V1 >  0 && V2 >  0) ||
    //      (V0 <  0 && V1 >= 0 && V2 >= 0) ) then V0 is singular;
    //
    // If our singular vertex is not on the positive side of the plane we reverse
    // the triangle winding so that the overlap comparisons will compare the 
    // correct edges with the correct signs.
    //
    XMVECTOR ADistIsLessEqual = XMVectorOrInt( ADistIsLess, ADistIsZero );
    XMVECTOR ADistIsGreaterEqual = XMVectorOrInt( ADistIsGreater, ADistIsZero );

    XMVECTOR AA0, AA1, AA2;
    bool bPositiveA;

    if( XMVector3AllTrue( XMVectorSelect( ADistIsGreaterEqual, ADistIsLess, Select0111 ) ) ||
        XMVector3AllTrue( XMVectorSelect( ADistIsGreater, ADistIsLessEqual, Select0111 ) ) )
    {
        // A0 is singular, crossing from positive to negative.
        AA0 = A0; AA1 = A1; AA2 = A2;
        bPositiveA = true;
    }
    else if( XMVector3AllTrue( XMVectorSelect( ADistIsLessEqual, ADistIsGreater, Select0111 ) ) ||
             XMVector3AllTrue( XMVectorSelect( ADistIsLess, ADistIsGreaterEqual, Select0111 ) ) )
    {
        // A0 is singular, crossing from negative to positive.
        AA0 = A0; AA1 = A2; AA2 = A1;
        bPositiveA = false;
    }
    else if( XMVector3AllTrue( XMVectorSelect( ADistIsGreaterEqual, ADistIsLess, Select1011 ) ) ||
             XMVector3AllTrue( XMVectorSelect( ADistIsGreater, ADistIsLessEqual, Select1011 ) ) )
    {
        // A1 is singular, crossing from positive to negative.
        AA0 = A1; AA1 = A2; AA2 = A0;
        bPositiveA = true;
    }
    else if( XMVector3AllTrue( XMVectorSelect( ADistIsLessEqual, ADistIsGreater, Select1011 ) ) ||
             XMVector3AllTrue( XMVectorSelect( ADistIsLess, ADistIsGreaterEqual, Select1011 ) ) )
    {
        // A1 is singular, crossing from negative to positive.
        AA0 = A1; AA1 = A0; AA2 = A2;
        bPositiveA = false;
    }
    else if( XMVector3AllTrue( XMVectorSelect( ADistIsGreaterEqual, ADistIsLess, Select1101 ) ) ||
             XMVector3AllTrue( XMVectorSelect( ADistIsGreater, ADistIsLessEqual, Select1101 ) ) )
    {
        // A2 is singular, crossing from positive to negative.
        AA0 = A2; AA1 = A0; AA2 = A1;
        bPositiveA = true;
    }
    else if( XMVector3AllTrue( XMVectorSelect( ADistIsLessEqual, ADistIsGreater, Select1101 ) ) ||
             XMVector3AllTrue( XMVectorSelect( ADistIsLess, ADistIsGreaterEqual, Select1101 ) ) )
    {
        // A2 is singular, crossing from negative to positive.
        AA0 = A2; AA1 = A1; AA2 = A0;
        bPositiveA = false;
    }
    else
    {
        XMASSERT( FALSE );
        return false;
    }

    XMVECTOR BDistIsLessEqual = XMVectorOrInt( BDistIsLess, BDistIsZero );
    XMVECTOR BDistIsGreaterEqual = XMVectorOrInt( BDistIsGreater, BDistIsZero );

    XMVECTOR BB0, BB1, BB2;
    bool bPositiveB;

    if( XMVector3AllTrue( XMVectorSelect( BDistIsGreaterEqual, BDistIsLess, Select0111 ) ) ||
        XMVector3AllTrue( XMVectorSelect( BDistIsGreater, BDistIsLessEqual, Select0111 ) ) )
    {
        // B0 is singular, crossing from positive to negative.
        BB0 = B0; BB1 = B1; BB2 = B2;
        bPositiveB = true;
    }
    else if( XMVector3AllTrue( XMVectorSelect( BDistIsLessEqual, BDistIsGreater, Select0111 ) ) ||
             XMVector3AllTrue( XMVectorSelect( BDistIsLess, BDistIsGreaterEqual, Select0111 ) ) )
    {
        // B0 is singular, crossing from negative to positive.
        BB0 = B0; BB1 = B2; BB2 = B1;
        bPositiveB = false;
    }
    else if( XMVector3AllTrue( XMVectorSelect( BDistIsGreaterEqual, BDistIsLess, Select1011 ) ) ||
             XMVector3AllTrue( XMVectorSelect( BDistIsGreater, BDistIsLessEqual, Select1011 ) ) )
    {
        // B1 is singular, crossing from positive to negative.
        BB0 = B1; BB1 = B2; BB2 = B0;
        bPositiveB = true;
    }
    else if( XMVector3AllTrue( XMVectorSelect( BDistIsLessEqual, BDistIsGreater, Select1011 ) ) ||
             XMVector3AllTrue( XMVectorSelect( BDistIsLess, BDistIsGreaterEqual, Select1011 ) ) )
    {
        // B1 is singular, crossing from negative to positive.
        BB0 = B1; BB1 = B0; BB2 = B2;
        bPositiveB = false;
    }
    else if( XMVector3AllTrue( XMVectorSelect( BDistIsGreaterEqual, BDistIsLess, Select1101 ) ) ||
             XMVector3AllTrue( XMVectorSelect( BDistIsGreater, BDistIsLessEqual, Select1101 ) ) )
    {
        // B2 is singular, crossing from positive to negative.
        BB0 = B2; BB1 = B0; BB2 = B1;
        bPositiveB = true;
    }
    else if( XMVector3AllTrue( XMVectorSelect( BDistIsLessEqual, BDistIsGreater, Select1101 ) ) ||
             XMVector3AllTrue( XMVectorSelect( BDistIsLess, BDistIsGreaterEqual, Select1101 ) ) )
    {
        // B2 is singular, crossing from negative to positive.
        BB0 = B2; BB1 = B1; BB2 = B0;
        bPositiveB = false;
    }
    else
    {
        return false;
    }

    XMVECTOR Delta0, Delta1;

    // Reverse the direction of the test depending on whether the singular vertices are
    // the same sign or different signs.
    if( bPositiveA ^ bPositiveB )
    {
        Delta0 = ( BB0 - AA0 );
        Delta1 = ( AA0 - BB0 );
    }
    else
    {
        Delta0 = ( AA0 - BB0 );
        Delta1 = ( BB0 - AA0 );
    }

    // Check if the triangles overlap on the line of intersection between the
    // planes of the two triangles by finding the signed line distances.
    XMVECTOR Dist0 = XMVector3Dot( Delta0, XMVector3Cross( ( BB2 - BB0 ), ( AA2 - AA0 ) ) );
    if( XMVector4Greater( Dist0, Zero ) )
        return false;

    XMVECTOR Dist1 = XMVector3Dot( Delta1, XMVector3Cross( ( BB1 - BB0 ), ( AA1 - AA0 ) ) );
    if( XMVector4Greater( Dist1, Zero ) )
        return false;

    return true;
}

bool IntersectTriangleSphere(const XMFLOAT3 p0, const XMFLOAT3 p1, const XMFLOAT3 p2, Sphere* pVolume)
{
	XMVECTOR V0, V1, V2;
	V0 = XMLoadFloat3(&p0);
	V1 = XMLoadFloat3(&p1);
	V2 = XMLoadFloat3(&p2);

	// Load the sphere.    
    XMVECTOR Center = XMLoadFloat3( &pVolume->mPosition );
    XMVECTOR Radius = XMVectorReplicatePtr( &pVolume->mRadius );

    // Compute the plane of the triangle (has to be normalized).
    XMVECTOR N = XMVector3Normalize( XMVector3Cross( V1 - V0, V2 - V0 ) );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( N, XMVectorZero() ) );

    // Find the nearest feature on the triangle to the sphere.
    XMVECTOR Dist = XMVector3Dot( Center - V0, N );

    // If the center of the sphere is farther from the plane of the triangle than
    // the radius of the sphere, then there cannot be an intersection.
    XMVECTOR NoIntersection = XMVectorLess( Dist, -Radius );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Dist, Radius ) );

    // Project the center of the sphere onto the plane of the triangle.
    XMVECTOR Point = Center - ( N * Dist );

    // Is it inside all the edges? If so we intersect because the distance 
    // to the plane is less than the radius.
    XMVECTOR Intersection = PointOnPlaneInsideTriangle( Point, V0, V1, V2 );

    // Find the nearest point on each edge.
    XMVECTOR RadiusSq = Radius * Radius;

    // Edge 0,1
    Point = PointOnLineSegmentNearestPoint( V0, V1, Center );

    // If the distance to the center of the sphere to the point is less than 
    // the radius of the sphere then it must intersect.
    Intersection = XMVectorOrInt( Intersection, XMVectorLessOrEqual( XMVector3LengthSq( Center - Point ), RadiusSq ) );

    // Edge 1,2
    Point = PointOnLineSegmentNearestPoint( V1, V2, Center );

    // If the distance to the center of the sphere to the point is less than 
    // the radius of the sphere then it must intersect.
    Intersection = XMVectorOrInt( Intersection, XMVectorLessOrEqual( XMVector3LengthSq( Center - Point ), RadiusSq ) );

    // Edge 2,0
    Point = PointOnLineSegmentNearestPoint( V2, V0, Center );

    // If the distance to the center of the sphere to the point is less than 
    // the radius of the sphere then it must intersect.
    Intersection = XMVectorOrInt( Intersection, XMVectorLessOrEqual( XMVector3LengthSq( Center - Point ), RadiusSq ) );

    return XMVector4EqualInt( XMVectorAndCInt( Intersection, NoIntersection ), XMVectorTrueInt() ) == TRUE;
}

bool IntersectTriangleAABB(const XMFLOAT3 p0, const XMFLOAT3 p1, const XMFLOAT3 p2, AABB* pVolume)
{
	static CONST XMVECTORI32 Permute0W1Z0Y0X =
                 {
                    XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_0X
                 };
    static CONST XMVECTORI32 Permute0Z0W1X0Y =
                 {
                    XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0Y
                 };
    static CONST XMVECTORI32 Permute1Y0X0W0Z =
                 {
                    XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z
                 };

    XMVECTOR Zero = XMVectorZero();

	XMVECTOR V0, V1, V2;
	V0 = XMLoadFloat3(&p0);
	V1 = XMLoadFloat3(&p1);
	V2 = XMLoadFloat3(&p2);

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->mCenter );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->mExtents );

    XMVECTOR BoxMin = Center - Extents;
    XMVECTOR BoxMax = Center + Extents;

    // Test the axes of the box (in effect test the AAB against the minimal AAB 
    // around the triangle).
    XMVECTOR TriMin = XMVectorMin( XMVectorMin( V0, V1 ), V2 );
    XMVECTOR TriMax = XMVectorMax( XMVectorMax( V0, V1 ), V2 );

    // for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then disjoint
    XMVECTOR Disjoint = XMVectorOrInt( XMVectorGreater( TriMin, BoxMax ), XMVectorGreater( BoxMin, TriMax ) );
    if( XMVector3AnyTrue( Disjoint ) )
        return FALSE;

    // Test the plane of the triangle.
    XMVECTOR Normal = XMVector3Cross( V1 - V0, V2 - V0 );
    XMVECTOR Dist = XMVector3Dot( Normal, V0 );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( Normal, Zero ) );

    // for each i in (x, y, z) if n(i) >= 0 then v_min(i)=b_min(i), v_max(i)=b_max(i)
    // else v_min(i)=b_max(i), v_max(i)=b_min(i)
    XMVECTOR NormalSelect = XMVectorGreater( Normal, Zero );
    XMVECTOR V_Min = XMVectorSelect( BoxMax, BoxMin, NormalSelect );
    XMVECTOR V_Max = XMVectorSelect( BoxMin, BoxMax, NormalSelect );

    // if n dot v_min + d > 0 || n dot v_max + d < 0 then disjoint
    XMVECTOR MinDist = XMVector3Dot( V_Min, Normal );
    XMVECTOR MaxDist = XMVector3Dot( V_Max, Normal );

    XMVECTOR NoIntersection = XMVectorGreater( MinDist, Dist );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( MaxDist, Dist ) );

    // Move the box center to zero to simplify the following tests.
    XMVECTOR TV0 = V0 - Center;
    XMVECTOR TV1 = V1 - Center;
    XMVECTOR TV2 = V2 - Center;

    // Test the edge/edge axes (3*3).
    XMVECTOR e0 = TV1 - TV0;
    XMVECTOR e1 = TV2 - TV1;
    XMVECTOR e2 = TV0 - TV2;

    // Make w zero.
    e0 = XMVectorInsert( e0, Zero, 0, 0, 0, 0, 1 );
    e1 = XMVectorInsert( e1, Zero, 0, 0, 0, 0, 1 );
    e2 = XMVectorInsert( e2, Zero, 0, 0, 0, 0, 1 );

    XMVECTOR Axis;
    XMVECTOR pp0, pp1, pp2;
    XMVECTOR Min, Max;
    XMVECTOR Radius;

    // Axis == (1,0,0) x e0 = (0, -e0.z, e0.y)
    Axis = XMVectorPermute( e0, -e0, Permute0W1Z0Y0X );
    pp0 = XMVector3Dot( TV0, Axis );
    // p1 = XMVector3Dot( V1, Axis ); // p1 = p0;
    pp2 = XMVector3Dot( TV2, Axis );
    Min = XMVectorMin( pp0, pp2 );
    Max = XMVectorMax( pp0, pp2 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (1,0,0) x e1 = (0, -e1.z, e1.y)
    Axis = XMVectorPermute( e1, -e1, Permute0W1Z0Y0X );
    pp0 = XMVector3Dot( TV0, Axis );
    pp1 = XMVector3Dot( TV1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p1;
    Min = XMVectorMin( pp0, pp1 );
    Max = XMVectorMax( pp0, pp1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (1,0,0) x e2 = (0, -e2.z, e2.y)
    Axis = XMVectorPermute( e2, -e2, Permute0W1Z0Y0X );
    pp0 = XMVector3Dot( TV0, Axis );
    pp1 = XMVector3Dot( TV1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p0;
    Min = XMVectorMin( pp0, pp1 );
    Max = XMVectorMax( pp0, pp1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,1,0) x e0 = (e0.z, 0, -e0.x)
    Axis = XMVectorPermute( e0, -e0, Permute0Z0W1X0Y );
    pp0 = XMVector3Dot( TV0, Axis );
    // p1 = XMVector3Dot( V1, Axis ); // p1 = p0;
    pp2 = XMVector3Dot( TV2, Axis );
    Min = XMVectorMin( pp0, pp2 );
    Max = XMVectorMax( pp0, pp2 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,1,0) x e1 = (e1.z, 0, -e1.x)
    Axis = XMVectorPermute( e1, -e1, Permute0Z0W1X0Y );
    pp0 = XMVector3Dot( TV0, Axis );
    pp1 = XMVector3Dot( TV1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p1;
    Min = XMVectorMin( pp0, pp1 );
    Max = XMVectorMax( pp0, pp1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e2 = (e2.z, 0, -e2.x)
    Axis = XMVectorPermute( e2, -e2, Permute0Z0W1X0Y );
    pp0 = XMVector3Dot( TV0, Axis );
    pp1 = XMVector3Dot( TV1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p0;
    Min = XMVectorMin( pp0, pp1 );
    Max = XMVectorMax( pp0, pp1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e0 = (-e0.y, e0.x, 0)
    Axis = XMVectorPermute( e0, -e0, Permute1Y0X0W0Z );
    pp0 = XMVector3Dot( TV0, Axis );
    // p1 = XMVector3Dot( V1, Axis ); // p1 = p0;
    pp2 = XMVector3Dot( TV2, Axis );
    Min = XMVectorMin( pp0, pp2 );
    Max = XMVectorMax( pp0, pp2 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e1 = (-e1.y, e1.x, 0)
    Axis = XMVectorPermute( e1, -e1, Permute1Y0X0W0Z );
    pp0 = XMVector3Dot( TV0, Axis );
    pp1 = XMVector3Dot( TV1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p1;
    Min = XMVectorMin( pp0, pp1 );
    Max = XMVectorMax( pp0, pp1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e2 = (-e2.y, e2.x, 0)
    Axis = XMVectorPermute( e2, -e2, Permute1Y0X0W0Z );
    pp0 = XMVector3Dot( TV0, Axis );
    pp1 = XMVector3Dot( TV1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p0;
    Min = XMVectorMin( pp0, pp1 );
    Max = XMVectorMax( pp0, pp1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    return XMVector4NotEqualInt( NoIntersection, XMVectorTrueInt() ) == TRUE;
}

bool IntersectTriangleOABB(const XMFLOAT3 p0, const XMFLOAT3 p1, const XMFLOAT3 p2, OABB* pVolume)
{
	 // Load the box center & orientation.
    XMVECTOR Center = XMLoadFloat3( &pVolume->mCenter );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->mOrientation );
	XMVECTOR V0, V1, V2;
	V0 = XMLoadFloat3(&p0);
	V1 = XMLoadFloat3(&p1);
	V2 = XMLoadFloat3(&p2);

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Transform the triangle vertices into the space of the box.
    XMVECTOR TV0 = XMVector3InverseRotate( V0 - Center, Orientation );
    XMVECTOR TV1 = XMVector3InverseRotate( V1 - Center, Orientation );
    XMVECTOR TV2 = XMVector3InverseRotate( V2 - Center, Orientation );

    AABB Box;
    Box.mCenter = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    Box.mExtents = pVolume->mExtents;

    // Use the triangle vs axis aligned box intersection routine.
	XMFLOAT3 one, two, three;
	XMStoreFloat3(&one, TV0);
	XMStoreFloat3(&two, TV1);
	XMStoreFloat3(&three, TV2);

    return IntersectTriangleAABB( one, two, three, &Box );
}

//sphere collision detection
bool IntersectSphereSphere(const Sphere* pVolumeA, const Sphere* pVolumeB)
{
	  // Load A.
    XMVECTOR CenterA = XMLoadFloat3( &pVolumeA->mPosition );
    XMVECTOR RadiusA = XMVectorReplicatePtr( &pVolumeA->mRadius );

    // Load B.
    XMVECTOR CenterB = XMLoadFloat3( &pVolumeB->mPosition );
    XMVECTOR RadiusB = XMVectorReplicatePtr( &pVolumeB->mRadius );

    // Distance squared between centers.    
    XMVECTOR Delta = CenterB - CenterA;
    XMVECTOR DistanceSquared = XMVector3LengthSq( Delta );

    // Sum of the radii sqaured.
    XMVECTOR RadiusSquared = RadiusA + RadiusB;
    RadiusSquared = RadiusSquared * RadiusSquared;

    return XMVector4LessOrEqual( DistanceSquared, RadiusSquared ) == TRUE;
}

bool IntersectSphereAABB(const Sphere* pVolumeA, const AABB* pVolumeB)
{
	XMVECTOR SphereCenter = XMLoadFloat3( &pVolumeA->mPosition );
    XMVECTOR SphereRadius = XMVectorReplicatePtr( &pVolumeA->mRadius );

    XMVECTOR BoxCenter = XMLoadFloat3( &pVolumeB->mCenter );
    XMVECTOR BoxExtents = XMLoadFloat3( &pVolumeB->mExtents );

    XMVECTOR BoxMin = BoxCenter - BoxExtents;
    XMVECTOR BoxMax = BoxCenter + BoxExtents;

    // Find the distance to the nearest point on the box.
    // for each i in (x, y, z)
    // if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
    // else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

    XMVECTOR d = XMVectorZero();

    // Compute d for each dimension.
    XMVECTOR LessThanMin = XMVectorLess( SphereCenter, BoxMin );
    XMVECTOR GreaterThanMax = XMVectorGreater( SphereCenter, BoxMax );

    XMVECTOR MinDelta = SphereCenter - BoxMin;
    XMVECTOR MaxDelta = SphereCenter - BoxMax;

    // Choose value for each dimension based on the comparison.
    d = XMVectorSelect( d, MinDelta, LessThanMin );
    d = XMVectorSelect( d, MaxDelta, GreaterThanMax );

    // Use a dot-product to square them and sum them together.
    XMVECTOR d2 = XMVector3Dot( d, d );

    return XMVector4LessOrEqual( d2, XMVectorMultiply( SphereRadius, SphereRadius ) ) == TRUE;
}

bool IntersectSphereOABB(const Sphere* pVolumeA, const OABB* pVolumeB)
{
	XMVECTOR SphereCenter = XMLoadFloat3( &pVolumeA->mPosition );
    XMVECTOR SphereRadius = XMVectorReplicatePtr( &pVolumeA->mRadius );

    XMVECTOR BoxCenter = XMLoadFloat3( &pVolumeB->mCenter );
    XMVECTOR BoxExtents = XMLoadFloat3( &pVolumeB->mExtents );
    XMVECTOR BoxOrientation = XMLoadFloat4( &pVolumeB->mOrientation );

    XMASSERT( XMQuaternionIsUnit( BoxOrientation ) );

    // Transform the center of the sphere to be local to the box.
    // BoxMin = -BoxExtents
    // BoxMax = +BoxExtents
    SphereCenter = XMVector3InverseRotate( SphereCenter - BoxCenter, BoxOrientation );

    // Find the distance to the nearest point on the box.
    // for each i in (x, y, z)
    // if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
    // else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

    XMVECTOR d = XMVectorZero();

    // Compute d for each dimension.
    XMVECTOR LessThanMin = XMVectorLess( SphereCenter, -BoxExtents );
    XMVECTOR GreaterThanMax = XMVectorGreater( SphereCenter, BoxExtents );

    XMVECTOR MinDelta = SphereCenter + BoxExtents;
    XMVECTOR MaxDelta = SphereCenter - BoxExtents;

    // Choose value for each dimension based on the comparison.
    d = XMVectorSelect( d, MinDelta, LessThanMin );
    d = XMVectorSelect( d, MaxDelta, GreaterThanMax );

    // Use a dot-product to square them and sum them together.
    XMVECTOR d2 = XMVector3Dot( d, d );

    return XMVector4LessOrEqual( d2, XMVectorMultiply( SphereRadius, SphereRadius ) ) == TRUE;
}

//AABB collision
bool IntersectAABBAABB(const AABB* pVolumeA, const AABB* pVolumeB)
{
	XMVECTOR CenterA = XMLoadFloat3( &pVolumeA->mCenter );
    XMVECTOR ExtentsA = XMLoadFloat3( &pVolumeA->mExtents );

    XMVECTOR CenterB = XMLoadFloat3( &pVolumeB->mCenter );
    XMVECTOR ExtentsB = XMLoadFloat3( &pVolumeB->mExtents );

    XMVECTOR MinA = CenterA - ExtentsA;
    XMVECTOR MaxA = CenterA + ExtentsA;

    XMVECTOR MinB = CenterB - ExtentsB;
    XMVECTOR MaxB = CenterB + ExtentsB;

    // for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then return FALSE
    XMVECTOR Disjoint = XMVectorOrInt( XMVectorGreater( MinA, MaxB ), XMVectorGreater( MinB, MaxA ) );

    return XMVector3AnyTrue( Disjoint ) == FALSE;
}

bool IntersectAABBOABB(const AABB* pVolumeA, const OABB* pVolumeB)
{
	 // Make the axis aligned box oriented and do an OBB vs OBB test.
    OABB BoxA;

    BoxA.mCenter = pVolumeA->mCenter;
    BoxA.mExtents = pVolumeA->mExtents;
    BoxA.mOrientation.x = 0.0f;
    BoxA.mOrientation.y = 0.0f;
    BoxA.mOrientation.z = 0.0f;
    BoxA.mOrientation.w = 1.0f;

    return IntersectOABBOABB( &BoxA, pVolumeB );
}

//OABB collision
bool IntersectOABBOABB(const OABB* pVolumeA, const OABB* pVolumeB)
{
	static CONST XMVECTORI32 Permute0W1Z0Y0X =
                 {
                    XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_0X
                 };
    static CONST XMVECTORI32 Permute0Z0W1X0Y =
                 {
                    XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0Y
                 };
    static CONST XMVECTORI32 Permute1Y0X0W0Z =
                 {
                    XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z
                 };
    static CONST XMVECTORI32 PermuteWZYX =
                 {
                    XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X
                 };
    static CONST XMVECTORI32 PermuteZWXY =
                 {
                    XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y
                 };
    static CONST XMVECTORI32 PermuteYXWZ =
                 {
                    XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z
                 };

    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    // Build the 3x3 rotation matrix that defines the orientation of B relative to A.
    XMVECTOR A_quat = XMLoadFloat4( &pVolumeA->mOrientation );
    XMVECTOR B_quat = XMLoadFloat4( &pVolumeB->mOrientation );

    XMASSERT( XMQuaternionIsUnit( A_quat ) );
    XMASSERT( XMQuaternionIsUnit( B_quat ) );

    XMVECTOR Q = XMQuaternionMultiply( A_quat, XMQuaternionConjugate( B_quat ) );
    XMMATRIX R = XMMatrixRotationQuaternion( Q );

    // Compute the translation of B relative to A.
    XMVECTOR A_cent = XMLoadFloat3( &pVolumeA->mCenter );
    XMVECTOR B_cent = XMLoadFloat3( &pVolumeB->mCenter );
    XMVECTOR t = XMVector3InverseRotate( B_cent - A_cent, A_quat );

    //
    // h(A) = extents of A.
    // h(B) = extents of B.
    //
    // a(u) = axes of A = (1,0,0), (0,1,0), (0,0,1)
    // b(u) = axes of B relative to A = (r00,r10,r20), (r01,r11,r21), (r02,r12,r22)
    //  
    // For each possible separating axis l:
    //   d(A) = sum (for i = u,v,w) h(A)(i) * abs( a(i) dot l )
    //   d(B) = sum (for i = u,v,w) h(B)(i) * abs( b(i) dot l )
    //   if abs( t dot l ) > d(A) + d(B) then disjoint
    //

    // Load extents of A and B.
    XMVECTOR h_A = XMLoadFloat3( &pVolumeA->mExtents );
    XMVECTOR h_B = XMLoadFloat3( &pVolumeB->mExtents );

    // Rows. Note R[0,1,2]X.w = 0.
    XMVECTOR R0X = R.r[0];
    XMVECTOR R1X = R.r[1];
    XMVECTOR R2X = R.r[2];

    R = XMMatrixTranspose( R );

    // Columns. Note RX[0,1,2].w = 0.
    XMVECTOR RX0 = R.r[0];
    XMVECTOR RX1 = R.r[1];
    XMVECTOR RX2 = R.r[2];

    // Absolute value of rows.
    XMVECTOR AR0X = XMVectorAbs( R0X );
    XMVECTOR AR1X = XMVectorAbs( R1X );
    XMVECTOR AR2X = XMVectorAbs( R2X );

    // Absolute value of columns.
    XMVECTOR ARX0 = XMVectorAbs( RX0 );
    XMVECTOR ARX1 = XMVectorAbs( RX1 );
    XMVECTOR ARX2 = XMVectorAbs( RX2 );

    // Test each of the 15 possible seperating axii.
    XMVECTOR d, d_A, d_B;

    // l = a(u) = (1, 0, 0)
    // t dot l = t.x
    // d(A) = h(A).x
    // d(B) = h(B) dot abs(r00, r01, r02)
    d = XMVectorSplatX( t );
    d_A = XMVectorSplatX( h_A );
    d_B = XMVector3Dot( h_B, AR0X );
    XMVECTOR NoIntersection = XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) );

    // l = a(v) = (0, 1, 0)
    // t dot l = t.y
    // d(A) = h(A).y
    // d(B) = h(B) dot abs(r10, r11, r12)
    d = XMVectorSplatY( t );
    d_A = XMVectorSplatY( h_A );
    d_B = XMVector3Dot( h_B, AR1X );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(w) = (0, 0, 1)
    // t dot l = t.z
    // d(A) = h(A).z
    // d(B) = h(B) dot abs(r20, r21, r22)
    d = XMVectorSplatZ( t );
    d_A = XMVectorSplatZ( h_A );
    d_B = XMVector3Dot( h_B, AR2X );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = b(u) = (r00, r10, r20)
    // d(A) = h(A) dot abs(r00, r10, r20)
    // d(B) = h(B).x
    d = XMVector3Dot( t, RX0 );
    d_A = XMVector3Dot( h_A, ARX0 );
    d_B = XMVectorSplatX( h_B );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = b(v) = (r01, r11, r21)
    // d(A) = h(A) dot abs(r01, r11, r21)
    // d(B) = h(B).y
    d = XMVector3Dot( t, RX1 );
    d_A = XMVector3Dot( h_A, ARX1 );
    d_B = XMVectorSplatY( h_B );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = b(w) = (r02, r12, r22)
    // d(A) = h(A) dot abs(r02, r12, r22)
    // d(B) = h(B).z
    d = XMVector3Dot( t, RX2 );
    d_A = XMVector3Dot( h_A, ARX2 );
    d_B = XMVectorSplatZ( h_B );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(u) x b(u) = (0, -r20, r10)
    // d(A) = h(A) dot abs(0, r20, r10)
    // d(B) = h(B) dot abs(0, r02, r01)
    d = XMVector3Dot( t, XMVectorPermute( RX0, -RX0, Permute0W1Z0Y0X ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX0, ARX0, PermuteWZYX ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR0X, AR0X, PermuteWZYX ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(u) x b(v) = (0, -r21, r11)
    // d(A) = h(A) dot abs(0, r21, r11)
    // d(B) = h(B) dot abs(r02, 0, r00)
    d = XMVector3Dot( t, XMVectorPermute( RX1, -RX1, Permute0W1Z0Y0X ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX1, ARX1, PermuteWZYX ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR0X, AR0X, PermuteZWXY ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(u) x b(w) = (0, -r22, r12)
    // d(A) = h(A) dot abs(0, r22, r12)
    // d(B) = h(B) dot abs(r01, r00, 0)
    d = XMVector3Dot( t, XMVectorPermute( RX2, -RX2, Permute0W1Z0Y0X ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX2, ARX2, PermuteWZYX ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR0X, AR0X, PermuteYXWZ ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(v) x b(u) = (r20, 0, -r00)
    // d(A) = h(A) dot abs(r20, 0, r00)
    // d(B) = h(B) dot abs(0, r12, r11)
    d = XMVector3Dot( t, XMVectorPermute( RX0, -RX0, Permute0Z0W1X0Y ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX0, ARX0, PermuteZWXY ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR1X, AR1X, PermuteWZYX ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(v) x b(v) = (r21, 0, -r01)
    // d(A) = h(A) dot abs(r21, 0, r01)
    // d(B) = h(B) dot abs(r12, 0, r10)
    d = XMVector3Dot( t, XMVectorPermute( RX1, -RX1, Permute0Z0W1X0Y ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX1, ARX1, PermuteZWXY ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR1X, AR1X, PermuteZWXY ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(v) x b(w) = (r22, 0, -r02)
    // d(A) = h(A) dot abs(r22, 0, r02)
    // d(B) = h(B) dot abs(r11, r10, 0)
    d = XMVector3Dot( t, XMVectorPermute( RX2, -RX2, Permute0Z0W1X0Y ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX2, ARX2, PermuteZWXY ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR1X, AR1X, PermuteYXWZ ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(w) x b(u) = (-r10, r00, 0)
    // d(A) = h(A) dot abs(r10, r00, 0)
    // d(B) = h(B) dot abs(0, r22, r21)
    d = XMVector3Dot( t, XMVectorPermute( RX0, -RX0, Permute1Y0X0W0Z ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX0, ARX0, PermuteYXWZ ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR2X, AR2X, PermuteWZYX ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(w) x b(v) = (-r11, r01, 0)
    // d(A) = h(A) dot abs(r11, r01, 0)
    // d(B) = h(B) dot abs(r22, 0, r20)
    d = XMVector3Dot( t, XMVectorPermute( RX1, -RX1, Permute1Y0X0W0Z ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX1, ARX1, PermuteYXWZ ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR2X, AR2X, PermuteZWXY ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(w) x b(w) = (-r12, r02, 0)
    // d(A) = h(A) dot abs(r12, r02, 0)
    // d(B) = h(B) dot abs(r21, r20, 0)
    d = XMVector3Dot( t, XMVectorPermute( RX2, -RX2, Permute1Y0X0W0Z ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX2, ARX2, PermuteYXWZ ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR2X, AR2X, PermuteYXWZ ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // No seperating axis found, boxes must intersect.
    return XMVector4NotEqualInt( NoIntersection, XMVectorTrueInt() ) == TRUE;
}

void TransformSphere(Sphere* pOut, float scale, const XMFLOAT3 rotation, const XMFLOAT3 translation)
{
	// Load the center of the sphere.
    XMVECTOR Center = XMLoadFloat3( &pOut->mPosition );
	XMVECTOR Rotation = XMLoadFloat3(&rotation);
	XMVECTOR Translation = XMLoadFloat3(&translation);

    // Transform the center of the sphere.
    Center = XMVector3Rotate( Center * XMVectorReplicate( scale ), Rotation ) + Translation;

    // Store the center sphere.
    XMStoreFloat3( &pOut->mPosition, Center );

    // Scale the radius of the pshere.
    pOut->mRadius = pOut->mRadius * scale;
}

void TransformAABB(AABB* pOut, float scale, const XMFLOAT3 rotation, const XMFLOAT3 translation)
{
    static XMVECTOR Offset[8] =
    {
        { -1.0f, -1.0f, -1.0f, 0.0f },
        { -1.0f, -1.0f,  1.0f, 0.0f },
        { -1.0f,  1.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f,  1.0f, 0.0f },
        {  1.0f, -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f,  1.0f, 0.0f },
        {  1.0f,  1.0f, -1.0f, 0.0f },
        {  1.0f,  1.0f,  1.0f, 0.0f }
    };

    // Load center and extents.
    XMVECTOR Center = XMLoadFloat3( &pOut->mCenter );
    XMVECTOR Extents = XMLoadFloat3( &pOut->mExtents );
	XMVECTOR Rotation = XMLoadFloat3(&rotation);
	XMVECTOR Translation = XMLoadFloat3(&translation);

    XMVECTOR VectorScale = XMVectorReplicate( scale );

    // Compute and transform the corners and find new min/max bounds.
    XMVECTOR Corner = Center + Extents * Offset[0];
    Corner = XMVector3Rotate( Corner * VectorScale, Rotation ) + Translation;

    XMVECTOR Min, Max;
    Min = Max = Corner;

    for( INT i = 1; i < 8; i++ )
    {
        Corner = Center + Extents * Offset[i];
        Corner = XMVector3Rotate( Corner * VectorScale, Rotation ) + Translation;

        Min = XMVectorMin( Min, Corner );
        Max = XMVectorMax( Max, Corner );
    }

    // Store center and extents.
    XMStoreFloat3( &pOut->mCenter, ( Min + Max ) * 0.5f );
    XMStoreFloat3( &pOut->mExtents, ( Max - Min ) * 0.5f );
}

void TransformOAAB(OABB* pOut, float scale, const XMFLOAT3 rotation, const XMFLOAT3 translation)
{
}

void TransformFrustum(Frustum* pOut, float scale, const XMFLOAT3, const XMFLOAT3 translation)
{
}