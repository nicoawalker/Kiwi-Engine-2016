#ifndef _KIWI_STATICMESHASSET_H_
#define _KIWI_STATICMESHASSET_H_

#include "Color.h"
#include "Mesh.h"

#include "../Core/IAsset.h"
#include "../Core/Vector2d.h"
#include "../Core/Vector3d.h"

#include <vector>
#include <string>

namespace Kiwi
{

	class StaticMeshAsset :
		public Kiwi::IAsset
	{
	protected:

		std::vector<Kiwi::Vector3d> m_vertices;
		std::vector<Kiwi::Vector2d> m_uvs;
		std::vector<Kiwi::Vector3d> m_normals;
		std::vector<Kiwi::Color> m_colors;
		std::vector<unsigned long> m_indices;

		std::vector<Kiwi::Mesh::Submesh> m_submeshes;

	public:

		StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals );
		StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<unsigned long>& indices );
		StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<Kiwi::Color>& vertexColors );
		StaticMeshAsset( std::wstring name, std::vector<Kiwi::Mesh::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<unsigned long>& indices, const std::vector<Kiwi::Color>& vertexColors );
		~StaticMeshAsset();

		const std::vector<Kiwi::Vector3d>& GetVertices()const { return m_vertices; }
		const std::vector<Kiwi::Vector2d>& GetUVs()const { return m_uvs; }
		const std::vector<Kiwi::Vector3d>& GetNormals()const { return m_normals; }
		const std::vector<unsigned long>& GetIndices()const { return m_indices; }
		const std::vector<Kiwi::Color>& GetColors()const { return m_colors; }
		const std::vector<Kiwi::Mesh::Submesh>& GetSubmeshes()const { return m_submeshes; }

	};

}

#endif