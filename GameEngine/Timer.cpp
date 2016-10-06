//(c) copyright Martin Hollstein

#include "Timer.h"

CTimer::CTimer(void)
{
	mCntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&mCntsPerSec);
	mSecsPerCnt = 1.0f / (float)mCntsPerSec;

	mPrevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&mPrevTimeStamp);
}

CTimer::~CTimer(void)
{
}

void CTimer::Start()
{
	mCurrTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTimeStamp);
	mDt = (mCurrTimeStamp - mPrevTimeStamp) * mSecsPerCnt;
}

void CTimer::End()
{
	mPrevTimeStamp = mCurrTimeStamp;
}

float CTimer::dt()
{
	return mDt;
}
