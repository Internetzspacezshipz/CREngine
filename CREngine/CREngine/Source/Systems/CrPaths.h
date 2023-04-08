#pragma once

#include "CrTypes.h"

//Shaders in this subfolder
const static Path ShaderPath { "Shaders/Compiled" };

//Assets such as textures and meshes in this subfolder
const static Path AssetPath { "Assets" };

//Json data in this subfolder
const static Path DataPath { "Data" };

static Path BasePath()
{
	return std::filesystem::current_path();
}
static Path GetShadersPath()
{
	return BasePath() / ShaderPath;
}

static Path GetAssetsPath()
{
	return BasePath() / AssetPath;
}

static Path GetDataPath()
{
	return BasePath() / DataPath;
}

