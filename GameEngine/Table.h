#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <cassert>

template<typename T>
class Table
{
public:

	typedef typename std::vector<T>::iterator iter;
	typedef typename std::vector<T>::const_iterator citer;

	Table(void)
	{
		mRows = 0;
		mColumns = 0;
	}
	
	Table(unsigned int i, unsigned int j)
	{
		mRows = i;
		mColumns = j;
		mMatrix.resize(i * j);
	}

	
	Table(unsigned int i, unsigned int j, const T& value)
	{
		mRows = i;
		mColumns = j;
		mMatrix.resize(i * j);
		mMatrix = value;
	}
	
	
	~Table(void)
	{
	}

	const T& operator()(unsigned int i, unsigned int j) const
	{
		return mMatrix[i * mColumns + j];
	}
	
	
	T& operator()(unsigned int i, unsigned int j)
	{
		return mMatrix[i * mColumns + j];
	}
	
	iter begin()
	{
		return mMatrix.begin();
	}
	
	iter end()
	{
		return mMatrix.end();
	}
	
	
	citer begin() const
	{
		return mMatrix.begin();
	}
	
	citer end() const
	{
		return mMatrix.end();
	}
	
	void resize(unsigned int i, unsigned int j)
	{
		mRows = i;
		mColumns = j;
		mMatrix.resize(i * j);
	}
	
	void resize(unsigned int i, unsigned int j, const T& value)
	{
		mRows = i;
		mColumns = j;
		mMatrix.resize(i * j, value);
	}
	
	unsigned int getRows() const
	{
		return mRows;
	}
	
	unsigned int getColumns() const
	{
		return mColumns;
	}
	
private:
	unsigned int mRows;
	unsigned int mColumns;
	std::vector<T> mMatrix;
};
	
#endif

