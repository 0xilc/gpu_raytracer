#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "parser.h"
#include "aabb.h"

extern parser::Scene scene;

class Triangle : public Hittable {
public:

	Triangle(parser::Triangle _triangle)
		: indices{ scene.vertex_data[_triangle.indices.v0_id - 1],
				   scene.vertex_data[_triangle.indices.v1_id - 1],
				   scene.vertex_data[_triangle.indices.v2_id - 1]},
	      material_id(_triangle.material_id)
	{
		Vec3 min = indices[0];
		Vec3 max = indices[0];
		for (int i = 1; i < 3; i++)
		{
			min.x = fmin(indices[i].x, min.x);
			max.x = fmax(indices[i].x, max.x);
			min.y = fmin(indices[i].y, min.y);
			max.y = fmax(indices[i].y, max.y);
			min.z = fmin(indices[i].z, min.z);
			max.z = fmax(indices[i].z, max.z);
		}
		bounding_box = AABB(min, max);
	}

	Triangle(parser::Face _face, int _material_id)
		: indices{ scene.vertex_data[_face.v0_id - 1],
				   scene.vertex_data[_face.v1_id - 1],
				   scene.vertex_data[_face.v2_id - 1] },
		  material_id(_material_id)
	{
		
		Vec3 min = indices[0];
		Vec3 max = indices[0];
		for (int i = 1; i < 3; i++)
		{
			min.x = fmin(indices[i].x, min.x);
			max.x = fmax(indices[i].x, max.x);
			min.y = fmin(indices[i].y, min.y);
			max.y = fmax(indices[i].y, max.y);
			min.z = fmin(indices[i].z, min.z);
			max.z = fmax(indices[i].z, max.z);
		}
		bounding_box = AABB(min, max);
	}

	bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override {
		Vec3 c1 = indices[0] - indices[1];
		Vec3 c2 = indices[0] - indices[2];
		Vec3 c3 = ray.direction;
		double detA = det(c1, c2, c3);
		if (detA == 0) return false;

		c1 = indices[0] - ray.origin;
		double beta = det(c1, c2, c3) / detA;
		
		c2 = c1;
		c1 = indices[0] - indices[1];
		double gamma = det(c1, c2, c3) / detA;

		c3 = c2;
		c2 = indices[0] - indices[2];
		double t = det(c1, c2, c3) / detA;

		if (t < ray_t.min + 0.0001|| 0.0001 + t > ray_t.max) return false;

		if (beta + gamma <= 1 && beta + 0.00001 >= 0 && gamma + 0.00001 >= 0)
		{
			Vec3 vec1 = indices[1] - indices[0];
			Vec3 vec2 = indices[2] - indices[0];
			vec1 = vec1.cross(vec2);
			vec1.normalize();

			rec.t = t;
			rec.normal = vec1;
			rec.p = ray.origin + ray.direction * t;
			rec.material_id = material_id;
			return true;
		}
		return false;
	}

	AABB getAABB() const override { return bounding_box; }

	int material_id;
	Vec3 indices[3];
	AABB bounding_box;

	inline double det(const Vec3& c0, const Vec3& c1, const Vec3& c2) const
	{
		double temp1 = c0.x *
			(c1.y * c2.z - c1.z * c2.y);

		double temp2 = c1.x *
			(c0.y * c2.z - c0.z * c2.y);

		double temp3 = c2.x *
			(c0.y * c1.z - c0.z * c1.y);

		return temp1 - temp2 + temp3;
	}

};

#endif // !TRIANGLE_H
