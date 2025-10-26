#include "CS3113/Entity.h"
#include "CS3113/cs3113.h"

constexpr float FIXED_TIMESTEP = 0.016f;

// Global Constants
constexpr int SCREEN_WIDTH  = 1800;
constexpr int SCREEN_HEIGHT = 1200;
constexpr int FPS           = 120;

constexpr char BG_COLOUR[]    = "#1e0028ff";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

Vector2 tilePositions[] = {
    {50, 450}, {125, 400}, {200, 450}, {275, 400}, {350, 450},
    {425, 400}, {500, 450}, {575, 400}, {650, 450}, {725, 400},
    {800, 450}, {875, 400}, {950, 450}, {1025, 400}, {1100, 450},
    {1175, 400}, {1250, 450}, {1325, 400},

    
    {1800, 850}, {1725, 900}, {1650, 850}, {1575, 900}, {1500, 850},
    {1425, 900}, {1350, 850}, {1275, 900}, {1200, 850}, {1125, 900},
    {1050, 850}, {975, 900}, {900, 850}, {825, 900}, {750, 850},
    {675, 900}, {600, 850}, {525, 900}, {475, 850}
};
constexpr int   NUMBER_OF_TILES     = 36;

constexpr float TILE_DIMENSION      = 50.0f;
constexpr float ROCKET_DIMENSION    = 10.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gTimeAccumulator = 0.0f;
float gFuel = 1000.0f;

Entity *gRocket = nullptr;
Entity *gTiles  = nullptr;
Entity* gMovingPlatform = nullptr;

float platformTime = 0.0f;
constexpr float PLATFORM_SPEED = 2.0f;
constexpr float PLATFORM_AMPLITUDE = 100.0f;
constexpr Vector2 PLATFORM_SIZE = {200.0f, 20.0f};

Texture2D gTextureBackground;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void drawHUD(Entity* rocket);

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");
    SetTargetFPS(FPS);
    
    gTextureBackground = LoadTexture("assets/game/background.png");

    gRocket = new Entity(
        { ORIGIN.x - 550.0f, 100.0f },
        { 3.0f, 3.0f },
        "assets/game/rocket.png"
    );

    gMovingPlatform = new Entity(
        { 100.0f, 5050.0f },  // x=300 to the left, y lower on the screen
        {3.0f, 3.0f},
        "assets/game/purpur.png"
    );

    gTiles = new Entity[NUMBER_OF_TILES];
    for (int i = 0; i < NUMBER_OF_TILES; i++) {
        gTiles[i].setTexture("assets/game/endstone.png");
        gTiles[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setPosition(tilePositions[i]);
    }
}

void processInput() 
{
    gRocket->clearInputFlags();

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) gRocket->rotateLeft();
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) gRocket->rotateRight();
    if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && gFuel > 0) {
        gRocket->engageThruster();
        gFuel--;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    if (IsKeyPressed(KEY_R) && (gAppStatus == FAILED || gAppStatus == WIN)) {
        delete gRocket;
        delete[] gTiles;

        gAppStatus = RUNNING;

        gRocket = new Entity(
            { ORIGIN.x - 550.0f, 100.0f },
            { 3.0f, 3.0f },
            "assets/game/rocket.png"
        );

        gTiles = new Entity[NUMBER_OF_TILES];
        for (int i = 0; i < NUMBER_OF_TILES; i++) {
            gTiles[i].setTexture("assets/game/endstone.png");
            gTiles[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
            gTiles[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION});
            gTiles[i].setPosition(tilePositions[i]);
        }

        gFuel = 1000.0f;
    }
}

void update() 
{
    float deltaTime = GetFrameTime();
    gTimeAccumulator += deltaTime;
    platformTime += deltaTime;

    while (gTimeAccumulator >= FIXED_TIMESTEP)
    {
        gRocket->update(FIXED_TIMESTEP);

        Vector2 rpos = gRocket->getPosition();
        if (rpos.x < 0 || rpos.x > SCREEN_WIDTH || rpos.y < 0 || rpos.y > SCREEN_HEIGHT) {
            gAppStatus = FAILED;
        }
        
        Vector2 platPos = gMovingPlatform->getPosition();
        platPos.y = (SCREEN_HEIGHT - 150.0f) + PLATFORM_AMPLITUDE * sinf(PLATFORM_SPEED * platformTime);
        gMovingPlatform->setPosition(platPos);
        
        for (int i = 0; i < NUMBER_OF_TILES; i++) {
            Vector2 rocketPos = gRocket->getPosition();
            Vector2 rocketScale = { 100.0f, 100.0f };

            Vector2 platPos = gMovingPlatform->getPosition();
            
            bool touchingX = rocketPos.x + rocketScale.x/2 > platPos.x - PLATFORM_SIZE.x/2 &&
                             rocketPos.x - rocketScale.x/2 < platPos.x + PLATFORM_SIZE.x/2;
            
            bool touchingBottom =
                rocketPos.y + rocketScale.y / 2 >= platPos.y - PLATFORM_SIZE.y / 2 - 5.0f &&
                rocketPos.y + rocketScale.y / 2 <= platPos.y + PLATFORM_SIZE.y / 2;


            if (touchingX && touchingBottom) {
                float rotation = gRocket->getAngle();
                if (rotation > 180.0f) rotation -= 360.0f;
                if (rotation < -180.0f) rotation += 360.0f;
                
                gRocket->setAngle(rotation);

                const float ANGLE_TOLERANCE = 15.0f;
                const float HEIGHT_TOLERANCE = 5.0f;

                bool abovePlatform = rocketPos.y + rocketScale.y / 2 <= platPos.y - PLATFORM_SIZE.y / 2 + HEIGHT_TOLERANCE;

                bool upright = rotation >= -ANGLE_TOLERANCE && rotation <= ANGLE_TOLERANCE;

                if (abovePlatform && upright) {
                    gAppStatus = WIN;
                } 
                else if (!upright && abovePlatform) {
                    gAppStatus = FAILED;
                }
            }
            
            Vector2 tilePos = gTiles[i].getPosition();
            Vector2 tileScale = { TILE_DIMENSION, TILE_DIMENSION };

            bool collisionX = rocketPos.x + rocketScale.x/2 > tilePos.x - tileScale.x/2 &&
                              rocketPos.x - rocketScale.x/2 < tilePos.x + tileScale.x/2;
            bool collisionY = rocketPos.y + rocketScale.y/2 > tilePos.y - tileScale.y/2 &&
                              rocketPos.y - rocketScale.y/2 < tilePos.y + tileScale.y/2;

            if (collisionX && collisionY) {
                gAppStatus = FAILED;
                break;
            }
        }

        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    float scaleX = (float)SCREEN_WIDTH / gTextureBackground.width;
    float scaleY = (float)SCREEN_HEIGHT / gTextureBackground.height;
    float scale = fmin(scaleX, scaleY) * 1.7;

    Rectangle dest = {
        SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f,
        gTextureBackground.width * scale,
        gTextureBackground.height * scale
    };
    Vector2 origin = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    DrawTexturePro(gTextureBackground, {0,0,(float)gTextureBackground.width,(float)gTextureBackground.height}, dest, origin, 0, WHITE);


    drawHUD(gRocket);

    gRocket->render();
    gMovingPlatform->render();
    
    Vector2 platPos = gMovingPlatform->getPosition();

    for (int i = 0; i < NUMBER_OF_TILES; i++)
    {
        gTiles[i].render();
    }

    if (gAppStatus == FAILED) {
        DrawText("Mission Failed!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2, 50, RED);
    }

    if (gAppStatus == WIN) {
        DrawText("Mission Success!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2, 50, GREEN);
    }

    EndDrawing();
}

void shutdown() 
{ 
    delete gRocket;
    delete[] gTiles;
    delete gMovingPlatform;
    UnloadTexture(gTextureBackground);

    CloseWindow();
}

int main(void)
{
    initialise();
    while (gAppStatus != TERMINATED)
    {
        processInput();
        if (gAppStatus == RUNNING) {
            update();
        }
        render();
    }
    shutdown();
    return 0;
}

/*
    this is something i used that wasn't covered in class (at least i dont remember it being covered), 
    i used it purely for visuals sake. i felt that it was wrong having a fuel mechanism but not being 
    able to display it anywhere so the player wouldn't be able to keep track of it. i just *had* to add it, 
    some ocd thing i guess. very sorry </3
*/

void drawHUD(Entity* rocket)
{
    int hudX = 20;
    int hudY = 20;
    int lineSpacing = 25;
    char buffer[64];

    snprintf(buffer, sizeof(buffer), "Rotation: %.2f deg", rocket->getAngle());
    DrawText(buffer, hudX, hudY, 20, WHITE);

    Vector2 pos = rocket->getPosition();
    snprintf(buffer, sizeof(buffer), "Position: %.2f, %.2f", pos.x, pos.y);
    DrawText(buffer, hudX, hudY + lineSpacing, 20, WHITE);

    snprintf(buffer, sizeof(buffer), "Fuel: %.2f", gFuel);
    DrawText(buffer, hudX, hudY + lineSpacing*2, 20, WHITE);
}
