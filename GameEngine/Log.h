//(c) copyright Martin Hollstein

#pragma once

#include <iostream>
#include <fstream>
#include <ostream>

#ifndef LOG_H
#define LOG_H

#define MAXENTITIES 64

#ifndef BADALLOC
#define BADALLOC "Failed to allocate memory"
#endif

namespace Log
{
	//error logging
	void BeginRunTimeStamp();

	void Error( std::string error, std::string NameOfObject, void* loc);

	void Inform( std::string error,  std::string NameOfObject,  void* loc);
	
};

#endif