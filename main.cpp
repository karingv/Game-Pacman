#include <array>

#include <chrono>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Pacman.h"

int main()
{
	// Variables de estado del juego
	bool game_won = 0;
	unsigned lag = 0;
	unsigned char level = 0;

	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	std::array<std::string, MAP_HEIGHT> map_sketch = {
		" ################### ",
		" #........#........# ",
		" #o##.###.#.###.##o# ",
		" #.................# ",
		" #.##.#.#####.#.##.# ",
		" #....#...#...#....# ",
		" ####.### # ###.#### ",
		"    #.#   0   #.#    ",
		"#####.# ##=## #.#####",
		"     .  #123#  .     ",
		"#####.# ##### #.#####",
		"    #.#       #.#    ",
		" ####.# ##### #.#### ",
		" #........#........# ",
		" #.##.###.#.###.##.# ",
		" #o.#.....P.....#.o# ",
		" ##.#.#.#####.#.#.## ",
		" #....#...#...#....# ",
		" #.######.#.######.# ",
		" #.................# ",
		" ################### "};

	// Mapa del juego inicializado con celdas
	std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map{};

	// Posiciones iniciales de los fantasmas
	std::array<Position, 4> ghost_positions;

	sf::Event event;

	// Ventana del juego
	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE), "Pac-Man", sf::Style::Close);
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));

	GhostManager ghost_manager;
	Pacman pacman;

	// Inicializa el generador de n�meros aleatorios
	srand(static_cast<unsigned>(time(0)));

	// Convierte el dise�o del mapa
	map = convert_sketch(map_sketch, ghost_positions, pacman);

	// Resetea los fantasmas al inicio del juego
	ghost_manager.reset(level, ghost_positions);

	// Obtener la hora actual y almacenarla en una variable.
	previous_time = std::chrono::steady_clock::now();

	// Cargar sonidos
	sf::SoundBuffer startBuffer;
	if (!startBuffer.loadFromFile("Recursos/Audio/startUp.wav"))
	{
		return -1; // Manejo de errores
	}

	sf::Sound startSound(startBuffer);

	// Reproducir el sonido de inicio
	// startSound.play();

	while (1 == window.isOpen())
	{
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();
		lag += delta_time;
		previous_time += std::chrono::microseconds(delta_time);

		// Actualiza el juego si ha pasado suficiente tiempo
		while (FRAME_DURATION <= lag)
		{
			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					window.close();
				}
				}
			}
			// Actualiza el estado del juego si no se ha ganado y Pacman no esta muerto
			if (0 == game_won && 0 == pacman.get_dead())
			{
				game_won = 1;
				pacman.update(level, map);
				ghost_manager.update(level, map, pacman);

				// Verifica si quedan pellets en el mapa
				for (const std::array<Cell, MAP_HEIGHT> &column : map)
				{
					for (const Cell &cell : column)
					{
						if (Cell::Pellet == cell)
						{
							game_won = 0;
							break;
						}
					}

					if (0 == game_won)
					{
						break;
					}
				}
				// Si no hay pellets, el juego se gana
				if (1 == game_won)
				{
					pacman.set_animation_timer(0);
				}
			}
			else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) // Cuando la tecla enter se presiona
			{
				game_won = 0;

				if (1 == pacman.get_dead())
				{
					level = 0; // Cuando muere y presiona enter, se reinicia
				}
				else
				{
					level++; // Cuando ganase pasa al siguiente nivel
				}

				map = convert_sketch(map_sketch, ghost_positions, pacman);

				ghost_manager.reset(level, ghost_positions);

				pacman.reset();
			}

			// Dibuja el estado actual del juego
			if (FRAME_DURATION > lag)
			{
				window.clear();

				if (0 == game_won && 0 == pacman.get_dead())
				{
					draw_map(map, window);

					ghost_manager.draw(GHOST_FLASH_START >= pacman.get_power_pellet_timer(), window);

					draw_text(1, 0, CELL_SIZE * MAP_HEIGHT, "Nivel: " + std::to_string(1 + level), window);
					draw_text(0, CELL_SIZE, CELL_SIZE * MAP_HEIGHT + 2, "Puntos: 0", window);
				}

				pacman.draw(game_won, window);

				if (1 == pacman.get_animation_over())
				{
					if (1 == game_won)
					{
						draw_text(1, 0, 0, "Siguiente nivel", window);
					}
					else
					{
						draw_text(1, 0, 0, "Perdiste :c", window);
					}
				}

				window.display();
			}
		}
	}
}
