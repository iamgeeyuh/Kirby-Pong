#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
    #include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include <ctime>
#include "cmath"

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 1000,
              WINDOW_HEIGHT = 700;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char PLAYER_SPRITESHEET_FILEPATH[] = "assets/hammer_kirby.png",
               BALL_SPRITESHEET_FILEPATH[]   = "assets/ball_kirby.png",
               PLAYER1_WINS_FILEPATH[]       = "assets/player1_wins.png",
               PLAYER2_WINS_FILEPATH[]       = "assets/player2_wins.png",
               TENNIS_COURT_FILEPATH[]       = "assets/tennis_court.jpg";

constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL    = 0,
                TEXTURE_BORDER     = 0;

/* -------------------------- NEW STUFF BELOW -------------------------- */
constexpr int PLAYER_SPRITESHEET_ROWS = 2;
constexpr int PLAYER_SPRITESHEET_COLS = 10;
constexpr int PLAYER_SECONDS_PER_FRAME = 10;
constexpr int MOVE = 0,
              HIT  = 1;

constexpr int BALL_SPRITESHEET_ROWS = 1;
constexpr int BALL_SPRITESHEET_COLS = 16;
constexpr int BALL_SECONDS_PER_FRAME = 16;

int g_hammer_kirby[PLAYER_SPRITESHEET_ROWS][PLAYER_SPRITESHEET_COLS] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
    { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }
};

int g_ball_kirby[BALL_SPRITESHEET_COLS] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

GLuint g_hammer_kirby_texture_id;
GLuint g_ball_kirby_texture_id;
GLuint g_font_texture_id;
GLuint g_player1_wins_texture_id;
GLuint g_player2_wins_texture_id;
GLuint g_tennis_court_texture_id;

int num_balls = 1;

constexpr glm::vec3 INIT_POS_PLAYER1        = glm::vec3(-4.0f, 0.0f, 0.0f),
                    INIT_POS_PLAYER2        = glm::vec3(4.0f, 0.0f, 0.0f),
                    INIT_POS_BALL           = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_POS_MESSAGE        = glm::vec3(0.0f, 1.0f, 0.0f),
                    INIT_POS_TENNIS_COURT   = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_PLAYER2      = glm::vec3(-1.0f, 1.0f, 1.0f),
                    INIT_SCALE_PLAYER1      = glm::vec3(1.0f, 1.0f, 1.0f),
                    INIT_SCALE_BALL         = glm::vec3(1.0f, 1.0f, 1.0f),
                    INIT_SCALE_MESSAGE      = glm::vec3(3.0f, 1.0f, 1.0f),
                    INIT_SCALE_TENNIS_COURT = glm::vec3(10.0f, 8.5f, 1.0f);

// player 1
int *g_player1_animation_indices = g_hammer_kirby[MOVE];
bool g_player1_hit_active = false;
int g_player1_animation_index = 0;
float g_player1_animation_time = 0.0f;
glm::vec3 g_player1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_player1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

// player 2
int *g_player2_animation_indices = g_hammer_kirby[MOVE];
bool g_player2_hit_active = false;
int g_player2_animation_index = 0;
float g_player2_animation_time = 0.0f;
glm::vec3 g_player2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_player2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

float g_player_speed = 1.5f;  // move 1 unit per second
int g_player_animation_frames = PLAYER_SPRITESHEET_COLS;

bool player2_is_active = true;

int winner = 0;

// ball
int g_ball1_animation_index = 0;
float g_ball1_animation_time = 0.0f;
glm::vec3 g_ball1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball1_movement = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 g_ball1_scale    = glm::vec3(1.0f, 1.0f, 1.0f);

int g_ball2_animation_index = 0;
float g_ball2_animation_time = 0.0f;
glm::vec3 g_ball2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball2_scale    = glm::vec3(1.0f, 1.0f, 1.0f);

int g_ball3_animation_index = 0;
float g_ball3_animation_time = 0.0f;
glm::vec3 g_ball3_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball3_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball3_scale    = glm::vec3(1.0f, 1.0f, 1.0f);

float g_ball_speed = 1.5f;  // move 1 unit per second
int g_ball_animation_frames =  BALL_SPRITESHEET_COLS;

void draw_sprite_from_texture_atlas(ShaderProgram *program, GLuint texture_id, int index,
                                    int rows, int cols);
/* -------------------------- NEW STUFF ABOVE -------------------------- */

SDL_Window* g_display_window = nullptr;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_player1_matrix,
          g_player2_matrix,
          g_ball1_matrix,
          g_ball2_matrix,
          g_ball3_matrix,
          g_message_matrix,
          g_tennis_court_matrix,
          g_projection_matrix;

float previous_ticks = 0.0f;

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath);
void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id);


void draw_sprite_from_texture_atlas(ShaderProgram *shaderProgram, GLuint texture_id, int index,
                                    int rows, int cols)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float) (index % cols) / (float) cols;
    float v_coord = (float) (index / cols) / (float) rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float) cols;
    float height = 1.0f / (float) rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width,
        v_coord, u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(shaderProgram->get_position_attribute(), 2, GL_FLOAT, false, 0,
                          vertices);
    glEnableVertexAttribArray(shaderProgram->get_position_attribute());

    glVertexAttribPointer(shaderProgram->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
                          tex_coords);
    glEnableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(shaderProgram->get_position_attribute());
    glDisableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());
}


GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
                                     STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Kirby Pong",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_player1_matrix      = glm::mat4(1.0f);
    g_player2_matrix      = glm::mat4(1.0f);
    g_ball1_matrix        = glm::mat4(1.0f);
    g_ball2_matrix        = glm::mat4(1.0f);
    g_ball3_matrix        = glm::mat4(1.0f);
    g_view_matrix         = glm::mat4(1.0f);
    g_message_matrix      = glm::mat4(1.0f);
    g_tennis_court_matrix = glm::mat4(1.0f);
    g_projection_matrix   = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_hammer_kirby_texture_id = load_texture(PLAYER_SPRITESHEET_FILEPATH);
    g_ball_kirby_texture_id   = load_texture(BALL_SPRITESHEET_FILEPATH);
    g_player1_wins_texture_id = load_texture(PLAYER1_WINS_FILEPATH);
    g_player2_wins_texture_id = load_texture(PLAYER2_WINS_FILEPATH);
    g_tennis_court_texture_id = load_texture(TENNIS_COURT_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    g_player1_movement = glm::vec3(0.0f);
    g_player2_movement = glm::vec3(0.0f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE: g_app_status = TERMINATED; break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q: g_app_status = TERMINATED; break;
                    case SDLK_t: player2_is_active = !player2_is_active; break;
                    case SDLK_1: 
                        num_balls = 1;
                        g_ball2_position = glm::vec3(0.0f);
                        g_ball3_position = glm::vec3(0.0f);
                        g_ball2_movement = glm::vec3(0.0f);
                        g_ball3_movement = glm::vec3(0.0f);
                        break;
                    case SDLK_2:
                        if (num_balls == 1) {
                            g_ball2_movement = glm::vec3(1.0f, 0.0f, 0.0f);
                            g_ball2_position = glm::vec3(0.0f, 0.0f, 0.0f);
                            g_ball2_scale    = glm::vec3(1.0f, 1.0f, 1.0f);
                        }
                        g_ball3_position = glm::vec3(0.0f);
                        g_ball3_movement = glm::vec3(0.0f);
                        num_balls = 2;
                        break;
                    case SDLK_3: 
                        if (num_balls != 3) {
                            g_ball3_movement = glm::vec3(1.0f, 0.0f, 0.0f);
                            g_ball3_position = glm::vec3(0.0f, 0.0f, 0.0f);
                            g_ball3_scale    = glm::vec3(1.0f, 1.0f, 1.0f);
                        }
                        if (num_balls == 1) {
                            g_ball2_movement = glm::vec3(1.0f, 0.0f, 0.0f);
                            g_ball2_position = glm::vec3(0.0f, 0.0f, 0.0f);
                            g_ball2_scale    = glm::vec3(1.0f, 1.0f, 1.0f);
                        }
                        num_balls = 3;
                        break;
                    default: break;
                }

            default: break;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    // player 1
    if (winner == 0 ) {
        if (g_player1_hit_active) {
            g_player1_animation_indices = g_hammer_kirby[HIT];
        } else {
            g_player1_animation_indices = g_hammer_kirby[MOVE];
        }
        
        if (key_state[SDL_SCANCODE_S])
        {
            if (g_player1_position.y > -3.5f) {
                g_player1_movement.y = -1.0f;
            }
        }
        else if (key_state[SDL_SCANCODE_W])
        {
            if (g_player1_position.y < 3.5f) {
                g_player1_movement.y = 1.0f;
            }
        }
        
        if (glm::length(g_player1_movement) > 1.0f)
        {
            g_player1_movement = glm::normalize(g_player1_movement);
        }
        
        // player 2
        
        if (g_player2_hit_active) {
            g_player2_animation_indices = g_hammer_kirby[HIT];
        } else {
            g_player2_animation_indices = g_hammer_kirby[MOVE];
        }
        
        if (player2_is_active) {
            if (key_state[SDL_SCANCODE_DOWN])
            {
                if (g_player2_position.y > -3.5f) {
                    g_player2_movement.y = -1.0f;
                }
            }
            else if (key_state[SDL_SCANCODE_UP])
            {
                if (g_player2_position.y < 3.5f) {
                    g_player2_movement.y = 1.0f;
                }
            }
        } else {
            float ball1_x_distance = fabs(g_ball1_position.x - g_player2_position.x + 0.5f);
            float ball2_x_distance = fabs(g_ball2_position.x - g_player2_position.x + 0.5f);
            float ball3_x_distance = fabs(g_ball3_position.x - g_player2_position.x + 0.5f);

            glm::vec3 closest_ball_position = g_ball1_position;
            if (num_balls >= 2 && ball2_x_distance < ball1_x_distance) {
                closest_ball_position = g_ball2_position;
            }
            if (num_balls == 3 && ball3_x_distance < fmin(ball1_x_distance, ball2_x_distance)) {
                closest_ball_position = g_ball3_position;
            }

            if (closest_ball_position.y > g_player2_position.y && g_player2_position.y < 3.5f) {
                g_player2_movement.y = 1.0f;
            }
            else if (closest_ball_position.y < g_player2_position.y && g_player2_position.y > -3.5f) {
                g_player2_movement.y = -1.0f;
            }
            else {
                g_player2_movement.y = 0.0f;
            }
        }
        
        if (glm::length(g_player2_movement) > 1.0f)
        {
            g_player2_movement = glm::normalize(g_player2_movement);
        }
    }
}


void ball_collision(glm::vec3& position, glm::vec3& movement, glm::vec3& scale) {
    if (winner != 0) {
        movement.x = 0.0f;
        movement.y = 0.0f;
        return;
    }
    
    if ((position.y <= -3.75) || (position.y >= 3.75)){
        movement.y *= -1;
    }
    
    float x1_distance = fabs(position.x + INIT_POS_BALL.x - INIT_POS_PLAYER1.x) - ((INIT_SCALE_PLAYER1.x + INIT_SCALE_BALL.x) / 2.0f);
    float y1_distance = fabs(position.y - g_player1_position.y) - ((INIT_SCALE_PLAYER1.y + INIT_SCALE_BALL.y) / 2.0f);
    float x2_distance = fabs(position.x + INIT_POS_BALL.x - INIT_POS_PLAYER2.x) - ((fabs(INIT_SCALE_PLAYER2.x) + INIT_SCALE_BALL.x) / 2.0f);
    float y2_distance = fabs(position.y - g_player2_position.y) - ((INIT_SCALE_PLAYER2.y + INIT_SCALE_BALL.y) / 2.0f);
    
    if (x1_distance < -0.5f && y1_distance < -0.5f) {
        g_player1_hit_active = true;
        g_player1_animation_index = 0;
        g_player1_animation_indices = g_hammer_kirby[HIT];
        movement.x *= -1;
        scale.x *= -1;
        position.x += movement.x * 0.1f;
        movement.y += g_player1_movement.y * 0.5f;
    } else if (x2_distance < -0.5f && y2_distance < -0.5f) {
        g_player2_hit_active = true;
        g_player2_animation_index = 0;
        g_player2_animation_indices = g_hammer_kirby[HIT];
        movement.x *= -1;
        scale.x *= -1;
        position.x += movement.x * 0.1f;
        movement.y += g_player2_movement.y * 0.5f;
    }
    
    if (position.x < -5.0f) {
        movement.x = 0.0f;
        movement.y = 0.0f;
        winner = 2;
    }
    
    if (position.x > 5.0f) {
        movement.x = 0.0f;
        movement.y = 0.0f;
        winner = 1;
    }
}


void update()
{
    /* DELTA TIME */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - previous_ticks;
    previous_ticks = ticks;

    /* ANIMATION */
    
    // player 1
    if (g_player1_hit_active)
    {
        g_player1_animation_time += delta_time;
        float frames_per_second = 1.0f / PLAYER_SECONDS_PER_FRAME;

        if (g_player1_animation_time >= frames_per_second)
        {
            g_player1_animation_time = 0.0f;
            g_player1_animation_index = (g_player1_animation_index + 1) % g_player_animation_frames;

            if (g_player1_animation_index == 5)
            {
                g_player1_hit_active = false;
                g_player1_animation_index = 0;
                g_player1_animation_indices = g_hammer_kirby[MOVE];
            }
        }
    }
    else if (glm::length(g_player1_movement) != 0)
    {
        g_player1_animation_time += delta_time;
        float frames_per_second = (float) 1 / PLAYER_SECONDS_PER_FRAME;

        if (g_player1_animation_time >= frames_per_second)
        {
            g_player1_animation_time = 0.0f;
            g_player1_animation_index = (g_player1_animation_index + 1) % g_player_animation_frames;
        }
    }
    
    // player 2
    if (g_player2_hit_active)
    {
        g_player2_animation_time += delta_time;
        float frames_per_second = 1.0f / PLAYER_SECONDS_PER_FRAME;

        if (g_player2_animation_time >= frames_per_second)
        {
            g_player2_animation_time = 0.0f;
            g_player2_animation_index = (g_player2_animation_index + 1) % g_player_animation_frames;

            if (g_player2_animation_index == 5)
            {
                g_player2_hit_active = false;
                g_player2_animation_index = 0;
                g_player2_animation_indices = g_hammer_kirby[MOVE];
            }
        }
    }
    else if (glm::length(g_player2_movement) != 0)
    {
        g_player2_animation_time += delta_time;
        float frames_per_second = (float) 1 / PLAYER_SECONDS_PER_FRAME;

        if (g_player2_animation_time >= frames_per_second)
        {
            g_player2_animation_time = 0.0f;
            g_player2_animation_index = (g_player2_animation_index + 1) % g_player_animation_frames;
        }
    }
    
    // ball
    g_ball1_animation_time += delta_time;
    float frames_per_second = (float) 1 / BALL_SECONDS_PER_FRAME;

    if (g_ball1_animation_time >= frames_per_second)
    {
        g_ball1_animation_time = 0.0f;
        g_ball1_animation_index = (g_ball1_animation_index + 1) % g_ball_animation_frames;
    }
    
    g_ball2_animation_time += delta_time;

    if (g_ball2_animation_time >= frames_per_second)
    {
        g_ball2_animation_time = 0.0f;
        g_ball2_animation_index = (g_ball2_animation_index + 1) % g_ball_animation_frames;
    }
    
    g_ball3_animation_time += delta_time;

    if (g_ball3_animation_time >= frames_per_second)
    {
        g_ball3_animation_time = 0.0f;
        g_ball3_animation_index = (g_ball3_animation_index + 1) % g_ball_animation_frames;
    }
    
    /* GAME LOGIC */
    g_player1_position += g_player1_movement * g_player_speed * delta_time;
    g_player2_position += g_player2_movement * g_player_speed * delta_time;
    g_ball1_position   += g_ball1_movement * g_ball_speed * delta_time;
    g_ball2_position   += g_ball2_movement * g_ball_speed * delta_time;
    g_ball3_position   += g_ball3_movement * g_ball_speed * delta_time;

    /* TRANSFORMATIONS */
    g_player1_matrix = glm::mat4(1.0f);
    g_player1_matrix = glm::translate(g_player1_matrix, INIT_POS_PLAYER1);
    g_player1_matrix = glm::translate(g_player1_matrix, g_player1_position);
    
    g_player2_matrix = glm::mat4(1.0f);
    g_player2_matrix = glm::translate(g_player2_matrix, INIT_POS_PLAYER2);
    g_player2_matrix = glm::translate(g_player2_matrix, g_player2_position);
    g_player2_matrix = glm::scale(g_player2_matrix, INIT_SCALE_PLAYER2);
    
    g_ball1_matrix = glm::mat4(1.0f);
    g_ball1_matrix = glm::translate(g_ball1_matrix, INIT_POS_BALL);
    g_ball1_matrix = glm::translate(g_ball1_matrix, g_ball1_position);
    g_ball1_matrix = glm::scale(g_ball1_matrix, INIT_SCALE_BALL);
    g_ball1_matrix = glm::scale(g_ball1_matrix, g_ball1_scale);
    
    g_ball2_matrix = glm::mat4(1.0f);
    g_ball2_matrix = glm::translate(g_ball2_matrix, INIT_POS_BALL);
    g_ball2_matrix = glm::translate(g_ball2_matrix, g_ball2_position);
    g_ball2_matrix = glm::scale(g_ball2_matrix, INIT_SCALE_BALL);
    g_ball2_matrix = glm::scale(g_ball2_matrix, g_ball2_scale);
    
    g_ball3_matrix = glm::mat4(1.0f);
    g_ball3_matrix = glm::translate(g_ball3_matrix, INIT_POS_BALL);
    g_ball3_matrix = glm::translate(g_ball3_matrix, g_ball3_position);
    g_ball3_matrix = glm::scale(g_ball3_matrix, INIT_SCALE_BALL);
    g_ball3_matrix = glm::scale(g_ball3_matrix, g_ball3_scale);
    
    g_message_matrix = glm::mat4(1.0f);
    g_message_matrix = glm::translate(g_message_matrix, INIT_POS_MESSAGE);
    g_message_matrix = glm::scale(g_message_matrix, INIT_SCALE_MESSAGE);
    
    g_tennis_court_matrix = glm::mat4(1.0f);
    g_tennis_court_matrix = glm::translate(g_tennis_court_matrix, INIT_POS_TENNIS_COURT);
    g_tennis_court_matrix = glm::scale(g_tennis_court_matrix, INIT_SCALE_TENNIS_COURT);
    
    /* COLLISIONS */
    ball_collision(g_ball1_position, g_ball1_movement, g_ball1_scale);
    ball_collision(g_ball2_position, g_ball2_movement, g_ball2_scale);
    ball_collision(g_ball3_position, g_ball3_movement, g_ball3_scale);
}


void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
                          0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
                          false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    draw_object(g_tennis_court_matrix, g_tennis_court_texture_id);

    // Bind texture
    if (winner == 1) {
        draw_object(g_message_matrix, g_player1_wins_texture_id);
    }
    if (winner == 2) {
        draw_object(g_message_matrix, g_player2_wins_texture_id);
    }

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    g_shader_program.set_model_matrix(g_player1_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_hammer_kirby_texture_id,
                                   g_player1_animation_indices[g_player1_animation_index],
                                   PLAYER_SPRITESHEET_ROWS, PLAYER_SPRITESHEET_COLS);
    
    g_shader_program.set_model_matrix(g_player2_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_hammer_kirby_texture_id,
                                   g_player2_animation_indices[g_player2_animation_index],
                                   PLAYER_SPRITESHEET_ROWS, PLAYER_SPRITESHEET_COLS);
    
    g_shader_program.set_model_matrix(g_ball1_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_ball_kirby_texture_id,
                                   g_ball1_animation_index,
                                   BALL_SPRITESHEET_ROWS, BALL_SPRITESHEET_COLS);
    
    if (num_balls <= 3 && num_balls > 1 ) {
        g_shader_program.set_model_matrix(g_ball2_matrix);
        draw_sprite_from_texture_atlas(&g_shader_program, g_ball_kirby_texture_id,
                                       g_ball2_animation_index,
                                       BALL_SPRITESHEET_ROWS, BALL_SPRITESHEET_COLS);
    }
    
    if (num_balls == 3) {
        g_shader_program.set_model_matrix(g_ball3_matrix);
        draw_sprite_from_texture_atlas(&g_shader_program, g_ball_kirby_texture_id,
                                       g_ball3_animation_index,
                                       BALL_SPRITESHEET_ROWS, BALL_SPRITESHEET_COLS);
    }

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
