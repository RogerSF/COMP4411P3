#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
	vec3f target_direction = getDirection(P);
	vec3f current_target = P;
	isect target_intersection;
	vec3f intensity = getColor(P);
	ray r = ray(current_target, target_direction);

	while (scene->intersect(r, target_intersection))
	{
		// Return black if the material is not transparent
		if (target_intersection.getMaterial().kt.iszero()) return vec3f(0, 0, 0);

		//use current intersection point as new light source
		current_target = r.at(target_intersection.t);
		r = ray(current_target, target_direction);
		intensity = prod(intensity, target_intersection.getMaterial().kt);
	}

	return intensity;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double distance_squared = (P - position).length_squared();
	double distance = sqrt(distance_squared);
	double distance_coeff = traceUI->getAttenConst() + traceUI->getAttenLinear() * distance + traceUI->getAttenQuad() * distance_squared;
	return distance_coeff == 0.0 ? 1.0 : minimum(1, 1 / distance_coeff);
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
	double distance = (position - P).length();
	vec3f light_direction = getDirection(P);
	vec3f current_target = P;
	isect target_intersection;
	vec3f intensity = getColor(P);
	ray r = ray(current_target, light_direction);

	while (scene->intersect(r, target_intersection))
	{
		// Keep intersection within point light source's range
		distance -= target_intersection.t;
		if (distance < RAY_EPSILON) return intensity;
		
		// Return black if the material is not transparent
		if (target_intersection.getMaterial().kt.iszero()) return vec3f(0, 0, 0);

		//use current intersection point as new light source
		current_target = r.at(target_intersection.t);
		r = ray(current_target, light_direction);
		intensity = prod(intensity, target_intersection.getMaterial().kt);
	}
	return intensity;
}
