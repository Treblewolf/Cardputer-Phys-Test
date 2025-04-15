/**
 * Basic 2D Physics Engine for M5Stack Cardputer with Sprites
 *
 * Features:
 * - Objects represented by graphical sprites (defined as C arrays).
 * - Gravity simulation, Euler integration.
 * - AABB collision detection based on sprite dimensions.
 * - Simple collision resolution.
 * - Keyboard input: Spacebar to jump, A/D to move.
 * - Rendering uses sprite buffering with canvas.pushImage().
 * - DeltaTime calculation, FPS display.
 */

#include <M5Cardputer.h>

// --- Configuration ---
const int MAX_OBJECTS = 10;
const float GRAVITY = 9.8f * 30;
const float MAX_DELTA_TIME = 0.05f;
const float MOVE_FORCE = 350.0f;
const float JUMP_VELOCITY = -200.0f;
const float HORIZONTAL_DAMPING = 0.90f;

// --- Simple Sprite Data (16-bit RGB565 color) ---
// Example: 16x16 White Stickman with Black Outline
const uint16_t sprite_player_data[15 * 20] = {
  0,0,0,0,0,0,WHITE,WHITE,WHITE,0,0,0,0,0,0,
  0,0,0,0,0,0,WHITE,WHITE,WHITE,0,0,0,0,0,0,
  0,0,0,0,0,0,WHITE,WHITE,WHITE,0,0,0,0,0,0,
  0,0,0,0,0,0,WHITE,WHITE,WHITE,0,0,0,0,0,0,
  0,0,0,0,0,0,0,WHITE,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,WHITE,0,0,0,0,0,0,0,
  0,0,0,0,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,0,0,0,0,
  0,0,0,0,0,0,0,WHITE,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,WHITE,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,WHITE,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,WHITE,0,0,0,0,0,0,0,
  0,0,0,0,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
  0,0,0,0,WHITE,0,0,0,0,0,WHITE,0,0,0,0,
};
const int SPRITE_PLAYER_WIDTH = 15;
const int SPRITE_PLAYER_HEIGHT = 20;

// Example: 20x20 Red Circle with Black Outline
const uint16_t sprite_enemy1_data[20 * 20] = {
  0,0,0,0,0,0,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,0,0,0,0,0,
  0,0,0,BLACK,BLACK,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,BLACK,BLACK,0,0,0,
  0,0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,0,
  0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,
  0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,
  0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,
  0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,
  0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,
  0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,
  0,0,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,0,0,
  0,0,0,BLACK,BLACK,BLACK,RED,RED,RED,RED,RED,RED,RED,RED,BLACK,BLACK,BLACK,0,0,0,
  0,0,0,0,0,0,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,0,0,0,0,0,
};
const int SPRITE_ENEMY_WIDTH = 20;
const int SPRITE_ENEMY_HEIGHT = 20;

// Example: 20x20 Blue Circle (approximate) with Black Outline
const uint16_t sprite_enemy2_data[20 * 20] = {
  0,0,0,0,0,0,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,0,0,0,0,0,
  0,0,0,BLACK,BLACK,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,BLACK,BLACK,0,0,0,
  0,0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,0,
  0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,
  0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,
  0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,
  0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,
  0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,
  0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,
  0,0,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,0,0,
  0,0,0,BLACK,BLACK,BLACK,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLUE,BLACK,BLACK,BLACK,0,0,0,
  0,0,0,0,0,0,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,0,0,0,0,0,0,
};


// --- Helper Structs ---
struct Vec2D { float x = 0.0f; float y = 0.0f; };

// Include sprite info in PhysicsObject
struct PhysicsObject {
    Vec2D position; Vec2D velocity; Vec2D acceleration;
    // Width/Height now refer to the sprite dimensions for drawing & collision AABB
    float width = 16.0f;
    float height = 16.0f;
    float mass = 1.0f;
    float restitution = 0.5f;
    bool isStatic = false;
    const uint16_t* spriteData = nullptr; // Pointer to the sprite pixel data array
};

// --- Global Variables ---
PhysicsObject objects[MAX_OBJECTS];
int objectCount = 0;
float screenWidth = 0, screenHeight = 0;
unsigned long lastUpdateTime = 0;
float deltaTime = 0.0f;
M5Canvas canvas(&M5Cardputer.Display); // Sprite buffer

// --- Function Declarations ---
void updatePhysics();
bool checkAABBCollision(const PhysicsObject& a, const PhysicsObject& b);
void resolveCollision(PhysicsObject& a, PhysicsObject& b);
void checkAndResolveCollisions();
void render();
void handleInput();

// --- Cardputer Setup Function ---
void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);

    screenWidth = M5Cardputer.Display.width();
    screenHeight = M5Cardputer.Display.height();
    canvas.createSprite(screenWidth, screenHeight); // Create sprite buffer

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setBrightness(50);

    // --- Initialize Physics Objects with Sprites ---
    // Controllable Stickman (Player)
    objects[objectCount++] = {
        {50, 10}, {80, 0}, {0, 0},                  // Pos, Vel, Accel
        SPRITE_PLAYER_WIDTH, SPRITE_PLAYER_HEIGHT, // Width, Height (from sprite)
        1.0f, 0.7f,                                // Mass, Restitution
        false,                                     // isStatic
        sprite_player_data                         // Pointer to sprite data
    };
    // Box 1 (Enemy)
    objects[objectCount++] = {
        {150, 75}, {-50, 20}, {0, 0},               // Pos, Vel, Accel
        SPRITE_ENEMY_WIDTH, SPRITE_ENEMY_HEIGHT,   // Width, Height (from sprite)
        2.0f, 0.6f,                                // Mass, Restitution
        false,                                     // isStatic
        sprite_enemy1_data                          // Pointer to sprite data
    };
    // Box 2 (Enemy)
    objects[objectCount++] = {
        {125, 25}, {0, 20}, {0, 0},               // Pos, Vel, Accel
        SPRITE_ENEMY_WIDTH, SPRITE_ENEMY_HEIGHT,   // Width, Height (from sprite)
        2.0f, 0.9f,                                // Mass, Restitution
        false,                                     // isStatic
        sprite_enemy2_data                          // Pointer to sprite data
    };
    // Floor (Represented by a colored rectangle for simplicity)
    objects[objectCount++] = {
        {0, screenHeight - 20}, {0, 0}, {0, 0},
        screenWidth, 20,                           // Width, Height
        10000.0f, 0.6f,
        true,                                      // isStatic
        nullptr                                    // No sprite data needed (will be drawn differently)
    };
    // Platform (A colored rectangle)
    objects[objectCount++] = {
        {100, 50}, {0, 0}, {0, 0},
        100, 5,                                    // Width, Height
        10000.0f, 0.6f,
        true,                                      // isStatic
        nullptr                                    // No sprite data needed (will be drawn differently)
    };
    // Ceiling (Also a colored rectangle)
     objects[objectCount++] = {
        {0, 0}, {0, 0}, {0, 0},
        screenWidth, 5,                            // Width, Height
        10000.0f, 0.6f,
        true,                                      // isStatic
        nullptr                                    // No sprite data needed
    };


    lastUpdateTime = 0;
    // Display init message
    M5Cardputer.Display.fillScreen(BLACK);
    M5Cardputer.Display.setCursor(5,5);
    M5Cardputer.Display.setTextColor(YELLOW, BLACK);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.println("Phys_Test by Deemuh");
    M5Cardputer.Display.println("Initializing Physics Engine...");
    M5Cardputer.Display.println("Screen: " + String(screenWidth) + "x" + String(screenHeight));
    M5Cardputer.Display.println("Sprite buffer created.");
    M5Cardputer.Display.println("Init complete. Sprites loaded.");
    delay(1000);
}

// --- Cardputer Loop Function ---
void loop() {
    M5Cardputer.update();
    unsigned long currentTime = millis();
    if (lastUpdateTime == 0) { lastUpdateTime = currentTime; render(); return; }
    deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
    lastUpdateTime = currentTime;
    if (deltaTime > MAX_DELTA_TIME) deltaTime = MAX_DELTA_TIME;
    handleInput();
    updatePhysics();
    checkAndResolveCollisions();
    render();
}

// --- Physics Update Function ---
// (Uses object width/height for boundary checks)
void updatePhysics() {
    for (int i = 0; i < objectCount; ++i) {
        if (objects[i].isStatic) continue;
        objects[i].acceleration.y += GRAVITY;
        objects[i].velocity.x += objects[i].acceleration.x * deltaTime;
        objects[i].velocity.y += objects[i].acceleration.y * deltaTime;
        objects[i].position.x += objects[i].velocity.x * deltaTime;
        objects[i].position.y += objects[i].velocity.y * deltaTime;
        objects[i].acceleration = {0, 0};
        if (objects[i].position.x < 0) { objects[i].position.x = 0; objects[i].velocity.x *= -objects[i].restitution; }
        else if (objects[i].position.x + objects[i].width > screenWidth) { objects[i].position.x = screenWidth - objects[i].width; objects[i].velocity.x *= -objects[i].restitution; }
    }
}

// --- Collision Detection Function (AABB vs AABB) ---
// (Uses object width/height for collision box)
bool checkAABBCollision(const PhysicsObject& a, const PhysicsObject& b) {
    bool cX = a.position.x < b.position.x + b.width && a.position.x + a.width > b.position.x;
    bool cY = a.position.y < b.position.y + b.height && a.position.y + a.height > b.position.y;
    return cX && cY;
}

// --- Collision Resolution Function (Simplified) ---
void resolveCollision(PhysicsObject& a, PhysicsObject& b) {
    if (a.isStatic && b.isStatic) return;
    float cAx = a.position.x + a.width / 2.0f, cAy = a.position.y + a.height / 2.0f;
    float cBx = b.position.x + b.width / 2.0f, cBy = b.position.y + b.height / 2.0f;
    float ovX = (a.width / 2.0f + b.width / 2.0f) - abs(cAx - cBx);
    float ovY = (a.height / 2.0f + b.height / 2.0f) - abs(cAy - cBy);
    if (ovX < ovY) { float s = (cAx < cBx) ? -1.0f : 1.0f; if (a.isStatic) { b.position.x -= ovX * s; b.velocity.x *= -b.restitution; } else if (b.isStatic) { a.position.x += ovX * s; a.velocity.x *= -a.restitution; } else { a.position.x += ovX / 2.0f * s; b.position.x -= ovX / 2.0f * s; float r = (a.restitution + b.restitution) / 2.0f; float t = a.velocity.x; a.velocity.x = b.velocity.x * r; b.velocity.x = t * r; } }
    else { float s = (cAy < cBy) ? -1.0f : 1.0f; if (a.isStatic) { b.position.y -= ovY * s; b.velocity.y *= -b.restitution; b.velocity.x *= 0.9f; } else if (b.isStatic) { a.position.y += ovY * s; a.velocity.y *= -a.restitution; a.velocity.x *= 0.9f; } else { a.position.y += ovY / 2.0f * s; b.position.y -= ovY / 2.0f * s; float r = (a.restitution + b.restitution) / 2.0f; float t = a.velocity.y; a.velocity.y = b.velocity.y * r; b.velocity.y = t * r; } }
}

// --- Function to Check All Object Pairs for Collisions ---
void checkAndResolveCollisions() {
    const int iterations = 2;
    for (int iter = 0; iter < iterations; ++iter) {
        for (int i = 0; i < objectCount; ++i) {
            for (int j = i + 1; j < objectCount; ++j) {
                if (checkAABBCollision(objects[i], objects[j])) { resolveCollision(objects[i], objects[j]); }
            }
        }
    }
}

// --- Input Handling Function ---
void handleInput() {
    int controlledObjectIndex = -1;
    for(int i = 0; i < objectCount; ++i) { if (!objects[i].isStatic) { controlledObjectIndex = i; break; } }
    if (controlledObjectIndex != -1) {
        PhysicsObject& controlledObject = objects[controlledObjectIndex];
        if (M5Cardputer.Keyboard.isKeyPressed(' ')) { if (abs(controlledObject.velocity.y) < 10.0f) { controlledObject.velocity.y = JUMP_VELOCITY; } }
        bool movingHorizontally = false;
        if (M5Cardputer.Keyboard.isKeyPressed('a')) { controlledObject.acceleration.x -= MOVE_FORCE; movingHorizontally = true; }
        if (M5Cardputer.Keyboard.isKeyPressed('d')) { controlledObject.acceleration.x += MOVE_FORCE; movingHorizontally = true; }
        if (!movingHorizontally && abs(controlledObject.velocity.x) > 0.1f) { controlledObject.velocity.x *= HORIZONTAL_DAMPING; }
        if (!movingHorizontally && abs(controlledObject.velocity.x) <= 0.1f) { controlledObject.velocity.x = 0; }
    }
}


// --- Rendering Function ---
void render() {
    // Clear the sprite buffer
    canvas.fillSprite(BLACK);

    // Draw all physics objects onto the sprite
    for (int i = 0; i < objectCount; ++i) {
        // Check if the object has sprite data assigned
        if (objects[i].spriteData != nullptr) {
            // Draw the sprite using pushImage
            canvas.pushImage(
                (int)objects[i].position.x, (int)objects[i].position.y, // Position (top-left)
                (int)objects[i].width, (int)objects[i].height,           // Sprite dimensions
                objects[i].spriteData                                    // Pointer to pixel data
            );
        } else if (objects[i].isStatic) {
            // Special case for static objects without sprites (Floor/Ceiling)
            // Draw them as simple rectangles using placeholder colors
            uint16_t staticColor = (objects[i].position.y == 0) ? DARKGREY : YELLOW; // Ceiling or Floor color
             canvas.fillRect(
                (int)objects[i].position.x, (int)objects[i].position.y,
                (int)objects[i].width, (int)objects[i].height,
                staticColor
            );
        }
        // Add else if needed for other non-sprite dynamic objects
    }

    // Draw FPS counter and instructions onto the sprite
    canvas.setCursor(5, 5);
    canvas.setTextColor(WHITE, BLACK);
    canvas.setTextSize(1);
    if (deltaTime > 0) { canvas.printf("FPS: %.1f", 1.0f / deltaTime); }
    else { canvas.print("FPS: ---"); }
    canvas.setCursor(5, 15);
    canvas.setTextColor(DARKGREY, BLACK);
    canvas.print("Move: A/D");
    canvas.setCursor(5, 25);
    canvas.setTextColor(DARKGREY, BLACK);
    canvas.print("Jump: Space");

    // Push the completed sprite buffer to the actual screen
    canvas.pushSprite(0, 0);
}
