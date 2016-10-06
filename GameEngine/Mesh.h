//(c) copyright Martin Hollstein

#include "DX10Util.h"

#ifndef MESH_H
#define MESH_H

//custom struct data for vertices
struct DXVertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
};

struct DXIndex
{
	int a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		i;
};

class Mesh
{
public:
	//Name: Mesh()
	//Purpose: Default constructor
	//Parameters: none
	//Return: none
	Mesh(void);

	//Name: Mesh()
	//Purpose: constructor
	//Parameters: string of the file that the mesh is stored in
	//Return: none
	Mesh(std::string meshFileName);

	//Name: Mesh()
	//Purpose: constructor
	//Parameters: vertices, indices, and their respective counts
	//Return: none
	Mesh(const std::vector<DXVertex>& vertices, const std::vector<unsigned long>& indices); 

	//Name:~Mesh()
	//Purpose: destructor
	//Parameters: none
	//Return: none
	~Mesh(void);

	//Name: loadMeshFromOBJ()
	//Purpose: loads .obj file of mesh into mesh
	//Parameters: none
	//return: none
	void loadMeshFromOBJ();

	//Name: CreateMesh()
	//purpose: takes vertex data to create a mesh
	//Parameters: none
	//return: none
	void createMesh();

	//Name: RenderMeshes()
	//Purpose: renders the mesh using the D3D10 device
	//Parameters: none
	//return: none
	void RenderMeshes();

	//Name: getNumIndices()
	//Purpose: getter for the number of indices in mesh
	//Parameters: none
	//return: number of indices
	unsigned int getNumIndices();

	//Name: getNumVertices()
	//Purpose: getter for the number of vertices in mesh
	//Parameters: none
	//return: number of vertices
	unsigned int getNumVertices();

	//Name: getVertices()
	//Purpose: returns the vertex struct of the vertices of mesh
	//Parameters: none
	//return: const pointer to vertex struct
	const DXVertex* getVertices() const;

	//Name:ReleaseMesh()
	//Purpose: Memory cleanup
	//Parameters: none
	//return: none
	void ReleaseMesh();

private:
	//Name: CalculateTangentBinormal()
	//Purpose: determines the tangents and binormals of the mesh
	//Parameters: none
	//return: none
	void calculateTangentBinormal();

	std::string mMeshFileName;

	ID3D10Buffer* mVertexBuffer;
	ID3D10Buffer* mIndexBuffer;

	unsigned int mVertexCount;
	unsigned int mIndexCount;

	DXVertex* mMeshVertices;
	std::vector<DXIndex> mIndices;

	unsigned long* mIndices2;
};

#endif