#include "RenderQueueGroup.h"
#include "Renderable.h"
#include "Mesh.h"
#include "Viewport.h"
#include "Camera.h"

#include "../Core/Scene.h"
#include "../Core/Entity.h"

#include <algorithm>

namespace Kiwi
{

	RenderQueueGroup::RenderQueueGroup( std::wstring groupName, std::wstring renderTarget ) :
	m_groupName( groupName ), m_renderTargetName( renderTarget )
	{	
	}


	RenderQueueGroup::~RenderQueueGroup()
	{



	}

	/*void RenderQueueGroup::AddRenderable( Kiwi::Renderable* renderable )
	{

		if( renderable )
		{
			if( renderable->GetRenderType() == Kiwi::Renderable::RENDER_3D )
			{
				if( renderable->GetMesh()->HasTransparency() )
				{
					m_transparents.push_back( renderable );
				} else
				{
					m_solids.push_back( renderable );
				}

			} else
			{
				m_2DRenderables.push_back( renderable );
			}
		}

	}*/

	void RenderQueueGroup::AddMesh( Kiwi::Mesh* mesh )
	{

		if( mesh )
		{
			Kiwi::Entity* entity = mesh->GetEntity();

			if( entity )
			{
				if( entity->GetType() == Kiwi::Entity::ENTITY_3D )
				{
					if( mesh->HasTransparency() )
					{
						m_tMeshes.push_back( mesh );

					} else
					{
						m_sMeshes.push_back( mesh );
					}

				} else if( entity->GetType() == Kiwi::Entity::ENTITY_2D )
				{
					m_2DMeshes.push_back( mesh );
				}
			}
		}

	}

	void RenderQueueGroup::Sort( Kiwi::Viewport& viewport )
	{

		//std::sort( m_sMeshes.begin(), m_sMeshes.end(), [this]( Kiwi::Mesh* m1, Kiwi::Mesh* m2 )
		//{
		//	if( m1 != m2 )
		//	{
		//		//meshes are different, sort by mesh pointer
		//		return (r1->GetMesh() < r2->GetMesh());

		//	} else
		//	{
		//		//same mesh, sort by the hashed value of the shader's name
		//		unsigned int s1Hash = std::hash<std::wstring>()(r1->GetShader());
		//		unsigned int s2Hash = std::hash<std::wstring>()(r2->GetShader());
		//		return s1Hash < s2Hash;
		//	}
		//} );

		std::sort( m_tMeshes.begin(), m_tMeshes.end(), [this, viewport]( Kiwi::Mesh* m1, Kiwi::Mesh* m2 )
		{
			//renderables have transparency, sort only by depth
			Kiwi::Camera* cam = viewport.GetCamera();
			Kiwi::Transform* camT = cam->FindComponent<Kiwi::Transform>();
			Kiwi::Transform* m1T = m1->GetEntity()->FindComponent<Kiwi::Transform>();
			Kiwi::Transform* m2T = m2->GetEntity()->FindComponent<Kiwi::Transform>();

			return m1T->GetSquareDistance(camT) < m2T->GetSquareDistance(camT);
		} );

		/*sorts the renderables, first by mesh, then by shader*/
		std::sort( m_2DMeshes.begin(), m_2DMeshes.end(), [this]( Kiwi::Mesh* m1, Kiwi::Mesh* m2 )
		{
			//renderables are 2D, sort only by depth
			Kiwi::Transform* m1T = m1->GetEntity()->FindComponent<Kiwi::Transform>();
			Kiwi::Transform* m2T = m2->GetEntity()->FindComponent<Kiwi::Transform>();
			return m1T->GetPosition().z < m2T->GetPosition().z;
		} );

	}

	//empties the group
	void RenderQueueGroup::Clear()
	{

		m_sMeshes.clear();
		m_tMeshes.clear();
		m_2DMeshes.clear();

	}

}