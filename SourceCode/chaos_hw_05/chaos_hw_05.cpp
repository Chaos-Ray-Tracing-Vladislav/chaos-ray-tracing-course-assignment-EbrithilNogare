#include <fstream>
#include <iostream>
#include <string> 
#include "main.hpp";

const int imageWidth = 1920/2;
const int imageHeight = 1080/2;
const int maxColorComponent = 255;

const Vector3 cameraPosition = Vector3(0, 0, 0);
const Vector3 cameraRotation = Vector3(0, 0, 0); // looks towards -z

Triangle triangles[] = {
	Triangle(Vector3(-0.115597,  1.738339,  -1.711655), Vector3(-2.112528,  1.738339,  -1.711655), Vector3(-1.114063, -0.258592,  -1.711655)),
	Triangle(Vector3(2.104024,  1.738339, -1.711655), Vector3(0.107094,  1.738339, -1.711655), Vector3(1.105559,  -0.258592, -1.711655)),
	Triangle(Vector3(0.989961,  0.981457, -1.878030), Vector3(-1.006969, 0.981457,  -1.878030), Vector3(-0.008504, -1.015474,  -1.878030)),
};

Vector3 RayTrace(int imageX, int imageY) {
    float x = (float)imageX / imageWidth;  // from 0 to 1
    float y = (float)imageY / imageHeight; // from 0 to 1
    
    x = (2.0f * x) - 1.0f; // from -1 to 1
    y = 1.0f - (2.0f * y); // from -1 to 1

	float aspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
    x *= aspectRatio; // from -ar to ar

    Vector3 ray = Vector3(x, y, -1.0f).normalize();

    Vector3 colorToReturn = Vector3(0,0,0);
    float closestDistance = std::numeric_limits<float>::infinity();
    
    for (auto& triangle : triangles) {
        float distance = triangle.intersect(ray);
        if (distance < closestDistance) {
            closestDistance = distance;
            colorToReturn = triangle.normal().normalize();
        }
    }

    return colorToReturn / closestDistance;
}

int colorFromDecimalToWholeRepresentation(float value) {
    value = std::min(1.0f, std::max(0.0f, value));
    return (int)std::round(value * maxColorComponent);
}

std::string colorToPPMFormat(Vector3 color) {
    return std::to_string(colorFromDecimalToWholeRepresentation(color.x)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.y)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.z));
}

int main() {
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

    return 0;
}
