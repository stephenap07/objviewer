#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


std::string getShaderSource(const char * filename) {
    std::ifstream shaderStream(filename);
    std::string shaderString;

    shaderString.assign(std::istreambuf_iterator<char>(shaderStream),
                              std::istreambuf_iterator<char>()); 

    shaderStream.close();
    return shaderString;
}

int main(int argc, char *argv[])
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create Vertex Buffer Object
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {
    //  Position   Color             Texcoords
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertString = getShaderSource("vertex.vert");
    const GLchar* vertexCStr = (const GLchar*)vertString.c_str();
    glShaderSource(vertexShader, 1, &vertexCStr, NULL);
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
    std::string fragString = getShaderSource("fragment.frag");
    const GLchar * fragmentCStr = (const GLchar*)fragString.c_str();
    glShaderSource(fragmentShader, 1, &fragmentCStr, NULL);
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
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));

    GLuint textures[2];
    glGenTextures(2, textures);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    SDL_Surface *kittySurface = IMG_Load("kitty.png");
    if (kittySurface == NULL) {
        std::cerr << "Failed to load image: kitty.png\n";
        return -1;
    }
    int mode = GL_RGB;
    if (kittySurface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, mode, kittySurface->w, kittySurface->h, 0, mode, GL_UNSIGNED_BYTE, kittySurface->pixels);
    SDL_FreeSurface(kittySurface);
    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glm::mat4 model;
    glm::mat4 scale;;
    glm::mat4 view;
    glm::mat4 projection;

    model = glm::rotate(model, 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::lookAt(
        glm::vec3(1.2f, 1.2f, 1.2f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    projection = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);

    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniScale = glGetUniformLocation(shaderProgram, "scale");
    GLint uniView= glGetUniformLocation(shaderProgram, "view");
    GLint uniProjection= glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(projection));

    SDL_Surface *puppySurface = IMG_Load("puppy.png");
    if (puppySurface == NULL) {
        std::cerr << "Failed to load image: puppy.png\n";
        return -1;
    }
    if (puppySurface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }

    mode = GL_RGB;
    if (puppySurface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, mode, puppySurface->w, puppySurface->h, 0, mode, GL_UNSIGNED_BYTE, puppySurface->pixels);
    SDL_FreeSurface(puppySurface);
    glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint uniTime = glGetUniformLocation(shaderProgram, "time");

    SDL_Event windowEvent;
    unsigned long currentTime = SDL_GetTicks();
    unsigned long lastTime = currentTime;
    float deltaTime = 0;
    bool rotateX = false;
    glm::vec3 rotationVec = glm::vec3(0.0f, 0.0f, 1.0f);

    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) break;
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE) {
                rotateX = !rotateX; 
            }
        }
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        currentTime = SDL_GetTicks();
        deltaTime = ((float)currentTime - (float)lastTime)/1000.0f;
        lastTime = SDL_GetTicks();

        glUniform1f(uniTime, (GLfloat)currentTime / 1000.0f);

        if (rotateX) {
            rotationVec = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else {
            rotationVec = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        
        model = glm::rotate(
            model,
            deltaTime * 180.0f,
            rotationVec
        );
        float s = sin(5.0f * (float)currentTime / 1000.0f) * 0.25f + 0.75f;
        scale = glm::scale(
            model,
            glm::vec3(s)
        );
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(scale));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
    }

    glDeleteTextures(2, textures);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
