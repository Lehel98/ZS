#include "Shader.h"

#include <glad/glad.h>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);

    int success = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        std::cerr << "Shader link error: " << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(programId);
}

void Shader::Use() const
{
    glUseProgram(programId);
}

void Shader::SetMat4(const char* name, const glm::mat4& matrix) const
{
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}

unsigned int Shader::CompileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error: " << infoLog << "\n";
    }

    return shader;
}

void Shader::SetVec3(const char* name, const glm::vec3& value) const
{
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetBool(const char* name, bool value) const
{
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniform1i(location, value ? 1 : 0);
    }
}