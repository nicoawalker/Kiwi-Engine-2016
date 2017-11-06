#include "SceneLoader.h"

#include "Scene.h"
#include "Utilities.h"
#include "IAsset.h"
#include "EngineRoot.h"
#include "Console.h"

#include "../Graphics/D3DAssetLoader.h"
#include "../Graphics/Texture.h"
#include "../Graphics/DirectX.h"
#include "../Graphics/OBJImporter.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Font.h"
#include "../Graphics/StaticMeshAsset.h"

#include "DDSTextureLoader.h"

#include <future>
#include <fstream>

namespace Kiwi
{

	SceneLoader::SceneLoader( Kiwi::Scene* scene )
	{

		if( !scene ) 
		{ 
			throw Kiwi::Exception( L"SceneLoader", L"Invalid scene" ); 
		}

		m_scene = scene;

		m_totalSizeInBytes = 0;
		m_numAssetsLoaded = 0;
		m_loadedSizeInBytes = 0;
		m_isLoading = false;

	}

	SceneLoader::~SceneLoader()
	{

		m_scene = 0;

	}

	Kiwi::IThreadParam* SceneLoader::_Load_Async( Kiwi::IThreadParam* param )
	{

		if( param != 0 )
		{
			AssetDesc* desc = dynamic_cast<AssetDesc*>(param);
			if( desc != 0 )
			{
				if( desc->assetType.compare( L"texture" ) == 0 )
				{

					TextureDesc* tDesc = dynamic_cast<TextureDesc*>(desc);
					if( desc != 0 )
					{
						this->_LoadTexture( tDesc );

					} else
					{
						throw Kiwi::Exception( L"SceneLoader::_Load_Async", L"Loading failed! Invalid IThreadParam (assetType is incorrect)" );
					}

				}else if( desc->assetType.compare( L"staticmesh" ) == 0 )
				{
					StaticMeshDesc* mDesc = dynamic_cast<StaticMeshDesc*>(desc);
					if( desc != 0 )
					{
						this->_LoadStaticMesh( mDesc );

					} else
					{
						throw Kiwi::Exception( L"SceneLoader::_Load_Async", L"Loading failed! Invalid IThreadParam (assetType is incorrect)" );
					}

				}else if( desc->assetType.compare( L"font" ) == 0 )
				{
					FontDesc* fDesc = dynamic_cast<FontDesc*>(desc);
					if( desc != 0 )
					{
						this->_LoadFont( fDesc );

					} else
					{
						throw Kiwi::Exception( L"SceneLoader::_Load_Async", L"Loading failed! Invalid IThreadParam (assetType is incorrect)" );
					}
				}

			} else
			{
				throw Kiwi::Exception( L"SceneLoader::_Load_Async", L"Loading failed! Invalid IThreadParam (cast to AssetDesc failed)" );
			}

		} else
		{
			throw Kiwi::Exception( L"SceneLoader::_Load_Async", L"Loading failed! Invalid IThreadParam (null)" );
		}

		return 0;

	}

	Kiwi::Texture* SceneLoader::_LoadTexture( TextureDesc* textureDesc )
	{

		assert( m_scene != 0 && m_scene->GetRenderer() != 0 && m_scene->GetRenderer()->GetDevice() != 0 );
		if( textureDesc == 0 || m_scene == 0 || m_scene->GetRenderer() == 0 || m_scene->GetRenderer()->GetDevice() == 0 )
		{
			return 0;
		}

		Kiwi::Texture* texture = 0;

		ID3D11ShaderResourceView* newShaderRes = 0;
		ID3D11Resource* resource = 0;
		ID3D11Texture2D* d3dTexture = 0;
		DirectX::DDS_ALPHA_MODE alphaMode;

		//d3d device in directx11 is thread safe, can use it here to create the texture
		ID3D11Device* device = m_scene->GetRenderer()->GetDevice();

		//HRESULT hr = D3DX11CreateShaderResourceViewFromFileW( device, textureDesc->textureFile.c_str(), NULL, NULL, &newShaderRes, NULL );
		HRESULT hr = DirectX::CreateDDSTextureFromFile( device, textureDesc->textureFile.c_str(), &resource, &newShaderRes, 0, &alphaMode );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"D3DAssetLoader::LoadTexture", L"Failed to create shader resource view for texture '" + textureDesc->assetName + L"'" );
		}

		D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
		resource->GetType( &resType );

		if( resType == D3D11_RESOURCE_DIMENSION_TEXTURE2D )
		{
			hr = resource->QueryInterface( IID_ID3D11Texture2D, (void**)&d3dTexture );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"Failed to convert d3d resource into d3d texture", L"A", MB_OK );
			} 
			/*else
			{
				D3D11_TEXTURE2D_DESC texDesc;
				d3dTexture->GetDesc( &texDesc );

				texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				texDesc.Usage = D3D11_USAGE_STAGING;
				texDesc.SampleDesc.Quality = 0;

				ID3D11Texture2D* stagingTex = 0;
				hr = device->CreateTexture2D( &texDesc, NULL, &stagingTex );
				if( FAILED( hr ) )
				{
					MessageBox( NULL, L"Failed to create staging texture, cannot determine texture alpha presence", L"A", MB_OK );
				} else
				{

					m_scene->GetRenderer()->CopyResource( stagingTex, d3dTexture );



				}
			}*/
		}

		texture = new Kiwi::Texture( textureDesc->assetName, textureDesc->textureFile, d3dTexture, newShaderRes, textureDesc->hasTransparency );

		if( texture != 0 )
		{
			m_byteMutex.lock();
				m_loadedSizeInBytes += sizeof( Kiwi::Texture ) + sizeof(ID3D11ShaderResourceView);
			m_byteMutex.unlock();
		}

		m_scene->AddAsset( texture );

		SAFE_DELETE( textureDesc );

		return texture;

	}

	Kiwi::StaticMeshAsset* SceneLoader::_LoadStaticMesh( StaticMeshDesc* meshDesc )
	{

		assert( m_scene != 0 && m_scene->GetRenderer() != 0 && m_scene->GetRenderer()->GetDevice() != 0 );
		if( meshDesc == 0 || m_scene == 0 || m_scene->GetRenderer() == 0 || m_scene->GetRenderer()->GetDevice() == 0 )
		{
			return 0;
		}

		try
		{
			std::wstring name = meshDesc->assetName;
			std::wstring objFile = meshDesc->objFile;

			Kiwi::OBJImporter objImporter;
			OBJMeshData* objMesh = 0;

			//import the raw data from the OBJ File
			objMesh = objImporter.Import( objFile );

			std::vector<Kiwi::Mesh::Submesh> subsets( objMesh->vGroups.size() );

			std::vector<Kiwi::Vector3d> vertices;
			std::vector<Kiwi::Vector2d> uvs;
			std::vector<Kiwi::Vector3d> normals;

			//for each materialdata in the mesh, create a material and load any textures
			//then place these in a MeshSubset, along with the vertices to load into the mesh
			unsigned int index = 0;
			for( unsigned int i = 0; i < objMesh->vGroups.size(); i++ )
			{

				OBJVertexGroup* data = &objMesh->vGroups[i];

				Kiwi::Texture* ambientMap = 0;
				Kiwi::Texture* diffuseMap = 0;
				Kiwi::Texture* specularMap = 0;
				Kiwi::Texture* bumpMap = 0;

				//any textures that aren't yet loaded, load them here
				if( data->mData.ambientMap.size() > 0 )
				{

					std::wstring filename = data->mData.ambientMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::wstring::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"/Ambient";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
					ambientMap = m_scene->FindAsset<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( ambientMap == 0 )
					{
						TextureDesc* textureDesc = new TextureDesc( texName, filename, false );
						if( textureDesc != 0 )
						{
							if( this->_LoadTexture( textureDesc ) == 0 )
							{
								//failed to load
							}
						}
					}
				}

				if( data->mData.diffuseMap.size() > 0 )
				{

					std::wstring filename = data->mData.diffuseMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" );
					if( pos != std::string::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::string::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) - (pos + 1) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"/Diffuse";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
					diffuseMap = m_scene->FindAsset<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( diffuseMap == 0 )
					{
						TextureDesc* textureDesc = new TextureDesc( texName, filename, false );
						if( textureDesc != 0 )
						{
							if( this->_LoadTexture( textureDesc ) == 0 )
							{
								//failed to load
							}
						}
					}
				}

				if( data->mData.specularMap.size() > 0 )
				{

					std::wstring filename = data->mData.specularMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::wstring::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"/Specular";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
					specularMap = m_scene->FindAsset<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( specularMap == 0 )
					{
						TextureDesc* textureDesc = new TextureDesc( texName, filename, false );
						if( textureDesc != 0 )
						{
							if( this->_LoadTexture( textureDesc ) == 0 )
							{
								//failed to load
							}
						}
					}
				}

				if( data->mData.bumpMap.size() > 0 )
				{

					std::wstring filename = data->mData.bumpMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::wstring::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"/Bump";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
					bumpMap = m_scene->FindAsset<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( bumpMap == 0 )
					{
						TextureDesc* textureDesc = new TextureDesc( texName, filename, false );
						if( textureDesc != 0 )
						{
							if( this->_LoadTexture( textureDesc ) == 0 )
							{
								//failed to load
							}
						}
					}
				}

				//now create the material
				Kiwi::Material newMaterial( diffuseMap, bumpMap, ambientMap, specularMap );
				newMaterial.SetColor( L"Diffuse", data->mData.diffuseColor );
				newMaterial.SetColor( L"Ambient", data->mData.ambientColor );
				newMaterial.SetColor( L"Specular", data->mData.specularColor );
				newMaterial.SetColor( L"Emissive", data->mData.emissiveColor );
				newMaterial.SetColor( L"Transmission", data->mData.transmissionFilter );
				newMaterial.SetIllumination( data->mData.illum );
				newMaterial.SetOpticalDensity( data->mData.opticalDensity );
				newMaterial.SetReflectivity( data->mData.reflectivity );

				subsets[i].material = newMaterial;
				subsets[i].startIndex = index;
				subsets[i].endIndex = index + (unsigned int)data->vertices.size() - 1;

				index += (unsigned int)data->vertices.size();

				//convert the raw vertex data into MeshVertex structs for the mesh
				for( unsigned int a = 0; a < data->vertices.size(); a++ )
				{
					vertices.push_back( Kiwi::Vector3d( data->vertices[a].position.x, data->vertices[a].position.y, data->vertices[a].position.z ) );
					uvs.push_back( Kiwi::Vector2d( data->vertices[a].textureUV.x, data->vertices[a].textureUV.y ) );
					normals.push_back( Kiwi::Vector3d( data->vertices[a].normals.x, data->vertices[a].normals.y, data->vertices[a].normals.z ) );
				}

			}

			SAFE_DELETE( objMesh );

			//the subsets, vertices, and materials are loaded and created, now create the mesh
			std::unique_lock<std::mutex> sLock( *m_scene->m_mutex );
			Kiwi::Renderer* renderer = m_scene->GetRenderer();
			sLock.unlock();

			Kiwi::StaticMeshAsset* meshAsset = new Kiwi::StaticMeshAsset( name, subsets, vertices, uvs, normals );
			meshAsset->AddAssetFile( objFile );

			Kiwi::FreeMemory( subsets );

			m_byteMutex.lock();
			m_loadedSizeInBytes += sizeof( meshAsset );
			m_byteMutex.unlock();

			m_scene->AddAsset( meshAsset );

			SAFE_DELETE( meshDesc );

			return meshAsset;

		} catch( ... )
		{
			SAFE_DELETE( meshDesc );
			throw;
		}

	}

	Kiwi::Font* SceneLoader::_LoadFont( FontDesc* fontDesc )
	{

		assert( m_scene != 0 && m_scene->GetRenderer() != 0 && m_scene->GetRenderer()->GetDevice() != 0 );
		if( fontDesc == 0 || m_scene == 0 )
		{
			return 0;
		}

		std::wifstream fin;

		std::vector<Kiwi::Font::Character> characterSet;
		std::wstring fontFile = fontDesc->fontFile;
		std::wstring temp;
		std::wstring name;
		std::wstring line;
		std::wstring textureName;
		std::wstring textureFilename;
		std::wstring directory;
		int size = 0;
		bool bold = false;
		bool italic = false;
		bool unicode = false;
		bool smoothing = false;
		int outlineThickness = 0;
		int base = 0;
		int numPages = 0;
		int charCount = 0;
		int imageWidth = 0, imageHeight = 0;
		int cellWidth = 0, cellHeight = 0;
		int fontHeight = 0;

		directory = fontFile.substr( 0, fontFile.find_last_of( L"/" ) + 1 );
		if( directory.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Invalid/unspecified directory in font file '" + fontFile + L"'" );
		}

		fin.open( fontFile );
		if( fin.fail() )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Failed to open font file '" + fontFile + L"'" );
		}

		//process line 1
		std::getline( fin, line );

		temp = line.substr( line.find( L"face=\"" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L"\"" ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Typeface not found in font file '" + fontFile + L"'" );
		}
		name = temp;

		temp = line.substr( line.find( L"size=" ) + 5, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Font size not found in font file '" + fontFile + L"'" );
		}
		size = stoi( temp );

		temp = line.substr( line.find( L"bold=" ) + 5, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Bold value not found in font file '" + fontFile + L"'" );
		}
		bold = (bool)stoi( temp );

		temp = line.substr( line.find( L"italic=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Italic value not found in font file '" + fontFile + L"'" );
		}
		italic = (bool)stoi( temp );

		temp = line.substr( line.find( L"unicode=" ) + 8, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Unicode value not found in font file '" + fontFile + L"'" );
		}
		unicode = (bool)stoi( temp );

		temp = line.substr( line.find( L"smooth=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Smoothing value not found in font file '" + fontFile + L"'" );
		}
		smoothing = (bool)stoi( temp );

		temp = line.substr( line.find( L"outline=" ) + 8, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Outline thickness not found in font file '" + fontFile + L"'" );
		}
		outlineThickness = stoi( temp );


		//process line 2
		std::getline( fin, line );

		temp = line.substr( line.find( L"lineHeight=" ) + 11, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Line Height not found in font file '" + fontFile + L"'" );
		}
		cellHeight = stoi( temp );

		temp = line.substr( line.find( L"base=" ) + 5, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Base value not found in font file '" + fontFile + L"'" );
		}
		base = stoi( temp );

		temp = line.substr( line.find( L"scaleW=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Image width not found in font file '" + fontFile + L"'" );
		}
		imageWidth = stoi( temp );

		temp = line.substr( line.find( L"scaleH=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Image height not found in font file '" + fontFile + L"'" );
		}
		imageHeight = stoi( temp );

		temp = line.substr( line.find( L"pages=" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Page count not found in font file '" + fontFile + L"'" );
		}
		numPages = stoi( temp );


		//process line 3
		std::getline( fin, line );

		temp = line.substr( line.find( L"file=\"" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L"\"" ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Font image not found in font file '" + fontFile + L"'" );
		}
		textureFilename = directory + temp;
		textureName = temp.substr( 0, temp.find_last_of( L"." ) ) + L"/Diffuse";

		//process line 4
		std::getline( fin, line );

		temp = line.substr( line.find( L"count=" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L"\n" ) );
		if( temp.size() == 0 )
		{
			throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character count not found in font file '" + fontFile + L"'" );
		}
		charCount = stoi( temp );

		/*amount to multiply the position in the texture with in order
		to get the texture coordinates between 0 and 1*/
		float texU = 1.0f / (float)imageWidth;
		float texV = 1.0f / (float)imageHeight;

		int lastCharId = 0;
		while( std::getline( fin, line ) )
		{
			//skip kerning lines
			if( line[0] == 'k' )
			{
				break;
			}

			int charId = 0;

			temp = line.substr( line.find( L"id=" ) + 3, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.size() == 0 )
			{
				throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character id not found in font file '" + fontFile + L"'" );
			}
			charId = stoi( temp );

			//generate empty characters in place of missing characters
			for( int i = lastCharId + 1; i < charId; i++ )
			{
				Kiwi::Font::Character newChar;
				newChar.uvBottom = 0;
				newChar.uvLeft = 0;
				newChar.uvRight = 0;
				newChar.uvTop = 0;
				newChar.charWidth = 0;
				newChar.charHeight = 0;
				characterSet.push_back( newChar );
			}
			lastCharId = charId;

			//generate the current character
			Kiwi::Font::Character newChar;

			temp = line.substr( line.find( L"x=" ) + 2, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.size() == 0 )
			{
				throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character x pos not found in font file '" + fontFile + L"'" );
			}
			newChar.x = stod( temp );
			newChar.uvLeft = newChar.x / (double)imageWidth;

			temp = line.substr( line.find( L"y=" ) + 2, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.size() == 0 )
			{
				throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character y pos not found in font file '" + fontFile + L"'" );
			}
			newChar.y = stod( temp );
			newChar.uvTop = newChar.y / (double)imageHeight;

			temp = line.substr( line.find( L"height=" ) + 7, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.size() == 0 )
			{
				throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character height not found in font file '" + fontFile + L"'" );
			}
			newChar.uvBottom = (newChar.y + stod( temp )) / (double)imageHeight;
			newChar.charHeight = stod( temp );

			temp = line.substr( line.find( L"xadvance=" ) + 9, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.size() == 0 )
			{
				throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character x advance not found in font file '" + fontFile + L"'" );
			}
			newChar.charWidth = stod( temp );

			temp = line.substr( line.find( L"width=" ) + 6, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.size() == 0 )
			{
				throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character width not found in font file '" + fontFile + L"'" );
			}
			newChar.uvRight = (newChar.x + stod( temp )) / (double)imageWidth;

			characterSet.push_back( newChar );

		}

		fin.close();

		//check if the texture is already loaded and in the scene
		Kiwi::Texture* fontTexture = m_scene->FindAsset<Kiwi::Texture>( textureName );

		if( fontTexture == 0 )
		{
			TextureDesc* textureDesc = new TextureDesc( textureName, textureFilename, true );
			if( textureDesc != 0 )
			{
				fontTexture = this->_LoadTexture( textureDesc );
				if( fontTexture == 0 )
				{
					//failed to load
					MessageBox( NULL, L"Failed to load texture", L"SceneLoader::_LoadFont", MB_OK );
				}
			}
		}

		Kiwi::Font* newFont = new Kiwi::Font( name, fontTexture, characterSet );

		m_byteMutex.lock();
		m_loadedSizeInBytes += sizeof( Kiwi::Font );
		m_byteMutex.unlock();

		m_scene->AddAsset( newFont );

		SAFE_DELETE( fontDesc );

		return newFont;

	}

	void SceneLoader::Start( unsigned int maxThreads )
	{

		assert( m_scene != 0 );

		if( m_scene == 0 )
		{
			return;
		}

		if( !m_isLoading )
		{

			Kiwi::EngineRoot* engine = m_scene->GetEngine();
			assert( engine != 0 );

			if( engine != 0 )
			{
				if( maxThreads == 0 ) maxThreads = UINT_MAX;

				m_maxThreadCount = maxThreads;
				m_isLoading = true;
				m_numAssetsLoaded = 0;
				m_loadedSizeInBytes = 0;

				m_loadQueueMutex.lock();
				while( m_loadQueue.size() > 0 && m_activeThreadIDs.size() < maxThreads )
				{
					Kiwi::IThreadParam* desc = m_loadQueue.front();
					if( desc != 0 )
					{
						unsigned int threadID = engine->SpawnThread<Kiwi::SceneLoader>( this, &Kiwi::SceneLoader::_Load_Async, desc );
						if( threadID != 0 )
						{
							m_activeThreadIDs.insert( threadID );
						}
					}

					m_loadQueue.pop_front();
				}
				m_loadQueueMutex.unlock();
			}

		}

	}

	void SceneLoader::OnUpdate()
	{

		if( m_isLoading )
		{
			Kiwi::EngineRoot* engine = m_scene->GetEngine();
			assert( engine != 0 );

			if( engine != 0 )
			{
				if( m_activeThreadIDs.size() == 0 && m_loadQueue.size() == 0 )
				{
					m_isLoading = false;

				} else
				{
					for( auto itr = m_activeThreadIDs.begin(); itr != m_activeThreadIDs.end(); )
					{
						Kiwi::ThreadStatus status = engine->GetThreadStatus( *itr );

						if( status == Kiwi::THREAD_READY )
						{
							try
							{
								engine->JoinThread( *itr );

							} catch( Kiwi::Exception& e )
							{
								m_caughtExceptions.push_back( e );
								Console* console = engine->GetConsole();
								console->Write( e.GetSource() + e.GetError() );
								console->Print( e.GetSource() + e.GetError() );
							}

							itr = m_activeThreadIDs.erase( itr );

						} else
						{
							itr++;
						}
					}

					m_loadQueueMutex.lock();
					while( m_loadQueue.size() > 0 && m_activeThreadIDs.size() < m_maxThreadCount )
					{
						unsigned int threadID = engine->SpawnThread<Kiwi::SceneLoader>( this, &SceneLoader::_Load_Async, m_loadQueue.front() );
						m_loadQueue.pop_front();
						if( threadID != 0 )
						{
							m_activeThreadIDs.insert( threadID );
						}
					}
					m_loadQueueMutex.unlock();
				}
			}
		}

	}

	/*loads a texture*/
	void SceneLoader::LoadTextureFromFile( std::wstring textureName, std::wstring textureFile, bool hasTransparency )
	{

		std::lock_guard<std::mutex> lock( m_loadQueueMutex );

		TextureDesc* textureDesc = new TextureDesc( textureName, textureFile, hasTransparency );
		if( textureDesc != 0 )
		{
			m_loadQueue.push_back( textureDesc );
		}

	}

	void SceneLoader::LoadFontFromFile( std::wstring fontFile )
	{

		std::lock_guard<std::mutex> lock( m_loadQueueMutex );

		FontDesc* fontDesc = new FontDesc( fontFile );
		if( fontDesc != 0 )
		{
			m_loadQueue.push_back( fontDesc );
		}


	}

	void SceneLoader::LoadStaticMeshFromFile( std::wstring meshName, std::wstring objFile )
	{

		std::lock_guard<std::mutex> lock( m_loadQueueMutex );

		StaticMeshDesc* meshDesc = new StaticMeshDesc( meshName, objFile );
		if( meshDesc != 0 )
		{
			m_loadQueue.push_back( meshDesc );
		}

	}

}