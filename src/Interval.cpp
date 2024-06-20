#include "Interval.h"


const void Interval::thicken() { min = min - 0.0001f; max = max + 0.0001f; }

inline Interval Interval::merge(const Interval& _other) const { return Interval(std::min(min, _other.min), std::min(max, _other.max)); }

inline bool Interval::overlap(const Interval& _other) const { return (min <= _other.max && _other.min <= max); }

inline bool Interval::consists(const double& point) const { return (min <= point && max >= point); }

inline double Interval::getLength() const { return max - min; }

