/*
	License
	~~~~~~~
	Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details. 
	Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9
	GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE
	
	This game of tetris is coded up by following the video at https://www.youtube.com/javidx9
	I highly recommend that channel for anyone wanting to code good mini projects.
	This code is not a fork of the repo at https://www.github.com/onelonecoder.
*/

#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>


using namespace std;


int TETROMINO_BLOCK_WIDTH = 4;
int BOARD_WIDTH = 12;
int BOARD_HEIGHT = 18;

// Adjust these values based on the size of your terminal. Go to properties -> Layout.
int SCREEN_WIDTH = 120;
int SCREEN_HEIGHT = 30;


wstring tetromino[7];
unsigned char* board = nullptr;
bool keyPressed[4];
vector<int> lines;

int Rotate(int x, int y, int rotate) {
	int i = 0;
	switch (rotate % 4) {
	case 0:
		i = y * TETROMINO_BLOCK_WIDTH + x;
		break;
	case 1:
		i = 12 + y - TETROMINO_BLOCK_WIDTH * x;
		break;
	case 2:
		i = 12 + x - TETROMINO_BLOCK_WIDTH * y;
		break;
	case 3:
		i = y + TETROMINO_BLOCK_WIDTH * x;
		break;
	}
	return i;
}

bool DoesPieceFit(int tetrominoNo, int rotation, int topLeftX, int topLeftY) {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; ++y) {
			int pi = Rotate(x, y, rotation);
			int bi = (topLeftY + y) * BOARD_WIDTH + topLeftX + x;

			if (topLeftX + x >= 0 && topLeftX + x < BOARD_WIDTH && topLeftY + y >= 0 && topLeftY + y < BOARD_HEIGHT) {
				if (tetromino[tetrominoNo][pi] == L'X' && board[bi] != 0) return false;
			}
		}
	}
}

int main()
{
	srand(time(NULL));
	// Create Screen Buffer
	wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"....");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L".X..");

	tetromino[2].append(L"....");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"..X.");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L".X..");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..XX");
	tetromino[6].append(L"...X");
	tetromino[6].append(L"....");

	// Initialize empty playing board.
	board = new unsigned char[BOARD_WIDTH * BOARD_HEIGHT];
	for (int x = 0; x < BOARD_WIDTH; ++x) {
		for (int y = 0; y < BOARD_HEIGHT; ++y) {
			if (y == BOARD_HEIGHT - 1 || x == 0 || x == BOARD_WIDTH - 1)
				board[y * BOARD_WIDTH + x] = 9;
			else board[y * BOARD_WIDTH + x] = 0;

			//cout << board[i * BOARD_WIDTH + j] << " ";
		}
		//cout << endl;
	}

	bool gameOver = false;
	int currentX = BOARD_WIDTH / 2;
	int currentY = 0;
	int tetrominoNo= 0;
	int currentRotation = 0;
	bool zHold = true;
	bool forceDown = false;
	int speed = 20;
	int speedCounter = 0;
	int score = 0;
	int pieceCount = 1;

	while (!gameOver) {
		// Timing
		this_thread::sleep_for(50ms);
		speedCounter++;
		forceDown = (speed == speedCounter);

		// Input
		for (int k = 0; k < 4; k++)								// R   L   D Z
			keyPressed[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		// Game Logic
		currentX += (keyPressed[0] && DoesPieceFit(tetrominoNo, currentRotation, currentX + 1, currentY)) ? 1 : 0;
		currentX -= (keyPressed[1] && DoesPieceFit(tetrominoNo, currentRotation, currentX - 1, currentY)) ? 1 : 0;
		currentY += (keyPressed[2] && DoesPieceFit(tetrominoNo, currentRotation, currentX, currentY + 1)) ? 1 : 0;

		if (keyPressed[3]) {
			currentRotation += (zHold && DoesPieceFit(tetrominoNo, currentRotation + 1, currentX, currentY)) ? 1 : 0;
			zHold = false;
		}
		else {
			zHold = true;
		}

		if (forceDown) {
			speedCounter = 0;
			if (DoesPieceFit(tetrominoNo, currentRotation, currentX, currentY + 1)) {
				currentY++;
			}
			else {
				// Fix it in place
				for (int px = 0; px < 4; px++) {
					for (int py = 0; py < 4; py++) {
						if (tetromino[tetrominoNo][Rotate(px, py, currentRotation)] != L'.')
							board[(currentY + py) * BOARD_WIDTH + currentX + px] = tetrominoNo + 1;
					}
				}

				// Check for lines
				bool line;
				for (int py = 0; py < 4; py++) {
					if (currentY + py < BOARD_HEIGHT - 1) {
						line = true;
						for (int x = 1; x < BOARD_WIDTH - 1; x++) {
							if (board[(currentY + py) * BOARD_WIDTH + x] == 0) {
								line = false;
								break;
							}
						}
						if (line) {
							for (int x = 1; x < BOARD_WIDTH - 1; x++) {
								board[(currentY + py) * BOARD_WIDTH + x] = 8;
							}
							lines.push_back(currentY + py);
						}
					}
				}

				score += 25;
				if (!lines.empty())	score += (1 << lines.size()) * 100;

				// Create new piece
				currentY = 0;
				currentX = BOARD_WIDTH / 2;
				tetrominoNo = rand() % 7;
				currentRotation = 0;
				pieceCount = (pieceCount + 1) % 5;
				if (pieceCount == 1 && speed >= 10) speed-=5;

				// If new piece doesn't fit, game over.

				gameOver = !DoesPieceFit(tetrominoNo, currentRotation, currentX, currentY);
			}

		}
		// Display

		for (int x = 0; x < BOARD_WIDTH; x++) {
			for (int y = 0; y < BOARD_HEIGHT; y++) {
				screen[(y + 2) * SCREEN_WIDTH + x + 2] = L" ABCDEFG=#"[board[y * BOARD_WIDTH + x]];
			}
		}

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[tetrominoNo][Rotate(px, py, currentRotation)] != L'.')
					screen[(currentY + py + 2) * SCREEN_WIDTH + (currentX + px + 2)] = tetrominoNo + 65;

		// Draw Score
		swprintf_s(&screen[2 * SCREEN_WIDTH + BOARD_WIDTH + 6], 16, L"SCORE: %8d", score);


		// Clear out line
		if (!lines.empty()) {
			WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH* SCREEN_HEIGHT, { 0,0 }, & dwBytesWritten);
			this_thread::sleep_for(400ms);

			for (int v: lines) {
				for (int x = 1; x < BOARD_WIDTH - 1; x++) {
					for (int y = v; y > 0; y--) {
						board[y * BOARD_WIDTH + x] = board[(y - 1) * BOARD_WIDTH + x];
					}
					board[x] = 0;
				}
			}
			lines.clear();
		}

		// Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);
	}
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << score << endl;
	system("pause");
	return 0;
}
