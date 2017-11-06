#include "EntityManager.h"
#include "Entity.h"
#include "Exception.h"
#include "Utilities.h"

#include "../Graphics/RenderQueue.h"
#include "..\Graphics\Mesh.h"

namespace Kiwi
{

	EntityManager::EntityManager( Kiwi::Scene& scene )
	{

		m_scene = &scene;
		m_renderQueue = new Kiwi::RenderQueue( *m_scene );
		assert( m_renderQueue );

	}

	EntityManager::~EntityManager()
	{

		for( auto it = m_entities.begin(); it != m_entities.end();)
		{
			SAFE_DELETE(it->second);
			it = m_entities.erase(it);
		}

		SAFE_DELETE( m_renderQueue );

	}

	void EntityManager::Update()
	{

		for( auto itr = m_entities.begin(); itr != m_entities.end(); itr++ )
		{
			//only want to update active non-children (parents are responsible for updating children)
			if( itr->second != 0 && itr->second->IsActive() && itr->second->GetParent() == 0 )
			{
				itr->second->Update();
			}
		}

	}

	void EntityManager::FixedUpdate()
	{

		for( auto itr = m_entities.begin(); itr != m_entities.end(); )
		{
			if( itr->second == 0 || (itr->second->IsShutdown() && itr->second->GetReferenceCount() == 0) )
			{
				SAFE_DELETE( itr->second );
				itr = m_entities.erase( itr );
				continue;

			} else if( itr->second != 0 && itr->second->IsActive() && itr->second->GetParent() == 0 )
			{
				itr->second->FixedUpdate();
			}
			itr++;
		}

	}

	Kiwi::RenderQueue* EntityManager::GenerateRenderQueue()
	{

		if( m_renderQueue )
		{
			m_renderQueue->Generate( &m_entities );
			return m_renderQueue;
		}

		return 0;

	}

	void EntityManager::ClearRenderQueue()
	{

		if( m_renderQueue )
		{
			m_renderQueue->Clear();
		}

	}

	void EntityManager::AddEntity(Kiwi::Entity* entity, bool destroyExisting)
	{

		if(entity == 0) return;

		/*if( this->FindWithName( entity->GetName() ) )
		{
			if( destroyExisting )
			{
				this->ShutdownWithName( entity->GetName() );
				this->DestroyAllShutdown();
				m_entities.insert( std::make_pair( entity->GetName(), entity ) );
			}

		}else
		{*/
			m_entities.insert( std::make_pair( entity->GetName(), entity ) );
		//}

		////add any children belonging to the entity
		//if( entity->HasChild() )
		//{
		//	auto childItr = entity->GetChildren().begin();
		//	for( ; childItr != entity->GetChildren().end(); childItr++ )
		//	{
		//		this->AddEntity( childItr->second, destroyExisting );
		//	}
		//}

	}

	Kiwi::Entity* EntityManager::CreateEntity( std::wstring name )
	{

		if( m_scene == 0 )
		{
			throw Kiwi::Exception( L"EntityManager::CreateEntity", L"Entity manager is not attached to a scene" );

		}
		/*else if( this->FindWithName( name ) )
		{
			throw Kiwi::Exception( L"EntityManager::CreateEntity", L"Entity with name '" + name + L"' already exists" );
		}*/

		Kiwi::Entity* newEntity = new Kiwi::Entity( name, *m_scene );

		m_entities.insert( std::make_pair( name, newEntity ) );

		return newEntity;

	}

	void EntityManager::Raytrace( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits )
	{

		auto itr = m_entities.begin();
		for( ; itr != m_entities.end(); itr++ )
		{
			if( itr->second != 0 && itr->second->IsShutdown() == false && itr->second->IsActive() && itr->second->HasTag(L"terrain") == false )
			{
				Kiwi::Mesh* mesh = itr->second->FindComponent<Kiwi::Mesh>();
				if( mesh != 0 && mesh->IsShutdown() == false && mesh->IsActive() == true )
				{
					Kiwi::Transform* transform = itr->second->FindComponent<Kiwi::Transform>();
					if( transform != 0 && transform->GetSquareDistance(origin) <= maxDepthFromOrigin * maxDepthFromOrigin )
					{
						std::vector<Kiwi::Mesh::Triangle> tIntersect;
						mesh->IntersectRay( origin, direction, maxDepthFromOrigin, tIntersect );
						if( tIntersect.size() > 0 )
						{
							hits.push_back( itr->second );
						}
					}
				}
			}
		}

		//sort the entities based on distance from the origin
		std::sort( hits.begin(), hits.end(), [origin]( Kiwi::Entity* e1, Kiwi::Entity* e2 )
		{
			Kiwi::Transform* m1T = e1->FindComponent<Kiwi::Transform>();
			Kiwi::Transform* m2T = e2->FindComponent<Kiwi::Transform>();

			if( m1T && m2T )
			{
				return m1T->GetSquareDistance( origin ) < m2T->GetSquareDistance( origin );

			} else
			{
				return true;
			}
		} );

	}

	void EntityManager::ShutdownWithName(std::wstring name)
	{

		auto it = m_entities.find(name);
		if(it != m_entities.end())
		{
			if( it->second != 0 )
			{
				it->second->Shutdown();
			}
		}

	}

	void EntityManager::ShutdownAll()
	{

		auto it = m_entities.begin();
		for(; it != m_entities.end(); it++)
		{
			if( it->second != 0 )
			{
				it->second->Shutdown();
			}
		}

	}

	void EntityManager::ShutdownInactive()
	{

		auto inactiveIt = m_entities.begin();
		for(; inactiveIt != m_entities.end();)
		{
			if( inactiveIt->second != 0 && !inactiveIt->second->IsActive() )
			{
				inactiveIt->second->Shutdown();
			}
		}

	}

	void EntityManager::DestroyAllShutdown()
	{

		auto itr = m_entities.begin();
		for( ; itr != m_entities.end(); )
		{
			if( itr->second != 0 && itr->second->IsShutdown() )
			{
				SAFE_DELETE( itr->second );
				itr = m_entities.erase( itr );
				continue;
			}
			itr++;
		}

	}

	Kiwi::Entity* EntityManager::FindWithName(std::wstring name)
	{

		auto it = m_entities.find(name);
		if(it != m_entities.end())
		{
			//found it
			return it->second;
		}

		return 0;

	}

	std::vector<Kiwi::Entity*> EntityManager::FindAllWithTag(std::wstring tag)
	{

		std::vector<Kiwi::Entity*> matches;

		for(auto entity : m_entities)
		{
			if( entity.second != 0 && entity.second->HasTag(tag))
			{
				matches.push_back(entity.second);
			}
		}

		return matches;

	}

};