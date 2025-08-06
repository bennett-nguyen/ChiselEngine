#ifndef PROC_GEN_HPP
#define PROC_GEN_HPP

#include <glm/vec2.hpp>
#include <glm/gtc/noise.hpp>

unsigned heightMap(unsigned num_iterations, float x, float z, float persistence, float scale, unsigned low, unsigned high);

#endif
