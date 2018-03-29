/*
 * Copyright ?2012-2015 Graham Sellers
 *
 * This code is part of the OpenGL Programming Guide, 9th Edition.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "LoadShaders.h"

#include <stdio.h>

float aspect;
GLuint render_prog;
GLuint vao[1];
GLuint vbo[1];
GLuint ebo[1];

GLint render_model_matrix_loc;
GLint render_projection_matrix_loc;

void Initialize()
{
    ShaderInfo shader_info[] =
    {
        { GL_VERTEX_SHADER, "primitive_restart.vs.glsl" },
        { GL_FRAGMENT_SHADER, "primitive_restart.fs.glsl" },
        { GL_NONE, NULL }
    };

    render_prog = LoadShaders(shader_info);

    glUseProgram(render_prog);

    // "model_matrix" is actually an array of 4 matrices
    render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
    render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

    // A single triangle
    static const GLfloat vertex_positions[] =
    {
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
    };

    // Color for each vertex
    static const GLfloat vertex_colors[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f
    };

    // Indices for the triangle strips
    static const GLushort vertex_indices[] =
    {
        0, 1, 2
    };

    // Set up the element array buffer
    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

    // Set up the vertex attributes
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void Display()
{
    static float q = 0.0f;
    static const glm::vec3 X(1.0f, 0.0f, 0.0f);
    static const glm::vec3 Y(0.0f, 1.0f, 0.0f);
    static const glm::vec3 Z(0.0f, 0.0f, 1.0f);
    static const glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    glm::mat4 model_matrix;

    // Setup
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glClearBufferfv(GL_COLOR, 0, &black[0]);

    // Activate simple shading program
    glUseProgram(render_prog);

    // Set up the model and projection matrix
    glm::mat4 projection_matrix = glm::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f);
    glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, &projection_matrix[0][0]);

    // Set up for a glDrawElements call
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

    // Draw Arrays...
    model_matrix = glm::translate(glm::mat4(), glm::vec3(-3.0f, 0.0f, -5.0f));
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, &model_matrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // DrawElements
    model_matrix = glm::translate(glm::mat4(), glm::vec3(-1.0f, 0.0f, -5.0f));
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, &model_matrix[0][0]);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

    // DrawElementsBaseVertex
    model_matrix = glm::translate(glm::mat4(), glm::vec3(1.0f, 0.0f, -5.0f));
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, &model_matrix[0][0]);
    glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

    // DrawArraysInstanced
    model_matrix = glm::translate(glm::mat4(), glm::vec3(3.0f, 0.0f, -5.0f));
    glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, &model_matrix[0][0]);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
}

void Finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(render_prog);
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);
}

int main(int argc, char** argv)
{
    const int width = 800;
    const int height = 600;
    aspect = float(height) / float(width);

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(width, height, "Drawing Commands Example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    Initialize();
    while (!glfwWindowShouldClose(window))
    {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Finalize();

    glfwDestroyWindow(window);
    glfwTerminate();
}