#include "Utils.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Parser.h"


void FlattenBVH(std::shared_ptr<Hittable> root, std::vector<GPU::BVHNode>& flatBVH, std::vector<GPU::Primitive>& primitives)
{
    if (root == nullptr) return;

    GPU::BVHNode node;
    node.leftChild = -1;
    node.rightChild = -1;
    node.primitiveIndex= -1;

    int currentIndex = flatBVH.size();
    flatBVH.push_back(node);

    // Internal Node type
    if (auto bvhNode = std::dynamic_pointer_cast<BVHNode>(root)) {
        node.minBounds = bvhNode->bounding_box.getMinBounds();
        node.maxBounds = bvhNode->bounding_box.getMaxBounds();

        if (bvhNode->left) {
            node.leftChild = flatBVH.size();
            FlattenBVH(bvhNode->left, flatBVH, primitives);
        }

        if (bvhNode->right) {
            node.rightChild = flatBVH.size();
            FlattenBVH(bvhNode->right, flatBVH, primitives);
        }
    }
    
    // Handle Sphere type
    else if (auto sphere = std::dynamic_pointer_cast<Sphere>(root)) {
        node.minBounds = sphere->bounding_box.getMinBounds();
        node.maxBounds = sphere->bounding_box.getMaxBounds();
        node.primitiveIndex = primitives.size();
        
        GPU::Primitive temp;
        temp.type = true;
        temp.materialId = sphere->material_id;
        temp.vertexData[0] = sphere->center;
        temp.vertexData[1].x = sphere->radius;
        primitives.push_back(temp);
    }
    
    // Handle Triangle type
    else if (auto triangle = std::dynamic_pointer_cast<Triangle>(root)) {
        node.minBounds = triangle->bounding_box.getMinBounds();
        node.maxBounds = triangle->bounding_box.getMaxBounds();
        node.primitiveIndex = primitives.size();

        GPU::Primitive temp;
        temp.type = false;
        temp.materialId = triangle->material_id;
        temp.vertexData[0] = triangle->indices[0];
        temp.vertexData[1] = triangle->indices[1];
        temp.vertexData[2] = triangle->indices[2];
        primitives.push_back(temp);
    }

    flatBVH[currentIndex] = node;
}


void ExtractMaterials(std::vector<GPU::Material>& materials, parser::Scene& scene)
{
    for (auto& mat : scene.materials)
    {
        GPU::Material temp;
        temp.ambient = glm::vec4(mat.ambient.x, mat.ambient.y, mat.ambient.z, 0);
        temp.specular = glm::vec4(mat.specular.x, mat.specular.y, mat.specular.z, 0);
        temp.diffuse = glm::vec4(mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, 0);
        temp.mirror = glm::vec4(mat.mirror.x, mat.mirror.y, mat.mirror.z, 0);
        temp.isMirror = mat.is_mirror;
        temp.phong_exponent = mat.phong_exponent;
        materials.push_back(temp);
    }
}

void ExtractLights(std::vector<GPU::Light>& lights, parser::Scene& scene)
{
    for (auto& light : scene.point_lights)
    {
        GPU::Light temp;
        temp.position = glm::vec3(light.position.x, light.position.y, light.position.z);
        temp.intensity = glm::vec3(light.intensity.x, light.intensity.y, light.intensity.z);
        temp.lightsSize = scene.point_lights.size();
        lights.push_back(temp);
    }
}