#ifndef MODELIMPORTER_H
#define MODELIMPORTER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include "learnopengl/camera.h"
#include "learnopengl/shader.h"

#include <string>
#include <fstream>
#include <sstream>

class modelimporter
{
public:
     modelimporter(const char* path);
     ~modelimporter();
    void draw(GLuint shader,Camera camera);
    void printModelData();

public:
    GLuint VAO, positionVBO,normalVBO,texCoordVBO, EBO;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<GLuint> indices;

    void processline(std::string line);
};

void modelimporter::processline(std::string line)
{
    std::stringstream ss(line);
    std::string type;
    ss >> type;
    if (type == "v")
    {
        float x, y, z;
        ss >> x >> y >> z;
        this->positions.push_back(x);
        this->positions.push_back(y);
        this->positions.push_back(z);
    }
    else if (type == "vn")
    {
        float x, y, z;
        ss >> x >> y >> z;
        normals.push_back(x);
        normals.push_back(y);
        normals.push_back(z);
    }
    else if (type == "vt")
    {
        float u, v;
        ss >> u >> v;
        texCoords.push_back(u);
        texCoords.push_back(v);
    }
    else if (type == "f")
    {
        std::string vertex[4];
        ss >> vertex[0] >> vertex[1] >> vertex[2] >> vertex[3];
        for (int i = 0; i < 4; i++)
        {
            std::string vertexIndex;
            if(vertex[i].empty())
            {
                break;
            }
            else {
                std::stringstream ssv(vertex[i]);
                while(getline(ssv, vertexIndex, '/'))
                {
                    this->indices.push_back(std::stoi(vertexIndex) - 1);
                }
            }
        }
    }
}


modelimporter::modelimporter(const char* path)
{
   std::ifstream inputmodel(path);
   if (!inputmodel.is_open())
   {
       std::cerr << "Failed to open file: " << path << std::endl;
       return;
   }
   std::string line;
   while (std::getline(inputmodel, line))
   {
       std::cout << line << std::endl;
       processline(line);
   }
   
   inputmodel.close();
   

   // create the VAO, VBOs, and EBO
   glGenVertexArrays(1, &this->VAO);
   GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
    else{
        std::cout << "VAO created successfully" << std::endl;
    }
   glGenBuffers(1, &this->positionVBO);
   glGenBuffers(1, &this->normalVBO);
   glGenBuffers(1, &this->texCoordVBO);
   glGenBuffers(1, &this->EBO);
   
// bind the VAO
   glBindVertexArray(this->VAO);
// bind the position VBO
   glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
   glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(0);
// bind the normal VBO
   glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
   glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(1);
// bind the texCoord VBO
   glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
   glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(2);
// bind the EBO
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
// unbind the VAO
   glBindVertexArray(0);
}



modelimporter::~modelimporter()
{
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteBuffers(1, &texCoordVBO);
    glDeleteBuffers(1, &EBO);
    this->positions.clear();
    this->normals.clear();
    this->texCoords.clear();
    this->indices.clear();
}

void modelimporter::printModelData()
{
    std::cout << "Positions: " << std::endl;
    for (int i = 0; i < positions.size(); i += 3)
    {
        std::cout << positions[i] << " " << positions[i + 1] << " " << positions[i + 2] << std::endl;
    }
    std::cout << "Normals: " << std::endl;
    if (!normals.empty())
    {
        for (int i = 0; i < normals.size(); i += 3)
        {
            std::cout << normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << std::endl;
        }       
    }
    std::cout << "TexCoords: " << std::endl;
    for (int i = 0; i < texCoords.size(); i += 2)
    {
        std::cout << texCoords[i] << " " << texCoords[i + 1] << std::endl;
    }
    std::cout << "Indices: " << std::endl;
    for (int i = 0; i < indices.size(); i += 3)
    {
        std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << std::endl;
    }
}

void modelimporter::draw(GLuint shader,Camera camera)
{  
    // bind the shader
    glUseProgram(shader);
    // bind the VAO
    glBindVertexArray(this->VAO);

    // set the projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // set the view matrix
    glm::mat4 view = camera.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    // set the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
   
    // draw the triangles
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // unbind the VAO
    glBindVertexArray(0);
}



#endif
