#include "Box.h"
#include <cmath>
#include <limits>

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// Algorithm amended from U of Utah: http://www.cs.utah.edu/~awilliam/box/box.pdf

	i.obj = this;

	std::numeric_limits<double> limit;
	double tmin = -limit.infinity();
	double tmax = limit.infinity();
	double curTmin, curTmax;

	vec3f nmin, nmax;

	vec3f& rayOrigin = r.getPosition();
	vec3f& rayDirection = r.getDirection();

	double lowerBound = -0.5, upperBound = 0.5;

	// Loop through as
	for (int i = 0; i < 3; ++i)
	{
		// Parallel case
		if (abs(rayDirection[i]) < RAY_EPSILON) {
			if (rayOrigin[i] < lowerBound || rayOrigin[i] > upperBound) {
				return false;
			}
		}

		vec3f curNmin, curNmax;

		if (rayDirection[i] >= 0) {
			curTmin = (lowerBound - rayOrigin[i]) / rayDirection[i];
			curTmax = (upperBound - rayOrigin[i]) / rayDirection[i];
			curNmin[i] = -1;
			curNmax[i] = 1;
		}
		else {
			curTmin = (upperBound - rayOrigin[i]) / rayDirection[i];
			curTmax = (lowerBound - rayOrigin[i]) / rayDirection[i];
			curNmin[i] = 1;
			curNmax[i] = -1;
		}

		if (tmin > curTmax || curTmin > tmax)
			return false;

		if (curTmin > tmin) {
			tmin = curTmin;
			nmin = curNmin;
		}

		if (curTmax < tmax) {
			tmax = curTmax;
			nmax = curNmax;
		}
	}

	i.setT(tmin);
	i.setN(nmin);
	return true;
}
