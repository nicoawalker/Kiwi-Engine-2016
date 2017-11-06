#include "Font.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

#include "..\Core\Utilities.h"
#include "..\Core\Logger.h"
#include "..\Core\Transform.h"
#include "..\Core\Exception.h"

namespace Kiwi
{

	Font::Font( std::wstring fontName, Kiwi::Texture* texture, std::vector<Font::Character> characters ) :
		Kiwi::IAsset( fontName, L"Font" )
	{

		m_texture = 0;
		if( texture == 0 )
		{
			throw Kiwi::Exception( L"Font", L"[" + m_fontName + L"] Font was provided with null texture" );
		}
		if( characters.size() == 0 )
		{
			throw Kiwi::Exception( L"Font", L"[" + m_fontName + L"] Font was provided with empty character set" );
		}

		m_fontName = fontName;
		m_texture = texture;
		m_fontCharacters = characters;
		m_characterSpacing = 0.0f;

	}

	Font::~Font()
	{

		Kiwi::FreeMemory( m_fontCharacters );

	}

	float Font::CalculateWidth( std::wstring text )
	{

		float width = 0.0f;
		for( unsigned int i = 0; i < text.size(); i++ )
		{
			if( (int)text[i] < m_fontCharacters.size() )
			{
				width += (float)m_fontCharacters[text[i]].charWidth;
				width += m_characterSpacing;
			}
		}

		return width;

	}

	float Font::GetCharacterWidth( wchar_t character )
	{

		if( character < m_fontCharacters.size() )
		{
			return (float)m_fontCharacters[character].charWidth;
		}

		return 0.0f;

	}

};