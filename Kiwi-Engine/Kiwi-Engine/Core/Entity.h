#ifndef _KIWI_ENTITY_H_
#define _KIWI_ENTITY_H_

#include "GameObject.h"
#include "Transform.h"
#include "Events\ITransformEventListener.h"
#include "../Physics/ICollisionEventListener.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace Kiwi
{

	class Scene;
	class EntityManager;
	class IEntitySpawner;
	class Component;
	class Mesh;

	struct ComponentKey;
	struct ComponentKeyHash;

	typedef std::unordered_multimap<std::wstring, Kiwi::Entity*> EntityMap;

	class Entity:
		public Kiwi::GameObject,
		public Kiwi::ITransformEventListener
	{
	friend class Scene;
	friend class EntityManager;
	public:

		enum EntityType { ENTITY_3D, ENTITY_2D, ENTITY_LIGHT, ENTITY_SOUND };

	protected:

		EntityType m_entityType;

		Kiwi::Scene* m_scene;

		Kiwi::Entity* m_parent;

		Kiwi::IEntitySpawner* m_spawner; //stores a pointer to the spawner that created this entity, if any

		Kiwi::Transform* m_transform;
		Kiwi::Mesh* m_mesh;
		Kiwi::Rigidbody* m_rigidbody;

		/*map of child entities, sorted by name*/
		std::unordered_multimap<std::wstring, Kiwi::Entity*> m_childEntities;
		std::unordered_map<ComponentKey, std::unique_ptr<Component>, ComponentKeyHash, ComponentKeyEquals> m_components;

	protected:

		virtual void _OnActivate();
		virtual void _OnDeactivate();

		void _AttachComponent( Kiwi::Component* component );
		void _DetachComponent( Kiwi::Component* component );

		Kiwi::Component* _FindComponent( ComponentKey key );

		//removes all shutdown entities and children
		//entities are detached and freed, components are deleted
		void _RemoveAllShutdown();

		template<class ComponentClass>
		ComponentClass* _FindComponent()
		{
			//return first component matching the given type
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr )
			{
				ComponentClass* comp = dynamic_cast<ComponentClass*>(compItr->second);
				if( comp != NULL )
				{
					return comp;
				}
			}

			return 0;
		}

	public:

		Entity(std::wstring name, Kiwi::Scene& scene);
		virtual ~Entity();

		virtual void FixedUpdate();
		virtual void Update();
		virtual void Shutdown();

		virtual void OnTranslate( const Kiwi::TransformEvent& evt ) {}
		virtual void OnRotate( const Kiwi::TransformEvent& evt ) {}

		virtual Kiwi::Entity* FindChildWithName(std::wstring name);
		virtual std::vector<Kiwi::Entity*> FindChildrenWithTag(std::wstring tag);

		virtual void AttachChild( Kiwi::Entity* entity );

		/*detaches the entity (does not destroy it)*/
		virtual Kiwi::Entity* DetachChild(Kiwi::Entity* entity);

		/*detaches the entity with the matching name (does not destroy it)*/
		virtual Kiwi::Entity* DetachChildWithName(std::wstring name);

		/*checks if the entity has any children*/
		virtual bool HasChild();

		/*attaches this entity to the target parent entity*/
		virtual void SetParent(Kiwi::Entity* parent);
		virtual void SetEntityType( EntityType entityType ) { m_entityType = entityType; }
		virtual void SetSpawner( Kiwi::IEntitySpawner* spawner ) { m_spawner = spawner; }

		virtual const EntityMap& GetChildren() { return m_childEntities; }
		Kiwi::Entity* GetParent()const { return m_parent; }
		virtual Entity::EntityType GetType()const { return m_entityType; }
		virtual Kiwi::Scene* GetScene()const { return m_scene; }

		Kiwi::Component* AttachComponent( Kiwi::Component* component );

		/*replaces the current transform with the new one*/
		Kiwi::Transform* AttachComponent( Kiwi::Transform* transform );

		Kiwi::Mesh* AttachComponent( Kiwi::Mesh* mesh );

		Kiwi::Rigidbody* AttachComponent( Kiwi::Rigidbody* rigidbody );

		/*detaches the component from the entity
		upon being detached the component will be destroyed and the memory freed*/
		void DetachComponent( Kiwi::Component* component );

		/*if a component with the matching name is attached, it is detached
		upon being detached the component will be destroyed and the memory freed*/
		void DetachComponent( std::wstring componentName );

		/*if a component with the matching id is attached, it is detached
		upon being detached the component will be destroyed and the memory freed*/
		void DetachComponent( int componentID );

		/*returns the first component that matches the given type*/
		template<class ComponentClass>
		ComponentClass* FindComponent()
		{
			//return first component matching the given type
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				ComponentClass* comp = dynamic_cast<ComponentClass*>(compItr->second.get());
				if( comp != NULL )
				{
					return comp;
				}
			}

			return 0;
		}

		template<>
		Kiwi::Transform* FindComponent<Kiwi::Transform>()
		{
			return m_transform;
		}

		template<>
		Kiwi::Mesh* FindComponent<Kiwi::Mesh>()
		{
			return m_mesh;
		}

		/*returns the first component that matches the given type*/
		template<>
		Kiwi::Rigidbody* FindComponent<Kiwi::Rigidbody>()
		{
			return m_rigidbody;
		}

		/*returns the component with the given name*/
		template<class ComponentClass>
		ComponentClass* FindComponent( std::wstring name )
		{
			return dynamic_cast<ComponentClass*>(this->_FindComponent( ComponentKey( 0, name ) ));
		}

		/*returns the component with the matching ID*/
		template<class ComponentClass>
		ComponentClass* FindComponent( int componentID )
		{
			return dynamic_cast<ComponentClass*>(this->_FindComponent( ComponentKey( componentID ) ));
		}

	};
};

#endif