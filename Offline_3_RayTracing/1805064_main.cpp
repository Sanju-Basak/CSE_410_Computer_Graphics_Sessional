#define _USE_MATH_DEFINES

#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <bits/stdc++.h>
#include <fstream>
#include "1805064_Objects.hpp"
#include "bitmap_image.hpp"
#define ROTATE_ANGLE PI/180
#define INF numeric_limits<double>::infinity()

using namespace std;

/* Global variables */

Point pos;
Point up;
Point r;
Point look;

Point cameraCurrent;
// vector<vector<Point>> pointBuffer;

double nearDist;
double farDist;
double fovY;
double fovX;
double aspectRatio;

int numOfRecursion;
int noOfPixels;

bool isAxes;
extern vector<Object*> objects;
extern vector<PointLight*> pointLights;
extern vector<SpotLight*> spotLights;
void init() {
   isAxes = true;

	pos = Point(0,-160,60);  // set the camera position
   r= Point(1,0,0);
   look= Point(0, 1, 0);
   up = Point(0, 0, 1);

	//bitmapImageCount = 0;

	/* clearing the screen */
	glClearColor(0, 0, 0, 0);  // color = black

	/* setting up projection here */
	/* loading the PROJECTION matrix */
	glMatrixMode(GL_PROJECTION);

	/* initializing the matrix */
	glLoadIdentity();

    /* setting the camera perspective by providing necessary parameters */
	//gluPerspective(fovY, aspectRatio, 1.0, 1000.0);
	gluPerspective(fovY, aspectRatio, nearDist, farDist);
}

void drawAxes(double axisLength) {
    if(!isAxes) {
        return ;
    }

    /* drawing x-axis */
	glColor3f(1.0, 0.0, 0.0);  // red
    glBegin(GL_LINES);
    {
        glVertex3f(axisLength, 0.0, 0.0);
        glVertex3f(-axisLength, 0.0, 0.0);
    }
    glEnd();

    /* drawing y-axis */
    glColor3f(0.0, 1.0, 0.0);    //green
    glBegin(GL_LINES);
    {
        glVertex3f(0.0, axisLength, 0.0);
        glVertex3f(0.0, -axisLength, 0.0);
    }
    glEnd();

    /* drawing z-axis */
    glColor3f(0.0, 0.0, 1.0); //blue
    glBegin(GL_LINES);
    {
        glVertex3f(0.0, 0.0, axisLength);
        glVertex3f(0.0, 0.0, -axisLength);
    }
    glEnd();
}

void loadData(){
   ifstream sceneFile;
   int noOfObjects, noOfPointLights, noOfSpotLights;

   sceneFile.open("sample.txt");

   if(!sceneFile.is_open()) {
        cout << "input.is_open(): failed to open input file" << endl;
        exit(EXIT_FAILURE);
    }
   sceneFile >> nearDist >> farDist >> fovY >> aspectRatio;
   sceneFile >> numOfRecursion;
   sceneFile >> noOfPixels;

   //Adding CheckerBoard
   double tileWidth;
   sceneFile >> tileWidth;
   double a, d, s, r;
   s=0;
   sceneFile >> a >> d >> r;
   Object *checkerBoard = new CheckerBoard(1000, tileWidth);
   checkerBoard->setCoEfficients(a, d, s, r);
   objects.push_back(checkerBoard);
   checkerBoard = NULL;
   sceneFile >> noOfObjects;
   Object *object= NULL;
   for(int i=0; i<noOfObjects; i++) {
        string objectType;
        double a, d, s, r;
         double shine;
        sceneFile >> objectType;
        if(objectType == "sphere") {
            Point center, color;
            double radius;
            sceneFile >> center.x >> center.y >> center.z;
            sceneFile >> radius;
            sceneFile >> color.x >> color.y >> color.z;
            object = new Sphere(center, radius, color);
			sceneFile >> a >> d >> s >> r;
        	sceneFile >> shine;
         	object->setShininess(shine);
         	object->setCoEfficients(a, d, s, r);
         	objects.push_back(object);
        }
        else if(objectType == "pyramid") {
			Point lb1, color;
			double width, height;
			sceneFile >> lb1.x >> lb1.y >> lb1.z;
			sceneFile >> width >> height;
			sceneFile >> color.x >> color.y >> color.z;
			sceneFile >> a >> d >> s >> r;
			sceneFile >> shine;
			Point lb2, lb3, lb4, u1;
			lb2= lb1+ Point(width, 0, 0);
			lb3= lb1+ Point(width, width, 0);
			lb4= lb1+ Point(0, width, 0);
			u1= lb1+ Point(width/2, width/2, height);
			object= new Triangle(lb1, lb2, u1, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);
			object= new Triangle(lb2, lb3, u1, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);
			object= new Triangle(lb3, lb4, u1, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);
			object= new Triangle(lb4, lb1, u1, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);
			object= new Triangle(lb1, lb2, lb3, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);
			object= new Triangle(lb1, lb3, lb4, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object); 
        }

		else if(objectType == "cube"){
			Point lb, color;
			double width;
			sceneFile >> lb.x >> lb.y >> lb.z;
			sceneFile >> width;
			sceneFile >> color.x >> color.y >> color.z;
			sceneFile >> a >> d >> s >> r;
			sceneFile >> shine;
			Point lb1, lb2, lb3, lb4, u1, u2, u3, u4;
			lb1 = lb;
			lb2 = lb + Point(width, 0, 0);
			lb3 = lb + Point(width, width, 0);
			lb4 = lb + Point(0, width, 0);
			u1 = lb + Point(0, 0, width);
			u2 = lb + Point(width, 0, width);
			u3 = lb + Point(width, width, width);
			u4 = lb + Point(0, width, width);

			object= new Triangle(lb1, lb2, u1, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb2, u2, u1, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb2, u2, u3,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb2, u3, lb3,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb3, u3, u4,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb3, u4, lb4, color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb1, u1, u4,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb1, u4, lb4,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb1, lb2, lb3,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(lb1, lb3, lb4,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(u1, u2, u3,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);

			object= new Triangle(u1, u3, u4,  color);
			object->setShininess(shine);
			object->setCoEfficients(a, d, s, r);
			objects.push_back(object);
		}
         
   }
   object= NULL;
   sceneFile >> noOfPointLights;

   for(int i=0; i< noOfPointLights; i++){
      Point position;
      double falloff;
      sceneFile >> position.x >> position.y >> position.z;
      sceneFile >> falloff;
      PointLight *light= new PointLight(position, falloff);
      pointLights.push_back(light);
   }

   sceneFile >> noOfSpotLights;

   for (int i=0; i< noOfSpotLights; i++){
	  Point position, direction;
	  double falloff, cutoff;
	  sceneFile >> position.x >> position.y >> position.z;
	  sceneFile >> falloff;
	  sceneFile >> direction.x >> direction.y >> direction.z;
	  sceneFile >> cutoff;
	  SpotLight *light= new SpotLight(position, direction, falloff, cutoff);
	  spotLights.push_back(light);
   }

   sceneFile.close();
}

void capture(){
   cout<< "Capturing bitmap image" << endl;

   cameraCurrent= pos;

   bitmap_image image(noOfPixels, noOfPixels);
   for(int i=0; i<noOfPixels; i++){
      for(int j=0; j< noOfPixels; j++){
         image.set_pixel(i, j, 0, 0, 0);
      }
   }

   fovX= fovY*aspectRatio;
   //cout<< "Near DIst: "<< nearDist;
   double planeDistance= nearDist;
   double height= 2*planeDistance*tan(fovY*ROTATE_ANGLE/2);
   double width= 2*planeDistance*tan(fovX*ROTATE_ANGLE/2);
   Point topLeft= pos+look*planeDistance+ up*(height/2)- r*(width/2);

   double pixelWidth= width/(noOfPixels*1.0);
   double pixelHeight= height/(noOfPixels*1.0);

   topLeft= topLeft+ r*(pixelWidth/2.0) - up*(pixelHeight/2.0);

   int nearestObjectIndex= -1;
   double t, tMin;
	// pointBuffer.resize(noOfPixels, vector<Point>(noOfPixels));
   for(int i=0; i<noOfPixels; i++){
      for(int j=0; j<noOfPixels; j++){
         Point currentPixel= topLeft+ r*pixelWidth*i - up*pixelHeight*j;
         Point direction= currentPixel-pos;
		//  pointBuffer[i][j]= currentPixel;
		 //cout<< currentPixel.x<< " " << currentPixel.y<< " "<< currentPixel.z<< endl; 
         Ray ray(currentPixel, direction);

         Point color;
         tMin= -1;
         nearestObjectIndex= -1;
         for(int k=0; k<objects.size(); k++){
            t= objects[k]->intersect(ray, color, 0);
            if(t>0 && (nearestObjectIndex == -1 ||  t<tMin)){
               tMin= t;
               nearestObjectIndex= k;
            }
         }
		 
		 //cout<< tMin<< endl;
		//  if(tMin != -1){
		// 	Point intersectionPoint = ray.origin + ray.dir * tMin;
		//  	//intersectionPoint.print();
		// 	image.set_pixel(i, j, 255, 255, 255);
		// 	continue;
		//  }
         if(nearestObjectIndex != -1){
            color= Point(0, 0, 0);
            double t= objects[nearestObjectIndex]->intersect(ray, color, 1);
			// cout<< color.x<< color.y<< color.z << endl;

            if(color.x > 1) color.x = 1;
				if(color.y > 1) color.y = 1;
				if(color.z > 1) color.z = 1;

				if(color.x < 0) color.x = 0;
				if(color.y < 0) color.y = 0;
				if(color.z < 0) color.z = 0;

            image.set_pixel(i, j, 255*color.x, 255*color.y, 255*color.z);
         }
      }
   }

    image.save_image("Out.bmp");
	cout<<"Saving Image"<<endl;	

}


void keyboardListener(unsigned char key, int x, int y) {
	switch(key) {
        case '0':
            capture();
            break;
		case '1':
		    /* rotate/look left */
		    r = r*cos(ROTATE_ANGLE)+(up^r)*sin(ROTATE_ANGLE);
		    look = look*cos(ROTATE_ANGLE)+(up^look)*sin(ROTATE_ANGLE);
			break;
        case '2':
            /* rotate/look right */
		    r = r*cos(-ROTATE_ANGLE)+(up^r)*sin(-ROTATE_ANGLE);
		    look = look*cos(-ROTATE_ANGLE)+(up^look)*sin(-ROTATE_ANGLE);
			break;
        case '3':
            /* look up */
		    look = look*cos(ROTATE_ANGLE)+(r^look)*sin(ROTATE_ANGLE);
		    up = up*cos(ROTATE_ANGLE)+(r^up)*sin(ROTATE_ANGLE);
			break;
        case '4':
            /* look down */
		    look = look*cos(-ROTATE_ANGLE)+(r^look)*sin(-ROTATE_ANGLE);
		    up = up*cos(-ROTATE_ANGLE)+(r^up)*sin(-ROTATE_ANGLE);
			break;
        case '5':
            /* tilt clockwise */
		    up = up*cos(ROTATE_ANGLE)+(look^up)*sin(ROTATE_ANGLE);
		    r = r*cos(ROTATE_ANGLE)+(look^r)*sin(ROTATE_ANGLE);
			break;
        case '6':
            /* tilt counter-clockwise */
		    up = up*cos(-ROTATE_ANGLE)+(look^up)*sin(-ROTATE_ANGLE);
		    r = r*cos(-ROTATE_ANGLE)+(look^r)*sin(-ROTATE_ANGLE);
			break;
		default:
		    break;
	}
	glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_DOWN:
            /* down arrow key: move backward */
            pos = pos-look;
			break;
		case GLUT_KEY_UP:
            /* up arrow key: move forward */
            pos = pos+look;
			break;
		case GLUT_KEY_RIGHT:
		    /* right arrow key: move right */
		    pos = pos+r;
			break;
		case GLUT_KEY_LEFT:
		    /* left arrow key: move left */
		    pos = pos-r;
			break;
		case GLUT_KEY_PAGE_UP:
		    /* page up key: move upward */
		    pos = pos+ up;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    /* page down key: move downward */
		    pos = pos- up;
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

void mouseListener(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN) {
                isAxes = !isAxes;
			}
			break;
        case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN) {

			}
			break;
		default:
			break;
	}
}

void display() {
	/* clearing the display */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);  // color = black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* setting up the camera here */
	/* loading the correct matrix: MODEL-VIEW matrix */
	glMatrixMode(GL_MODELVIEW);

	/* initializing the matrix */
	glLoadIdentity();

	/* setting the camera by providing necessary parameters */
	gluLookAt(pos.x, pos.y, pos.z, 
			pos.x + look.x, pos.y + look.y, pos.z + look.z, 
			up.x, up.y, up.z);

	/* again, selecting MODEL-VIEW matrix */
	glMatrixMode(GL_MODELVIEW);

    /* adding objects from here */
	/* adding axes */
	drawAxes(300.0);

	// /* adding objects */
	for(int i=0; i<objects.size(); i++) {
        objects[i]->draw();
	}

	// /* adding point buffer */
	// drawing point buffer lines for debugging
	// for(int i=0; i<pointBuffer.size(); i++){
	// 	for(int j=0; j< pointBuffer[i].size(); j++){
	// 		glColor3d(1, 1, 1);
	// 		//cout<< pointBuffer[i][j].x << " "<< pointBuffer[i][j].y<< " " << pointBuffer[i][j].z<< endl;
	// 		glBegin(GL_LINES);
	// 			glVertex3d(cameraCurrent.x, cameraCurrent.y, cameraCurrent.z);
	// 			glVertex3d(pointBuffer[i][j].x, pointBuffer[i][j].y, pointBuffer[i][j].z);
	// 		glEnd();
	// 	}
	// }

	// /* adding lights */
	for(int i=0; i<pointLights.size(); i++) {
        pointLights[i]->draw();
	}

	for(int i=0; i<spotLights.size(); i++) {
		spotLights[i]->draw();
	}

	//gluPerspective(fovY, aspectRatio, nearDist, farDist);
	/* ADD this line in the end: if you use double buffer (i.e. GL_DOUBLE) */
	glutSwapBuffers();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);  // depth, double buffer, rgb color

	glutCreateWindow("Ray Tracing");
    loadData();
	init();  // initialization

	glEnable(GL_DEPTH_TEST);  // enable depth testing

	glutDisplayFunc(display);  // display(): callback function

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();  // the main loop of OpenGL
	return 0;
}