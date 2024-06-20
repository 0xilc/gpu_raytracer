#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "interval.h"
#include "aabb.h"

struct HitRecord
{
	Vec3 p;
	Vec3 normal;
	double t;
	int material_id;
};

class Hittable {
public:

	virtual ~Hittable() = default;

	virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const = 0;
	virtual AABB getAABB() const = 0;

};
#endif // !HITTABLE_H
