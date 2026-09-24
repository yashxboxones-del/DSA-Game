#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <queue>
#include <string>
#include <vector>

using namespace std;

const int SCREEN_W = 1100;
const int SCREEN_H = 700;

struct EnemyCar {
    Rectangle rect;
    float speed;
    Color color;
    int id;
};

struct Coin {
    Vector2 position;
    float radius;
    bool active;
};

float clampValue(float value, float minimum, float maximum) {
    return max(minimum, min(value, maximum));
}

Color randomCarColor() {
    Color colors[] = {
        RED, BLUE, ORANGE, PURPLE, LIME, GOLD, MAROON
    };

    return colors[GetRandomValue(0, 6)];
}

void drawCar(Rectangle car, Color body, bool player = false) {
    // Shadow
    DrawEllipse(
        (int)(car.x + car.width / 2),
        (int)(car.y + car.height + 7),
        (int)(car.width * 0.55f),
        8,
        Fade(BLACK, 0.3f)
    );

    // Main body
    DrawRectangleRounded(car, 0.18f, 8, body);

    // Windows
    Rectangle window = {
        car.x + car.width * 0.15f,
        car.y + car.height * 0.13f,
        car.width * 0.70f,
        car.height * 0.30f
    };

    DrawRectangleRounded(window, 0.15f, 8, DARKBLUE);

    // Windshield reflection
    DrawRectangleRounded(
        {
            car.x + car.width * 0.22f,
            car.y + car.height * 0.19f,
            car.width * 0.56f,
            car.height * 0.12f
        },
        0.1f,
        6,
        Fade(SKYBLUE, 0.7f)
    );

    // Lights
    DrawRectangle(
        (int)(car.x + 5),
        (int)(car.y + car.height * 0.72f),
        (int)(car.width * 0.20f),
        6,
        YELLOW
    );

    DrawRectangle(
        (int)(car.x + car.width - 5 - car.width * 0.20f),
        (int)(car.y + car.height * 0.72f),
        (int)(car.width * 0.20f),
        6,
        YELLOW
    );

    // Wheels
    DrawCircle((int)(car.x + 7), (int)(car.y + car.height * 0.25f), 6, BLACK);
    DrawCircle((int)(car.x + car.width - 7), (int)(car.y + car.height * 0.25f), 6, BLACK);
    DrawCircle((int)(car.x + 7), (int)(car.y + car.height * 0.78f), 6, BLACK);
    DrawCircle((int)(car.x + car.width - 7), (int)(car.y + car.height * 0.78f), 6, BLACK);

    if (player) {
        DrawRectangleLinesEx(car, 2, WHITE);
    }
}

bool circleRectangleCollision(Vector2 circle, float radius, Rectangle rectangle) {
    float closestX = clampValue(
        circle.x,
        rectangle.x,
        rectangle.x + rectangle.width
    );

    float closestY = clampValue(
        circle.y,
        rectangle.y,
        rectangle.y + rectangle.height
    );

    float dx = circle.x - closestX;
    float dy = circle.y - closestY;

    return dx * dx + dy * dy <= radius * radius;
}

int main() {
    InitWindow(
        SCREEN_W,
        SCREEN_H,
        "Street Rush - Queue Based C++ DSA Game"
    );

    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(nullptr));

    enum GameState {
        MENU,
        PLAYING,
        PAUSED,
        GAME_OVER
    };

    GameState gameState = MENU;

    // --------------------------------------------------------
    // ROAD
    // --------------------------------------------------------

    const float roadLeft = 230.0f;
    const float roadRight = 790.0f;
    const float laneWidth = (roadRight - roadLeft) / 3.0f;

    // --------------------------------------------------------
    // PLAYER
    // --------------------------------------------------------

    Rectangle player = {
        SCREEN_W / 2.0f - 30.0f,
        SCREEN_H - 145.0f,
        60.0f,
        105.0f
    };

    // --------------------------------------------------------
    // QUEUE DATA STRUCTURE
    // --------------------------------------------------------
    //
    // This queue stores enemy cars.
    //
    // FRONT -> [Car 1] [Car 2] [Car 3] [Car 4] <- BACK
    //
    // dequeue() removes Car 1.
    // enqueue() adds a new car after Car 4.
    //
    // --------------------------------------------------------

    queue<EnemyCar> trafficQueue;

    // Cars that are currently visible on the road.
    // They are copied from the queue when they become active.
    vector<EnemyCar> activeCars;

    vector<Coin> coins;

    int nextCarID = 1;

    // --------------------------------------------------------
    // GAME VARIABLES
    // --------------------------------------------------------

    float score = 0;
    int coinCount = 0;

    float speed = 7.0f;
    float nitro = 100.0f;

    bool usingNitro = false;

    float roadOffset = 0;
    float spawnTimer = 0;
    float coinTimer = 0;

    // --------------------------------------------------------
    // RESET GAME
    // --------------------------------------------------------

    auto resetGame = [&]() {
        while (!trafficQueue.empty()) {
            trafficQueue.pop();
        }

        activeCars.clear();
        coins.clear();

        player.x = SCREEN_W / 2.0f - player.width / 2.0f;
        player.y = SCREEN_H - 145.0f;

        score = 0;
        coinCount = 0;

        speed = 7.0f;
        nitro = 100.0f;

        usingNitro = false;

        roadOffset = 0;
        spawnTimer = 0;
        coinTimer = 0;

        nextCarID = 1;
    };

    // --------------------------------------------------------
    // MAIN GAME LOOP
    // --------------------------------------------------------

    while (!WindowShouldClose()) {

        float deltaTime = GetFrameTime();

        // ====================================================
        // INPUT
        // ====================================================

        bool moveLeft =
            IsKeyDown(KEY_LEFT) ||
            IsKeyDown(KEY_A);

        bool moveRight =
            IsKeyDown(KEY_RIGHT) ||
            IsKeyDown(KEY_D);

        usingNitro =
            IsKeyDown(KEY_SPACE) &&
            nitro > 0 &&
            gameState == PLAYING;

        // Pause
        if (IsKeyPressed(KEY_P)) {
            if (gameState == PLAYING)
                gameState = PAUSED;
            else if (gameState == PAUSED)
                gameState = PLAYING;
        }

        // Start
        if (gameState == MENU &&
            IsKeyPressed(KEY_ENTER)) {

            resetGame();
            gameState = PLAYING;
        }

        // Restart
        if (gameState == GAME_OVER &&
            IsKeyPressed(KEY_R)) {

            resetGame();
            gameState = PLAYING;
        }

        // ====================================================
        // GAME UPDATE
        // ====================================================

        if (gameState == PLAYING) {

            float currentSpeed = speed;

            // ----------------------------
            // NITRO
            // ----------------------------

            if (usingNitro) {
                currentSpeed += 7.0f;
                nitro -= 30.0f * deltaTime;
            }
            else {
                nitro += 10.0f * deltaTime;
            }

            nitro = clampValue(nitro, 0, 100);

            // ----------------------------
            // PLAYER MOVEMENT
            // ----------------------------

            const float steeringSpeed = 430.0f;

            if (moveLeft)
                player.x -= steeringSpeed * deltaTime;

            if (moveRight)
                player.x += steeringSpeed * deltaTime;

            player.x = clampValue(
                player.x,
                roadLeft + 18,
                roadRight - player.width - 18
            );

            // ----------------------------
            // ROAD
            // ----------------------------

            roadOffset += currentSpeed * 70 * deltaTime;

            if (roadOffset > 80)
                roadOffset -= 80;

            // ----------------------------
            // SCORE
            // ----------------------------

            score += currentSpeed * 2 * deltaTime;

            // Increase difficulty
            speed += 0.035f * deltaTime;

            speed = clampValue(speed, 7, 16);

            // =================================================
            // QUEUE ENQUEUE
            // =================================================
            //
            // New enemy car enters the BACK of the queue.
            //
            // queue.push(enemyCar)
            //
            // =================================================

            spawnTimer += deltaTime;

            float spawnTime =
                max(0.45f, 1.15f - speed * 0.035f);

            if (spawnTimer >= spawnTime) {

                spawnTimer = 0;

                int lane = GetRandomValue(0, 2);

                float x =
                    roadLeft +
                    lane * laneWidth +
                    laneWidth / 2 -
                    27;

                EnemyCar newCar;

                newCar.rect = {
                    x,
                    -125,
                    54,
                    96
                };

                newCar.speed =
                    currentSpeed *
                    (0.55f +
                     GetRandomValue(0, 35) / 100.0f);

                newCar.color = randomCarColor();

                newCar.id = nextCarID++;

                // QUEUE ENQUEUE
                trafficQueue.push(newCar);
            }

            // =================================================
            // MOVE QUEUED CAR TO ACTIVE ROAD
            // =================================================
            //
            // If the road has space for another car,
            // take the FRONT car from the queue.
            //
            // front() -> view it
            // pop()   -> dequeue it
            //
            // =================================================

            if (!trafficQueue.empty() &&
                activeCars.size() < 8) {

                EnemyCar nextCar = trafficQueue.front();

                trafficQueue.pop();

                // QUEUE DEQUEUE COMPLETE
                activeCars.push_back(nextCar);
            }

            // =================================================
            // MOVE ACTIVE CARS
            // =================================================

            for (auto &car : activeCars) {

                car.rect.y +=
                    (currentSpeed + car.speed) *
                    35.0f *
                    deltaTime;
            }

            // =================================================
            // COLLISION
            // =================================================

            Rectangle playerHitbox = {
                player.x + 8,
                player.y + 10,
                player.width - 16,
                player.height - 18
            };

            for (const auto &car : activeCars) {

                Rectangle enemyHitbox = {
                    car.rect.x + 7,
                    car.rect.y + 10,
                    car.rect.width - 14,
                    car.rect.height - 18
                };

                if (CheckCollisionRecs(
                    playerHitbox,
                    enemyHitbox
                )) {

                    gameState = GAME_OVER;
                    usingNitro = false;
                    break;
                }
            }

            // =================================================
            // REMOVE CARS THAT LEFT THE ROAD
            // =================================================
            //
            // This demonstrates FIFO processing:
            // once the car finishes its journey, it is removed.
            //
            // =================================================

            activeCars.erase(
                remove_if(
                    activeCars.begin(),
                    activeCars.end(),
                    [](const EnemyCar &car) {
                        return car.rect.y >
                               SCREEN_H + 150;
                    }
                ),
                activeCars.end()
            );

            // =================================================
            // COINS
            // =================================================

            coinTimer += deltaTime;

            if (coinTimer >= 0.65f) {

                coinTimer = 0;

                int lane = GetRandomValue(0, 2);

                float x =
                    roadLeft +
                    lane * laneWidth +
                    laneWidth / 2;

                Coin coin;

                coin.position = {x, -20};
                coin.radius = 12;
                coin.active = true;

                coins.push_back(coin);
            }

            for (auto &coin : coins) {

                coin.position.y +=
                    currentSpeed * 38 * deltaTime;

                if (
                    coin.active &&
                    circleRectangleCollision(
                        coin.position,
                        coin.radius,
                        playerHitbox
                    )
                ) {

                    coin.active = false;

                    coinCount++;

                    score += 100;

                    nitro =
                        clampValue(
                            nitro + 8,
                            0,
                            100
                        );
                }
            }

            coins.erase(
                remove_if(
                    coins.begin(),
                    coins.end(),
                    [](const Coin &coin) {
                        return !coin.active ||
                               coin.position.y >
                               SCREEN_H + 30;
                    }
                ),
                coins.end()
            );
        }

        // ====================================================
        // DRAW
        // ====================================================

        BeginDrawing();

        ClearBackground(SKYBLUE);

        // Sky
        DrawRectangle(
            0, 0,
            SCREEN_W,
            SCREEN_H,
            Color{105, 190, 235, 255}
        );

        // Mountains
        DrawTriangle(
            {0, 280},
            {110, 170},
            {220, 280},
            DARKGREEN
        );

        DrawTriangle(
            {170, 280},
            {310, 145},
            {440, 280},
            DARKGREEN
        );

        DrawTriangle(
            {(float)SCREEN_W, 280},
            {900, 150},
            {780, 280},
            DARKGREEN
        );

        DrawTriangle(
            {800, 280},
            {690, 160},
            {570, 280},
            DARKGREEN
        );

        // Grass
        DrawRectangle(
            0,
            280,
            SCREEN_W,
            SCREEN_H - 280,
            GREEN
        );

        // Road
        DrawRectangle(
            roadLeft,
            0,
            roadRight - roadLeft,
            SCREEN_H,
            DARKGRAY
        );

        // Road borders
        DrawRectangle(
            roadLeft,
            0,
            10,
            SCREEN_H,
            WHITE
        );

        DrawRectangle(
            roadRight - 10,
            0,
            10,
            SCREEN_H,
            WHITE
        );

        // Lane markings
        for (int lane = 1; lane < 3; lane++) {

            float x =
                roadLeft +
                lane * laneWidth;

            for (
                float y = -80 + roadOffset;
                y < SCREEN_H;
                y += 80
            ) {

                DrawRectangle(
                    x - 4,
                    y,
                    8,
                    42,
                    WHITE
                );
            }
        }

        // Coins
        for (const auto &coin : coins) {

            DrawCircle(
                coin.position.x,
                coin.position.y,
                coin.radius,
                GOLD
            );

            DrawCircleLines(
                coin.position.x,
                coin.position.y,
                coin.radius,
                ORANGE
            );

            DrawText(
                "$",
                coin.position.x - 5,
                coin.position.y - 9,
                18,
                BROWN
            );
        }

        // Active enemy cars
        for (const auto &car : activeCars) {
            drawCar(car.rect, car.color);
        }

        // Player
        drawCar(player, RED, true);

        // Nitro flame
        if (usingNitro) {

            DrawTriangle(
                {
                    player.x + 17,
                    player.y + player.height
                },
                {
                    player.x + 30,
                    player.y + player.height + 42
                },
                {
                    player.x + 43,
                    player.y + player.height
                },
                ORANGE
            );

            DrawTriangle(
                {
                    player.x + 22,
                    player.y + player.height
                },
                {
                    player.x + 30,
                    player.y + player.height + 27
                },
                {
                    player.x + 38,
                    player.y + player.height
                },
                YELLOW
            );
        }

        // ====================================================
        // MAIN HUD
        // ====================================================

        DrawRectangle(
            18,
            18,
            325,
            125,
            Fade(BLACK, 0.65f)
        );

        DrawText(
            "STREET RUSH",
            32,
            27,
            25,
            WHITE
        );

        DrawText(
            TextFormat(
                "Score: %06i",
                (int)score
            ),
            32,
            59,
            20,
            WHITE
        );

        DrawText(
            TextFormat(
                "Coins: %i",
                coinCount
            ),
            32,
            86,
            20,
            GOLD
        );

        DrawText(
            TextFormat(
                "Speed: %i",
                (int)(
                    speed *
                    (usingNitro ? 1.8f : 1.0f)
                )
            ),
            175,
            86,
            20,
            SKYBLUE
        );

        // ====================================================
        // QUEUE PANEL
        // ====================================================
        //
        // This is the important DSA demonstration.
        //
        // ====================================================

        DrawRectangle(
            SCREEN_W - 300,
            100,
            270,
            250,
            Fade(BLACK, 0.72f)
        );

        DrawText(
            "TRAFFIC QUEUE",
            SCREEN_W - 280,
            118,
            23,
            GOLD
        );

        DrawText(
            "FIFO: First In, First Out",
            SCREEN_W - 280,
            148,
            16,
            LIGHTGRAY
        );

        DrawText(
            TextFormat(
                "Waiting: %i",
                (int)trafficQueue.size()
            ),
            SCREEN_W - 280,
            177,
            19,
            WHITE
        );

        if (!trafficQueue.empty()) {

            DrawText(
                TextFormat(
                    "FRONT: Car #%i",
                    trafficQueue.front().id
                ),
                SCREEN_W - 280,
                208,
                18,
                GREEN
            );

            DrawText(
                "Next car will enter road",
                SCREEN_W - 280,
                233,
                15,
                LIGHTGRAY
            );
        }
        else {

            DrawText(
                "Queue is EMPTY",
                SCREEN_W - 280,
                208,
                18,
                RED
            );
        }

        DrawLine(
            SCREEN_W - 280,
            260,
            SCREEN_W - 55,
            260,
            GRAY
        );

        DrawText(
            "ENQUEUE -> Add car",
            SCREEN_W - 280,
            275,
            15,
            WHITE
        );

        DrawText(
            "DEQUEUE -> Remove front",
            SCREEN_W - 280,
            298,
            15,
            WHITE
        );

        DrawText(
            "FRONT -> View first car",
            SCREEN_W - 280,
            321,
            15,
            WHITE
        );

        // ====================================================
        // NITRO
        // ====================================================

        DrawText(
            "NITRO",
            SCREEN_W - 235,
            25,
            18,
            WHITE
        );

        DrawRectangle(
            SCREEN_W - 235,
            50,
            200,
            20,
            Fade(BLACK, 0.65f)
        );

        DrawRectangle(
            SCREEN_W - 235,
            50,
            (int)(200 * nitro / 100.0f),
            20,
            ORANGE
        );

        DrawRectangleLines(
            SCREEN_W - 235,
            50,
            200,
            20,
            WHITE
        );

        // ====================================================
        // MENU
        // ====================================================

        if (gameState == MENU) {

            DrawRectangle(
                0,
                0,
                SCREEN_W,
                SCREEN_H,
                Fade(BLACK, 0.48f)
            );

            const char *title =
                "STREET RUSH";

            const char *subtitle =
                "QUEUE-BASED HIGHWAY RACING";

            const char *start =
                "PRESS ENTER TO START";

            DrawText(
                title,
                SCREEN_W / 2 -
                    MeasureText(title, 58) / 2,
                190,
                58,
                WHITE
            );

            DrawText(
                subtitle,
                SCREEN_W / 2 -
                    MeasureText(subtitle, 24) / 2,
                260,
                24,
                GOLD
            );

            DrawText(
                "DSA CONCEPT: QUEUE (FIFO)",
                SCREEN_W / 2 -
                    MeasureText(
                        "DSA CONCEPT: QUEUE (FIFO)",
                        23
                    ) / 2,
                310,
                23,
                SKYBLUE
            );

            DrawText(
                start,
                SCREEN_W / 2 -
                    MeasureText(start, 25) / 2,
                375,
                25,
                WHITE
            );

            DrawText(
                "A/D or ARROWS = STEER    SPACE = NITRO    P = PAUSE",
                SCREEN_W / 2 - 285,
                425,
                18,
                LIGHTGRAY
            );
        }

        // ====================================================
        // PAUSE
        // ====================================================

        if (gameState == PAUSED) {

            DrawRectangle(
                0,
                0,
                SCREEN_W,
                SCREEN_H,
                Fade(BLACK, 0.5f)
            );

            const char *paused =
                "PAUSED";

            DrawText(
                paused,
                SCREEN_W / 2 -
                    MeasureText(paused, 58) / 2,
                285,
                58,
                WHITE
            );

            DrawText(
                "Press P to continue",
                SCREEN_W / 2 -
                    MeasureText(
                        "Press P to continue",
                        22
                    ) / 2,
                360,
                22,
                LIGHTGRAY
            );
        }

        // ====================================================
        // GAME OVER
        // ====================================================

        if (gameState == GAME_OVER) {

            DrawRectangle(
                0,
                0,
                SCREEN_W,
                SCREEN_H,
                Fade(BLACK, 0.6f)
            );

            const char *gameOver =
                "GAME OVER";

            DrawText(
                gameOver,
                SCREEN_W / 2 -
                    MeasureText(gameOver, 58) / 2,
                220,
                58,
                RED
            );

            DrawText(
                TextFormat(
                    "Final Score: %i",
                    (int)score
                ),
                SCREEN_W / 2 - 100,
                310,
                25,
                WHITE
            );

            DrawText(
                TextFormat(
                    "Coins: %i",
                    coinCount
                ),
                SCREEN_W / 2 - 55,
                348,
                22,
                GOLD
            );

            const char *restart =
                "PRESS R TO RESTART";

            DrawText(
                restart,
                SCREEN_W / 2 -
                    MeasureText(restart, 25) / 2,
                420,
                25,
                WHITE
            );
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}