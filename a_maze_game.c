#include <GL/glut.h>
#include "amaze_menu.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

// Game states (make these extern in amaze_menu.h)
bool game_start = false;
bool game_over = false;
bool zoom_out = false;

// Player and light states
struct Light {
    bool active;
    float pos[3];
    float cor[3];
    float dre[2];
    bool thrown;
    float distance;
    float max_distance;
    bool hit_wall;
};

struct Player {
    float pos[2];
    int remaining_lights;
};

Player player = {{0.0f, 0.0f}, 3};
std::vector<Light> lights;
std::vector<std::pair<float, float>> maze_walls;
std::pair<float, float> end_point = {15.0f, 15.0f};

// Camera and view
float camera_distance = 5.0f;
float spin = 0.0f;
float light_direction[2] = {0.0f, 1.0f}; // Default direction is "up"
float camera_angle = 45.0f; // Angle of the camera looking down
float camera_height = 10.0f; // Height of the camera above the ground

void initMaze() {
    // Simple maze walls
    maze_walls = {
        {2.0f, 0.0f}, {2.0f, 4.0f}, 
        {5.0f, 3.0f}, {5.0f, 7.0f},
        {8.0f, 0.0f}, {8.0f, 4.0f},
        {11.0f, 5.0f}, {11.0f, 9.0f},
        {14.0f, 2.0f}, {14.0f, 6.0f}
    };
}

void initLights() {
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    GLfloat global_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // Initialize four lights with different colors in fixed order
    lights = {
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Red light (first)
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Green light (second)
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Blue light (third)
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}  // Yellow light (fourth)
    };
}

void drawTextGame(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawMaze() {
    for (auto &wall : maze_walls) {
        // Default wall color (black if not illuminated)
        GLfloat wall_color[] = {0.0f, 0.0f, 0.0f, 1.0f};

        // Check if any light illuminates this wall
        for (auto &light : lights) {
            if (light.thrown && !light.hit_wall) {
                // Set wall color to the color of the light
                wall_color[0] = light.cor[0]; // Red component
                wall_color[1] = light.cor[1]; // Green component
                wall_color[2] = light.cor[2]; // Blue component
                break;
            }
        }

        // Apply the wall's material properties
        glMaterialfv(GL_FRONT, GL_AMBIENT, wall_color);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_color);
        glMaterialfv(GL_FRONT, GL_SPECULAR, wall_color);

        // Declare shininess as a variable
        GLfloat shininess[] = {50.0f};
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

        // Draw the wall as a stretched cube
        glPushMatrix();
        glTranslatef(wall.first, wall.second + 2.0f, 0.0f); // Center the wall vertically
        glScalef(1.0f, 4.0f, 3.0f); // Stretch the cube into a vertical wall
        glutSolidCube(1.0f);        // Draw a solid cube (stretched into a wall)
        glPopMatrix();
    }
}

void drawEndPoint() {
    // Material properties for the endpoint
    GLfloat end_ambient[] = {0.0f, 0.5f, 0.0f, 1.0f};  // Green ambient reflection
    GLfloat end_diffuse[] = {0.0f, 1.0f, 0.0f, 1.0f};  // Bright green diffuse reflection
    GLfloat end_specular[] = {0.5f, 1.0f, 0.5f, 1.0f}; // Green specular reflection
    GLfloat end_shininess[] = {50.0f};                 // Moderate shininess

    // Apply the material properties to the endpoint
    glMaterialfv(GL_FRONT, GL_AMBIENT, end_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, end_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, end_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, end_shininess);

    // Draw the endpoint as a sphere
    glPushMatrix();
    glTranslatef(end_point.first, end_point.second, 0.5f); // Position the endpoint slightly above the ground
    glutSolidSphere(0.5f, 20, 20); // Draw a solid sphere
    glPopMatrix();
}

void drawPlayer() {
    // Mirror-like sphere material properties
    GLfloat sphere_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};  // No ambient reflection
    GLfloat sphere_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};  // High diffuse reflection (white)
    GLfloat sphere_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Maximum specular reflection (white)
    GLfloat sphere_shininess[] = {128.0f};                // Maximum shininess for mirror effect

    // Apply the material properties to the sphere
    glMaterialfv(GL_FRONT, GL_AMBIENT, sphere_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sphere_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, sphere_shininess);

    glPushMatrix();
    glTranslatef(player.pos[0], player.pos[1], 0.0f);
    glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();
}

void drawLights() {
    for (int i = 0; i < lights.size(); i++) {
        if (!lights[i].active) continue;

        if (lights[i].thrown) {
            glEnable(GL_LIGHT0 + i);

            // Position the light (keep z at 0.0 for ground level)
            GLfloat light_position[] = {lights[i].pos[0], lights[i].pos[1], 0.0f, 1.0f};
            
            // Use the pre-set color from initialization (don't modify it)
            GLfloat light_diffuse[] = {lights[i].cor[0], lights[i].cor[1], lights[i].cor[2], 1.0f};
            GLfloat light_specular[] = {lights[i].cor[0], lights[i].cor[1], lights[i].cor[2], 1.0f};
            GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

            glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular);
            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient);

            // Draw the wireframe sphere with the correct color
            glPushMatrix();
            glTranslatef(lights[i].pos[0], lights[i].pos[1], 0.0f);
            glColor3f(lights[i].cor[0], lights[i].cor[1], lights[i].cor[2]);
            glutWireSphere(0.3f, 16, 16);
            glPopMatrix();
        } else {
            glDisable(GL_LIGHT0 + i);
        }
    }
}

void gameDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up perspective view
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Position the camera at an angle

    if (zoom_out) {
        // Zoomed out view shows whole maze
        gluLookAt(7.5, 7.5, camera_height,  // Camera position (center of maze, from above)
                  7.5, 7.5, 0.0,            // Looking at center of maze
                  0.0, 1.0, 0.0);           // Up vector
    } else {
        // Normal view follows player
        gluLookAt(player.pos[0] - 3.0, player.pos[1] - 3.0, camera_height, // Camera position (offset from player)
                  player.pos[0], player.pos[1], 0.0,                       // Looking at player
                  0.0, 0.0, 1.0);                                         // Up vector
    }
    
    drawMaze();
    drawEndPoint();
    drawPlayer();
    drawLights();
    
    if (game_over) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        // Position text in 3D space
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        drawTextGame(glutGet(GLUT_WINDOW_WIDTH)/2 - 100, glutGet(GLUT_WINDOW_HEIGHT)/2, "YOU WIN! Maze Complete");
        glEnable(GL_LIGHTING);
    }
    
    glutSwapBuffers();
}

void throwLight() {
    if (player.remaining_lights <= -1) return;

    // Find the next available light in order
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i].active && !lights[i].thrown) {
            lights[i].thrown = true;
            lights[i].pos[0] = player.pos[0];
            lights[i].pos[1] = player.pos[1];
            
            // Set direction based on current light_direction
            lights[i].dre[0] = light_direction[0];
            lights[i].dre[1] = light_direction[1];
            /*lights[i].dir[0] = light_direction[0];
            lights[i].dir[1] = light_direction[1];*/
            
            // Keep the original color (don't modify lights[i].dir[2] as it contains the color info)
            lights[i].distance = 0.0f;
            lights[i].hit_wall = false;
            player.remaining_lights--;

            printf("Throwing %s light in direction (%f, %f)\n", 
                  (i == 0) ? "Red" : 
                  (i == 1) ? "Green" : 
                  (i == 2) ? "Blue" : "Yellow", 
                  light_direction[0], light_direction[1]);
            break;
        }
    }
}

void updateLights() {
    for (auto &light : lights) {
        if (light.thrown && !light.hit_wall) {
            // Move the light
            light.pos[0] += light.dre[0] * 0.05f; // Slower movement
            light.pos[1] += light.dre[1] * 0.05f;
            light.distance += 0.05f;

            // Check for collisions with walls
            for (auto &wall : maze_walls) {
                if (fabs(light.pos[0] - wall.first) < 0.3f && 
                    light.pos[1] >= wall.second && 
                    light.pos[1] <= wall.second + 4.2f) {
                    light.hit_wall = true;
                    break;
                }
            }

            // Stop the light if it reaches its maximum distance
            if (light.distance >= light.max_distance) {
                light.hit_wall = true;
            }
        }
    }
}

void update(int value) {
    if (game_start && !game_over) {
        spin += 0.1f;
        if (spin > 360.0f) spin -= 360.0f;
        
        updateLights();
        
        // Check if the player has reached the endpoint
        if (sqrt(pow(player.pos[0] - end_point.first, 2) + pow(player.pos[1] - end_point.second, 2)) < 1.0f) {
            game_over = true;
            zoom_out = true;
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void gameKeyboard(unsigned char key, int x, int y) {
    float move_speed = 0.2f;
    float new_x = player.pos[0];
    float new_y = player.pos[1];

    switch (key) {
        case ' ':
            if (!game_over) throwLight();
            break;
        case 'w': // Move up
            if (!game_over) new_y += move_speed;
            light_direction[0] = 0.0f;
            light_direction[1] = 1.0f;
            break;
        case 's': // Move down
            if (!game_over) new_y -= move_speed;
            light_direction[0] = 0.0f;
            light_direction[1] = -1.0f;
            break;
        case 'a': // Move left
            if (!game_over) new_x -= move_speed;
            light_direction[0] = -1.0f;
            light_direction[1] = 0.0f;
            break;
        case 'd': // Move right
            if (!game_over) new_x += move_speed;
            light_direction[0] = 1.0f;
            light_direction[1] = 0.0f;
            break;
        case 27: // Exit on ESC
            exit(0);
            break;
    }

    // Collision detection
    bool can_move = true;
    for (auto &wall : maze_walls) {
        if (fabs(new_x - wall.first) < 0.7f && new_y >= wall.second - 0.7f && new_y <= wall.second + 4.7f) {
            can_move = false;
            break;
        }
    }

    if (can_move) {
        player.pos[0] = new_x;
        player.pos[1] = new_y;
    }

    glutPostRedisplay();
}

void gameSpecialKeyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: // Light direction: up
            light_direction[0] = 0.0f;
            light_direction[1] = 1.0f;
            printf("Light direction set to UP\n");
            break;
        case GLUT_KEY_DOWN: // Light direction: down
            light_direction[0] = 0.0f;
            light_direction[1] = -1.0f;
            printf("Light direction set to DOWN\n");
            break;
        case GLUT_KEY_LEFT: // Light direction: left
            light_direction[0] = -1.0f;
            light_direction[1] = 0.0f;
            printf("Light direction set to LEFT\n");
            break;
        case GLUT_KEY_RIGHT: // Light direction: right
            light_direction[0] = 1.0f;
            light_direction[1] = 0.0f;
            printf("Light direction set to RIGHT\n");
            break;
    }
}

void switchToGameMode() {
    // Initialize game components
    initLights();
    initMaze();
    
    // Reset game state
    player = {{0.0f, 0.0f}, 3};
    game_over = false;
    zoom_out = false;
    
    // Switch to game callbacks
    glutDisplayFunc([]() {
        if (game_start) {
            gameDisplay();
        } else {
            menuDisplay();
        }
    });
    glutKeyboardFunc(gameKeyboard);
    glutSpecialFunc(gameSpecialKeyboard); // Register special keyboard function
    glutTimerFunc(0, update, 0);
    
    game_start = true;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 1200);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Maze Ball Game");
    
    initMenuLights();
    
    // Start with menu
    glutDisplayFunc(menuDisplay);
    glutReshapeFunc(menuReshape);
    glutKeyboardFunc(menuKeyboard);
    glutTimerFunc(0, updateMenu, 0);
    
    glutMainLoop();
    return 0;
}
