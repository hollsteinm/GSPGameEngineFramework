#include "Contacts.h"
#include "DX10Util.h"

float inline Magnitude(XMVECTOR v)
{
	return sqrtf(
		powf(XMVectorGetX(v), 2) +
		powf(XMVectorGetY(v), 2) +
		powf(XMVectorGetZ(v), 2));
}

Contacts::Contacts(void)
{
	mNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mPenetrationDepth = 0.0f;
}

Contacts::~Contacts(void)
{
}

XMFLOAT3 Contacts::getContactNormal()
{
	return mNormal;
}

XMFLOAT3 Contacts::getContactPoint()
{
	return mPoint;
}

float Contacts::getPenetrationDepth()
{
	return mPenetrationDepth;
}

const XMFLOAT3 Contacts::getContactNormal() const
{
	return mNormal;
}

const XMFLOAT3 Contacts::getContactPoint() const
{
	return mPoint;
}

const float Contacts::getPenetrationDepth() const
{
	return mPenetrationDepth;
}

void Contacts::collectContact(const AABBCollider& A, const AABBCollider& B)
{
}

void Contacts::collectContact(const SphereCollider& A, const SphereCollider& B)
{
	//Get penetration depth
	float distance = Distance(A.getCenter(), B.getCenter());
	float bothRadii = A.getRadius() + B.getRadius();

	//will be positive if both penetrate
	mPenetrationDepth = bothRadii - distance; 

	//Get penetration point
	XMVECTOR AVec = XMLoadFloat3(&A.getCenter());
	XMVECTOR BVec = XMLoadFloat3(&B.getCenter());
	XMVECTOR midline = AVec - BVec;

	XMStoreFloat3(&mPoint, midline);	

	//Get penetration normal
	distance = Magnitude(midline);

	XMVECTOR normal = midline * (1.0f / distance);
	XMStoreFloat3(&mNormal, normal);
}