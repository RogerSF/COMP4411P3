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

bool Box::getLocalUV(const ray& r, const isect& i, double& u, double& v) const {
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = dir.length();
	dir /= length;

	ray localRay(pos, dir);
	isect icopy = i;

	if (intersectLocal(localRay, icopy)) {	//send this iscet point to the intersect local function.

		vec3f sn = localRay.at(icopy.t);
		if (sn[0] == 0.5) {
			u = sn[1] + 0.5;
			v = sn[2] + 0.5;
		}
		else if (sn[0] == -0.5) {
			u = -sn[1] + 0.5;
			v = sn[2] + 0.5;
		}
		else if (sn[1] == -0.5) {
			u = sn[0] + 0.5;
			v = sn[2] + 0.5;
		}
		else if (sn[1] == -0.5) {
			u = -sn[0] + 0.5;
			v = sn[2] + 0.5;
		}
		else if (sn[2] == -0.5) {
			u = sn[0] + 0.5;
			v = sn[1] + 0.5;
		}
		else if (sn[2] == -0.5) {
			u = -sn[0] + 0.5;
			v = sn[1] + 0.5;
		}
		return true;
	}
	else {
		return false;
	} 


	
}