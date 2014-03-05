#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <math.h> 


int maxFPS = 61.0f; 						// refresh interval in milliseconds
int blur = 0;      							// 0: no motion blur, 1: eye tracking, 2: shutter simulation
int speedFactor = 2;      					// speed: 1x to 8x
int refresh = 0;
char title[] = "A-buffer motion blur. Type = 0, Speed = 2x";  			
int iterations = 10; 						// Motion blur quality
int windowWidth  = 640;     				// Windowed mode's width
int windowHeight = 480;     				// Windowed mode's height
int windowPosX   = 50;      				// Windowed mode's top-left corner x
int windowPosY   = 50;      				// Windowed mode's top-left corner y


GLfloat xPos = 0.0f;         				// Object position
GLfloat yPos = 0.0f;
GLfloat xVel = 0.0025f;         			// Objects speeds
GLfloat yVel = 0.02f;
GLfloat angle = 0.0f;  						// rotational angle per frame
GLfloat dx = 0.0f;  						// rotational angle of the shapes
	
void initGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 	// Black and opaque Background
}

/* Called back when the timer expired */
void Timer(int value) {
	glutPostRedisplay();    								
	if (!blur) glutTimerFunc(1/(maxFPS), Timer, 0); 		// subsequent timer call at milliseconds
	else glutTimerFunc(1/(maxFPS*iterations), Timer, 0); 	// subsequent timer call at milliseconds
}


void display() {
	if (blur == 0){							//Calculating step sizes
		xPos+=xVel*speedFactor;
		yPos+=yVel*speedFactor;
		angle -= dx*speedFactor;
	}
	else if (blur == 1){
		if (refresh == 0){
			xPos+=xVel*speedFactor;
			yPos+=yVel*speedFactor;
		}
		angle -= dx/iterations*speedFactor;
	}
	else {
		xPos+=xVel/iterations*speedFactor;
		yPos+=yVel/iterations*speedFactor;
		angle -= dx/iterations*speedFactor;
	}

	glClear(GL_COLOR_BUFFER_BIT);   					
	glMatrixMode(GL_MODELVIEW);     
	glLoadIdentity();               

	glPushMatrix();                     // Creating figure
	glTranslatef(xPos, yPos, 0.0f);    	// Translate
	glRotatef(angle, 0.0f, 0.0f, 1.0f); // rotate 
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 1.0f, 1.0f); 
	glVertex2f(-0.3f, -0.2f);
	glVertex2f( 0.3f, -0.2f);
	glVertex2f( 0.0f,  0.3f);
	glEnd();
	glPopMatrix();                      

	glPushMatrix();                     // Inner triangle
	glTranslatef(xPos, yPos, 0.0f);    
	glRotatef(angle, 0.0f, 0.0f, 1.0f); 
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f); 
	glVertex2f(-0.3f*0.8, -0.2f*0.8);
	glVertex2f( 0.3f*0.8, -0.2f*0.8);
	glVertex2f( 0.0f*0.8,  0.3f*0.8);
	glEnd();
	glPopMatrix();                      

	if (blur) {							//Motion blur algorithm
		refresh++;
		if(refresh == 0) glAccum(GL_LOAD, 1.0 / iterations);
		else glAccum(GL_ACCUM, 1.0 / iterations);
		if(refresh >= iterations ) {
			refresh = 0;
			glAccum(GL_RETURN, 1.0);
			glutSwapBuffers(); 				
			glClear(GL_ACCUM_BUFFER_BIT);
		}
	}
	else glutSwapBuffers();
	
	if (yPos > 0.7) yVel=-fabs(yVel);	//Bouncing
	else if (yPos < -0.7) { yVel=fabs(yVel); dx = 10*xPos*yPos;}
	if (xPos > 0.7) xVel=-fabs(xVel);
	else if (xPos < -0.7) xVel=fabs(xVel);
   
}
void mouse(int button, int state, int x, int y) {			//Control
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		blur++;
		if (blur == 3) blur = 0;
		glClear(GL_ACCUM_BUFFER_BIT);
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		speedFactor*=2;
		if (speedFactor == 16) speedFactor =1;
		glClear(GL_ACCUM_BUFFER_BIT);
	}
	title[29]=(char)(((int)'0')+blur);;
	title[40]=(char)(((int)'0')+speedFactor);
	glutSetWindowTitle(title); 
}

void reshape(GLsizei width, GLsizei height) {  }
 

int main(int argc, char** argv) {
	glutInit(&argc, argv);          // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE);  // Enable double buffered mode
	glutInitWindowSize(windowWidth, windowHeight);   
	glutInitWindowPosition(windowPosX, windowPosY); 
	glutCreateWindow(title);  		
	glutDisplayFunc(display);       
	glutReshapeFunc(reshape);       
	glutMouseFunc(mouse);           
	glutTimerFunc(0, Timer, 0);     
	initGL();                       
	glutMainLoop();                 
	return 0;
}
