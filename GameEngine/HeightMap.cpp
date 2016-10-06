#include "HeightMap.h"
#include "Log.h"
#include <fstream>
HeightMap::HeightMap(void)
{
}

HeightMap::HeightMap(unsigned int i, unsigned int j)
{
	resize(i, j);
}

HeightMap::HeightMap(unsigned int i, unsigned int j,
	const std::string& filename,
	float heightScale, float heightOffset,
	RAWBITSIZE dataSize)
{
	switch(dataSize)
	{
	case EIGHTBIT:
		if(!loadRAW8(i, j, filename, heightScale, heightOffset))
		{
			std::string error = "Error loading 8-bit heightmap: ";
			error.append(filename);
			Log::Error(error, "HeightMap", this);
		}
		else
		{
			Log::Inform("Success loading 8-bit heightmap", "HeightMap", this);
		}

		break;

	case SIXTEENBIT:
		if(!loadRAW16(i, j, filename, heightScale, heightOffset))
		{
			std::string error = "Error loading 16-bit heightmap: ";
			error.append(filename);
			Log::Error(error, "HeightMap", this);
		}
		else
		{
			Log::Inform("Success loading 16-bit heightmap", "HeightMap", this);
		}
		break;

	default:
		std::string error = "Unrecognized identifier for bit size in file: ";
		error.append(filename);
		Log::Error(error, "HeightMap", this);
		break;
	};
}

HeightMap::~HeightMap(void)
{
}

void HeightMap::resize(unsigned int i, unsigned int j)
{
	mHeightScale = 1.0f;
	mHeightOffset = 0.0f;
	mHeightMap.resize(i, j, 0.0f);
}

bool HeightMap::loadRAW8(unsigned int i, unsigned int j,
	const std::string& filename,
	float heightScale, float heightOffset)
{
	mFilename = filename;
	mHeightScale = heightScale;
	mHeightOffset = heightOffset;

	std::vector<unsigned char> in(i * j);

	std::ifstream inFile;
	inFile.open(filename.c_str(), std::ios_base::binary);
	if(!inFile)
	{
		Log::Error("Error opening RAW 8-bit file", "HeightMap", this);
		return false;
	}

	inFile.read((char*)&in[0], (std::streamsize)in.size());

	inFile.close();

	mHeightMap.resize(i, j, 0);
	for(unsigned int ii = 0; ii < i; ++ii)
	{
		for(unsigned int jj = 0; jj < j; ++jj)
		{
			unsigned int k = ii * j + jj;
			mHeightMap(ii, jj) = (float)in[k] * heightScale + heightOffset;
		}
	}

	//TODO: not sure if i want to force this
	filter();
	return true;
}

bool HeightMap::loadRAW16(unsigned int i, unsigned int j,
	const std::string& filename,
	float heightScale, float heightOffset)
{
	mFilename = filename;
	mHeightScale = heightScale;
	mHeightOffset = heightOffset;

	std::vector<unsigned short> in(i * j);

	std::ifstream inFile;
	inFile.open(filename.c_str(), std::ios_base::binary);
	if(!inFile)
	{
		Log::Error("Error opening RAW 16-bit file", "HeightMap", this);
		return false;
	}

	inFile.read((char*)&in[0], (std::streamsize)in.size());

	inFile.close();

	mHeightMap.resize(i, j, 0);
	for(unsigned int ii = 0; ii < i; ++ii)
	{
		for(unsigned int jj = 0; jj < j; ++jj)
		{
			unsigned int k = ii * j + jj;
			mHeightMap(ii, jj) = (float)in[k] * heightScale + heightOffset;
		}
	}

	//TODO: not sure if i want to force this
	filter();
	return true;
}

const int HeightMap::getRows() const
{
	return mHeightMap.getRows();
}

const int HeightMap::getColumns() const
{
	return mHeightMap.getColumns();
}

float& HeightMap::operator()(unsigned int i, unsigned int j)
{
	return mHeightMap(i, j);
}

const float& HeightMap::operator()(unsigned int i, unsigned int j) const
{
	return mHeightMap(i, j);
}

void HeightMap::filter()
{
	Table<float> temp(mHeightMap.getRows(), mHeightMap.getColumns());

	for(unsigned int ii = 0; ii < mHeightMap.getRows(); ++ii)
	{
		for(unsigned int jj = 0; jj < mHeightMap.getColumns(); ++jj)
		{
			temp(ii, jj) = sampleHeight(ii, jj);
		}
	}

	mHeightMap = temp;
}

bool HeightMap::inBounds(unsigned int i, unsigned int j)
{
	return 
		i >= 0 &&
		i < (unsigned int)mHeightMap.getRows() &&
		j >= 0 &&
		j < (unsigned int)mHeightMap.getColumns();
}

float HeightMap::sampleHeight(unsigned int i, unsigned int j)
{
	float average = 0.0f;
	float number = 0.0f;

	for(unsigned int ii = i - 1; ii <= i + 1; ++ii)
	{
		for(unsigned int jj = j - 1; jj <= j + 1; ++jj)
		{
			if(inBounds(ii, jj))
			{
				average += mHeightMap(ii, jj);
				number += 1.0f;
			}
		}
	}

	return (average / number);
}