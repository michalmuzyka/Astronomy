#pragma once
#include <random>
#include <vector>
#include "IDrawable.hpp"
#include "DataContainers.hpp"
#define M_PI 3.14159265358979323846
#define SEED 1

class Sphere
{
public:
    Sphere()
    {
        generate();
        divideTriangles();
        prepareVerticesData();
    }
    
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> indices;
    std::vector<int> lineIndices;

    std::vector<float> trianglesData;

private:
    static const int sectors = 32;
    static const int stacks = 32;

    void generate()
    {
        float x, y, z, xy;
        float nx, ny, nz;

        float sectorStep = 2 * M_PI / sectors;
        float stackStep = M_PI / stacks;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stacks; ++i)
        {
            stackAngle = M_PI / 2 - i * stackStep;       
            xy = cosf(stackAngle);          
            z = sinf(stackAngle);             

            for (int j = 0; j <= sectors; ++j)
            {
                sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                nx = x;
                ny = y;
                nz = z;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
        }
    }
    void divideTriangles()
    {
        int k1, k2;
        for (int i = 0; i < stacks; ++i)
        {
            k1 = i * (sectors + 1);
            k2 = k1 + sectors + 1;

            for (int j = 0; j < sectors; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (stacks - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }

                lineIndices.push_back(k1);
                lineIndices.push_back(k2);
                if (i != 0) 
                {
                    lineIndices.push_back(k1);
                    lineIndices.push_back(k1 + 1);
                }
            }
        }
    }
    void prepareVerticesData()
    {
        for(int i=0; i<vertices.size(); i+=3)
        {
            trianglesData.push_back(vertices[i]);
            trianglesData.push_back(vertices[i+1]);
            trianglesData.push_back(vertices[i+2]);
            trianglesData.push_back(normals[i]);
            trianglesData.push_back(normals[i + 1]);
            trianglesData.push_back(normals[i + 2]);
        }
    }
};

struct PlanetData
{
    glm::vec3 postion;
    glm::vec3 color;
    float scale_factor;
};

class PlanetsController :public IDrawable
{
public:
    PlanetsController()
    {
        generate_graphics_data();
        generate_planets();
        generate_stars();
    }

    ~PlanetsController()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void Draw(Shader& shader) override
    {
        glBindVertexArray(VAO);


        shader.setBool("star", true);

        for (auto& star : stars)
            DrawPlanet(star, shader);

        shader.setBool("star", false);

        for (auto& planet : planets)
            DrawPlanet(planet, shader);

        glBindVertexArray(0);
    }

    PointLight* getLightPoints()
    {
        return lightSpots;
    }

    static const int planetsCount = 64;
    static const int lightSpotsCount = 2;
    static Sphere sphere_;
private:

    GLuint VBO, EBO, VAO;
    PlanetData planets[planetsCount];
    PlanetData stars[lightSpotsCount];
    PointLight lightSpots[lightSpotsCount];

    void DrawPlanet(PlanetData& planet, Shader& shader)
    {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, planet.scale_factor*planet.postion);
        m = glm::scale(m, glm::vec3(planet.scale_factor, planet.scale_factor, planet.scale_factor));
        shader.setMat4("model", m);
        shader.setVec3("position", planet.postion);
        shader.setVec3("objectColor", planet.color);
        glDrawElements(GL_TRIANGLES, sphere_.indices.size(), GL_UNSIGNED_INT, 0);
    }

    void generate_graphics_data()
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sphere_.trianglesData.size() * sizeof(float), sphere_.trianglesData.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_.indices.size() * sizeof(int), sphere_.indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void generate_planets()
    {
        std::mt19937 gen(SEED);
        std::uniform_real_distribution<> scale(1, 1);
        std::uniform_real_distribution<> position_x(-20, 20);
        std::uniform_real_distribution<> position_z(-10, 100);
        std::uniform_real_distribution<> color(0.05, 0.2);

        for(int i=0; i<planetsCount; ++i)
        {
            planets[i] = PlanetData{
                glm::vec3(position_x(gen), position_x(gen), position_z(gen)),
                glm::vec3(0.58, 0.3,color(gen)),
                static_cast<float>(scale(gen))
            };
        }
    }

    void generate_stars()
    {
        std::mt19937 gen(SEED);
        std::uniform_real_distribution<> scale(1, 1);
        std::uniform_real_distribution<> position_x(-10, 10);
        std::uniform_real_distribution<> position_z(5, 20);
        std::uniform_real_distribution<> color(0.95, 1);

        for (int i = 0; i < lightSpotsCount; ++i)
        {
            glm::vec3 pos = glm::vec3(position_x(gen), position_x(gen), position_z(gen));
            glm::vec3 col = glm::vec3(color(gen), color(gen), color(gen));
            float sc = static_cast<float>(scale(gen));

            stars[i] = PlanetData{
                pos,
                col,
                sc
            },

            lightSpots[i] = PointLight{
                    pos,
                    0.1f * col,
                    0.5f * col,
                    1.f * col,
                    1.0, 0.027f, 0.0028f
            };
        }
    }
};

