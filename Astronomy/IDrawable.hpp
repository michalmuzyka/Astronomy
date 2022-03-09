#pragma once
#include "Shader.hpp"

class IDrawable
{
public:
    virtual void Draw(Shader& shader) = 0;
    virtual ~IDrawable() = default;
};