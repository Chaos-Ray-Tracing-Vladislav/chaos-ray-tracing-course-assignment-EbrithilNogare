#include <fstream>
#include <iostream>
#include <iomanip>
#include <string> 
#include <sstream>
#include "main.hpp";


const int imageWidth = 1920/1;
const int imageHeight = 1080/1;
const int maxColorComponent = 255;

Vector3 cameraPosition = Vector3(0, 0, 3);
Vector3 cameraRotation = Vector3(0, 0, 0); //pitch, yaw, roll in degrees

Triangle triangles[] = {
    /*/ // switch - triangle X icosphere
    Triangle(Vector3(-1.75, -1.75, -3),Vector3(1.75, -1.75, -3),Vector3(0, 1.75, -3)),
    /*/
    Triangle(Vector3(0.000000,-1.000000,0.000000),Vector3(0.723600,-0.447215,0.525720),Vector3(0.723600,-0.447215,-0.525720)),
    Triangle(Vector3(0.276385,0.447215,0.850640),Vector3(-0.723600,0.447215,0.525720),Vector3(0.000000,1.000000,0.000000)),
    Triangle(Vector3(0.276385,0.447215,0.850640),Vector3(0.894425,0.447215,0.000000),Vector3(0.000000,1.000000,0.000000)),
    Triangle(Vector3(0.276385,0.447215,-0.850640),Vector3(0.894425,0.447215,0.000000),Vector3(0.000000,1.000000,0.000000)),
    Triangle(Vector3(-0.723600,0.447215,-0.525720),Vector3(0.276385,0.447215,-0.850640),Vector3(0.000000,1.000000,0.000000)),
    Triangle(Vector3(-0.723600,0.447215,0.525720),Vector3(-0.723600,0.447215,-0.525720),Vector3(0.000000,1.000000,0.000000)),
    Triangle(Vector3(0.723600,-0.447215,0.525720),Vector3(0.276385,0.447215,0.850640),Vector3(0.894425,0.447215,0.000000)),
    Triangle(Vector3(0.723600,-0.447215,0.525720),Vector3(-0.276385,-0.447215,0.850640),Vector3(0.276385,0.447215,0.850640)),
    Triangle(Vector3(-0.276385,-0.447215,0.850640),Vector3(0.276385,0.447215,0.850640),Vector3(-0.723600,0.447215,0.525720)),
    Triangle(Vector3(-0.276385,-0.447215,0.850640),Vector3(-0.894425,-0.447215,0.000000),Vector3(-0.723600,0.447215,0.525720)),
    Triangle(Vector3(-0.894425,-0.447215,0.000000),Vector3(-0.723600,0.447215,0.525720),Vector3(-0.723600,0.447215,-0.525720)),
    Triangle(Vector3(-0.894425,-0.447215,0.000000),Vector3(-0.276385,-0.447215,-0.850640),Vector3(-0.723600,0.447215,-0.525720)),
    Triangle(Vector3(-0.276385,-0.447215,-0.850640),Vector3(-0.723600,0.447215,-0.525720),Vector3(0.276385,0.447215,-0.850640)),
    Triangle(Vector3(-0.276385,-0.447215,-0.850640),Vector3(0.723600,-0.447215,-0.525720),Vector3(0.276385,0.447215,-0.850640)),
    Triangle(Vector3(0.723600,-0.447215,-0.525720),Vector3(0.276385,0.447215,-0.850640),Vector3(0.894425,0.447215,0.000000)),
    Triangle(Vector3(0.723600,-0.447215,0.525720),Vector3(0.723600,-0.447215,-0.525720),Vector3(0.894425,0.447215,0.000000)),
    Triangle(Vector3(0.000000,-1.000000,0.000000),Vector3(-0.276385,-0.447215,0.850640),Vector3(-0.894425,-0.447215,0.000000)),
    Triangle(Vector3(0.000000,-1.000000,0.000000),Vector3(0.723600,-0.447215,0.525720),Vector3(-0.276385,-0.447215,0.850640)),
    Triangle(Vector3(0.000000,-1.000000,0.000000),Vector3(-0.894425,-0.447215,0.000000),Vector3(-0.276385,-0.447215,-0.850640)),
    /**/
};

Vector3 rotateVector(const Vector3& vec, const Vector3& rotation) {
    float pitch = degreesToRadians(rotation.x);
    float yaw = degreesToRadians(rotation.y);
    float roll = degreesToRadians(rotation.z);

    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);
    float cosRoll = cos(roll);
    float sinRoll = sin(roll);

    float m00 = cosYaw * cosRoll;
    float m01 = -cosYaw * sinRoll;
    float m02 = sinYaw;
    float m10 = sinPitch * sinYaw * cosRoll + cosPitch * sinRoll;
    float m11 = -sinPitch * sinYaw * sinRoll + cosPitch * cosRoll;
    float m12 = -sinPitch * cosYaw;
    float m20 = -cosPitch * sinYaw * cosRoll + sinPitch * sinRoll;
    float m21 = cosPitch * sinYaw * sinRoll + sinPitch * cosRoll;
    float m22 = cosPitch * cosYaw;

    return Vector3(
        vec.x * m00 + vec.y * m01 + vec.z * m02,
        vec.x * m10 + vec.y * m11 + vec.z * m12,
        vec.x * m20 + vec.y * m21 + vec.z * m22
    );
}

Vector3 RayTrace(int imageX, int imageY) {
    float x = (float)imageX / imageWidth;  // from 0 to 1
    float y = (float)imageY / imageHeight; // from 0 to 1
    
    x = (2.0f * x) - 1.0f; // from -1 to 1
    y = 1.0f - (2.0f * y); // from -1 to 1

	float aspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
    x *= aspectRatio; // from -ar to ar

    Vector3 ray = Vector3(x, y, -1).rotate(cameraRotation).normalize();

    Vector3 colorToReturn = Vector3(0,0,0);
    float closestDistance = std::numeric_limits<float>::infinity();
    
    for (auto& triangle : triangles) {
        float distance = triangle.intersect(ray, cameraPosition);
        if (distance < closestDistance) {
            closestDistance = distance;
            colorToReturn = Vector3(1, 1, 1) * abs(ray.dot(triangle.normal())) / closestDistance;
        }
    }

    return colorToReturn;
}

int colorFromDecimalToWholeRepresentation(float value) {
    value = std::min(1.0f, std::max(0.0f, value));
    return (int)std::round(value * maxColorComponent);
}

std::string colorToPPMFormat(Vector3 color) {
    return std::to_string(colorFromDecimalToWholeRepresentation(color.x)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.y)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.z));
}

void renderFrame(int frameNumber) {
    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << frameNumber;
    std::string fileName = "output/frame_" + ss.str() + ".ppm";

    std::ofstream ppmFileStream(fileName, std::ios::out | std::ios::binary);
    if (!ppmFileStream.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return;
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
}

int main() {
    int totalFrames = 300;
    for (int frameNumber = 0; frameNumber < totalFrames; ++frameNumber) {
        cameraPosition.x = sin(degreesToRadians(360.0f * (float)frameNumber / (float)totalFrames)) * 2.0f;
        cameraPosition.z = cos(degreesToRadians(360.0f * (float)frameNumber / (float)totalFrames)) * 2.0f;
        cameraRotation.y = 360.0f * (float)frameNumber / (float)totalFrames;
        // cameraRotation.y = 30.0f;

        renderFrame(frameNumber);
        std::cout << "frame " << frameNumber << " done\n";
    }
    return 0;
}
