#include "Texture.h"
#include <glad/gl.h>
#include <cmath>
#include <cstdlib>
#include <vector>

const int SIZE = 128;

static float random01() {
    return (float)rand() / RAND_MAX;
}

static unsigned int uploadTexture(const std::vector<unsigned char>& pixels) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return tex;
}

static void setPixel(std::vector<unsigned char>& pixels, int x, int y, float r, float g, float b) {
    int i = (y * SIZE + x) * 3;
    pixels[i + 0] = (unsigned char)(fminf(fmaxf(r, 0.0f), 1.0f) * 255);
    pixels[i + 1] = (unsigned char)(fminf(fmaxf(g, 0.0f), 1.0f) * 255);
    pixels[i + 2] = (unsigned char)(fminf(fmaxf(b, 0.0f), 1.0f) * 255);
}

unsigned int makeGrassTexture() {
    std::vector<unsigned char> pixels(SIZE * SIZE * 3);
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            float stripe = (x < SIZE / 2) ? 1.0f : 0.88f;   // mowed lawn stripes
            float noise = 0.8f + 0.3f * random01();
            setPixel(pixels, x, y, 0.20f * stripe * noise, 0.55f * stripe * noise, 0.17f * stripe * noise);
        }
    }
    return uploadTexture(pixels);
}

unsigned int makeWoodTexture() {
    std::vector<unsigned char> pixels(SIZE * SIZE * 3);
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            int plankY = y % (SIZE / 4);                     // 4 planks
            float seam = (plankY < 2) ? 0.5f : 1.0f;
            // sine grain, numbers picked so it repeats without a seam
            float grain = 0.85f + 0.15f * sinf(plankY * 0.589f + sinf(x * 0.0982f) * 0.8f);
            float noise = 0.95f + 0.1f * random01();
            float k = seam * grain * noise;
            setPixel(pixels, x, y, 0.55f * k, 0.36f * k, 0.20f * k);
        }
    }
    return uploadTexture(pixels);
}
