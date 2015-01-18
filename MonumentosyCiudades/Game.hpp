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
#include <AR/gsub.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/video.h>
#include "object.hpp"

#define COLLIDE_DIST 30000.0

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
	static int checkCollisions( ObjectData_T object0, ObjectData_T object1, float collide_dist);
	static int draw( ObjectData_T *object, int objectnum );
	static int draw_object( int obj_id, double gl_para[16], int collide_flag );

	static ARUint8 *getAndDisplayVideo();

	/*Static members*/
	static char *vconf;
	static char *cparam_name;
	static ARParam cparam;
	static int xsize;
	static int ysize;	
	static int thresh;
	static int count;
	static char *model_name;
	static ObjectData_T *object;
	static int objectnum;
	static User user;
};

#endif