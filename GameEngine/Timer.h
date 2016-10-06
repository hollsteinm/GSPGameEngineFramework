//(c) copyright Martin Hollstein

#include <Windows.h>

#ifndef TIMER_H
#define TIMER_H

class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	//Name: Start()
	//Purpose: begins the timer for the precision timer
	//Return: none
	void Start();

	//name: End()
	//Purpose: ends the timer for the precision timer
	//Return: none
	void End();

	//Name: dt()
	//Purpose: get delta time
	//return: float of change in time
	float dt();

private:
	__int64 mCntsPerSec;
	__int64 mPrevTimeStamp;
	__int64 mCurrTimeStamp;
	float mSecsPerCnt;
	float mDt;
};

#endif

