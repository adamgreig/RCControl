#include "camera.h"

/**
* Initialise the camera
*/
Camera::Camera() {
	_updateMutex = CreateMutex(NULL, FALSE, NULL);
	_runningMutex = CreateMutex(NULL, TRUE, NULL);
	_connected = false;
	this->_thread = _beginthread(camera_thread, 0, this);

	if( !this->_thread ) {
		cout << "Error starting camera thread. Exiting." << endl;
		exit(1);
	}
}

Camera::~Camera() {
	ReleaseMutex(_runningMutex);
	cvReleaseCapture( &capture );
	cvWaitKey();
}

bool Camera::connected() {
	WaitForSingleObject(_updateMutex, INFINITE);
	bool result = _connected;
	ReleaseMutex(_updateMutex);
	return _connected;
}

void Camera::release() {
	ReleaseMutex(_runningMutex);
}

IplImage* Camera::get_frame() {
	WaitForSingleObject(_updateMutex, INFINITE);
	IplImage* result = _frame;
	ReleaseMutex(_updateMutex);
	return result;
}

void camera_thread(void* void_camera) {
	Camera* camera = (Camera*)void_camera;
	HANDLE runningMutex = camera->_runningMutex;

	camera->capture = 0;
	camera->capture = cvCaptureFromCAM( 0 );

	if(camera->capture == 0x00)
		return;

	int frameNo = 0;

	WaitForSingleObject(camera->_updateMutex, INFINITE);
	camera->_connected = true;
	ReleaseMutex(camera->_updateMutex);

	for(;;) {
		if( WaitForSingleObject(runningMutex, 0) == WAIT_TIMEOUT ) {
			static IplImage *frame = NULL;
			cvGrabFrame(camera->capture);
			frame = cvRetrieveFrame( camera->capture );
			WaitForSingleObject(camera->_updateMutex, INFINITE);
			camera->_frame = frame;
			ReleaseMutex(camera->_updateMutex);
			Sleep(20);
		} else {
			cvReleaseCapture(&camera->capture);
			camera->_connected = false;
			_endthread();
		}
	}
}