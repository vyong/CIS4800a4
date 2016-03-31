
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

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define WIDTH 1024
#define HEIGHT 768
#define AMBIENT_K 0.5
#define DIFFUSE_K 0.5
#define SPECULAR_K 0.3

#define AMBIENT_R 0.4
#define AMBIENT_G 0.4
#define AMBIENT_B 0.4

/* flags used to control the appearance of the image */
int lineDrawing = 0;	// draw polygons as solid or lines
int lighting = 1;	// use diffuse and specular lighting
int smoothShading = 1;  // smooth or flat shading
int textures = 1;
float spin = 0, lightHeight = 5;
int textureFile = 0;

int width, height;

struct vectorStruct {
	float x, y, z;
};
typedef struct vectorStruct Vector;

struct rgbColour {
	float red, green, blue;
};
typedef struct rgbColour rgb;

struct Circles {
	float radius;
	struct vectorStruct * coord;
	struct rgbColour * colours;
	struct Circles * nextCir;
};
typedef struct Circles Circle;

struct lights {
	struct vectorStruct * coord;
	struct rgbColour * colours;
};
typedef struct lights Light;

struct rayStruct {
	struct vectorStruct * start;
	struct vectorStruct * direction;
};
typedef struct rayStruct Ray;

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 10;
float deltaMove = 0;
float t = 20000.0;

Circle * head;
Light * lightPoint;
Ray * ray;

unsigned char checkImage[3 * WIDTH * HEIGHT];

/* Subtract two vectors and return the resulting vector */
Vector * vectorSub(Vector *v1, Vector *v2){
	Vector * result;
	result = (Vector *)malloc(sizeof(Vector));
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
	result->z = v1->z - v2->z;

	return result;
}

/* Multiply two vectors and return the resulting scalar (dot product) */
float vectorDot(Vector * v1, Vector * v2){
	float result;
	result = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;

	return result;
}

Vector * vectorScale (float x, Vector * v) {
	Vector * result;
	result = (Vector *)malloc(sizeof(Vector));
	result->x = v->x * x;
	result->y = v->y * x;
	result->z = v->z * x;

	return result;
}

Vector * vectorAdd (Vector * v1, Vector * v2){
	Vector * result;
	result = (Vector *)malloc(sizeof(Vector));
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
	result->z = v1->z + v2->z;

	return result;
}


int rayIntersect(Ray * r, Circle * c, float *t){
	float A, B, C, discriminant, t0, t1, sqrtDiscriminant = 0;
	int result;
	Vector * dist;

	//	return 0;
	dist = (Vector *)malloc(sizeof(Vector));
	
	A = vectorDot(r->direction, r->direction); 

	dist = vectorSub(r->start, c->coord);

	B = 2 * vectorDot(r->direction, dist);

	C = vectorDot(dist, dist) - (c->radius * c->radius);
	
	/* Solving the discriminant */
	discriminant = B * B - 4 * A * C;
	
	/* If the discriminant is negative, there are no real roots.
	 * Return false in that case as the ray misses the sphere.
	 * Return true in all other cases (can be one or two intersections)
	 */
	if(discriminant < 0){
		result = 0;
	}

	else{
		sqrtDiscriminant = sqrtf(sqrtDiscriminant);
		t0 = (-B + sqrtDiscriminant)/2;
		t1 = (-B - sqrtDiscriminant)/2;

		if(t0 > t1){
			t0 = t1;
		}

		if((t0 > 0.001f) && (t0 < *t)) {
			*t = t0;
			result = 1;
		}
		else {
			result = 0;
		}
	}

	//printf("Result: %i\n", result);
	free(dist);
	return result;
}

void calculatePixel (void){
	int y, x, hit = 0, inShadow = 0;
	float temp, lambert, red, green, blue, redIllumination, greenIllumination, blueIllumination;
	Circle * node, * intersectedCircle;
	Vector  *scaled, *newStart, *normal, *distance, *tmp;
	Ray * lightRay;


	for(y = 0; y < 768; y++){
		for(x = 0; x < 1024; x++){
			node = head;

			redIllumination = 0;
			greenIllumination = 0;
			blueIllumination = 0;

			ray->start->y = y;
			ray->start->x = x;
			ray->start->z = -2000;

			ray->direction->x = 0;
			ray->direction->y = 0;
			ray->direction->z = 1;

			//while(coef > 0.0){
				//find intersection
			while(node != NULL){
				t = 20000.0;
				hit = rayIntersect(ray, node, &t);


				if(hit == 1){
					intersectedCircle = node;
					//printf("AAAAAAAA\n");
					break;
				}
				else{
					//printf("Count: %d\n", count);
					node = node->nextCir;
				}
			}

			if(hit == 1){
				//printf("point reached\n");
				scaled = vectorScale(t, ray->direction);
				newStart = vectorAdd(ray->start, scaled);

				//find normal for new vector
				normal = vectorSub(newStart, intersectedCircle->coord);
				temp = vectorDot(normal, normal);


				temp = 1/sqrtf(temp);
				normal = vectorScale(temp, normal);

				//find light value
				distance = vectorSub(lightPoint->coord, newStart);

				t = sqrtf(vectorDot(distance, distance));
				if(t <= 0.0){
					continue;
				}

				lightRay->start = newStart;
				lightRay->direction = vectorScale((1/t), distance);

				lambert = vectorDot(lightRay->direction, normal);
				//printf("lambert: %f\n", lambert);
				redIllumination = (intersectedCircle->colours->red * AMBIENT_K) + (intersectedCircle->colours->red * DIFFUSE_K * lambert);
				greenIllumination = (intersectedCircle->colours->green * AMBIENT_K) + (intersectedCircle->colours->green * DIFFUSE_K * lambert);
				blueIllumination = (intersectedCircle->colours->blue * AMBIENT_K) + (intersectedCircle->colours->blue * DIFFUSE_K * lambert);


				// if(vectorDot(normal, distance) <= 0.0){
				// 	//printf("Result: %f\n", vectorDot(normal, distance));
					
				// 	checkImage[(x + y*WIDTH)*3 + 0] = (unsigned char) intersectedCircle->colours->red * 0.3;
				// 	checkImage[(x + y*WIDTH)*3 + 1] = (unsigned char) intersectedCircle->colours->green * 0.3;
				// 	checkImage[(x + y*WIDTH)*3 + 2] = (unsigned char) intersectedCircle->colours->blue * 0.3;	
				// 	continue;
				// }

				// node = head;
				// while(node != NULL){
				// 	t = 20000.0;
				// 	inShadow = rayIntersect(lightRay, intersectedCircle, &t);

				// 	if(inShadow == 1){
				// 		break;
				// 	}
				// 	else{
				// 		//printf("Count: %d\n", count);
				// 		node = node->nextCir;
				// 	}
				// }

				// if(inShadow == 0) {
				// 	lambert = vectorDot(lightRay->direction, normal);
				// 	red += lambert * (lightPoint->colours->red/255) * (intersectedCircle->colours->red/255);
				// 	green += lambert * (lightPoint->colours->green/255) * (intersectedCircle->colours->green/255);
				// 	blue += lambert * (lightPoint->colours->blue/255) * (intersectedCircle->colours->blue/255);

				// }
				if(redIllumination < 0){
					redIllumination = 0;
				}
				else if (redIllumination > 255){
					redIllumination = 255;
				}

				if(greenIllumination < 0){
					greenIllumination = 0;
				}
				else if (greenIllumination > 255){
					greenIllumination = 255;
				}

				if(blueIllumination < 0){
					blueIllumination = 0;
				}
				else if (blueIllumination > 255){
					blueIllumination = 255;
				}


				checkImage[(x + y*WIDTH)*3 + 0] = (unsigned char) redIllumination * 255;
				checkImage[(x + y*WIDTH)*3 + 1] = (unsigned char) greenIllumination * 255;
				checkImage[(x + y*WIDTH)*3 + 2] = (unsigned char) blueIllumination * 255;	
			}			
		}
	}
	/* free
	Circle * node, * intersectedCircle;
	Vector  *scaled, *newStart, *normal, *distance, *tmp;
	Ray * lightRay;
	*/
}

void display (void)
{
GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_gray[] = {0.8, 0.8, 0.8, 0.8};
GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };

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

	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);

	//glRasterPos2i(0,0);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, checkImage); 

	//glTranslatef(5,0.5,5);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	

	//glTranslated (lightX, lightY, lightZ);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	//glutWireSphere (0.1, 15, 15);
	glEnable (GL_LIGHTING);

	glFlush ();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
		case 'q':
			exit(0);
			break;
	}
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

		node = (Circle *)malloc(sizeof(Circle));
		node->coord = (Vector *)malloc(sizeof(Vector));
		node->colours = (rgb *)malloc(sizeof(rgb));
		node->nextCir = NULL;

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
					//light
					lightPoint = (Light *)malloc(sizeof(Light));
					lightPoint->coord = (Vector *)malloc(sizeof(Vector));
					lightPoint->colours = (rgb *)malloc(sizeof(rgb));

					for (x = 0; x < 6; x++)	{
						buffer = strtok(NULL, "  ");
						switch(x) {
							case 0: //x
								lightPoint->coord->x = atof(buffer);
								break;
							case 1: //y
								lightPoint->coord->y = atof(buffer);
								break;
							case 2: //z
								lightPoint->coord->z = atof(buffer);
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
				curr = (Circle *)malloc(sizeof(Circle));
				curr->coord = (Vector *)malloc(sizeof(Vector));
				curr->colours = (rgb *)malloc(sizeof(rgb));
				curr->nextCir = NULL;

				for (x = 0; x < 7; x++)	{

					buffer = strtok(NULL, "  ");

					switch(x) {
						case 0: //x
							curr->coord->x = atof(buffer);
							break;
						case 1: //y
							curr->coord->y = atof(buffer);
							break;
						case 2: //z
							curr->coord->z = atof(buffer);
							break;
						case 3: //R
							curr->radius = atof(buffer);
							break;
						case 4: //r
							curr->colours->red = atof(buffer);
							break;
						case 5: //g
							curr->colours->green = atof(buffer);
							break;
						case 6:
							curr->colours->blue = atof(buffer);
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
		}//ends while
	}//end else	
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv) {
	int i, j, c;
	Circle * test;
	height = HEIGHT;
	width = WIDTH;
	head = NULL;
	lightPoint = NULL;
	ray = (Ray *)malloc(sizeof(Ray));
	ray->start = (Vector *)malloc(sizeof(Vector));
	ray->direction = (Vector *)malloc(sizeof(Vector));

	readFile(argv);
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize (1024, 768);
	glutCreateWindow (argv[0]);
	calculatePixel();
	glutDisplayFunc(display);
	glutKeyboardFunc (keyboard);
	glutMainLoop();

	test = head;
	while(test != NULL){
		head = test->nextCir;
		free(test->coord);
		free(test->colours);
		free(test);
		test = head;
	}
	free(lightPoint->colours);
	free(lightPoint->coord);
	free(lightPoint);
	free(ray->direction);
	free(ray->start);
	free(ray);
	free(head);
	return 0; 
}