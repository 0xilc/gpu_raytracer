#include "AABB.h"

AABB::AABB(const Vec3& p1, const Vec3& p2) 
{
	x = Interval(p1.x, p2.x);
	y = Interval(p1.y, p2.y);
	z = Interval(p1.z, p2.z);
	thicken();
}

AABB::AABB(const AABB& box1, const AABB& box2)
{
	x = Interval(box1.x, box2.x);
	y = Interval(box1.y, box2.y);
	z = Interval(box1.z, box2.z);
	thicken();
}

glm::vec3 AABB::getMinBounds()
{
	return glm::vec3(
		x.min,
		y.min,
		z.min
	);
}

glm::vec3 AABB::getMaxBounds()
{
	return glm::vec3(
		x.max,
		y.max,
		z.max
	);
}

const void AABB::thicken() 
{
	x.thicken();
	y.thicken();
	z.thicken();
}

const Interval& AABB::axis(int i) const
{
	if (i == 0) return x;
	if (i == 1) return y;
	else return z;
}

bool AABB::hit(const Ray& ray, Interval ray_t) const
{
	for (int i = 0; i < 3; i++)
	{
		double t0 = (axis(i).min - ray.origin[i]) / ray.direction[i];
		double t1 = (axis(i).max - ray.origin[i]) / ray.direction[i];
		if (t0 > t1) std::swap(t0, t1);

		// check whether overlaps
		if (ray_t.min < t0) ray_t.min = t0;
		if (ray_t.max > t1) ray_t.max = t1;

		if (ray_t.max <= ray_t.min) return false; // means no overlap between three axices.
	}
	return true;
}

const Interval& AABB::operator[](int axis) const 
{
	switch (axis) {
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		throw std::out_of_range("Invalid axis index");
	}
}