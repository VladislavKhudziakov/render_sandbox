

#include "renderer.hpp"

#include <glad/glad.h>


void renderer::renderer::update(float time)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
