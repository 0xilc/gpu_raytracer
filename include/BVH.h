#ifndef BVH_H
#define BVH_H

#include "Hittable.h"
#include "Parser.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <random>

class BVHNode : public Hittable{
public:
	BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, int begin, int end);

	bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;

	AABB getAABB() const override;

	AABB bounding_box;
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
};

#endif // !BVH_H
