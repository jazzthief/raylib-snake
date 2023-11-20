#include <iostream>
#include <random>
#include <deque>
#include <raylib.h>
#include <raymath.h>

using namespace std;

// Colors
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

// Cell-based rendering
const int cellSize = 30;
const int cellCount = 25;
const int borderWidth = 75;  // px
// Playable area
const int screenWidth = cellSize * cellCount;
const int screenHeight = cellSize * cellCount;

double lastUpdateTime = 0;

// Check if `interval` (seconds) has passed
static bool updateTriggered(double interval)
{
    double curTime = GetTime();
    if (curTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = curTime;
        return true;
    }
    return false;
}

static bool isSnakeBody(Vector2 cell, deque<Vector2> snakeBody)
{
    for (unsigned int i = 0; i < snakeBody.size(); i++)
    {
        if (Vector2Equals(snakeBody[i], cell)) return true;
    }
    return false;
}

class Food
{
public:
    Vector2 pos;
    int lifetime;

    // pass a pointer to snakeBody?
    Food(deque<Vector2> snakeBody)
    {   
        pos = spawnRandom(snakeBody);
    }

    void draw()
    {
        Rectangle rect = {
            borderWidth + pos.x * cellSize,
            borderWidth + pos.y * cellSize,
            (float)cellSize,
            (float)cellSize
        };
        // using this to draw a circle because it's easier to position
        DrawRectangleRounded(rect, 1, 6, darkGreen);
    }

    Vector2 generateRandomPos()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 spawnRandom(deque<Vector2> snakeBody)
    {
        Vector2 foodPos = generateRandomPos();
        while (isSnakeBody(foodPos, snakeBody))
        {
            foodPos = generateRandomPos();
        }
        return foodPos;
    }
};

class Snake
{
public:
    // init at the center?
    deque<Vector2> body = {
        Vector2{6, 9},
        Vector2{5, 9},
        Vector2{4, 9}
    };
    Vector2 direction = {0, -1};
    bool shouldGrow = false;

    void draw()
    {
        for(int i = 0; i < body.size(); i++)
        {
            Rectangle segment = {
                borderWidth + body[i].x * cellSize,
                borderWidth + body[i].y * cellSize,
                (float)cellSize,
                (float)cellSize
            };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (shouldGrow) shouldGrow = false;
        else body.pop_back();
    }

    void reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;

    void draw()
    {
        food.draw();
        snake.draw();
    }

    void update()
    {
        if (running)
        {
            snake.update();
            checkCollisionFood();
            checkCollisionBody();
            checkCollisionEdge();
        }
    }

    void checkCollisionFood()
    {
        if (Vector2Equals(snake.body[0], food.pos))
        {
            snake.shouldGrow = true;
            food.pos = food.spawnRandom(snake.body);
            score++;
        }
    }

    void checkCollisionBody()
    {
        // Don't copy the deque?
        deque<Vector2> bodyWithoutHead = snake.body;
        bodyWithoutHead.pop_front();
        if (isSnakeBody(snake.body[0], bodyWithoutHead)) gameOver();
    }

    void checkCollisionEdge()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1) gameOver();
        if (snake.body[0].y == cellCount || snake.body[0].y == -1) gameOver();
    }

    void gameOver()
    {
        snake.reset();
        food.pos = food.spawnRandom(snake.body);
        running = false;
        score = 0;
    }
};

int main()
{
    cout << "Starting the game..." << endl;
    InitWindow(2 * borderWidth + screenWidth, 2 * borderWidth + screenHeight, "Snake");
    SetTargetFPS(60);

    Game game = Game();

    // Game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (updateTriggered(0.2)) game.update();

        // Controls
        // pointer to direction?
        if (IsKeyDown(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }
        if (IsKeyDown(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyDown(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyDown(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }

        ClearBackground(green);

        // Interface
        Rectangle borderRectangle = {
            (float)borderWidth - 5,
            (float)borderWidth - 5,
            (float)cellSize * cellCount + 10,
            (float)cellSize * cellCount + 10
        };
        DrawRectangleLinesEx(borderRectangle, 5, darkGreen);
        DrawFPS(borderWidth + screenWidth - 10, 10);
        DrawText(TextFormat("%03i", game.score), borderWidth - 5, 20, 40, darkGreen);
        game.draw();

        EndDrawing();
    }

    // Deinitialization
    CloseWindow();
    return 0;
}
