#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <cglm/cglm.h>
#include <cglm/affine.h>

struct Shader {
    unsigned int id;
};

char* shader_read_file(const char* filename) {
    FILE* file;
    file = fopen(filename, "r");
    if (!(file == NULL)) {
        fseek(file, 0L, SEEK_END);
        char *shadercode = malloc(sizeof(char) * ftell(file));
        rewind(file);
        char c; int i = 0;
        char bracket = '}';
        int lastbracket = 0;
        while ((c = fgetc(file)) != EOF) {
            shadercode[i] = c;
            if (c == bracket) {
                lastbracket = i;
                //printf("%d\n", lastbracket);
            }  
            i++;
        }

        for (int b = lastbracket + 1; b < i; b++) {
            shadercode[b] = ' ';
        }
        return shadercode;
    } else {
        printf("failed to read %s - reason: missing or invalid source!\n", filename);
        return "";
    }
}

unsigned int shader(const char * vertex_path, const char* fragment_path) {
    struct Shader shader;
    shader.id = glCreateProgram();
    int success;
    char infoLog[512];

    const char * vertex_shader_src = shader_read_file(vertex_path);
    const char * fragment_shader_src = shader_read_file(fragment_path);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        printf("VERTEX\n%s\n\n", infoLog);
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        printf("FRAGMENT\n%s\n\n", infoLog);
    }

    glAttachShader(shader.id, vertex_shader);
	glAttachShader(shader.id, fragment_shader);

    glLinkProgram(shader.id);

    glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

    free((void*) vertex_shader_src);
	free((void*) fragment_shader_src);

    return shader.id;
}

SDL_Window * createWindow(int width, int hight, char* title) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,16);

    SDL_Window* window;
    
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm)) {
        printf("Error: Getting desktop display mode\n");
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, hight, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    
    
    GLenum err = glewInit(); //GLEW Error handling
    if(err != GLEW_OK) {
        printf("Error: %s\n", glewGetErrorString(err));
    }

    SDL_GL_SetSwapInterval(1);

    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    return window;
}

void windowSwapBuffer(SDL_Window * window) {
    SDL_GL_SwapWindow(window);
}

void window_clear() {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_enable_settings() {
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

float * createCircle(int triangles) {
    int num_vertices = triangles * 3;
    int num_floats = num_vertices * 3;
    float * vertices = malloc(sizeof(float) * num_floats);
    float radius = 0.5f;
    float step_length = (radius*2) / (triangles/2);
    int eswitch = 1;
    float x_position = -radius;

    for (int i = 0; i < triangles; i++) {

        if (x_position + step_length > radius) {
            step_length = radius - x_position;
        }
        
        vertices[i*9 + 0] = 0.0f; //x 
        vertices[i*9 + 1] = 0.0f; //y
        vertices[i*9 + 2] = 0.0f; //z

        vertices[i*9 + 3] = x_position;
        vertices[i*9 + 4] = sqrt(pow(radius, 2) - pow(x_position, 2)) * eswitch;
        vertices[i*9 + 5] = 0.0f;

        vertices[i*9 + 6] = x_position + step_length;
        vertices[i*9 + 7] = sqrt(pow(radius, 2) - pow(x_position + step_length, 2)) * eswitch;
        vertices[i*9 + 8] = 0.0f;

        eswitch *= -1;
        if (eswitch > 0) {
            x_position += step_length;
        }
    }
    
    return vertices;
}

int main() {
    int triangles = 1000;
    SDL_Window * window = createWindow(720, 720, "Sphere");
    GLuint sphere_shader = shader("./shader/sphere_vertex.glsl", "./shader/sphere_fragment.glsl");
    renderer_enable_settings();
    bool running = true;
    float * vertices = createCircle(triangles);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, triangles * 9 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    glUseProgram(sphere_shader);
    int lightSourceLocation = glGetUniformLocation(sphere_shader, "lightSource");
    vec3 lightdirection = {-1.0f, -1.0f, -1.0f};
    glUniform3f(lightSourceLocation, lightdirection[0], lightdirection[1], lightdirection[2]);
    int transformLocation = glGetUniformLocation(sphere_shader, "transform");
    mat4 transformation;
    glm_mat4_identity(transformation);
    
    while (running) {
        window_clear();

        glUseProgram(sphere_shader);
        float counter = SDL_GetTicks64() / 1000.0f;
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, transformation[0]);
        glUniform3f(lightSourceLocation, lightdirection[0], lightdirection[1], lightdirection[2]);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, triangles * 9);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case (SDLK_ESCAPE):
                        running = false;
                        break;
                    case (SDLK_w):
                        lightdirection[1] -= 0.2f;
                        break;
                    case (SDLK_s):
                        lightdirection[1] += 0.2f;
                        break;
                    case (SDLK_a):
                        lightdirection[0] += 0.2f;
                        break;
                    case (SDLK_d):
                        lightdirection[0] -= 0.2f;
                        break;
                    case (SDLK_q):
                        lightdirection[2] -= 0.2f;
                        break;
                    case (SDLK_e):
                        lightdirection[2] += 0.2f;
                        break;
                    case (SDLK_DELETE):
                        glm_vec3_copy((vec3) {0.0f, 0.0f, -1.0f}, lightdirection);
                        break;
                }
            }
        }

        windowSwapBuffer(window);
    }

    free(vertices);
    return 0;
}