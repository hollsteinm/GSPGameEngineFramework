//(c) copyright Martin Hollstein

#include "Log.h"
#include <ctime>

void Log::BeginRunTimeStamp()
{
	//get the date and time
	time_t now = time(0);
	char dt[255];
	ctime_s(dt, sizeof(dt), &now);

	const char* errorFileName = "..\\Logs\\Error.txt";

	//stamp time to both the error and inform files.
	std::ofstream errorFile;
	errorFile.open(errorFileName, std::ios::in | std::ios::out | std::ios::app);

	if(errorFile.is_open())
	{
		errorFile << dt;
	}

	errorFile.close();

	//only stamp the inform file during a debug run
	#if defined(DEBUG) | defined(_DEBUG)
		const char* informFileName = "..\\Logs\\Info.txt";

		std::ofstream informFile;
		informFile.open(informFileName, std::ios::in | std::ios::out | std::ios::app);

		if(informFile.is_open())
		{
			informFile << dt;
		}

		informFile.close();
	#endif
}

void Log::Error( std::string error, std::string NameOfObject, void* location)
{
	const char* fileName = "..\\Logs\\Error.txt";

	std::ofstream errorFile;
	errorFile.open(fileName, std::ios::in | std::ios::out | std::ios::app);

	//write error logging message here
	if(errorFile.is_open())
	{
		errorFile << "[ERROR]: " << error.c_str() << " @ " << NameOfObject.c_str() << " : " << &location << std::endl;
	}

	errorFile.close();
}

void Log::Inform( std::string Information,  std::string NameOfObject,  void* location)
{
	#if defined(DEBUG) | defined(_DEBUG)
		const char* fileName = "..\\Logs\\Info.txt";

		std::ofstream informFile;
		informFile.open(fileName, std::ios::in | std::ios::out | std::ios::app);

		//write error logging message here
		if(informFile.is_open())
		{
			informFile << "[INFO]: " << Information.c_str() << " @ " << NameOfObject.c_str() << " : " << &location << std::endl;
		}

		informFile.close();
	#endif
}