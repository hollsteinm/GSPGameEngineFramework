//(c) copyright Martin Hollstein

#include "Mesh.h"
#include "DXManager.h"
#include <sstream>

Mesh::Mesh(void)
{
	mMeshFileName = "../Models/Box.obj";

	mVertexBuffer = NULL;
	mIndexBuffer = NULL;

	mVertexCount = 0;
	mIndexCount = 0;

	//mMeshVertices.clear();
	mIndices.clear();
}

Mesh::Mesh(std::string meshFileName)
{
	mMeshFileName = meshFileName;

	mVertexBuffer = NULL;
	mIndexBuffer = NULL;

	mVertexCount = 0;
	mIndexCount = 0;

	//mMeshVertices.clear();
	mIndices.clear();
}

Mesh::Mesh(const std::vector<DXVertex>& vertices, const std::vector<unsigned long>& indices)
{
	mMeshFileName = "";

	mVertexBuffer = NULL;
	mIndexBuffer = NULL;

	mVertexCount = vertices.size();
	mIndexCount = indices.size();

	mIndices.clear();

	try
	{
		mMeshVertices = new DXVertex[mVertexCount];
		mIndices2 = new unsigned long[mIndexCount];
	}catch(std::bad_alloc){
		throw(&std::bad_alloc());
	}

	for(unsigned int i = 0; i < mVertexCount; ++i)
	{
		mMeshVertices[i] = vertices[i];
	}

	for(unsigned int i = 0; i < mIndexCount; ++i)
	{
		mIndices2[i] = indices[i];
	}
	calculateTangentBinormal();
}

Mesh::~Mesh(void)
{
	if(mVertexBuffer)
		mVertexBuffer->Release();

	if(mIndexBuffer)
		mIndexBuffer->Release();

	mIndices.clear();
}

void Mesh::loadMeshFromOBJ()
{
	std::ifstream fin;
	char input, input2;

	//counts
	UINT vc = 0;
	UINT tc = 0;
	UINT nc = 0;
	UINT fc = 0;

	fin.open(mMeshFileName);

	if(fin.fail())
	{
		Log::Error("Could not open specified file", "Mesh", this);
		return;
	}

	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ')
				vc++;
			if(input == 't')
				tc++;
			if(input == 'n')
				nc++;
		}
		if(input == 'f')
		{
			fin.get(input);
			if(input == ' ')
				fc++;
		}

		while(input!='\n')
			fin.get(input);

		fin.get(input);
	}

	fin.close();

	XMFLOAT3 *inV;
	XMFLOAT2 *inTC;
	XMFLOAT3 *inN;
	DXIndex *inI;

	inV = new XMFLOAT3[vc];
	inTC = new XMFLOAT2[tc];
	inN = new XMFLOAT3[nc];
	inI = new DXIndex[fc];

	int vI = 0;
	int tI = 0;
	int nI = 0;
	int iI = 0;

	fin.open(mMeshFileName);

	if(fin.fail())
	{
		Log::Error("Could not open specified file", "Mesh", this);
		return;
	}

	fin.get(input);

	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);

			if(input == ' ')
			{
				fin >> inV[vI].x >> inV[vI].y >> inV[vI].z;

				//invert
				inV[vI].z = inV[vI].z * -1.0f;
				vI++;
			}

			if(input == 't')
			{
				fin >> inTC[tI].x >> inTC[tI].y;

				//invert
				inTC[tI].y = 1.0f - inTC[tI].y;
				tI++;
			}

			if(input == 'n')
			{
				fin >> inN[nI].x >> inN[nI].y >> inN[nI].z;

				//invert
				inN[nI].z = inN[nI].z * -1.0f;
				nI++;
			}
		}

		if(input == 'f')
		{
			fin.get(input);

			if(input == ' ')
			{
				fin >> inI[iI].c >> input2 >> inI[iI].f >> input2 >> inI[iI].i
					>> inI[iI].b >> input2 >> inI[iI].e >> input2 >> inI[iI].h
					>> inI[iI].a >> input2 >> inI[iI].d >> input2 >> inI[iI].g;
				iI++;
			}
		}

		while(input != '\n')
			fin.get(input);

		fin.get(input);
	}

	fin.close();

	mVertexCount = fc * 3;
	mIndexCount = fc * 3;
	mMeshVertices = new DXVertex[mVertexCount];
	mIndices.resize(mIndexCount);

	DXVertex zero = 
		{
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		};

	//zero out values
	for(UINT i = 0; i < mVertexCount; ++i)
	{
		mMeshVertices[i] = zero;
	}

	for(UINT i = 0; i < fc; ++i)
		mIndices[i] = inI[i];

	DXVertex* buff = new DXVertex[mVertexCount];

	std::stringstream temp;
	//std::ofstream temp;
	//temp.open("../Models/temp.txt");

	for(int i = 0; i < iI; ++i)
	{
		vI = inI[i].a - 1;
		tI = inI[i].d - 1;
		nI = inI[i].g - 1;

		temp << inV[vI].x << ' ' << inV[vI].y << ' ' << inV[vI].z << ' '
			<< inTC[tI].x << ' ' << inTC[tI].y << ' '
			<< inN[nI].x << ' ' << inN[nI].y << ' ' << inN[nI].z << std::endl;

		vI = inI[i].b - 1;
		tI = inI[i].e - 1;
		nI = inI[i].h - 1;

		temp << inV[vI].x << ' ' << inV[vI].y << ' ' << inV[vI].z << ' '
			<< inTC[tI].x << ' ' << inTC[tI].y << ' '
			<< inN[nI].x << ' ' << inN[nI].y << ' ' << inN[nI].z << std::endl;

		vI = inI[i].c - 1;
		tI = inI[i].f - 1;
		nI = inI[i].i - 1;

		temp << inV[vI].x << ' ' << inV[vI].y << ' ' << inV[vI].z << ' '
			<< inTC[tI].x << ' ' << inTC[tI].y << ' '
			<< inN[nI].x << ' ' << inN[nI].y << ' ' << inN[nI].z << std::endl;
	}

	//temp.close();
	//fin.open("../Models/temp.txt");

	mIndices2 = new unsigned long[mVertexCount];

	for(UINT i = 0; i < mVertexCount; ++i)
	{
		temp >> mMeshVertices[i].position.x >> mMeshVertices[i].position.y >> mMeshVertices[i].position.z
			>> mMeshVertices[i].uv.x >> mMeshVertices[i].uv.y
			>> mMeshVertices[i].normal.x >> mMeshVertices[i].normal.y >> mMeshVertices[i].normal.z;
	
			mIndices2[i] = i;
	}

	//fin.close();

	delete inV;
	delete inTC;
	delete inN;
	delete inI;
	delete [] buff;

	calculateTangentBinormal();

	Log::Inform(".obj file parsed sucessfully", "Mesh", this);
}

void Mesh::createMesh()
{

	D3D10_BUFFER_DESC vertexbd, indexbd;
	D3D10_SUBRESOURCE_DATA vData, iData;
	
	vertexbd.Usage = D3D10_USAGE_DEFAULT;
	vertexbd.ByteWidth = sizeof(DXVertex) * mVertexCount;
	vertexbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexbd.CPUAccessFlags = 0;
	vertexbd.MiscFlags = 0;

	vData.pSysMem = mMeshVertices;

	if(FAILED(CDXManager::instance()->getDevice()->CreateBuffer(&vertexbd, &vData, &mVertexBuffer)))
		Log::Error("Could not create vertex buffer", "Mesh", this);
	else
		Log::Inform("Created vertex buffer", "Mesh", this);

	indexbd.Usage = D3D10_USAGE_DEFAULT;
	indexbd.ByteWidth = sizeof(unsigned long) * mIndexCount;
	indexbd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexbd.CPUAccessFlags = 0;
	indexbd.MiscFlags = 0;

	iData.pSysMem = mIndices2;

	if(FAILED(CDXManager::instance()->getDevice()->CreateBuffer(&indexbd, &iData, &mIndexBuffer)))
		Log::Error("Could not create index buffer", "Mesh", this);
	else
		Log::Inform("Created index buffer", "Mesh", this);	
	
}

void Mesh::RenderMeshes()
{
	unsigned int stride = sizeof(DXVertex);
	unsigned int offset = 0;
	
	CDXManager::instance()->getDevice()->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	CDXManager::instance()->getDevice()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		
	CDXManager::instance()->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Mesh::calculateTangentBinormal()
{
	DXVertex temp1, temp2, temp3;
	unsigned int numFaces = mVertexCount / 3;
	unsigned int index = 0;

	for(unsigned int i = 0; i < numFaces; ++i)
	{
		//get the three vertices for the face
		temp1.position.x = mMeshVertices[index].position.x;
		temp1.position.y = mMeshVertices[index].position.y;
		temp1.position.z = mMeshVertices[index].position.z;

		temp1.uv.x = mMeshVertices[index].uv.x;
		temp1.uv.y = mMeshVertices[index].uv.y;

		temp1.normal.x = mMeshVertices[index].normal.x;
		temp1.normal.y = mMeshVertices[index].normal.y;
		temp1.normal.z = mMeshVertices[index].normal.z;
		index++;

		temp2.position.x = mMeshVertices[index].position.x;
		temp2.position.y = mMeshVertices[index].position.y;
		temp2.position.z = mMeshVertices[index].position.z;

		temp2.uv.x = mMeshVertices[index].uv.x;
		temp2.uv.y = mMeshVertices[index].uv.y;

		temp2.normal.x = mMeshVertices[index].normal.x;
		temp2.normal.y = mMeshVertices[index].normal.y;
		temp2.normal.z = mMeshVertices[index].normal.z;
		index++;

		temp3.position.x = mMeshVertices[index].position.x;
		temp3.position.y = mMeshVertices[index].position.y;
		temp3.position.z = mMeshVertices[index].position.z;

		temp3.uv.x = mMeshVertices[index].uv.x;
		temp3.uv.y = mMeshVertices[index].uv.y;

		temp3.normal.x = mMeshVertices[index].normal.x;
		temp3.normal.y = mMeshVertices[index].normal.y;
		temp3.normal.z = mMeshVertices[index].normal.z;
		index++;

		//calculate tangent and binormal of face
		XMFLOAT3 vec1, vec2, tangent, binormal, normal;
		XMFLOAT2 texCoord1, texCoord2;
		float denominator;
		float length;

		//calculate the vectors for face
		vec1.x = temp2.position.x - temp1.position.x;
		vec1.y = temp2.position.y - temp1.position.y;
		vec1.z = temp2.position.z - temp1.position.z;

		vec2.x = temp3.position.x - temp1.position.x;
		vec2.y = temp3.position.y - temp1.position.y;
		vec2.z = temp3.position.z - temp1.position.z;

		texCoord1.x = temp2.uv.x - temp1.uv.x;
		texCoord1.y = temp2.uv.y - temp1.uv.y;

		texCoord2.x = temp3.uv.x - temp2.uv.x;
		texCoord2.y = temp3.uv.y - temp2.uv.y;

		//denominator of tangent/binormal equation
		denominator = 1.0f / ((texCoord1.x * texCoord2.y) - (texCoord2.x * texCoord1.y));

		//cross products
		tangent.x = (texCoord2.y * vec1.x - texCoord1.y * vec2.x) * denominator;
		tangent.y = (texCoord2.y * vec1.y - texCoord1.y * vec2.y) * denominator;
		tangent.z = (texCoord2.y * vec1.z - texCoord1.y * vec2.z) * denominator;

		binormal.x = (texCoord1.x * vec2.x - texCoord2.x * vec1.x) * denominator;
		binormal.y = (texCoord1.x * vec2.y - texCoord2.x * vec1.y) * denominator;
		binormal.z = (texCoord1.x * vec2.z - texCoord2.x * vec1.z) * denominator;


		//calculate new normal of face using tangent and binormal
		length = sqrt(powf(tangent.x, 2.0f) + powf(tangent.y, 2.0f) + powf(tangent.z, 2.0f));
		tangent.x = tangent.x / length;
		tangent.y = tangent.y / length;
		tangent.z = tangent.z / length;

		length = sqrt(powf(binormal.x, 2.0f) + powf(binormal.y, 2.0f) + powf(binormal.z, 2.0f));
		binormal.x = binormal.x / length;
		binormal.y = binormal.y / length;
		binormal.z = binormal.z / length;

		normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
		normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
		normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

		length = sqrt(powf(normal.x, 2.0f) + powf(normal.y, 2.0f) + powf(normal.z, 2.0f));

		normal.x = normal.x / length;
		normal.y = normal.y / length;
		normal.z = normal.z / length;

		//Store new values
		int storeIndex = 0;
		storeIndex = index - 3;
		mMeshVertices[storeIndex].normal.x = normal.x;
		mMeshVertices[storeIndex].normal.y = normal.y;
		mMeshVertices[storeIndex].normal.z = normal.z;

		mMeshVertices[storeIndex].tangent.x = tangent.x;
		mMeshVertices[storeIndex].tangent.y = tangent.y;
		mMeshVertices[storeIndex].tangent.z = tangent.z;

		mMeshVertices[storeIndex].binormal.x = binormal.x;
		mMeshVertices[storeIndex].binormal.y = binormal.y;
		mMeshVertices[storeIndex].binormal.z = binormal.z;

		storeIndex++;
		mMeshVertices[storeIndex].normal.x = normal.x;
		mMeshVertices[storeIndex].normal.y = normal.y;
		mMeshVertices[storeIndex].normal.z = normal.z;

		mMeshVertices[storeIndex].tangent.x = tangent.x;
		mMeshVertices[storeIndex].tangent.y = tangent.y;
		mMeshVertices[storeIndex].tangent.z = tangent.z;

		mMeshVertices[storeIndex].binormal.x = binormal.x;
		mMeshVertices[storeIndex].binormal.y = binormal.y;
		mMeshVertices[storeIndex].binormal.z = binormal.z;

		storeIndex++;
		mMeshVertices[storeIndex].normal.x = normal.x;
		mMeshVertices[storeIndex].normal.y = normal.y;
		mMeshVertices[storeIndex].normal.z = normal.z;

		mMeshVertices[storeIndex].tangent.x = tangent.x;
		mMeshVertices[storeIndex].tangent.y = tangent.y;
		mMeshVertices[storeIndex].tangent.z = tangent.z;

		mMeshVertices[storeIndex].binormal.x = binormal.x;
		mMeshVertices[storeIndex].binormal.y = binormal.y;
		mMeshVertices[storeIndex].binormal.z = binormal.z;
	}	
}

unsigned int Mesh::getNumIndices()
{
	return mIndexCount;
}

void Mesh::ReleaseMesh()
{
	if(mIndices2)
		delete mIndices2;

	if(mMeshVertices)
		delete mMeshVertices;
}

const DXVertex* Mesh::getVertices() const
{
	return mMeshVertices;
}

unsigned int Mesh::getNumVertices()
{
	return mVertexCount;
}