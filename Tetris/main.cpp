#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <time.h>
#include <string>
using namespace sf;
using namespace std;

// розмір тетраміно
const int SIZE = 25;
const int SIZE_NEXT = 20;

// поле для гри
const int HEIGHT = 20;
const int WIDTH = 10;
int game_screen[HEIGHT][WIDTH];

// масив тетраміно
const int TYPE = 7;
const int POS = 4;
const int FILL = 4;
int tetramino[TYPE][POS][FILL] =
{
	// I
	{
		1,5,9,13,
		0,1,2,3,
		1,5,9,13,
		0,1,2,3,
	},
	// J
	{
		1,5,9,8,
		0,5,4,6,
		0,4,1,8,
		0,1,2,6,
	},
	// L
	{
		0,4,8,9,
		2,1,0,4,
		0,5,9,1,
		2,5,6,4,
	},
	// O
	{
		0,1,4,5,
		0,1,4,5,
		0,1,4,5,
		0,1,4,5,
	},
	// S
	{
		0,5,4,9,
		2,5,1,4,
		0,5,4,9,
		2,5,1,4,
	},
	// T
	{
		0,4,8,5,
		0,1,2,5,
		1,5,9,4,
		1,5,4,6,
	},
	// Z
	{
		1,4,5,8,
		0,5,1,6,
		1,4,5,8,
		0,5,1,6,
	}
};

int score_max = 99900;

// для визначення координат
struct Coordinates
{
	int x, y;
} base_coord[FILL], check_coord[FILL], next_coord[FILL];

// перевірка на вихід за межі поля для гри
bool checkScreen()
{
	for (int i = 0; i < FILL; i++)
	{
		if (base_coord[i].x < 0 || base_coord[i].x >= WIDTH || base_coord[i].y < 0 || base_coord[i].y >= HEIGHT)
		{
			return 0;
		}
		else if (game_screen[base_coord[i].y][base_coord[i].x])
		{
			return 0;
		}
	}
	return 1;
};

// перевірка на кінець гри
bool Endgame()
{
	// поразка
	for (int i = 0; i < WIDTH; i++)
	{
		if (game_screen[1][i])
		{
			return 0;
		}
	}
}

// функція для стартового меню
void Menu(RenderWindow &tetris)
{
	// текстури, спрайти та звуки меню
	Texture menu, play, exit;
	menu.loadFromFile("images\\menu.png");
	play.loadFromFile("images\\play.png");
	exit.loadFromFile("images\\exit.png");
	Sprite spr_menu(menu), spr_play(play), spr_exit(exit);
	Music menu_music;
	menu_music.openFromFile("sounds\\sound_menu.ogg");

	bool isMenu = 1; // змінна для відображення меню

	// музика в меню
	if (isMenu)
	{
		menu_music.play();
		menu_music.setLoop(true);
	}

	// == МЕНЮ == //
	while (isMenu)
	{
		Event event;
		while (tetris.pollEvent(event))

			tetris.clear(Color::White);

		// підсвічуємо кнопки "Play" та "Exit"
		IntRect(200, 326, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_play.setColor(Color::Green) : spr_play.setColor(Color::White);
		IntRect(200, 428, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_exit.setColor(Color::Red) : spr_exit.setColor(Color::White);

		// клик на кнопки "Play" та "Exit"
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (IntRect(200, 326, 200, 88).contains(Mouse::getPosition(tetris)))
				isMenu = false;
			if (IntRect(200, 428, 200, 88).contains(Mouse::getPosition(tetris)))
			{
				tetris.close();
				isMenu = false;
			}
		}
		tetris.draw(spr_menu);
		spr_play.setPosition(200, 326);
		tetris.draw(spr_play);
		spr_exit.setPosition(200, 428);
		tetris.draw(spr_exit);
		tetris.display();
	}
}

int main()
{
	srand(time(NULL));

	RenderWindow tetris(VideoMode(600, 540), "TETRIS");

	// виклик меню
	Menu(tetris);

	// текстури для гри
	Texture square, square_next, background, play, exit, lose;
	square.loadFromFile("images\\squares.png");
	square_next.loadFromFile("images\\squares_next.png");
	background.loadFromFile("images\\background.png");
	play.loadFromFile("images\\play.png");
	exit.loadFromFile("images\\exit.png");
	lose.loadFromFile("images\\lose.png");

	// спрайти з текстурами
	Sprite spr_square(square), spr_square_next(square_next), spr_background(background), spr_lose(lose);
	Sprite spr_play(play), spr_exit(exit);

	// текст для відображення балів
	Font font;
	font.loadFromFile("fonts\\PressStart2P.ttf");
	Text text("", font, 25);
	text.setFillColor(Color::Blue);

	// звуки для гри
	SoundBuffer mov_buf, rot_buf, hit_buf, lose_buf, one_buf, two_buf, three_buf, four_buf;
	mov_buf.loadFromFile("sounds\\sound_move.ogg");
	rot_buf.loadFromFile("sounds\\sound_rotate.ogg");
	hit_buf.loadFromFile("sounds\\sound_hit.ogg");
	lose_buf.loadFromFile("sounds\\sound_lose.ogg");
	one_buf.loadFromFile("sounds\\sound_line_one.ogg");
	two_buf.loadFromFile("sounds\\sound_line_two.ogg");
	three_buf.loadFromFile("sounds\\sound_line_three.ogg");
	four_buf.loadFromFile("sounds\\sound_line_four.ogg");
	Sound t_move(mov_buf), t_rotate(rot_buf), t_hit(hit_buf), game_lose(lose_buf);
	Sound line_one(one_buf), line_two(two_buf), line_three(three_buf), line_four(four_buf);

	int move = 0; // горизонтальне переміщення
	bool rotate = 0; // обертання
	int type, position, type_next, position_next; // тип тетраміно
	bool beginGame = true; // змінна для початку гри (для першого тетраміно)

	// змінні для знищення лінії
	int check_line, count_squares, count_lines;

	// змінні для підрахунку та відображення кількості балів
	int score = 0;
	String Player_score;
	Player_score = to_string(score);

	// змінні для таймеру та затримки
	float timer = 0, delay = 0.9;
	Clock clock;

	while (tetris.isOpen())
	{
		// отримуємо час, що минув з початку і конвертуємо в секунди
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		// обробляємо події в циклі
		Event event;
		while (tetris.pollEvent(event))
		{
			// користувач натиснув на "хрестик" - закриваємо вікно
			if (event.type == Event::Closed)
				tetris.close();

			// користувач натискає кнопку на клавіатурі
			if (event.type == Event::KeyPressed)
			{
				// кнопка "стрілочка вгору"
				if (event.key.code == Keyboard::Up)
				{
					rotate = true;
				}
				// кнопка <- або ->
				if (event.key.code == Keyboard::Left)
				{
					move = -1;
				}
				else if (event.key.code == Keyboard::Right)
				{
					move = 1;
				}
			}
		}

		// наступний код (генерація, рух, обертання) виконується, якщо гра не закінчена
		if (Endgame())
		{
			// кнопка "стрілочка вниз" - прискорюємо падіння
			if (Keyboard::isKeyPressed(Keyboard::Down))
			{
				delay = 0.03;
			}

			// горизонтальне переміщення
			for (int i = 0; i < FILL; i++)
			{
				check_coord[i] = base_coord[i];
				base_coord[i].x += move;
			}

			// перевірка на вихід за межі при горизонтальному переміщенні
			if (!checkScreen())
			{
				for (int i = 0; i < FILL; i++)
				{
					base_coord[i] = check_coord[i];
				}
			}

			// обертання
			if (rotate)
			{
				Coordinates p = base_coord[1]; // центр обертання
				switch (type)
				{
				case 1:
					for (int i = 0; i < FILL; i++)
					{
						int x = base_coord[i].y - p.y; //y-y0
						int y = base_coord[i].x - p.x; //x-x0
						base_coord[i].x = p.x + x;
						base_coord[i].y = p.y + y;
					}
					break;
				case 2:
				case 3:
				case 5:
				case 6:
				case 7:
					for (int i = 0; i < FILL; i++)
					{
						int x = base_coord[i].y - p.y; //y-y0
						int y = base_coord[i].x - p.x; //x-x0
						base_coord[i].x = p.x - x;
						base_coord[i].y = p.y + y;
					}
					break;
				case 4:
					break;
				}
				// перевірка на вихід за межі при обертанні
				if (!checkScreen())
				{
					for (int i = 0; i < FILL; i++)
					{
						base_coord[i] = check_coord[i];
					}
				}
			}

			// зміщення тетраміно вниз і генерація нового тетраміно
			if (timer > delay)
			{
				for (int i = 0; i < FILL; i++)
				{
					check_coord[i] = base_coord[i];
					base_coord[i].y += 1;
				}
				if (!checkScreen())
				{
					t_hit.play(); // звук при приземленні тетраміно
					for (int i = 0; i < FILL; i++)
					{
						game_screen[check_coord[i].y][check_coord[i].x] = type;
					}

					// координати нового тетраміно
					type = type_next;
					position = position_next;
					for (int i = 0; i < FILL; i++)
					{
						if (type == 1 && position % 2 == 0)
							base_coord[i].x = tetramino[type - 1][position - 1][i] % 4 + 3;
						else
							base_coord[i].x = tetramino[type - 1][position - 1][i] % 4 + 4;
						base_coord[i].y = tetramino[type - 1][position - 1][i] / 4;
					}

					// координати для відображення наступного тетраміно
					type_next = rand() % 7 + 1;
					position_next = rand() % 4 + 1;
					for (int i = 0; i < FILL; i++)
					{
						next_coord[i].x = tetramino[type_next - 1][position_next - 1][i] % 4;
						next_coord[i].y = tetramino[type_next - 1][position_next - 1][i] / 4;
					}
				}
				timer = 0;
			}

			// перевірка і знищення лінії
			check_line = HEIGHT - 1;
			count_lines = 0;
			for (int i = HEIGHT - 1; i > 0; i--)
			{
				count_squares = 0;
				for (int j = 0; j < WIDTH; j++)
				{
					if (game_screen[i][j])
					{
						count_squares++;
					}
					game_screen[check_line][j] = game_screen[i][j];
				}

				if (count_squares < WIDTH)
					check_line--;
				else
					count_lines++; // додаємо знищені лінії для бонусних балів
			}

			// нарахування балів за знищену лінію (з бонусами)
			switch (count_lines)
			{
			case 1:
				if (score < score_max)
					score += 100;
				else
					score += 0;
				line_one.play(); // звук знищення однієї лінії
				Player_score = to_string(score);
				break;
			case 2:
				if (score < score_max)
					score += 300;
				else
					score += 0;
				line_two.play(); // звук знищення двох ліній
				Player_score = to_string(score);
				break;
			case 3:
				if (score < score_max)
					score += 500;
				else
					score += 0;
				line_three.play(); // звук знищення трьох ліній
				Player_score = to_string(score);
				break;
			case 4:
				if (score < score_max)
					score += 1000;
				else
					score += 0;
				line_four.play(); // звук знищення чотирьох ліній
				Player_score = to_string(score);
				break;
			default:
				break;
			}

			// вираховуємо стартові координати першого тетраміно
			if (beginGame)
			{
				beginGame = false;
				type = rand() % 7 + 1;
				position = rand() % 4 + 1;
				for (int i = 0; i < FILL; i++)
				{
					if (type == 1 && position % 2 == 0)
						base_coord[i].x = tetramino[type - 1][position - 1][i] % 4 + 3;
					else
						base_coord[i].x = tetramino[type - 1][position - 1][i] % 4 + 4;
					base_coord[i].y = tetramino[type - 1][position - 1][i] / 4;
				}

				// вираховуємо стартові координати для відображення наступного тетраміно
				type_next = rand() % 7 + 1;
				position_next = rand() % 4 + 1;
				for (int i = 0; i < FILL; i++)
				{
					next_coord[i].x = tetramino[type_next - 1][position_next - 1][i] % 4;
					next_coord[i].y = tetramino[type_next - 1][position_next - 1][i] / 4;
				}
			}

			// звуки
			if (rotate == true)
			{
				t_rotate.play(); // обертання
			}
			if (move == 1 || move == -1)
			{
				t_move.play(); // рух ліворуч/праворуч
			}
			for (int i = 0; i < WIDTH; i++)
			{
				if (game_screen[1][i])
				{
					game_lose.play(); // поразка
				}
			}

			move = 0;
			rotate = 0;
			delay = 0.9;
		}

		// відображення фону
		tetris.clear(Color::White);
		tetris.draw(spr_background);

		// відображення статичних тетраміно (або їх частин-квадратів)
		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				if (game_screen[i][j] == 0)
				{
					continue;
				}
				spr_square.setTextureRect(IntRect(game_screen[i][j] * SIZE, 0, SIZE, SIZE));
				spr_square.setPosition(j * SIZE + 25, i * SIZE + 20);
				tetris.draw(spr_square);
			}
		}

		// відображення нового тетраміно
		for (int i = 0; i < FILL; i++)
		{
			// колір тетраміно
			spr_square.setTextureRect(IntRect(type * SIZE, 0, SIZE, SIZE));
			spr_square.setPosition(base_coord[i].x * SIZE + 25, base_coord[i].y * SIZE + 20);
			tetris.draw(spr_square);
		}

		// відображення наступного тетраміно
		for (int i = 0; i < FILL; i++)
		{
			// колір тетраміно
			spr_square_next.setTextureRect(IntRect(type_next * SIZE_NEXT, 0, SIZE_NEXT, SIZE_NEXT));
			spr_square_next.setPosition(next_coord[i].x * SIZE_NEXT, next_coord[i].y * SIZE_NEXT);

			// корегуємо відображення наступного тетраміно в залежності від типу
			if (type_next == 1)
			{
				position_next % 2 == 0 ? spr_square_next.move(410, 240) : spr_square_next.move(420, 210);
			}
			else if (type_next == 4)
			{
				spr_square_next.move(430, 230);
			}
			else
			{
				position_next % 2 == 0 ? spr_square_next.move(420, 230) : spr_square_next.move(430, 220);
			}
			tetris.draw(spr_square_next);
		}

		// відображення балів
		text.setString(Player_score);
		text.setPosition(389, 380);
		tetris.draw(text);

		// кінець гри
		if (!Endgame())
		{
			spr_lose.setPosition(0, 20);
			tetris.draw(spr_lose);
			spr_play.setPosition(50, 326);
			tetris.draw(spr_play);
			spr_exit.setPosition(50, 428);
			tetris.draw(spr_exit);

			// пропонуємо зіграти нову гру або вийти
			IntRect(50, 326, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_play.setColor(Color::Green) : spr_play.setColor(Color::White);
			IntRect(50, 428, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_exit.setColor(Color::Red) : spr_exit.setColor(Color::White);

			// гравець тисне "Play", обнуляємо лічильник балів і очищаємо поле для гри (або виходимо "Exit")
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				if (IntRect(50, 326, 200, 88).contains(Mouse::getPosition(tetris)))
				{
					score = 0;
					Player_score = to_string(score);
					for (int i = 0; i < HEIGHT; i++)
					{
						for (int j = 0; j < WIDTH; j++)
						{
							game_screen[i][j] = 0;
						}
					}
				}
				if (IntRect(50, 428, 200, 88).contains(Mouse::getPosition(tetris)))
				{
					tetris.close();
				}
			}
		}

		// відображення вікна з грою
		tetris.display();
	}
	return 0;
}