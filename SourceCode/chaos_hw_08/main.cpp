#include <fstream>
#include <iostream>
#include <iomanip>
#include <string> 
#include <sstream>
#include <vector>
#include "main.hpp"
#include "include/rapidjson/document.h"

// Global variables
const int maxColorComponent = 255;

int imageWidth = 1920;
int imageHeight = 1080;
Vector3 defaultColor = Vector3(0, 0, 0);
Vector3 cameraPosition = Vector3(0, 0, 3);
Matrix3x3 cameraRotation = Matrix3x3(1, 0, 0, 0, 1, 0, 0, 0, 1);
std::vector<Triangle> triangles;
std::vector<Light> lights;

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

void WorldIntersection(const Vector3& ray, const Vector3& position, float& closestDistance, Vector3& intersectionPoint, Vector3& surfaceNormal)
{
    for (auto& triangle : triangles) {
        float distance = triangle.intersect(ray, position);
        if (distance < closestDistance) {
            closestDistance = distance;
            intersectionPoint = position + ray * distance;
            surfaceNormal = triangle.normal().normalize();
        }
    }
}

Vector3 RayTrace(int imageX, int imageY) {
    float x = (float)imageX / imageWidth;  // from 0 to 1
    float y = (float)imageY / imageHeight; // from 0 to 1

    x = (2.0f * x) - 1.0f; // from -1 to 1
    y = 1.0f - (2.0f * y); // from -1 to 1

    float aspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
    x *= aspectRatio; // from -ar to ar

    Vector3 ray = (cameraRotation * Vector3(x, y, -1)).normalize();

    float closestDistance = std::numeric_limits<float>::infinity();
    Vector3 intersectionPoint = Vector3(0, 0, 0);
    Vector3 surfaceNormal = Vector3(0, 0, 0);
    WorldIntersection(ray, cameraPosition, closestDistance, intersectionPoint, surfaceNormal);
    if (closestDistance == std::numeric_limits<float>::infinity())
        return defaultColor;

    Vector3 colorToReturn = Vector3(1, 1, 1);
    Vector3 lightContribution(0, 0, 0);
    for (const auto& light : lights) {
        Vector3 lightDir = (light.position - intersectionPoint).normalize();
        Vector3 fromLightDir = (intersectionPoint - light.position).normalize();

        float closestDistance = std::numeric_limits<float>::infinity();
        Vector3 _dummy1 = Vector3(0, 0, 0);
        Vector3 _dummy2 = Vector3(0, 0, 0);
        WorldIntersection(fromLightDir, light.position, closestDistance, _dummy1, _dummy2);

        if (closestDistance + EPSILON < (light.position - intersectionPoint).length())
            continue;

        float distanceToLightSq = (light.position - intersectionPoint).lengthSquared();
        float inverseSquareFactor = 1.0f / distanceToLightSq;

        float diffuseIntensity = std::max(0.0f, surfaceNormal.dot(lightDir)) * light.intensity;

        float finalIntensity = diffuseIntensity * inverseSquareFactor;

        lightContribution = lightContribution + Vector3(1, 1, 1) * finalIntensity;
    }

    colorToReturn = Vector3(colorToReturn.x * lightContribution.x, colorToReturn.y * lightContribution.y, colorToReturn.z * lightContribution.z);

    return colorToReturn;
}

int colorFromDecimalToWholeRepresentation(float value) {
    value = std::min(1.0f, std::max(0.0f, value));
    return (int)std::round(value * maxColorComponent);
}

std::string colorToPPMFormat(Vector3 color) {
    return std::to_string(colorFromDecimalToWholeRepresentation(color.x)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.y)) + " " + std::to_string(colorFromDecimalToWholeRepresentation(color.z));
}

void loadScene(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Could not open the scene file!" << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string sceneContent = buffer.str();
    ifs.close();

    rapidjson::Document document;
    if (document.Parse(sceneContent.c_str()).HasParseError()) {
        std::cerr << "Error parsing the scene file!" << std::endl;
        return;
    }

    if (document.HasMember("settings") && document["settings"].HasMember("background_color")) {
        const auto& backgroundColor = document["settings"]["background_color"].GetArray();
        defaultColor = Vector3(backgroundColor[0].GetFloat(), backgroundColor[1].GetFloat(), backgroundColor[2].GetFloat());
    }

    if (document.HasMember("settings") && document["settings"].HasMember("image_settings")) {
        const auto& imageSettings = document["settings"]["image_settings"];
        if (imageSettings.HasMember("width") && imageSettings.HasMember("height")) {
            imageWidth = imageSettings["width"].GetInt();
            imageHeight = imageSettings["height"].GetInt();
        }
    }

    if (document.HasMember("camera")) {
        const auto& camera = document["camera"];
        if (camera.HasMember("position")) {
            const auto& position = camera["position"].GetArray();
            cameraPosition = Vector3(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat());
        }
        if (camera.HasMember("matrix")) {
            const auto& matrix = camera["matrix"].GetArray();
            cameraRotation = Matrix3x3(
                matrix[0].GetFloat(), matrix[1].GetFloat(), matrix[2].GetFloat(),
                matrix[3].GetFloat(), matrix[4].GetFloat(), matrix[5].GetFloat(),
                matrix[6].GetFloat(), matrix[7].GetFloat(), matrix[8].GetFloat()
            );
        }
    }

    triangles.clear();
    if (document.HasMember("objects") && document["objects"].IsArray()) {
        const rapidjson::Value& objects = document["objects"];
        for (rapidjson::SizeType i = 0; i < objects.Size(); ++i) {
            const rapidjson::Value& object = objects[i];
            if (object.HasMember("vertices") && object["vertices"].IsArray() &&
                object.HasMember("triangles") && object["triangles"].IsArray()) {

                std::vector<Vector3> vertices;
                const rapidjson::Value& verticesArray = object["vertices"];
                for (rapidjson::SizeType j = 0; j < verticesArray.Size(); j += 3) {
                    vertices.push_back(Vector3(verticesArray[j].GetFloat(), verticesArray[j + 1].GetFloat(), verticesArray[j + 2].GetFloat()));
                }

                const rapidjson::Value& trianglesArray = object["triangles"];
                for (rapidjson::SizeType j = 0; j < trianglesArray.Size(); j += 3) {
                    triangles.push_back(Triangle(vertices[trianglesArray[j].GetInt()], vertices[trianglesArray[j + 1].GetInt()], vertices[trianglesArray[j + 2].GetInt()]));
                }
            }
        }
    }

    lights.clear();
    if (document.HasMember("lights") && document["lights"].IsArray()) {
        const rapidjson::Value& lightsArray = document["lights"];
        for (rapidjson::SizeType i = 0; i < lightsArray.Size(); ++i) {
            const rapidjson::Value& light = lightsArray[i];
            Vector3 position = Vector3(light["position"][0].GetFloat(), light["position"][1].GetFloat(), light["position"][2].GetFloat());
            float intensity = light["intensity"].GetFloat();
            intensity /= 100;
            lights.push_back(Light(position, intensity));
        }
    }
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
        std::cout << "progress: " << imageY / (float)imageHeight * 100.0f << "%\n";
    }
    ppmFileStream.close();
}

int main() {
    //loadScene("./scenes/scene0.crtscene"); renderFrame(0);
    //loadScene("./scenes/scene1.crtscene"); renderFrame(1);
    //loadScene("./scenes/scene2.crtscene"); renderFrame(2);
    loadScene("./scenes/scene3.crtscene"); renderFrame(3);
    
    return 0;
}
