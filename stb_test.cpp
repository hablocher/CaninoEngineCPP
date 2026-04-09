#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
int main() {
    int w, h, c;
    unsigned char* data = stbi_load("assets/floor_texture.jpg", &w, &h, &c, 4);
    if (!data) {
        data = stbi_load("demos/fpsdemo/assets/floor_texture.jpg", &w, &h, &c, 4);
    }
    if (!data) {
        std::cout << "FAIL to load image" << std::endl;
        return 1;
    }
    std::cout << "SUCCESS: " << w << "x" << h << " channels: " << c << std::endl;
    std::cout << "Pixels: " << (int)data[0] << ", " << (int)data[1] << ", " << (int)data[2] << ", " << (int)data[3] << std::endl;
    stbi_image_free(data);
    return 0;
}
