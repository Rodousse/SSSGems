#pragma once

#include <vector>
#include <string>
#include "Mesh.h"

namespace loader
{

bool load(const std::string& path, Mesh& scene);

}
