#include "Text.h"
#include "Font.h"
#include "Mesh.h"
#include "Renderable.h"

#include "..\Core\Scene.h"
#include "..\Core\Utilities.h"

namespace Kiwi
{

	Text::Text( std::wstring name, std::wstring font, const Kiwi::Vector2d& maxDimensions ):
		Kiwi::Component(name)
	{

		m_font = 0;
		m_fontName = font;
		m_textAlignment = Kiwi::Font::ALIGN_LEFT;
		m_maxCharacters = -1; //no maximum
		m_textColor = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );
		m_maxDimensions = maxDimensions;
		m_textMesh = 0;

	}

	Text::~Text()
	{
	}

	void Text::_OnAttached()
	{

		if( m_entity != 0 )
		{
			Kiwi::Scene* scene = m_entity->GetScene();
			if( scene != 0 )
			{
				m_font = scene->FindAsset<Kiwi::Font>( m_fontName );

				if( m_font == 0 )
				{
					throw Kiwi::Exception( L"Text", L"Font '" + m_fontName + L"' does not exist" );
				}

				m_textMesh = new Kiwi::Mesh( m_objectName + L"/mesh" );
				if( m_textMesh != 0 )
				{
					m_entity->AttachComponent( m_textMesh );
				}
			}

			m_entity->SetEntityType( Kiwi::Entity::ENTITY_2D );

			this->_GenerateTextMesh( m_text );
		}

	}

	void Text::_GenerateTextMesh( std::wstring text )
	{

		if( m_entity != 0 && text.size() > 0 )
		{
			if( m_textMesh == 0 )
			{
				m_textMesh = new Kiwi::Mesh( m_objectName + L"/mesh" );
				if( m_textMesh != 0 )
				{
					m_entity->AttachComponent( m_textMesh );
				}
			}

			double x = 0.0;
			double y = 0.0;
			float width = m_font->CalculateWidth( text );
			float height = m_font->GetCharacterHeight();

			if( width > m_maxDimensions.x )
			{
				std::wstring temp;
				float totalWidth = 0.0;
				float totalHeight = height;
				for( unsigned int charIndex = 0; charIndex < text.size(); charIndex++ )
				{
					if( text[charIndex] == L'\n' )
					{
						if( charIndex + 1 < text.size() )
						{
							temp += text[charIndex];
							totalHeight += height;
						}
						continue;
					}

					//std::wstring character = L"" + text[charIndex];
					float charWidth = m_font->GetCharacterWidth( text[charIndex] );
					totalWidth += charWidth;
					if( totalWidth > m_maxDimensions.x )
					{
						if( totalHeight + height <= m_maxDimensions.y )
						{//insert a new line
							totalWidth = 0.0;
							temp += L'\n';
							temp += text[charIndex];
							totalHeight += height;
							continue;

						} else
						{//exceeding max height, dont need any more characters
							break;
						}

					}
					temp += text[charIndex];
				}

				text = temp;
				width = m_maxDimensions.x;
			}

			m_dimensions.x = width;
			m_dimensions.y = height;

			switch( m_textAlignment )
			{
				case Font::ALIGN_LEFT:
					{
						//left-most edge of text will be at an x position of 0
						x = 0.0;
						y = height / 2.0;
						break;
					}
				case Font::ALIGN_CENTRE:
					{
						//text will be centered around the origin
						x = -width / 2.0;
						y = height / 2.0;
						break;
					}
				case Font::ALIGN_RIGHT:
					{
						//right-most edge of text will be at an x position of 0
						x = -width;
						y = height / 2.0;
						break;
					}
				default: return;
			}

			std::vector<Kiwi::Vector3d> vertices;
			std::vector<Kiwi::Vector2d> uvs;
			std::vector<Kiwi::Vector3d> normals;
			std::vector<Kiwi::Color> colors;

			float initialX = x;

			//for each letter, create a quad in the right position and of the right size
			for( unsigned int i = 0; i < text.length(); i++ )
			{

				if( text[i] == '\n' )
				{
					y -= height;
					x = initialX;
					continue;
				}

				int letter = (int)text[i];

				if( letter == 32 )
				{
					//space, just move over
					x += m_font->GetCharacter( 32 ).charWidth;

				}else
				{
					Kiwi::Font::Character character = m_font->GetCharacter( letter );

					//top left
					vertices.push_back( Kiwi::Vector3d( x, y, 0.0 ) );
					uvs.push_back( Kiwi::Vector2d( character.uvLeft, character.uvTop ) );
					normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
					colors.push_back( m_textColor );

					//top right
					vertices.push_back( Kiwi::Vector3d( x + character.charWidth, y, 0.0 ) );
					uvs.push_back( Kiwi::Vector2d( character.uvRight, character.uvTop ) );
					normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
					colors.push_back( m_textColor );

					//bottom left
					vertices.push_back( Kiwi::Vector3d( x, y - character.charHeight, 0.0 ) );
					uvs.push_back( Kiwi::Vector2d( character.uvLeft, character.uvBottom ) );
					normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
					colors.push_back( m_textColor );

					//bottom left
					vertices.push_back( Kiwi::Vector3d( x, y - character.charHeight, 0.0 ) );
					uvs.push_back( Kiwi::Vector2d( character.uvLeft, character.uvBottom ) );
					normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
					colors.push_back( m_textColor );

					//top right
					vertices.push_back( Kiwi::Vector3d( x + character.charWidth, y, 0.0 ) );
					uvs.push_back( Kiwi::Vector2d( character.uvRight, character.uvTop ) );
					normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
					colors.push_back( m_textColor );

					//bottom right
					vertices.push_back( Kiwi::Vector3d( x + character.charWidth, y - character.charHeight, 0.0 ) );
					uvs.push_back( Kiwi::Vector2d( character.uvRight, character.uvBottom ) );
					normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
					colors.push_back( m_textColor );

					x += (double)(character.charWidth + m_font->GetCharacterSpacing());
				}
			}

			m_textMesh->ClearAll();
			m_textMesh->SetVertices( vertices );
			m_textMesh->SetUVs( uvs );
			m_textMesh->SetNormals( normals );
			m_textMesh->SetColors( colors );

			Kiwi::Mesh::Submesh submesh;
			submesh.startIndex = 0;
			submesh.endIndex = (unsigned int)vertices.size();
			submesh.material.SetTexture( L"Diffuse", m_font->GetTexture() );
			submesh.material.SetColor( L"Diffuse", m_textColor );
			m_textMesh->AddSubmesh( submesh );

			m_textMesh->BuildMesh();
			m_textMesh->SetShader( m_shaderName );

			//release the memory used by vertices
			FreeMemory( vertices );
			FreeMemory( uvs );
			FreeMemory( normals );
		}

	}

	void Text::SetText( std::wstring text )
	{

		if( m_maxCharacters != -1 && text.length() > m_maxCharacters )
		{
			text = text.substr( 0, m_maxCharacters );
		}

		this->_GenerateTextMesh( text );

		m_text = text;

	}

	void Text::SetAlignment( Font::ALIGNMENT alignment )
	{

		if( alignment != m_textAlignment )
		{
			m_textAlignment = alignment;
			this->_GenerateTextMesh( m_text );
		}

	}

	void Text::SetColor( const Kiwi::Color& color )
	{

		if( m_textMesh )
		{
			if( m_textMesh->GetSubmeshCount() > 0 )
			{
				Kiwi::Mesh::Submesh* submesh = m_textMesh->GetSubmesh( 0 );
				if( submesh )
				{
					submesh->material.SetColor( L"Diffuse", color );
				}
			}
		}
		m_textColor = color;

	}

	void Text::SetShader( std::wstring shaderName )
	{

		if( m_textMesh != 0 )
		{
			m_textMesh->SetShader( shaderName );
		}

		m_shaderName = shaderName;

	}

	double Text::GetCharacterHeight()const
	{

		if( m_font != 0 )
		{
			return (double)m_font->GetCharacterHeight();

		} else
		{
			return 0.0;
		}

	}

};