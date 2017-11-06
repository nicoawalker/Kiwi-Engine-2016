#include "Renderable2D.h"

#include "../Core/Exception.h"

namespace Kiwi
{

	Renderable2D::Renderable2D(Kiwi::Entity* parentEntity):
		Kiwi::Renderable(parentEntity)
	{
		
		m_renderType = RENDER_2D;

		float xPos = 0.5f;
		float yPos = 0.5f;

		std::vector<Kiwi::Vector3d> meshVertices = {
			Kiwi::Vector3d( -xPos, yPos, 0.0f ),
			Kiwi::Vector3d( xPos, yPos, 0.0f ),
			Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
			Kiwi::Vector3d( -xPos, -yPos, 0.0f ),
			Kiwi::Vector3d( xPos, yPos, 0.0f ),
			Kiwi::Vector3d( xPos, -yPos, 0.0f )
		};

		std::vector<Kiwi::Vector3d> meshNormals = {
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
			Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
		};

		std::vector<Kiwi::Vector2d> meshUVs = {
			Kiwi::Vector2d( 0.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 0.0, 1.0 ),
			Kiwi::Vector2d( 1.0, 0.0 ),
			Kiwi::Vector2d( 1.0, 1.0 ),
		};

		m_mesh->SetVertices( meshVertices );
		m_mesh->SetUVs( meshUVs );
		m_mesh->SetNormals( meshNormals );
		m_mesh->BuildMesh();

		m_renderTarget = L"BackBuffer";
		m_shader = L"Default2DShader";

	}

}