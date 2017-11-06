#include "RenderQueue.h"

#include "../Core/Entity.h"
#include "../Core/Scene.h"
#include "../Core/EngineRoot.h"
#include "../Core/Console.h"
#include "../Core/EntityManager.h"

namespace Kiwi
{

	RenderQueue::RenderQueue( Kiwi::Scene& scene ) :
		m_parentScene( &scene )
	{

		m_defaultRenderGroupName = L"default";
		this->CreateRenderGroup( m_defaultRenderGroupName );

	}

	RenderQueue::~RenderQueue()
	{

		auto itr = m_renderGroups.begin();
		for( ; itr != m_renderGroups.end(); )
		{
			SAFE_DELETE( itr->second );
			itr = m_renderGroups.erase( itr );
		}

	}

	//clears the render queue
	void RenderQueue::Clear()
	{

		auto itr = m_renderGroups.begin();
		for( ; itr != m_renderGroups.end(); itr++ )
		{
			itr->second->Clear();
		}

	}

	void RenderQueue::Generate( Kiwi::EntityMap* entityMap )
	{

		if( entityMap )
		{
			for( auto entityItr = entityMap->begin(); entityItr != entityMap->end(); entityItr++ )
			{
				Kiwi::Entity* entity = entityItr->second;
				if( entity && entity->IsActive() && !entity->IsShutdown() )
				{
					//if there's a mesh attached to the entity, retrieve it
					Kiwi::Mesh* entityMesh = entity->FindComponent<Kiwi::Mesh>();
					if( entityMesh && entityMesh->IsActive() == true && entityMesh->IsShutdown() == false )
					{
						std::wstring rGroup = entityMesh->GetRenderGroup(); //get the render group the mesh belongs to
						if( rGroup.compare( L"" ) == 0 )
						{
							Kiwi::RenderQueueGroup* defaultGroup = this->GetDefaultRenderGroup();
							if( defaultGroup )
							{
								defaultGroup->AddMesh( entityMesh );
							}

						} else
						{
							Kiwi::RenderQueueGroup* group = this->GetRenderGroup( rGroup );
							if( group )
							{
								group->AddMesh( entityMesh );

							} else
							{//render group doesnt exist yet, need to create it first
								RenderQueueGroup* newGroup = this->CreateRenderGroup( rGroup );
								if( newGroup )
								{
									newGroup->AddMesh( entityMesh );
								}
								//m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Mesh '" + entity->GetName() + L"' belongs to non-existant render group '" + entityMesh->GetRenderGroup() + L"'" );
							}
						}
					}
				}
			}
		}

	}

	//void RenderQueue::AddRenderable( Kiwi::Renderable* renderable, std::wstring renderGroup )
	//{

	//	if( renderable && renderable->GetMesh() && renderable->GetShader() != L"" && renderable->GetParentEntity() )
	//	{
	//		std::wstring targetGroup = renderGroup;
	//		if( targetGroup.compare( L"" ) == 0 )
	//		{
	//			targetGroup = renderable->GetRenderGroup();
	//		}

	//		if( targetGroup.compare( L"" ) != 0 )
	//		{
	//			//renderable belongs to a custom render group
	//			auto itr = m_renderGroups.find( targetGroup );
	//			if( itr != m_renderGroups.end() )
	//			{
	//				itr->second->AddRenderable( renderable );

	//			} else
	//			{
	//				m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Renderable '" + renderable->GetName() + L"' belongs to non-existant render group '" + renderable->GetRenderGroup() + L"'" );
	//			}

	//		} else
	//		{
	//			//no render group specified, use default
	//			auto itr = m_renderGroups.find( m_defaultGroup );
	//			if( itr != m_renderGroups.end() )
	//			{
	//				itr->second->AddRenderable( renderable );

	//			} else
	//			{
	//				m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Default render group '" + m_defaultGroup + L"' does not exist" );
	//			}
	//		}

	//	} else
	//	{
	//		//invalid renderable
	//		if( renderable )
	//		{
	//			m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Invalid renderable '" + renderable->GetName() + L"' was added to the render queue" );

	//		} else
	//		{
	//			m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Null renderable was added to the render queue" );
	//		}
	//	}

	//}

	void RenderQueue::QueueMesh( Kiwi::Mesh* mesh )
	{

		if( mesh && mesh->GetEntity() )
		{
			std::wstring targetGroup = mesh->GetRenderGroup();

			//renderable belongs to a custom render group
			auto itr = m_renderGroups.find( targetGroup );
			if( itr != m_renderGroups.end() )
			{
				itr->second->AddMesh( mesh );

			} else
			{
				m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Mesh '" + mesh->GetName() + L"' belongs to non-existant render group '" + mesh->GetRenderGroup() + L"'" );
			}

		} else
		{
			//invalid renderable
			if( mesh )
			{
				m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Invalid mesh '" + mesh->GetName() + L"' (no parent entity) was added to the render queue" );

			} else
			{
				m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Null mesh was added to the render queue" );
			}
		}

	}

	Kiwi::RenderQueueGroup* RenderQueue::CreateRenderGroup( std::wstring groupName )
	{

		if( this->GetRenderGroup( groupName ) )
		{
			m_parentScene->GetEngine()->GetConsole()->PrintDebug( L"Tried to create duplicate render group '" + groupName + L"'" );
			return 0;
		}

		Kiwi::RenderQueueGroup* group = new Kiwi::RenderQueueGroup( groupName );
		m_renderGroups[groupName] = group;

		return group;

	}

	RenderQueueGroup* RenderQueue::GetRenderGroup( std::wstring groupName )
	{

		auto itr = m_renderGroups.find( groupName );
		if( itr != m_renderGroups.end() )
		{
			return itr->second;

		} else
		{
			return 0;
		}

	}

	RenderQueueGroup* RenderQueue::GetDefaultRenderGroup()
	{

		auto itr = m_renderGroups.find( m_defaultRenderGroupName );
		if( itr != m_renderGroups.end() )
		{
			return itr->second;

		} else
		{
			return 0;
		}

	}

}