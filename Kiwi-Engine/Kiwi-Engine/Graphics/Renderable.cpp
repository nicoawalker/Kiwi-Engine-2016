#include "Renderable.h"
#include "IShaderEffect.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"
#include "../Core/Entity.h"
#include "../Core/Scene.h"

namespace Kiwi
{

	Renderable::Renderable( Kiwi::Entity* parentEntity )
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"Renderable", L"Failed to initialize renderable: No parent entity specified" );
		}
		if( !parentEntity->GetScene() )
		{
			throw Kiwi::Exception( L"Renderable", L"Failed to initialize renderable: parent entity doesn't belong to any scene" );
		}

		m_parentEntity = parentEntity;

		m_renderableName = m_parentEntity->GetName() + L"/Renderable";

		m_mesh = new Kiwi::Mesh();
		m_shaderEffect = 0;
		m_renderType = RENDER_3D;
		m_renderTarget = L"BackBuffer";
		m_currentMeshSubset = 0;
		m_shader = L"";
		m_active = true;
		m_shutdown = false;

	}

	Renderable::Renderable( Kiwi::Entity* parentEntity, Kiwi::Mesh* mesh )
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"Renderable", L"Failed to initialize renderable: No parent entity specified" );
		}

		m_parentEntity = parentEntity;

		m_renderableName = m_parentEntity->GetName() + L"/Renderable";

		if( mesh != 0 )
		{
			m_mesh = mesh;
		} else
		{

		}

		m_shaderEffect = 0;
		m_renderType = RENDER_3D;
		m_renderTarget = L"BackBuffer";
		m_currentMeshSubset = 0;
		m_shader = L"";
		m_active = true;
		m_shutdown = false;

	}

	Renderable::Renderable( std::wstring name, Kiwi::Entity* parentEntity, Kiwi::Mesh* mesh, std::wstring shader )
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"Renderable", L"[" + name + L"] Renderable initialized with no parent entity" );
		}

		m_parentEntity = parentEntity;
		m_renderableName = name;
		m_shaderEffect = 0;
		m_renderType = RENDER_3D;
		m_mesh = mesh;
		m_renderTarget = L"BackBuffer";
		m_currentMeshSubset = 0;
		m_shader = shader;
		m_active = true;
		m_shutdown = false;

	}

	Renderable::Renderable(std::wstring name, Kiwi::Entity* parentEntity, Kiwi::Mesh* mesh, Kiwi::IShaderEffect* effect )
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"Renderable", L"["+name+L"] Renderable initialized with no parent entity" );
		}

		m_parentEntity = parentEntity;
		m_renderableName = name;
		m_shaderEffect = effect;
		m_mesh = mesh;
		if( m_shaderEffect ) m_shader = m_shaderEffect->GetShaderName();
		m_renderType = RENDER_3D;
		m_renderTarget = L"BackBuffer";
		m_currentMeshSubset = 0;
		m_active = true;
		m_shutdown = false;

	}

	Renderable::~Renderable()
	{

		SAFE_DELETE( m_mesh );
		SAFE_DELETE( m_shaderEffect );

	}

	void Renderable::Shutdown()
	{

		if( m_shutdown )
		{
			return;
		}

		if( m_parentEntity )
		{
			//m_parentEntity->SetRenderable( 0 );
		}

		m_shutdown = true;
		m_active = false;

		this->OnShutdown();

	}

	/*void Renderable::SetEffect( Kiwi::IShaderEffect* effect )
	{

		if( m_shaderEffect )
		{
			SAFE_DELETE( m_shaderEffect );
		}

		m_shaderEffect = effect;

	}*/

	void Renderable::SetMesh( Kiwi::Mesh* mesh )
	{
		
		m_mesh = mesh;

	}

	//sets one of the mesh's subsets as the currently active/rendering subset
	void Renderable::SetCurrentMeshSubset( unsigned int subsetIndex )
	{

		if( m_mesh )
		{
			if( subsetIndex < m_mesh->GetSubmeshCount() )
			{
				m_currentMeshSubset = subsetIndex;
			}
		}

	}

	Kiwi::Mesh::Submesh* Renderable::GetCurrentSubmesh()
	{

		return (m_mesh) ? &m_mesh->GetSubmesh( m_currentMeshSubset ) : 0;

	}

	/*std::wstring Renderable::GetShader()const
	{

		return (m_shaderEffect != 0) ? m_shaderEffect->GetShaderName() : L"";

	}*/

};