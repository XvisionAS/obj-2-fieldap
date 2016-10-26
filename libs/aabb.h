#pragma once

#include <limits>
#include "v3.h"

struct aabb {
	v3 		min;
	v3		max;

	int		index;
	
	void init() {
		min.set(std::numeric_limits<float>::max());
		max.set(-std::numeric_limits<float>::max());
	}
	void add(const v3& v) {
		min.min(v);
		max.max(v);
	}

	void size(v3& v) const {
		v.x = fabsf(max.x - min.x);
		v.y = fabsf(max.y - min.y);
		v.z = fabsf(max.z - min.z);
	}

	bool bigger(const aabb& that) const {
		v3 a, b;

		size(a);
		that.size(b);
		return (a.x + a.y) > (b.x + b.y);
	}


};