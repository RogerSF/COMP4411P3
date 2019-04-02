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

	vec3f specularSum;

	for(Scene::cliter light_it = scene->beginLights(); light_it != scene->endLights(); ++light_it)
	{
		Light* light = *light_it;
		vec3f light_direction = light->getDirection(r.getPosition()).normalize();
		vec3f normal = i.N.normalize();

		// kd * lambertian
		vec3f diffuse_reflection = kd * maximum(normal.dot(-light_direction), 0);

		vec3f reflected_light = light_direction - 2 * ((light_direction).dot(normal)) * normal;
		vec3f view_vector = r.getDirection().normalize();

		vec3f specular_reflection = ks * pow(maximum(reflected_light.dot(view_vector), 0), shininess);
		const double distance_attenuation = light->distanceAttenuation(normal);

		specularSum += distance_attenuation * (diffuse_reflection + specular_reflection);
	}
	return ke + ka + specularSum;
}
