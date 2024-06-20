#ifndef VEC3_H
#define VEC3_H
#include "parser.h"
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

class Vec3 {
public:
	double x, y, z;

	Vec3() : x(0), y(0), z(0) {}
	Vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	Vec3(parser::Vec3f _other)
		: x(_other.x), y(_other.y), z(_other.z) {}
	Vec3(parser::Vec3i _other)
		: x(_other.x), y(_other.y), z(_other.z) {}

	inline Vec3 normalize() {
		double len = length();
		x /= len;
		y /= len;
		z /= len;
		return *this;
	}

	inline Vec3 reverse() {
		x *= -1;
		y *= -1;
		z *= -1;
		return *this;
	}
	
	inline Vec3 cross(const Vec3& _other) const{
		return Vec3(
			y * _other.z - z * _other.y,
			z * _other.x - x * _other.z,
			x * _other.y - y * _other.x);
	}

	inline double dot(const Vec3& _other) const {
		return x * _other.x + y * _other.y + z * _other.z;
	}

	inline double length() const {
		return sqrt(x * x + y * y + z * z);
	}

	// Overloads
	inline Vec3 operator+(const Vec3& other) const {
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	inline Vec3 operator-(const Vec3& other) const {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	inline Vec3 operator*(const Vec3& other) const {
		return Vec3(x * other.x, y * other.y, z * other.z);
	}

	inline Vec3 operator*(const double& other) const {
		return Vec3(x * other, y * other, z * other);
	}

	inline Vec3 operator/(const double& other) const {
		return Vec3(x / other, y / other, z / other);
	}
	
	inline operator glm::vec3() const {
		return glm::vec3(x, y, z);
	}
	
	inline operator glm::vec4() const {
		return glm::vec4(x, y, z, 0);
	}

	inline double operator[](int i) const {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		throw std::out_of_range("vec3, axis index out of range");
	}
};

#endif // !VEC3_H
