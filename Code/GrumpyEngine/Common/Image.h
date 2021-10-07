#pragma once

struct Image
{
    Image(const char* filename);
    Vec3 GetColorAt(Vec2 position);

    int myWidth{};
    int myHeight{};
    uint8 myChannels;
    std::vector<uint8> myPixels;
};