#include "RenderableManager.h"
#include "Renderable.h"
#include "IShaderEffect.h"
#include "RenderTarget.h"
#include "Renderer.h"
#include "RenderQueue.h"

#include "../Core/Assert.h"
#include "../Core/Scene.h"
#include "../Core/Entity.h"
#include "../Core/EngineRoot.h"
#include "../Core/Console.h"

namespace Kiwi
{

	RenderableManager::RenderableManager( Kiwi::Scene& scene )
	{

		m_parentScene = &scene;

	}

	RenderableManager::~RenderableManager()
	{
		auto itr = m_renderables.begin();
		for( ; itr != m_renderables.end(); itr++ )
		{
			if( itr->second )
			{
				itr->second->Shutdown();
				SAFE_DELETE( itr->second );
				itr = m_renderables.erase( itr );
			}
		}
	}

	void RenderableManager::FixedUpdate()
	{

		//check for any shutdown renderables and remove them
		auto itr = m_renderables.begin();
		for( ; itr != m_renderables.end(); itr++ )
		{
			if( itr->second->IsShutdown() )
			{
				SAFE_DELETE( itr->second );
				itr = m_renderables.erase( itr );
			}
		}

	}

	void RenderableManager::AddRenderable( Kiwi::Renderable* renderable )
	{

		if( renderable == 0 || renderable->GetRenderTarget().compare( L"" ) == 0 || renderable->GetParentEntity() == 0 )
		{
			return;
		}

		if( m_renderables.find( renderable->GetName() ) != m_renderables.end() )
		{
			m_parentScene->GetEngine()->GetConsole()->Print( L"Tried to add duplicate renderable '" + renderable->GetName() + L"' to RenderableManager" );
			return;
		}

		m_renderables[renderable->GetName()] = renderable;

	}

	Kiwi::RenderQueue* RenderableManager::GenerateRenderQueue()
	{

		Kiwi::RenderQueue* renderQueue = new Kiwi::RenderQueue( *m_parentScene );

		auto itr = m_renderables.begin();
		for( ; itr != m_renderables.end(); itr++ )
		{
			if( itr->second->IsActive() )
			{
				renderQueue->AddRenderable( itr->second );
			}
		}

		return renderQueue;

	}

	void RenderableManager::AddFromEntity( Kiwi::Entity* entity )
	{

		if( entity )
		{
			//MessageBox( NULL, entity->GetName().c_str(), L"A", MB_OK );
			this->AddRenderable( entity->GetRenderable() );

			//add renderables from any children as well
			if( entity->HasChild() )
			{
				auto childItr = entity->GetChildren().begin();
				for( ; childItr != entity->GetChildren().end(); childItr++ )
				{
					this->AddFromEntity( childItr->second );
				}
			}
		}

	}

	void RenderableManager::ShutdownWithName( std::wstring name )
	{

		Kiwi::Renderable* rend = this->FindWithName( name );
		if( rend )
		{
			rend->Shutdown();
		}

	}

	void RenderableManager::ShutdownAll()
	{

		auto itr = m_renderables.begin();
		for( ; itr != m_renderables.end(); itr++ )
		{
			if( itr->second )
			{
				itr->second->Shutdown();
			}
		}

	}

	Kiwi::Renderable* RenderableManager::FindWithName( std::wstring name )
	{

		auto itr = m_renderables.find( name );
		if( itr != m_renderables.end() )
		{
			return itr->second;
		}

		return 0;

	}

}