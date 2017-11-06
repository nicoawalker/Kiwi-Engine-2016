#include "GameObject.h"
#include "Utilities.h"

namespace Kiwi
{

	int GameObject::GlobalObjectID = 1;
	std::mutex GameObject::GlobalObjectIDMutex;

	GameObject::GameObject()
	{

		GlobalObjectIDMutex.lock();
		m_objectID = GlobalObjectID++;
		if( GlobalObjectID == 0 )
		{
			GlobalObjectID++;
		}
		GlobalObjectIDMutex.unlock();

		m_isShutdown = false;
		m_isActive = true;

	}

	GameObject::GameObject(std::wstring name)
	{

		GlobalObjectIDMutex.lock();
		m_objectID = GlobalObjectID++;
		if( GlobalObjectID == 0 )
		{
			GlobalObjectID++;
		}
		GlobalObjectIDMutex.unlock();

		m_objectName = name;
		m_isShutdown = false;
		m_isActive = true;

	}

	GameObject::~GameObject()
	{

		if( !m_isShutdown )
		{
			this->Shutdown();
		}

		Kiwi::FreeMemory( m_tags );

	}

	void GameObject::Shutdown()
	{
		if( !m_isShutdown )
		{
			m_isShutdown = true;
			m_isActive = false;

			this->_OnShutdown();
		}
	}

	void GameObject::FixedUpdate()
	{
		if( !m_isShutdown && m_isActive )
		{
			this->_OnFixedUpdate();
		}
	}

	void GameObject::Update()
	{
		if( !m_isShutdown && m_isActive )
		{
			this->_OnUpdate();
		}
	}

	void GameObject::RemoveAllTags()
	{

		//empty the tag list and free the memory
		Kiwi::FreeMemory( m_tags );

	}

	/*checks if the entity has a certain tag*/
	bool GameObject::HasTag( std::wstring tag )
	{

		auto it = m_tags.find( tag );
		if( it != m_tags.end() )
		{
			return true;
		} else
		{
			return false;
		}

	}

	void GameObject::SetActive( bool isActive )
	{
		if( isActive != m_isActive )
		{
			m_isActive = isActive;
			if( m_isActive )
			{
				this->_OnActivate();

			} else
			{
				this->_OnDeactivate();
			}
		}
	}

}