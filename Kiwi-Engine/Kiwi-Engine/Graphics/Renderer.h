#ifndef _KIWI_RENDERER_H_
#define _KIWI_RENDERER_H_

#include "DirectX.h"
#include "RasterStateManager.h"
#include "Color.h"

#include "../Core/Vector2.h"
#include "../Core/Vector4.h"
#include "../Core/IThreadSafe.h"

#include <string>

namespace Kiwi
{

	struct BackBuffer;

	class D3D11Interface;
	class RenderWindow;
	class RenderTarget;
	class DepthStencil;
	class BlendState;
	class Viewport;
	class IShader;
	class IBuffer;
	class RenderQueue;

	enum PrimitiveTopology
	{
		POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	struct MultisampleDesc
	{
		//The number of multisamples per pixel.
		unsigned int count;

		/*The image quality level. The higher the quality, the lower the performance.
		The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultisampleQualityLevels*/
		unsigned int quality;
	};

	class Renderer:
		public Kiwi::IThreadSafe
	{
	private:

		std::wstring m_name;

		Kiwi::D3D11Interface* m_d3dInterface;

		Kiwi::RenderWindow* m_renderWindow;

		//the render target that is currently set to be drawn to
		Kiwi::RenderTarget* m_activeRenderTarget;

		Kiwi::RenderTarget* m_backBuffer;

		Kiwi::RasterStateManager* m_rasterStateManager;

		Kiwi::DepthStencil* m_activeDepthStencil;

		Kiwi::PrimitiveTopology m_activePrimitiveTopology;

		bool m_depthEnable;

	public:

		Renderer(std::wstring name, Kiwi::RenderWindow* window);
		~Renderer();

		/*Draws an indexed mesh, without instancing.
		indexCount: Number of indices to draw.
		startIndex: The location of the first index read by the GPU from the index buffer.
		baseVertex: A value added to each index before reading a vertex from the vertex buffer.
		*/
		void DrawIndexed(unsigned int indexCount, unsigned int startIndex, int baseVertex);

		/*Draws an indexed, instanced mesh. A Kiwi::InstancedMesh must be bound to be rendered
		indexCountPerInstance: Number of indices read from the index buffer for each instance.
		instanceCount: Number of instances to draw.
		indexStart: The location of the first index read by the GPU from the index buffer.
		baseVertexLocation: A value added to each index before reading a vertex from the vertex buffer.
		instanceStart: A value added to each index before reading per-instance data from a vertex buffer.*/
		void DrawIndexedInstanced( unsigned int indexCountPerInstance, unsigned instanceCount, unsigned int indexStart, int baseVertexLocation, unsigned int instanceStart );

		void Present();

		/*creates a new render target with the given name and dimensions. 
		If a pre-initialized texture is passed (e.g. a back buffer) it is used and a new texture is not created*/
		//Kiwi::RenderTarget* CreateRenderTarget(std::wstring name, const Kiwi::Vector2& dimensions, ID3D11Texture2D* texture = 0);

		/*For each viewport in the render target, renders all renderqueuegroups that are assigned to it from renderQueue*/
		void Render( Kiwi::RenderQueue* renderQueue );

		HRESULT MapResource( ID3D11Resource* resource, unsigned int subResource, D3D11_MAP mapType, unsigned int flags, D3D11_MAPPED_SUBRESOURCE* mappedResource );
		void UnmapResource( ID3D11Resource* resource, unsigned int subResource );

		void CopyResource( ID3D11Resource* destination, ID3D11Resource* source );

		void UpdateSubresource( ID3D11Resource* resource, unsigned int subResource, const D3D11_BOX* destBox, const void* data, unsigned int srcRowPitch, unsigned int srcDepthPitch );

		void EnableDepthBuffer( bool enable );

		/*clears the currently active render target, including its depth buffer*/
		void ClearRenderTarget();
		/*clears the currently active render target, including its depth buffer with the given color*/
		void ClearRenderTarget(const Kiwi::Color& color);
		/*clears the passed render target, including its depth buffer*/
		void ClearRenderTarget( Kiwi::RenderTarget* rt );

		Kiwi::RasterState* CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc );

		ID3D11Buffer* CreateBuffer( const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* srData );
		ID3D11SamplerState* CreateSampler( D3D11_SAMPLER_DESC* samplerDesc );
		ID3D11InputLayout* CreateInputLayout(Kiwi::IShader* shader, D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int layoutSize );

		void SetPrimitiveTopology(Kiwi::PrimitiveTopology topology);

		void SetRenderTarget(Kiwi::RenderTarget* target);
		/*sets the back buffer to be the active render target*/
		void SetBackBufferRenderTarget();

		void SetDepthStencil( Kiwi::DepthStencil* depthStencil );

		void SetRasterState( Kiwi::RasterState* rasterState );
		void SetRasterState( std::wstring name );

		void SetBlendState( Kiwi::BlendState* blendState );

		void SetViewport( Kiwi::Viewport* viewport );

		void SetShader( Kiwi::IShader* shader );

		void SetPixelShaderResources( unsigned int startSlot, unsigned int numViews, ID3D11ShaderResourceView** resources );

		/*binds a single vertex buffer*/
		void SetVertexBuffer( unsigned int startSlot, Kiwi::IBuffer* buffer, unsigned int stride, unsigned int offset );

		/*binds an array of vertex buffers*/
		void SetVertexBuffers( unsigned int startSlot, std::vector<Kiwi::IBuffer*>& buffers, std::vector<unsigned int>& strides, std::vector<unsigned int>& offsets );

		/*binds an index buffer*/
		void SetIndexBuffer( Kiwi::IBuffer* buffer, DXGI_FORMAT format, unsigned int offset );

		void DestroyAllRasterStates();
		void DestroyRasterStateWithName( std::wstring name );

		std::wstring GetName()const { return m_name; }

		DXGI_FORMAT GetDXGIFormat()const;
		ID3D11Device* GetDevice()const;

		Kiwi::RenderTarget* GetActiveRenderTarget()const { return m_activeRenderTarget; }
		Kiwi::DepthStencil* GetActiveDepthStencil()const { return m_activeDepthStencil; }
		Kiwi::RasterState* FindRasterStateWithName( std::wstring name );
		Kiwi::MultisampleDesc GetMultisampleDesc();
		Kiwi::RenderTarget* GetBackBuffer()const { return m_backBuffer; }
		Kiwi::RenderWindow* GetRenderWindow()const { return m_renderWindow; }

	};
};

#endif