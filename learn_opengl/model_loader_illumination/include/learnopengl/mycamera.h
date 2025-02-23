#ifndef MYCAMERA_H
#define MYCAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define worldUp glm::vec3(0.0f, 1.0f, 0.0f)
extern const int SCR_WIDTH;
extern const int SCR_HEIGHT;

class MyCamera
{
public:
    MyCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 getViewMatrix();
    void processInput(GLFWwindow* window, float deltaTime);
    void camera_rotate(float angle, glm::vec3 axis);
    void camera_translate(float distance, glm::vec3 direction);
    void camera_raise(float angle, glm::vec3 axis);
    glm::vec3 getPosition() const { return position; }
    glm::mat4 getPerspective(float zoom) const { return glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); }

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    glm::vec3 front;
};  

MyCamera::MyCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
{
    this->position = position;
    this->target = target;
    this->up = up;
    this->front=glm::normalize(this->target - this->position);
}

glm::mat4 MyCamera::getViewMatrix()
{
    return glm::lookAt(position, target, up);
}

void MyCamera::camera_rotate(float angle, glm::vec3 axis)
{
    float radians = glm::radians(angle);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), radians, axis);
    this->position = glm::vec3(rotate * glm::vec4(this->position, 1.0f));
    this->front=glm::normalize(this->target - this->position);
}

void MyCamera::camera_translate(float distance, glm::vec3 direction)
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), direction * distance);
    this->position = glm::vec3(translate * glm::vec4(this->position, 1.0f));
    this->front=glm::normalize(this->target - this->position);  
}

void MyCamera::camera_raise(float angle, glm::vec3 axis)
{
    float radians = glm::radians(angle);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), radians, axis);
    glm::vec3 nfront=this->target - this->position;
    glm::vec3 nfront_new=glm::vec3(rotate * glm::vec4(nfront, 1.0f));
    this->target = this->position + nfront_new;
    this->front=glm::normalize(this->target - this->position);
   
    glm::vec3 right=glm::normalize(glm::cross(this->front, worldUp));
    this-> up=glm::normalize(glm::cross(right, this->front));
}




#endif // MYCAMERA_H
