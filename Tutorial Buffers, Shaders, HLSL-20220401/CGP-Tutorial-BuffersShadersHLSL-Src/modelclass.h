////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h 
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;  /*X,Y,Z float*/
	    XMFLOAT4 color;     /*R,B,G,A float*/
	}; //구조체를 통해서 위치와 색깔 정보를 넣음 -> 다각형을 만듬

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer; //vertex위치를 넣음, indexbuffer=vertex 순서 넣기 
	int m_vertexCount, m_indexCount;
};
//모든 기하학 정보를 같고 있음
#endif