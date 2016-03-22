
/* Derived from scene.c in the The OpenGL Programming Guide */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// #include <GL/gl.h>
// #include <GL/glu.h>
// #include <GL/glx.h>
// #include <GL/glut.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

	/* flags used to control the appearance of the image */
int lineDrawing = 0;	// draw polygons as solid or lines
int lighting = 1;	// use diffuse and specular lighting
int smoothShading = 1;  // smooth or flat shading
int textures = 1;
float spin = 0, lightHeight = 5;
int textureFile = 0;

int width, height, depth, maxDepth = 0, lButtonPressed = 0, rButtonPressed = 0;
float camX, camY, camZ;
float lightX, lightY, lightZ;

struct rgbColour {
	float red, green, blue;
};
typedef struct rgbColour rgb;

struct Circles {
	float x, y, z;
	float radius;
	struct rgbColour * colours;
	struct Circles * nextCir;
};
typedef struct Circles Circle;

struct lights {
	float x, y, z;
	struct rgbColour * colours;
};
typedef struct lights Light;

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 10;
float deltaMove = 0;

GLubyte  *Image[7];
GLuint   textureID[7];

Circle * head;
Light * lightPoint;


void lightRotation(void) {
	spin = spin + 1; /*MAC LINE */
	//spin = spin + 0.1; //Ubuntu line

	if(spin >= 360) {
		spin = spin - 360;
	}

	lightX = cos((spin * M_PI)/180) * 4;
	lightY = 3;
	lightZ = sin((spin * M_PI)/180) * 4;
	//printf("%f, %f, %f\n",lightX, lightY, lightZ);
	glutPostRedisplay();
}

/*  Initialize material property and light source.
 */
void init (void)
{
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_full_on[] = {1.0, 1.0, 1.0, 1.0};

	GLfloat light_position[] = { 0.5, 1.0, 1.0, 0.0 };

	/* if lighting is turned on then use ambient, diffuse and specular
		lights, otherwise use ambient lighting only */
	if (lighting == 1) {
		glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
	} else {
		glLightfv (GL_LIGHT0, GL_AMBIENT, light_full_on);
		glLightfv (GL_LIGHT0, GL_DIFFUSE, light_full_off);
		glLightfv (GL_LIGHT0, GL_SPECULAR, light_full_off);
	}
	glLightfv (GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void display (void)
{
GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_gray[] = {0.8, 0.8, 0.8, 0.8};
GLfloat brown[] = {0.5, 0.35, 0.05, 0.35};
GLfloat random[] = {1.0, 1.0, 1.0, 1.0};
GLfloat position[] = { lightX, lightY, lightZ, 1.0 };
GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };

float bottomThirdLimit, topThirdLimit;
float randomR, randomG, randomB;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw surfaces as either smooth or flat shaded */
	if (smoothShading == 1)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	/* draw polygons as either solid or outlines */
	if (lineDrawing == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, light_gray);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_gray);
	//draw plane

	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_full_off);

	glBegin(GL_TRIANGLES);
		glVertex3f( 0,-0.5, 0);
		glVertex3f( 0,-0.5,10);
		glVertex3f(10,-0.5,10);

		glVertex3f( 0,-0.5, 0);
		glVertex3f(10,-0.5,10);
		glVertex3f(10,-0.5, 0);
	glEnd();

	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);

	/* turn texturing on */
	if (textures == 1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID[textureFile]); //change texture
		if(textureFile == 7){
			textureFile = 0;
		}
	/* if textured, then use white as base colour */
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, light_gray);
	}

	glPushMatrix();

	glTranslatef(5,0.5,5);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	

	if (textures == 1){
		glDisable(GL_TEXTURE_2D);
	}
	
	glLightfv (GL_LIGHT0, GL_POSITION, position);

	glTranslated (lightX, lightY, lightZ);
	glutIdleFunc(lightRotation);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glutWireSphere (0.1, 15, 15);
	glEnable (GL_LIGHTING);

	glPopMatrix ();

	glPopMatrix ();
	glFlush ();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	camX = 10 * sin(deltaAngle * (M_PI/180)) + 5;
	camY = 10;
	camZ = 10 * cos(deltaAngle * (M_PI/180)) + 5;

	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 1000.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	// Set the camera
	gluLookAt(camX, camY, camZ,    // Look at point
			5, 0, 5,
			0, 1, 0); 
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
		case 'q':
			exit(0);
			break;
		case '1':		// texture with  smooth shading
			lineDrawing = 0;
			lighting = 1;
			smoothShading = 1;
			textures = 1;
			textureFile++;
			init();
			display();
			break;
		case '2':		// draw polygons as outlines
			lineDrawing = 1;
			lighting = 0;
			smoothShading = 0;
			textures = 0;
			init();
			display();
			break;
		case '3':		// draw polygons as filled
			lineDrawing = 0;
			lighting = 0;
			smoothShading = 0;
			textures = 0;
			init();
			display();
			break;
		case '4':		// diffuse and specular lighting, smooth shading
			lineDrawing = 0;
			lighting = 1;
			smoothShading = 1;
			textures = 0;
			init();
			display();
			break;
		
		case 'w':		// draw polygons as outlines
			gluLookAt(5, 5, 5,    // Look at point
					  0, -20, 0,
					  0, 1, 0); 
			init();
			display();
			break;
		case 'd':		// draw polygons as outlines
			glRotatef(5, 0.0, 0.5, 0.0);
			init();
			display();
			break;
		case 'a':		// draw polygons as outlines
			glRotatef(5, 1.0, -1.0, -1.0);
			init();
			display();
			break;
		case 's':		// draw polygons as outlines
			glTranslatef(0.0, 0.0,-0.5);
			init();
			display();
			break;
		case 'f':		// draw polygons as outlines
			glTranslatef(0.0, -1.0, 0.0);
			init();
			display();
			break;
		case 'v':		// draw polygons as outlines
			glTranslatef(0.0, 1.0,0.0);
			init();
			display();
			break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //left button and pressed
		lButtonPressed = x;
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		lButtonPressed = -1;
	}

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { //right button and pressed
		rButtonPressed = y;
	}
	else {
		rButtonPressed = -1;
	}
}

void motion(int x, int y) {
	if(lButtonPressed >= 0){

		deltaAngle += 3;
		camX = 10 * sin(deltaAngle * (M_PI/180)) + 5;
		camY = 10;
		camZ = 10 * cos(deltaAngle * (M_PI/180)) + 5;
		

		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();

		gluLookAt(camX, camY, camZ,    // Look at point
			5, 0, 5,
			0, 1, 0); 
		display ();
	}

	// else if(rButtonPressed >= 1){
	// 	glMatrixMode (GL_MODELVIEW);
	// 	glLoadIdentity ();

	// 	gluLookAt(x, y, 0,    // Look at point
	// 		5, 0, 5,
	// 		0.0, 1.0, 0.0);   // Up vector
	// 	display ();

	// }
}

/* read data file and store in arrays */
void readFile(char **argv) {
FILE *fp;
char instr[1024];
char * buffer;
int readLight = 0, x, z = 0, convertedNum, front = 0;
Circle * curr, * node;


	/* open file and print error message if name incorrect */
	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("ERROR, could not open file.\n");
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	/* the code to read the input file goes here */
	/* numlevels is set to the number of lines in the file not including the first comment line */
	else {
		while (fgets(instr, sizeof(instr), fp)) {

			buffer = strtok(instr, "  ");

			if (instr[0] == '#' || instr[0] == 'P') {
				//skip reading in line
			}

			else if (strcmp("light", buffer) == 0){
				//light
				if(readLight == 1){
					printf("ERROR. SECOND LIGHT FOUND. IGNORING\n");
				}

				else {
					printf("Light found\n");
					lightPoint = (Light *)malloc(sizeof(Light));
					lightPoint->colours = (rgb *)malloc(sizeof(rgb));

					for (x = 0; x < 6; x++)	{
						buffer = strtok(NULL, "  ");
						switch(x) {
							case 0: //x
								lightPoint->x = atof(buffer);
								break;
							case 1: //y
								lightPoint->y = atof(buffer);
								break;
							case 2: //z
								lightPoint->z = atof(buffer);
								break;
							case 3: //r
								lightPoint->colours->red = atof(buffer);
								break;
							case 4: //g
								lightPoint->colours->green = atof(buffer);
								break;
							case 5: //b
								lightPoint->colours->blue = atof(buffer);
								break;
						}//end switch
					}//end for
					readLight = 1;
				}
			}//end light read
			else if (strcmp("sphere", buffer) == 0){
				//sphere
				printf("Circle found\n");
				node = (Circle *)malloc(sizeof(Circle));
				node->nextCir = NULL;

				for (x = 0; x < 7; x++)	{
					curr = (Circle *)malloc(sizeof(Circle));
					curr->nextCir = NULL;

					buffer = strtok(NULL, "  ");

					switch(x) {
						case 0: //x
							curr->x = atof(buffer);
							break;
						case 1: //y
							curr->y = atof(buffer);
							break;
						case 2: //z
							curr->z = atof(buffer);
							break;
						case 3: //R
							curr->radius = atof(buffer);
							break;
						case 4: //r
							lightPoint->colours->red = atof(buffer);
							break;
						case 5: //g
							lightPoint->colours->green = atof(buffer);
							break;
						case 6:
							lightPoint->colours->blue = atof(buffer);
							break;
					}//end switch
				}//end for

				if(front == 0){
					head = curr;
					node = curr;
					front = 1;
				}

				else {
					node->nextCir = curr;
					node = curr;
				}

			}
		}//ends if buffer = NULL
	}//end else	
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
	height = 768;
	width = 1024;
	head = NULL;
	lightPoint = NULL;
	readFile(argv);
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize (1024, 768);
	glutCreateWindow (argv[0]);
	init();
	//loadTexture();
	glutReshapeFunc (reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc (keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);


	glutMainLoop();
	return 0; 
}

