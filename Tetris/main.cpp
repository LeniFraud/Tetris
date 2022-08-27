#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <time.h>
#include <string>
using namespace sf;
using namespace std;

// ����� ��������
const int SIZE = 25;
const int SIZE_NEXT = 20;

// ���� ��� ���
const int HEIGHT = 20;
const int WIDTH = 10;
int game_screen[HEIGHT][WIDTH];

// ����� ��������
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

// ��� ���������� ���������
struct Coordinates
{
	int x, y;
} base_coord[FILL], check_coord[FILL], next_coord[FILL];

// �������� �� ����� �� ��� ���� ��� ���
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

// �������� �� ����� ���
bool Endgame()
{
	// �������
	for (int i = 0; i < WIDTH; i++)
	{
		if (game_screen[1][i])
		{
			return 0;
		}
	}
}

// ������� ��� ���������� ����
void Menu(RenderWindow &tetris)
{
	// ��������, ������� �� ����� ����
	Texture menu, play, exit;
	menu.loadFromFile("images\\menu.png");
	play.loadFromFile("images\\play.png");
	exit.loadFromFile("images\\exit.png");
	Sprite spr_menu(menu), spr_play(play), spr_exit(exit);
	Music menu_music;
	menu_music.openFromFile("sounds\\sound_menu.ogg");

	bool isMenu = 1; // ����� ��� ����������� ����

	// ������ � ����
	if (isMenu)
	{
		menu_music.play();
		menu_music.setLoop(true);
	}

	// == ���� == //
	while (isMenu)
	{
		Event event;
		while (tetris.pollEvent(event))

			tetris.clear(Color::White);

		// �������� ������ "Play" �� "Exit"
		IntRect(200, 326, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_play.setColor(Color::Green) : spr_play.setColor(Color::White);
		IntRect(200, 428, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_exit.setColor(Color::Red) : spr_exit.setColor(Color::White);

		// ���� �� ������ "Play" �� "Exit"
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

	// ������ ����
	Menu(tetris);

	// �������� ��� ���
	Texture square, square_next, background, play, exit, lose;
	square.loadFromFile("images\\squares.png");
	square_next.loadFromFile("images\\squares_next.png");
	background.loadFromFile("images\\background.png");
	play.loadFromFile("images\\play.png");
	exit.loadFromFile("images\\exit.png");
	lose.loadFromFile("images\\lose.png");

	// ������� � ����������
	Sprite spr_square(square), spr_square_next(square_next), spr_background(background), spr_lose(lose);
	Sprite spr_play(play), spr_exit(exit);

	// ����� ��� ����������� ����
	Font font;
	font.loadFromFile("fonts\\PressStart2P.ttf");
	Text text("", font, 25);
	text.setFillColor(Color::Blue);

	// ����� ��� ���
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

	int move = 0; // ������������� ����������
	bool rotate = 0; // ���������
	int type, position, type_next, position_next; // ��� ��������
	bool beginGame = true; // ����� ��� ������� ��� (��� ������� ��������)

	// ���� ��� �������� ��
	int check_line, count_squares, count_lines;

	// ���� ��� ��������� �� ����������� ������� ����
	int score = 0;
	String Player_score;
	Player_score = to_string(score);

	// ���� ��� ������� �� ��������
	float timer = 0, delay = 0.9;
	Clock clock;

	while (tetris.isOpen())
	{
		// �������� ���, �� ����� � ������� � ���������� � �������
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		// ���������� ��䳿 � ����
		Event event;
		while (tetris.pollEvent(event))
		{
			// ���������� �������� �� "�������" - ��������� ����
			if (event.type == Event::Closed)
				tetris.close();

			// ���������� ������� ������ �� ��������
			if (event.type == Event::KeyPressed)
			{
				// ������ "�������� �����"
				if (event.key.code == Keyboard::Up)
				{
					rotate = true;
				}
				// ������ <- ��� ->
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

		// ��������� ��� (���������, ���, ���������) ����������, ���� ��� �� ��������
		if (Endgame())
		{
			// ������ "�������� ����" - ����������� ������
			if (Keyboard::isKeyPressed(Keyboard::Down))
			{
				delay = 0.03;
			}

			// ������������� ����������
			for (int i = 0; i < FILL; i++)
			{
				check_coord[i] = base_coord[i];
				base_coord[i].x += move;
			}

			// �������� �� ����� �� ��� ��� ��������������� ���������
			if (!checkScreen())
			{
				for (int i = 0; i < FILL; i++)
				{
					base_coord[i] = check_coord[i];
				}
			}

			// ���������
			if (rotate)
			{
				Coordinates p = base_coord[1]; // ����� ���������
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
				// �������� �� ����� �� ��� ��� ��������
				if (!checkScreen())
				{
					for (int i = 0; i < FILL; i++)
					{
						base_coord[i] = check_coord[i];
					}
				}
			}

			// ������� �������� ���� � ��������� ������ ��������
			if (timer > delay)
			{
				for (int i = 0; i < FILL; i++)
				{
					check_coord[i] = base_coord[i];
					base_coord[i].y += 1;
				}
				if (!checkScreen())
				{
					t_hit.play(); // ���� ��� ���������� ��������
					for (int i = 0; i < FILL; i++)
					{
						game_screen[check_coord[i].y][check_coord[i].x] = type;
					}

					// ���������� ������ ��������
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

					// ���������� ��� ����������� ���������� ��������
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

			// �������� � �������� ��
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
					count_lines++; // ������ ������ �� ��� �������� ����
			}

			// ����������� ���� �� ������� ��� (� ��������)
			switch (count_lines)
			{
			case 1:
				if (score < score_max)
					score += 100;
				else
					score += 0;
				line_one.play(); // ���� �������� ���� ��
				Player_score = to_string(score);
				break;
			case 2:
				if (score < score_max)
					score += 300;
				else
					score += 0;
				line_two.play(); // ���� �������� ���� ���
				Player_score = to_string(score);
				break;
			case 3:
				if (score < score_max)
					score += 500;
				else
					score += 0;
				line_three.play(); // ���� �������� ����� ���
				Player_score = to_string(score);
				break;
			case 4:
				if (score < score_max)
					score += 1000;
				else
					score += 0;
				line_four.play(); // ���� �������� �������� ���
				Player_score = to_string(score);
				break;
			default:
				break;
			}

			// ���������� ������� ���������� ������� ��������
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

				// ���������� ������� ���������� ��� ����������� ���������� ��������
				type_next = rand() % 7 + 1;
				position_next = rand() % 4 + 1;
				for (int i = 0; i < FILL; i++)
				{
					next_coord[i].x = tetramino[type_next - 1][position_next - 1][i] % 4;
					next_coord[i].y = tetramino[type_next - 1][position_next - 1][i] / 4;
				}
			}

			// �����
			if (rotate == true)
			{
				t_rotate.play(); // ���������
			}
			if (move == 1 || move == -1)
			{
				t_move.play(); // ��� ������/��������
			}
			for (int i = 0; i < WIDTH; i++)
			{
				if (game_screen[1][i])
				{
					game_lose.play(); // �������
				}
			}

			move = 0;
			rotate = 0;
			delay = 0.9;
		}

		// ����������� ����
		tetris.clear(Color::White);
		tetris.draw(spr_background);

		// ����������� ��������� �������� (��� �� ������-��������)
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

		// ����������� ������ ��������
		for (int i = 0; i < FILL; i++)
		{
			// ���� ��������
			spr_square.setTextureRect(IntRect(type * SIZE, 0, SIZE, SIZE));
			spr_square.setPosition(base_coord[i].x * SIZE + 25, base_coord[i].y * SIZE + 20);
			tetris.draw(spr_square);
		}

		// ����������� ���������� ��������
		for (int i = 0; i < FILL; i++)
		{
			// ���� ��������
			spr_square_next.setTextureRect(IntRect(type_next * SIZE_NEXT, 0, SIZE_NEXT, SIZE_NEXT));
			spr_square_next.setPosition(next_coord[i].x * SIZE_NEXT, next_coord[i].y * SIZE_NEXT);

			// �������� ����������� ���������� �������� � ��������� �� ����
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

		// ����������� ����
		text.setString(Player_score);
		text.setPosition(389, 380);
		tetris.draw(text);

		// ����� ���
		if (!Endgame())
		{
			spr_lose.setPosition(0, 20);
			tetris.draw(spr_lose);
			spr_play.setPosition(50, 326);
			tetris.draw(spr_play);
			spr_exit.setPosition(50, 428);
			tetris.draw(spr_exit);

			// ��������� ������ ���� ��� ��� �����
			IntRect(50, 326, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_play.setColor(Color::Green) : spr_play.setColor(Color::White);
			IntRect(50, 428, 200, 88).contains(Mouse::getPosition(tetris)) ? spr_exit.setColor(Color::Red) : spr_exit.setColor(Color::White);

			// ������� ����� "Play", ��������� �������� ���� � ������� ���� ��� ��� (��� �������� "Exit")
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

		// ����������� ���� � ����
		tetris.display();
	}
	return 0;
}