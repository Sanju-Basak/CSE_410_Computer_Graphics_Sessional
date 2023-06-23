#define _USE_MATH_DEFINES

#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

#define TRISUB 1.0f / 24.0f
#define TRIADD 1.0f / 48.0f
#define SCALING_FACTOR 1.0f / (16.0f * sqrt(2))
#define RADIAN M_PI / 180.0f

#define SPHERE_SUBDIVISIONS 4

struct point
{
   GLfloat x, y, z;
};

/* Initialize OpenGL Graphics */
void initGL()
{
   // Set "clearing" or background color
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
   glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}


struct point eye = {2, 2, 2};       // Camera position
struct point center = {-2, -2, -2}; // Look-at point
struct point up = {0, 1, 0};        // Up vector
struct point r = {1, 0, 0};         // right direction

// Point of the base triangle

struct point py = {0, 1, 0};
struct point pz = {0, 0, 1};
struct point px = {1, 0, 0};

double step = 0;
GLfloat radius = 0.0f;
double rate = 0.01f;   // Rate of change of movement
double rotateX = 0.0f; // Rate of change of rotation

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes()
{
   glLineWidth(3);
   glBegin(GL_LINES);
   glColor3f(1, 0, 0); // Red
   // X axis
   glVertex3f(0, 0, 0);
   glVertex3f(1.5, 0, 0);

   glColor3f(0, 1, 0); // Green
   // Y axis
   glVertex3f(0, 0, 0);
   glVertex3f(0, 1.5, 0);

   glColor3f(0, 0, 1); // Blue
   // Z axis
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, 1.5);
   glEnd();
}

void drawTriangle()
{
   GLfloat factor = 1.0f - sqrt(2) * radius;
   // cout << "radius" << radius << endl;
   // cout << "factor: " << factor << endl;
   glPushMatrix();
   glTranslatef(radius / sqrt(3), radius / sqrt(3), radius / sqrt(3));
   glScalef(factor, factor, factor);
   glBegin(GL_TRIANGLES); // Begin drawing the pyramid with 4 triangles
   glVertex3f(py.x, py.y, py.z);
   glVertex3f(pz.x, pz.y, pz.z);
   glVertex3f(px.x, px.y, px.z);
   glEnd(); // Done drawing the pyramid
   glPopMatrix();
}

/* Draw a pyramid centered at the origin */
void drawOctagon()
{
   glPushMatrix();              // Create a new scope
   glColor3f(0.0f, 1.0f, 1.0f); // Red
   drawTriangle();
   glPopMatrix();

   glPushMatrix(); // Create a new scope
   glColor3f(1.0f, 0.0f, 1.0f);
   glRotatef(90, 0, 1, 0);
   drawTriangle();
   glPopMatrix();

   glPushMatrix(); // Create a new scope
   glColor3f(1.0f, 0.0f, 1.0f);
   glRotatef(-90, 0, 1, 0);
   drawTriangle();
   glPopMatrix();

   glPushMatrix(); // Create a new scope
   glColor3f(0.0f, 1.0f, 1.0f);
   glRotatef(180, 0, 1, 0);
   drawTriangle();
   glPopMatrix();

   // mirror

   glPushMatrix(); // Create a new scope
   glRotatef(180, 1, 0, 0);
   glColor3f(0.0f, 1.0f, 1.0f); // Red
   drawTriangle();
   glPopMatrix();

   glPushMatrix(); // Create a new scope
   glColor3f(1.0f, 0.0f, 1.0f);
   glRotatef(180, 1, 0, 0);
   glRotatef(90, 0, 1, 0);
   drawTriangle();
   glPopMatrix();

   glPushMatrix(); // Create a new scope
   glColor3f(1.0f, 0.0f, 1.0f);
   glRotatef(180, 1, 0, 0);
   glRotatef(-90, 0, 1, 0);
   drawTriangle();
   glPopMatrix();

   glPushMatrix(); // Create a new scope
   glColor3f(0.0f, 1.0f, 1.0f);
   ;
   glRotatef(180, 1, 0, 0);
   glRotatef(180, 0, 1, 0);
   drawTriangle();
   glPopMatrix();
}



vector<vector<point>> buildUnitPositiveX(int subdivision)
{
   const float DEG2RAD = acos(-1) / 180.0f;

   // compute the number of vertices per row, 2^n + 1
   int pointsPerRow = (int)pow(2, subdivision) + 1;

   vector<vector<point>> vertices(pointsPerRow);
   float n1[3]; // normal of longitudinal plane rotating along Y-axis
   float n2[3]; // normal of latitudinal plane rotating along Z-axis
   float v[3];  // direction vector intersecting 2 planes, n1 x n2
   float a1;    // longitudinal angle along Y-axis
   float a2;    // latitudinal angle along Z-axis

   // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
   for (unsigned int i = 0; i < pointsPerRow; ++i)
   {
      // normal for latitudinal plane
      // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
      // therefore, it is rotating (0,1,0) vector by latitude angle a2
      a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
      n2[0] = -sin(a2);
      n2[1] = cos(a2);
      n2[2] = 0;

      // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
      for (unsigned int j = 0; j < pointsPerRow; ++j)
      {
         // normal for longitudinal plane
         // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
         // therefore, it is rotating (0,0,-1) vector by longitude angle a1
         a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
         n1[0] = -sin(a1);
         n1[1] = 0;
         n1[2] = -cos(a1);

         // find direction vector of intersected line, n1 x n2
         v[0] = n1[1] * n2[2] - n1[2] * n2[1];
         v[1] = n1[2] * n2[0] - n1[0] * n2[2];
         v[2] = n1[0] * n2[1] - n1[1] * n2[0];

         // normalize direction vector
         float scale = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
         v[0] *= scale;
         v[1] *= scale;
         v[2] *= scale;

         // add a vertex into array
         vertices[i].push_back({v[0], v[1], v[2]});

         // vertices.push_back({v[0], v[1], v[2]});
         //  vertices.push_back(v[1]);
         //  vertices.push_back(v[2]);
      }
   }

   return vertices;
}

void drawOneFace(int subdivision)
{
   vector<vector<point>> vertices = buildUnitPositiveX(subdivision);

   // draw +X face of the cube using quadrilaterals
   glBegin(GL_QUADS);
   for (unsigned int i = 0; i < vertices.size() - 1; ++i)
   {
      for (unsigned int j = 0; j < vertices[i].size() - 1; ++j)
      {
         // draw a quadrilateral using 4 vertices of 2 adjacent rows
         glVertex3f(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
         glVertex3f(vertices[i][j + 1].x, vertices[i][j + 1].y, vertices[i][j + 1].z);
         glVertex3f(vertices[i + 1][j + 1].x, vertices[i + 1][j + 1].y, vertices[i + 1][j + 1].z);
         glVertex3f(vertices[i + 1][j].x, vertices[i + 1][j].y, vertices[i + 1][j].z);
      }
   }
   glEnd();
}

void drawSphere(int subdivision)
{
   glPushMatrix(); // Create a new scope     for +X face
   glColor3f(1, 0, 0);
   glTranslatef((1 - sqrt(2) * radius), 0, 0);
   glScalef(radius, radius, radius);
   drawOneFace(subdivision);
   glPopMatrix();

   glPushMatrix(); // Create a new scope     for -Z face
   glColor3f(0, 1, 0);
   glTranslatef(0, 0, -(1 - sqrt(2) * radius));
   glRotatef(90, 0, 1, 0);
   glScalef(radius, radius, radius);
   drawOneFace(subdivision);
   glPopMatrix();

   glPushMatrix(); // Create a new scope     for -X face
   glColor3f(1, 0, 0);
   glTranslatef(-(1 - sqrt(2) * radius), 0, 0);
   glRotatef(180, 0, 1, 0);
   glScalef(radius, radius, radius);
   drawOneFace(subdivision);
   glPopMatrix();

   glPushMatrix(); // Create a new scope     for +Z face
   glColor3f(0, 1, 0);
   glTranslatef(0, 0, (1 - sqrt(2) * radius));
   glRotatef(270, 0, 1, 0);
   glScalef(radius, radius, radius);
   drawOneFace(subdivision);
   glPopMatrix();

   glPushMatrix(); // Create a new scope     for +Y face
   glColor3f(0, 0, 1);
   glTranslatef(0, (1 - sqrt(2) * radius), 0);
   glRotatef(90, 0, 0, 1);
   glScalef(radius, radius, radius);
   drawOneFace(subdivision);
   glPopMatrix();

   glPushMatrix(); // Create a new scope     for -Y face
   glColor3f(0, 0, 1);
   glTranslatef(0, -(1 - sqrt(2) * radius), 0);
   glRotatef(-90, 0, 0, 1);
   glScalef(radius, radius, radius);
   drawOneFace(subdivision);
   glPopMatrix();
}

void drawCylinderSegment(float angle, float radius, float height)
{
   int numSegments = 100;
   float segmentAngle = angle * RADIAN;

   glPushMatrix();
   glTranslatef(0, -height / 2, 0);
   glRotatef(angle / 2, 0.0f, 1.0f, 0.0f);

   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i <= numSegments; ++i)
   {
      float theta = i * segmentAngle / numSegments;
      float x = radius * cos(theta);
      float z = radius * sin(theta);

      glVertex3f(x, 0.0f, z);
      glVertex3f(x, height, z);
   }
   glEnd();
   glPopMatrix();
}

void drawCylinder()
{
   double angle = 70.53;
   double height = sqrt(2) - 2.0f * radius;
   double translate = (1.0f - sqrt(2) * radius) / 2.0f;

   glColor3f(1, 1, 0);
   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      glRotatef(90 * i, 0, 1, 0);
      glTranslatef(translate, translate, 0);
      glRotatef(45, 0, 0, 1);

      drawCylinderSegment(angle, radius, height);
      glPopMatrix();
   }
   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      glRotatef(180, 1, 0, 0);
      glRotatef(90 * i, 0, 1, 0);
      glTranslatef(translate, translate, 0);
      glRotatef(45, 0, 0, 1);

      drawCylinderSegment(angle, radius, height);
      glPopMatrix();
   }

   for (int i = 0; i < 4; i++)
   {
      glPushMatrix();
      glRotatef(90 * i, 0, 1, 0);
      glRotatef(90, 1, 0, 0);

      glTranslatef(translate, translate, 0);
      glRotatef(45, 0, 0, 1);

      drawCylinderSegment(angle, radius, height);
      glPopMatrix();
   }


}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display()
{
   // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
   glLoadIdentity();           // Reset the model-view matrix

   // default arguments of gluLookAt
   // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

   // cross product of center- eye  and up
   r.x = (center.y - eye.y) * up.z - (center.z - eye.z) * up.y;
   r.y = (center.z - eye.z) * up.x - (center.x - eye.x) * up.z;
   r.z = (center.x - eye.x) * up.y - (center.y - eye.y) * up.x;

   // control viewing (or camera)
   gluLookAt(eye.x, eye.y, eye.z,
             eye.x + center.x, eye.y + center.y, eye.z + center.z,
             up.x, up.y, up.z);
   // draw
   glPushMatrix();
   glRotatef(rotateX, 0, 1, 0);
   drawAxes();
   drawSphere(SPHERE_SUBDIVISIONS);
   drawOctagon();
   drawCylinder();
   glPopMatrix();

   glutSwapBuffers(); // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0)
      height = 1; // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);

   // Set the aspect ratio of the clipping area to match the viewport
   glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
   glLoadIdentity();            // Reset the projection matrix
   /*if (width >= height) {
       // aspect >= 1, set the height from -1 to 1, with larger width
       gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
   } else {
       // aspect < 1, set the width to -1 to 1, with larger height
       gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
   }*/
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y)
{

   double lx = center.x - eye.x;
   double lz = center.z - eye.z;
   double s;
   switch (key)
   {
   // Control eye (location of the eye)
   case '1':
      r.x = r.x * cos(rate) + center.x * sin(rate);
      r.y = r.y * cos(rate) + center.y * sin(rate);
      r.z = r.z * cos(rate) + center.z * sin(rate);

      center.x = center.x * cos(rate) - r.x * sin(rate);
      center.y = center.y * cos(rate) - r.y * sin(rate);
      center.z = center.z * cos(rate) - r.z * sin(rate);
      break;

   case '2':
      r.x = r.x * cos(-rate) + center.x * sin(-rate);
      r.y = r.y * cos(-rate) + center.y * sin(-rate);
      r.z = r.z * cos(-rate) + center.z * sin(-rate);

      center.x = center.x * cos(-rate) - r.x * sin(-rate);
      center.y = center.y * cos(-rate) - r.y * sin(-rate);
      center.z = center.z * cos(-rate) - r.z * sin(-rate);
      break;

   case '3':
      center.x = center.x * cos(rate) + up.x * sin(rate);
      center.y = center.y * cos(rate) + up.y * sin(rate);
      center.z = center.z * cos(rate) + up.z * sin(rate);

      up.x = up.x * cos(rate) - center.x * sin(rate);
      up.y = up.y * cos(rate) - center.y * sin(rate);
      up.z = up.z * cos(rate) - center.z * sin(rate);
      break;

   case '4':
      center.x = center.x * cos(-rate) + up.x * sin(-rate);
      center.y = center.y * cos(-rate) + up.y * sin(-rate);
      center.z = center.z * cos(-rate) + up.z * sin(-rate);

      up.x = up.x * cos(-rate) - center.x * sin(-rate);
      up.y = up.y * cos(-rate) - center.y * sin(-rate);
      up.z = up.z * cos(-rate) - center.z * sin(-rate);
      break;

   case '5':
      up.x = up.x * cos(rate) + r.x * sin(rate);
      up.y = up.y * cos(rate) + r.y * sin(rate);
      up.z = up.z * cos(rate) + r.z * sin(rate);

      r.x = r.x * cos(rate) - up.x * sin(rate);
      r.y = r.y * cos(rate) - up.y * sin(rate);
      r.z = r.z * cos(rate) - up.z * sin(rate);
      break;

   case '6':
      up.x = up.x * cos(-rate) + r.x * sin(-rate);
      up.y = up.y * cos(-rate) + r.y * sin(-rate);
      up.z = up.z * cos(-rate) + r.z * sin(-rate);

      r.x = r.x * cos(-rate) - up.x * sin(-rate);
      r.y = r.y * cos(-rate) - up.y * sin(-rate);
      r.z = r.z * cos(-rate) - up.z * sin(-rate);
      break;

   case ',':
      // cout << "radius: " << radius << endl;

      radius = radius + SCALING_FACTOR;
      if (radius >= 1.0f / sqrt(2))
      {
         radius = 1.0f / sqrt(2);
      }

      // cout << "After radius: " << radius << endl;
      break;


   case '.':
      radius = radius - SCALING_FACTOR;
      if (radius < 0)
      {
         radius = 0;
      }
      break;


      // Control center (location where the eye is looking at)
      // control centerx

      // Controlling the object

   case 'a':
      rotateX = rotateX - 8;
      break;
   case 'd':
      rotateX = rotateX + 8;
      break;
   // case 's':
   //    eye.y -= v;
   //    break;
   // case 'w':
   //    eye.y += v;
   //    break;

   // Control exit
   case 27:    // ESC key
      exit(0); // Exit window
      break;
   }
   glutPostRedisplay(); // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x, int y)
{
   switch (key)
   {
   case GLUT_KEY_UP: // down arrow key
      eye.x += center.x * rate;
      eye.y += center.y * rate;
      eye.z += center.z * rate;
      break;
   case GLUT_KEY_DOWN: // up arrow key
      eye.x -= center.x * rate;
      eye.y -= center.y * rate;
      eye.z -= center.z * rate;
      break;

   case GLUT_KEY_RIGHT:
      eye.x += r.x * rate;
      eye.y += r.y * rate;
      eye.z += r.z * rate;
      break;
   case GLUT_KEY_LEFT:
      eye.x -= r.x * rate;
      eye.y -= r.y * rate;
      eye.z -= r.z * rate;
      break;

   case GLUT_KEY_PAGE_UP:
      eye.x += up.x * rate;
      eye.y += up.y * rate;
      eye.z += up.z * rate;
      break;
   case GLUT_KEY_PAGE_DOWN:
      eye.x -= up.x * rate;
      eye.y -= up.y * rate;
      eye.z -= up.z * rate;
      break;

   case GLUT_KEY_INSERT:
      break;

   case GLUT_KEY_HOME:
      break;
   case GLUT_KEY_END:
      break;

   default:
      break;
   }
   glutPostRedisplay();
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
   glutInit(&argc, argv);                                    // Initialize GLUT
   glutInitWindowSize(640, 640);                             // Set the window's initial width & height
   glutInitWindowPosition(50, 50);                           // Position the window's initial top-left corner
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
   glutCreateWindow("OpenGL 3D Drawing");                    // Create a window with the given title
   glutDisplayFunc(display);                                 // Register display callback handler for window re-paint
   glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
   glutKeyboardFunc(keyboardListener);                       // Register callback handler for normal-key event
   glutSpecialFunc(specialKeyListener);                      // Register callback handler for special-key event
   initGL();                                                 // Our own OpenGL initialization
   glutMainLoop();                                           // Enter the event-processing loop
   return 0;
}
