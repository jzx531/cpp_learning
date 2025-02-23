#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <myheaders/myshader.h>
#include <myheaders/camera.h>
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void processInput(GLFWwindow *window);
int width = 800;
int height = 600;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//声明光源位置
glm::vec3 lightPos(0.0f, 0.0f, -2.0f);
//光源的模型矩阵
glm::mat4 lightmodel = glm::mat4(1.0f);
// 调整窗口大小的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 顶点着色器源码
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// 片段着色器源码
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0); // set all 4 vector values to 1.0
}
)";

const char* lightvertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Normal = aNormal;  
    // Normal=vec3(model * vec4(aNormal, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* lightShaderSource = R"(
#version 330 core
out vec4 FragColor;

struct Light{
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutOff;
};

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform vec3 viewPos;
uniform Light light;

void main()
{

    

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result;
    // spotlight (soft edges) 平滑/软化边缘
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutoff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    result = (ambient + diffuse+ specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 
)";



// 定义模型矩阵、视图矩阵和投影矩阵(定义为全局变量)
glm::mat4 model = glm::mat4(1.0f); // 单位矩阵
// glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// // 定义一个正交投影矩阵
// float left = -1.0f;
// float right = 1.0f;
// float bottom = -1.0f;
// float top = 1.0f;
// float near_plane = 0.1f;
// float far_plane = 100.0f;
// glm::mat4 projection = glm::ortho(left, right, bottom, top, near_plane, far_plane);

// 定义光源的模型矩阵、视图矩阵和投影矩阵(定义为全局变量)
// glm::mat4 lightmodel = glm::mat4(1.0f); // 单位矩阵
// glm::mat4 lightview = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// // 定义一个正交投影矩阵
// glm::mat4 lightprojection = glm::ortho(left, right, bottom, top, near_plane, far_plane);

glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
glm::mat4 view = camera.GetViewMatrix();
glm::mat4 lightprojection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
glm::mat4 lightview = camera.GetViewMatrix();

int main()
{
    // 初始化GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 配置GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif


    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Triangle Pyramid", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 编译物体着色器程序
    unsigned int lightShaderProgram = glCreateProgram();
    myshader(lightShaderProgram, lightvertexShaderSource, lightShaderSource);

    GLuint ligntModelLoc = glGetUniformLocation(lightShaderProgram, "model");
    GLuint lightViewLoc = glGetUniformLocation(lightShaderProgram, "view");
    GLuint lightProjectionLoc = glGetUniformLocation(lightShaderProgram, "projection");

    GLuint viewPosLoc = glGetUniformLocation(lightShaderProgram, "viewPos");
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));
    // glUniform3f(viewPosLoc, lightPos.x, lightPos.y, lightPos.z);
    glUniform1f(glGetUniformLocation(lightShaderProgram, "light.cutoff"), glm::cos(glm::radians(90.0f)));
    glUniform1f(glGetUniformLocation(lightShaderProgram, "light.outerCutOff"), glm::cos(glm::radians(120.0f)));
    glUniform3f(glGetUniformLocation(lightShaderProgram, "light.direction"), -lightPos.x, lightPos.y, lightPos.z);
    glUniform3fv(glGetUniformLocation(lightShaderProgram, "light.position"),
     1,glm::value_ptr(lightPos));

    // 编译光源着色器程序
    unsigned int shaderProgram = glCreateProgram();
    myshader(shaderProgram, vertexShaderSource, fragmentShaderSource);
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // 设置图形
    // 顶点坐标
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    

    model=glm::mat4(1.0f);
    model=glm::translate(model, lightPos);
    model=glm::scale(model, glm::vec3(0.2f));

    glm::vec3 viewPos=camera.Position;

    // //启用背面剔除
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST); // 启用深度测试
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     // render loop
    // -----------
    while(!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        glUseProgram(lightShaderProgram);

        // light properties
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);

        // set light uniforms
        glUniform3f(glGetUniformLocation(lightShaderProgram, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightShaderProgram, "objectColor"), objectColor.x, objectColor.y, objectColor.z);
        glUniform3f(glGetUniformLocation(lightShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

        // set transformation matrices
        glUniformMatrix4fv(ligntModelLoc, 1, GL_FALSE, glm::value_ptr(lightmodel));
        glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(lightview));
        glUniformMatrix4fv(lightProjectionLoc, 1, GL_FALSE, glm::value_ptr(lightprojection));

        glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

        glUniform1f(glGetUniformLocation(lightShaderProgram, "light.cutoff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(lightShaderProgram, "light.outerCutOff"), glm::cos(glm::radians(17.5f)));
        glUniform3f(glGetUniformLocation(lightShaderProgram, "light.direction"), -lightPos.x, -lightPos.y, -lightPos.z);
        glUniform3fv(glGetUniformLocation(lightShaderProgram, "light.position"),
        1,glm::value_ptr(lightPos));


        // draw the light object (using light shader)
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //绘制第二个被照射物
        /*
        glm::mat4 lightmodel2 = glm::mat4(1.0f);
        lightmodel2 = glm::translate(lightmodel2, glm::vec3(0.0f, 0.0f, 2.0f));
        // set transformation matrices
        glUniformMatrix4fv(ligntModelLoc, 1, GL_FALSE, glm::value_ptr(lightmodel2));
        glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(lightview));
        glUniformMatrix4fv(lightProjectionLoc, 1, GL_FALSE, glm::value_ptr(lightprojection));

        // draw the light object (using light shader)
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);*/


        //绘制光源
        // now bind the regular shader program
        glUseProgram(shaderProgram);

        // set object uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // draw the object (using regular shader)
        glBindVertexArray(lightCubeVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized    
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
        // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        view = glm::rotate(view, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightview = glm::rotate(lightview, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        view = glm::rotate(view, glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        lightview = glm::rotate(lightview, glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)   
    {
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f));
        lightview = glm::translate(lightview, glm::vec3(0.0f, 0.0f, -0.1f));
    }
    
    

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        view = glm::rotate(view, glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightview = glm::rotate(lightview, glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        lightmodel = glm::rotate(lightmodel, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        lightmodel = glm::rotate(lightmodel, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        lightmodel = glm::translate(lightmodel, glm::vec3(-0.01f, 0.0f, 0.0f));
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        lightmodel = glm::translate(lightmodel, glm::vec3(0.01f, 0.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        lightmodel=glm::mat4(1.0f);
    }
    
    
}

