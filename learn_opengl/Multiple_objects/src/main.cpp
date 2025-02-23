#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <myheaders/myshape.h>

// 顶点着色器源码
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 mycolor;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    mycolor = aColor;
    TexCoord = aTexCoord;
}
)";

// 片段着色器源码
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 mycolor;
uniform sampler2D ourTexture;
in vec2 TexCoord;

void main()
{
    FragColor = texture(ourTexture, TexCoord) * vec4(mycolor, 1.0);
}
)";

void printMatrix(const glm::mat4& matrix) {
    for (int i = 0; i < 4; ++i) {                // 遍历行
        for (int j = 0; j < 4; ++j) {            // 遍历列
            std::cout << matrix[i][j] << " ";    // 输出矩阵的元素
        }
        std::cout << std::endl;                  // 换行以打印矩阵的下一行
    }
}

// 调整窗口大小的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//设置窗口大小
int width = 800, height = 600;

// 定义模型矩阵、视图矩阵和投影矩阵(定义为全局变量)
glm::mat4 model = glm::mat4(1.0f); // 单位矩阵
glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// 定义一个正交投影矩阵
float left = -1.0f;
float right = 1.0f;
float bottom = -1.0f;
float top = 1.0f;
float near_plane = 0.1f;
float far_plane = 100.0f;

glm::mat4 projection = glm::ortho(left, right, bottom, top, near_plane, far_plane);


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

    // 为每个面的顶点定义不同颜色
    float vertices[] = {
        // 底面（红色）      // 颜色              // 纹理坐标
        -0.5f, 0.0f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        0.5f, 0.0f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        0.0f, 0.0f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,

        // 侧面1（绿色）
        -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        0.0f, 0.8f,  0.0f,  0.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // 侧面2（蓝色）
        0.5f, 0.0f, -0.5f,  1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        0.0f, 0.0f,  0.5f,  1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        0.0f, 0.8f,  0.0f,  1.0f, 0.0f, 1.0f,   1.0f,0.0f,

        // 侧面3（黄色）
        0.0f, 0.0f,  0.5f,  1.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,  1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.0f, 0.8f,  0.0f,  1.0f, 1.0f, 0.0f,    1.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,  // 底面
        3, 4, 5,  // 侧面1
        6, 7, 8,  // 侧面2
        9, 10, 11 // 侧面3
    };

    // 创建VAO和VBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    
    // 加载纹理
    unsigned int texture;
    get_texture("src/demo.jpg", texture);

    // 编译片段着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 检查着色器编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // 检查着色器编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 创建着色器程序并链接着色器
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器，已经不需要它们
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 获取模型矩阵、视图矩阵和投影矩阵的uniform位置
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    //关闭混合模式，开启深度测试
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入处理 按下ESC键退出
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        //设置按键交换程序
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {

            if (key == GLFW_KEY_R /*&& action == GLFW_PRESS*/)
            {
                std::cout << "R pressed" << std::endl;
                model = glm::mat4(1.0f); // 单位矩阵
                // glm::mat4 view = glm::mat4(1.0f); // 单位矩阵
                view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
            }

            if (key == GLFW_KEY_W /*&& action == GLFW_PRESS*/)
            {
                std::cout << "W pressed" << std::endl;
                view = glm::rotate(view, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            if (key == GLFW_KEY_S /*&& action == GLFW_PRESS*/)
            {
                std::cout << "S pressed" << std::endl;
                view = glm::rotate(view, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            if(key==GLFW_KEY_A /*&& action == GLFW_PRESS*/)
            {
                std::cout << "A pressed" << std::endl;
                model  = glm::rotate(model, glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            }

            if(key==GLFW_KEY_D /*&& action == GLFW_PRESS*/){
                std::cout << "D pressed" << std::endl;
                model  = glm::rotate(model, glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            }

            if(key==GLFW_KEY_Z /*&& action == GLFW_PRESS*/){
                std::cout << "Z pressed" << std::endl;
                // view  = view * glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                view=glm::translate(view, glm::vec3(0.1f, 0.0f, 0.0f));
            }
            if(key==GLFW_KEY_X /*&& action == GLFW_PRESS*/){
                std::cout << "X pressed" << std::endl;
                // view  = view * glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
                view=glm::translate(view, glm::vec3(-0.1f, 0.0f, 0.0f));
            }

            if(key==GLFW_KEY_UP )
            {
                std::cout << "UP pressed" << std::endl;
                model  = glm::translate(model, glm::vec3(0.1f, 0.0f, 0.0f));
            }
            
            if(key==GLFW_KEY_DOWN )
            {
                std::cout << "DOWN pressed" << std::endl;
                model  = glm::translate(model, glm::vec3(-0.1f, 0.0f, 0.0f));
                printMatrix(model);
            }

            });

        // 清除颜色缓冲
        glClear(GL_COLOR_BUFFER_BIT);

        // 清除深度缓冲
        glClear(GL_DEPTH_BUFFER_BIT);

        // 使用着色器程序
        glUseProgram(shaderProgram);
         // 更新uniform颜色
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "mycolor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        //绑定纹理
        glActiveTexture(GL_TEXTURE0); // 激活纹理单元0
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // 设置纹理单元0

        // 传递模型矩阵、视图矩阵和投影矩阵到着色器
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);

        // 绘制第一个可动三角锥
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // 绘制第二个不可动三角锥
        
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 view2 = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection2 = glm::ortho(left, right, bottom, top, near_plane, far_plane);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view2));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection2));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT,0);


        // 交换缓冲区
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
