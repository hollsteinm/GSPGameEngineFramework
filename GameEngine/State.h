//(c) copyright Martin Hollstein

#pragma once

//define state flags here to be used as unsigned int
#define INACTIVE	0
#define ACTIVE		1

class CState
{
public:
	CState(void);
	CState(unsigned int state);
	~CState(void);

	unsigned int getState();
	void setState(unsigned int state);

private:
	unsigned int mState;
};