#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "tgaimage.h"

constexpr int width  = 800;
constexpr int height = 800;

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

void
line_3d () {}

typedef struct vec3d {
    float x;
    float y;
    float z;
} Vec3d;

typedef struct face {
    int index1;
    int index2;
    int index3;
} Face;

void
get_vertices_and_faces (std::fstream &FILE,
                        std::vector<Vec3d> &vertices,
                        std::vector<Face> &faces)
{
    std::string line;
    while (std::getline (FILE,line))
    {
        if (line.size() > 2 && line[0] == 'v'
            && std::isspace(line[1]))
        {
            float x, y, z;

            // skip 'v' then three floats
            if (std::sscanf(line.c_str() + 1,
                            "%f %f %f", &x, &y, &z) == 3)
            {
                vertices.push_back(Vec3d{ x, y, z });
            }
        }
        else if (line.size() > 2 && line[0] == 'f'
                 && std::isspace(line[1]))
        {
            int index1, index2, index3;

            if (std::sscanf(line.c_str() + 1,
                            "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &index1, &index2, &index3) == 3)
            {
                faces.push_back(Face{index1,index2,index3});
            }
        }
    }
}

std::tuple<int, int>
project (Vec3d v)
{
    return {
            (v.x + 1.f) * width / 2,
            (v.y + 1.f) * height / 2
           };
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::vector<Vec3d> vertices;
    std::vector<Face> faces;
    std::fstream obj_file;

    obj_file.open("obj/diablo3_pose/diablo3_pose.obj", std::ios::in |
                  std::ios::out);

    if (obj_file.is_open())
    {
        get_vertices_and_faces(obj_file, vertices, faces);
        obj_file.close();
        size_t faces_size = faces.size();
        for (size_t i = 0; i < faces_size; ++i)
            {
                auto [ax, ay] = project(vertices[faces[i].index1 - 1]);
                auto [bx, by] = project(vertices[faces[i].index2 - 1]);
                auto [cx, cy] = project(vertices[faces[i].index3 - 1]);
                line(ax, ay, bx, by,
                     framebuffer,
                     red);
                line(bx, by, cx, cy,
                     framebuffer,
                     red);
            }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

