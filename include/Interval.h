#pragma once
#include <cmath>
#include <algorithm>

class Interval {
public:
	double min, max;

	Interval() : min(INFINITY), max(-INFINITY) {};
	Interval(double _min, double _max) : min(_min), max(_max) {}
	Interval(Interval _i0, Interval _i1) : min(std::min(_i0.min, _i1.min)), max(std::max(_i0.max, _i1.max)) {}

	const void thicken();
	inline Interval merge(const Interval& _other) const;
	inline bool overlap(const Interval& _other) const;
	inline bool consists(const double& point) const;
	inline double getLength() const;
};

