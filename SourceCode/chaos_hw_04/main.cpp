#include <fstream>
#include <cmath>
#include <iostream>
#include <string> 
#include "main.hpp";

const int imageWidth = 1920 / 10;
const int imageHeight = 1080 / 10;
const int maxColorComponent = 255;

Vector3 RayTrace(int x, int y) {
    return Vector3(.5, .2, 0);
}

int colorFromDecimalToWholeRepresentation(float value) {
    value = std::min(1.0f, std::max(0.0f, value));
    return (int)std::round(value * maxColorComponent);
}

std::string colorToPPMFormat(Vector3 color) {
    return std::to_string(colorFromDecimalToWholeRepresentation(color.x)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.y)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.z));
}

int main() {
    /*
    std::ofstream ppmFileStream("output.ppm", std::ios::out | std::ios::binary);
    if (!ppmFileStream.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    // init ppm
    ppmFileStream << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColorComponent << "\n";

    for (int imageY = 0; imageY < imageHeight; ++imageY) {
        for (int imageX = 0; imageX < imageWidth; ++imageX) {
            Vector3 color = RayTrace(imageX, imageY);
            ppmFileStream << colorToPPMFormat(color) << " ";
        }
        ppmFileStream << "\n";
    }
    ppmFileStream.close();
    */

    // Homework tasks

    Vector3 task2_1 = Vector3(3.5, 0, 0).cross(Vector3(1.75, 3.5, 0));
    std::cout << "task2_1: " << task2_1.x << " " << task2_1.y << " " << task2_1.z << "\n";

    Vector3 task2_2 = Vector3(3, -3, 1).cross(Vector3(4, 9, 3));
    std::cout << "task2_2: " << task2_2.x << " " << task2_2.y << " " << task2_2.z << "\n";

    float task2_3 = Vector3(3, -3, 1).area(Vector3(4, 9, 3));
    std::cout << "task2_3: " << task2_3 << "\n";

    float task2_4 = Vector3(3, -3, 1).area(Vector3(-12, 12, -4));
    std::cout << "task2_4: " << task2_4 << "\n";

    Triangle task3_1_triangle = Triangle(Vector3(-1.75, -1.75, -3), Vector3(1.75, -1.75, -3), Vector3(0, 1.75, -3));
    Vector3 task3_1_normal = task3_1_triangle.normal();
    std::cout << "task3_1: " << task3_1_normal.x << " " << task3_1_normal.y << " " << task3_1_normal.z << "\n";

    Triangle task3_2_triangle = Triangle(Vector3(0, 0, -1), Vector3(1, 0, 1), Vector3(-1, 0, 1));
    Vector3 task3_2_normal = task3_2_triangle.normal();
    std::cout << "task3_2: " << task3_2_normal.x << " " << task3_2_normal.y << " " << task3_2_normal.z << "\n";

    Triangle task3_3_triangle = Triangle(Vector3(0.56, 1.11, 1.23), Vector3(0.44, -2.368, -0.54), Vector3(-1.56, 0.15, -1.92));
    Vector3 task3_3_normal = task3_3_triangle.normal();
    std::cout << "task3_3: " << task3_3_normal.x << " " << task3_3_normal.y << " " << task3_3_normal.z << "\n";

    std::cout << "task4_1: " << task3_1_triangle.area() << "\n";

    std::cout << "task4_2: " << task3_2_triangle.area() << "\n";
    
    std::cout << "task4_3: " << task3_3_triangle.area() << "\n";


    return 0;
}
