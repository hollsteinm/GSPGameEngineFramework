#include "ContentCreator.h"


ContentCreator::ContentCreator(void)
{
	mData = 0;
	numContentKeys = 0;
	mCacheStream = 0;
	numContentLines = 0;
}

ContentCreator::~ContentCreator(void)
{
}

void ContentCreator::loadContentFileToStream(std::string filename)
{
	//nested ifs to continue only if previous success
	if(OpenContentFile(filename))
	{
		if(CreateCacheStream())
		{
			CloseContentFile();
		}
	}
}

void collectStringLine(char* buffer, std::string* string)
{
	assert(buffer != 0);
	assert(string != 0);

	unsigned int i = 0;
	while(string[0][i] != '\n')
	{
		buffer[i] = string[0][i];
	}
}

void ContentCreator::loadContentCacheToData()
{
	if(!mCacheStream)
	{
		Log::Error("No cache stream exsists!", "ContentCreator", this);
		return;
	}

	char buff[512];
	unsigned int i = 0;

	while(i < mCacheStream->size())
	{
		collectStringLine(buff, mCacheStream);

		//now scan the buffer and look for identifiers
	}
}

void ContentCreator::loadLevelDataFromContent(Level* level)
{
	if(!mData)
	{
		//log and early out
		Log::Error("No data to parse into level", "ContentCreator", this);
		return;
	}

	if(!mCacheStream)
	{
		Log::Error("No cache stream to parse into level", "ContentCreator", this);
		return;
	}

	//Iterate through mData and parse proper data and put it into the level
}

bool ContentCreator::OpenContentFile(std::string filename)
{
	if(!mFileStream.is_open())
		mFileStream.open(filename.c_str());
	else
	{
		Log::Error("Content File is already open!", "ContentCreator", this);
		return false;
	}

	if(mFileStream.fail())
	{
		Log::Error("Conent File could not be opened.", "ContentCreator", this);
		return false;
	}

	return true;
}

bool ContentCreator::CloseContentFile()
{
	if(mFileStream.is_open())
	{
		mFileStream.close();
		return true;
	}
	else
		return false;
}

bool ContentCreator::CreateCacheStream()
{
	try
	{
		mCacheStream = new std::string();
	}
	catch(std::bad_alloc)
	{
		throw(&std::bad_alloc());
		return false;
	}

	//now load all data from file into stream
	if(!mFileStream.is_open())
	{
		//log error and early out
		Log::Error("No file is currently opened to load data!", "ContentCreator", this);
		return false;
	}

	//load all data from content file into memory and keep track of identifiers
	while(!mFileStream.eof())
	{
		std::string temp;
		std::getline(mFileStream, temp);

		temp.append("\n");
		mCacheStream->append(temp);
	}
	#if defined(DEBUG) | defined(_DEBUG)
		Log::Inform(mCacheStream->c_str(), "ContentCreator Debug Data", this);
	#endif

	return true;
}

bool ContentCreator::CloseCacheStream()
{
	if(mCacheStream)
	{
		delete mCacheStream;
	}

	return true;
}

void ContentCreator::parseFPSPlayer()
{
}

void ContentCreator::parseProp()
{
}

void ContentCreator::Shutdown()
{
	CloseContentFile();
	CloseCacheStream();

	if(mData > 0)
	{
		if(sizeof(mData) == sizeof(ParsedData))
			delete mData;
		else
			delete [] mData;
	}
}