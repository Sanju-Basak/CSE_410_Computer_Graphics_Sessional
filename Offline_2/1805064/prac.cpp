#include <iostream>
#include <algorithm>

struct Point {
    float x, y;
};

bool comparePoints(const Point& p1, const Point& p2) {
    return (p1.y != p2.y) ? (p1.y < p2.y) : (p1.x < p2.x);
}

int main() {
    Point p1 = {0, 0};
    Point p2 = {.13, 0};
    Point p3 = {0, .13};
    
    // Arrange the points
    Point points[] = {p1, p2, p3};
    std::sort(points, points + 3, comparePoints);
    
    // Display the rearranged points
    for (const auto& p : points) {
        std::cout << "x: " << p.x << ", y: " << p.y << std::endl;
    }

    return 0;
}
