#ifndef _KIWI_SCENELOADER_H_
#define _KIWI_SCENELOADER_H_

#include "Exception.h"
#include "IAsset.h"
#include "IThreadParam.h"

#include "../Graphics/Font.h"

#include <string>
#include <deque>
#include <vector>
#include <set>
#include <mutex>
#include <thread>

namespace Kiwi
{

	class Scene;

	class SceneLoader
	{
	protected:

		class AssetDesc :
			public Kiwi::IThreadParam
		{
		public:

			std::wstring assetType;
			std::wstring assetName;

			AssetDesc() {}
			virtual ~AssetDesc() {}

		};

		class TextureDesc :
			public AssetDesc
		{
		public:

			std::wstring textureFile;
			bool hasTransparency;

			TextureDesc( std::wstring textureName, std::wstring textureFile, bool hasTransparency )
			{
				assetType = L"texture";
				assetName = textureName;
				this->textureFile = textureFile;
				this->hasTransparency = hasTransparency;
			}
		};

		class FontDesc :
			public AssetDesc
		{
		public:

			std::wstring fontFile;

			FontDesc( std::wstring fontFile )
			{
				assetType = L"font";
				this->fontFile = fontFile;
			}
		};

		class StaticMeshDesc :
			public AssetDesc
		{
		public:

			std::wstring objFile;

			StaticMeshDesc( std::wstring meshName, std::wstring objFile )
			{
				assetType = L"staticmesh";
				assetName = meshName;
				this->objFile = objFile;
			}
		};

		Kiwi::Scene* m_scene;

		std::deque<AssetDesc*> m_loadQueue;

		std::set<unsigned int> m_activeThreadIDs;

		//stores all of the exceptions caught from the loading threads
		std::vector<Kiwi::Exception> m_caughtExceptions;

		std::mutex m_loadQueueMutex;

		std::mutex m_byteMutex; //mutex for the m_loadedSizeInBytes variable
		
		unsigned int m_maxThreadCount;

		//size of all of the assets that are to be loaded, obtained using sizeof() on their types
		unsigned long m_totalSizeInBytes;
		//size of all of the assets that have been loaded
		unsigned long m_loadedSizeInBytes;

		unsigned int m_numAssetsLoaded;

		//stores whether or not the loader is in the process of loading
		bool m_isLoading;

	protected:

		/*spawns threads to load up to loadCount assets*/
		virtual Kiwi::IThreadParam* _Load_Async( Kiwi::IThreadParam* param );

		Kiwi::Texture* _LoadTexture( TextureDesc* textureDesc );
		Kiwi::StaticMeshAsset* _LoadStaticMesh( StaticMeshDesc* meshDesc );
		Kiwi::Font* _LoadFont( FontDesc* fontDesc );

	public:

		SceneLoader( Kiwi::Scene* scene );
		~SceneLoader();

		void OnUpdate();

		/*loads a texture*/
		void LoadTextureFromFile( std::wstring textureName, std::wstring textureFile, bool hasTransparency );

		void LoadFontFromFile( std::wstring fontFile );

		void LoadStaticMeshFromFile( std::wstring meshName, std::wstring objFile );

		//void LoadFromFile( std::wstring filename );

		void Start( unsigned int maxThreads = 40 );

		//gets the total number of bytes that have been queued for loading
		unsigned long GetBytesTotal()const { return m_totalSizeInBytes; }

		//gets the number of bytes that have been loaded so far
		unsigned long GetBytesLoaded()const { return m_loadedSizeInBytes; }

		/*returns a vector containing all of the exceptions that were caught during loading*/
		std::vector<Kiwi::Exception> GetExceptions()const { return m_caughtExceptions; }

		bool IsFinished()const { return !m_isLoading; }

	};	

}

#endif