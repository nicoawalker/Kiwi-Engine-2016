#ifndef _KIWI_RENDERQUEUE_H_
#define _KIWI_RENDERQUEUE_H_

#include "../Core/Entity.h"

#include "RenderQueueGroup.h"

#include <string>
#include <unordered_map>

namespace Kiwi
{

	class Renderable;
	class Scene;

	class RenderQueue
	{
	protected:

		Kiwi::Scene* m_parentScene;

		std::unordered_map<std::wstring, Kiwi::RenderQueueGroup*> m_renderGroups;

		std::wstring m_defaultRenderGroupName;

		unsigned int m_defaultPriority; //default priority to use for renderables that do not specify a priority

	public:

		RenderQueue( Kiwi::Scene& scene );
		~RenderQueue();

		virtual void Generate( Kiwi::EntityMap* entityMap );

		//clears the render queue
		virtual void Clear();

		//virtual void AddRenderable( Kiwi::Renderable* renderable, std::wstring renderGroup = L"" );
		virtual void QueueMesh( Kiwi::Mesh* mesh );

		virtual Kiwi::RenderQueueGroup* CreateRenderGroup( std::wstring groupName );

		void SetDefaultRenderGroupName( std::wstring name ) { m_defaultRenderGroupName = name; }
		
		RenderQueueGroup* GetRenderGroup( std::wstring groupName );
		RenderQueueGroup* GetDefaultRenderGroup();

		std::wstring GetDefaultRenderGroupName()const { return m_defaultRenderGroupName; }

		Kiwi::Scene* GetScene()const { return m_parentScene; }

	};
}

#endif