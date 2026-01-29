#include "Mesh.h"
#include <cmath>

#include <glad/glad.h>

Mesh::Mesh()
{
    constexpr float HalfSize = 0.5f;

    constexpr float Red[] = { 1.0f, 0.0f, 0.0f };
    constexpr float Green[] = { 0.0f, 1.0f, 0.0f };
    constexpr float Blue[] = { 0.0f, 0.0f, 1.0f };

    float vertices[] =
    {
        // +X (piros)
        HalfSize, -HalfSize, -HalfSize,  Red[0], Red[1], Red[2],
        HalfSize,  HalfSize, -HalfSize,  Red[0], Red[1], Red[2],
        HalfSize,  HalfSize,  HalfSize,  Red[0], Red[1], Red[2],
        HalfSize,  HalfSize,  HalfSize,  Red[0], Red[1], Red[2],
        HalfSize, -HalfSize,  HalfSize,  Red[0], Red[1], Red[2],
        HalfSize, -HalfSize, -HalfSize,  Red[0], Red[1], Red[2],

        // -X (piros)
       -HalfSize, -HalfSize, -HalfSize,  Red[0], Red[1], Red[2],
       -HalfSize, -HalfSize,  HalfSize,  Red[0], Red[1], Red[2],
       -HalfSize,  HalfSize,  HalfSize,  Red[0], Red[1], Red[2],
       -HalfSize,  HalfSize,  HalfSize,  Red[0], Red[1], Red[2],
       -HalfSize,  HalfSize, -HalfSize,  Red[0], Red[1], Red[2],
       -HalfSize, -HalfSize, -HalfSize,  Red[0], Red[1], Red[2],

       // +Y (zöld)
      -HalfSize,  HalfSize, -HalfSize,  Green[0], Green[1], Green[2],
      -HalfSize,  HalfSize,  HalfSize,  Green[0], Green[1], Green[2],
       HalfSize,  HalfSize,  HalfSize,  Green[0], Green[1], Green[2],
       HalfSize,  HalfSize,  HalfSize,  Green[0], Green[1], Green[2],
       HalfSize,  HalfSize, -HalfSize,  Green[0], Green[1], Green[2],
      -HalfSize,  HalfSize, -HalfSize,  Green[0], Green[1], Green[2],

      // -Y (zöld)
     -HalfSize, -HalfSize, -HalfSize,  Green[0], Green[1], Green[2],
      HalfSize, -HalfSize, -HalfSize,  Green[0], Green[1], Green[2],
      HalfSize, -HalfSize,  HalfSize,  Green[0], Green[1], Green[2],
      HalfSize, -HalfSize,  HalfSize,  Green[0], Green[1], Green[2],
     -HalfSize, -HalfSize,  HalfSize,  Green[0], Green[1], Green[2],
     -HalfSize, -HalfSize, -HalfSize,  Green[0], Green[1], Green[2],

     // +Z (kék)
    -HalfSize, -HalfSize,  HalfSize,  Blue[0], Blue[1], Blue[2],
     HalfSize, -HalfSize,  HalfSize,  Blue[0], Blue[1], Blue[2],
     HalfSize,  HalfSize,  HalfSize,  Blue[0], Blue[1], Blue[2],
     HalfSize,  HalfSize,  HalfSize,  Blue[0], Blue[1], Blue[2],
    -HalfSize,  HalfSize,  HalfSize,  Blue[0], Blue[1], Blue[2],
    -HalfSize, -HalfSize,  HalfSize,  Blue[0], Blue[1], Blue[2],

    // -Z (kék)
   -HalfSize, -HalfSize, -HalfSize,  Blue[0], Blue[1], Blue[2],
   -HalfSize,  HalfSize, -HalfSize,  Blue[0], Blue[1], Blue[2],
    HalfSize,  HalfSize, -HalfSize,  Blue[0], Blue[1], Blue[2],
    HalfSize,  HalfSize, -HalfSize,  Blue[0], Blue[1], Blue[2],
    HalfSize, -HalfSize, -HalfSize,  Blue[0], Blue[1], Blue[2],
   -HalfSize, -HalfSize, -HalfSize,  Blue[0], Blue[1], Blue[2],
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float),
        reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Mesh::Draw() const
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}