#include "Mesh.h"
#include "StaticMeshAsset.h"

#include "../Core/Entity.h"
#include "../Core/Scene.h"
#include "../Core/Utilities.h"
#include "../Core/Exception.h"

namespace Kiwi
{

	Mesh::Mesh():
		IAsset(L"", L"StaticMesh")
	{

		m_renderer = 0;
		m_indexBuffer = 0;
		m_vertexBuffer = 0;
		m_hasTransparency = false;
		m_isTextured = false;
		m_usingPerVertexColor = false;
		m_isInstanced = false;
		m_instanceCount = 0;
		m_instanceCapacity = 0;
		m_renderGroup = L"";
		m_primitiveTopology = Kiwi::PrimitiveTopology::TRIANGLE_LIST;

	}

	Mesh::Mesh(std::wstring name) :
		IAsset( name, L"StaticMesh" ),
		Kiwi::Component( name )
	{

		m_renderer = 0;
		m_indexBuffer = 0;
		m_vertexBuffer = 0;
		m_hasTransparency = false;
		m_isTextured = false;
		m_usingPerVertexColor = false;
		m_isInstanced = false;
		m_instanceCount = 0;
		m_instanceCapacity = 0;
		m_renderGroup = L"";
		m_primitiveTopology = Kiwi::PrimitiveTopology::TRIANGLE_LIST;

	}

	Mesh::Mesh( std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals ) :
		IAsset( L"", L"StaticMesh" )
	{

		if( vertices.size() != uvs.size() || vertices.size() != normals.size() )
		{
			throw Kiwi::Exception( L"Mesh", L"[" + Kiwi::ToWString( m_objectID ) + L", " + m_objectName + L"] Vertex, UV, and Normal arrays are not the same size" );
		}

		m_renderer = 0;
		m_indexBuffer = 0;
		m_vertexBuffer = 0;
		m_hasTransparency = false;
		m_isTextured = false;
		m_usingPerVertexColor = false;
		m_isInstanced = false;
		m_instanceCount = 0;
		m_instanceCapacity = 0;
		m_renderGroup = L"";
		m_primitiveTopology = Kiwi::PrimitiveTopology::TRIANGLE_LIST;

		m_vertices.reserve( vertices.size() );
		m_vertices = vertices;
		m_uvs.reserve( uvs.size() );
		m_uvs = uvs;
		m_normals.reserve( normals.size() );
		m_normals = normals;

	}

	Mesh::Mesh( std::wstring name, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals ) :
		IAsset( name, L"StaticMesh" ),
		Kiwi::Component( name )
	{

		if( vertices.size() != uvs.size() || vertices.size() != normals.size() )
		{
			throw Kiwi::Exception( L"Mesh", L"[" + Kiwi::ToWString( m_objectID ) + L", " + m_objectName + L"] Vertex, UV, and Normal arrays are not the same size" );
		}

		m_renderer = 0;
		m_indexBuffer = 0;
		m_vertexBuffer = 0;
		m_hasTransparency = false;
		m_isTextured = false;
		m_usingPerVertexColor = false;
		m_isInstanced = false;
		m_instanceCount = 0;
		m_instanceCapacity = 0;
		m_renderGroup = L"";
		m_primitiveTopology = Kiwi::PrimitiveTopology::TRIANGLE_LIST;

		m_vertices.reserve( vertices.size() );
		m_vertices = vertices;
		m_uvs.reserve( uvs.size() );
		m_uvs = uvs;
		m_normals.reserve( normals.size() );
		m_normals = normals;

	}

	Mesh::~Mesh()
	{

		Kiwi::FreeMemory( m_assetFiles );
		Kiwi::FreeMemory( m_submeshes );
		Kiwi::FreeMemory( m_vertices );
		Kiwi::FreeMemory( m_normals );
		Kiwi::FreeMemory( m_uvs );
		Kiwi::FreeMemory( m_colors );

		SAFE_DELETE( m_indexBuffer );
		SAFE_DELETE( m_vertexBuffer );

	}

	void Mesh::_OnAttached()
	{

		if( m_entity )
		{
			Kiwi::Scene* scene = m_entity->GetScene();
			if( scene )
			{
				Kiwi::Renderer* renderer = scene->GetRenderer();
				if( renderer )
				{
					if( renderer != m_renderer )
					{//renderer has changed, re-create the index and vertex buffers
						m_renderer = renderer;

						//rebuild the mesh
						this->BuildMesh();
					}

				} else
				{
					SAFE_DELETE( m_indexBuffer );
					SAFE_DELETE( m_vertexBuffer );
				}
			}
		}

	}

	bool Mesh::_RebuildBuffers( std::vector<Vertex>& bufferVertices, std::vector<unsigned long>& bufferIndices )
	{

		if( m_renderer )
		{
			if( m_vertexBuffer != 0 || m_indexBuffer != 0 )
			{
				this->ClearBuffers();
			}

			m_indexBuffer = new Kiwi::IndexBuffer( *m_renderer, m_indices.size() + 1 );
			m_indexBuffer->SetData( bufferIndices );

			m_vertexBuffer = new Kiwi::VertexBuffer<Kiwi::Mesh::Vertex>( *m_renderer, bufferVertices.size(), bufferVertices, D3D11_USAGE_IMMUTABLE );
			if( m_indexBuffer && m_vertexBuffer )
			{
				return true;
			}
		}
			
		return false;

	}

	unsigned int Mesh::_CreateSubmesh( const Kiwi::Material& material, unsigned long startIndex, unsigned long endIndex )
	{

		Submesh defaultMesh;
		defaultMesh.material = material;
		defaultMesh.material.SetMesh( this );
		if( material.GetShader().size() == 0 )
		{
			defaultMesh.material.SetShader( m_submeshShader );
		}
		defaultMesh.startIndex = startIndex;
		defaultMesh.endIndex = endIndex;
		defaultMesh.parent = this;

		if( material.IsTextured() )
		{
			m_isTextured = true;
			m_hasTransparency = material.HasTransparency();

		} else if( material.GetColor( L"Diffuse" ).alpha != 1.0 )
		{
			m_hasTransparency = true;
		}

		m_submeshes.push_back( defaultMesh );

		return m_submeshes.size() - 1; //returns the index of the created submesh

	}

	void Mesh::Clear()
	{

		Kiwi::FreeMemory( m_vertices );
		Kiwi::FreeMemory( m_indices );
		Kiwi::FreeMemory( m_normals );
		Kiwi::FreeMemory( m_uvs );
		Kiwi::FreeMemory( m_colors );

	}

	void Mesh::ClearBuffers()
	{

		SAFE_DELETE( m_indexBuffer );
		SAFE_DELETE( m_vertexBuffer );

	}

	void Mesh::ClearAll()
	{

		Kiwi::FreeMemory( m_vertices );
		Kiwi::FreeMemory( m_indices );
		Kiwi::FreeMemory( m_normals );
		Kiwi::FreeMemory( m_uvs );
		Kiwi::FreeMemory( m_colors );
		Kiwi::FreeMemory( m_submeshes );

		SAFE_DELETE( m_indexBuffer );
		SAFE_DELETE( m_vertexBuffer );

		m_isTextured = false;
		m_hasTransparency = false;

	}

	void Mesh::Bind( Kiwi::Renderer& renderer )
	{

		if( m_indexBuffer && m_vertexBuffer )
		{
			// set the vertex buffer to active so it can be rendered
			renderer.SetVertexBuffer( 0, m_vertexBuffer, sizeof( Vertex ), 0 );

			// same for index buffer
			renderer.SetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

		} else
		{
			throw Kiwi::Exception( L"Mesh::Bind", L"[" + Kiwi::ToWString( m_objectID ) + L", " + m_objectName + L"] There are no buffers to bind" );
		}

	}

	/*tests for intersection between a ray and the individual triangles in this mesh
	the vertices of the closest intersection are returned in 'closest'*/
	bool Mesh::IntersectRay( const Kiwi::Vector3d& rayOrigin, const Kiwi::Vector3d& rayDirection, double maxDepth, std::vector<Kiwi::Mesh::Triangle>& closest, bool culling )
	{
		
		/*
		uses Möller–Trumbore intersection algorithm
		https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
		http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
		*/

		Kiwi::Transform* entTransform = m_entity->FindComponent<Kiwi::Transform>();
		if( entTransform == 0 )
		{
			return false;
		}

		Kiwi::Vector3d globalPos = entTransform->GetPosition();
		Kiwi::Vector3d scale = entTransform->GetScale();
		Kiwi::Vector3d maxPos = rayOrigin + (rayDirection * maxDepth);

		if( m_entity != 0 && m_primitiveTopology == Kiwi::TRIANGLE_LIST )
		{
			for( unsigned int i = 0; i < m_vertices.size(); i += 3 )
			{

				if( (m_vertices[i].z + globalPos.z > maxPos.z && m_vertices[i + 1].z + globalPos.z > maxPos.z && m_vertices[i + 2].z + globalPos.z > maxPos.z) ||
					(m_vertices[i].z + globalPos.z < rayOrigin.z && m_vertices[i + 1].z + globalPos.z < rayOrigin.z && m_vertices[i + 2].z + globalPos.z < rayOrigin.z) )
				{
					continue;
				}

				Kiwi::Vector3d edge1, edge2;
				edge1 = m_vertices[i + 1] - m_vertices[i]; //v1 - v0
				edge2 = m_vertices[i + 2] - m_vertices[i]; //v2 - v0
				edge1 = edge1 * scale.x;
				edge2 = edge2 * scale.y;

				Kiwi::Vector3d phit = rayDirection.Cross( edge2 ); //point where ray intersects plane of triangle

				double determinant = edge1.Dot( phit );

				double t = 0.0;

				if( culling )
				{
					// if the determinant is negative the triangle is backfacing
					// if the determinant is close to 0, the ray misses the triangle
					if( determinant < 0.000001 ) continue;

					Kiwi::Vector3d tVec = rayOrigin - ((m_vertices[i] * scale.x) + globalPos); //origin - v0

					double u = tVec.Dot( phit );
					if( u < 0.0 || u > determinant ) continue;

					Kiwi::Vector3d qVec = tVec.Cross( edge1 );
					double v = rayDirection.Dot( qVec );
					if( v < 0.0 || u + v > determinant ) continue;

				} else
				{
					// ray and triangle are parallel if det is close to 0
					if( std::abs( determinant ) < 0.000001 ) continue;

					double invDet = 1.0 / determinant;

					Kiwi::Vector3d tVec = rayOrigin - ((m_vertices[i] * scale.x) + globalPos); //origin - v0

					double u = tVec.Dot( phit ) * invDet;
					if( u < 0.0 || u > 1.0 ) continue;

					Kiwi::Vector3d qVec = tVec.Cross( edge1 );
					double v = rayDirection.Dot( qVec ) * invDet;
					if( v < 0.0 || u + v > 1.0 ) continue;
				}

				

				//if( t > 0.000001 )
				//{
					Triangle tri;
					tri.v1 = m_vertices[i];
					tri.v2 = m_vertices[i + 1];
					tri.v3 = m_vertices[i + 2];

					if( m_normals.size() > 0 )
					{
						tri.n1 = m_normals[i];
						tri.n2 = m_normals[i + 1];
						tri.n3 = m_normals[i + 2];
					}
					if( m_indices.size() > 0 )
					{
						tri.i1 = m_indices[i];
						tri.i2 = m_indices[i + 1];
						tri.i3 = m_indices[i + 2];
					}
					if( m_colors.size() > 0 )
					{
						tri.c1 = m_colors[i];
						tri.c2 = m_colors[i + 1];
						tri.c3 = m_colors[i + 2];
					}

					closest.push_back( tri );

					return (closest.size() != 0);
				//}
			}
		}

		return (closest.size() != 0);
	}

	void Mesh::SetVertices( const std::vector<Kiwi::Vector3d>& vertices )
	{

		m_vertices = vertices;

	}
	void Mesh::SetUVs( const std::vector<Kiwi::Vector2d>& uvs )
	{

		m_uvs = uvs;

	}
	void Mesh::SetNormals( const std::vector<Kiwi::Vector3d>& normals )
	{

		m_normals = normals;

	}
	void Mesh::SetIndices( const std::vector<unsigned long>& indices )
	{

		m_indices = indices;

	}
	void Mesh::SetColors( const std::vector<Kiwi::Color>& vertexColors )
	{

		m_colors = vertexColors;

	}

	void Mesh::SetShader( std::wstring shaderName )
	{

		m_submeshShader = shaderName;

		if( m_submeshes.size() > 0 )
		{
			for( auto itr = m_submeshes.begin(); itr != m_submeshes.end(); itr++ )
			{
				(*itr).material.SetShader( shaderName );
			}
		}

	}

	void Mesh::BuildMesh()
	{

		if( m_indices.size() > 0 && m_vertices.size() != m_indices.size() )
		{
			throw Kiwi::Exception( L"Mesh::BuildMesh", L"[" + Kiwi::ToWString( m_objectID ) + L", " + m_objectName + L"] Vertex and Index arrays are not the same size" );
		}

		if( m_vertices.size() > 0 )
		{
			m_usingPerVertexColor = (m_colors.size() > 0) ? true : false;

			//generate the vertices to send to the GPU
			std::vector<Vertex> bufferVertices;
			for( unsigned int i = 0; i < m_vertices.size(); i++ )
			{
				Vertex v;

				Kiwi::Vector3dToXMFLOAT3( m_vertices[i], v.position );

				if( m_normals.size() == m_vertices.size() )
				{
					Kiwi::Vector3dToXMFLOAT3( m_normals[i], v.normal );
				}
				if( m_uvs.size() == m_vertices.size() )
				{
					Kiwi::Vector2dToXMFLOAT2( m_uvs[i], v.tex );
				}
				if( m_colors.size() == m_vertices.size() )
				{
					v.color = DirectX::XMFLOAT4( (float)m_colors[i].red, (float)m_colors[i].green, (float)m_colors[i].blue, (float)m_colors[i].alpha );
				}

				bufferVertices.push_back( v );
			}

			//if the index array is empty, automatically generate the default one
			if( m_indices.size() == 0 )
			{
				for( unsigned long i = 0; i < (unsigned long)m_vertices.size(); i++ )
				{
					m_indices.push_back( i );
				}
			}

			//fill the index buffer
			std::vector<unsigned long> bufferIndices;
			for( unsigned int i = 0; i < m_indices.size(); i++ )
			{
				bufferIndices.push_back( m_indices[i] );
			}

			//if there are no submeshes set, create a new one that contains the entire mesh
			if( m_submeshes.size() == 0 || m_submeshes[0].endIndex == 0 )
			{
				Kiwi::FreeMemory( m_submeshes );
				this->_CreateSubmesh( Kiwi::Material(), 0, (unsigned long)m_vertices.size() - 1 );
			}

			this->_RebuildBuffers( bufferVertices, bufferIndices );

			Kiwi::FreeMemory( bufferVertices );
			Kiwi::FreeMemory( bufferIndices );

		} else
		{//there are no vertices, so empty the mesh buffers and submesh list
			this->ClearAll();
		}
			

	}

	void Mesh::FromAsset( const Kiwi::StaticMeshAsset* staticMeshAsset )
	{

		if( staticMeshAsset )
		{
			this->ClearAll();

			m_vertices = staticMeshAsset->GetVertices();
			m_indices = staticMeshAsset->GetIndices();
			m_uvs = staticMeshAsset->GetUVs();
			m_normals = staticMeshAsset->GetNormals();
			m_colors = staticMeshAsset->GetColors();
			m_submeshes = staticMeshAsset->GetSubmeshes();

			for( auto itr = m_submeshes.begin(); itr != m_submeshes.end(); itr++ )
			{
				if( itr->material.GetColor( L"Diffuse" ).alpha != 1.0 )
				{
					m_hasTransparency = true;
				}
				if( itr->material.IsTextured() )
				{
					m_isTextured = true;
				}
				if( itr->material.GetShader().size() == 0 )
				{
					itr->material.SetShader( m_submeshShader );
				}
			}
		}

	}

	void Mesh::AddSubmesh( Kiwi::Mesh::Submesh& submesh )
	{

		if( submesh.material.IsTextured() )
		{
			m_isTextured = true;
			m_hasTransparency = submesh.material.HasTransparency();

		} else if( submesh.material.GetColor( L"Diffuse" ).alpha != 1.0 )
		{
			m_hasTransparency = true;
		}
		if( submesh.material.GetShader().size() == 0 )
		{
			submesh.material.SetShader( m_submeshShader );
		}
		submesh.material.SetMesh( this );
		submesh.parent = this;
		m_submeshes.push_back( submesh );

	}

	unsigned int Mesh::CreateSubmesh( const Kiwi::Material& material, unsigned long startIndex, unsigned long endIndex )
	{

		return this->_CreateSubmesh( material, startIndex, endIndex );

	}

	Kiwi::Mesh::Submesh* Mesh::GetSubmesh(unsigned int submeshIndex)
	{

		if( submeshIndex >= m_submeshes.size() )
		{
			return 0;
		}

		return &m_submeshes[submeshIndex];

	}

	Kiwi::IBuffer* Mesh::GetVertexBuffer()
	{

		return m_vertexBuffer;

	}

	Kiwi::IBuffer* Mesh::GetIndexBuffer()
	{

		return m_indexBuffer;

	}


	//----Static member functions

	Kiwi::Mesh* Mesh::Cube()
	{

		double xPos = 0.5;
		double yPos = 0.5;
		double zPos = 0.5;

		std::vector<Kiwi::Vector3d> meshVertices = {
			//back face
			Kiwi::Vector3d( -xPos, yPos, -zPos ),
			Kiwi::Vector3d( xPos, yPos, -zPos ),
			Kiwi::Vector3d( -xPos, -yPos, -zPos ),
			Kiwi::Vector3d( -xPos, -yPos, -zPos ),
			Kiwi::Vector3d( xPos, yPos, -zPos ),
			Kiwi::Vector3d( xPos, -yPos, -zPos ),
			//right face
			Kiwi::Vector3d( xPos, yPos, -zPos ),
			Kiwi::Vector3d( xPos, yPos, zPos ),
			Kiwi::Vector3d( xPos, -yPos, -zPos ),
			Kiwi::Vector3d( xPos, -yPos, -zPos ),
			Kiwi::Vector3d( xPos, yPos, zPos ),
			Kiwi::Vector3d( xPos, -yPos, zPos ),
			//front face
			Kiwi::Vector3d( xPos, yPos, zPos ),
			Kiwi::Vector3d( -xPos, yPos, zPos ),
			Kiwi::Vector3d( xPos, -yPos, zPos ),
			Kiwi::Vector3d( xPos, -yPos, zPos ),
			Kiwi::Vector3d( -xPos, yPos, zPos ),
			Kiwi::Vector3d( -xPos, -yPos, zPos ),
			//left face
			Kiwi::Vector3d( -xPos, yPos, zPos ),
			Kiwi::Vector3d( -xPos, yPos, -zPos ),
			Kiwi::Vector3d( -xPos, -yPos, zPos ),
			Kiwi::Vector3d( -xPos, -yPos, zPos ),
			Kiwi::Vector3d( -xPos, yPos, -zPos ),
			Kiwi::Vector3d( -xPos, -yPos, -zPos ),
			//top face
			Kiwi::Vector3d( -xPos, yPos, zPos ),
			Kiwi::Vector3d( xPos, yPos, zPos ),
			Kiwi::Vector3d( -xPos, yPos, -zPos ),
			Kiwi::Vector3d( -xPos, yPos, -zPos ),
			Kiwi::Vector3d( xPos, yPos, zPos ),
			Kiwi::Vector3d( xPos, yPos, -zPos ),
			//bottom face
			Kiwi::Vector3d( -xPos, -yPos, -zPos ),
			Kiwi::Vector3d( xPos, -yPos, -zPos ),
			Kiwi::Vector3d( -xPos, -yPos, zPos ),
			Kiwi::Vector3d( -xPos, -yPos, zPos ),
			Kiwi::Vector3d( xPos, -yPos, -zPos ),
			Kiwi::Vector3d( xPos, -yPos, zPos )
		};

		std::vector<Kiwi::Vector3d> meshNormals = {
			//back face
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			//right face
			Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
			//front face
			Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
			//left face
			Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
			Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
			//top face
			Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
			//bottom face
			Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
			Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
		};

		std::vector<Kiwi::Vector2d> meshUVs = {
			//back face
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
			//right face
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
			//front face
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
			//left face
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
			//top face
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
			//bottom face
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
		};

		Kiwi::Mesh* cube = new Kiwi::Mesh( meshVertices, meshUVs, meshNormals );

		return cube;

	}

	Kiwi::Mesh* Mesh::Quad()
	{

		float xPos = 0.5;
		float yPos = 0.5;

		std::vector<Kiwi::Vector3d> meshVertices = {
			Kiwi::Vector3d( -xPos, yPos, 0.0f ),
			Kiwi::Vector3d( xPos, yPos, 0.0f ),
			Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
			Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
			Kiwi::Vector3d( xPos, yPos, 0.0f ),
			Kiwi::Vector3d( xPos, -yPos, 0.0f )
		};

		std::vector<Kiwi::Vector3d> meshNormals = {
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
		};

		std::vector<Kiwi::Vector2d> meshUVs = {
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
		};

		Kiwi::Mesh* quad = new Kiwi::Mesh( meshVertices, meshUVs, meshNormals );

		return quad;

	}

	Kiwi::Mesh* Mesh::Primitive( Kiwi::Mesh::PRIMITIVE_TYPE primitiveType, std::wstring name )
	{

		switch( primitiveType )
		{
			case Mesh::PRIMITIVE_TYPE::QUAD:
				{
					float xPos = 0.5;
					float yPos = 0.5;

					std::vector<Kiwi::Vector3d> meshVertices = {
						Kiwi::Vector3d( -xPos, yPos, 0.0f ),
						Kiwi::Vector3d( xPos, yPos, 0.0f ),
						Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
						Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
						Kiwi::Vector3d( xPos, yPos, 0.0f ),
						Kiwi::Vector3d( xPos, -yPos, 0.0f )
					};

					std::vector<Kiwi::Vector3d> meshNormals = {
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
					};

					std::vector<Kiwi::Vector2d> meshUVs = {
						Kiwi::Vector2d( 0.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 1.0 ),
					};

					Kiwi::Mesh* quad = new Kiwi::Mesh( name, meshVertices, meshUVs, meshNormals );

					return quad;
				}
			case Mesh::PRIMITIVE_TYPE::CUBE:
				{
					double xPos = 0.5;
					double yPos = 0.5;
					double zPos = 0.5;

					std::vector<Kiwi::Vector3d> meshVertices = {
						//back face
						Kiwi::Vector3d( -xPos, yPos, -zPos ),
						Kiwi::Vector3d( xPos, yPos, -zPos ),
						Kiwi::Vector3d( -xPos, -yPos, -zPos ),
						Kiwi::Vector3d( -xPos, -yPos, -zPos ),
						Kiwi::Vector3d( xPos, yPos, -zPos ),
						Kiwi::Vector3d( xPos, -yPos, -zPos ),
						//right face
						Kiwi::Vector3d( xPos, yPos, -zPos ),
						Kiwi::Vector3d( xPos, yPos, zPos ),
						Kiwi::Vector3d( xPos, -yPos, -zPos ),
						Kiwi::Vector3d( xPos, -yPos, -zPos ),
						Kiwi::Vector3d( xPos, yPos, zPos ),
						Kiwi::Vector3d( xPos, -yPos, zPos ),
						//front face
						Kiwi::Vector3d( xPos, yPos, zPos ),
						Kiwi::Vector3d( -xPos, yPos, zPos ),
						Kiwi::Vector3d( xPos, -yPos, zPos ),
						Kiwi::Vector3d( xPos, -yPos, zPos ),
						Kiwi::Vector3d( -xPos, yPos, zPos ),
						Kiwi::Vector3d( -xPos, -yPos, zPos ),
						//left face
						Kiwi::Vector3d( -xPos, yPos, zPos ),
						Kiwi::Vector3d( -xPos, yPos, -zPos ),
						Kiwi::Vector3d( -xPos, -yPos, zPos ),
						Kiwi::Vector3d( -xPos, -yPos, zPos ),
						Kiwi::Vector3d( -xPos, yPos, -zPos ),
						Kiwi::Vector3d( -xPos, -yPos, -zPos ),
						//top face
						Kiwi::Vector3d( -xPos, yPos, zPos ),
						Kiwi::Vector3d( xPos, yPos, zPos ),
						Kiwi::Vector3d( -xPos, yPos, -zPos ),
						Kiwi::Vector3d( -xPos, yPos, -zPos ),
						Kiwi::Vector3d( xPos, yPos, zPos ),
						Kiwi::Vector3d( xPos, yPos, -zPos ),
						//bottom face
						Kiwi::Vector3d( -xPos, -yPos, -zPos ),
						Kiwi::Vector3d( xPos, -yPos, -zPos ),
						Kiwi::Vector3d( -xPos, -yPos, zPos ),
						Kiwi::Vector3d( -xPos, -yPos, zPos ),
						Kiwi::Vector3d( xPos, -yPos, -zPos ),
						Kiwi::Vector3d( xPos, -yPos, zPos )
					};

					std::vector<Kiwi::Vector3d> meshNormals = {
						//back face
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
						//right face
						Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( 1.0, 0.0, 0.0 ),
						//front face
						Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
						Kiwi::Vector3d( 0.0, 0.0, 1.0 ),
						//left face
						Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
						Kiwi::Vector3d( -1.0, 0.0, 0.0 ),
						//top face
						Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, 1.0, 0.0 ),
						//bottom face
						Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
						Kiwi::Vector3d( 0.0, -1.0, 0.0 ),
					};

					std::vector<Kiwi::Vector2d> meshUVs = {
						//back face
						Kiwi::Vector2d( 0.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 1.0 ),
						//right face
						Kiwi::Vector2d( 0.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 1.0 ),
						//front face
						Kiwi::Vector2d( 0.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 1.0 ),
						//left face
						Kiwi::Vector2d( 0.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 1.0 ),
						//top face
						Kiwi::Vector2d( 0.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 1.0 ),
						//bottom face
						Kiwi::Vector2d( 0.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 0.0, 1.0 ),
						Kiwi::Vector2d( 1.0, 0.0 ),
						Kiwi::Vector2d( 1.0, 1.0 ),
					};

					Kiwi::Mesh* cube = new Kiwi::Mesh( name, meshVertices, meshUVs, meshNormals );

					return cube;
				}
			default: return 0;
		}
		

	}

};