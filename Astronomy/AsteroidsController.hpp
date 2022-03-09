#pragma once
#include <random>
#include "IDrawable.hpp"
#define SEED 123

class Asteroid :public IDrawable
{
public:
    Asteroid(std::mt19937& generator, glm::vec3 position, glm::vec3 color, float scale_factor)
        :g(generator), position(position), color(color), scale_factor(scale_factor)
    {
        vertices.reserve(216);
        create_triangles();
        prepare_graphics_data();
    }

    void Draw(Shader& shader) override
    {
        glBindVertexArray(VAO);

        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, position);
        m = glm::scale(m, glm::vec3(scale_factor, scale_factor, scale_factor));
        shader.setMat4("model", m);
        shader.setVec3("objectColor", color);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
    }

    glm::vec3 position;
    glm::vec3 color;
    float scale_factor;

private:
    GLuint VBO, VAO;
    
    std::vector<float> vertices;
    std::mt19937& g;
    
    void create_triangles()
    {
        std::uniform_real_distribution<> p(0.7, 2.4);
        
        glm::vec3 v_tmp[8]
        {
            glm::vec3(-p(g),-p(g),-p(g)), //0 
            glm::vec3(p(g), -p(g),-p(g)), //1
            glm::vec3(-p(g),p(g),-p(g)),  //2
            glm::vec3(p(g),p(g),-p(g)),   //3
            glm::vec3(-p(g),-p(g),p(g)),  //4
            glm::vec3(p(g),-p(g),p(g)),   //5
            glm::vec3(-p(g),p(g),p(g)),   //6
            glm::vec3(p(g),p(g),p(g)),    //7

        }; 

        add_triangle(0, 3, 1, v_tmp);
        add_triangle(0, 2, 3, v_tmp);
        add_triangle(0, 4, 6, v_tmp);
        add_triangle(0, 6, 2, v_tmp);
        add_triangle(2, 6, 7, v_tmp);
        add_triangle(2, 7, 3, v_tmp);
        add_triangle(3, 7, 5, v_tmp);
        add_triangle(3, 5, 1, v_tmp);
        add_triangle(6, 4, 5, v_tmp);
        add_triangle(6, 5, 7, v_tmp);
        add_triangle(4, 0, 1, v_tmp);
        add_triangle(4, 1, 5, v_tmp);
    }

    void add_triangle(int v1, int v2, int v3, glm::vec3 tmp[])
    {
        glm::vec3 U = tmp[v2] - tmp[v1];
        glm::vec3 V = tmp[v3] - tmp[v1];

        glm::vec3 normal = glm::cross(U, V);

        add_vertices(tmp[v1]);
        add_normal(normal);

        add_vertices(tmp[v2]);
        add_normal(normal);
        
        add_vertices(tmp[v3]);
        add_normal(normal);
    }

    void add_vertices(glm::vec3 vec)
    {
        vertices.push_back(vec.x);
        vertices.push_back(vec.y);
        vertices.push_back(vec.z);
    }

    void add_normal(glm::vec3 normal)
    {
        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);
    }

    void prepare_graphics_data()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1); 

        glBindVertexArray(0);
    }
};

class AsteroidsController :public IDrawable
{
public:
    AsteroidsController()
    {
        generate();
    }

    void Draw(Shader& shader) override
    {
        for (int i=0; i<AsteroidsCount; ++i)
            asteroids_[i].Draw(shader);
    }
private:
    static const int AsteroidsCount = 256;
    std::mt19937 generator = std::mt19937(SEED);
    std::vector<Asteroid> asteroids_;

    void generate()
    {
        std::uniform_real_distribution<> scale(0.1, 0.5);
        std::uniform_real_distribution<> position_x(-10, 10);
        std::uniform_real_distribution<> position_z(-10, 100);
        std::uniform_real_distribution<> color(0.05, 0.1);
        
        for (int i = 0; i < AsteroidsCount; ++i)
            asteroids_.emplace_back(
                generator,
                glm::vec3(position_x(generator), position_x(generator), position_z(generator)),
                glm::vec3(0.58, 0.3, color(generator)),
                scale(generator)
            ); 
    }

};