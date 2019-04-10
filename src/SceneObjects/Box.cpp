#include <cmath>
#include <assert.h>

#include "Box.h"
#include "../vecmath/vecmath.h"

bool Box::intersectLocal(const ray& r, isect& i) const
{
	double tMin, tMax;
	bool intersected = ComputeLocalBoundingBox().intersect(r, tMin, tMax);
	if (intersected) {
		double intersectT = (tMin < 0) ? tMax : tMin;

		vec3f intersection = r.at(intersectT);

		if (abs(intersection[0] - 0.5) < NORMAL_EPSILON) {
			i.setN(vec3f(1, 0, 0));
		}
		else if (abs(intersection[0] + 0.5) < NORMAL_EPSILON) {
			i.setN(vec3f(-1, 0, 0));
		}
		else if (abs(intersection[1] - 0.5) < NORMAL_EPSILON) {
			i.setN(vec3f(0, 1, 0));
		}
		else if (abs(intersection[1] + 0.5) < NORMAL_EPSILON) {
			i.setN(vec3f(0, -1, 0));
		}
		else if (abs(intersection[2] - 0.5) < NORMAL_EPSILON) {
			i.setN(vec3f(0, 0, 1));
		}
		else if (abs(intersection[2] + 0.5) < NORMAL_EPSILON) {
			i.setN(vec3f(0, 0, -1));
		}

		i.t = intersectT;
		i.obj = this;
		return true;
	}
	return false;
}