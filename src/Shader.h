#pragma once

#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Shader
{
public:
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    void Use() const;
    void SetMat4(const char* name, const glm::mat4& matrix) const;
    void SetVec3(const char* name, const glm::vec3& value) const;
    void SetBool(const char* name, bool value) const;

private:
    unsigned int programId;

private:
    unsigned int CompileShader(unsigned int type, const char* source);
};