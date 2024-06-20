#include "bvh.h"

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, int begin, int end) 
{
	int axis = rand() % 3;
	if (begin == end)
	{
		left = objects[begin];
		right = objects[begin];
	}
	else if (begin == end - 1)
	{
		left = objects[begin];
		right = objects[end];
	}
	else
	{
		std::sort(objects.begin() + begin, objects.begin() + end + 1,
			[axis](const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable> b)
			{
				return a->getAABB()[axis].min < b->getAABB()[axis].min;
			}
		);
		int mid = (begin + end) / 2;
		left = std::make_shared<BVHNode>(objects, begin, mid);
		right = std::make_shared<BVHNode>(objects, mid, end);
	}

	bounding_box = AABB(left->getAABB(), right->getAABB());
}


bool BVHNode::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
	if (!bounding_box.hit(ray, ray_t)) return false;
	HitRecord rec1, rec2;

	bool hit_left = left->hit(ray, ray_t, rec1);
	bool hit_right = right->hit(ray, ray_t, rec2);

	if (hit_left && hit_right) {
		if (rec1.t < rec2.t) {
			rec = rec1;
		}
		else {
			rec = rec2;
		}
		return true;
	}
	else if (hit_left) {
		rec = rec1;
		return true;
	}
	else if (hit_right) {
		rec = rec2;
		return true;
	}

	return false;
}


AABB BVHNode::getAABB() const { return bounding_box; }