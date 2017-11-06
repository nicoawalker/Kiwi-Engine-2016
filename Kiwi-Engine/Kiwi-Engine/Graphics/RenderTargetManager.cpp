#include "RenderTargetManager.h"

#include "../Core/Utilities.h"

#include <algorithm>

namespace Kiwi
{

	void RenderTargetManager::AddRenderTarget( Kiwi::RenderTarget* renderTarget )
	{

		if( renderTarget )
		{
			m_renderTargets.Add( renderTarget->GetName(), renderTarget );
		}

	}

	Kiwi::RenderTargetList RenderTargetManager::GetRenderTargetList()
	{

		Kiwi::RenderTargetList list;

		for( auto itr = m_renderTargets.Front(); itr != m_renderTargets.Back(); itr++ )
		{
			list.targets.push_back( itr->second );
		}

		return list;

	}

}