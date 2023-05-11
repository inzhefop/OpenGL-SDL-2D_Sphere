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

#define PI 3.1415926535897932384626433832795

struct Sphere {
    unsigned int VBO;
    unsigned int VAO;
    int polygons;
    float radius;
    unsigned int shader;
    vec3 light_direction;
    mat4 transformation;
};

struct Shader {
    unsigned int id;
};

char * read_to_string(const char * filename) {
    char * buffer = 0;
    long length;
    FILE *f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell (f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer) {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }
    return buffer;
}

unsigned int shader(const char * vertex_path, const char* fragment_path) {
    struct Shader shader;
    shader.id = glCreateProgram();
    int success;
    char infoLog[512];

    const char * vertex_shader_src = read_to_string(vertex_path);
    const char * fragment_shader_src = read_to_string(fragment_path);

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

float * createBetterCircle (int triangles, float radius) {
    float * vertices = malloc(sizeof(float) * 9.0f * triangles);
    float angle = 0.0f;
    float angle_step = 2*PI / (float) triangles;
    int active_triangle = 0;
    int multiplier = 1;

    while (angle < (2 * PI) && active_triangle < triangles) {
        if (angle >= PI) { multiplier = -1; } else { multiplier = 1; }

        vertices[9*active_triangle + 0] = 0.0f;
        vertices[9*active_triangle + 1] = 0.0f;
        vertices[9*active_triangle + 2] = 0.0f;

        vertices[9*active_triangle + 3] = cos(angle) * radius;
        vertices[9*active_triangle + 4] = sqrt(-pow(cos(angle) * radius, 2.0f) + pow(radius, 2.0f)) * multiplier;
        vertices[9*active_triangle + 5] = 0.0f;

        if (angle + angle_step > PI) { multiplier = -1; } else { multiplier = 1; }

        vertices[9*active_triangle + 6] = cos(angle + angle_step) * radius;
        vertices[9*active_triangle + 7] = sqrt(-pow(cos(angle + angle_step) * radius, 2.0f) + pow(radius, 2.0f)) * multiplier;
        vertices[9*active_triangle + 8] = 0.0f;

        angle += angle_step;
        active_triangle++;
    }

    return vertices;
}

void opengl(struct Sphere * sphere) {
    float * vertices = createBetterCircle(sphere->polygons, sphere->radius);
    glBindVertexArray(sphere->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBO);
    glBufferData(GL_ARRAY_BUFFER, sphere->polygons * 9 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 
    free(vertices);
}

void sphereInit(struct Sphere * sphere) {
    sphere->polygons = 100;
    sphere->radius = 0.75f;

    glGenBuffers(1, &sphere->VBO);
    glGenVertexArrays(1, &sphere->VAO);

    sphere->shader = shader("./shader/sphere_vertex.glsl", "./shader/sphere_fragment.glsl");
    opengl(sphere);

    glUseProgram(sphere->shader);
    int lightSourceLocation = glGetUniformLocation(sphere->shader, "lightSource");
    int radiusLocation = glGetUniformLocation(sphere->shader, "radius");

    glm_vec3_copy((vec3) {-1.0f, -1.0f, -1.0f}, sphere->light_direction);
    glUniform3f(lightSourceLocation, sphere->light_direction[0], sphere->light_direction[1], sphere->light_direction[2]);
    glUniform1f(radiusLocation, sphere->radius);
    glm_mat4_identity(sphere->transformation);
}

int main() {
    struct Sphere sphere;
    SDL_Window * window = createWindow(720, 720, "Sphere");
    renderer_enable_settings();
    bool running = true;
    sphereInit(&sphere);

    char title[128];
    sprintf(title, "2D Sphere - Rendering %d Polygons", sphere.polygons);
    SDL_SetWindowTitle(window, title);
    
    while (running) {
        window_clear();

        glUseProgram(sphere.shader);
        float counter = SDL_GetTicks64() / 2000.0f;
        glm_rotate(sphere.transformation, 0.01f, (vec3) {0.0f, 0.0f, 1.0f});
        glUniformMatrix4fv(glGetUniformLocation(sphere.shader, "transform"), 1, GL_FALSE, sphere.transformation[0]);
        glm_vec3_copy((vec3) {-sin(counter), -cos(counter), -cos(counter)}, sphere.light_direction);
        glUniform3f(glGetUniformLocation(sphere.shader, "lightSource"), sphere.light_direction[0], sphere.light_direction[1], sphere.light_direction[2]);
        glBindVertexArray(sphere.VAO);
        glDrawArrays(GL_TRIANGLES, 0, sphere.polygons * 9);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case (SDLK_ESCAPE):
                        running = false;
                        break;
                    case (SDLK_DELETE):
                        glm_vec3_copy((vec3) {0.0f, 0.0f, -1.0f}, sphere.light_direction);
                        break;
                    case (SDLK_PLUS):
                        sphere.polygons += 1;
                        sprintf(title, "2D Sphere - Rendering %d Polygons", sphere.polygons);
                        SDL_SetWindowTitle(window, title);
                        opengl(&sphere);
                        break;
                    case (SDLK_MINUS):
                        sphere.polygons -= 1;
                        sprintf(title, "2D Sphere - Rendering %d Polygons", sphere.polygons);
                        SDL_SetWindowTitle(window, title);
                        opengl(&sphere);
                        break;
                }
            }
        }
        windowSwapBuffer(window);
    }

    return 0;
}
