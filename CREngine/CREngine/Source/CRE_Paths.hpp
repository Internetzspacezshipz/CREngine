#pragma once

#include <filesystem>

//TODO: LoadTexture from JSON.
static std::filesystem::path ShaderPath { "Shaders/Compiled" };
static std::filesystem::path AssetPath { "Assets" };

static std::filesystem::path getShadersPath()
{
	return std::filesystem::current_path() / ShaderPath;
}

static std::filesystem::path getAssetsPath()
{
	return std::filesystem::current_path() / AssetPath;
}