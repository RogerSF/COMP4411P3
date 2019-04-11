#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ), constant_attenuation_coeff(0.0), linear_attenuation_coeff(0.0), quadratic_attenuation_coeff(0.0) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	double constant_attenuation_coeff;
	double linear_attenuation_coeff;
	double quadratic_attenuation_coeff;

protected:
	vec3f position;
};

class SpotLight : public PointLight {
public:
	SpotLight(Scene* scene, const vec3f& pos, const vec3f& color, const double& ang, const vec3f& centralDir) :
		PointLight(scene, pos, color), angle(ang), centralDirection(centralDir.normalize()) {}
	virtual vec3f getColor(const vec3f& p) const;

protected:
	double angle;
	vec3f centralDirection;
};

//https://books.google.com.hk/books?id=rHuc4yQ7TDoC&pg=PA204&lpg=PA204&dq=Warn+Model+Light&source=bl&ots=hvzGRrCkwW&sig=ACfU3U3mFePEtdq0cWIakRHSeL9UwgD8QQ&hl=en&sa=X&ved=2ahUKEwik9dLHucfhAhWUE4gKHXo-Dj4Q6AEwBnoECAcQAQ#v=onepage&q=Warn%20Model%20Light&f=false
class WarnModelLight : public PointLight {
public:
	WarnModelLight(Scene* scene, const vec3f& pos, const vec3f& color, const vec3f& centralDir, double& specExp,
		double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) :
		PointLight(scene, pos, color), centralDirection(centralDir.normalize()), specularExponent(specExp),
		xflapmin(xmin), xflapmax(xmax), yflapmin(ymin), yflapmax(ymax), zflapmin(zmin), zflapmax(zmax) {}
	virtual vec3f getColor(const vec3f& p) const;
protected:
	vec3f centralDirection;
	double specularExponent;

	double xflapmin;
	double xflapmax;
	double yflapmin;
	double yflapmax;
	double zflapmin;
	double zflapmax;
};

#endif // __LIGHT_H__
