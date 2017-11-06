#ifndef _KIWI_ENTITYMANAGER_H_
#define _KIWI_ENTITYMANAGER_H_

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Kiwi
{

	class Renderer;
	class Scene;
	class RenderQueue;

	class EntityManager
	{
	private:

		struct LightMap
		{
			//stores the name of a render target, and a vector of light entities bound to the render target
			std::unordered_multimap<std::wstring, std::vector<Kiwi::Entity*>> map;
		};

	protected:

		Kiwi::Scene* m_scene;

		/*stores the active entities (these are updated every frame)*/
		EntityMap m_entities;

		//stores all of the lights active in the scene
		LightMap m_lights;

		Kiwi::RenderQueue* m_renderQueue;

	public:

		EntityManager( Kiwi::Scene& scene );
		~EntityManager();

		void Update();
		void FixedUpdate();

		/*generates a new render queue based on the entities currently managed by the entity manager*/
		Kiwi::RenderQueue* GenerateRenderQueue();

		/*clears the render queue and all it's render groups so that it contains no entities*/
		void ClearRenderQueue();

		/*attempts to add the entity to the entity pool
		if destroyExisting is true, and an entity with the same name already exists, that entity
		will be destroyed and the new entity will be added
		otherwise, if there is an existing entity with the same name, the new entity is not added*/
		void AddEntity(Kiwi::Entity* entity, bool destroyExisting = false);

		Kiwi::Entity* CreateEntity( std::wstring name );

		void Raytrace( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits );

		void ShutdownWithName(std::wstring name);
		void ShutdownAll();
		void ShutdownInactive();

		/*immediately destroys all entities that have been shutdown*/
		void DestroyAllShutdown();

		Kiwi::Entity* FindWithName(std::wstring name);
		std::vector<Kiwi::Entity*> FindAllWithTag(std::wstring tag);

		const EntityMap* GetEntityMap()const { return &m_entities; }

	};
};

#endif