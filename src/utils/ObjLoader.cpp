#include "utils/ObjLoader.h"
#include <algorithm>
#include <unordered_map>
#include <glm/vec3.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


template <class Hasher, class Hashed>
inline void hash_combine(std::size_t& seed, const Hashed& v)
{
    Hasher hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


struct VertexIndicesHasher
{
    size_t operator()(const tinyobj::index_t& hashed)const
    {
        size_t seed = 0xab12f56c;
        hash_combine<std::hash<int>, int>(seed, hashed.vertex_index);
        hash_combine<std::hash<int>, int>(seed, hashed.normal_index);
        return seed;
    }
};

struct VertexIndicesEquals
{
    bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b)const
    {
        return a.vertex_index == b.vertex_index && a.normal_index == b.normal_index;
    }
};

namespace loader
{
bool load(const std::string& path, Mesh& scene)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    std::string warn;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

    if(!ret)
    {
        return false;
    }


    uint32_t indexVertex = 0;
    std::unordered_map<tinyobj::index_t, uint32_t, VertexIndicesHasher, VertexIndicesEquals>
    attributeIndices;

    // Loop over shapes
    for(tinyobj::shape_t shape : shapes)
    {
        size_t index_offset = 0;


        for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];

            for(int v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                if(attributeIndices.count(idx) == 0)
                {

                    size_t indexTemp;
                    Vertex newVertex;

                    if(idx.vertex_index > -1)
                    {
                        indexTemp = idx.vertex_index * 3;
                        newVertex.position = glm::vec3(attrib.vertices[indexTemp], attrib.vertices[indexTemp + 1],
                                                       attrib.vertices[indexTemp + 2]);
                    }

                    if(idx.normal_index > -1)
                    {
                        indexTemp = idx.normal_index * 3;
                        newVertex.normal = glm::vec3(attrib.normals[indexTemp], attrib.normals[indexTemp + 1],
                                                     attrib.normals[indexTemp + 2]);
                    }

                    scene.vertices.push_back(newVertex);
                    attributeIndices[idx] = indexVertex;
                    indexVertex++;
                }

                scene.indices.push_back(attributeIndices[idx]);
            }

            index_offset += fv;
        }

    }

    return true;
}

}
