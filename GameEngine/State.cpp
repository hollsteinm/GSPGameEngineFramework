//(c) copyright Martin Hollstein

#include "State.h"

CState::CState(void)
{
	mState = INACTIVE;
}

CState::CState(unsigned int state) 
{
	mState = state;
}

CState::~CState(void)
{
}

unsigned int CState::getState()
{
	return mState;
}

void CState::setState(unsigned int state)
{
	mState = state;
}