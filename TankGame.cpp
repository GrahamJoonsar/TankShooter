#include "raylib.h"
#include <math.h>

const int screenWidth = 1000;
const int screenHeight = 500;
int bulletSpeed = 10;
int enemiesKilled = 0;
int roundNum = 1;
int cash = 0;

float distance(float x1, float y1, float x2, float y2){
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2)); // Distance formula
}

bool mouseInGameSpace(bool playerDead){ // This is what checks if the mouse is in the game space
    if (GetMouseX() > 250 && GetMouseX() < 750 && !playerDead){
        return true;
    } else {
        return false;
    }
}

void roundAdvancer(){ // This advances the round after every 50 enemies
    roundNum = enemiesKilled/50 + 1 - (enemiesKilled % 1);
}   // My record is round 32

// This is the button class for buying upgrades
class Button{
    public:
        float x, y, width, height;
        Color color;
    void draw(){
        DrawRectangle(x, y, width, height, color); // Creating the square of the button
        // Creating the plus on the button
        DrawRectangle(x + width/2.5, y + height/8, width/5, height/1.2, DARKGRAY);
        DrawRectangle(x + width/8, y + height/2.5, width/1.2, height/5, DARKGRAY);
    }
    void mousePressedOnButton(int * valueToBeIncremented, int price, bool playerStatus){
        if (IsMouseButtonPressed(0) && !playerStatus){ // Checks if the mouse is clicked and the player is alive
            if (distance(x + width/2, y + height/2, GetMouseX(), GetMouseY()) < 20){ // checks the mouse distance to the button
                if (cash >= price){ // Checks if the player has enough money to buy the product
                    cash -= price; // takes away the money
                    *valueToBeIncremented += 1; // Increments the value for the button
                } // For some reason using ++ to increment didn't work
            }
        }
    }
};

// Declaring all the buttons
Button bulletButton;
Button movementButton;
Button healthButton;

class Player{
    public:
        float x = 500;
        float y = 250;
        int health = 5;
        int movSpeed = 3;
        bool dead = false;
    void move(){ // Movement WASD keys
        if (mouseInGameSpace(dead)){
            if (IsKeyDown(KEY_W) && y > 40){
                y -= movSpeed;
            }
            if (IsKeyDown(KEY_S) && y < screenHeight - 40){
                y += movSpeed;
            }
            if (IsKeyDown(KEY_A) && x > 250 + 40){
                x -= movSpeed;
            }
            if (IsKeyDown(KEY_D) && x < 750 - 40){
                x += movSpeed;
            }
        }
    }
    void checkHealth(){ // Checks if the player is dead
        if (health <= 0){
            dead = true;
        }
    }
};

// Declaring the player
Player player;

class Bullet{
    public:
        float x, y, xVel, yVel;
    void draw(){
        DrawCircle(x, y, 5, BLACK);
    }
    void move(){
        if (mouseInGameSpace(player.dead)){ // Only moves if the mouse is in the game space
            x += xVel * bulletSpeed; // Moving the bullet by the velocity and movement speed
            y += yVel * bulletSpeed;
        }
    }
};

// Declaring th bullets
Bullet bullet;
Bullet bullet2;
Bullet bullet3;

class Enemy{
    public:
        float x, y, xVel, yVel;
    void checkBullet(){
        if (distance(bullet.x , bullet.y, x, y) < 20 || distance(bullet2.x , bullet2.y, x, y) < 20 || distance(bullet3.x , bullet3.y, x, y) < 20){ // checking the distances to the bullets
            x = GetRandomValue(0, 250); // Resetting the enemy to the left of the screen
            y = GetRandomValue(0, 500);
            enemiesKilled++; // Increment the kill count
            cash += 5; // Add cash
        }
    }
    void draw(){
        DrawCircle(x, y, 20, RED);
    }
    void move(float px, float py){
        if (mouseInGameSpace(player.dead)){ // Checks if the mouse is in the game space
            xVel = cos(atan2(py - y, px - x)); // Moves the x at an angle towards the player
            yVel = sin(atan2(py - y, px - x)); // Moves the y at an angle towards the player
            // The reason its at an angle is so it moves at a constant speed, even at a diagonal
            x += xVel * (floor(roundNum/4) + 1); // Every 4 rounds the speed increases
            y += yVel * (floor(roundNum/4) + 1);
        }
    }
    void checkPlayerCollision(float px, float py){
        if (distance(x, y, px, py) < 20){ // Checks the distance to the player
            player.health--; // Decrements the player health
            x = GetRandomValue(0, 250); // Resets the enemy position
            y = GetRandomValue(0, 500);
        }
    }
};

// Declaring enemies
Enemy enemy;
Enemy enemies[10];
void generateEnemies(){ // Creates 10 enemies
    for (int i = 0; i < 10; i++){
        enemy.x = GetRandomValue(0, 250); // Setting random positions
        enemy.y = GetRandomValue(0, 500);
        enemies[i] = enemy;
    }
}

void drawUI(){
    DrawRectangle(0, 0, 250, 500, GRAY); // Left side
    DrawRectangle(750, 0, 250, 500, GRAY); // Right side
    DrawText(TextFormat("Round: %03i", roundNum), 20, 20, 20, WHITE); // Round number
    DrawText(TextFormat("Enemies Killed: %04i", enemiesKilled), 20, 60, 20, WHITE); // Kill counter
    DrawText(TextFormat("Health: %01i", player.health), 20, 80, 20, GREEN); // Health
    DrawText(TextFormat("$ %05i", cash), 20, 102, 20, WHITE); // Cash counter
    
    // This is the button that upgrades bullet speed
    bulletButton.draw(); 
    // Drawing the label of the button
    DrawText("Upgrade bullet speed:", 760, 22, 19, WHITE); // Button label
    DrawText("Price: $500", 760, 42, 19, WHITE); // Price of the bullet speed upgrade
    // Showing the current bullet speed
    DrawText(TextFormat("Bullet Speed: %02i", bulletSpeed), 760, 62, 19, WHITE);
    
    // Movement button
    movementButton.draw();
    DrawText("Upgrade movement speed:", 760, 82, 19, WHITE);
    DrawText("Price: $250", 760, 102, 19, WHITE);
    DrawText(TextFormat("Movement Speed: %01i", player.movSpeed), 760, 122, 19, WHITE);
    
    // Health button
    DrawText("+1 Health:", 760, 142, 19, WHITE);
    DrawText("Price: $1000", 760, 162, 19, WHITE);
    healthButton.draw();
    
    // Losing screen
    if (player.dead){
        DrawText("You Lose!", 400, 240, 40, WHITE);
    }
}

void drawGameBackground(){
    if (mouseInGameSpace(player.dead)){ // If the mouse is in the game space draws white
        DrawRectangle(250, 0, 500, 500, WHITE); // Middle
    } else { // Draws Gray otherwise
        DrawRectangle(250, 0, 500, 500, Color{200, 200, 200, 255}); // Middle
    }
}

void drawPlayer(){
    DrawCircle(player.x, player.y, 20, GRAY); // Drawing the outer circle
    DrawCircle(player.x, player.y, 12, DARKGRAY); // Drawing the inner circle
    float angle = atan2(GetMouseY() - player.y, GetMouseX() - player.x); // angle from the player to the mouse
    float centerX = 30 * cos(angle); // This is the tip of the gun
    float centerY = 30 * sin(angle);
    // Drawing the gun
    // This is the red tip of the barrel
    DrawPoly((Vector2){centerX + player.x, centerY + player.y}, 4, 15, (angle/M_PI) * 180 + 45, RED);
    // This is the rest of the barrel
    DrawPoly((Vector2){centerX * 0.5 + player.x, centerY * 0.5 + player.y}, 4, 15, (angle/M_PI) * 180 + 45, DARKGRAY);
}

void checkInput(){
    if (IsMouseButtonPressed(0) && mouseInGameSpace(player.dead)){ // Checks if the mouse is in the game area
        // Checks if the 1st bullet is outside of the game area
        if (bullet.x < 250 || bullet.x > 750 || bullet.y < 0 || bullet.y > 500){ // First bullet
            bullet.x = player.x;
            bullet.y = player.y;
            bullet.xVel = cos(atan2(GetMouseY() - player.y, GetMouseX() - player.x));
            bullet.yVel = sin(atan2(GetMouseY() - player.y, GetMouseX() - player.x));
            
        } else if (bullet2.x < 250 || bullet2.x > 750 || bullet2.y < 0 || bullet2.y > 500){ // Second bullet
            bullet2.x = player.x;
            bullet2.y = player.y;
            bullet2.xVel = cos(atan2(GetMouseY() - player.y, GetMouseX() - player.x));
            bullet2.yVel = sin(atan2(GetMouseY() - player.y, GetMouseX() - player.x));
            
        } else if (bullet3.x < 250 || bullet3.x > 750 || bullet3.y < 0 || bullet3.y > 500){ // Third bullet
            bullet3.x = player.x;
            bullet3.y = player.y;
            bullet3.xVel = cos(atan2(GetMouseY() - player.y, GetMouseX() - player.x));
            bullet3.yVel = sin(atan2(GetMouseY() - player.y, GetMouseX() - player.x));
        }
    }
    player.move(); // Moving the player
    
    // Checks if the buttons are being pressed
    bulletButton.mousePressedOnButton(&bulletSpeed, 500, player.dead);
    movementButton.mousePressedOnButton(&player.movSpeed, 250, player.dead);
    healthButton.mousePressedOnButton(&player.health, 1000, player.dead);
}

void drawAndMoveEnemies(){
    // Drawing and moving all 10 enemies
    for (int i = 0; i < 10; i++){
        enemies[i].draw();
        enemies[i].checkBullet();
        enemies[i].move(player.x, player.y);
        enemies[i].checkPlayerCollision(player.x, player.y);
    }
}

int main(void){
    
    generateEnemies(); // Creating the ten enemies
    
    // Initializing the buttons
    // Bullet button
    bulletButton.x = 960;
    bulletButton.y = 48;
    bulletButton.width = bulletButton.height = 20;
    bulletButton.color = LIGHTGRAY;
    // Movement button
    movementButton.x = 960;
    movementButton.y = 105;
    movementButton.width = movementButton.height = 20;
    movementButton.color = LIGHTGRAY;
    // Health button
    healthButton.x = 960;
    healthButton.y = 155;
    healthButton.width = healthButton.height = 20;
    healthButton.color = LIGHTGRAY;
    
    InitWindow(screenWidth, screenHeight, "TankGame"); // Initializing the window

    SetTargetFPS(60);
    
    while (!WindowShouldClose()) // Main game loop
    {
        BeginDrawing();

        ClearBackground(BLACK);
        
        roundAdvancer(); // Checks the enemies killed for advancing the round
        
        checkInput(); // Checks for user input
        
        drawGameBackground(); // Draws the white backgroung in the game space
        
        // Drawing and moving the bullets
        bullet.draw(); // For some reason, putting the bullets in an array did not work
        bullet.move();
        bullet2.draw();
        bullet2.move();
        bullet3.draw();
        bullet3.move();
        
        drawPlayer(); // Drawing the player and checking the health // I probably need to make this a function of the player class
        player.checkHealth(); // Checking if the player is dead
        
        drawAndMoveEnemies(); // Drawing and updating the Enemies
        
        drawUI(); // Draws the UI elements

        EndDrawing();
    }

    CloseWindow();
} // Exactly 300 lines