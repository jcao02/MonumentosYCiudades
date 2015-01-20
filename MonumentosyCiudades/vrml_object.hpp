#ifndef __vrml_object_h__
#define __vrml_object_h__


#define   VRML_OBJECT_MAX       30

#ifdef __cplusplus
extern "C" {
#endif	

typedef struct {
    char       name[256];
    int        id;
    int        visible;
	int		   matched;
	char	   **failures;
    double     marker_coord[4][2];
    double     trans[3][4];
	int        vrml_id;
	int        vrml_id_orig;
    double     marker_width;
    double     marker_center[2];
	char       place[256];
	int		   last_failure;
} ObjectVRML_T;

ObjectVRML_T  *read_VRMLdata (char *name, int *objectnum);
int checkFailures(char **failures, char *place);

#ifdef __cplusplus
}
#endif	

#endif // __object_h__
