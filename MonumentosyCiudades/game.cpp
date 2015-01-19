#include "Game.hpp"
#include "Monument.hpp"

#define VIEW_SCALEFACTOR		1.0			// 1.0 ARToolKit unit becomes 1.0 of my OpenGL units.
#define VIEW_DISTANCE_MIN		10.0		// Objects closer to the camera than this will not be displayed.
#define VIEW_DISTANCE_MAX		10000.0		// Objects further away from the camera than this will not be displayed.

// Preferences.
static int prefWindowed = TRUE;
static int prefWidth = 640;					// Fullscreen mode width.
static int prefHeight = 480;				// Fullscreen mode height.
static int prefDepth = 32;					// Fullscreen mode bit depth.
static int prefRefresh = 0;					// Fullscreen mode refresh rate. Set to 0 to use default rate.

static long			gCallCountMarkerDetect = 0;

char *Game::model_name = "Data/object_data2";
char *Game::vrml_model_name = "Data/object_data_vrml";
ObjectData_T *Game::object;
ObjectVRML_T *Game::gObjectData;

ARUint8 *Game::gARTImage = NULL;

ARParam Game::gARTCparam;
ARGL_CONTEXT_SETTINGS_REF Game::gArglSettings = NULL;
int Game::objectnum;
int Game::gObjectDataCount;
int Game::xsize;
int Game::ysize;
int Game::thresh = 100;
int Game::count = 0;
User Game::user;

#ifdef _WIN32
char *Game::vconf = "Data\\WDM_camera_flipV.xml";
#else
char *Game::vconf = "";
#endif

char *Game::cparam_name = "Data/camera_para.dat";
ARParam Game::cparam;

static void debugReportMode(const ARGL_CONTEXT_SETTINGS_REF arglContextSettings);

Game::Game(void)
{
}


Game::~Game(void)
{
}

void Game::start(int argc, char **argv) {

	char glutGamemode[32];
	int i;
	//initialize applications
	glutInit(&argc, argv);

	if (!setupCamera(cparam_name, vconf, &gARTCparam)) {
		fprintf(stderr, "main(): Unable to set up AR camera.\n");
		exit(-1);
	}
    //init();

#ifdef _WIN32
	CoInitialize(NULL);
#endif
	// ----------------------------------------------------------------------------
	// Library setup.
	//

	// Set up GL context(s) for OpenGL to draw into.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	if (!prefWindowed) {
		if (prefRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", prefWidth, prefHeight, prefDepth, prefRefresh);
		else sprintf(glutGamemode, "%ix%i:%i", prefWidth, prefHeight, prefDepth);
		glutGameModeString(glutGamemode);
		glutEnterGameMode();
	} else {
		glutInitWindowSize(prefWidth, prefHeight);
		glutCreateWindow(argv[0]);
	}

	// Setup argl library for current context.
	if ((gArglSettings = arglSetupForCurrentContext()) == NULL) {
		fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
		cleanup();
		exit(-1);
	}
	debugReportMode(gArglSettings);
	glEnable(GL_DEPTH_TEST);
	arUtilTimerReset();

	if (!setupMarkersObjects(vrml_model_name, &gObjectData, &gObjectDataCount)) {
		fprintf(stderr, "main(): Unable to set up AR objects and markers.\n");
		cleanup();
		exit(-1);
	}
	
	// Test render all the VRML objects.
    fprintf(stdout, "Pre-rendering the VRML objects...");
	fflush(stdout);
    glEnable(GL_TEXTURE_2D);
    for (i = 0; i < gObjectDataCount; i++) {
		arVrmlDraw(gObjectData[i].vrml_id);
    }
    glDisable(GL_TEXTURE_2D);
	fprintf(stdout, " done\n");
	
	// Register GLUT event-handling callbacks.
	// NB: mainLoop() is registered by Visibility.
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutVisibilityFunc(Visibility);
	glutKeyboardFunc(keyEvent);
	
	glutMainLoop();

	//start the main event loop
    //argMainLoop( NULL, keyEvent, mainLoop );
}

void Game::mainLoop() {
	static int ms_prev;
	int ms;
	float s_elapsed;
	ARUint8 *image;

	ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
    int             marker_num;						// Count of number of markers detected.
    int             i, j, k;
	
	// Find out how long since mainLoop() last ran.
	ms = glutGet(GLUT_ELAPSED_TIME);
	s_elapsed = (float)(ms - ms_prev) * 0.001;
	if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
	ms_prev = ms;
	
	// Update drawing.
	arVrmlTimerUpdate();
	
	// Grab a video frame.
	if ((image = arVideoGetImage()) != NULL) {
		gARTImage = image;	// Save the fetched image.
		
		gCallCountMarkerDetect++; // Increment ARToolKit FPS counter.
		
		// Detect the markers in the video frame.
		if (arDetectMarker(gARTImage, thresh, &marker_info, &marker_num) < 0) {
			exit(-1);
		}
				
		// Check for object visibility.
		
		for (i = 0; i < gObjectDataCount; i++) {
		
			// Check through the marker_info array for highest confidence
			// visible marker matching our object's pattern.
			k = -1;
			for (j = 0; j < marker_num; j++) {
				if (marker_info[j].id == gObjectData[i].id) {
					if( k == -1 ) k = j; // First marker detected.
					else if (marker_info[k].cf < marker_info[j].cf) k = j; // Higher confidence marker detected.
				}
			}
			
			if (k != -1) {
				// Get the transformation between the marker and the real camera.
				//fprintf(stderr, "Saw object %d.\n", i);
				if (gObjectData[i].visible == 0) {
					arGetTransMat(&marker_info[k],
								  gObjectData[i].marker_center, gObjectData[i].marker_width,
								  gObjectData[i].trans);
				} else {
					arGetTransMatCont(&marker_info[k], gObjectData[i].trans,
									  gObjectData[i].marker_center, gObjectData[i].marker_width,
									  gObjectData[i].trans);
				}
				gObjectData[i].visible = 1;
			} else {
				gObjectData[i].visible = 0;
			}
		}

		checkCollidingMarkers(gObjectData, marker_num); 
		
		// Tell GLUT the display has changed.
		glutPostRedisplay();
	}
}

void Game::checkCollidingMarkers(ObjectVRML_T *objects, int size) {
	int i,j; 

	for (i = 0; i < size; ++i) {
		if (objects[i].visible == 0) continue;
		printf("Checking %s\n", objects[i].place);
		for (j = i + 1; j < size; ++j) {
			if (objects[j].visible == 0) continue;
			printf("Checking %s vs %s\n", objects[i].name, objects[j].name);
			if (checkCollisions(objects[i], objects[j], COLLIDE_DIST) &&
				Monument::match(objects[i].place, objects[j].place)) {

				printf("%s and %s matched\n", objects[i].place, objects[j].place);
				objects[i].matched = 1;
				objects[i].matched = 1;
			} else {
				printf("%s and %s didnt match\n", objects[i].place, objects[j].place);
				objects[i].matched = -1;
				objects[i].matched = -1;
			}
		}
	}
}

bool Game::setupMarkersObjects(char *objectDataFilename, 
									ObjectVRML_T **objectDataRef,
									int *objectDataCountRef) {

	// Load in the object data - trained markers and associated bitmap files.
    if ((*objectDataRef = read_VRMLdata(objectDataFilename, objectDataCountRef)) == NULL) {
        fprintf(stderr, "setupMarkersObjects(): read_VRMLdata returned error !!\n");
        return false;
    }
    printf("Object count = %d\n", *objectDataCountRef);

	return true;
}

/* check collision between two markers */
int Game::checkCollisions( ObjectVRML_T object0, ObjectVRML_T object1, float collide_dist)
{
	float x1,y1,z1;
	float x2,y2,z2;
	float dist;

	x1 = object0.trans[0][3];
	y1 = object0.trans[1][3];
	z1 = object0.trans[2][3];

	x2 = object1.trans[0][3];
	y2 = object1.trans[1][3];
	z2 = object1.trans[2][3];

	dist = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2);

	printf("Dist = %3.2f\n",dist);

	if(dist < collide_dist)
		return 1;
	else 
		return 0;
}
void Game::init() {
	ARParam  wparam;
	
	if (!setupCamera(cparam_name, vconf, &gARTCparam)) {
		fprintf(stderr, "main(): Unable to set up AR camera.\n");
		exit(-1);
	}
	/* load in the object data - trained markers and associated bitmap files */
    if( (object=read_ObjData(model_name, &objectnum)) == NULL ) exit(0);
    printf("Objectfile num = %d\n", objectnum);

    /* open the graphics window */
    //argInit( &cparam, 2.0, 0, 0, 0, 0 );
}
void Game::keyEvent(unsigned char key, int x, int y) {
	/* quit if the ESC key is pressed */
	if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
    }
}

void Game::cleanup(void)
{
	arVideoCapStop();
    arVideoClose();
    argCleanup();
}

/* draw the the AR objects */
int Game::draw( ObjectData_T *object, int objectnum )
{
    int     i;
    double  gl_para[16];
       
	/* OpenGL configuration for drawing objects*/
	glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);

    /* calculate the viewing parameters - gl_para */
    for( i = 0; i < objectnum; i++ ) {
        if( object[i].visible == 0 ) continue;
        argConvGlpara(object[i].trans, gl_para); /* Converts ARToolKit matrix to OpenGL format*/
        draw_object( object[i].id, gl_para, object[i].collide );
    }
     
	glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
	
    return(0);
}

/* draw the user object */
int Game::draw_object( int obj_id, double gl_para[16], int collide_flag )
{
    GLfloat   mat_ambient[]				= {0.0, 0.0, 1.0, 1.0};
	GLfloat   mat_ambient_collide[]     = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash[]				= {0.0, 0.0, 1.0, 1.0};
	GLfloat   mat_flash_collide[]       = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
 
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );

 	/* set the material */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	

	if(collide_flag){
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_collide);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_collide);
		/* draw a cube */
		glTranslatef( 0.0, 0.0, 30.0 );
		glutSolidSphere(30,12,6); // Here is drawn the sphere
	}
	else {
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		/* draw a cube */
		glTranslatef( 0.0, 0.0, 30.0 );
		glutSolidCube(60); // Here is drawn the cube
	}

    argDrawMode2D();

    return 0;
}


ARUint8 *Game::getAndDisplayVideo() {
	ARUint8         *dataPtr;

    /* grab a video frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return NULL;
    }
	
    if( count == 0 ) arUtilTimerReset();  
    count++;

	/*draw the video*/
    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

	/* capture the next video frame */
	arVideoCapNext();
	
	/* Specify color and line width */
	glColor3f( 1.0, 0.0, 0.0 );
	glLineWidth(6.0);

	return dataPtr;
}

bool Game::setupCamera(const char *cparam_name, char *vconf, ARParam *cparam)
{	
    ARParam			wparam;
	int				xsize, ysize;

    // Open the video path.
    if (arVideoOpen(vconf) < 0) {
    	fprintf(stderr, "setupCamera(): Unable to open connection to camera.\n");
    	return false;
	}
	
    // Find the size of the window.
    if (arVideoInqSize(&xsize, &ysize) < 0) return (FALSE);
    fprintf(stdout, "Camera image size (x,y) = (%d,%d)\n", xsize, ysize);
	
	// Load the camera parameters, resize for the window and init.
    if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		fprintf(stderr, "setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
        return false;
    }
    arParamChangeSize(&wparam, xsize, ysize, cparam);
    fprintf(stdout, "*** Camera Parameter ***\n");
    arParamDisp(cparam);
	
    arInitCparam(cparam);

	if (arVideoCapStart() != 0) {
    	fprintf(stderr, "setupCamera(): Unable to begin camera data capture.\n");
		return false;		
	}
	
	return true;
}

bool setupMarkersObjects(char *objectDataFilename, ObjectVRML_T **objectDataRef, int *objectDataCountRef)
{	
	// Load in the object data - trained markers and associated bitmap files.
    if ((*objectDataRef = read_VRMLdata(objectDataFilename, objectDataCountRef)) == NULL) {
        fprintf(stderr, "setupMarkersObjects(): read_VRMLdata returned error !!\n");
        return false;
    }
    printf("Object count = %d\n", *objectDataCountRef);

	return true;
}

//
//	This function is called on events when the visibility of the
//	GLUT window changes (including when it first becomes visible).
//
void Game::Visibility(int visible)
{
	if (visible == GLUT_VISIBLE) {
		printf("Visible\n");
		glutIdleFunc(mainLoop);
	} else {
		glutIdleFunc(NULL);
	}
}

//
//	This function is called when the
//	GLUT window is resized.
//
void Game::Reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Call through to anyone else who needs to know about window sizing here.
}

//
// This function is called when the window needs redrawing.
//
void Game::Display(void)
{
	int i, j;
    GLdouble p[16];
	GLdouble m[16];
	
	// Select correct buffer for this context.
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers for new frame.
	
	arglDispImage(gARTImage, &gARTCparam, 1.0, gArglSettings);	// zoom = 1.0.
	arVideoCapNext();
	gARTImage = NULL; // Image data is no longer valid after calling arVideoCapNext().
				
	// Projection transformation.
	arglCameraFrustumRH(&gARTCparam, VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(p);
	glMatrixMode(GL_MODELVIEW);
		
	// Viewing transformation.
	glLoadIdentity();
	// Lighting and geometry that moves with the camera should go here.
	// (I.e. must be specified before viewing transformations.)
	//none
	
	for (i = 0; i < gObjectDataCount; i++) {
	
		if ((gObjectData[i].visible != 0) && (gObjectData[i].vrml_id >= 0)) {
	
			// Calculate the camera position for the object and draw it.
			// Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).

			if (gObjectData[i].matched == 1) {
				arglCameraViewRH(gObjectData[i].trans, m, VIEW_SCALEFACTOR);
				glLoadMatrixd(m);

				// All lighting and geometry to be drawn relative to the marker goes here.
				//fprintf(stderr, "About to draw object %i\n", i);
				arVrmlDraw(gObjectData[i].vrml_id);
			}
		}			
	}
	
	// Any 2D overlays go here.
	//none
	
	glutSwapBuffers();
}

static void debugReportMode(const ARGL_CONTEXT_SETTINGS_REF arglContextSettings)
{
	if (arFittingMode == AR_FITTING_TO_INPUT) {
		fprintf(stderr, "FittingMode (Z): INPUT IMAGE\n");
	} else {
		fprintf(stderr, "FittingMode (Z): COMPENSATED IMAGE\n");
	}
	
	if (arImageProcMode == AR_IMAGE_PROC_IN_FULL) {
		fprintf(stderr, "ProcMode (X)   : FULL IMAGE\n");
	} else {
		fprintf(stderr, "ProcMode (X)   : HALF IMAGE\n");
	}
	
	if (arglDrawModeGet(arglContextSettings) == AR_DRAW_BY_GL_DRAW_PIXELS) {
		fprintf(stderr, "DrawMode (C)   : GL_DRAW_PIXELS\n");
	} else if (arglTexmapModeGet(arglContextSettings) == AR_DRAW_TEXTURE_FULL_IMAGE) {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (FULL RESOLUTION)\n");
	} else {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (HALF RESOLUTION)\n");
	}
		
	if (arTemplateMatchingMode == AR_TEMPLATE_MATCHING_COLOR) {
		fprintf(stderr, "TemplateMatchingMode (M)   : Color Template\n");
	} else {
		fprintf(stderr, "TemplateMatchingMode (M)   : BW Template\n");
	}
	
	if (arMatchingPCAMode == AR_MATCHING_WITHOUT_PCA) {
		fprintf(stderr, "MatchingPCAMode (P)   : Without PCA\n");
	} else {
		fprintf(stderr, "MatchingPCAMode (P)   : With PCA\n");
	}
}