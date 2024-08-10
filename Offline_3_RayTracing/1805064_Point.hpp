#include <bits/stdc++.h>
using namespace std;

class Point {
public:
    double x, y, z;
    Point() {
        x = y = z = 0;
    }
    Point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    void normalize() {
        double len = sqrt(x * x + y * y + z * z);
        x /= len;
        y /= len;
        z /= len;
    }


    void print() {
        cout << x << " " << y << " " << z << endl;
    }

    Point operator+(Point b) {
        return Point(x + b.x, y + b.y, z + b.z);
    }

    Point operator*(double b) {
        return Point(x * b, y * b, z * b);
    }

    Point operator-(Point b) {
        return Point(x - b.x, y - b.y, z - b.z);
    }

    Point operator/(double b) {
        return Point(x / b, y / b, z / b);
    }

    Point operator^(Point b) {
        return Point(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }

    double dot(Point b) {
        return x * b.x + y * b.y + z * b.z;
    }

    double length() {
        return sqrt(x * x + y * y + z * z);
    }

    void operator=(Point b) {
        x = b.x;
        y = b.y;
        z = b.z;
    }

};