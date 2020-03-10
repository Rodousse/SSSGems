#include "ShaderUtils.h"
#include <fstream>
#include <algorithm>
#include <exception>

namespace shader
{

std::vector<char> loadShader(const std::string& path)
{
    std::ifstream file;
    file.open(path, std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("[SHADER UTILS] : Can't open shader : " + path);
    }

    size_t fileSize = file.tellg();

    std::vector<char> shaderData;
    shaderData.resize(fileSize);

    file.seekg(0);
    file.read(shaderData.data(), fileSize);
    file.close();

    return shaderData;
}

}
