/* 
** ARToolKit object parsing function 
**   - reads in object data from object file in Data/object_data
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AR/ar.h>
#include <AR/arvrml.h>
#include "vrml_object.hpp"

static char *get_buff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(ret); // Skip blank lines and comments.
    }
}

ObjectVRML_T *read_VRMLdata( char *name, int *objectnum )
{
    FILE          *fp;
    ObjectVRML_T  *object;
    char           buf[256], buf1[256];
    int            i;

	printf("Opening model file %s\n", name);

    if ((fp=fopen(name, "r")) == NULL) return(0);

    get_buff(buf, 256, fp);
    if (sscanf(buf, "%d", objectnum) != 1) {
		fclose(fp); return(0);
	}

	printf("About to load %d models.\n", *objectnum);

    if ((object = (ObjectVRML_T *)malloc(sizeof(ObjectVRML_T) * (*objectnum))) == NULL) exit (-1);

    for (i = 0; i < *objectnum; i++) {
		
        get_buff(buf, 256, fp);
        if (sscanf(buf, "%s %s", buf1, object[i].name) != 2) {
            fclose(fp); free(object); return(0);
        }
		
        if (strcmp(buf1, "VRML") == 0) {
			printf("Model %d: %20s\n", i + 1, &(object[i].name[0]));
            object[i].vrml_id = arVrmlLoadFile(object[i].name);
			printf("VRML id - %d \n", object[i].vrml_id);
            if (object[i].vrml_id < 0) {
                fclose(fp); free(object); return(0);
            }
			get_buff(buf, 256, fp);
			if (sscanf(buf, "%s %s", buf1, object[i].place) != 2) {
		        fclose(fp); free(object); return(0);
			}

        } else {
			printf("Model %d: No VRML\n", i + 1);
			object[i].vrml_id = -1;
			if (strcmp(buf1, "PLACE") == 0) {
				strcpy(object[i].place, object[i].name); 
			}
        
		}
	
		get_buff(buf, 256, fp);
        if (sscanf(buf, "%s", buf1) != 1) {
			fclose(fp); free(object); return(0);
		}

		object[i].vrml_id_orig = object[i].vrml_id;
		object[i].visible = 0;
		object[i].matched = 0;
        
		printf("Loading patt %s\n", buf1);
        if ((object[i].id = arLoadPatt(buf1)) < 0) {
			fclose(fp); free(object); return(0);
		}

        get_buff(buf, 256, fp);
        if (sscanf(buf, "%lf", &object[i].marker_width) != 1) {
			fclose(fp); free(object); return(0);
		}

        get_buff(buf, 256, fp);
        if (sscanf(buf, "%lf %lf", &object[i].marker_center[0], &object[i].marker_center[1]) != 2) {
            fclose(fp); free(object); return(0);
        }

		printf("Patt id:%d\n", object[i].id);
		printf("Place: %s\n\n", object[i].place);
        
    }

    fclose(fp);

    return( object );
}
