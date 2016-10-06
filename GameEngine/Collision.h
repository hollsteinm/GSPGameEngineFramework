#include "DX10Util.h"
#include "Mesh.h"

#ifndef COLLISION_H
#define COLLISION_H

//forward declarations
class SphereCollider;
class AABBCollider;

inline float Distance(const XMFLOAT3& a, const XMFLOAT3& b);

//functions for collision detection (could probably be made helpers in the .cpp)
bool Intersect(const SphereCollider& pVolume, const SphereCollider& other);
bool Intersect(const SphereCollider& pVolume, const AABBCollider& other);

bool Intersect(const AABBCollider& pVolume, const AABBCollider& other);
bool Intersect(const AABBCollider& pVolume,  const SphereCollider& other);


class RayCollider
{
public:
	RayCollider();
	~RayCollider();

	XMFLOAT3 getOrigin();
	XMFLOAT3 getDirection();
	float getDistance();

private:
	XMFLOAT3 mOrigin;
	XMFLOAT3 mDirection;
	float mDistance;
};

class SphereCollider
{
public:
	SphereCollider();
	~SphereCollider();

	XMFLOAT3 getCenter();
	float getRadius();

	const XMFLOAT3 getCenter() const;
	const float getRadius() const;

	bool Intersect(const SphereCollider& other);
	bool Intersect(const AABBCollider& other);

	void setValues(XMFLOAT3 center, float radius);
	void setCenter(XMFLOAT3 center);
	void setRadius(float radius);

private:
	XMFLOAT3 mCenter;
	float mRadius;
};

class AABBCollider
{
public:
	AABBCollider();
	~AABBCollider();

	XMFLOAT3 getCenter();
	XMFLOAT3 getExtents();

	const XMFLOAT3 getCenter() const;
	const XMFLOAT3 getExtents() const;

	bool Intersect(const SphereCollider& other);
	bool Intersect(const AABBCollider& other);

	void setValues(XMFLOAT3 center, XMFLOAT3 extents);
	void setCenter(XMFLOAT3 center);
	void setExtents(XMFLOAT3 extents);

	void computeExtentsFromPoints(const DXVertex* pVertices, const unsigned int numVertices);

private:
	XMFLOAT3 mCenter;
	XMFLOAT3 mExtents;
};

#endif

