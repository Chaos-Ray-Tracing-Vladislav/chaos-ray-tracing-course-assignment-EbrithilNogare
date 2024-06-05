#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>


void CreateGridImage(const int numRectY, const int numRectX, const int maxColorComponent, const int rectHeight, const int rectWidth, std::vector<std::vector<std::vector<int>>>& image)
{
    for (int y = 0; y < numRectY; ++y) {
        for (int x = 0; x < numRectX; ++x) {
            std::vector<int> color = { rand() % (maxColorComponent + 1) , rand() % (maxColorComponent + 1) , rand() % (maxColorComponent + 1) };
            for (int j = 0; j < rectHeight; ++j) {
                for (int i = 0; i < rectWidth; ++i) {
                    int pixelX = x * rectWidth + i;
                    int pixelY = y * rectHeight + j;
                    switch (rand() % 4) {
                    case 0: image[pixelY][pixelX] = { color[0] , 0, 0 }; break;
                    case 1: image[pixelY][pixelX] = { 0, color[1], 0 }; break;
                    case 2: image[pixelY][pixelX] = { 0, 0, color[2] }; break;
                    case 3: image[pixelY][pixelX] = color; break;
                    }
                }
            }
        }
    }
}

void CreateCircleImage(const int imageHeight, const int imageWidth, std::vector<std::vector<std::vector<int>>>& image)
{
    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            float distance = std::sqrt(std::pow(x - imageWidth / 2, 2) + std::pow(y - imageHeight / 2, 2));
            if (distance < imageHeight / 4 && distance < imageWidth / 4) {
                image[y][x] = { 55, 115, 35 };
            }
            else {
                image[y][x] = { 180, 180, 180 };
            }
        }
    }
}

void SaveFile(const char* fileName, const int imageWidth, const int imageHeight, const int maxColorComponent, std::vector<std::vector<std::vector<int>>>& image)
{
    std::ofstream ppmFileStream(fileName, std::ios::out | std::ios::binary);
    ppmFileStream << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColorComponent << "\n";

    for (int rowIdx = 0; rowIdx < imageHeight; ++rowIdx) {
        for (int colIdx = 0; colIdx < imageWidth; ++colIdx) {
            ppmFileStream << image[rowIdx][colIdx][0] << " " << image[rowIdx][colIdx][1] << " " << image[rowIdx][colIdx][2] << " ";
        }
        ppmFileStream << "\n";
    }

    ppmFileStream.close();

    std::cout << "PPM file '" << fileName << "' generated successfully." << std::endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    // Config
    static const int imageWidth = 1280;
    static const int imageHeight = 720;
    static const int maxColorComponent = 255;
    static const int numRectX = 4;
    static const int numRectY = 4;

    const int rectWidth = imageWidth / numRectX;
    const int rectHeight = imageHeight / numRectY;

    std::vector<std::vector<std::vector<int>>> image(imageHeight, std::vector<std::vector<int>>(imageWidth, std::vector<int>(3, 0)));

    CreateGridImage(numRectY, numRectX, maxColorComponent, rectHeight, rectWidth, image);
    SaveFile("grid.ppm", imageWidth, imageHeight, maxColorComponent, image);

    CreateCircleImage(imageHeight, imageWidth, image);
    SaveFile("circle.ppm", imageWidth, imageHeight, maxColorComponent, image);

    return 0;
}

