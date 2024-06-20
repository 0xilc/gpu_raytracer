#pragma once

#include "BVH.h"
#include "GPUStructs.h"
#include <vector>

void FlattenBVH(std::shared_ptr<Hittable> root, std::vector<GPU::BVHNode>& flatBVH, 
				std::vector<GPU::Primitive>& primitives);

void ExtractMaterials(std::vector<GPU::Material>& materials, parser::Scene& scene);

void ExtractLights(std::vector<GPU::Light>& lights, parser::Scene& scene);