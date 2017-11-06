#ifndef _KIWI_ISHADER_H_
#define _KIWI_ISHADER_H_

#include "DirectX.h"
#include "Mesh.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class Scene;
	class Renderer;
	class RenderTarget;
	class IShaderEffect;

	class IShader
	{
	friend class Renderer;
	protected:

		std::wstring m_shaderName;

		//shader objects
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		//raw shader data loaded from compiled shader .cso files
		ID3D10Blob* m_vertexBlob;
		ID3D10Blob* m_pixelBlob;

		std::vector<ID3D11Buffer*> m_vBuffers;
		std::vector<ID3D11Buffer*> m_pBuffers;
		std::vector<ID3D11SamplerState*> m_samplerStates;

		ID3D11InputLayout* m_inputLayout;

		std::wstring m_vertexShaderFile;
		std::wstring m_pixelShaderFile;

	public:

		IShader( std::wstring shaderName, Kiwi::Renderer* renderer, std::wstring vertexShaderFile, std::wstring pixelShaderFile );
		virtual ~IShader() = 0;

		/*sets parameters that are only updated once per scene*/
		virtual void SetSceneParameters( Kiwi::Scene* scene ) {}
		/*sets parameters that are updated once each frame*/
		virtual void SetFrameParameters( Kiwi::Scene* scene ) {}

		/*sets parameters that are updated for each entity*/
		virtual void SetObjectParameters( Kiwi::Scene* scene, Kiwi::RenderTarget* renderTarget, Kiwi::Mesh::Submesh* submesh ) {}

		std::wstring GetName()const { return m_shaderName; }

	};
}

#endif