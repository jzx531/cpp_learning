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
    void draw(Shader & shader,Camera &camera);
    void printModelData();

public:
    GLuint VAO,vtVAO,vnVAO,VBO, positionVBO,normalVBO,texCoordVBO, EBO,texEBO,nEBO;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<GLuint> indices;
    std::vector<GLuint> vtIndices;
    std::vector<GLuint> vnIndices;
    std::vector<float> vertices;

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
                for (int j = 0; j < 3; j++)
                {
                    getline(ssv, vertexIndex, '/');
                    if(vertexIndex.empty())
                    {
                        break;
                    }
                    else {
                        int index = std::stoi(vertexIndex) - 1;
                        if (j == 0)
                        {
                            indices.push_back(index);
                        }
                        else if (j == 1)
                        {
                            vtIndices.push_back(index);
                        }
                        else if (j == 2)
                        {
                            vnIndices.push_back(index);
                        }
                }
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
    //    std::cout << line << std::endl;
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

    // std::cout << indices.size()<< std::endl;
    for (int i = 0; i < indices.size(); i++)
    {
        this->vertices.push_back(positions[indices[i]*3]);
        this->vertices.push_back(positions[indices[i]*3+1]);
        this->vertices.push_back(positions[indices[i]*3+2]);
        this->vertices.push_back(normals[vnIndices[i]*3]);
        this->vertices.push_back(normals[vnIndices[i]*3+1]);
        this->vertices.push_back(normals[vnIndices[i]*3+2]);
        this->vertices.push_back(texCoords[vtIndices[i]*2]);
        this->vertices.push_back(texCoords[vtIndices[i]*2+1]);
    }

    std::cout << "Vertices: " << std::endl;
    // for (int i = 0; i < vertices.size(); i += 8)
    // {
    //     std::cout << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << " " << vertices[i + 3] << " " << vertices[i + 4] << " " << vertices[i + 5] << " " << vertices[i + 6] << " " << vertices[i + 7] << std::endl;
    // }
    
    // glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
   std::cout << "Model imported successfully" << std::endl;


    /*
   glGenVertexArrays(1, &this->vtVAO);
   glGenVertexArrays(1, &this->vnVAO);
   glGenBuffers(1, &this->positionVBO);
   glGenBuffers(1, &this->normalVBO);
   glGenBuffers(1, &this->texCoordVBO);
   glGenBuffers(1, &this->EBO);
   glGenBuffers(1, &this->texEBO);
   glGenBuffers(1, &this->nEBO);
   
// bind the VAO
   glBindVertexArray(this->VAO);
// bind the position VBO
   glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
   glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(0);
   // bind the EBO
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
// bind the normal VBO
   glBindVertexArray(this->vnVAO);
   glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
   glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
   // bind the nEBO
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nEBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, vnIndices.size() * sizeof(GLuint), vnIndices.data(), GL_STATIC_DRAW);
   
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(1);
   
    // bind the texCoord VBO
    glBindVertexArray(this->vtVAO);
   glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
   glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
   // bind the texEBO
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texEBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, vtIndices.size() * sizeof(GLuint), vtIndices.data(), GL_STATIC_DRAW);
   
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(2);
   
// unbind the VAO
   glBindVertexArray(0);*/
   

}



modelimporter::~modelimporter()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteBuffers(1, &texCoordVBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &texEBO);
    glDeleteBuffers(1, &nEBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &vtVAO);
    glDeleteVertexArrays(1, &vnVAO);
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
    std::cout << "Texture Indices: " << std::endl;
    for (int i = 0; i < vtIndices.size(); i += 3)
    {
        std::cout << vtIndices[i] << " " << vtIndices[i + 1] << " " << vtIndices[i + 2] << std::endl;
    }
    std::cout << "Normal Indices: " << std::endl;
    for (int i = 0; i < vnIndices.size(); i += 3)
    {
        std::cout << vnIndices[i] << " " << vnIndices[i + 1] << " " << vnIndices[i + 2] << std::endl;
    }
}

void modelimporter::draw(Shader & shader,Camera &camera)
{  
    // bind the shader
    shader.use();
    // bind the VAO
    glBindVertexArray(this->VAO);

        shader.setMat4("model", glm::mat4(1.0f));
        shader.setMat4("projection", glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f));
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("lightPos", glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setVec3("viewPos", camera.Position);


    // draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // unbind the VAO
    glBindVertexArray(0);
}



#endif
