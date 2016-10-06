#include <string>
#include "Table.h"

#ifndef HEIGHT_MAP_H
#define HEIGHT_MAP_H

enum RAWBITSIZE{
	EIGHTBIT = 0,
	SIXTEENBIT
};

class HeightMap
{
public:
	HeightMap(void);
	HeightMap(unsigned int i, unsigned int j);
	HeightMap(unsigned int i, unsigned int j,
		const std::string& filename,
		float heightScale, float heightOffset,
		RAWBITSIZE dataSize);

	~HeightMap(void);

	bool loadRAW8(unsigned int i, unsigned int j,
		const std::string& filename,
		float heightScale, float heightOffset);

	bool loadRAW16(unsigned int i, unsigned int j,
		const std::string& filename,
		float heightScale, float heightOffset);

	const int getRows() const;
	const int getColumns() const;

	float& operator()(unsigned int i, unsigned int j);

	const float& operator()(unsigned int i, unsigned int j) const;

	void resize(unsigned int i, unsigned int j);

private:
	bool inBounds(unsigned int i, unsigned int j);
	float sampleHeight(unsigned int i, unsigned int j);
	void filter();

	std::string mFilename;
	Table<float> mHeightMap;
	float mHeightScale;
	float mHeightOffset;
};

#endif

