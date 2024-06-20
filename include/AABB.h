#ifndef AABB_H
#define AABB_H

#include "Interval.h"
#include "vec3.h"
#include "ray.h"
#include <glm/glm.hpp>

class AABB {
public:
	Interval x, y, z;
	
	AABB() {};
	AABB(const Vec3& p1, const Vec3& p2);
	AABB(const AABB& box1, const AABB& box2);
	
	glm::vec3 getMinBounds();
	glm::vec3 getMaxBounds();

	const void thicken();
	const Interval& axis(int i) const;
	bool hit(const Ray& ray, Interval ray_t) const;
	
	const Interval& operator[](int axis) const;
};


#endif // !AABBH_H
