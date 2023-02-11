#include "CRE_GraphicsPipeline.hpp"

#include <fstream>
#include <iostream>

CRE_GraphicsPipeline::CRE_GraphicsPipeline(const std::string& VertShaderFilePath, const std::string& FragShaderFilePath)
{
    CreateGraphicsPipeline(VertShaderFilePath, FragShaderFilePath);
}

std::vector<char> CRE_GraphicsPipeline::ReadFile(const std::string& FilePath)
{
    std::ifstream File(FilePath, std::ios::ate | std::ios::binary);

    if (!File.is_open())
    {
        throw std::runtime_error("Failed to open file: +" + FilePath);
        return std::vector<char>();
    }

    size_t FileSize = static_cast<size_t>(File.tellg());

    std::vector<char> FileBuffer(FileSize);

    File.seekg(0);
    File.read(FileBuffer.data(), FileSize);

    File.close();
    return FileBuffer;
}

void CRE_GraphicsPipeline::CreateGraphicsPipeline(const std::string& VertShaderFilePath, const std::string& FragShaderFilePath)
{
    auto VertCode = ReadFile(VertShaderFilePath);
    auto FragCode = ReadFile(FragShaderFilePath);

    std::cout << "Vert shader size = " << VertCode.size() << "\n";
    std::cout << "Frag shader size = " << FragCode.size() << "\n";
}
