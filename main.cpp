#include "dependencies/Include/GLFW/glfw3.h"
#include <vector>
#include <cstdlib>
#include <ctime>

// Function Declarations
void DrawSquare(const float r, const float g, const float b, const float x, const float y);
void SpawnFood();

// Struct Definitions
struct Point {
    float x;
    float y;
};

// Global Game Variables
const int GRID_SIZE = 8;
const float CELL_SIZE = 0.25f;
const float START_OFFSET = -0.875f; // Centers the tiles exactly within [-1.0, 1.0]

std::vector<Point> snake;
Point direction = { 0.0f, 0.0f }; // Starts stationary until a key is pressed
Point food;

// Timing variables for grid-locked movement
double lastMoveTime = 0.0;
const double MOVE_DELAY = 0.25; // Snake moves every 0.25 seconds

void SpawnFood() {
    // Generate a random position on the 8x8 grid
    int gridX = rand() % GRID_SIZE;
    int gridY = rand() % GRID_SIZE;
    
    food.x = START_OFFSET + (gridX * CELL_SIZE);
    food.y = START_OFFSET + (gridY * CELL_SIZE);
}

int main(void) {
    srand(static_cast<unsigned int>(time(NULL)));

    if (!glfwInit()) {
        return -1;
    }

    // Changed to 600x600 square window so the grid isn't stretched stretched
    GLFWwindow* window = glfwCreateWindow(600, 600, "Snake Game", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize Snake in the exact middle of the 8x8 grid
    // Rows/cols are index 0 to 7. Middle indices are 3 and 4.
    float spawnX = START_OFFSET + (3 * CELL_SIZE); 
    float spawnY = START_OFFSET + (3 * CELL_SIZE);
    snake.push_back({ spawnX, spawnY });

    SpawnFood();
    lastMoveTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // 1. INPUT HANDLING (Updates intent instantly)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if (direction.y == 0.0f) { direction = { 0.0f, CELL_SIZE }; }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (direction.y == 0.0f) { direction = { 0.0f, -CELL_SIZE }; }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (direction.x == 0.0f) { direction = { -CELL_SIZE, 0.0f }; }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (direction.x == 0.0f) { direction = { CELL_SIZE, 0.0f }; }
        }

        // 2. GAME LOGIC (Runs on a tick timer for classic step-by-step movement)
        double currentTime = glfwGetTime();
        if (currentTime - lastMoveTime >= MOVE_DELAY) {
            if (direction.x != 0.0f || direction.y != 0.0f) {
                // Calculate new head position
                Point newHead = { snake.front().x + direction.x, snake.front().y + direction.y };

                // Wall Collision Check (Out of NDC bounds [-1, 1])
                if (newHead.x < -1.0f || newHead.x > 1.0f || newHead.y < -1.0f || newHead.y > 1.0f) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE); // Game Over
                }

                // Self Collision Check
                for (const auto& segment : snake) {
                    if (abs(newHead.x - segment.x) < 0.01f && abs(newHead.y - segment.y) < 0.01f) {
                        glfwSetWindowShouldClose(window, GLFW_TRUE); // Game Over
                    }
                }

                // Insert new head
                snake.insert(snake.begin(), newHead);

                // Food Collision Check
                if (abs(newHead.x - food.x) < 0.01f && abs(newHead.y - food.y) < 0.01f) {
                    SpawnFood(); // Keep tail segment (grow)
                } else {
                    snake.pop_back(); // Remove tail segment (normal movement)
                }
            }
            lastMoveTime = currentTime;
        }

        // 3. RENDERING
        // Draw symmetrical 8x8 checkerboard grid
        for (int rows = 0; rows < GRID_SIZE; ++rows) {
            for (int cols = 0; cols < GRID_SIZE; ++cols) {
                float targetX = START_OFFSET + (cols * CELL_SIZE);
                float targetY = START_OFFSET + (rows * CELL_SIZE);
                
                if ((rows + cols) % 2 == 0) {
                    DrawSquare(0.0f, 0.8f, 0.0f, targetX, targetY);
                } else {
                    DrawSquare(0.0f, 0.6f, 0.0f, targetX, targetY);
                }
            }
        }

        // Draw Food (Red)
        DrawSquare(1.0f, 0.0f, 0.0f, food.x, food.y);

        // Draw Snake Body segments (Blue)
        for (const auto& segment : snake) {
            DrawSquare(0.0f, 0.0f, 1.0f, segment.x, segment.y);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// Fixed sizing and rendering boundaries to fill cells precisely without overlapping
void DrawSquare(const float r, const float g, const float b, const float x, const float y) {
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x - 0.125f, y - 0.125f);
    glVertex2f(x + 0.125f, y - 0.125f);
    glVertex2f(x + 0.125f, y + 0.125f);
    glVertex2f(x - 0.125f, y + 0.125f);
    glEnd();
}
