#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include <fstream>
#include <vector>

class Mesh
{

public:
	Mesh(Vertex *verticesParam, int vertexNum, unsigned int *usIndices,
		int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> device); //Parameterized Constructor

	Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D11Device> device); //second constructor for OBJ

	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> dContext);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	void CreateBuffers(Vertex *verticesParam, int vertexNum, unsigned int *usIndices,
		int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> device);

	int indexCount; //For Drawing
};

