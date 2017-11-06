#include "Entity.h"
#include "Transform.h"
#include "Utilities.h"
#include "Scene.h"
#include "IEntitySpawner.h"
#include "EngineRoot.h"

#include "../Graphics/Renderable.h"
#include "../Graphics/Mesh.h"

#include "../Physics/Rigidbody.h"

namespace Kiwi
{

	Entity::Entity( std::wstring name, Kiwi::Scene& scene ):
		Kiwi::GameObject(name)
	{

		m_isActive = true;
		m_scene = &scene;
		m_parent = 0;
		m_entityType = ENTITY_3D;
		m_spawner = 0;
		m_mesh = 0;
		m_transform = 0;
		m_rigidbody = 0;

		this->AttachComponent( new Kiwi::Transform() );

	}

	Entity::~Entity()
	{

		if( !m_isShutdown )
		{
			//shutdown and free any active children
			for( auto childItr = m_childEntities.begin(); childItr != m_childEntities.end(); childItr++ )
			{
				if( childItr->second != 0 )
				{
					//free the entity so it may be deleted
					childItr->second->Free();
					childItr->second->Shutdown();
				}
			}

			//destroy components
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second != 0 )
				{
					compItr->second->Shutdown();
				}
			}
		}

		Kiwi::FreeMemory( m_components );
		Kiwi::FreeMemory( m_childEntities );

	}

	void Entity::Shutdown()
	{

		if( !m_isShutdown )
		{
			this->_OnShutdown();

			m_isShutdown = true;
			m_isActive = false;

			if( m_parent )
			{
				//m_parent->DetachChild( this );
			}

			if( m_spawner )
			{
				//m_spawner->RemoveEntity( this );
			}

			//shutdown child entities
			for( auto childItr = m_childEntities.begin(); childItr != m_childEntities.end(); childItr++ )
			{
				if( childItr->second != 0 )
				{
					childItr->second->Free(); //free the entity so it may be deleted
					childItr->second->Shutdown();
				}
			}

			//shutdown components
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second != 0 )
				{
					compItr->second->Shutdown();
				}
			}
		}

	}

	void Entity::_AttachComponent( Kiwi::Component* component )
	{
		ComponentKey key( component->GetID(), component->GetName() );
		if( this->_FindComponent( key ) )
		{//component already exists
			return;

		} else
		{
			m_components.insert( std::make_pair( key, std::unique_ptr<Kiwi::Component>( component ) ) );
			component->_SetEntity( this );
			component->_OnAttached();
		}
	}

	void Entity::_DetachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			if( component == m_transform )
			{
				m_transform = 0;

			} else if( component == m_mesh )
			{
				m_mesh = 0;

			} else if( component == m_rigidbody )
			{
				m_rigidbody = 0;
			}

			auto compItr = m_components.find( ComponentKey( component->GetID(), component->GetName() ) );
			if( compItr != m_components.end() && compItr->second.get() != m_transform )
			{
				Kiwi::Component* comp = compItr->second.get();
				if( comp->IsShutdown() == false )
				{
					comp->_OnDetached();
					comp->_SetEntity( 0 );
					comp->Shutdown();
				}
				m_components.erase( compItr );
			}
		}

	}

	Kiwi::Component* Entity::_FindComponent( ComponentKey key )
	{

		auto compItr = m_components.find( key );
		if( compItr != m_components.end() )
		{
			return compItr->second.get();
		}

		return 0;
	}

	void Entity::_RemoveAllShutdown()
	{

		for( auto compItr = m_components.begin(); compItr != m_components.end(); )
		{
			if( compItr->second->IsShutdown() == true )
			{
				compItr = m_components.erase( compItr );
				continue;
			}

			compItr++;
		}

		//update children
		std::vector<Kiwi::Entity*> shutdownChildren;
		for( auto childItr = m_childEntities.begin(); childItr != m_childEntities.end(); childItr++ )
		{
			if( childItr->second != 0 )
			{
				if( childItr->second->IsShutdown() == false && childItr->second->IsActive() == true )
				{
					childItr->second->FixedUpdate();

				} else if( childItr->second->IsShutdown() == true )
				{
					shutdownChildren.push_back( childItr->second );
				}
			}
		}

		//detach and free all shutdown children
		for( auto itr = shutdownChildren.begin(); itr != shutdownChildren.end(); itr++ )
		{
			this->DetachChild( *itr );
		}

	}

	void Entity::Update()
	{

		static double elapsed = 0.0;

		if( !m_isShutdown && m_isActive )
		{
			this->_OnUpdate();

			m_transform->Update();

			//update all components
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second->IsShutdown() == false && compItr->second->IsActive() == true )
				{
					compItr->second->Update();
				}
			}

			//update children
			for( auto childItr = m_childEntities.begin(); childItr != m_childEntities.end(); )
			{
				if( childItr->second != 0 )
				{
					childItr->second->Update();

				} else
				{
					childItr = m_childEntities.erase( childItr );
					continue;
				}
				childItr++;
			}

			elapsed += m_scene->GetEngine()->GetGameTimer()->GetDeltaTime();
			//once per second check for and remove shutdown children and components
			if( elapsed >= 1.0 )
			{
				elapsed -= 1.0;
				//this->_RemoveAllShutdown();
			}

		}

	}

	void Entity::FixedUpdate()
	{

		if( !m_isShutdown && m_isActive )
		{
			this->_OnFixedUpdate();

			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second->IsShutdown() == false && compItr->second->IsActive() == true )
				{
					compItr->second->FixedUpdate();

				}
			}

			//update children
			for( auto childItr = m_childEntities.begin(); childItr != m_childEntities.end(); childItr++ )
			{
				if( childItr->second != 0 )
				{
					if( childItr->second->IsShutdown() == false && childItr->second->IsActive() == true )
					{
						childItr->second->FixedUpdate();

					}
				}
			}
		}

	}

	void Entity::_OnActivate()
	{

		for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
		{
			itr->second->SetActive( true );
		}

		for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
		{
			compItr->second->SetActive( true );
		}
	}

	void Entity::_OnDeactivate()
	{

		for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
		{
			itr->second->SetActive( false );
		}

		for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
		{
			compItr->second->SetActive( false );
		}
	}

	Kiwi::Entity* Entity::FindChildWithName( std::wstring name )
	{

		auto it = m_childEntities.find( name );
		if( it != m_childEntities.end() )
		{
			return it->second;

		} else
		{
			return 0;
		}

	}

	std::vector<Kiwi::Entity*> Entity::FindChildrenWithTag( std::wstring tag )
	{

		std::vector<Kiwi::Entity*> matches;

		for( const auto& it : m_childEntities )
		{
			if( it.second->HasTag( tag ) )
			{
				matches.push_back( it.second );
			}
		}

		return matches;

	}

	void Entity::AttachChild( Kiwi::Entity* entity )
	{

		if( entity == 0 ) return;

		m_childEntities.insert( std::make_pair( entity->GetName(), entity ) );
		entity->m_parent = this;
		entity->Reserve(); //reserve the entity so it will not be deleted until it is released

		Kiwi::Transform* entityTransform = entity->FindComponent<Kiwi::Transform>();
		if( entityTransform != 0 )
		{
			entityTransform->Update();
		}

	}

	Kiwi::Entity* Entity::DetachChild( Kiwi::Entity* entity )
	{

		if( entity == 0 ) return 0;

		for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
		{
			if( itr->second == entity )
			{
				Kiwi::Entity* ent = itr->second;

				if( ent != 0 )
				{
					ent->m_parent = 0;
					ent->Free();

					Kiwi::Transform* entityTransform = ent->FindComponent<Kiwi::Transform>();
					if( entityTransform != 0 )
					{
						entityTransform->Update();
					}
				}
				m_childEntities.erase( itr );

				return  ent;
			}
		}

		return 0;

	}

	Kiwi::Entity* Entity::DetachChildWithName( std::wstring name )
	{

		auto it = m_childEntities.find( name );
		if( it != m_childEntities.end() )
		{
			Kiwi::Entity* ent = it->second;
			if( ent != 0 )
			{
				ent->m_parent = 0;
				ent->Free();
				Kiwi::Transform* entityTransform = ent->FindComponent<Kiwi::Transform>();
				if( entityTransform != 0 )
				{
					entityTransform->Update();
				}
			}
			m_childEntities.erase( it );

			return ent;

		} else
		{//wasnt found, try the same for all children

			for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
			{
				if( itr->second != 0 )
				{
					Kiwi::Entity* ent = itr->second->DetachChildWithName( name );
					if( ent != 0 )
					{
						return ent;
					}
				}
			}
		}

		return 0;

	}

	void Entity::SetParent( Kiwi::Entity* parent )
	{

		if( m_parent != 0 )
		{
			m_parent->DetachChild( this );
			m_parent = 0;
		}

		m_parent = parent;

		if( m_parent ) m_parent->AttachChild( this );

	}

	bool Entity::HasChild()
	{

		if( m_childEntities.size() > 0 )
		{
			return true;
		} else
		{
			return false;
		}

	}

	Kiwi::Component* Entity::AttachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			this->_AttachComponent( component );
		}

		return component;

	}

	Kiwi::Transform* Entity::AttachComponent( Kiwi::Transform* transform )
	{

		if( transform )
		{
			if( m_transform != 0 )
			{
				this->_DetachComponent( m_transform );
				m_transform->RemoveAllListeners();
			}

			this->_AttachComponent( transform );
			transform->AddTransformListener( this );
			m_transform = transform;
		}

		return transform;

	}

	Kiwi::Mesh* Entity::AttachComponent( Kiwi::Mesh* mesh )
	{

		if( mesh )
		{
			if( m_mesh != 0 )
			{
				this->_DetachComponent( m_mesh );
				//m_mesh->Shutdown();
				//SAFE_DELETE( m_mesh );
			}

			this->_AttachComponent( mesh );
			m_mesh = mesh;
		}

		return mesh;

	}

	Kiwi::Rigidbody* Entity::AttachComponent( Kiwi::Rigidbody* rigidbody )
	{

		if( m_rigidbody == rigidbody )
		{
			return m_rigidbody;
		}

		if( rigidbody )
		{
			if( m_rigidbody )
			{
				m_scene->GetPhysicsSystem()->RemoveRigidbody( m_rigidbody );
				this->_DetachComponent( m_rigidbody );
			}

			this->_AttachComponent( rigidbody );
			m_rigidbody = rigidbody;

			m_scene->GetPhysicsSystem()->AddRigidbody( rigidbody );

		} else if( m_rigidbody )
		{
			m_scene->GetPhysicsSystem()->RemoveRigidbody( m_rigidbody );
			this->_DetachComponent( m_rigidbody );
		}

		return rigidbody;

	}

	void Entity::DetachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			this->_DetachComponent( component );
		}

	}

	void Entity::DetachComponent( std::wstring componentName )
	{

		auto compItr = m_components.find( ComponentKey( 0, componentName ) );
		if( compItr != m_components.end() && compItr->second != 0 )
		{
			this->_DetachComponent( compItr->second.get() );
		}

	}

	void Entity::DetachComponent( int componentID )
	{

		auto compItr = m_components.find( ComponentKey( componentID ) );
		if( compItr != m_components.end() && compItr->second != 0 )
		{
			this->_DetachComponent( compItr->second.get() );
		}

	}

};