#include "StaticMeshAsset.h"

namespace Kiwi
{

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals ):
		Kiwi::IAsset( name, L"StaticMesh" )
	{

		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;

	}

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<unsigned long>& indices ) :
		Kiwi::IAsset( name, L"StaticMesh" )
	{

		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;
		m_indices = indices;

	}

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<Kiwi::Color>& vertexColors ) :
		Kiwi::IAsset( name, L"StaticMesh" )
	{

		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;
		m_colors = vertexColors;

	}

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<unsigned long>& indices, const std::vector<Kiwi::Color>& vertexColors ) :
		Kiwi::IAsset( name, L"StaticMesh" )
	{
		
		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;
		m_indices = indices;
		m_colors = vertexColors;

	}

	StaticMeshAsset::~StaticMeshAsset()
	{
	}

}