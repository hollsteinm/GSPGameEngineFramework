#include "DX10Util.h"
#include "Collision.h"

#ifndef CONTACT_H
#define CONTACT_H
//TODO: Finish
class Contacts
{
public:
	Contacts(void);
	~Contacts(void);

	XMFLOAT3 getContactPoint();
	XMFLOAT3 getContactNormal();
	float getPenetrationDepth();

	const XMFLOAT3 getContactPoint() const;
	const XMFLOAT3 getContactNormal() const;
	const float getPenetrationDepth() const;

	void collectContact(const AABBCollider& A, const AABBCollider& B);
	void collectContact(const SphereCollider& A, const SphereCollider& B);

private:
	XMFLOAT3 mPoint;
	XMFLOAT3 mNormal;
	float mPenetrationDepth;
};

#endif

