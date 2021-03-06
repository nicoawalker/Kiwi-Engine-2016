#ifndef _KIWI_MATERIAL_H_
#define _KIWI_MATERIAL_H_

#include "Color.h"

#include "../Core/Component.h"

namespace Kiwi
{

	class Texture;
	class Mesh;
	class IShaderEffect;

	class Material
	{
	protected:

		//store a pointer to the parent mesh for this material
		Kiwi::Mesh* m_mesh;

		Kiwi::IShaderEffect* m_shaderEffect;

		std::wstring m_shader;
		std::wstring m_renderGroup;

		Kiwi::Texture* m_diffuseMap; //main texture
		Kiwi::Texture* m_bumpMap;
		Kiwi::Texture* m_ambientMap;
		Kiwi::Texture* m_specularMap;

		Kiwi::Color m_diffuseColor; //color if not textured
		Kiwi::Color m_ambientColor;
		Kiwi::Color m_specularColor;
		Kiwi::Color m_emissiveColor;
		Kiwi::Color m_transmissionFilter;

		float m_reflectivity;
		float m_opticalDensity;
		int m_illum;

	public:

		//creates an empty colored material, initialized to the color white
		Material();
		//creates a colored material 
		Material( const Kiwi::Color& diffuseColor );
		//creates a textured material
		Material( Kiwi::Texture* diffuseMap, Kiwi::Texture* bumpMap = 0, Kiwi::Texture* ambientMap = 0, Kiwi::Texture* specularMap = 0 );

		virtual ~Material();

		void SetMesh( Kiwi::Mesh* mesh ) { m_mesh = mesh; }

		/*sets one of the material's textures, depending on the value of 'textureType'
		possible values are: Diffuse, Ambient, Bump, or Specular*/
		void SetTexture( std::wstring textureType, Kiwi::Texture* texture );

		/*sets one of the material's color values, depending on the value of 'colorType'
		possible values are: Diffuse, Ambient, Specular, Emissive, or Transmission*/
		void SetColor( std::wstring colorType, const Kiwi::Color& color );

		void SetReflectivity( float reflectivity ) { m_reflectivity = reflectivity; }
		void SetOpticalDensity( float opticalDensity ) { m_opticalDensity = opticalDensity; }
		void SetIllumination( int illumination ) { m_illum = illumination; }

		/*sets the alpha value of this material's diffuse color, which stores
		the transparency value for the material*/
		void SetTransparency( float transparency ) { m_diffuseColor.alpha = transparency; }

		void SetShader( std::wstring shader ) { m_shader = shader; }
		void SetRenderGroup( std::wstring renderGroup ) { m_renderGroup = renderGroup; }

		bool IsTextured()const { return m_diffuseMap != 0; }
		bool HasTransparency()const;

		float GetReflectivity()const { return m_reflectivity; }
		float GetOpticalDensity()const { return m_opticalDensity; }
		int GetIllumination()const { return m_illum; }
		std::wstring GetShader()const { return m_shader; }
		std::wstring GetRenderGroup()const { return m_renderGroup; }

		/*returns one of the material's textures, depending on the value of 'textureType'
		possible values are: Diffuse, Ambient, Bump, or Specular*/
		Kiwi::Texture* GetTexture(std::wstring textureType)const;
		/*returns one of the material's color values, depending on the value of 'colorType'
		possible values are: Diffuse, Ambient, Specular, Emissive, or Transmission*/
		const Kiwi::Color& GetColor(std::wstring colorType)const;

	};
};

#endif