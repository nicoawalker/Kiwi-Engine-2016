#ifndef _KIWI_GAMEOBJECT_H_
#define _KIWI_GAMEOBJECT_H_

#include "IReferencedObject.h"

#include <string>
#include <mutex>
#include <unordered_set>

namespace Kiwi
{

	class GameObject:
		public Kiwi::IReferencedObject
	{
	protected:
		//stores the current object ID that will be assigned to the next created object
		static int GlobalObjectID; 
		static std::mutex GlobalObjectIDMutex;

		int m_objectID;

		std::wstring m_objectName;

		/*optional, non-unique identifiers*/
		std::unordered_set<std::wstring> m_tags;

		bool m_isShutdown;
		bool m_isActive;

	protected:

		virtual void _OnShutdown() {}
		virtual void _OnFixedUpdate() {}
		virtual void _OnUpdate() {}
		virtual void _OnActivate() {}
		virtual void _OnDeactivate() {}
		
	public:

		GameObject();
		GameObject( std::wstring name );
		virtual ~GameObject() = 0;

		void Shutdown();
		virtual void FixedUpdate();
		virtual void Update();

		virtual void AddTag( std::wstring tag ) { m_tags.insert( tag ); }

		/*removes the matching string from the entity*/
		virtual void RemoveTag( std::wstring tag ) { m_tags.erase( tag ); }
		/*removes all tags from the entity*/
		virtual void RemoveAllTags();

		/*checks if the entity has a certain tag*/
		virtual bool HasTag( std::wstring tag );

		bool IsShutdown()const { return m_isShutdown; }
		bool IsActive()const { return m_isActive; }

		void SetActive( bool isActive );
		void SetName( std::wstring name ) { m_objectName = name; }

		std::wstring GetName()const { return m_objectName; }
		int GetID()const { return m_objectID; }

	};
}

#endif