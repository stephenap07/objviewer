#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    float vertices[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Red Vertex 1 (X, Y) Top left
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // Green Vertex 2 (X, Y) Top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f,   // Blue Vertex 3 (X, Y) Bottom right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // White Vertex 2 (X, Y) Bottom left
    };

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements),
                 elements, GL_STATIC_DRAW);

    const char * vertexSource =
        "#version 150 core\n "
        "in vec2 position; "
        "in vec3 color; "
        "out vec3 Color; "
        "void main() { "
            "Color = color; "
            "gl_Position = vec4(position, 0.0, 1.0); "
        "}";

    const char * fragmentSource =
        "#version 150 core\n "
        "in vec3 Color; "
        "out vec4 outColor; "
        "void main() { "
        "   outColor = vec4(Color, 1.0); "
        "}"; 

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLint vertexStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);

    if (vertexStatus != GL_TRUE) {
        std::cerr << "Vertex Shader failed to compile!\n";
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        std::cerr << "Error Log for vertex shader:\n\n" << buffer << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    GLint fragmentStatus;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);

    if (fragmentStatus != GL_TRUE) {
        std::cerr << "Fragment Shader failed to compile!\n";
        char buffer[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
        std::cerr << "Error Log for fragment shader:\n\n" << buffer << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));


    SDL_Event windowEvent;
    unsigned long currentTime = SDL_GetTicks();
    unsigned long lastTime = currentTime;
    float deltaTime = 0;
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) break;
        }
        currentTime = SDL_GetTicks();
        deltaTime = ((float)currentTime - (float)lastTime)/1000.0f;
        lastTime = SDL_GetTicks();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
