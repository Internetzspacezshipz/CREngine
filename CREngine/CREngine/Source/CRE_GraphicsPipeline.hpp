#pragma once

#include <string>
#include <vector>

class CRE_GraphicsPipeline
{
public:
	CRE_GraphicsPipeline(const std::string& VertShaderFilePath, const std::string& FragShaderFilePath);
private:
	static std::vector<char> ReadFile(const std::string& FilePath);
	void CreateGraphicsPipeline(const std::string& VertShaderFilePath, const std::string& FragShaderFilePath);
};