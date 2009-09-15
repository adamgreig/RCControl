#pragma once
#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <cvcam.h>
#include <highgui.h>

/**
* Captures images from a webcam
*/
class Camera {
    public:
        Camera();
		~Camera();
		IplImage* get_frame();
		bool connected();
		void release();

	private:
		IplImage* _frame;
		CvCapture* capture;
		HANDLE _updateMutex;
		HANDLE _runningMutex;
		uintptr_t _thread;
		bool _connected;

	friend void camera_thread(void*);
};

void camera_thread(void* void_camera);