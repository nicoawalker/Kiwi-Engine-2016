#include "Component.h"
#include "Entity.h"
#include "Utilities.h"

namespace Kiwi
{

	Component::~Component()
	{

		if( !m_isShutdown )
		{
			MessageBox( NULL, m_objectName.c_str(), L"Component destroyed without being shutdown", MB_OK );
		}

		for( auto itr = m_childComponents.begin(); itr != m_childComponents.end(); itr++ )
		{
			SAFE_DELETE( itr->second );
		}
		Kiwi::FreeMemory( m_childComponents );

	}

	void Component::_AttachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			ComponentKey key( component->GetID(), component->GetName() );
			if( this->_FindComponent( key ) )
			{//component already exists
				return;

			} else
			{
				m_childComponents[key] = component;
				component->m_entity = m_entity;
				component->_OnAttached();
			}
		}
	}

	void Component::_DetachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			auto compItr = m_childComponents.find( ComponentKey( component->GetID(), component->GetName() ) );
			if( compItr != m_childComponents.end() )
			{
				compItr->second->_OnDetached();

				/*shutdown and erase the component. it will be deleted by the scene*/
				compItr->second->Shutdown();
				SAFE_DELETE( compItr->second );
				m_childComponents.erase( compItr );
			}
		}

	}

	void Component::_SetEntity( Kiwi::Entity* entity )
	{

		m_entity = entity;

		for( auto compItr = m_childComponents.begin(); compItr != m_childComponents.end(); compItr++ )
		{
			if( compItr->second != 0 )
			{
				compItr->second->_SetEntity( m_entity );
			}
		}

	}

	void Component::FixedUpdate()
	{
		if( !m_isShutdown && m_isActive )
		{
			for( auto compItr = m_childComponents.begin(); compItr != m_childComponents.end(); compItr++ )
			{
				compItr->second->FixedUpdate();
			}

			this->_OnFixedUpdate();
		}
	}

	void Component::Update()
	{
		if( !m_isShutdown && m_isActive )
		{
			//update all child components
			for( auto compItr = m_childComponents.begin(); compItr != m_childComponents.end(); compItr++ )
			{
				compItr->second->Update();
			}

			this->_OnUpdate();
		}
	}

	void Component::Shutdown()
	{

		if( !m_isShutdown )
		{
			m_isShutdown = true;
			m_isActive = false;

			//this->_OnShutdown();

			//shutdown the child components
			for( auto itr = m_childComponents.begin(); itr != m_childComponents.end(); itr++ )
			{
				itr->second->Shutdown();
			}

			/*if( m_entity )
			{
				m_entity->DetachComponent( this );
				m_entity = 0;
			}*/
		}

	}

	Kiwi::Component* Component::AttachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			this->_AttachComponent( component );
		}

		return component;

	}

	void Component::DetachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			this->_DetachComponent( component );
		}

	}

	void Component::DetachComponent( std::wstring componentName )
	{

		auto compItr = m_childComponents.find( ComponentKey( 0, componentName ) );
		if( compItr != m_childComponents.end() )
		{
			compItr->second->_OnDetached();

			/*shutdown and erase the component. it will be deleted by the scene*/
			compItr->second->Shutdown();
			SAFE_DELETE( compItr->second );
			m_childComponents.erase( compItr );
		}

	}

	void Component::DetachComponent( int componentID )
	{

		auto compItr = m_childComponents.find( ComponentKey( componentID ) );
		if( compItr != m_childComponents.end() )
		{
			compItr->second->_OnDetached();

			/*shutdown and erase the component. it will be deleted by the scene*/
			compItr->second->Shutdown();
			SAFE_DELETE( compItr->second );
			m_childComponents.erase( compItr );
		}

	}

	void Component::DestroyComponent( std::wstring componentName )
	{

		auto compItr = m_childComponents.find( ComponentKey( 0, componentName ) );
		if( compItr != m_childComponents.end() )
		{
			Kiwi::Component* comp = compItr->second;
			comp->_OnDetached();

			/*shutdown and erase the component. it will be deleted by the scene*/
			comp->Shutdown();
			m_childComponents.erase( compItr );
			SAFE_DELETE( comp );
		}

	}

	void Component::DestroyComponent( int componentID )
	{

		auto compItr = m_childComponents.find( ComponentKey( componentID ) );
		if( compItr != m_childComponents.end() )
		{
			Kiwi::Component* comp = compItr->second;
			comp->_OnDetached();

			/*shutdown and erase the component. it will be deleted by the scene*/
			comp->Shutdown();
			m_childComponents.erase( compItr );
			SAFE_DELETE( comp );
		}

	}

	Kiwi::Component* Component::_FindComponent( ComponentKey key )
	{
		auto compItr = m_childComponents.find( key );
		if( compItr != m_childComponents.end() )
		{
			return compItr->second;
		}

		return 0;
	}

}