#include "gui.h"

bool GUI::camera_active;
bool GUI::wiimote_active;
bool GUI::servo_active;
bool GUI::gps_active;
bool GUI::mftech_active;

bool GUI::start_pressed;
bool GUI::stop_pressed;

bool GUI::close;

GLuint GUI::background_texture;

GUI::GUI() {
	this->_thread = _beginthread(gui_thread, 0, this);

	if( !this->_thread ) {
		cout << "Error starting GUI thread. Exiting." << endl;
		exit(1);
	}
}

void gui_thread(void* void_gui) {
	GUI* gui = (GUI*)void_gui;
	int argc = 0;
	char* argv;
	glutInit(&argc, &argv);

	GUI::camera_active = false;
	GUI::wiimote_active = false;
	GUI::servo_active = false;
	GUI::gps_active = false;
	GUI::mftech_active = false;

	GUI::start_pressed = false;
	GUI::stop_pressed = false;

	GUI::close = false;

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("RCControl");
	//glutFullScreen();

	glutKeyboardFunc(GUI::gl_key);
	glutReshapeFunc(GUI::gl_reshape);
	glutDisplayFunc(GUI::gl_display);
	glutIdleFunc(GUI::gl_display);
	glutMouseFunc(GUI::gl_mouse);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1024, 600, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);

	GUI::background_texture = gui->load_texture_raw("C:/background.raw");

	glutMainLoop();
}

GUI::~GUI() {
}

void GUI::gl_display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, background_texture );
	glBegin( GL_QUADS );
	glTexCoord2d(0.0, 0.0); glVertex2d(0.0,0.0);
	glTexCoord2d(1.0,0.0); glVertex2d(1024.0,0.0);
	glTexCoord2d(1.0,1.0); glVertex2d(1024.0,600.0);
	glTexCoord2d(0.0,1.0); glVertex2d(0.0,600.0);
	glEnd();
	glDisable( GL_TEXTURE_2D );

	if( !GUI::camera_active ) {
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_QUADS);
		glVertex2f(541.0, 126.0);
		glVertex2f(841.0, 126.0);
		glVertex2f(841.0, 196.0);
		glVertex2f(541.0, 196.0);
		glEnd();
	}
	if( !GUI::wiimote_active ) {
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_QUADS);
		glVertex2f(541.0, 196.0);
		glVertex2f(841.0, 196.0);
		glVertex2f(841.0, 262.0);
		glVertex2f(541.0, 262.0);
		glEnd();
	}
	if( !GUI::servo_active ) {
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_QUADS);
		glVertex2f(541.0, 262.0);
		glVertex2f(841.0, 262.0);
		glVertex2f(841.0, 328.0);
		glVertex2f(541.0, 328.0);
		glEnd();
	}
	if( !GUI::gps_active ) {
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_QUADS);
		glVertex2f(541.0, 328.0);
		glVertex2f(841.0, 328.0);
		glVertex2f(841.0, 426.0);
		glVertex2f(541.0, 426.0);
		glEnd();
	}
	if( !GUI::mftech_active ) {
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_QUADS);
		glVertex2f(541.0, 426.0);
		glVertex2f(841.0, 426.0);
		glVertex2f(841.0, 520.0);
		glVertex2f(541.0, 520.0);
		glEnd();
	}

	if( GUI::start_pressed ) {
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(42.0, 116.0);
		glVertex2f(481.0, 116.0);
		glVertex2f(481.0, 278.0);
		glVertex2f(42.0, 278.0);
		glEnd();
	}
	if( GUI::stop_pressed ) {
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(42.0, 308.0);
		glVertex2f(481.0, 308.0);
		glVertex2f(481.0, 475.0);
		glVertex2f(42.0, 475.0);
		glEnd();
	}

	glutSwapBuffers();
}

void GUI::gl_reshape(int w, int h) {
}



void GUI::gl_key(unsigned char key, int x, int y) {
	switch (key) {
		case ESCAPE:
			break;
	}
}

void GUI::gl_mouse(int button, int state, int x, int y) {
	if( button == GLUT_LEFT_BUTTON ) {
		if( state == GLUT_DOWN ) {
			if( x >= 42 && x <= 480 && y >= 116 && y <= 277 ) {
				GUI::start_pressed = true;
			} else if( x >= 42 && x <= 480 && y >= 308 && y <= 474 ) {
				GUI::stop_pressed = true;
			} else if( x >= 895 && x <= 1010 && y >= 470 && y <= 587 ) {
				GUI::close = true;
			}
		} else {
			GUI::start_pressed = false;
			GUI::stop_pressed = false;
		}
	}
}

GLuint GUI::load_texture_raw(char* filename) {

    GLuint texture;
    int width, height;
    void * data;
    FILE * file;

    // open texture data
    fopen_s( &file, filename, "rb" );
    if ( file == NULL ) return 0;

    // allocate buffer
    width = 1024;
    height = 600;
    data = malloc( width * height * 3 );

    // read texture data
    fread( data, width * height * 3, 1, file );
    fclose( file );

    // allocate a texture name
    glGenTextures( 1, &texture );

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     GL_CLAMP );

    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                       GL_RGB, GL_UNSIGNED_BYTE, data );

    // free buffer
    free( data );

    return texture;
}

void GUI::set_camera(bool status) {
	GUI::camera_active = status;
}

void GUI::set_wiimote(bool status) {
	GUI::wiimote_active = status;
}

void GUI::set_servo(bool status) {
	GUI::servo_active = status;
}

void GUI::set_gps(bool status) {
	GUI::gps_active = status;
}

void GUI::set_mftech(bool status) {
		GUI::mftech_active = status;
}