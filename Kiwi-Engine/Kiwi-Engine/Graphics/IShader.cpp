#include "IShader.h"
#include "Renderer.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"

namespace Kiwi
{

	IShader::IShader( std::wstring shaderName, Kiwi::Renderer* renderer, std::wstring vertexShaderFile, std::wstring pixelShaderFile )
	{

		assert( renderer != 0 );

		m_vertexBlob = 0;
		m_pixelBlob = 0;
		m_vertexShader = 0;
		m_pixelShader = 0;
		m_inputLayout = 0;

		m_shaderName = shaderName;
		m_vertexShaderFile = vertexShaderFile;
		m_pixelShaderFile = pixelShaderFile;

		//first load the compiled shaders from the cso files
		//load vertex shader
		HRESULT hr = D3DReadFileToBlob( vertexShaderFile.c_str(), &m_vertexBlob );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to D3DReadFileToBlob failed for file '" + vertexShaderFile + L"': " + Kiwi::GetD3DErrorString( hr ) );
		}
		//load pixel shader
		hr = D3DReadFileToBlob( pixelShaderFile.c_str(), &m_pixelBlob );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to D3DReadFileToBlob failed for file '" + pixelShaderFile + L"': " + Kiwi::GetD3DErrorString( hr ) );
		}

		//create the vertex shader from the compiled data
		hr = renderer->GetDevice()->CreateVertexShader( m_vertexBlob->GetBufferPointer(), m_vertexBlob->GetBufferSize(), NULL, &m_vertexShader );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to CreateVertexShader failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

		//create the pixel shader from the compiled data
		hr = renderer->GetDevice()->CreatePixelShader( m_pixelBlob->GetBufferPointer(), m_pixelBlob->GetBufferSize(), NULL, &m_pixelShader );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to CreatePixelShader failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

	}

	IShader::~IShader()
	{

		auto vItr = m_vBuffers.begin();
		for( ; vItr != m_vBuffers.end(); )
		{
			SAFE_RELEASE( *vItr );
			vItr = m_vBuffers.erase( vItr );
		}

		auto pItr = m_pBuffers.begin();
		for( ; pItr != m_pBuffers.end(); )
		{
			SAFE_RELEASE( *pItr );
			pItr = m_pBuffers.erase( pItr );
		}

		auto sItr = m_samplerStates.begin();
		for( ; sItr != m_samplerStates.end(); )
		{
			SAFE_RELEASE( *sItr );
			sItr = m_samplerStates.erase( sItr );
		}

		SAFE_RELEASE( m_inputLayout );
		SAFE_RELEASE( m_vertexBlob );
		SAFE_RELEASE( m_pixelBlob );
		SAFE_RELEASE( m_vertexShader );
		SAFE_RELEASE( m_pixelShader );

	}

}