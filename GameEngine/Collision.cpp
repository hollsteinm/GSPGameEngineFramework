#include "Collision.h"
#include <cmath>

//helpers
inline float Distance(const XMFLOAT3& a, const XMFLOAT3& b)
{
	float xd = pow((b.x - a.x), 2);
	float yd = pow((b.y - a.y), 2);
	float zd = pow((b.z - a.z), 2);
	float td = xd + yd + zd;
	return sqrt(td);
}

//intersection tests
bool Intersect(const SphereCollider& pVolume, const SphereCollider& other)
{
	float distance = Distance(pVolume.getCenter(), other.getCenter());
	
	return (distance < (pVolume.getRadius() + other.getRadius()));
}

bool Intersect(const SphereCollider& pVolume, const AABBCollider& other)
{
	return false;
}

bool Intersect(const AABBCollider& pVolume, const AABBCollider& other)
{
	XMVECTOR volumeCenter = XMLoadFloat3(&pVolume.getCenter());
	XMVECTOR otherCenter = XMLoadFloat3(&other.getCenter());

	FXMVECTOR length = otherCenter - volumeCenter;

	return fabs(XMVectorGetX(length)) <= (pVolume.getExtents().x + other.getExtents().x) &&
		fabs(XMVectorGetY(length)) <= (pVolume.getExtents().y + other.getExtents().y) &&
		fabs(XMVectorGetZ(length)) <= (pVolume.getExtents().z + other.getExtents().z);
}

bool Intersect(const AABBCollider& pVolume,  const SphereCollider& other)
{
	return false;
}

//AABB
AABBCollider::AABBCollider()
{
	mCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

AABBCollider::~AABBCollider()
{
}

XMFLOAT3 AABBCollider::getCenter()
{
	return mCenter;
}

XMFLOAT3 AABBCollider::getExtents()
{
	return mExtents;
}

const XMFLOAT3 AABBCollider::getCenter() const
{
	return mCenter;
}

const XMFLOAT3 AABBCollider::getExtents() const
{
	return mExtents;
}

void AABBCollider::setCenter(XMFLOAT3 center)
{
	mCenter = center;
}

void AABBCollider::setValues(XMFLOAT3 center, XMFLOAT3 extents)
{
	mCenter = center;
	mExtents = extents;
}

void AABBCollider::setExtents(XMFLOAT3 extents)
{
	mExtents = extents;
}

void AABBCollider::computeExtentsFromPoints(const DXVertex* pVertices, const unsigned int numVertices)
{
	XMFLOAT3 tempExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for(unsigned int i = 0; i < numVertices; ++i)
	{
		if(pVertices[i].position.x > tempExtents.x)
			tempExtents.x = pVertices[i].position.x;

		if(pVertices[i].position.y > tempExtents.y)
			tempExtents.y = pVertices[i].position.y;

		if(pVertices[i].position.z > tempExtents.z)
			tempExtents.z = pVertices[i].position.z;			
	}

	mExtents = tempExtents;
}

//Sphere
SphereCollider::SphereCollider()
{
	mCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mRadius = 0.0f;
}

SphereCollider::~SphereCollider()
{
}

XMFLOAT3 SphereCollider::getCenter()
{
	return mCenter;
}

const XMFLOAT3 SphereCollider::getCenter() const
{
	return mCenter;
}

float SphereCollider::getRadius()
{
	return mRadius;
}

const float SphereCollider::getRadius() const
{
	return mRadius;
}

void SphereCollider::setCenter(XMFLOAT3 center)
{
	mCenter = center;
}

void SphereCollider::setValues(XMFLOAT3 center, float radius)
{
	mCenter = center;
	mRadius = radius;
}

void SphereCollider::setRadius(float radius)
{
	mRadius = radius;
}


