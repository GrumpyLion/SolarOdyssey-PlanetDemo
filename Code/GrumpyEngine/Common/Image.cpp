#include "pch.h"

#include "Image.h"
#include "stb_image.h"

Image::Image(const char* filename)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* image = stbi_load(filename,
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);

    GE_ASSERT(image != nullptr, "STB image couldn't load");

    myPixels.resize(width * height * channels);

    memcpy(myPixels.data(), &image[0], width * height * channels);

    stbi_image_free(image);

    myWidth = width;
    myHeight = height;
    myChannels = channels;
}

Vec3 Image::GetColorAt(Vec2 position)
{
    position.x = Math::Clamp(position.x, 0.0f, (float)myWidth - 1);
    position.y = Math::Clamp(position.y, 0.0f, (float)myHeight - 1);

    uint x = (uint)position.x;
    uint y = (uint)position.y;
    uint index = myChannels * (y * myWidth + x);
    float r = myPixels[index];
    float g = myPixels[index + 1];
    float b = myPixels[index + 2];

    return Vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}
