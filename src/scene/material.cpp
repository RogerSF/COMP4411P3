#include "ray.h"
#include "material.h"
#include "light.h"
#include <list>

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	// return kd;
	vec3f intersect_position = r.at(i.t);
	vec3f normal = i.N.normalize();
	vec3f result = ke + ka;

	for(Scene::cliter light_it = scene->beginLights(); light_it != scene->endLights(); ++light_it)
	{
		Light* light = *light_it;
		const double distance_attenuation = light->distanceAttenuation(intersect_position);
		const vec3f shadow_attenuation = light->shadowAttenuation(intersect_position);
		vec3f attenuation = distance_attenuation * shadow_attenuation;
		vec3f light_direction = light->getDirection(intersect_position).normalize();

		// Phong diffuse reflection equation
		// kd * lambertian (angle between normal and light direction)
		vec3f diffuse_reflection = kd * maximum(normal.dot(light_direction), 0);

		// Negate the reflected light vector to get vector pointing out the surface
		// This is because the origin light ray points towards the surface
		// and the immediate reflection of this will result in a ray pointing towards the surface too
		vec3f reflected_light = (light_direction - 2 * (-(light_direction).dot(normal)) * normal).normalize();
		// Negate the ray vector to get vector pointing to the ray origin
		vec3f view_vector = r.getDirection().normalize();

		// Phong specular reflection equation
		vec3f specular_reflection = ks * pow(maximum(-reflected_light.dot(view_vector), 0), shininess*128);

		result += prod(attenuation, (diffuse_reflection + specular_reflection));
	}

	result = result.clamp();

	return result;
}
