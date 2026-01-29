#pragma once

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void Draw() const;

private:
    unsigned int vao;
    unsigned int vbo;
};