#include <fstream>
#include <cmath>
#include <iostream>

const int imageWidth = 1920;
const int imageHeight = 1080;
const int maxColorComponent = 255;

void normalize(float& x, float& y, float& z) {
    float length = std::sqrt(x * x + y * y + z * z);
    x /= length;
    y /= length;
    z /= length;
}

int colorFromDecimalToWholeRepresentation(float value) {
    value = std::min(1.0f, std::max(0.0f, value));
    return static_cast<uint8_t>(value * maxColorComponent);
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
            float x = (float)imageX / imageWidth;  // from 0 to 1
            float y = (float)imageY / imageHeight; // from 0 to 1

            x = (2.0f * x) - 1.0f; // from -1 to 1
            y = 1.0f - (2.0f * y); // from -1 to 1

            float aspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
            x *= aspectRatio; // from -r to r

            float rayDirX = x;
            float rayDirY = y;
            float rayDirZ = -1.0f;
            
            normalize(rayDirX, rayDirY, rayDirZ);

            int r = colorFromDecimalToWholeRepresentation(rayDirX);
            int g = colorFromDecimalToWholeRepresentation(rayDirY);
            int b = colorFromDecimalToWholeRepresentation(rayDirZ);
         
            ppmFileStream << r << " " << g << " " << b << " ";
        }
        ppmFileStream << "\n";
    }
    ppmFileStream.close();
    return 0;
}
