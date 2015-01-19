#ifndef MONUMENTOS_Y_CIUDADES_GAME_HPP_
#define MONUMENTOS_Y_CIUDADES_GAME_HPP_

#ifdef _WIN32
#  include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __APPLE__
#  include <GL/glut.h>
#else
#  include <GLUT/glut.h>
#endif

#include <AR/config.h>
#include <AR/gsub.h>
#include <AR/gsub_lite.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/arvrml.h>
#include <AR/video.h>

#include "object.hpp"
#include "vrml_object.hpp"

#define COLLIDE_DIST 100000.0

#include "User.hpp"


class Game {
public:
	Game(void);
	~Game(void);
	void start(int argc, char **argv);
private:
	/*Functions*/
	static void mainLoop();
	static void init();
	static void keyEvent(unsigned char key, int x, int y);
	static void cleanup(void);
	static int checkCollisions( ObjectVRML_T object0, ObjectVRML_T object1, float collide_dist);
	static int draw( ObjectData_T *object, int objectnum );
	static int draw_object( int obj_id, double gl_para[16], int collide_flag );
	static bool setupCamera(const char *cparam_name, char *vconf, ARParam *cparam);
	static bool setupMarkersObjects(char *objectDataFilename, 
									ObjectVRML_T **objectDataRef,
									int *objectDataCountRef);
	static void Visibility(int visible);
	static void Reshape(int w, int h);
	static void Display(void);
	static void checkCollidingMarkers(ObjectVRML_T *objects, int size);
	static ARUint8 *getAndDisplayVideo();
	

	/*Static members*/
	static char *vconf;
	static char *cparam_name;
	static ARParam cparam;
	static ARParam gARTCparam;
	static ARGL_CONTEXT_SETTINGS_REF gArglSettings;
	static int xsize;
	static int ysize;	
	static int thresh;
	static int count;
	static char *model_name;
	static char *vrml_model_name;
	static ObjectData_T *object;
	static ObjectVRML_T *gObjectData;
	static int objectnum;
	static int gObjectDataCount;
	static ARUint8 *gARTImage;


	static User user;
};

#endif