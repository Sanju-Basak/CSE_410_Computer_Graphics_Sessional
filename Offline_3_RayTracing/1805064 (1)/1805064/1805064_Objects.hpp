#include <bits/stdc++.h>
#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include "1805064_Point.hpp"
#define PI 3.14159265f
#define INF numeric_limits<double>::infinity()
#define EPS 1e-9
#define NO_OF_TILES 20  // number of tiles in checkerboard
using namespace std;

extern Point pos;
extern int numOfRecursion;

class Object;

class Ray
{
public:
    Point origin;
    Point dir;
    Ray(){};
    Ray(Point s, Point d)
    {
        origin = s;
        dir = d;
        dir.normalize();
    }
    void printOrigin()
    {
        cout << origin.x << " " << origin.y << " " << origin.z << endl;
    }

    void printDirection()
    {
        cout << dir.x << " " << dir.y << " " << dir.z << endl;
    }
};

class CoEfficients
{
public:
    double ambient;
    double diffuse;
    double specular;
    double reflection;
    CoEfficients(){};
    CoEfficients(double a, double d, double s, double r)
    {
        ambient = a;
        diffuse = d;
        specular = s;
        reflection = r;
    }

    void print()
    {
        cout << "CoEfficients:\n";
        cout << ambient << " " << diffuse << " " << specular << " " << reflection << endl;
    }
};

class PointLight
{
public:
    Point position;
    double falloff;
    PointLight(){};
    PointLight(Point p, double f)
    {
        position = p;
        falloff = f;
    }

    void draw()
    {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glColor3f(1, 1, 1);
        glutSolidSphere(5, 30, 30);
        glPopMatrix();
    }

    void print(){
        cout<< "Point Light: "<< endl;
        cout<< "Position: ";
        position.print();
        cout<< "Falloff: "<< falloff<< endl;
    }
};

class SpotLight
{
public:
    Point position;
    Point direction;
    double falloff;
    double cutoff;
    SpotLight(){};
    SpotLight(Point p, Point d, double f, double c)
    {
        position = p;
        direction = d;
        falloff = f;
        cutoff = c;
    }

    void draw()
    {

        GLUquadric *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);

        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);

        // Calculate the rotation angle and axis
        float coneDirectionLength = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        float angle = acos(direction.z / coneDirectionLength) * 180.0f / M_PI;
        float axisX = -direction.y / coneDirectionLength;
        float axisY = direction.x / coneDirectionLength;
        float axisZ = 0.0f;

        glRotatef(angle, axisX, axisY, axisZ);

        // Draw the cone
        glColor3f(1, 1, 0);
        gluCylinder(quadric, 5, 0.0f, 40, 50, 50);

        glPopMatrix();
        gluDeleteQuadric(quadric);
    }
};

vector<PointLight *> pointLights;
vector<SpotLight *> spotLights;
vector<Object *> objects;
class Object
{
public:
    Point reference_point;
    double height, width, length;
    Point color;
    CoEfficients coEfficients;
    double shininess;
    Object(){};
    void setShininess(double s)
    {
        shininess = s;
    }
    void setCoEfficients(double a, double d, double s, double r)
    {
        coEfficients = CoEfficients(a, d, s, r);
    }

    virtual void draw(){};

    virtual double intersectingPoint(Ray ray, Point &color, int level)
    {
        return -1;
    };

    virtual Ray getNormal(Point intersectionPoint, Ray incedentRay)
    {
        return Ray();
    }

    virtual double intersect(Ray ray, Point &color, int level)
    {
        double t = intersectingPoint(ray, color, level);

        if (t == -1)
            return -1;
        if (level == 0)
        {
            return t;
        }
        Point intersectionPoint = ray.origin + ray.dir * t;
        Point colorAtIntersection = getColorAt(intersectionPoint);
        color = colorAtIntersection * coEfficients.ambient;

        double lambert = 0;
        double phong = 0;

        for (int i = 0; i < pointLights.size(); i++)
        {

            Point lightPosition = pointLights[i]->position;
            Point lightDirection = intersectionPoint - lightPosition;

            // cast incident ray, from light position to intersection point
            Ray lightRay = Ray(lightPosition, lightDirection);

            // calculate normal at intersectionPoint
            Ray normal = getNormal(intersectionPoint, lightRay);


            double t2 = (intersectionPoint - lightPosition).length();
            if (t2 < 1e-5)
                continue;

            //calculate scaling factor
            double scaling_factor = exp(-pointLights[i]->falloff * t2 * t2);

            bool obscured = false;

            for (Object *obj : objects)
            {
                double t3 = obj->intersectingPoint(lightRay, color, 0);
                if (t3 > 0 && t3 + 1e-5 < t2)
                {
                    obscured = true;
                    break;
                }
            }

            if (!obscured)
            {

                //lambert value
                lambert = lambert+ max(0.0, -lightRay.dir.dot(normal.dir))* scaling_factor;

                // find reflected ray
                Ray reflection = Ray(intersectionPoint, lightRay.dir - normal.dir * 2 * (lightRay.dir.dot(normal.dir)));
                double tempPhong =  max(0.0, -ray.dir.dot(reflection.dir)) * scaling_factor;
                tempPhong= pow(tempPhong, shininess);
                phong= phong+ tempPhong;
            }
        }

        for (int i = 0; i < spotLights.size(); i++)
        {

            Point lightPosition = spotLights[i]->position;
            Point lightDirection = intersectionPoint - lightPosition;
            lightDirection.normalize();

            double dot = lightDirection.dot(spotLights[i]->direction);
            double angle = acos(abs(dot / (lightDirection.length() * spotLights[i]->direction.length()))) * (180.0 / PI);

            if (angle < spotLights[i]->cutoff)
            {

                Ray lightRay = Ray(lightPosition, lightDirection);
                Ray normal = getNormal(intersectionPoint, lightRay);

                Ray reflection = Ray(intersectionPoint, lightRay.dir - normal.dir * 2 * (lightRay.dir.dot(normal.dir)));

                double t2 = (intersectionPoint - lightPosition).length();
                if (t2 < 1e-5)
                    continue;

                double scaling_factor = exp(-spotLights[i]->falloff * t2 * t2);

                bool obscured = false;

                for (Object *obj : objects)
                {
                    double t3 = obj->intersectingPoint(lightRay, color, 0);
                    if (t3 > 0 && t3 + 1e-5 < t2)
                    {
                        obscured = true;
                        break;
                    }
                }

                if (!obscured)
                {

                    lambert = lambert+  max(0.0, -lightRay.dir.dot(normal.dir))* scaling_factor;
                    double tempPhong = max(0.0, -ray.dir.dot(reflection.dir))* scaling_factor;
                    tempPhong= pow(tempPhong, shininess);
                    phong= phong+ tempPhong;
                }
            }
        }

        //adding lambert and phong to color
        color.x= color.x+ colorAtIntersection.x * coEfficients.diffuse * lambert + coEfficients.specular * phong;
        color.y= color.y+ colorAtIntersection.y * coEfficients.diffuse * lambert + coEfficients.specular * phong;
        color.z= color.z+ colorAtIntersection.z * coEfficients.diffuse * lambert + coEfficients.specular * phong;

        // adding recursive reflection

        if(level< numOfRecursion){
            Ray normal= getNormal(intersectionPoint, ray);

            Ray reflectionRay= Ray(intersectionPoint, ray.dir- normal.dir*2* (ray.dir.dot(normal.dir)));

            //slightly forwarding the point

            reflectionRay.origin= reflectionRay.origin + reflectionRay.dir* EPS;

            //finding the nearest intersection object

            int nearestObjIndex= -1;
            double t= -1;
            double tmin= INF;

            for(int k=0; k< objects.size(); k++){
                t= objects[k]->intersect(reflectionRay, color, 0);
                if(t> 0 && t< tmin){
                    tmin= t;
                    nearestObjIndex= k;
                }
            }

            if(nearestObjIndex != -1){
                Point tempColor(0, 0, 0);
                double t= objects[nearestObjIndex]->intersect(reflectionRay, tempColor, level +1);

                color= color+ tempColor * coEfficients.reflection;
            }
        }

        

        return t;
    };

    virtual Point getColorAt(Point intersectionPoint)
    {
        return color;
    }
};

// vector<Light> lights;

class Sphere : public Object
{
public:
    Sphere(Point center, double radius, Point color)
    {
        reference_point = center;
        length = radius;
        width = radius;
        height = radius;
        this->color = color;
    }
    void draw()
    {
        int slices = 24;
        int stacks = 20;
        double radius = length;

        Point points[stacks + 1][slices + 1];
        for (int j = 0; j <= stacks; j++)
        {
            double phi = -M_PI / 2.0 + j * M_PI / stacks;
            double r = radius * cos(phi);
            double h = radius * sin(phi);
            for (int i = 0; i < slices + 1; i++)
            {
                double theta = i * 2.0 * M_PI / slices;
                points[j][i].x = r * cos(theta);
                points[j][i].y = r * sin(theta);
                points[j][i].z = h;
            }
        }

        // draw quads using generated points
        for (int i = 0; i < stacks; i++)
        {
            glPushMatrix();
            glTranslatef(reference_point.x, reference_point.y, reference_point.z);
            glColor3f(color.x, color.y, color.z);
            for (int j = 0; j < slices; j++)
            {
                glBegin(GL_QUADS);
                {
                    // upper hemisphere
                    glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                    // lower hemisphere
                    glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }

    double intersectingPoint(Ray ray, Point &color, int level)
    {
        ray.origin = ray.origin - reference_point; // adjust ray origin

        double a = 1.0;
        double b = 2 * (ray.dir.dot(ray.origin));
        double c = (ray.origin.dot(ray.origin)) - (length * length);

        double discriminant = pow(b, 2) - 4 * a * c;
        double t = -1;
        if (discriminant < 0)
        {
            t = -1;
        }
        else
        {


            double t1 = (-b - sqrt(discriminant)) / (2 * a);
            double t2 = (-b + sqrt(discriminant)) / (2 * a);

            if (t2 < t1)
                swap(t1, t2);

            if (t1 > 0)
            {
                t = t1;
            }
            else if (t2 > 0)
            {
                t = t2;
            }
            else
            {
                t = -1;
            }
        }


        return t;
    }

    Ray getNormal(Point intersectionPoint, Ray incedentRay)
    {
        Point normal = intersectionPoint - reference_point;
        // if(normal.dot(incedentRay.dir)< 0)
        //     normal= normal * (-1);
        return Ray(intersectionPoint, normal);
    }
};

double determinant(double mat[3][3])
{
    double det = 0;
    double v1 = mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]);
    double v2 = mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]);
    double v3 = mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
    det = v1 - v2 + v3;
    return det;
}

class Triangle : public Object
{
public:
    Point a, b, c;
    Triangle(Point a, Point b, Point c, Point color)
    {
        this->a = a;
        this->b = b;
        this->c = c;
        this->color = color;
    }
    void draw()
    {
        glPushMatrix();
        {
            glColor3f(color.x, color.y, color.z);
            glBegin(GL_TRIANGLES);
            {
                glVertex3f(a.x, a.y, a.z);
                glVertex3f(b.x, b.y, b.z);
                glVertex3f(c.x, c.y, c.z);
            }
        }
        glEnd();
        glPopMatrix();
    }

    double intersectingPoint(Ray ray, Point &color, int level)
    {
        double betaMat[3][3] = {
            {a.x - ray.origin.x, a.x - c.x, ray.dir.x},
            {a.y - ray.origin.y, a.y - c.y, ray.dir.y},
            {a.z - ray.origin.z, a.z - c.z, ray.dir.z}};
        double gammaMat[3][3] = {
            {a.x - b.x, a.x - ray.origin.x, ray.dir.x},
            {a.y - b.y, a.y - ray.origin.y, ray.dir.y},
            {a.z - b.z, a.z - ray.origin.z, ray.dir.z}};
        double tMat[3][3] = {
            {a.x - b.x, a.x - c.x, a.x - ray.origin.x},
            {a.y - b.y, a.y - c.y, a.y - ray.origin.y},
            {a.z - b.z, a.z - c.z, a.z - ray.origin.z}};
        double AMat[3][3]{
            {a.x - b.x, a.x - c.x, ray.dir.x},
            {a.y - b.y, a.y - c.y, ray.dir.y},
            {a.z - b.z, a.z - c.z, ray.dir.z}};

        double Adet = determinant(AMat);
        double beta = determinant(betaMat) / Adet;
        double gamma = determinant(gammaMat) / Adet;
        double t = determinant(tMat) / Adet;

        if (beta + gamma < 1 && beta > 0 && gamma > 0 && t > 0)
        {
            return t;
        }
        else
        {
            return -1;
        }
    }

    Ray getNormal(Point intersectionPoint, Ray incedentRay)
    {
        Point normal = (b - a) ^ (c - a);
        normal.normalize();

        if (incedentRay.dir.dot(normal) < 0)
            normal = normal * -1;
        return Ray(intersectionPoint, normal);
    }
};

class CheckerBoard : public Object
{
public:
    double floorWidth, tileWidth;
    CheckerBoard(double floorWidth, double tileWidth)
    {
        length = tileWidth;
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
    }
    void draw()
    {

        const int numTiles = 20;
        // Calculate the camera's position 

        float cameraX = pos.x;
        float cameraY = pos.y;


        float i= floor(cameraX/length);
        float j= floor(cameraY/length);

        for(int n=-10; n<=10; n++){
            for(int m= -10; m<=10; m++){
                glBegin(GL_POLYGON);

                if ((n + m) % 2 == 0)
                {
                    glColor3f(0.0f, 0.0f, 0.0f); // Black
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 1.0f); // White
                }

                glVertex3f(i + n * length, j + m * length, 0.0f);
                glVertex3f(i + (n + 1) * length, j + m * length, 0.0f);
                glVertex3f(i + (n + 1) * length, j + (m + 1) * length, 0.0f);
                glVertex3f(i + n * length, j + (m + 1) * length, 0.0f);

                glEnd();
            }
        }

    }

    Point getColorAt(Point intersectionPoint)
    {
        int x = floor(intersectionPoint.x / length);
        int y = floor(intersectionPoint.y / length);

        if ((x + y) % 2 == 0)
        {
            return Point(0, 0, 0);
        }
        else
        {
            return Point(1, 1, 1);
        }
    }

    double intersectingPoint(Ray ray, Point &color, int level)
    {
        Point normal = Point(0, 0, 1);
        double dotP = normal.dot(ray.dir);

        if (dotP == EPS)
            return -1;

        double t = -(normal.dot(ray.origin)) / dotP;

        if(t>1000){
            return -1;
        }

        Point p = ray.origin + ray.dir * t;

        int x = floor(pos.x / length);
        int y = floor(pos.y / length);


        return t;
    }

    Ray getNormal(Point intersectionPoint, Ray incedentRay)
    {
        Point normal = Point(0, 0, 1);
        // if (incedentRay.dir.z < 0)
        //     normal = normal * (-1);
        return Ray(intersectionPoint, normal);
    }
};
