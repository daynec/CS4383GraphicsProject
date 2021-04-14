#include <GL/glew.h>
#include <GL/freeglut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define M_PI 3.1415926535897932384626433832795

Shader shader; // loads our vertex and fragment shaders
Model *body; //a body 
Model *plane; //a plane
//Model *lArm; //a left arm // TODO: Convert to Wheels
//Model *rArm; //a right arm
//Model *lLeg; //a left leg
//Model *rLeg; //a right leg
//Model *head; //a head
glm::mat4 projection; // projection matrix
glm::mat4 view; // where the camera is looking
glm::mat4 model; // Main-Body Model
//glm::mat4 modelLArm; //Left Arm swing model // TODO: Convert to Wheels
//glm::mat4 modelRArm; //Right Arm swing model
//glm::mat4 modelLLeg; //Left Leg swing model
//glm::mat4 modelRLeg; //Right Leg swing model
//glm::mat4 modelHead; //Head swivel model
float angle = 0;
float angle2 = 0;
glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 direction;
int firstPerson = 1; //1 is first person, 0 is free-look
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 20.0f);
glm::vec3 camDir = glm::vec3(0.0f, 0.0f, 0.0f);
float camAngle;
float camPitch = 0.0f;
float speed = 0.0f;
/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
		std::cerr << "GL error " << error << " detected in " << functionName << std::endl;
	}
}

void initShader(void)
{
	shader.InitializeFromFile("shaders/phong.vert", "shaders/phong.frag");
	shader.AddAttribute("vertexPosition");
	shader.AddAttribute("vertexNormal");

	checkError ("initShader");
}

void initRendering(void)
{
	glClearColor (0.117f, 0.565f, 1.0f, 0.0f); // Dodger Blue
	checkError ("initRendering");
}

void init(void) 
{	
	// Perspective projection matrix.
	projection = glm::perspective(45.0f, 800.0f/600.0f, 1.0f, 1000.0f);

	
	// Load identity matrix into model matrix (no initial translation or rotation)
	

	initShader ();
	initRendering ();
}

/* This prints in the console when you start the program*/
void dumpInfo(void)
{
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	checkError ("dumpInfo");
}

/*This gets called when the OpenGL is asked to display. This is where all the main rendering calls go*/
void display(void)
{

	//glm::rot
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// camera positioned at 20 on the z axis, looking into the screen down the -Z axis.
	model = glm::translate(position) * glm::rotate(angle, 0.0f, 1.0f, 0.0f); //Body Model
	direction = glm::normalize(glm::vec3(sin(angle * (M_PI / 180)), 0, cos(angle * (M_PI / 180))));
	if (firstPerson == 0) {
		camDir = glm::normalize(glm::vec3(
			cos(glm::radians(camAngle)) * cos(glm::radians(camPitch)),	//X
			sin(glm::radians(camPitch)),								//Y
			sin(glm::radians(camAngle)) * cos(glm::radians(camPitch))));//Z
		view = glm::lookAt(camPos, camPos+camDir, glm::vec3(0.0f, 1.0f, 0.0f));
	} 
	else
		view = glm::lookAt(position + glm::vec3(0.0f, 1.0f, 0.0f), position+(-direction)+glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));//camera render location
	/*
	modelLArm = model * glm::translate(1.4f, 1.0f, 0.0f) * glm::rotate(45 * sin(angle2), 1.0f, 0.0f, 0.0f) * glm::translate(0.0f, -1.0f, 0.0f)* glm::scale(0.4f, 1.0f, 0.4f);
	modelRArm = model * glm::translate(-1.4f, 1.0f, 0.0f) * glm::rotate(45 * sin(angle2), -1.0f, 0.0f, 0.0f) * glm::translate(0.0f, -1.0f, 0.0f) * glm::scale(0.4f, 1.0f, 0.4f);
	modelLLeg = model * glm::translate(0.8f, -1.0f, 0.0f) * glm::rotate(45 * sin(angle2), -1.0f, 0.0f, 0.0f) * glm::translate(0.0f, -2.0f, 0.0f) * glm::scale(0.4f, 1.0f, 0.4f);
	modelRLeg = model * glm::translate(-0.8f, -1.0f, 0.0f) * glm::rotate(45 * sin(angle2), 1.0f, 0.0f, 0.0f) * glm::translate(0.0f, -2.0f, 0.0f) * glm::scale(0.4f, 1.0f, 0.4f);
	modelHead = model * glm::translate(0.0f, 2.5f, 0.0f) * glm::scale(0.5f, 0.5f, 0.5f);
	*/
	body->render(view *model * glm::scale(1.0f, 1.0f, 1.0f), projection); // Render current active model.
	/*
	// lArm is a child of the body
	lArm->render(view *modelLArm, projection);
	// rArm is a child of the body
	rArm->render(view *modelRArm, projection);
	// lLeg is a child of the body
	lArm->render(view *modelLLeg, projection);
	// rLeg is a child of the body
	rArm->render(view *modelRLeg, projection);
	// head is a child of the body, not rendered if first person
	if (firstPerson == 0)
		head->render(view * modelHead, projection);
	*/
	plane->render(view * glm::translate(0.0f,-5.0f,0.0f)*glm::scale(20.0f,1.0f,20.0f), projection);
	position -= direction * speed;
	
	glutSwapBuffers(); // Swap the buffers.
	checkError ("display");
}

/*This gets called when nothing is happening (OFTEN)*/
void idle()
{
	glutPostRedisplay(); // create a display event. Display calls as fast as CPU will allow when put in the idle function
}

/*Called when the window is resized*/
void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	checkError ("reshape");
}

/*Called when a normal key is pressed*/
void keyboard(unsigned char key, int x, int y)
{
	//glm::vec3 x1 = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction));
	switch (key) {
	case 27: // this is an ascii value
		exit(0);
		break;
	case 119: //w key
		if(speed <= 1.0f)
			speed += .01f;
		/*position -= direction; //Moved to display, modified here
		angle2 += .2;*/
		break;
	case 97: //a key
		//position += x1;
		angle += 10;
		break;
	case 115: //s key
		if (speed >= -1.0f)
			speed -= .01f;
		/*position += direction;
		angle2 += .2;*/
		break;
	case 100: //d key
		//position -= x1;
		angle -= 10;
		break;
	case 32: //space bar
		if (speed >= 0.1f || speed <= -0.1f) {
			if (speed >= 0.1f)
				speed -= 0.1f;
			if (speed <= -0.1f)
				speed += 0.1f;
		}
		else
			speed = 0.0f;
		break;
	case 99: //c key
		if (firstPerson == 1)
			firstPerson = 0;
		else
			firstPerson = 1;
		break;
	case 102: //f key
		//forward camera
		camPos += camDir;
		break;
	case 118: //v key
		//backward camera
		camPos -= camDir;
		break;
	}
}

void specialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		//turnleft camera
		camAngle -= 5.0f;
		break;
	case GLUT_KEY_RIGHT:
		//turnright camera
		camAngle += 5.0f;
		break;
	case GLUT_KEY_UP:
		//angleup camera
		camPitch += 5.0f;
		if (camPitch > 45.0f)
			camPitch = 45.0f;
		if (camPitch < -45.0f)
			camPitch = -45.0f;
		break;
	case GLUT_KEY_DOWN:
		//angledown camera
		camPitch -= 5.0f;
		if (camPitch > 45.0f)
			camPitch = 45.0f;
		if (camPitch < -45.0f)
			camPitch = -45.0f;
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE| GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	glewInit();
	dumpInfo ();
	init ();
	glutDisplayFunc(display); 
	glutIdleFunc(idle); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glutSpecialFunc(specialKeys);
	glEnable(GL_DEPTH_TEST);

	
	//body = new Model(&shader, "models/cylinder.obj");
	body = new Model(&shader, "models/dodge-challenger_model.obj", "models/");
	plane = new Model(&shader, "models/plane.obj");
	//sphere = new Model(&shader, "models/dodge-challenger_model.obj", "models/"); // you must specify the material path for this to load
	//lArm = new Model(&shader, "models/cylinder.obj");
	//rArm = new Model(&shader, "models/cylinder.obj");
	//lLeg = new Model(&shader, "models/cylinder.obj");
	//rLeg = new Model(&shader, "models/cylinder.obj");
	//head = new Model(&shader, "models/cylinder.obj");
	

	glutMainLoop();

	return 0;
}