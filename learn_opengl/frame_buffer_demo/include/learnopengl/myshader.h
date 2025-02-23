#ifndef MYSHADER_H
#define MYSHADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MyShader {
    public:
        GLuint m_RendererID;
        MyShader(const GLchar* vertexPath, const GLchar* fragmentPath);
        ~MyShader();

        void use() const
        { 
            glUseProgram(m_RendererID); 
        }

        // utility uniform functions
        // ------------------------------------------------------------------------
        void setBool(const std::string &name, bool value) const
        {         
            glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), (int)value); 
        }
        // ------------------------------------------------------------------------
        void setInt(const std::string &name, int value) const
        { 
            glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value); 
        }
        // ------------------------------------------------------------------------
        void setFloat(const std::string &name, float value) const
        { 
            glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value); 
        }
        // ------------------------------------------------------------------------
        void setVec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]); 
        }
        void setVec2(const std::string &name, float x, float y) const
        { 
            glUniform2f(glGetUniformLocation(m_RendererID, name.c_str()), x, y); 
        }
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]); 
        }
        void setVec3(const std::string &name, float x, float y, float z) const
        { 
            glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), x, y, z); 
        }
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]); 
        }
        void setVec4(const std::string &name, float x, float y, float z, float w) const
        { 
            glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), x, y, z, w); 
        }
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    
    private:
        
        std::string m_VertexSource;
        std::string m_FragmentSource;
};

MyShader::MyShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::ifstream inputmodel(vertexPath);
    if (!inputmodel.is_open())
    {
        std::cerr << "Failed to open file: " << vertexPath << std::endl;
        return;
    }
    std::string line;
    while (std::getline(inputmodel, line))
    {
        //    std::cout << line << std::endl;
        this->m_VertexSource += line + "\n";
    }
    inputmodel.close();
    //fragment shader
    inputmodel.open(fragmentPath);
    if (!inputmodel.is_open())
    {
        std::cerr << "Failed to open file: " << fragmentPath << std::endl;
        return;
    }
    while (std::getline(inputmodel, line))
    {
        //    std::cout << line << std::endl;
        this->m_FragmentSource += line + "\n";
    }
    inputmodel.close();

    const char* vShaderCode = m_VertexSource.data();
    const char* fShaderCode = m_FragmentSource.data();

    // std::cout << "Vertex Shader:" << std::endl << m_VertexSource << std::endl;
    // std::cout << "Fragment Shader:" << std::endl << m_FragmentSource << std::endl;
    std::cout<< "Vertex Shader:\n" <<vShaderCode<<std::endl;
    std::cout<< "Fragment Shader:\n" <<fShaderCode<<std::endl;

    //2.compile the shader
    // 编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    // 检查顶点着色器编译是否成功
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    // 检查片段着色器编译是否成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 创建着色器程序
    this->m_RendererID = glCreateProgram();
    glAttachShader(m_RendererID, vertexShader);
    glAttachShader(m_RendererID, fragmentShader);
    glLinkProgram(m_RendererID);

    // 检查着色器程序链接是否成功
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);    
}

MyShader::~MyShader() {
    glDeleteProgram(m_RendererID);
}





#endif // MYSHADER_H

