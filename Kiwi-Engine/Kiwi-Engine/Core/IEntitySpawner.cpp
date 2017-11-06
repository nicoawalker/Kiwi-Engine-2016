#include "IEntitySpawner.h"
#include "Scene.h"
#include "Entity.h"
#include "EngineRoot.h"

namespace Kiwi
{

	IEntitySpawner::IEntitySpawner( std::wstring name, Kiwi::Scene& scene ) :
		Kiwi::Entity( name, scene )
	{

		m_scene = &scene;
		m_spawnFrequency = 0.0;
		m_spawnRadius = 0.0;
		m_currentSpawnCount = 0;
		m_totalSpawnCount = 0;
		m_maxConcurrentEntities = 0;
		m_maxSpawnCount = 0;

	}

	IEntitySpawner::~IEntitySpawner()
	{
		//MessageBox( NULL, L"B", L"B", MB_OK );
		//empty the entity map
		FreeMemory( m_spawnedEntities );

	}

	void IEntitySpawner::Shutdown()
	{

		//decouple the spawned entities from the spawner, without destroying them
		auto itr = m_spawnedEntities.begin();
		for( ; itr != m_spawnedEntities.end();)
		{
			if( itr->second != 0 )
			{
				itr->second->SetSpawner( 0 );
				itr = m_spawnedEntities.erase( itr );
			}
		}

		Entity::Shutdown();

	}

	void IEntitySpawner::Update()
	{

		static double timer = 0.0;

		if( m_totalSpawnCount < (unsigned int)m_maxSpawnCount && m_currentSpawnCount < m_maxConcurrentEntities )
		{
			//check whether it's time to spawn a new entity
			Kiwi::EngineRoot* engine = m_scene->GetEngine();
			if( engine )
			{
				double deltaTime = engine->GetGameTimer()->GetDeltaTime();
				timer += deltaTime;

				if( timer > m_spawnFrequency || m_spawnFrequency == 0.0 )
				{
					timer -= m_spawnFrequency;
					Kiwi::Entity* newEntity = this->CreateEntity();

					if( newEntity )
					{
						if( m_spawnedEntities.find( newEntity->GetName() ) != m_spawnedEntities.end() )
						{
							//already an entity with the same name, do nothing
							newEntity->Shutdown();
							SAFE_DELETE( newEntity );
							return;
						}

						m_totalSpawnCount += 1;
						m_currentSpawnCount += 1;

						newEntity->SetSpawner( this );
						m_spawnedEntities[newEntity->GetName()] = newEntity;
						m_scene->AddEntity( newEntity );
					}
				}
			}
		}

		//update the entity as well
		Entity::Update();

	}

	void IEntitySpawner::RemoveEntity( Kiwi::Entity* entity )
	{

		if( entity )
		{
			auto itr = m_spawnedEntities.find( entity->GetName() );
			if( itr != m_spawnedEntities.end() )
			{
				m_spawnedEntities.erase( itr );
				m_currentSpawnCount -= 1;
			}
		}

	}

}