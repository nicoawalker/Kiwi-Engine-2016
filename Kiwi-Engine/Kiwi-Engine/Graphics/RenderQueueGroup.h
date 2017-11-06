#ifndef _KIWI_RENDERQUEUEGROUP_H_
#define _KIWI_RENDERQUEUEGROUP_H_

#include <vector>

namespace Kiwi
{

	class Renderable;
	class Mesh;
	class Scene;
	class Viewport;

	class RenderQueueGroup
	{
	protected:

		std::wstring m_groupName;
		std::wstring m_renderTargetName;

		std::vector<Kiwi::Mesh*> m_sMeshes; //solid meshes
		std::vector<Kiwi::Mesh*> m_tMeshes; //transparent meshes
		std::vector<Kiwi::Mesh*> m_2DMeshes; //2d meshes

	public:

		RenderQueueGroup( std::wstring groupName, std::wstring renderTarget = L"BackBuffer" );
		~RenderQueueGroup();

		//void AddRenderable( Kiwi::Renderable* renderable );

		void AddMesh( Kiwi::Mesh* mesh );

		//sorts all of the renderables in the group relative to the passed viewport
		void Sort( Kiwi::Viewport& viewport );

		std::wstring GetName()const { return m_groupName; }
		std::wstring GetRenderTargetName()const { return m_renderTargetName; }

		std::vector<Kiwi::Mesh*>::iterator BeginSolids() { return m_sMeshes.begin(); }
		std::vector<Kiwi::Mesh*>::iterator EndSolids() { return m_sMeshes.end(); }

		std::vector<Kiwi::Mesh*>::iterator BeginTransparents() { return m_tMeshes.begin(); }
		std::vector<Kiwi::Mesh*>::iterator EndTransparents() { return m_tMeshes.end(); }

		std::vector<Kiwi::Mesh*>::iterator Begin2D() { return m_2DMeshes.begin(); }
		std::vector<Kiwi::Mesh*>::iterator End2D() { return m_2DMeshes.end(); }

		//empties the group
		void Clear();

	};
}

#endif