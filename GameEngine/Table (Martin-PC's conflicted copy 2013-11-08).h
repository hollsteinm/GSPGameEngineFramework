#include <assert.h>
#include <vector>

#ifndef TABLE_H
#define TABLE_H

template<typename T>
class Table
{
public:
	Table(void);
	~Table(void);

	/////////////////////
	//iterator typedefs//
	/////////////////////
	typedef typename std::vector<T>::iterator iter;
	typedef typename std::vector<T>::const_interator citer;

private:
	unsigned int mRows;
	unsigned int mColumns;
	std::vector<T> mMatrix;
};

#endif

