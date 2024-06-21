#include <array>
#include <string>
#include <SFML/Graphics.hpp>
#include "Pacman.h"

// Convierte el boceto del mapa
std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> convert_sketch(const std::array<std::string, MAP_HEIGHT> &i_map_sketch, std::array<Position, 4> &i_ghost_positions, Pacman &i_pacman)
{
	std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> output_map{};

	for (unsigned char a = 0; a < MAP_HEIGHT; a++)
	{
		for (unsigned char b = 0; b < MAP_WIDTH; b++)
		{

			output_map[b][a] = Cell::Empty;

			switch (i_map_sketch[a][b])
			{ // Boceto del mapa de array de strings

			case '#': // Pared
				output_map[b][a] = Cell::Wall;
				break;
			case '=': // Puerta
				output_map[b][a] = Cell::Door;
				break;
			case '.': // Pellet
				output_map[b][a] = Cell::Pellet;
				break;
			case '0':									// Fantasma rojo: Blinky
				i_ghost_positions[0].x = CELL_SIZE * b; // Posici�n inicial en el eje x
				i_ghost_positions[0].y = CELL_SIZE * a; // Posici�n inicial en el eje y
				break;
			case '1': // Fantasma rosa: Pinky
				i_ghost_positions[1].x = CELL_SIZE * b;
				i_ghost_positions[1].y = CELL_SIZE * a;
				break;
			case '2': // Fantasma cian: Inky
				i_ghost_positions[2].x = CELL_SIZE * b;
				i_ghost_positions[2].y = CELL_SIZE * a;
				break;
			case '3': // Fantasma naranja: Clyde
				i_ghost_positions[3].x = CELL_SIZE * b;
				i_ghost_positions[3].y = CELL_SIZE * a;
				break;
			case 'P': // Pacman
				i_pacman.set_position(CELL_SIZE * b, CELL_SIZE * a);
				break;
			case 'o': // Power Pellet
				output_map[b][a] = Cell::Power_Pellet;
				break;
			}
		}
	}

	return output_map;
}

// Dibuja el mapa
void draw_map(const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, sf::RenderWindow &i_window)
{
	sf::RectangleShape wall_shape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
	sf::RectangleShape door_shape(sf::Vector2f(CELL_SIZE, CELL_SIZE / 4));
	sf::CircleShape pellet_shape(CELL_SIZE / 8);
	sf::CircleShape power_pellet_shape(CELL_SIZE / 4);

	for (unsigned char a = 0; a < MAP_WIDTH; a++)
	{
		for (unsigned char b = 0; b < MAP_HEIGHT; b++)
		{
			wall_shape.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));

			switch (i_map[a][b])
			{
			case Cell::Door: // Puerta
				door_shape.setFillColor(sf::Color::White);
				door_shape.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
				i_window.draw(door_shape);
				break;
			case Cell::Power_Pellet: // PowerPellet
				power_pellet_shape.setFillColor(sf::Color::White);
				power_pellet_shape.setPosition(static_cast<float>(CELL_SIZE * a + (CELL_SIZE / 2 - power_pellet_shape.getRadius())), static_cast<float>(CELL_SIZE * b + (CELL_SIZE / 2 - power_pellet_shape.getRadius())));
				i_window.draw(power_pellet_shape);
				break;
			case Cell::Pellet: // Pellet
				pellet_shape.setFillColor(sf::Color::White);
				pellet_shape.setPosition(static_cast<float>(CELL_SIZE * a + (CELL_SIZE / 2 - pellet_shape.getRadius())), static_cast<float>(CELL_SIZE * b + (CELL_SIZE / 2 - pellet_shape.getRadius())));
				i_window.draw(pellet_shape);
				break;
			case Cell::Wall: // Pared
				wall_shape.setFillColor(sf::Color::Blue);
				i_window.draw(wall_shape);
				break;
			}
		}
	}
}

// Verifica colisiones en el mapa
bool map_collision(bool i_collect_pellets, bool i_use_door, short i_x, short i_y, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map)
{
	bool output = 0;

	// Obtener la posici�n
	float cell_x = i_x / static_cast<float>(CELL_SIZE);
	float cell_y = i_y / static_cast<float>(CELL_SIZE);

	// Un fantasma o pacman puede intersectar hasta 4 celdas.
	for (unsigned char a = 0; a < 4; a++)
	{
		short x = 0;
		short y = 0;

		switch (a)
		{
		case 0: // Celda superior izquierda
			x = static_cast<short>(floor(cell_x));
			y = static_cast<short>(floor(cell_y));
			break;
		case 1: // Celda superior derecha
			x = static_cast<short>(ceil(cell_x));
			y = static_cast<short>(floor(cell_y));
			break;
		case 2: // Celda inferior izquierda
			x = static_cast<short>(floor(cell_x));
			y = static_cast<short>(ceil(cell_y));
			break;
		case 3: // Celda inferior derecha
			x = static_cast<short>(ceil(cell_x));
			y = static_cast<short>(ceil(cell_y));
			break;
		}

		// Verificar que las coordenadas estan dentro del limite
		if (0 <= x && 0 <= y && MAP_HEIGHT > y && MAP_WIDTH > x)
		{
			if (0 == i_collect_pellets)
			{ // Verifica si hay una pared o puerta
				if (Cell::Wall == i_map[x][y])
				{
					output = 1;
				}
				else if (0 == i_use_door && Cell::Door == i_map[x][y])
				{
					output = 1;
				}
			}
			else
			{ // Verifica si hay power pelet o un pellet
				if (Cell::Power_Pellet == i_map[x][y])
				{ // Recoge pellet
					output = 1;
					i_map[x][y] = Cell::Empty;
				}
				else if (Cell::Pellet == i_map[x][y])
				{ // Celda vac�a
					i_map[x][y] = Cell::Empty;
				}
			}
		}
	}

	return output;
}

// Pacman
Pacman::Pacman() : animation_over(0), dead(0), direction(0), power_pellet_timer(0), position({0, 0}) {}

bool Pacman::get_animation_over() const
{
	return animation_over;
}

bool Pacman::get_dead() const
{
	return dead;
}

unsigned char Pacman::get_direction()
{
	return direction;
}

unsigned short Pacman::get_power_pellet_timer() const
{
	return power_pellet_timer;
}

// Dibuja Pacman
void Pacman::draw(bool i_victory, sf::RenderWindow &i_window)
{
	unsigned char frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(PACMAN_ANIMATION_SPEED)));
	sf::Sprite sprite;
	sf::Texture texture;
	sprite.setPosition(position.x, position.y);

	sf::CircleShape pacman_shape(CELL_SIZE / 2);
	pacman_shape.setPosition(position.x, position.y);

	if (1 == dead || 1 == i_victory) // Si Pacman est� muerto o ha ganado
	{
		if (animation_timer < PACMAN_DEATH_FRAMES * PACMAN_ANIMATION_SPEED)
		{
			animation_timer++;
			pacman_shape.setFillColor(sf::Color::White);
			i_window.draw(pacman_shape);
		}
		else
		{
			animation_over = 1;
		}
	}
	else // Si Pacman est� vivo y no ha ganado
	{
		animation_timer = (1 + animation_timer) % (PACMAN_ANIMATION_FRAMES * PACMAN_ANIMATION_SPEED);
		pacman_shape.setFillColor(sf::Color::Yellow);
		i_window.draw(pacman_shape);
	}
}

void Pacman::reset() // Reinicia el estado de Pacman
{
	animation_over = 0;
	dead = 0;
	direction = 0;
	animation_timer = 0;
	power_pellet_timer = 0;
}

void Pacman::set_animation_timer(unsigned short i_animation_timer)
{
	animation_timer = i_animation_timer;
}

void Pacman::set_dead(bool i_dead)
{
	dead = i_dead;

	if (1 == dead)
	{
		animation_timer = 0;
	}
}

void Pacman::set_position(short i_x, short i_y)
{
	position = {i_x, i_y};
}

// Actualizar posici�n y estado Pacman
void Pacman::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map)
{
	// Detectar colisiones en las cuatro direcciones
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, PACMAN_SPEED + position.x, position.y, i_map);
	walls[1] = map_collision(0, 0, position.x, position.y - PACMAN_SPEED, i_map);
	walls[2] = map_collision(0, 0, position.x - PACMAN_SPEED, position.y, i_map);
	walls[3] = map_collision(0, 0, position.x, PACMAN_SPEED + position.y, i_map);

	// Cambiar direcci�n de Pacman
	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if (0 == walls[0])
		{
			direction = 0;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (0 == walls[1])
		{
			direction = 1;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (0 == walls[2])
		{
			direction = 2;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (0 == walls[3])
		{
			direction = 3;
		}
	}

	if (0 == walls[direction]) // Mover a Pacman si no hay colisi�n en la direcci�n seleccionada
	{
		switch (direction)
		{
		case 0:
		{
			position.x += PACMAN_SPEED;

			break;
		}
		case 1:
		{
			position.y -= PACMAN_SPEED;

			break;
		}
		case 2:
		{
			position.x -= PACMAN_SPEED;

			break;
		}
		case 3:
		{
			position.y += PACMAN_SPEED;
		}
		}
	}

	// Mover la posico�n de Pacman al otro extremo
	if (-CELL_SIZE >= position.x)
	{
		position.x = CELL_SIZE * MAP_WIDTH - PACMAN_SPEED;
	}
	else if (CELL_SIZE * MAP_WIDTH <= position.x)
	{
		position.x = PACMAN_SPEED - CELL_SIZE;
	}

	// Actualizar el temporizador del power pellet si Pacman toma uno
	if (1 == map_collision(1, 0, position.x, position.y, i_map))
	{
		power_pellet_timer = static_cast<unsigned short>(ENERGIZER_DURATION / pow(2, i_level));
	}
	else
	{
		power_pellet_timer = std::max(0, power_pellet_timer - 1);
	}
}

// Devuelve la posici�n de Pacman
Position Pacman::get_position() const
{
	return position;
}

// Ghost
Ghost::Ghost(unsigned char i_id) : id(i_id)
{
}
// Verifica si el fantasma colisiona con Pacman
bool Ghost::pacman_collision(const Position &i_pacman_position)
{
	if (position.x > i_pacman_position.x - CELL_SIZE && position.x < CELL_SIZE + i_pacman_position.x)
	{
		if (position.y > i_pacman_position.y - CELL_SIZE && position.y < CELL_SIZE + i_pacman_position.y)
		{
			return 1; // colisi�n detectada
		}
	}

	return 0; // no hay colisi�n
}
// Calcula la distancia al objetivo en una direcci�n espec�fica
float Ghost::get_target_distance(unsigned char i_direction)
{
	short x = position.x;
	short y = position.y;

	switch (i_direction)
	{
	case 0:
	{
		x += GHOST_SPEED;

		break;
	}
	case 1:
	{
		y -= GHOST_SPEED;

		break;
	}
	case 2:
	{
		x -= GHOST_SPEED;

		break;
	}
	case 3:
	{
		y += GHOST_SPEED;
	}
	}
	// retorna la distancia al objetivo
	return static_cast<float>(sqrt(pow(x - target.x, 2) + pow(y - target.y, 2)));
}
// dibuja fantasma
void Ghost::draw(bool i_flash, sf::RenderWindow &i_window)
{
	unsigned char body_frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(GHOST_ANIMATION_SPEED)));
	sf::CircleShape ghost_shape(CELL_SIZE / 2);
	ghost_shape.setPosition(position.x, position.y);

	if (0 == frightened_mode) // pacman no come power pellet
	{
		switch (id)
		{
		case 0:
		{
			// color rojo
			ghost_shape.setFillColor(sf::Color(255, 0, 0));
			break;
		}
		case 1:
		{
			// color rosa
			ghost_shape.setFillColor(sf::Color(255, 182, 255));
			break;
		}
		case 2:
		{
			// color cyan
			ghost_shape.setFillColor(sf::Color(0, 255, 255));
			break;
		}
		case 3:
		{
			// color naranja
			ghost_shape.setFillColor(sf::Color(255, 182, 85));
			break;
		}
		}
		i_window.draw(ghost_shape);
	}
	else if (1 == frightened_mode) // pacman come power pellet
	{
		ghost_shape.setFillColor(sf::Color(36, 36, 255)); // Azul
		if (1 == i_flash && 0 == body_frame % 2)
		{
			ghost_shape.setFillColor(sf::Color(255, 255, 255)); // Blanco parpadeando
		}
		else
		{
			ghost_shape.setFillColor(sf::Color(36, 36, 255)); // Azul
		}
		i_window.draw(ghost_shape);
	}

	// Prevenir desbordamiento del contador de animaci�n
	animation_timer = (1 + animation_timer) % (GHOST_ANIMATION_FRAMES * GHOST_ANIMATION_SPEED);
}

void Ghost::reset(const Position &i_home, const Position &i_home_exit) // Reinicia el estado del fantasma
{
	movement_mode = 0;
	use_door = 0 < id;
	direction = 0;
	frightened_mode = 0;
	frightened_speed_timer = 0;
	animation_timer = 0;
	home = i_home;
	home_exit = i_home_exit;
	target = i_home_exit;
}

void Ghost::set_position(short i_x, short i_y)
{
	position = {i_x, i_y};
}

void Ghost::switch_mode()
{
	movement_mode = 1 - movement_mode;
}

void Ghost::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Ghost &i_ghost_0, Pacman &i_pacman)
{
	bool move = 0;
	unsigned char available_ways = 0;
	unsigned char speed = GHOST_SPEED;

	std::array<bool, 4> walls{};

	// Comienza y termina el modo asustado
	if (0 == frightened_mode && i_pacman.get_power_pellet_timer() == ENERGIZER_DURATION / pow(2, i_level))
	{
		frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

		frightened_mode = 1;
	}
	else if (0 == i_pacman.get_power_pellet_timer() && 1 == frightened_mode)
	{
		frightened_mode = 0;
	}

	if (2 == frightened_mode && 0 == position.x % GHOST_ESCAPE_SPEED && 0 == position.y % GHOST_ESCAPE_SPEED)
	{
		speed = GHOST_ESCAPE_SPEED;
	}

	// Actualiza el objetivo del fantasma
	update_target(i_pacman.get_direction(), i_ghost_0.get_position(), i_pacman.get_position());
	// Comprueba las colisiones con las paredes
	walls[0] = map_collision(0, use_door, speed + position.x, position.y, i_map);
	walls[1] = map_collision(0, use_door, position.x, position.y - speed, i_map);
	walls[2] = map_collision(0, use_door, position.x - speed, position.y, i_map);
	walls[3] = map_collision(0, use_door, position.x, speed + position.y, i_map);

	if (1 != frightened_mode)
	{
		unsigned char optimal_direction = 4;
		move = 1;

		// Encuentra la mejor direcci�n para moverse
		for (unsigned char a = 0; a < 4; a++)
		{
			// Gohsts can't turn back! (Unless they really have to)
			if (a == (2 + direction) % 4)
			{
				continue;
			}
			else if (0 == walls[a])
			{
				if (4 == optimal_direction)
				{
					optimal_direction = a;
				}

				available_ways++;

				if (get_target_distance(a) < get_target_distance(optimal_direction))
				{
					// The optimal direction is the direction that's closest to the target.
					optimal_direction = a;
				}
			}
		}
		// Si hay m�s de un camino disponible, elige la direcci�n �ptima
		if (1 < available_ways)
		{
			direction = optimal_direction;
		}
		else
		{
			if (4 == optimal_direction)
			{
				direction = (2 + direction) % 4;
			}
			else
			{
				direction = optimal_direction;
			}
		}
	}
	else
	{
		unsigned char random_direction = rand() % 4;

		if (0 == frightened_speed_timer)
		{
			move = 1;

			frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

			for (unsigned char a = 0; a < 4; a++)
			{
				if (a == (2 + direction) % 4)
				{
					continue;
				}
				else if (0 == walls[a])
				{
					available_ways++;
				}
			}

			if (0 < available_ways)
			{
				while (1 == walls[random_direction] || random_direction == (2 + direction) % 4)
				{
					random_direction = rand() % 4;
				}

				direction = random_direction;
			}
			else
			{
				direction = (2 + direction) % 4;
			}
		}
		else
		{
			frightened_speed_timer--;
		}
	}

	// Mueve el fantasma si puede moverse
	if (1 == move)
	{
		switch (direction)
		{
		case 0:
		{
			position.x += speed; // Mueve a la derecha

			break;
		}
		case 1:
		{
			position.y -= speed; // Mueve hacia arriba

			break;
		}
		case 2:
		{
			position.x -= speed; // Mueve a la izquierda

			break;
		}
		case 3:
		{
			position.y += speed; // Mueve hacia abajo
		}
		}

		// mueve el fantasma al otro lado del mapa si sale de los l�mites
		if (-CELL_SIZE >= position.x)
		{
			position.x = CELL_SIZE * MAP_WIDTH - speed;
		}
		else if (position.x >= CELL_SIZE * MAP_WIDTH)
		{
			position.x = speed - CELL_SIZE;
		}
	}

	// Comprueba la colisi�n con Pacman
	if (1 == pacman_collision(i_pacman.get_position()))
	{
		if (0 == frightened_mode)
		{
			i_pacman.set_dead(1); // Pacman muere si colisiona con un fantasma no asustado
		}
		else
		{
			use_door = 1;

			frightened_mode = 2;

			target = home; // El fantasma asustado vuelve a casa
		}
	}
}
// Actualiza el objetivo del fantasma seg�n su modo y el estado de Pacman
void Ghost::update_target(unsigned char i_pacman_direction, const Position &i_ghost_0_position, const Position &i_pacman_position)
{
	if (1 == use_door)
	{
		if (position == target)
		{
			if (home_exit == target)
			{
				use_door = 0;
			}
			else if (home == target)
			{
				frightened_mode = 0;

				target = home_exit;
			}
		}
	}
	else
	{
		if (0 == movement_mode) // Si el fantasma esta en modo de dispersi�n
		{
			// Cada fantasma se dirige a una esquina asignada en el mapa
			switch (id)
			{
			case 0: // Esquina superior derecha.
			{
				target = {CELL_SIZE * (MAP_WIDTH - 1), 0};

				break;
			}
			case 1: // Esquina superior izquierda.
			{
				target = {0, 0};

				break;
			}
			case 2: // Esquina inferior derecha.
			{
				target = {CELL_SIZE * (MAP_WIDTH - 1), CELL_SIZE * (MAP_HEIGHT - 1)};

				break;
			}
			case 3: // Esquina inferior izquierda.
			{
				target = {0, CELL_SIZE * (MAP_HEIGHT - 1)};
			}
			}
		}
		else // Modo persecuci�n
		{
			switch (id)
			{
			case 0: // fantasma rojo su objetivo es la posici�n actual de pacman
			{
				target = i_pacman_position;

				break;
			}
			case 1: // fantasma rosa su objetivo es la cuarta celda delante de la direcci�n actual de Pacman
			{
				target = i_pacman_position;

				switch (i_pacman_direction)
				{
				case 0:
				{
					target.x += CELL_SIZE * GHOST_1_CHASE;

					break;
				}
				case 1:
				{
					target.y -= CELL_SIZE * GHOST_1_CHASE;

					break;
				}
				case 2:
				{
					target.x -= CELL_SIZE * GHOST_1_CHASE;

					break;
				}
				case 3:
				{
					target.y += CELL_SIZE * GHOST_1_CHASE;
				}
				}

				break;
			}
			case 2: // fantasma azul objetivo es calcular una posici�n usando la posici�n de Pacman
					// y la posici�n del fantasma rojo, creando un vector

				target = i_pacman_position;
				switch (i_pacman_direction)
				{
				case 0:
				{
					target.x += CELL_SIZE * GHOST_2_CHASE;

					break;
				}
				case 1:
				{
					target.y -= CELL_SIZE * GHOST_2_CHASE;

					break;
				}
				case 2:
				{
					target.x -= CELL_SIZE * GHOST_2_CHASE;

					break;
				}
				case 3:
				{
					target.y += CELL_SIZE * GHOST_2_CHASE;
				}
				}

				// Enviar un vector desde el fantasma rojo a la segunda celda frente a Pacman.
				target.x += target.x - i_ghost_0_position.x;
				target.y += target.y - i_ghost_0_position.y;

				break;

			case 3: // fantasma naranja objetivo es la posici�n de Pacman si esta lejos, si esta cerca se usa el teorema de pitagoras para calcular la distancia
				if (CELL_SIZE * GHOST_3_CHASE <= sqrt(pow(position.x - i_pacman_position.x, 2) + pow(position.y - i_pacman_position.y, 2)))
				{
					target = i_pacman_position;
				}
				else
				{
					target = {0, CELL_SIZE * (MAP_HEIGHT - 1)}; // objetivo es la esquina inferior izquierda
				}
			}
		}
	}
}

// Devuelve la posici�n del fantasma
Position Ghost::get_position() const
{
	return position;
}

// Ghost Manager
GhostManager::GhostManager() : current_wave(0),									// Inicializa la oleada actual en 0
							   wave_timer(LONG_SCATTER_DURATION),				// Inicializa el temporizador de oleada con la duraci�n de dispersi�n larga
							   ghosts({Ghost(0), Ghost(1), Ghost(2), Ghost(3)}) // Crea cuatro fantasmas
{
}

// Dibuja todos los fantasmas en la ventana
void GhostManager::draw(bool i_flash, sf::RenderWindow &i_window)
{
	for (Ghost &ghost : ghosts)
	{
		ghost.draw(i_flash, i_window);
	}
}

// Reinicia el estado de los fantasmas
void GhostManager::reset(unsigned char i_level, const std::array<Position, 4> &i_ghost_positions)
{
	current_wave = 0;

	// Aumenta la dificultad dividiendo la duraci�n de dispersi�n larga por 2 elevado al nivel
	wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));

	// Coloca cada fantasma en su posici�n inicial
	for (unsigned char a = 0; a < 4; a++)
	{
		ghosts[a].set_position(i_ghost_positions[a].x, i_ghost_positions[a].y);
	}

	for (Ghost &ghost : ghosts)
	{
		// Usa la posici�n del fantasma azul ghosts[2] como la ubicaci�n de la casa
		// y la del fantasma rojo ghosts[0] como la salida
		ghost.reset(ghosts[2].get_position(), ghosts[0].get_position());
	}
}

// Actualiza el estado de los fantasmas
void GhostManager::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Pacman &i_pacman)
{
	if (0 == i_pacman.get_power_pellet_timer()) // No se actualiza el temporizador de oleada cuando Pacman ha tomado el power pellet
	{
		if (0 == wave_timer)
		{
			if (7 > current_wave)
			{
				current_wave++;

				for (Ghost &ghost : ghosts) // Cambia el modo de movimiento de cada fantasma
				{
					ghost.switch_mode();
				}
			}

			// Ajusta el temporizador de oleada seg�n las reglas
			if (1 == current_wave % 2)
			{
				wave_timer = CHASE_DURATION; // duraci�n de persecuci�n
			}
			else if (2 == current_wave)
			{
				wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level)); // Duraci�n de dispersi�n larga
			}
			else
			{
				wave_timer = static_cast<unsigned short>(SHORT_SCATTER_DURATION / pow(2, i_level)); // Duraci�n de dispersi�n corta
			}
		}
		else
		{
			wave_timer--; // Decrementa el temporizador de oleada
		}
	}

	// Actualiza cada fantasma
	for (Ghost &ghost : ghosts)
	{
		ghost.update(i_level, i_map, ghosts[0], i_pacman);
	}
}

void draw_text(bool i_center, unsigned short i_x, unsigned short i_y, const std::string &i_text, sf::RenderWindow &i_window)
{
	short character_x = i_x;
	short character_y = i_y;

	unsigned char character_width;

	sf::Sprite character_sprite;

	sf::Texture font_texture;
	font_texture.loadFromFile("Recursos/Images/Font.png");

	character_width = font_texture.getSize().x / 96;

	character_sprite.setTexture(font_texture);

	if (1 == i_center)
	{

		character_x = static_cast<short>(round(0.5f * (CELL_SIZE * MAP_WIDTH - character_width * i_text.substr(0, i_text.find_first_of('\n')).size())));
		character_y = static_cast<short>(round(0.5f * (CELL_SIZE * MAP_HEIGHT - FONT_HEIGHT * (1 + std::count(i_text.begin(), i_text.end(), '\n')))));
	}

	for (std::string::const_iterator a = i_text.begin(); a != i_text.end(); a++)
	{
		if ('\n' == *a)
		{
			if (1 == i_center)
			{

				character_x = static_cast<short>(round(0.5f * (CELL_SIZE * MAP_WIDTH - character_width * i_text.substr(1 + a - i_text.begin(), i_text.find_first_of('\n', 1 + a - i_text.begin()) - (1 + a - i_text.begin())).size())));
			}
			else
			{
				character_x = i_x;
			}

			character_y += FONT_HEIGHT;

			continue;
		}

		character_sprite.setPosition(character_x, character_y);

		character_sprite.setTextureRect(sf::IntRect(character_width * (*a - 32), 0, character_width, FONT_HEIGHT));

		character_x += character_width;

		i_window.draw(character_sprite);
	}
}
