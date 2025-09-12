#include <cmath>
#include <cstdlib>
#include <ctime>
#include "tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void
line (int x1, int y1, int x2, int y2, TGAImage &framebuffer, TGAColor color)
{
    // line goes more vertical than horizontal
    bool steep = std::abs(x2 - x1) < std::abs(y2 - y1);
    if(steep){ // transpose image
        std::swap(x1,y1);
        std::swap(x2,y2);
    }

    // ensure line is drawn left to right
    if(x1 > x2){
        std::swap(x1,x2);
        std::swap(y1,y2);
    }

    int y = y1;
    int ierror = 0;
    for (int x=x1; x<=x2; x++) {
        if (steep) // if transposed, de−transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        // y += (y2-y1) / static_cast<float>(x2-x1);
        ierror += 2 * std::abs(y2-y1);
        y += (y2 > y1 ? 1 : -1) * (ierror > x2 - x1);
        ierror -= 2 * (x2-x1)   * (ierror > x2 - x1);
    }
}

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    line(ax, ay, bx, by, framebuffer, blue);
    line(cx, cy, bx, by, framebuffer, green);
    line(cx, cy, ax, ay, framebuffer, yellow);
    line(ax, ay, cx, cy, framebuffer, red);

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

