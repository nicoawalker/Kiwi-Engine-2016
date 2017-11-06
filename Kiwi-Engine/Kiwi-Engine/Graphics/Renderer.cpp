#include "Renderer.h"
#include "D3D11Interface.h"
#include "RenderWindow.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RasterStateManager.h"
#include "BlendState.h"
#include "Viewport.h"
#include "IShader.h"
#include "IBuffer.h"
#include "RenderQueue.h"
#include "RenderQueueGroup.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"
#include "../Core/Scene.h"
#include "../Core/Console.h"
#include "../Core/EngineRoot.h"

namespace Kiwi
{

	Renderer::Renderer(std::wstring name, Kiwi::RenderWindow* window)
	{

		if(window == 0)
		{
			throw Kiwi::Exception(L"Renderer", L"Invalid window");
		}

		m_d3dInterface = 0;
		m_rasterStateManager = 0;
		m_backBuffer = 0;
		m_depthEnable = true;
		m_activePrimitiveTopology = Kiwi::TRIANGLE_LIST;

		try
		{
			//create the D3D interface
			m_d3dInterface = new Kiwi::D3D11Interface(window->GetHandle(), window->IsFullscreen(), (unsigned int)window->GetClientSize().x, (unsigned int)window->GetClientSize().y, window->GetVSyncEnabled());
			
			//create a default rendertarget for the back buffer
			Kiwi::BackBuffer* bb = m_d3dInterface->GetBackBuffer();
			m_backBuffer = new Kiwi::RenderTarget( L"BackBuffer", this, bb->texture, bb->renderTargetView );

			//create the back buffer's depth stencil
			Kiwi::DepthStencil* bbDepth = new Kiwi::DepthStencil( L"BackBuffer_Depth", bb->depthBuffer, bb->depthStencilView, bb->depthStencilState, bb->depthDisabledState );
			m_backBuffer->AttachDepthStencil(bbDepth);

			//create a default viewport for the back buffer
			m_backBuffer->CreateViewport( L"BackBuffer_Viewport", 0 );

			m_activeRenderTarget = m_backBuffer;

			m_name = name;
			m_renderWindow = window;

			//create a render state manager to manage depth stencils, blend states and raster states
			m_rasterStateManager = new Kiwi::RasterStateManager( this );
		
		}catch(...)
		{
			SAFE_DELETE(m_d3dInterface);
			throw;
		}

	}

	Renderer::~Renderer()
	{
		
		m_activeRenderTarget = 0;
		m_renderWindow = 0;

		SAFE_DELETE( m_rasterStateManager );
		SAFE_DELETE( m_d3dInterface );

	}

	void Renderer::DrawIndexed( unsigned int indexCount, unsigned int startIndex, int baseVertex )
	{

		assert( m_d3dInterface != 0 );

		m_d3dInterface->DrawIndexed( indexCount, startIndex, baseVertex );

	}

	void Renderer::DrawIndexedInstanced( unsigned int indexCountPerInstance, unsigned instanceCount, unsigned int indexStart, int baseVertexLocation, unsigned int instanceStart )
	{

		assert( m_d3dInterface != 0 );

		m_d3dInterface->DrawIndexedInstanced( indexCountPerInstance, instanceCount, indexStart, baseVertexLocation, instanceStart );

	}

	void Renderer::Present()
	{

		assert( m_d3dInterface != 0 && m_renderWindow != 0 );

		m_d3dInterface->Present( m_renderWindow->GetVSyncEnabled() );

	}

	void Renderer::Render( Kiwi::RenderQueue* renderQueue )
	{

		Kiwi::RenderTarget* renderTarget = this->GetActiveRenderTarget();

		if( renderTarget == 0 || renderQueue == 0 )
		{
			return;
		}

		Kiwi::Scene* scene = renderQueue->GetScene();
		if( !scene )
		{
			return;
		}

		Kiwi::Console* console = scene->GetEngine()->GetConsole();
		if( !console )
		{
			return;
		}

		this->EnableDepthBuffer( true );
		this->SetRasterState( L"Cull CCW" );

		for( unsigned int i = 0; i < renderTarget->GetViewportCount(); i++ )
		{
			Kiwi::Viewport* vp = renderTarget->GetViewport( i );

			std::vector<std::wstring> renderGroups;
			if( vp->UsingDefaultRenderGroup() )
			{
				renderGroups.push_back( renderQueue->GetDefaultRenderGroupName() );

			} else
			{
				renderGroups = vp->GetRenderGroupList();
			}

			for( auto GroupName : renderGroups )
			{
				Kiwi::RenderQueueGroup* rcq = renderQueue->GetRenderGroup( GroupName );

				if( rcq )
				{
					//sort the renderables in the render group based on the position of the camera in this viewport
					rcq->Sort( *vp );

					Kiwi::IShader* currentShader = 0;
					Kiwi::Mesh* currentMesh = 0;

					//first render all of the solid 3D renderables
					auto solidItr = rcq->BeginSolids();
					for( ; solidItr != rcq->EndSolids(); solidItr++ )
					{
						if( currentMesh != (*solidItr) )
						{
							currentMesh = (*solidItr);
							if( currentMesh == 0 )
							{
								console->PrintDebug( L"Attempted to render null mesh in render group " + rcq->GetName() );
								continue;
							}
							try
							{
								//bind will throw if the mesh buffers have not been initialized
								currentMesh->Bind( *this );

								if( currentMesh->GetPrimitiveTopology() != m_activePrimitiveTopology )
								{
									this->SetPrimitiveTopology( currentMesh->GetPrimitiveTopology() );
								}

							} catch( Kiwi::Exception& e )
							{
								//cant render this mesh, move on to the next one
								console->PrintDebug( L"Failed to bind mesh '" + currentMesh->GetName() + L"'" );
								continue;
							}
						}

						//mesh is bound
						for( unsigned int i = 0; i < currentMesh->GetSubmeshCount(); i++ )
						{
							//get the current subset
							Kiwi::Mesh::Submesh* subset = currentMesh->GetSubmesh( i );
							if( !subset )
							{
								console->PrintDebug( L"Attempted to render null submesh in mesh " + currentMesh->GetName() );
								continue;
							}

							//bind the material's shader
							std::wstring matShaderName = subset->material.GetShader();
							if( matShaderName.compare( L"" ) == 0 )
							{
								matShaderName = L"default";
							}
							if( currentShader == 0 || currentShader->GetName().compare( matShaderName ) != 0 )
							{
								currentShader = scene->FindAsset<Kiwi::IShader>( matShaderName );
								if( currentShader == 0 )
								{
									console->PrintDebug( L"Mesh " + currentMesh->GetName() + L" contains material with invalid shader" );
									continue;
								}
								this->SetShader( currentShader );
								currentShader->SetFrameParameters( scene );
							}

							//find the size of the subset
							unsigned int subsetSize = (subset->endIndex - subset->startIndex) + 1;

							//set the renderable's shader parameters
							currentShader->SetObjectParameters( scene, this->GetActiveRenderTarget(), subset );

							if( currentMesh->IsInstanced() )
							{
								//draw all of the instances of the mesh
								this->DrawIndexedInstanced( subsetSize, currentMesh->GetInstanceCount(), subset->startIndex, 0, 0 );

							} else
							{
								//render the mesh without instancing
								this->DrawIndexed( subsetSize, subset->startIndex, 0 );
							}
						}

					}

					//now the transparent 3d renderables
					auto transItr = rcq->BeginTransparents();
					for( ; transItr != rcq->EndTransparents(); transItr++ )
					{

						currentMesh = (*transItr);
						if( currentMesh == 0 )
						{
							console->PrintDebug( L"Attempted to render null mesh in render group " + rcq->GetName() );
							continue;
						}
						try
						{
							//bind will throw if the mesh buffers have not been initialized
							currentMesh->Bind( *this );

							if( currentMesh->GetPrimitiveTopology() != m_activePrimitiveTopology )
							{
								this->SetPrimitiveTopology( currentMesh->GetPrimitiveTopology() );
							}

						} catch( Kiwi::Exception& e )
						{
							//cant render this mesh, move on to the next one
							console->PrintDebug( L"Failed to bind mesh '" + currentMesh->GetName() + L"'" );
							continue;
						}

						//mesh is bound, now render each subset of the mesh
						for( unsigned int i = 0; i < currentMesh->GetSubmeshCount(); i++ )
						{
							//get the current subset
							Kiwi::Mesh::Submesh* subset = currentMesh->GetSubmesh( i );

							if( !subset )
							{
								console->PrintDebug( L"Attempted to render null submesh in mesh " + (*solidItr)->GetName() );
								continue;
							}

							//bind the material's shader
							std::wstring matShaderName = subset->material.GetShader();
							if( matShaderName.compare( L"" ) == 0 )
							{
								matShaderName = L"default";
							}
							if( currentShader == 0 || currentShader->GetName().compare( matShaderName ) != 0 )
							{
								currentShader = scene->FindAsset<Kiwi::IShader>( matShaderName );
								if( currentShader == 0 )
								{
									console->PrintDebug( L"Mesh " + currentMesh->GetName() + L" contains material with invalid shader" );
									continue;
								}
								this->SetShader( currentShader );
								currentShader->SetFrameParameters( scene );
							}

							//find the size of the subset
							unsigned int subsetSize = (subset->endIndex - subset->startIndex) + 1;

							//set the renderable's shader parameters
							currentShader->SetObjectParameters( scene, this->GetActiveRenderTarget(), subset );

							if( currentMesh->IsInstanced() )
							{
								//draw all of the instances of the mesh
								this->SetRasterState( L"Cull CW" );
								this->DrawIndexedInstanced( subsetSize, currentMesh->GetInstanceCount(), subset->startIndex, 0, 0 );
								this->SetRasterState( L"Cull CCW" );
								this->DrawIndexedInstanced( subsetSize, currentMesh->GetInstanceCount(), subset->startIndex, 0, 0 );

							} else
							{
								//now draw the renderable to the render target twice, once for the back faces
								//and then again for the front faces
								this->SetRasterState( L"Cull CW" );
								this->DrawIndexed( subsetSize, subset->startIndex, 0 );
								this->SetRasterState( L"Cull CCW" );
								this->DrawIndexed( subsetSize, subset->startIndex, 0 );
							}

						}

					}

					this->EnableDepthBuffer( false );

					//then render all of the 2D renderables
					auto itr2D = rcq->Begin2D();
					for( ; itr2D != rcq->End2D(); itr2D++ )
					{
						currentMesh = (*itr2D);
						if( currentMesh == 0 )
						{
							console->PrintDebug( L"Attempted to render null mesh in render group " + rcq->GetName() );
							continue;
						}
						try
						{
							//bind will throw if the mesh buffers have not been initialized
							currentMesh->Bind( *this );

							if( currentMesh->GetPrimitiveTopology() != m_activePrimitiveTopology )
							{
								this->SetPrimitiveTopology( currentMesh->GetPrimitiveTopology() );
							}

						} catch( Kiwi::Exception& e )
						{
							//cant render this mesh, move on to the next one
							console->PrintDebug( L"Failed to bind mesh '" + currentMesh->GetName() + L"'" );
							continue;
						}

						//mesh is bound, now render each subset of the mesh
						for( unsigned int i = 0; i < currentMesh->GetSubmeshCount(); i++ )
						{
							//get the current subset
							Kiwi::Mesh::Submesh* subset = currentMesh->GetSubmesh( i );
							if( !subset )
							{
								console->PrintDebug( L"Attempted to render null submesh in mesh " + currentMesh->GetName() );
								continue;
							}

							//bind the material's shader
							std::wstring matShaderName = subset->material.GetShader();
							if( matShaderName.compare( L"" ) == 0 )
							{
								matShaderName = L"default";
							}
							if( currentShader == 0 || currentShader->GetName().compare( matShaderName ) != 0 )
							{
								currentShader = scene->FindAsset<Kiwi::IShader>( matShaderName );
								if( currentShader == 0 )
								{
									console->PrintDebug( L"Mesh " + currentMesh->GetName() + L" contains material with invalid shader" );
									continue;
								}
								this->SetShader( currentShader );
								currentShader->SetFrameParameters( scene );
							}

							//find the size of the subset
							unsigned int subsetSize = (subset->endIndex - subset->startIndex) + 1;

							//set the renderable's shader parameters
							currentShader->SetObjectParameters( scene, this->GetActiveRenderTarget(), subset );

							if( currentMesh->IsInstanced() )
							{
								//draw all of the instances of the mesh
								this->DrawIndexedInstanced( subsetSize, currentMesh->GetInstanceCount(), subset->startIndex, 0, 0 );

							} else
							{
								//render the mesh without instancing
								this->DrawIndexed( subsetSize, subset->startIndex, 0 );
							}
						}

					}

					this->EnableDepthBuffer( true );
				}
			}

		}

	}

	HRESULT Renderer::MapResource( ID3D11Resource* resource, unsigned int subResource, D3D11_MAP mapType, unsigned int flags, D3D11_MAPPED_SUBRESOURCE* mappedResource )
	{

		assert( m_d3dInterface != 0 );

		return m_d3dInterface->MapResource( resource, subResource, mapType, flags, mappedResource );

	}

	void Renderer::UnmapResource( ID3D11Resource* resource, unsigned int subResource )
	{

		assert( m_d3dInterface != 0 );

		m_d3dInterface->UnmapResource( resource, subResource );

	}

	void Renderer::CopyResource( ID3D11Resource* destination, ID3D11Resource* source )
	{

		assert( m_d3dInterface != 0 );

		m_d3dInterface->CopyResource( destination, source );

	}

	void Renderer::UpdateSubresource( ID3D11Resource* resource, unsigned int subResource, const D3D11_BOX* destBox, const void* data, unsigned int srcRowPitch, unsigned int srcDepthPitch )
	{

		assert( m_d3dInterface != 0 );

		m_d3dInterface->UpdateSubresource( resource, subResource, destBox, data, srcRowPitch, srcDepthPitch );

	}

	//Kiwi::RenderTarget* Renderer::CreateRenderTarget(std::wstring name, const Kiwi::Vector2& dimensions, ID3D11Texture2D* tex)
	//{

	//	assert( m_d3dInterface != 0 );

	//	try
	//	{

	//		ID3D11Texture2D* texture = tex;
	//		ID3D11RenderTargetView* view = 0;
	//		ID3D11ShaderResourceView* shaderRes = 0;

	//		D3D11_TEXTURE2D_DESC textureDesc;
	//		D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
	//		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	//		HRESULT hr;

	//		ZeroMemory(&textureDesc, sizeof(textureDesc));

	//		if(texture == 0)
	//		{
	//			//create a new texture
	//			textureDesc.Width = (unsigned int)dimensions.x;
	//			textureDesc.Height = (unsigned int)dimensions.y;
	//			textureDesc.MipLevels = 1;
	//			textureDesc.ArraySize = 1;
	//			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//			textureDesc.SampleDesc.Count = 1;
	//			textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//			textureDesc.CPUAccessFlags = 0;
	//			textureDesc.MiscFlags = 0;

	//			hr = m_d3dInterface->GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
	//			if(FAILED(hr))
	//			{
	//				throw Kiwi::Exception(L"D3D11Interface::CreateRenderTarget", L"Failed to create render target texture");
	//			}
	//		}else
	//		{
	//			//a pre-existing texture was passed in (e.g. back buffer) so just use it
	//			texture->GetDesc(&textureDesc);
	//		}

	//		rtViewDesc.Format = textureDesc.Format;
	//		rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//		rtViewDesc.Texture2D.MipSlice = 0;

	//		hr = m_d3dInterface->GetDevice()->CreateRenderTargetView(texture, &rtViewDesc, &view);
	//		if(FAILED(hr))
	//		{
	//			SAFE_RELEASE(texture);

	//			throw Kiwi::Exception(L"D3D11Interface::CreateRenderTarget", L"Failed to create render target view");
	//		}

	//		shaderResourceViewDesc.Format = textureDesc.Format;
	//		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	//		shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//		hr = m_d3dInterface->GetDevice()->CreateShaderResourceView(texture, &shaderResourceViewDesc, &shaderRes);
	//		if(FAILED(hr))
	//		{
	//			SAFE_RELEASE(texture);
	//			SAFE_RELEASE(view);

	//			throw Kiwi::Exception(L"D3D11Interface::CreateRenderTarget", L"Failed to create render target shader resource");
	//		}

	//		Kiwi::RenderTarget* newRT = new Kiwi::RenderTarget(name, texture, view, shaderRes);

	//		return newRT;

	//	}catch(...)
	//	{
	//		throw;
	//	}

	//}

	void Renderer::EnableDepthBuffer( bool enable )
	{

		if( enable != m_depthEnable )
		{
			m_depthEnable = enable;

			this->SetDepthStencil( m_activeDepthStencil );
		}

	}

	void Renderer::ClearRenderTarget()
	{

		this->ClearRenderTarget( m_activeRenderTarget->GetClearColor() );

	}

	/*clears the currently active render target, including its depth stencil*/
	void Renderer::ClearRenderTarget(const Kiwi::Color& color)
	{

		assert( m_d3dInterface != 0 );

		if(m_activeRenderTarget == 0) return;

		float col[4] = { (float)color.red, (float)color.green, (float)color.blue, (float)color.alpha };

		m_d3dInterface->ClearRenderTargetView(m_activeRenderTarget->GetView(), col);

		Kiwi::DepthStencil* stencil = m_activeRenderTarget->GetDepthStencil();
		if(stencil != 0 && stencil->GetD3DView() != 0 )
		{
			m_d3dInterface->ClearDepthStencilView(stencil->GetD3DView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

	}

	/*clears the passed render target as well as its depth stencil*/
	void Renderer::ClearRenderTarget( Kiwi::RenderTarget* rt )
	{

		assert( m_d3dInterface != 0 );

		if(rt == 0) return;

		Kiwi::Color color = rt->GetClearColor();
		float col[4] = { (float)color.red, (float)color.green, (float)color.blue, (float)color.alpha };

		m_d3dInterface->ClearRenderTargetView(rt->GetView(), col);

		Kiwi::DepthStencil* stencil = rt->GetDepthStencil();
		if(stencil != 0 && stencil->GetD3DView() != 0 )
		{
			m_d3dInterface->ClearDepthStencilView( stencil->GetD3DView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
		}

	}

	Kiwi::RasterState* Renderer::CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc )
	{

		assert( m_rasterStateManager != 0 );

		return m_rasterStateManager->CreateRasterState( name, desc );

	}

	ID3D11Buffer* Renderer::CreateBuffer( const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* srData )
	{

		assert( m_d3dInterface != 0 );

		if( bufferDesc )
		{
			ID3D11Buffer* buffer = 0;

			HRESULT hr = m_d3dInterface->CreateBuffer( bufferDesc, srData, &buffer );
			if( FAILED(hr) )
			{
				throw Kiwi::Exception( L"Renderer::CreateBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}

			return buffer;
		}

		return 0;

	}

	ID3D11SamplerState* Renderer::CreateSampler( D3D11_SAMPLER_DESC* samplerDesc )
	{

		assert( m_d3dInterface != 0 );

		if( samplerDesc )
		{
			ID3D11SamplerState* sampler = 0;

			HRESULT hr = m_d3dInterface->CreateSampler( samplerDesc, &sampler );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"Renderer::CreateSampler", L"Failed to create sampler: " + Kiwi::GetD3DErrorString( hr ) );
			}

			return sampler;
		}

		return 0;

	}

	ID3D11InputLayout* Renderer::CreateInputLayout( Kiwi::IShader* shader, D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int layoutSize )
	{

		assert( m_d3dInterface != 0 );

		if( shader && shader->m_vertexBlob && polygonLayout )
		{
			ID3D11InputLayout* layout = 0;
			HRESULT hr = m_d3dInterface->CreateInputLayout( shader->m_vertexBlob, polygonLayout, layoutSize, &layout );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"Renderer::CreateInputLayout", L"Failed to create input layout: " + Kiwi::GetD3DErrorString( hr ) );
			}

			return layout;
		}

		return 0;

	}

	void Renderer::SetPrimitiveTopology(Kiwi::PrimitiveTopology topology)
	{

		assert( m_d3dInterface != 0 );

		try
		{
			m_d3dInterface->SetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)topology);
			m_activePrimitiveTopology = topology;

		}catch(...)
		{
			throw;
		}

	}

	void Renderer::SetRenderTarget(Kiwi::RenderTarget* renderTarget)
	{

		assert( m_d3dInterface != 0 );

		if(renderTarget)
		{

			m_d3dInterface->SetRenderTarget( renderTarget->GetView(), NULL );

			Kiwi::DepthStencil* depthState = renderTarget->GetDepthStencil();
			Kiwi::BlendState* blendState = renderTarget->GetBlendState();

			this->SetDepthStencil( depthState );

			if( depthState != 0 )
			{
				
				m_d3dInterface->SetRenderTarget( renderTarget->GetView(), depthState->GetD3DView() );

			} else
			{
				m_d3dInterface->SetRenderTarget( renderTarget->GetView(), NULL );
			}

			this->SetBlendState( blendState );

			unsigned int viewportCount = renderTarget->GetViewportCount();
			D3D11_VIEWPORT* viewports = new D3D11_VIEWPORT[viewportCount];
			for( unsigned int i = 0; i < viewportCount; i++ )
			{
				Kiwi::Viewport* vp = renderTarget->GetViewport(i);
				if( vp )
				{
					viewports[i] = vp->GetD3DViewport();
				}
			}
			m_d3dInterface->SetViewports( viewports, viewportCount );

			m_activeRenderTarget = renderTarget;
		}

	}

	void Renderer::SetBackBufferRenderTarget()
	{

		assert( m_d3dInterface != 0 );

		this->SetRenderTarget( m_backBuffer );

	}

	void Renderer::SetDepthStencil( Kiwi::DepthStencil* depthStencil )
	{

		assert( m_d3dInterface != 0 );

		if( depthStencil )
		{
			if( m_depthEnable )
			{
				m_d3dInterface->SetDepthStencilState( depthStencil->GetD3DState() );

			} else
			{
				m_d3dInterface->SetDepthStencilState( depthStencil->GetD3DDisabledState() );
			}
			//m_d3dInterface->SetRenderTarget( m_activeRenderTarget->GetView(), depthStencil->GetD3DView() );

		} else
		{
			m_d3dInterface->SetDepthStencilState( 0 );
			//m_d3dInterface->SetRenderTarget( m_activeRenderTarget->GetView(), 0 );
		}

		if( m_depthEnable )
		{
			m_activeDepthStencil = depthStencil;
		}

	}

	void Renderer::SetRasterState( Kiwi::RasterState* rasterState )
	{

		if( rasterState )
		{
			m_d3dInterface->SetRasterState( rasterState->GetD3DRasterState() );
		}

	}

	void Renderer::SetRasterState( std::wstring name )
	{

		assert( m_rasterStateManager != 0 );

		this->SetRasterState( m_rasterStateManager->FindRasterStateWithName( name ) );

	}

	void Renderer::SetBlendState( Kiwi::BlendState* blendState )
	{

		if( blendState )
		{
			m_d3dInterface->SetBlendState( blendState->GetD3DBlendState(), blendState->GetBlendFactor(), blendState->GetSampleMask() );

		} else
		{
			//turn off blending if 0 is passed
			m_d3dInterface->SetBlendState();
		}

	}

	void Renderer::SetViewport( Kiwi::Viewport* viewport )
	{

		if( viewport )
		{
			D3D11_VIEWPORT viewports[1] = { viewport->GetD3DViewport() };

			m_d3dInterface->SetViewports( viewports, 1 );
		}

	}

	void Renderer::SetShader( Kiwi::IShader* shader )
	{

		assert( m_d3dInterface != 0 );

		if( shader )
		{

			m_d3dInterface->SetVertexShader( shader->m_vertexShader, 0, 0 );
			m_d3dInterface->SetPixelShader( shader->m_pixelShader, 0, 0 );
			m_d3dInterface->SetInputLayout( shader->m_inputLayout );

			if( shader->m_vBuffers.size() > 0 )
			{
				m_d3dInterface->SetVertexShaderCBuffers( 0, (unsigned int)shader->m_vBuffers.size(), &shader->m_vBuffers[0] );
			}
			if( shader->m_pBuffers.size() > 0 )
			{
				m_d3dInterface->SetPixelShaderCBuffers( 0, (unsigned int)shader->m_pBuffers.size(), &shader->m_pBuffers[0] );
			}
			if( shader->m_samplerStates.size() > 0 )
			{
				m_d3dInterface->SetShaderSamplers( 0, (unsigned int)shader->m_samplerStates.size(), &shader->m_samplerStates[0] );
			}

		} else
		{
			//unbind shaders
			m_d3dInterface->SetVertexShader( 0, 0, 0 );
			m_d3dInterface->SetPixelShader( 0, 0, 0 );
		}

	}

	void Renderer::SetPixelShaderResources( unsigned int startSlot, unsigned int numViews, ID3D11ShaderResourceView** resources )
	{

		assert( m_d3dInterface != 0 );

		if( resources )
		{
			m_d3dInterface->SetPixelShaderResources( startSlot, numViews, resources );
		}

	}

	/*binds a single vertex buffer*/
	void Renderer::SetVertexBuffer( unsigned int startSlot, Kiwi::IBuffer* buffer, unsigned int stride, unsigned int offset )
	{

		assert( m_d3dInterface != 0 );

		if( buffer )
		{
			ID3D11Buffer* buf = buffer->GetD3DBuffer();
			m_d3dInterface->SetVertexBuffers( startSlot, 1, &buf, &stride, &offset );
		}

	}

	/*binds an array of vertex buffers*/
	void Renderer::SetVertexBuffers( unsigned int startSlot, std::vector<Kiwi::IBuffer*>& buffers, std::vector<unsigned int>& strides, std::vector<unsigned int>& offsets )
	{

		assert( m_d3dInterface != 0 );

		if( buffers.size() > 0 )
		{
			ID3D11Buffer** buf = new ID3D11Buffer*[buffers.size()];
			for( unsigned int i = 0; i < buffers.size(); i++ )
			{
				buf[i] = buffers[i]->GetD3DBuffer();
			}
			m_d3dInterface->SetVertexBuffers( startSlot, (unsigned int)buffers.size(), buf, &strides[0], &offsets[0] );
		}

	}

	/*binds an index buffer*/
	void Renderer::SetIndexBuffer( Kiwi::IBuffer* buffer, DXGI_FORMAT format, unsigned int offset )
	{

		assert( m_d3dInterface != 0 );

		if( buffer )
		{
			m_d3dInterface->SetIndexBuffer( buffer->GetD3DBuffer(), format, offset );
		}

	}

	void Renderer::DestroyAllRasterStates()
	{

		assert( m_rasterStateManager != 0 );

		m_rasterStateManager->DestroyAllRasterStates();

	}

	void Renderer::DestroyRasterStateWithName( std::wstring name )
	{

		assert( m_rasterStateManager != 0 );

		m_rasterStateManager->DestroyRasterStateWithName( name );

	}

	DXGI_FORMAT Renderer::GetDXGIFormat()const
	{

		assert( m_d3dInterface != 0 );

		return m_d3dInterface->GetDXGIFormat();

	}

	Kiwi::MultisampleDesc Renderer::GetMultisampleDesc()
	{

		assert( m_d3dInterface != 0 );

		return Kiwi::MultisampleDesc{ m_d3dInterface->GetMultisampleCount(), m_d3dInterface->GetMultisampleQuality() };

	}

	ID3D11Device* Renderer::GetDevice()const
	{
		
		assert( m_d3dInterface != 0 );
		
		return m_d3dInterface->GetDevice();

		return 0;

	}

	Kiwi::RasterState* Renderer::FindRasterStateWithName( std::wstring name )
	{

		assert( m_rasterStateManager != 0 );

		return m_rasterStateManager->FindRasterStateWithName( name );

	}

};