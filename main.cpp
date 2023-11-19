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
const int screenWidth = cellSize * cellCount;
const int screenHeight = cellSize * cellCount;

double lastUpdateTime = 0;

// Local functions
static void UpdateFrame(void); // Update and draw one frame

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
        // Bad representation for collision?
        DrawCircle(pos.x * cellSize, pos.y * cellSize, cellSize / 2, darkGreen);
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

    Snake()  // init 3-cell snake in the center
    {   
        deque<Vector2> body = {
            Vector2{6, 9},
            Vector2{5, 9},
            Vector2{4, 9}
        };
    }

    void draw()
    {
        for(int i = 0; i < body.size(); i++)
        {
            Rectangle segment = {
                body[i].x * cellSize,
                body[i].y * cellSize,
                (float)cellSize,
                (float)cellSize,
            };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void move()
    {
        body.pop_back();
        body.push_front(Vector2Add(body[0], direction));
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);

    void draw()
    {
        food.draw();
        snake.draw();
    }

    void update()
    {
        snake.move();
        checkCollisionFood();
    }

    void checkCollisionFood()
    {
        if (Vector2Equals(snake.body[0], food.pos))
        {
            food.pos = food.spawnRandom(snake.body);
        }
    }
};

int main()
{
    cout << "Starting the game..." << endl;
    InitWindow(screenWidth, screenHeight, "Snake");
    SetTargetFPS(60);

    Game game = Game();

    // Game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (updateTriggered(0.2)) game.update();

        // Controls
        // pointer to direction?
        if (IsKeyDown(KEY_RIGHT) && game.snake.direction.x != -1) game.snake.direction = {1, 0};
        if (IsKeyDown(KEY_LEFT) && game.snake.direction.x != 1) game.snake.direction = {-1, 0};
        if (IsKeyDown(KEY_UP) && game.snake.direction.y != 1) game.snake.direction = {0, -1};
        if (IsKeyDown(KEY_DOWN) && game.snake.direction.y != -1) game.snake.direction = {0, 1};

        ClearBackground(green);
        DrawFPS(10, 10);
        game.draw();

        EndDrawing();
    }

    // Deinitialization
    CloseWindow();
    return 0;
}
