// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"

extern TraceUI* traceUI;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		// Recursive ray-trace the secondary rays as follows:
		// 1. Shadow ray to light source(s)
		// 2. Reflected ray
		// 3. Refracted ray
		vec3f final_intensity;

		const Material& m = i.getMaterial();
		final_intensity += m.shade(scene, r, i);
		
		if (depth >= traceUI->getDepth())
		{
			return final_intensity;
		}

		vec3f intersect_position = r.at(i.t);
		vec3f normal = i.N.normalize();
		vec3f ray_direction = r.getDirection().normalize();

		// Negate the reflected light vector to get vector pointing out the surface
		// This is because the origin light ray points towards the surface
		// and the immediate reflection of this will result in a ray pointing towards the surface too
		vec3f reflected_ray_direction = (ray_direction - 2 * ((ray_direction).dot(normal)) * normal).normalize();

		// Reflected ray tracing
		if(!i.getMaterial().kr.iszero()) {
			ray reflected_ray = ray(intersect_position, reflected_ray_direction);
			final_intensity += prod(i.getMaterial().kr, traceRay(scene, reflected_ray, prod(thresh, i.getMaterial().kr), depth + 1));
		}

		// Refracted ray tracing
		if (!i.getMaterial().kt.iszero()) {
			double index1, index2;
			bool isLeavingObject = material_stack.empty() ? false : &i.getMaterial() == material_stack.front();

			if(isLeavingObject)
			{
				// Leaving object
				index1 = i.getMaterial().index;
				index2 = material_stack.size() <= 1 ? 1.0 : material_stack[1]->index;
				material_stack.pop_front();
			} else
			{
				// Entering object
				index1 = material_stack.empty() ? 1.0 : material_stack.front()->index;
				index2 = i.getMaterial().index;
				material_stack.push_front(&i.getMaterial());
			}

			double ratio_of_refraction = index1 / index2;
			double cos_incident_angle = maximum(1, minimum(-1, normal.dot(-ray_direction))); // Invert the ray direction vector to compute angle between vectors, clamp between -1, 1
			double sin_incident_angle = sqrt(1 - pow(cos_incident_angle, 2)); // sin cos identity
			double sin_refracted_angle = sin_incident_angle * ratio_of_refraction;

			if(sin_refracted_angle > 1.0)
			{
				// tir
				return vec3f();
			} else
			{
				double cos_refracted_angle = sqrt(1 - pow(sin_refracted_angle, 2));
				vec3f refracted_ray_direction = (ratio_of_refraction * cos_refracted_angle - cos_refracted_angle)*-normal - (ratio_of_refraction * -ray_direction);
				ray refracted_ray = ray(intersect_position, refracted_ray_direction);

				final_intensity += prod(i.getMaterial().kt, traceRay(scene, refracted_ray, prod(thresh, i.getMaterial().kt), depth + 1));
			}
		}

		final_intensity = final_intensity.clamp();
		return final_intensity;
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}