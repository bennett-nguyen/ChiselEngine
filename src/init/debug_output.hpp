#ifndef DEBUG_OUTPUT
#define DEBUG_OUTPUT

#include <iostream>

#include <SDL2/SDL.h>
#include <glad.h>

void requestDebugContext();
void initDebugOutput();
void filterDebugMessage(GLenum source, GLenum type, GLenum severity);
void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam);

#endif