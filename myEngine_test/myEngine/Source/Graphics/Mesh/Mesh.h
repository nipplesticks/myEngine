#pragma once
#include <string>
#include <vector>
#include "../GraphicsExtern.h"
#include "Skeleton.h"

class Mesh
{
public:
	Mesh() = default;
	Mesh(const Mesh & other);
	~Mesh();

	bool LoadMesh(const std::string & path);
	const std::vector<myStructs::Vertex> * GetVertices() const;
	UINT GetNumberOfVertices() const;
	const D3D12_VERTEX_BUFFER_VIEW & GetVertexBufferView() const;

	void SetVertices(const std::vector<myStructs::Vertex> & vertices);
	void SetPrivateSkeleton(const Skeleton & skeleton);
	void SetSharedSkeleton(Skeleton * skeleton);

	Skeleton * GetSkeleton();

	void Update(double dt);

	Mesh& operator=(const Mesh & other);

private:
	UINT m_vertexBufferSize = 0u;
	ID3D12Resource * m_vertexBuffer = nullptr;
	ID3D12Resource * m_vertexUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	struct SkeletonVars
	{
		bool		isPrivate = false;
		Skeleton *	skeleton = nullptr;
	} m_skeletonVars;

	std::vector<myStructs::Vertex> m_vertices;

private:
	void _copy(const Mesh & other);
	void _cleanup();
};