#pragma once
#include "stdafx.h"

#include <gl/glut.h>

#define ESCAPE 27
#define HEIGHT 600
#define WIDTH 1024

/**
* Captures images from a webcam
*/
class GUI {
    public:
        GUI();
		~GUI();
		static void gl_key(unsigned char key, int x, int y);
		static void gl_reshape(int w, int h);
		static void gl_display();
		static void gl_mouse(int button, int state, int x, int y);
		static GLuint load_texture_raw(char* filename);

		void set_camera(bool status);
		void set_wiimote(bool status);
		void set_servo(bool status);
		void set_gps(bool status);
		void set_mftech(bool status);

		static bool close;

	private:
		uintptr_t _thread;

		static bool camera_active;
		static bool wiimote_active;
		static bool servo_active;
		static bool gps_active;
		static bool mftech_active;

		static bool start_pressed;
		static bool stop_pressed;

		static GLuint background_texture;
		
	friend void gui_thread(void*);
};

void gui_thread(void* void_gui);