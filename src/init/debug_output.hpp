#ifndef DEBUG_OUTPUT
#define DEBUG_OUTPUT

#include <iostream>

#include <SDL3/SDL.h>
#include <glad/gl.h>

void initDebugOutput();
void filterDebugMessage(GLenum source, GLenum type, GLenum severity);
void GLAPIENTRY glDebugOutput(GLenum source,
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam);

#endif