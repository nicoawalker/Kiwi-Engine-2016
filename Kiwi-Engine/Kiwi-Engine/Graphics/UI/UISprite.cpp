#include "UISprite.h"

#include "../Texture.h"

#include "../../Core/Scene.h"
#include "../../Core/Exception.h"

namespace Kiwi
{

	UISprite::UISprite( std::wstring name, std::wstring textureName ) :
		Kiwi::Component( name )
	{

		m_textureName = textureName;
		m_spriteTexture = 0;

	}

	UISprite::~UISprite()
	{
	}

	void UISprite::_OnAttached()
	{

		Kiwi::Scene* scene = m_entity->GetScene();

		m_spriteTexture = scene->FindAsset<Kiwi::Texture>( m_textureName );

		m_entity->AttachComponent( Kiwi::Mesh::Primitive( Kiwi::Mesh::PRIMITIVE_TYPE::QUAD, m_objectName + L"/mesh" ) );
		
		Kiwi::Mesh* mesh = m_entity->FindComponent<Kiwi::Mesh>();
		if( mesh != 0 )
		{
			mesh->SetShader( L"Default2DShader" );
			Kiwi::Mesh::Submesh* submesh = mesh->GetSubmesh( 0 );

			if( submesh != 0 )
			{
				submesh->material.SetColor( L"Diffuse", Kiwi::Color( 0.0, 0.0, 0.0, 1.0 ) );

				if( m_spriteTexture != 0 )
				{
					submesh->material.SetTexture( L"Diffuse", m_spriteTexture );
				}
			}
		}

		m_entity->SetEntityType( Kiwi::Entity::ENTITY_2D );

	}

	void UISprite::SetTexture( std::wstring textureName )
	{

		if( m_entity != 0 )
		{
			Kiwi::Scene* scene = m_entity->GetScene();
			assert( scene != 0 );
			if( scene == 0 ) return;

			Kiwi::Texture* texture = scene->FindAsset<Kiwi::Texture>( textureName );

			if( texture )
			{
				m_spriteTexture = texture;
				m_textureName = textureName;
				Kiwi::Mesh* mesh = m_entity->FindComponent<Kiwi::Mesh>();
				if( mesh != 0 )
				{
					Kiwi::Mesh::Submesh* submesh = mesh->GetSubmesh( 0 );
					if( submesh != 0 )
					{
						submesh->material.SetTexture( L"Diffuse", texture );
					}
				}
			}

		} else
		{
			m_textureName = textureName;
		}

	}

	void UISprite::SetColor( const Kiwi::Color& color )
	{
		if( m_entity )
		{
			Kiwi::Mesh* mesh = m_entity->FindComponent<Kiwi::Mesh>();
			if( mesh != 0 )
			{
				Kiwi::Mesh::Submesh* submesh = mesh->GetSubmesh( 0 );
				if( submesh != 0 )
				{
					submesh->material.SetColor( L"Diffuse", color );
				}
			}
		}
	}

}