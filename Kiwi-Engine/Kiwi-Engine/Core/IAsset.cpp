#include "IAsset.h"

namespace Kiwi
{

	int IAsset::GlobalAssetID = 1;
	std::mutex IAsset::GlobalAssetIDMutex;

	IAsset::IAsset( std::wstring name, std::wstring assetType )
	{

		m_mutex->lock();

			m_assetLoadState = UNLOADED;
			m_assetName = name;
			m_assetType = assetType;

			GlobalAssetIDMutex.lock();
				m_assetID = GlobalAssetID++;
				if( GlobalAssetID == 0 )
				{
					GlobalAssetID++;
				}
			GlobalAssetIDMutex.unlock();

		m_mutex->unlock();

	}

	IAsset::~IAsset()
	{

		Kiwi::FreeMemory( m_assetFiles );

	}

	std::wstring IAsset::GetAssetFile( unsigned int fileIndex )const
	{

		if( fileIndex >= m_assetFiles.size() ) return L"";

		return m_assetFiles[fileIndex];

	}

}