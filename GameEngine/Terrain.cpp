#include "Terrain.h"


Terrain::Terrain(void)
{
	mHeightMap = 0;
	mTexture = 0;
	mMovement = 0;
	mMesh = 0;
	mType = MESHED;
}

Terrain::Terrain(HeightMap heightmap, Texture texture, Movement movement)
{
	mHeightMap = new HeightMap(heightmap);
	mTexture = new Texture(texture);
	mMovement = new Movement(movement);
	mMesh = 0;
	mType = MAPPED;
	createGrid();
}

Terrain::Terrain(Texture texture, Mesh mesh, Movement movement)
{
	mHeightMap = 0;
	mTexture = new Texture(texture);
	mMovement = new Movement(movement);
	mMesh = new Mesh(mesh);
	mType = MESHED;
	mMesh->loadMeshFromOBJ();
}

Terrain::~Terrain(void)
{
}

void Terrain::Load()
{
	mMesh->createMesh();

	mTexture->loadTextureFromFile();

	mMovement->transformToWorld();
}

void Terrain::Update(float dt)
{
}

void Terrain::Release()
{
	mMesh->ReleaseMesh();

	if(mMesh)
		delete mMesh;
	if(mHeightMap)
		delete mHeightMap;
	if(mTexture)
		delete mTexture;
	if(mMovement)
		delete mMovement;
}

void Terrain::Render()
{
	mMesh->RenderMeshes();
}

void Terrain::createGrid()
{
	assert(mHeightMap);
	assert(!mMesh);
	assert(mHeightMap->getColumns());
	assert(mHeightMap->getRows());

	std::vector<DXVertex> verticeslist;
	std::vector<unsigned long> indices;

	int cols = mHeightMap->getColumns();
	int rows = mHeightMap->getRows();

	verticeslist.resize(cols * rows);
	indices.resize((cols - 1) * (rows - 1) * 2 * 3);

	DXVertex** vertices = new DXVertex*[rows];
	for(int i = 0; i < rows; ++i)
	{
		vertices[i] = new DXVertex[cols];
	}

	unsigned int index = 0;
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			vertices[i][j].position = XMFLOAT3(
				(float)i,
				(float)(*mHeightMap)((int)i, (int)j),
				(float)j);

			vertices[i][j].uv =	XMFLOAT2(
				(float)i / ((float)rows - 1),
				(float)j / ((float)cols - 1));

			vertices[i][j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			verticeslist[index] = vertices[i][j];
			index++;
		}
	}



	/*
	unsigned int cols = mHeightMap->getColumns();
	unsigned int rows = mHeightMap->getRows();
	unsigned int tot = cols * rows;

	vertices.resize(tot);

	unsigned int numIndices = (rows * 2) * (cols - 1) + (cols - 2);
	indices.resize(numIndices);

	XMFLOAT3 norm(0.0f, 1.0f, 0.0f);
	unsigned int index = 0;
	for(unsigned int j = 0; j < cols; ++j)
	{
		for(unsigned int i = 0; i < rows; ++i)
		{
			//unsigned int index = j + i * cols;
			float halfWidth = ((float)rows - 1.0f) / 2.0f;
			float halfLength = ((float)cols - 1.0f) / 2.0f;

			XMFLOAT3 position(
				(float)i - halfWidth,
				0.0f,
				(float)j - halfLength);
			
			XMFLOAT2 uv(
				(float)i / ((float)rows - 1),
				(float)j / ((float)cols - 1));
			
			index = j * cols + i;
			vertices[index].position = position;
			vertices[index].normal = norm;
			vertices[index].uv = uv;
			//index++;
		}
	}

	index = 0;
	for(unsigned int z = 0; z < cols - 1; ++z)
	{
		if(z % 2 == 0)
		{
			int x;
			for(x = 0; x < rows; ++x)
			{
				indices[index++] = x + (z * rows);
				indices[index++] = x + (z * rows) + cols;
			}

			if(z != cols - 2)
			{
				indices[index++] = --x + (z * rows);
			}
		}
		else
		{
			int x;
			for(x = rows - 1; x >= 0; x--)
			{
				indices[index++] = x + (z * rows);
				indices[index++] = x + (z * rows) + rows;
			}
			if(z != cols - 2)
			{
				indices[index++] = ++x + (z * rows);
			}
		}
	}
	*/
	index = 0;
	for(int i = 0; i < rows - 1; ++i)
	{
		for(int j = 0; j < cols - 1; ++j)
		{
			int k = (i * rows) + j;
			indices[index] = k;
			indices[index+1] = k + 1;
			indices[index+2] = k + rows;
			indices[index+3] = k + 1;
			indices[index+4] = k + 1 + rows;
			indices[index+5] = k + rows;
			index+=6;

		}
	}
	/*
	index = 0;
	for(unsigned int j = 0; j < rows; ++j)
	{
		for(unsigned int i = 0; i < cols; ++i)
		{
			unsigned int k = j * rows + i;
			indices[index++] = k;
			indices[index++] = k + 1;
			indices[index++] = k + rows;
			indices[index++] = k + 1;
			indices[index++] = k + 1 + rows;
			indices[index++] = k + rows;

		}
	}
	*/
	mMesh = new Mesh(verticeslist, indices);

	for(int i = 0; i < rows; ++i){
		delete [] vertices[i];
	}
	delete [] vertices;
	/*
	unsigned int numVerts = mHeightMap->getRows() * mHeightMap->getColumns();
	unsigned int numCellRows = mHeightMap->getRows() - 1;
	unsigned int numCellColumns = mHeightMap->getColumns() - 1;
	unsigned int numTriangles = numCellRows * numCellColumns * 2;

	std::vector<DXVertex> vertices;
	std::vector<unsigned long> indices;

	vertices.resize(numVerts);

	//the position of vertices in local coordinates for the grid
	unsigned int k = 0;
	for(unsigned int i = 0; i < mHeightMap->getRows(); ++i)
	{
		for(unsigned int j = 0; j < mHeightMap->getColumns(); ++j)
		{
			k = i * mHeightMap->getColumns() + j;

			float y = 0.0f;
			if(mHeightMap)
				y = (*mHeightMap)(i, j);

			vertices[k].position.x = (float)j;
			vertices[k].position.y = y;
			vertices[k].position.z = (float)i;
		}
	}

	//indices
	indices.resize(numTriangles * 3);
	k = 0;
	for(unsigned int i = 0; i < numCellRows; ++i)
	{
		for(unsigned int j = 0; j < numCellColumns; ++j)
		{
			indices[k] = (i * mHeightMap->getColumns() + j);
			indices[k + 1] = (i * mHeightMap->getColumns() + j + 1);
			indices[k + 2] = ((i + 1) * mHeightMap->getColumns() + j);

			indices[k + 3] = ((i + 1) * mHeightMap->getColumns() + j);
			indices[k + 4] = (i * mHeightMap->getColumns() + j + 1);
			indices[k + 5] = ((i + 1) * mHeightMap->getColumns() + j + 1);

			//next
			k += 6;
		}
	}

	//normals
	for(int i = 0; i < indices.size() / 3; ++i)
	{
		unsigned int i1 = indices[i * 3];
		unsigned int i2 = indices[i * 3 + 1];
		unsigned int i3 = indices[i * 3 + 2];

		XMVECTOR v1 = XMLoadFloat3(&vertices[i1].position);
		XMVECTOR v2 = XMLoadFloat3(&vertices[i2].position);
		XMVECTOR v3 = XMLoadFloat3(&vertices[i3].position);

		XMVECTOR s1 = v1 - v3;
		XMVECTOR s2 = v1 - v2;
		XMVECTOR norm = XMVector3Cross(s1, s2);

		XMVector3Normalize(norm);

		XMStoreFloat3(&vertices[i1].normal, norm);
		XMStoreFloat3(&vertices[i2].normal, norm);
		XMStoreFloat3(&vertices[i3].normal, norm);
	}

	//texture coordinates
	float du = 1.0f / (mHeightMap->getColumns()-1);
	float dv = 1.0f / (mHeightMap->getRows() - 1);
	k = 0;
	for(unsigned int i = 0; i < mHeightMap->getRows(); ++i)
	{
		for(unsigned int j = 0; j < mHeightMap->getColumns(); ++j)
		{
			k = i * mHeightMap->getColumns() + j;
			vertices[k].uv.x = j * du;
			vertices[k].uv.y = i * dv;
		}
	}

	try
	{
		mMesh = new Mesh(vertices, indices);
	}catch(std::bad_alloc){
		Log::Error("Could not create Terrain Mesh Grid", "Terrain", this);
		throw(&std::bad_alloc());
	}
	*/
	Log::Inform("Terrain Mesh created Successfully", "Terrain", this);
}

const Mesh* Terrain::getMesh() const
{
	return mMesh;
}

const Texture* Terrain::getTexture() const
{
	return mTexture;
}

const Movement* Terrain::getMovement() const
{
	return mMovement;
}

Mesh* Terrain::getMesh()
{
	return mMesh;
}

Texture* Terrain::getTexture()
{
	return mTexture;
}

Movement* Terrain::getMovement()
{
	return mMovement;
}