#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
extern parser::Scene scene;

class Sphere : public Hittable {
public:
	Vec3 center;
	double radius;
	int material_id;

	Sphere(parser::Sphere _sphere)
		: center(scene.vertex_data[_sphere.center_vertex_id - 1]), radius(_sphere.radius), material_id(_sphere.material_id)
	{
		bounding_box = AABB(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
	}

	bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override
	{
		
		Vec3 oc = ray.origin - center;
		double a = ray.direction.dot(ray.direction);
		double b = oc.dot(ray.direction);
		double c = oc.dot(oc) - radius * radius;
		double discriminant = b * b - a * c;

		// Intersection occurs
		if (discriminant >= 0) {
			double t1 = (-b - sqrt(discriminant)) / a;
			double t2 = (-b + sqrt(discriminant)) / a;

			if (t1 >= 0 || t2 >= 0) {
				double t = (t1 >= 0) ? t1 : t2;

				Vec3 hitPoint = ray.origin + ray.direction * t;
				Vec3 normal = (hitPoint - center).normalize();

				rec.t = t;
				rec.p = hitPoint;
				rec.normal = normal;
				rec.material_id = material_id;
				return true;
			}
		}
		return false;

	};
	AABB getAABB() const override {
		return bounding_box;
	}

	AABB bounding_box;
};
#endif // !SPHERE_H
