#include "Mesh.h"

bool Mesh::LoadMesh(const std::string & path)
{
	// TODO :: Load mesh from Assimp

	return false;
}

const std::vector<myStructs::Vertex>* Mesh::GetVertices() const
{
	return &m_vertices;
}

UINT Mesh::GetNumberOfVertices() const
{
	return (UINT)m_vertices.size();
}

const D3D12_VERTEX_BUFFER_VIEW & Mesh::GetVertexBufferView() const
{
	return m_vertexBufferView;
}

void Mesh::SetVertices(const std::vector<myStructs::Vertex>& vertices)
{
	m_vertices = vertices;
}

void Mesh::SetPrivateSkeleton(const Skeleton & skeleton)
{
	if (m_skeletonVars.skeleton && m_skeletonVars.isPrivate)
	{
		delete m_skeletonVars.skeleton;
	}
	m_skeletonVars.skeleton = nullptr;

	m_skeletonVars.isPrivate = true;
	m_skeletonVars.skeleton = new Skeleton(skeleton);
}

void Mesh::SetSharedSkeleton(Skeleton * skeleton)
{
	if (m_skeletonVars.skeleton && m_skeletonVars.isPrivate)
	{
		delete m_skeletonVars.skeleton;
	}
	m_skeletonVars.skeleton = nullptr;

	m_skeletonVars.isPrivate = false;
	m_skeletonVars.skeleton = skeleton;
}

Skeleton * Mesh::GetSkeleton()
{
	return m_skeletonVars.skeleton;
}

void Mesh::Update(double dt)
{
	if (m_skeletonVars.skeleton)
	{
		// TODO:: update Skeleton
	}
}

Mesh & Mesh::operator=(const Mesh & other)
{
	if (this != &other)
	{
		_cleanup();
		_copy(other);
	}

	return *this;
}

void Mesh::_copy(const Mesh & other)
{
	if (other.m_skeletonVars.skeleton && other.m_skeletonVars.isPrivate)
	{
		m_skeletonVars.isPrivate = true;
		m_skeletonVars.skeleton = new Skeleton(*other.m_skeletonVars.skeleton);
	}
	else
	{
		m_skeletonVars.skeleton = other.m_skeletonVars.skeleton;
		m_skeletonVars.isPrivate = false;
	}

	// TODO :: COPY D3D12 Stuff
}

void Mesh::_cleanup()
{
	if (m_skeletonVars.skeleton && m_skeletonVars.isPrivate)
	{
		delete m_skeletonVars.skeleton;
	}
	m_skeletonVars.skeleton = nullptr;

	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_vertexUploadBuffer);
}

Mesh::Mesh(const Mesh & other)
{
	_copy(other);
}

Mesh::~Mesh()
{
	_cleanup();
}
