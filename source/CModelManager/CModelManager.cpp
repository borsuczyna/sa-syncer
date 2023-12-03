#include "../stdafx.h"

bool CModelManager::IsModelLoaded(int modelId)
{
	return CStreaming::ms_aInfoForModel[modelId].m_nLoadState == LOADSTATE_LOADED;
}

bool CModelManager::LoadModel(int modelId)
{
	bool loaded = IsModelLoaded(modelId);
	if (loaded) return true;

	unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelId].m_nFlags;
	CStreaming::RequestModel(modelId, GAME_REQUIRED | PRIORITY_REQUEST | MISSION_REQUIRED);
	CStreaming::LoadAllRequestedModels(true);

	return IsModelLoaded(modelId);
}