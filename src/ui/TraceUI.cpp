//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"
#include <iostream>

using namespace std;
static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
			pUI->raytracer->getScene()->setAttenConst(pUI->m_attenConst);
			pUI->raytracer->getScene()->setAttenLinear(pUI->m_attenLinear);
			pUI->raytracer->getScene()->setAttenQuad(pUI->m_attenQuad);

			pUI->raytracer->getScene()->setTexture(pUI->textureImg);
			pUI->raytracer->getScene()->setTextureWidth(pUI->textureWidth);
			pUI->raytracer->getScene()->setTextureHeight(pUI->textureHeight);
			pUI->raytracer->getScene()->setTextureMapping(pUI->m_enableTextureMapping);
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_load_background(Fl_Menu_ * o, void * v)
{
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("Load Background", "*.bmp", NULL);

	if (newfile) {
		unsigned char* data;
		int width, height;
		if ((data = readBMP(newfile, width, height)) == NULL)
		{
			fl_alert("Can't load bitmap file");
			return;
		}
		if (width != pUI->getSize() || height != pUI->getSize()) {
			fl_alert("size doesn't match");
			return;
		}

		pUI->backgroundImg = data;
		// cout << pUI->backgroundImg << endl;
		pUI->m_enableBackgroundButton->activate();
		pUI->raytracer->setBackgroundImg(data);
	}
}

void TraceUI::cb_load_texture(Fl_Menu_ * o, void * v)
{
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("Load Texture", "*.bmp", NULL);

	if (newfile) {
		unsigned char* data;
		int width, height;
		if ((data = readBMP(newfile, width, height)) == NULL)
		{
			fl_alert("Can't load bitmap file");
			return;
		}

		pUI->textureImg = data;
		pUI->m_enableTextureMappingButton->activate();
		//initialize or update the existing texture in the current scene, keep them synced
		if (pUI->raytracer->sceneLoaded()) {
			
			pUI->textureWidth = width;
			pUI->textureHeight = height;
			pUI->raytracer->getScene()->setTexture(pUI->textureImg);
			pUI->raytracer->getScene()->setTextureWidth(width);
			pUI->raytracer->getScene()->setTextureHeight(height);
		}
	}

}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
	
}

void TraceUI::cb_attenConstSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	((TraceUI*)(o->user_data()))->m_attenConst = float(((Fl_Slider *)o)->value());
	if (pUI->raytracer->getScene()) {
		pUI->raytracer->getScene()->setAttenConst(pUI->m_attenConst);
	}
	
}

void TraceUI::cb_attenLinearSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	((TraceUI*)(o->user_data()))->m_attenLinear = float(((Fl_Slider *)o)->value());
	if (pUI->raytracer->getScene()) {
		pUI->raytracer->getScene()->setAttenLinear(pUI->m_attenLinear);
	}
	
	
}

void TraceUI::cb_attenQuadSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	((TraceUI*)(o->user_data()))->m_attenQuad = float(((Fl_Slider *)o)->value());
	if (pUI->raytracer->getScene()) {
		pUI->raytracer->getScene()->setAttenQuad(pUI->m_attenQuad);
	}
}

void TraceUI::cb_enableBackground(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());

	pUI->m_enableBackground = bool(((Fl_Light_Button *)o)->value());
}

void TraceUI::cb_enableJittering(Fl_Widget * o, void * v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());

	pUI->m_enableJittering = bool(((Fl_Light_Button *)o)->value());
}

void TraceUI::cb_enableAntialiasing(Fl_Widget * o, void * v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());

	pUI->m_enableAntialiasing = bool(((Fl_Light_Button *)o)->value());
}

void TraceUI::cb_enableTextureMapping(Fl_Widget * o, void * v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_enableTextureMapping = bool(((Fl_Light_Button *)o)->value());
	//sync to current scene (if any)
	if (pUI->raytracer->sceneLoaded()) {
		pUI->raytracer->getScene()->setTextureMapping(pUI->m_enableTextureMapping);
	}
}

void TraceUI::cb_numSubPixelsSlides(Fl_Widget * o, void * v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->m_numSubPixels = int(((Fl_Slider *)o)->value());
}

void TraceUI::cb_ambientLightSlides(Fl_Widget * o, void * v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());
	pUI->ambientLight = double(((Fl_Slider *)o)->value());

	//sync this value to scene if any
	if (pUI->raytracer->sceneLoaded()) {
		pUI->raytracer->getScene()->ambientLight = vec3f(pUI->ambientLight, pUI->ambientLight, pUI->ambientLight);
	}
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

bool TraceUI::getEnableBackground()
{
	return m_enableBackground;
}

bool TraceUI::getEnableAntialiasing()
{
	return m_enableAntialiasing;
}

bool TraceUI::getEnableJittering()
{
	return this->m_enableJittering;
}

bool TraceUI::getEnableTextureMapping()
{
	return this->m_enableTextureMapping;
}

int TraceUI::getNumSubpixels()
{
	return this->m_numSubPixels;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Load Background...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_load_background },
		{ "&Load Texture...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_load_texture },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 3;
	m_nSize = 200;

	m_attenConst = 0.2;
	m_attenLinear = 0.1;
	m_attenQuad = 0.01;
	backgroundImg = NULL;
	m_enableBackground = false;
	m_enableAntialiasing = false;
	m_numSubPixels = 2;
	m_enableJittering = false;
	m_enableTextureMapping = false;
	textureImg = NULL;
	textureHeight = 0;
	textureWidth = 0;
	ambientLight = 1.0f;

	m_mainWindow = new Fl_Window(100, 40, 400, 500, "Ray <Not Loaded>");

		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 400, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		m_attenConstSlider = new Fl_Value_Slider(10, 80, 180, 20, "Attenuation, Constant");
		m_attenConstSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenConstSlider->type(FL_HOR_NICE_SLIDER);
		m_attenConstSlider->labelfont(FL_COURIER);
		m_attenConstSlider->labelsize(12);
		m_attenConstSlider->minimum(0);
		m_attenConstSlider->maximum(1);
		m_attenConstSlider->step(0.01);
		m_attenConstSlider->value(m_attenConst);
		m_attenConstSlider->align(FL_ALIGN_RIGHT);
		m_attenConstSlider->callback(cb_attenConstSlides);

		m_attenLinearSlider = new Fl_Value_Slider(10, 105, 180, 20, "Attenuation, Linear");
		m_attenLinearSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenLinearSlider->type(FL_HOR_NICE_SLIDER);
		m_attenLinearSlider->labelfont(FL_COURIER);
		m_attenLinearSlider->labelsize(12);
		m_attenLinearSlider->minimum(0);
		m_attenLinearSlider->maximum(1);
		m_attenLinearSlider->step(0.01);
		m_attenLinearSlider->value(m_attenLinear);
		m_attenLinearSlider->align(FL_ALIGN_RIGHT);
		m_attenLinearSlider->callback(cb_attenLinearSlides);

		m_attenQuadSlider = new Fl_Value_Slider(10, 130, 180, 20, "Attenuation, Quadratic");
		m_attenQuadSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attenQuadSlider->type(FL_HOR_NICE_SLIDER);
		m_attenQuadSlider->labelfont(FL_COURIER);
		m_attenQuadSlider->labelsize(12);
		m_attenQuadSlider->minimum(0);
		m_attenQuadSlider->maximum(1);
		m_attenQuadSlider->step(0.01);
		m_attenQuadSlider->value(m_attenQuad);
		m_attenQuadSlider->align(FL_ALIGN_RIGHT);
		m_attenQuadSlider->callback(cb_attenQuadSlides);

		//use background image or not
		m_enableBackgroundButton = new Fl_Light_Button(10, 155, 100, 25, "&Background");
		m_enableBackgroundButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_enableBackgroundButton->value(m_enableBackground);
		m_enableBackgroundButton->callback(cb_enableBackground);
		m_enableBackgroundButton->deactivate();


		//use antialiasing image or not
		m_enableAntialiasingButton = new Fl_Light_Button(115, 155, 100, 25, "&Antialiasing");
		m_enableAntialiasingButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_enableAntialiasingButton->value(m_enableAntialiasing);
		m_enableAntialiasingButton->callback(cb_enableAntialiasing);
		m_enableAntialiasingButton->activate();

		//use jittering or not
		m_enableJitteringButton = new Fl_Light_Button(220, 155, 100, 25, "&Jittering");
		m_enableJitteringButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_enableJitteringButton->value(m_enableJittering);
		m_enableJitteringButton->callback(cb_enableJittering);
		m_enableJitteringButton->activate();

		m_numSubPixelsSlider = new Fl_Value_Slider(10, 180, 180, 20, "# of subpixels");
		m_numSubPixelsSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_numSubPixelsSlider->type(FL_HOR_NICE_SLIDER);
		m_numSubPixelsSlider->labelfont(FL_COURIER);
		m_numSubPixelsSlider->labelsize(12);
		m_numSubPixelsSlider->minimum(2);
		m_numSubPixelsSlider->maximum(5);
		m_numSubPixelsSlider->step(1);
		m_numSubPixelsSlider->value(m_numSubPixels);
		m_numSubPixelsSlider->align(FL_ALIGN_RIGHT);
		m_numSubPixelsSlider->callback(cb_numSubPixelsSlides);

		//use texture mapping image or not
		m_enableTextureMappingButton = new Fl_Light_Button(10, 205, 100, 25, "&Texture");
		m_enableTextureMappingButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_enableTextureMappingButton->value(m_enableTextureMapping);
		m_enableTextureMappingButton->callback(cb_enableTextureMapping);
		m_enableTextureMappingButton->deactivate();

		// Ambient Light
		m_ambientLightSlider = new Fl_Value_Slider(10, 230, 180, 20, "Ambient Light");
		m_ambientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightSlider->labelfont(FL_COURIER);
		m_ambientLightSlider->labelsize(12);
		m_ambientLightSlider->minimum(0.00);
		m_ambientLightSlider->maximum(1.00);
		m_ambientLightSlider->step(0.01);
		m_ambientLightSlider->value(ambientLight);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_ambientLightSlides);



		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}