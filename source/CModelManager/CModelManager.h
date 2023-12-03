#pragma once

class CModelManager
{
public:
    static bool IsModelLoaded(int modelId);
    static bool LoadModel(int modelId);
};