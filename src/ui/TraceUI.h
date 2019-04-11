//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser
#include "../fileio/bitmap.h"

#include "TraceGLWindow.h"
#include "../scene/light.h"
#include "../RayTracer.h"
class RayTracer;
class TraceGLWindow;

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_attenConstSlider;
	Fl_Slider*			m_attenLinearSlider;
	Fl_Slider*			m_attenQuadSlider;
	Fl_Slider*			m_numSubPixelsSlider;
	Fl_Slider*			m_ambientLightSlider;
	Fl_Slider*			m_adaptiveTerminationSlider;

	Fl_Light_Button*	m_enableBackgroundButton;
	Fl_Light_Button*	m_enableAntialiasingButton;
	Fl_Light_Button*	m_enableJitteringButton;
	Fl_Light_Button*	m_enableTextureMappingButton;
	

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	TraceGLWindow*	m_traceGlWindow;

	// member functions
	void show();

	void						setRayTracer(RayTracer *tracer);

	int						getSize();
	int						getDepth();

	void						setAttenConst(float atten) { this->m_attenConst = atten; this->m_attenConstSlider->value(atten); }
	void						setAttenLinear(float atten) { this->m_attenLinear = atten; this->m_attenLinearSlider->value(atten); }
	void						setAttenQuad(float atten) { this->m_attenQuad = atten; this->m_attenQuadSlider->value(atten); }
	void						setAdaptiveTermination(float ter) { this->adaptiveTermination = ter; this->m_adaptiveTerminationSlider->value(ter); }
	float						getAttenConst() { return this->m_attenConst;  }
	float						getAttenLinear() { return this->m_attenLinear; }
	float						getAttenQuad() { return this->m_attenQuad; }
	float						getAdaptiveTermination() { return this->adaptiveTermination; }

	unsigned char*		backgroundImg;	//for bonus
	unsigned char*		textureImg;	//for bonus
	int 						textureWidth;
	int 						textureHeight;

	bool						getEnableBackground();
	bool						getEnableAntialiasing();
	bool						getEnableJittering();
	bool						getEnableTextureMapping();

	int						getNumSubpixels();


private:
	RayTracer*			raytracer;

	int						m_nSize;
	int						m_nDepth;
	int 						m_numSubPixels;
	float						m_attenConst;
	float						m_attenLinear;
	float						m_attenQuad;
	float						ambientLight;
	float						adaptiveTermination;

	bool						m_enableBackground;
	bool						m_enableAntialiasing;
	bool						m_enableJittering;
	bool						m_enableTextureMapping;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_load_background(Fl_Menu_* o, void* v);
	static void cb_load_texture(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_attenConstSlides(Fl_Widget* o, void* v);
	static void cb_attenLinearSlides(Fl_Widget* o, void* v);
	static void cb_attenQuadSlides(Fl_Widget* o, void* v);
	static void cb_adaptiveTerminationSlides(Fl_Widget* o, void* v);

	static void cb_enableBackground(Fl_Widget* o, void* v);
	static void cb_enableJittering(Fl_Widget* o, void* v);
	static void cb_enableAntialiasing(Fl_Widget* o, void* v);
	static void cb_enableTextureMapping(Fl_Widget* o, void* v);
	static void cb_numSubPixelsSlides(Fl_Widget* o, void* v);
	static void cb_ambientLightSlides(Fl_Widget* o, void* v);


	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
