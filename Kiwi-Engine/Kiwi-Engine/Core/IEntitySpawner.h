#ifndef _IENTITYSPAWNER_H_
#define _IENTITYSPAWNER_H_

#include "Vector3d.h"
#include "Entity.h"

#include <string>
#include <unordered_map>

namespace Kiwi
{

	class Scene;

	class IEntitySpawner:
		public Kiwi::Entity
	{
	protected:

		std::unordered_map<std::wstring, Kiwi::Entity*> m_spawnedEntities;

		Kiwi::Vector3d m_spawnPosition;

		double m_spawnFrequency; //time in milliseconds between spawns (a value of 0 means entities will be spawned all at once, then never again)
		double m_spawnRadius;

		unsigned int m_currentSpawnCount; //current number of entities spawned
		unsigned int m_maxConcurrentEntities; //total number of entities that can be spawned at any one time
		int m_maxSpawnCount; //total number of entities that can be spawned by this spawner -1 = infinite
		unsigned int m_totalSpawnCount; //total number of entities that have been spawned

	protected:

		virtual void OnUpdate() {}
		virtual void OnFixedUpdate() {}
		virtual void OnShutdown() {}

		/*creates a new entity to be spawned*/
		virtual Kiwi::Entity* CreateEntity() = 0;

	public:

		IEntitySpawner( std::wstring name, Kiwi::Scene& scene );

		virtual ~IEntitySpawner();

		virtual void Update();
		virtual void Shutdown();

		virtual void RemoveEntity( Kiwi::Entity* entity );

		void SetSpawnFrequency( double milliseconds ) { m_spawnFrequency = milliseconds; }
		void SetSpawnRadius( double radius ) { m_spawnRadius = radius; }
		void SetSpawnPosition( const Kiwi::Vector3d& position ) { m_spawnPosition = position; }
		void SetMaxConcurrentEntities( unsigned int numEntities ) { m_maxConcurrentEntities = numEntities; }

		const Kiwi::Vector3d& GetSpawnPosition()const { return m_spawnPosition; }


	};
}

#endif