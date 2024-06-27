#pragma once

#include <array>
#include <string>
#include <SFML/Graphics.hpp>

// Constantes del juego
const unsigned char CELL_SIZE = 16;
const unsigned char FONT_HEIGHT = 16;
const unsigned char GHOST_1_CHASE = 2;
const unsigned char GHOST_2_CHASE = 1;
const unsigned char GHOST_3_CHASE = 4;
const unsigned char GHOST_ANIMATION_FRAMES = 6;
const unsigned char GHOST_ANIMATION_SPEED = 4;
const unsigned char GHOST_ESCAPE_SPEED = 4;
const unsigned char GHOST_FRIGHTENED_SPEED = 3;
const unsigned char GHOST_SPEED = 1;
const unsigned char PACMAN_ANIMATION_FRAMES = 6;
const unsigned char PACMAN_ANIMATION_SPEED = 4;
const unsigned char PACMAN_DEATH_FRAMES = 12;
const unsigned char PACMAN_SPEED = 2;
const unsigned char SCREEN_RESIZE = 2;
const unsigned short CHASE_DURATION = 1024;
const unsigned short ENERGIZER_DURATION = 512;
const unsigned short FRAME_DURATION = 16667;
const unsigned short GHOST_FLASH_START = 64;
const unsigned short LONG_SCATTER_DURATION = 512;
const unsigned short SHORT_SCATTER_DURATION = 256;

const unsigned char MAP_HEIGHT = 21;
const unsigned char MAP_WIDTH = 21;

// Enum para representar diferentes tipos de celdas en el mapa
enum class Cell
{
    Door,
    Empty,
    Power_Pellet,
    Pellet,
    Wall
};

// Estructura para representar posiciones en el mapa
struct Position
{
    short x;
    short y;

    bool operator==(const Position &i_position) const
    {
        return this->x == i_position.x && this->y == i_position.y;
    }
};
// Clase Pacman
class Pacman
{
    bool animation_over;
    bool dead;
    unsigned char direction;
    unsigned short animation_timer;
    unsigned short power_pellet_timer;
    Position position;

    sf::SoundBuffer bufferPowerPellet;
    sf::SoundBuffer bufferEating;
    sf::SoundBuffer bufferDying;
    sf::Sound soundPowerPellet;
    sf::Sound soundEating;
    sf::Sound soundDying;

    int points = 0;

public:
    Pacman();

    bool get_animation_over() const;
    bool get_dead() const;
    unsigned char get_direction();
    unsigned short get_power_pellet_timer() const;

    void draw(bool i_victory, sf::RenderWindow &i_window);
    void reset();
    void set_animation_timer(unsigned short i_animation_timer);
    void set_dead(bool i_dead);
    void set_position(short i_x, short i_y);
    void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map);

    int get_points();
    void set_points(int new_points);
    void reset_points();

    Position get_position() const;

    void loadSounds();

    void playEatingSound()
    {
        soundEating.play();
        soundEating.setLoop(true);
    }
};

// Clase Ghost
class Ghost
{
    bool movement_mode;
    bool use_door;
    unsigned char direction;
    unsigned char frightened_mode;
    unsigned char frightened_speed_timer;
    unsigned char id;
    unsigned short animation_timer;
    Position home;
    Position home_exit;
    Position position;
    Position target;
    sf::SoundBuffer bufferSound;
    sf::SoundBuffer bufferSoundClose;
    sf::Sound soundSound;
    sf::Sound soundSoundClose;

public:
    Ghost(unsigned char i_id);

    bool pacman_collision(const Position &i_pacman_position);
    float get_target_distance(unsigned char i_direction);
    void draw(bool i_flash, sf::RenderWindow &i_window);
    void reset(const Position &i_home, const Position &i_home_exit);
    void set_position(short i_x, short i_y);
    void switch_mode();
    void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Ghost &i_ghost_0, Pacman &i_pacman);
    void update_target(unsigned char i_pacman_direction, const Position &i_ghost_0_position, const Position &i_pacman_position);

    Position get_position() const;
    void loadSounds();
};

// Clase GhostManager
class GhostManager
{
    unsigned char current_wave;
    unsigned short wave_timer;
    std::array<Ghost, 4> ghosts;

public:
    GhostManager();
    void draw(bool i_flash, sf::RenderWindow &i_window);
    void reset(unsigned char i_level, const std::array<Position, 4> &i_ghost_positions);
    void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Pacman &i_pacman);
};

// Declaraci�n de funciones
std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> convert_sketch(const std::array<std::string, MAP_HEIGHT> &i_map_sketch, std::array<Position, 4> &i_ghost_positions, Pacman &i_pacman);
void draw_map(const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, sf::RenderWindow &i_window);
void draw_text(bool i_center, unsigned short i_x, unsigned short i_y, const std::string &i_text, sf::RenderWindow &i_window);
bool map_collision(bool i_collect_pellets, bool i_use_door, short i_x, short i_y, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Pacman &pacman);
