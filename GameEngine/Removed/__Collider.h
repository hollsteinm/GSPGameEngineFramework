#include "DX10Util.h"

#ifndef COLLIDER_H
#define COLLIDER_H


struct Ray
{
	Ray() : mOrigin(XMFLOAT3(0.0f, 0.0f, 0.0f)), mDirection(XMFLOAT3(0.0f, 0.0f, 0.0f)), mDistance(0){}
	Ray(XMFLOAT3 origin, XMFLOAT3 direction, float distance) : mOrigin(origin), mDirection(direction), mDistance(distance){}

	XMFLOAT3 mOrigin;
	XMFLOAT3 mDirection;
	float mDistance;
};

struct AABB
{
	AABB() : mCenter(XMFLOAT3(0.0f, 0.0f, 0.0f)), mExtents(XMFLOAT3(0.0f, 0.0f, 0.0f)){}
	AABB(XMFLOAT3 center, XMFLOAT3 extents) : mCenter(center), mExtents(extents){}

	XMFLOAT3 mCenter;
	XMFLOAT3 mExtents;
};

struct OABB
{
	OABB() : mCenter(XMFLOAT3(0.0f, 0.0f, 0.0f)), mExtents(XMFLOAT3(0.0f, 0.0f, 0.0f)), mOrientation(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)){}
	OABB(XMFLOAT3 center, XMFLOAT3 extents, XMFLOAT4 orientation) : mCenter(center), mExtents(extents), mOrientation(orientation){}

	XMFLOAT3 mCenter;
	XMFLOAT3 mExtents;
	XMFLOAT4 mOrientation;
};

struct Sphere
{
	Sphere() : mRadius(0), mPosition(XMFLOAT3(0.0f, 0.0f, 0.0f)){}
	Sphere(float radius, XMFLOAT3 position) : mRadius(radius), mPosition(position){}

	float mRadius;
	XMFLOAT3 mPosition;
};

struct Frustum
{
	Frustum() : mOrigin(XMFLOAT3(0.0f, 0.0f, 0.0f)), mOrientation(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), mRightSlope(0), mLeftSlope(0), mTopSlope(0), mBottomSlope(0), mNear(0.1f), mFar(1000.0f){}
	Frustum(XMFLOAT3 origin, XMFLOAT4 orientation, float rSlope, float lSlope, float tSlope, float bSlope, float near, float far) :
		mOrigin(origin), mOrientation(orientation), mRightSlope(rSlope), mLeftSlope(lSlope), mTopSlope(tSlope), mBottomSlope(bSlope), mNear(near), mFar(far){}

	XMFLOAT3 mOrigin;
	XMFLOAT4 mOrientation;

	float mRightSlope;
	float mLeftSlope;
	float mTopSlope;
	float mBottomSlope;
	float mNear;
	float mFar;
};

//call this on loading of objects as well as during updates to reconstruct after movements
void ConstructBoundingSphere(Sphere* pOutSphere, unsigned int count, const XMFLOAT3* pPoints, unsigned int stride);
void ConstructAABB(AABB* pOutAABB, unsigned int count, const XMFLOAT3* pPoints, unsigned int stride);
void ConstructOABB(OABB* poutOABB, unsigned int count, const XMFLOAT3* pPoints, unsigned int stride);
void ConstructFrustumFromProjection(Frustum* pOutFrustum, const XMFLOAT4X4* pProjectionMatrix);

//void ConstructPlanesFromFrustum(const Frustum* pVolume, const XMFLOAT3* pPlane0, const XMFLOAT3* pPlane1, 
//	const XMFLOAT3* pPlane2, const XMFLOAT3* pPlane3, const XMFLOAT3* pPlane4, const XMFLOAT3* pPlane5);

//collision detection
//point collisions
bool IntersectPointSphere(const XMFLOAT3 pPoint, const Sphere* pVolume);
bool IntersectPointAABB(const XMFLOAT3 pPoint, const AABB* pVolume);
bool IntersectPointOABB(const XMFLOAT3 pPoint, const OABB* pVolume);
bool IntersectPointFrustum(const XMFLOAT3 pPoint, const Frustum* pVolume);

//ray collisions
bool IntersectRayTriangle(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, const XMFLOAT3 & v0, 
	const XMFLOAT3 & v1, XMFLOAT3 & v2, float* pDistance);

bool IntersectRayAABB(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, AABB* pVolume, float* pDistance);
bool IntersectRayOABB(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, OABB* pVolume, float* pDistance);
bool IntersectRaySphere(const XMFLOAT3 pOrigin, const XMFLOAT3 pDirection, Sphere* pVolume, float* pDistance);

//triangle collision detection (vertices)
bool IntersectTriangleTriangle(const XMFLOAT3 a0, const XMFLOAT3 a1, const XMFLOAT3 a2, const XMFLOAT3 & b0, const XMFLOAT3 & b1, const XMFLOAT3 & b2 );
bool IntersectTriangleSphere(const XMFLOAT3 p0, const XMFLOAT3 p1, const XMFLOAT3 p2, Sphere* pVolume);
bool IntersectTriangleAABB(const XMFLOAT3 p0, const XMFLOAT3 p1, const XMFLOAT3 p2, AABB* pVolume);
bool IntersectTriangleOABB(const XMFLOAT3 p0, const XMFLOAT3 p1, const XMFLOAT3 p2, OABB* pVolume);

//sphere collision detection
bool IntersectSphereSphere(const Sphere* pVolumeA, const Sphere* pVolumeB);
bool IntersectSphereAABB(const Sphere* pVolumeA, const AABB* pVolumeB);
bool IntersectSphereOABB(const Sphere* pVolumeA, const OABB* pVolumeB);

//AABB collision
bool IntersectAABBAABB(const AABB* pVolumeA, const AABB* pVolumeB);
bool IntersectAABBOABB(const AABB* pVolumeA, const OABB* pVolumeB);

//OABB collision
bool IntersectOABBOABB(const OABB* pVolumeA, const OABB* pVolumeB);

//transform coordinates
void TransformSphere(Sphere* pOut, float scale, const XMFLOAT3 rotation, const XMFLOAT3 translation);
void TransformAABB(AABB* pOut, float scale, const XMFLOAT3 rotation, const XMFLOAT3 translation);
void TransformOAAB(OABB* pOut, float scale, const XMFLOAT3 rotation, const XMFLOAT3 translation);
void TransformFrustum(Frustum* pOut, float scale, const XMFLOAT3, const XMFLOAT3 translation);


#endif