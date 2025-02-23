#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include<cmath>

#include <vector>

#define M_PI 3.1415926

class Sphere {
public:
    Sphere(glm::vec3 center, float radius, glm::vec3 color);
    ~Sphere();
    void get_position_Vertices(std::vector<float> &vertices, int precision);
    void get_index_Vertices(std::vector<unsigned int> &indices, int precision);
    void get_normal_Vertices(std::vector<float> &normals, int precision);

private:
    glm::vec3 center;
    float radius;
    glm::vec3 color;
};

Sphere::Sphere(glm::vec3 center, float radius, glm::vec3 color) {
    this->center = center;
    this->radius = radius;
    this->color = color;
}

void Sphere::get_position_Vertices(std::vector<float> &vertices, int precision) {
    int num_vertices = 180/precision;
    float vx, vy, vz;
    for (int j = 0; j < int(180/precision); j++){
         for (int i = 0; i < int(360/precision); i++) {
            vx = center.x + radius * cos(i * 2*M_PI/num_vertices ) * sin(j *2* M_PI/num_vertices );
            vy = center.y + radius * sin(i * 2*M_PI /num_vertices ) * sin(j *2* M_PI/num_vertices );
            vz = center.z + radius * cos(j * M_PI/num_vertices );

            vertices.push_back(vx);
            vertices.push_back(vy);
            vertices.push_back(vz);
        }
    }
}

void Sphere::get_index_Vertices(std::vector<unsigned int> &indices, int precision) {
    int num_vertices = 180/precision;
    for (int j = 0; j < int(180/precision); j++) {
         for (int i = 0; i < int(360/precision); i++){
            indices.push_back(j*num_vertices + i);
            indices.push_back((j+1)*num_vertices + i);
            indices.push_back((j+1)*num_vertices + (i+1)%num_vertices);
            indices.push_back(j*num_vertices + i);
            indices.push_back(j*num_vertices + (i+1)%num_vertices);
            indices.push_back((j+1)*num_vertices + (i+1)%num_vertices);
         }
    }
}

void Sphere::get_normal_Vertices(std::vector<float> &normals, int precision) {
    int num_vertices = 180/precision;
    float nx, ny, nz;
    for (int j = 0; j < int(180/precision); j++){
         for (int i = 0; i < int(360/precision); i++) {
            nx = cos(i * 2*M_PI/num_vertices ) * sin(j *2* M_PI/num_vertices );
            ny = sin(i * 2*M_PI /num_vertices ) * sin(j *2* M_PI/num_vertices );
            nz = cos(j * M_PI/num_vertices );

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
    }
}


   


#endif // SPHERE_H

