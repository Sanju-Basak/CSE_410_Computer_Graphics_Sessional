#define _USE_MATH_DEFINES

#include <windows.h> // For MS Windows
#include <GL/glut.h> // GLUT, includes glu.h and gl.h
#include <cmath>
#include <iostream>

// Global variables

//Circle variables
GLfloat circleRadius = 0.25f;
GLfloat centerX= 0.0f;
GLfloat centerY= 0.5f;

//Pendulum variables
GLfloat angle = 0;        // Initial angle (in degrees)
GLfloat length = 0.35f;    // Pendulum length
GLfloat maxAngle = 30.0f; // Maximum angle (in degrees)
GLfloat dt= 0.0f;         // Time step

//handAngle variables
GLfloat hourAngle= 0.0f;
GLfloat minuteAngle= 0.0f;
GLfloat secondAngle= 0.0f;

void initGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0); // Set background (clear) color to black
}

void drawCircle(float radius)
{
    int numSegments = 100;
    float angle;

    glBegin(GL_LINE_LOOP);

    for (int i = 0; i <= numSegments; i++)
    {
        angle = i * 2.0f * 3.14159f / numSegments;
        glVertex2f(radius * cos(angle), radius * sin(angle));
    }

    glEnd();
}

void drawBob(float radius){
    glBegin(GL_POLYGON);
   for (int i = 0; i < 360; i++)
   {
      float degInRad = i * 3.14 / 180;
      glVertex2f(cos(degInRad) * radius, sin(degInRad) * radius);
   }
   glEnd();
}

//function for drawing line
void drawLine(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINES);
    glVertex2f(x1, y1); // origin of the line
    glVertex2f(x2, y2); // ending point of the line
    glEnd();
}

void drawHour(){
    
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glRotatef(hourAngle, 0, 0, 1);
    glBegin(GL_POLYGON);
        glVertex2f(-0.01f, 0.0f);
        glVertex2f(0.01f, 0.0f);
        glVertex2f(0.01f, 0.1f);
        glVertex2f(0, 0.15f);
        glVertex2f(-0.01f, 0.1f);
    glEnd();

    glPopMatrix();
}

void drawMinute(){

    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glRotatef(minuteAngle, 0, 0, 1);
    glBegin(GL_POLYGON);
        glVertex2f(-0.007f, 0.0f);
        glVertex2f(0.007f, 0.0f);
        glVertex2f(0.007f, 0.15f);
        glVertex2f(0, 0.2f);
        glVertex2f(-0.007f, 0.15f);
    glEnd();
    glPopMatrix();
}

void drawSecond(){

    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glRotatef(secondAngle, 0, 0, 1);
    glBegin(GL_POLYGON);
        glVertex2f(-0.005f, 0.0f);
        glVertex2f(0.005f, 0.0f);
        glVertex2f(0.005f, 0.2f);
        glVertex2f(0, 0.25f);
        glVertex2f(-0.005f, 0.2f);
    glEnd();
    glPopMatrix();
}

void drawPendulum()
{

   // Calculate the pendulum position
   float posX = length * sin(angle * M_PI / 180.0);
   float posY = -length * cos(angle * M_PI / 180.0);

   // Draw the pendulum
    glTranslatef(0, .17f, 0.0f);
   glBegin(GL_POLYGON);                  
      glVertex2f(-0.01f, 0.0f);
      glVertex2f( posX- 0.01f, posY);
      glVertex2f( posX+ 0.01f,  posY);
      glVertex2f(0.01f,  0.0f);
   glEnd();
   drawBob(0.03f);
   glTranslatef(posX, posY, 0.0f);
   // draw a circle
   drawBob(0.06f);

}

void update(int value)
{
   // Update the pendulum angle here (e.g., using physics equations)
   angle = maxAngle * sin(M_PI * dt); // Increment the angle by 1 degree for simplicity
   if (dt > 2)
   {
      dt = 0;
   }
   // Redraw the scene
   glutPostRedisplay();
   dt += 0.01;
   // Call the update function again after a specific time interval (e.g., 16 milliseconds)
   glutTimerFunc(10, update, 0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // To operate on the model-view matrix
    glLoadIdentity();           // Reset the model-view matrix

    time_t t = time(0);
    tm* now = localtime(&t);
    
    hourAngle= -((now->tm_hour%12)* 30+ now->tm_min* .5);
    minuteAngle= -(now->tm_min*6+ .1* now->tm_sec);
    secondAngle= -(now->tm_sec*6);


    //two circles of clock
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    drawCircle(circleRadius);
    glScalef(1.2f, 1.2f, 1.0f);
    drawCircle(circleRadius);
    glPopMatrix();

    //Midddle of clock
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-0.005f, -0.005f); // Bottom-left vertex
        glVertex2f(0.005f, -0.005f);  // Bottom-right vertex
        glVertex2f(0.005f, 0.005f);   // Top-right vertex
        glVertex2f(-0.005f, 0.005f);  // Top-left vertex
    glEnd();
    glPopMatrix();

    //lines
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glRotatef(i * 90.0f, 0.0f, 0.0f, 1.0f);
        drawLine(0.0f, 0.2f, 0.0f, 0.25f);
        glPopMatrix();
    }

    for (int i=0; i<12; i++){
        if(i%3 != 0){
            glPushMatrix();
            glRotatef(i * 30.0f, 0.0f, 0.0f, 1.0f);
            drawLine(0.0f, 0.23f, 0.0f, 0.25f);
            glPopMatrix();
        }
    }
    glPopMatrix();

    //finishing box

    glPushMatrix();
    glTranslatef(centerX, centerY, 0);
    glBegin(GL_LINES);
        glVertex2f(-.225, -.2);
        glVertex2f(-.225, -.703);

        glVertex2f(-.225, -.703);
        glVertex2f(0, -.9);

        glVertex2f(0, -.9);
        glVertex2f(.225, -.703);

        glVertex2f(.225, -.703);
        glVertex2f(.225, -.2);
    glEnd();
    glPopMatrix();

    drawHour();
    //glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
    drawMinute();
   // glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
    drawSecond();


    drawPendulum();



   // Swap the buffers to display the result
   glutSwapBuffers();
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitWindowSize(640, 640);          // Set the window's initial width & height
    glutInitWindowPosition(50, 50);        // Position the window's initial top-left corner
    glutCreateWindow("Clock"); // Create a window with the given title
    glutDisplayFunc(display);              // Register display callback handler for window re-paint
    initGL();                              // Our own OpenGL initialization
    glutTimerFunc(10, update, 0);          // First timer call immediately
    glutMainLoop();                        // Enter the infinitely event-processing loop
    return 0;
}
