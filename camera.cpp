#include "camera.h"

/**
* Initialise the camera
*/
Camera::Camera() {
	_updateMutex = CreateMutex(NULL, FALSE, NULL);
	_runningMutex = CreateMutex(NULL, TRUE, NULL);
	_connected = false;
	_frame = NULL;
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
	CvCapture *capture;
	capture = cvCaptureFromCAM( 0 );

	if(capture == 0x00)
		return;

	camera->writer = cvCreateVideoWriter("C:/video/rccontrol.mpeg", CV_FOURCC('P', 'I', 'M', '1'), 24, cvSize(320,240));

	int frameNo = 0;

	WaitForSingleObject(camera->_updateMutex, INFINITE);
	camera->_connected = true;
	ReleaseMutex(camera->_updateMutex);

	for(;;) {
		if( WaitForSingleObject(runningMutex, 0) == WAIT_TIMEOUT ) {
			static IplImage *frame = NULL;
			cvGrabFrame(capture);
			frame = cvRetrieveFrame( capture );
			if(frame != NULL) {
				cvWriteFrame(camera->writer, frame);
				WaitForSingleObject(camera->_updateMutex, INFINITE);
				camera->_frame = frame;
				ReleaseMutex(camera->_updateMutex);
			}
			Sleep(20);
		} else {
			cvReleaseCapture(&capture);
			cvReleaseVideoWriter(&camera->writer);
			camera->_connected = false;
			_endthread();
		}
	}
}