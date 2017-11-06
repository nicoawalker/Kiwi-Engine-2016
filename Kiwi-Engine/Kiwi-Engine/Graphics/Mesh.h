#ifndef _KIWI_MESH_H_
#define _KIWI_MESH_H_

#include "DirectX.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"

#include "../Core/Component.h"
#include "../Core/Math.h"
#include "../Core/IAsset.h"
#include "../Core/Vector2d.h"
#include "../Core/Vector3d.h"

#include <vector>
#include <string>

namespace Kiwi
{

	class StaticMeshAsset;

	class Mesh :
		public Kiwi::IAsset,
		public Kiwi::Component
	{
		friend class Material;

	public:

		struct Vertex
		{
			Vertex()
			{
				position = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
				tex = DirectX::XMFLOAT2( 0.0f, 0.0f );
				normal = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
				color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			}

			Vertex( const Kiwi::Vector3d& pos, const Kiwi::Vector2d& textureUVs, const Kiwi::Vector3d& normals )
			{
				Kiwi::Vector3dToXMFLOAT3( pos, position );
				Kiwi::Vector2dToXMFLOAT2( textureUVs, tex );
				Kiwi::Vector3dToXMFLOAT3( normals, normal );
				color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			}

			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 tex;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT4 color;
		};

		struct Triangle
		{
			Kiwi::Vector3d v1, v2, v3; //vertices
			Kiwi::Color c1, c2, c3; //colors
			Kiwi::Vector3d n1, n2, n3; //normals
			unsigned long i1, i2, i3; //indices

			Triangle()
			{
				i1 = i2 = i3 = 0;
			}
		};

		struct Submesh
		{
			Kiwi::Mesh* parent;
			Kiwi::Material material;
			unsigned long startIndex; //stores the position in the vertex buffer of this subset
			unsigned long endIndex;

			Submesh()
			{
				parent = 0;
				startIndex = 0;
				endIndex = 0;
			}
		};

		enum PRIMITIVE_TYPE { QUAD = 0, CUBE = 1 };

	protected:

		Kiwi::Renderer* m_renderer;

		Kiwi::PrimitiveTopology m_primitiveTopology;

		std::vector<Kiwi::Mesh::Submesh> m_submeshes;

		Kiwi::VertexBuffer<Kiwi::Mesh::Vertex>* m_vertexBuffer;
		Kiwi::IndexBuffer* m_indexBuffer;

		std::vector<Kiwi::Vector3d> m_vertices;
		std::vector<Kiwi::Vector2d> m_uvs;
		std::vector<Kiwi::Vector3d> m_normals;
		std::vector<Kiwi::Color> m_colors;
		std::vector<unsigned long> m_indices;

		bool m_isTextured;
		bool m_hasTransparency;
		bool m_usingPerVertexColor;

		/*if the mesh was created as an InstancedMesh this will be true*/
		bool m_isInstanced;
		long m_instanceCount;
		long m_instanceCapacity;

		std::wstring m_renderGroup;
		std::wstring m_submeshShader; //if not empty, all created submeshes will use this shader by default

	protected:

		void _OnAttached();
		bool _RebuildBuffers( std::vector<Vertex>& bufferVertices, std::vector<unsigned long>& bufferIndices );
		unsigned int _CreateSubmesh( const Kiwi::Material& material, unsigned long startIndex, unsigned long endIndex );

	public:

		Mesh();
		Mesh( std::wstring name );
		Mesh( std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals );
		Mesh( std::wstring name, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals );

		virtual ~Mesh();

		/*empties the vertex, normal, color, uv, and index arrays, but leaves the vertex and index buffers intact*/
		virtual void Clear();

		/*empties the vertex and index buffers*/
		virtual void ClearBuffers();

		/*empties the vertex, normal, color, uv, and index arrays, as well as the vertex and index buffers*/
		virtual void ClearAll();

		virtual void Bind( Kiwi::Renderer& renderer );

		/*tests for intersection between a ray and the individual triangles in this mesh 
		the vertices of the closest intersection are returned in 'closest'
		if 'culling' is true, triangles that are facing away from the ray are not counted in the collision*/
		virtual bool IntersectRay( const Kiwi::Vector3d& rayOrigin, const Kiwi::Vector3d& rayDirection, double maxDepthFromOrigin, std::vector<Kiwi::Mesh::Triangle>& closest, bool culling = true );

		/*tests for intersection between a ray and the individual triangles in this mesh 
		the vertices of the closest intersection are returned in 'closest' and all intersected triangles are returned in 'all'*/
		virtual bool IntersectRay( const Kiwi::Vector3d& rayOrigin, const Kiwi::Vector3d& rayDirection, std::vector<Kiwi::Mesh::Triangle>& closest, std::vector<Kiwi::Mesh::Triangle>& all ) { return false; }

		bool IsInstanced()const { return m_isInstanced; }

		void SetMeshFile( std::wstring filename ) { m_assetFiles.push_back( filename ); }

		void SetRenderGroup( std::wstring renderGroup ) { m_renderGroup = renderGroup; }

		void SetPrimitiveTopology( Kiwi::PrimitiveTopology topology ) { m_primitiveTopology = topology; }

		virtual void BuildMesh();
		
		/*builds the mesh as a copy of the mesh asset*/
		void FromAsset( const Kiwi::StaticMeshAsset* staticMeshAsset );

		void AddSubmesh( Kiwi::Mesh::Submesh& submesh );
		unsigned int CreateSubmesh( const Kiwi::Material& material, unsigned long startIndex, unsigned long endIndex );

		virtual void SetVertices( const std::vector<Kiwi::Vector3d>& vertices );
		virtual void SetUVs( const std::vector<Kiwi::Vector2d>& uvs );
		virtual void SetNormals( const std::vector<Kiwi::Vector3d>& normals );
		virtual void SetIndices( const std::vector<unsigned long>& indices );
		virtual void SetColors( const std::vector<Kiwi::Color>& vertexColors );

		//sets the target shader used to render all submesh materials
		//all submeshes added in the future whos materials do not have a set shader will also use this shader by default
		void SetShader( std::wstring shaderName );

		std::vector<Kiwi::Vector3d>& GetVertices() { return m_vertices; }
		std::vector<Kiwi::Vector2d>& GetUVs() { return m_uvs; }
		std::vector<Kiwi::Vector3d>& GetNormals() { return m_normals; }
		std::vector<unsigned long>& GetIndices() { return m_indices; }
		std::vector<Kiwi::Color>& GetColors() { return m_colors; }

		Kiwi::Mesh::Submesh* GetSubmesh( unsigned int submeshIndex );

		Kiwi::PrimitiveTopology GetPrimitiveTopology()const { return m_primitiveTopology; }

		unsigned int GetSubmeshCount()const { return (unsigned int)m_submeshes.size(); }

		std::wstring GetRenderGroup()const { return m_renderGroup; }

		long GetInstanceCount()const { return m_instanceCount; }
		long GetInstanceCapacity()const { return m_instanceCapacity; }

		bool IsTextured()const { return m_isTextured; }
		bool HasTransparency()const { return m_hasTransparency; }
		bool UsingPerVertexColor()const { return m_usingPerVertexColor; }

		Kiwi::IBuffer* GetVertexBuffer();
		Kiwi::IBuffer* GetIndexBuffer();

		/*creates a mesh representing a 1x1x1 cube primitive*/
		static Kiwi::Mesh* Cube();
		static Kiwi::Mesh* Quad();
		static Kiwi::Mesh* Primitive( Kiwi::Mesh::PRIMITIVE_TYPE primitiveType, std::wstring name = L"" );

	};
};

#endif