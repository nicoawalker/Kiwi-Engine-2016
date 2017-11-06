#ifndef _KIWI_COMPONENT_H_
#define _KIWI_COMPONENT_H_

#include "GameObject.h"

#include <unordered_map>

namespace Kiwi
{
	
	class Entity;

	struct ComponentKey
	{
		std::wstring name;
		int id;

		ComponentKey() :
			id( 0 )
		{
		}
		ComponentKey( int id, std::wstring name = L"" )
		{
			this->id = id;
			this->name = name;
		}

		bool operator== ( const ComponentKey& comp )const
		{
			//if both have names compare by name, otherwise compare by id
			return (name.compare( L"" ) != 0 && comp.name.compare( L"" ) != 0) ? ( name.compare( comp.name ) == 0 && id == comp.id ) : id == comp.id;
		}
	};

	struct ComponentKeyHash
	{
		std::size_t operator() ( const ComponentKey& key )const
		{
			//hash by name if there is one, otherwise use the id
			return (key.id == 0) ? std::hash<std::wstring>()(key.name) : std::hash<int>()(key.id);
		}
	};

	struct ComponentKeyEquals
	{
		bool operator() ( const ComponentKey& c1, const ComponentKey& c2 )const
		{
			//if both have names compare by name, otherwise compare by id
			return (c1.name.compare( L"" ) != 0 && c2.name.compare( L"" ) != 0) ? c1.name.compare( c2.name ) == 0 : c1.id == c2.id;
		}
	};

	class Component:
		public GameObject
	{
	friend class Kiwi::Entity;
	protected:

		Kiwi::Entity* m_entity;

		std::unordered_map<ComponentKey, Component*, ComponentKeyHash, ComponentKeyEquals> m_childComponents;

	protected:

		/*called immediately after this component has been attached to an entity*/
		virtual void _OnAttached() {}

		/*called immediately before the component is detached from an entity*/
		virtual void _OnDetached() {}

		void _AttachComponent( Kiwi::Component* component );
		void _DetachComponent( Kiwi::Component* component );

		void _SetEntity( Kiwi::Entity* entity );

		Kiwi::Component* _FindComponent( ComponentKey key );

		template<class ComponentClass>
		ComponentClass* _FindComponent()
		{
			//return first component matching the given type
			for( auto compItr = m_childComponents.begin(); compItr != m_childComponents.end(); compItr )
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

		Component() :
			m_entity( 0 ) { }
		Component( std::wstring name ) :
			GameObject( name ),
			m_entity( 0 ) { }
		virtual ~Component() = 0;

		virtual void FixedUpdate();
		virtual void Update();
		virtual void Shutdown();

		/*returns the entity to which the component is attached*/
		Kiwi::Entity* GetEntity()const { return m_entity; }

		virtual Kiwi::Component* AttachComponent( Kiwi::Component* component );

		/*detaches the component from the entity*/
		virtual void DetachComponent( Kiwi::Component* component );
		virtual void DetachComponent( std::wstring componentName );
		virtual void DetachComponent( int componentID );

		/*detaches and destroys the component*/
		void DestroyComponent( std::wstring componentName );
		void DestroyComponent( int componentID );

		/*returns the first component that matches the given type*/
		template<class ComponentClass>
		ComponentClass* FindComponent()
		{
			return this->_FindComponent<ComponentClass>();
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
}

#endif