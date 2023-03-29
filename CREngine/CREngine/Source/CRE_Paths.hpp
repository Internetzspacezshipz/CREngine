#pragma once

#include "CRE_Types.hpp"

//Shaders in this subfolder
const static Path ShaderPath { "Shaders/Compiled" };

//Assets such as textures and meshes in this subfolder
const static Path AssetPath { "Assets" };

//Json data in this subfolder
const static Path DataPath { "Data" };

static Path getShadersPath()
{
	return std::filesystem::current_path() / ShaderPath;
}

static Path getAssetsPath()
{
	return std::filesystem::current_path() / AssetPath;
}