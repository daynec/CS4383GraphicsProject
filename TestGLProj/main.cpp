#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "Shader.h"
#include "Shader3.h"
#include "Mesh.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define M_PI 3.1415926535897932384626433832795

void collisionDetectionSphere(glm::vec3 pos, float radius);
void collisionResolutionSphere();
void generateCarBox();
bool roadCheck(glm::vec3 roadPos, float roadXWidth, float roadYHeight, float roadZLength);

Shader shader; // loads our vertex and fragment shaders
//Shader3 shaderSL; // loads our vertex and fragment shaders
Model *car; //a car 
Mesh *sphere; //a light??? 
Model *cone; //a traffic-cone 
Model *plane; //a plane (depreciated)
Model *street; //a street-segment
Model *lamp; //a lamp-post
glm::mat4 projection; // projection matrix
glm::mat4 view; // where the camera is looking
glm::mat4 model; // Main-Body Model
glm::vec3 carBoxMinXYZ; //Contains the minimum x, y, and z values for the car's top-down 2D bounding box.
glm::vec3 carBoxMaxXYZ; //Contains the maximum x, y, and z values for the car's top-down 2D bounding box.
float angle = 0; //The angle of the car
glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f); //The position of the player/car
glm::vec3 direction; //The direction the car is facing.
int cameraMode = 1; //1 is first person, 0 is free-look, 2 is top down, 3 is thrid person
glm::vec3 camPos = glm::vec3(0.0f, 5.0f, -5.0f);
glm::vec3 camDir = glm::vec3(0.0f, 0.0f, 0.0f);
float camAngle;
float camPitch = 0.0f;
float speed = 0.0f; //Value of the car's current speed multiplier.
int onRoad = 0; //Increments if the car is currently on a road. Total is the number of road tiles the car is concurrently on.
int isGravity = 1; //1 for gravity to be turned on, 0 for gravity to be turned off (such as when currently driving on the road)

const float CAR_COLLIDER_RADIUS = 1.0f; //The radius of the car's collider sphere for obstacle collision.
const float GRAVITY_SPEED = 0.02f; // Speed at which the car will fall when not on the road.
const float MAX_SPEED = 1.0f; //Maxiumum speed multiploer the car can reach.
const float ACCELERATION = 0.01f; //Change in speed when w or s are held
const float HANDBREAK_STRENGTH = 0.1f; //Change in speed when Spacebar is held
const float TURN_SPEED = 10; // Angle the car will turn each press of s or d

float rotation = 0.0f;
glm::vec4 lightPosition = glm::vec4(0.0f, 3.0f, 0.0f, 1.0f);

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
	//shaderSL.InitializeFromFile("shaders/phong.vert", "shaders/phong3.frag");
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
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera positioned at 20 on the z axis, looking into the screen down the -Z axis.
	model = glm::translate(position) * glm::rotate(angle, 0.0f, 1.0f, 0.0f); //Car Body Model
	generateCarBox();
	direction = glm::normalize(glm::vec3(sin(angle * (M_PI / 180)), 0, cos(angle * (M_PI / 180))));
	if (cameraMode == 0) { //Free-look
		camDir = glm::normalize(glm::vec3(
			cos(glm::radians(camAngle)) * cos(glm::radians(camPitch)),	//X
			sin(glm::radians(camPitch)),								//Y
			sin(glm::radians(camAngle)) * cos(glm::radians(camPitch))));//Z
		view = glm::lookAt(camPos, camPos+camDir, glm::vec3(0.0f, 1.0f, 0.0f));
	} 
	else if(cameraMode == 1) //First Person
		//The camera fix to allow turning was adding direction to the eye location since you moved the turnpoint to be on the front axel.
		view = glm::lookAt(position + direction + glm::vec3(0.0f, 1.0f, 0.0f), position+ (-direction)+glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));//camera render location
	else if(cameraMode == 2) //Top Down
		view = glm::lookAt(position + glm::vec3(0.0f, 20.0f, 0.0f), position + (-direction) + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	else if(cameraMode == 3) //Third Person
		//Is there a better way to do +direction*5, yeah but I put this. Cahnge it if you feel like it.
		view = glm::lookAt(position + direction + direction + direction + direction + direction + glm::vec3(0.0f, 2.0f, 0.0f), position + (-direction) + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	car->render(view *model * glm::scale(1.0f, 1.0f, 1.0f) * glm::translate(0.0f,0.0f,1.0f), projection); // Render car
	//plane->render(view * glm::translate(0.0f,0.0f,-175.0f)*glm::scale(10.0f,1.0f,180.0f), projection);
	/*This section will check if the car is driving on any of the roads*/
	onRoad = 0;
	for (int i = 0; i < 350; i += 20) {
		street->render(view * glm::translate(-3.0f, 0.0f, 0.0f - i) * glm::scale(2.0f, 2.0f, 2.0f), projection);
		if (roadCheck(glm::vec3(-3.0f, 0.0f, 0.0f-i), 19.0f, 0.5f, 19.0f)) { //These road-values besides position are approximations due to lack of exact measurements.
			onRoad++;
		}
	}
	if (onRoad == 0) { //This if statement is if the car is not on any roads.
		printf("Not on road. ");
		isGravity = 1;
		//TODO: This is where you will call things for if the car is not on ANY road. Falling or other punishment if car is not on the track for example.
	} else { //This if statement is if the car is on at least one road.
		isGravity = 0;
	}
	
	for (int i = 0; i < 350; i += 20) {
		//Add lamps on Right hand side
		lamp->render(view * glm::translate(5.0f, 0.0f, 0.0f - i) * glm::scale(2.0f, 2.0f, 2.0f), projection);
		//Test collision
		collisionDetectionSphere(glm::vec3(5.0f, 0.0f, 0.0f-i), .7f);
		// TODO: Better establish the base of the lamp's coordinates so that the collision spheres are exactly at 
		// the base of the lamp so you don't collide in the space underneath the light which is techinally the middle of the lamp model.
		//Add lamps on Left hand side
		lamp->render(view * glm::translate(-12.0f, 0.0f, 0.0f - i)* glm::rotate(180.0f, 0.0f, 1.0f, 0.0f) * glm::scale(2.0f, 2.0f, 2.0f), projection);
		//Test collision
		collisionDetectionSphere(glm::vec3(-12.0f, 0.0f, 0.0f-i), .7f);
		// TODO: Better establish the base of the lamp's coordinates so that the collision spheres are exactly at 
		// the base of the lamp so you don't collide in the space underneath the light which is techinally the middle of the lamp model.
	}

	position -= (direction * speed) + glm::vec3(0.0f, isGravity * GRAVITY_SPEED, 0.0f); //Now includes gravity Effects

	//lightPosition = glm::rotate(0.1f, 0.0f, 0.0f, 1.0f) * lightPosition;
	//shaderSL.Activate(); // Bind shader.
	//shaderSL.SetUniform("lightPosition", view * lightPosition);
	//shaderSL.SetUniform("lightDiffuse", glm::vec4(1.0, 1.0, 1.0, 1.0));
	//shaderSL.SetUniform("lightSpecular", glm::vec4(1.0, 1.0, 1.0, 1.0));
	//shaderSL.SetUniform("lightAmbient", glm::vec4(0.0, 0.0, 0.0, 1.0));

	//shaderSL.SetUniform("surfaceDiffuse", glm::vec4(1.0, 0.0, 0.0, 1.0));
	//shaderSL.SetUniform("surfaceSpecular", glm::vec4(1.0, 1.0, 1.0, 1.0));
	//shaderSL.SetUniform("surfaceAmbient", glm::vec4(1.0, 0.0, 0.0, 1.0));
	//shaderSL.SetUniform("shininess", 90.0f);
	//shaderSL.SetUniform("surfaceEmissive", glm::vec4(0.0, 0.0, 0.0, 1.0));


	//shaderSL.Activate();
	////draw the light!
	//shaderSL.SetUniform("surfaceEmissive", glm::vec4(1.0, 1.0, 1.0, 1.0));
	//sphere->render(view * glm::translate(lightPosition.x, lightPosition.y, lightPosition.z) * glm::scale(.1f, .1f, .1f), projection);


	//shaderSL.DeActivate(); // Unbind shader.
	
	glutSwapBuffers(); // Swap the buffers.
	checkError ("display");
}

/*Put in the location and radius for the collision sphere of the object for the car to test against. If a collision is detected collisionResolution is automatically called.*/
void collisionDetectionSphere(glm::vec3 pos, float radius) {
	if (glm::distance(position, pos) < CAR_COLLIDER_RADIUS + radius) {
		collisionResolutionSphere();
	}
}

/*Automatically called if collisionDetection detects the car's collision sphere has entered another collision sphere.*/
void collisionResolutionSphere() {
	if (speed != 0) {
		printf("Hit something. ");
		//Sets speed to 0 after negating the movement that lead to being stuck to ensure they remain where they were last safe instead of just slamming into the wall immediately again.
		//TODO: Richochet collision, is that's something we wish to do.
		position -= direction * -speed;
		speed = 0;
		//TODO: Add a visual, maybe a flash of light or a color change to show what was collided, or a sound, like a metal smash noise. Not essential, but would be a better indicator.
	}
	else {
		//TODO: Make error message for when what was once safe a frame ago is no longer safe since that is where they should be safe.
		//TODO: Reset the car either to the start or back into the middle of the road.
	}
}

/*Regenerates the cars simple bounding box based on it's current position and the radius of the sphere-collider used for object collision*/
void generateCarBox() {
	//Approxiamtion due to lack of car-measurements
	carBoxMaxXYZ = glm::vec3(position.x + CAR_COLLIDER_RADIUS, position.y+1.0f, position.z + CAR_COLLIDER_RADIUS);
	carBoxMinXYZ = glm::vec3(position.x - CAR_COLLIDER_RADIUS, position.y, position.z - CAR_COLLIDER_RADIUS);
}

/*Will return false if the car is not currently on the road, true if the car is currently colliding on the road.*/
bool roadCheck(glm::vec3 roadPos, float roadXWidth, float roadYHeight, float roadZLength) {
	//Generate Road bounding values, 
	glm::vec3 roadMaxXYZ(roadPos.x + (roadXWidth / 2), roadPos.y+roadYHeight, roadPos.z + (roadZLength / 2));//The coordinate that contains the Max X and Max Z of this road tile.
	glm::vec3 roadMinXYZ(roadPos.x - (roadXWidth / 2), roadPos.y, roadPos.z - (roadZLength / 2));//The coordinate that contains the Min X and Min Z of this road tile.
	//Test corners for inclusion in box
	if ((carBoxMaxXYZ.x <= roadMaxXYZ.x && carBoxMaxXYZ.x >= roadMinXYZ.x //If car's max X is within road box
		||															//Or
		carBoxMinXYZ.x <= roadMaxXYZ.x && carBoxMinXYZ.x >= roadMinXYZ.x) //If car's min X is within road box
		&&																//AND
		(carBoxMaxXYZ.z <= roadMaxXYZ.z && carBoxMaxXYZ.z >= roadMinXYZ.z //If car's max Z is within road box
		||															//Or
		carBoxMinXYZ.z <= roadMaxXYZ.z && carBoxMinXYZ.z >= roadMinXYZ.z))//If car's min Z is within road box
	{
		if (carBoxMinXYZ.y == roadMaxXYZ.y) { //On the road face still
			return true;//Gravity Off (driving on road)
		}
		if (carBoxMinXYZ.y > roadMaxXYZ.y || carBoxMaxXYZ.y < roadMinXYZ.y) { //If the car is above the road-box or under the roadbox
			return false;//Gravity On (not colliding with road)
		}
		if (carBoxMinXYZ.y > roadMinXYZ.y && carBoxMinXYZ.y < roadMaxXYZ.y){ //If bottom of car is colliding with the road
			position += glm::vec3(0.0f, roadMaxXYZ.y - carBoxMinXYZ.y, 0.0f);
			return true;//Gravity Off (collision with road resolved, now driving on road)
		}
		if (carBoxMaxXYZ.y > roadMinXYZ.y && carBoxMaxXYZ.y < roadMaxXYZ.y) {//Checking if top of car is colliding with road
			//Since the last if statement would have returned if true this means the car is bumping into the bottom of the road so we will push it beneath the road
			position -= glm::vec3(0.0f, carBoxMaxXYZ.y - roadMinXYZ.y, 0.0f);
			return false;//Gravity On (collision with road resolved, not driving on the road)
		}
		printf("ERROR: This is not supposed to be reachable. \n roadCheck unresolved case. \n carBoxMaxXYZ.y = %f \n carBoxMinXYZ.y = %f \n roadBoxMaxXYZ.y = %f \n roadBoxMinXYZ.y = %f",
			carBoxMaxXYZ.y, carBoxMinXYZ.y, roadMaxXYZ.y, roadMinXYZ.y);
		return true; //Shouldn't be reachable but if it is the error message above will print diagnostics.
	} else {
		return false; //If no corner is within the road's box
	}
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
		if(speed <= MAX_SPEED)
			speed += ACCELERATION;
		break;
	case 97: //a key
		angle += TURN_SPEED;
		break;
	case 115: //s key
		if (speed >= -MAX_SPEED)
			speed -= ACCELERATION;
		break;
	case 100: //d key
		angle -= TURN_SPEED;
		break;
	case 32: //space bar
		if (speed >= HANDBREAK_STRENGTH || speed <= -HANDBREAK_STRENGTH) {
			if (speed >= HANDBREAK_STRENGTH)
				speed -= HANDBREAK_STRENGTH;
			if (speed <= -HANDBREAK_STRENGTH)
				speed += HANDBREAK_STRENGTH;
		}
		else
			speed = 0.0f;
		break;
	case 99: //c key
		cameraMode++;

		if (cameraMode == 4)
			cameraMode = 0;

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
	glutInitWindowSize (1080, 720); 
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



	
	car = new Model(&shader, "models/dodge-challenger_model.obj", "models/");
	//sphere = new Mesh("models/sphere.obj", &shaderSL);
	plane = new Model(&shader, "models/plane.obj");
	street = new Model(&shader, "models/street_str.obj", "models/");
	lamp = new Model(&shader, "models/street lamp.obj", "models/");
	cone = new Model(&shader, "models/road_cone_obj.obj");
	

	glutMainLoop();

	return 0;
}