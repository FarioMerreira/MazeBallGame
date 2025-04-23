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
    float dir[3];
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
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Set global ambient light to black (pitch black scene)
    GLfloat global_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // Initialize four white lights (but keep them off initially)
    lights = {
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, false, 0.0f, 10.0f, false}
    };
}

void drawTextGame(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawMaze() {
    // Mirror-like wall material properties
    GLfloat wall_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};  // No ambient reflection
    GLfloat wall_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};  // High diffuse reflection (white)
    GLfloat wall_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Maximum specular reflection (white)
    GLfloat wall_shininess[] = {128.0f};                // Maximum shininess for mirror effect

    // Apply the material properties to the walls
    glMaterialfv(GL_FRONT, GL_AMBIENT, wall_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, wall_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, wall_shininess);

    // Draw maze walls as stretched cubes (rectangular prisms)
    for (auto &wall : maze_walls) {
        glPushMatrix();
        glTranslatef(wall.first, wall.second + 2.0f, 0.0f); // Center the wall vertically
        glScalef(1.0f, 4.0f, 1.0f); // Stretch the cube into a vertical wall
        glutSolidCube(1.0f);        // Draw a solid cube (stretched into a wall)
        glPopMatrix();
    }
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
            // Enable the light
            glEnable(GL_LIGHT0 + i);

            // Set white light properties
            GLfloat light_position[] = {lights[i].pos[0], lights[i].pos[1], 0.0f, 1.0f}; // Position of the light
            GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White diffuse light
            GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White specular light
            GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f}; // No ambient light

            glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular);
            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient);

            // Draw a wireframe sphere around the light
            glPushMatrix();
            glTranslatef(lights[i].pos[0], lights[i].pos[1], 0.0f); // Position the sphere at the light's position
            glColor3f(1.0f, 1.0f, 1.0f); // Set the sphere color to white
            glutWireSphere(0.3f, 16, 16); // Draw a wireframe sphere with radius 0.3
            glPopMatrix();
        } else {
            glDisable(GL_LIGHT0 + i); // Disable the light if not thrown
        }
    }
}

void gameDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up top-down view
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (zoom_out) {
        gluOrtho2D(-5, 20, -5, 20);
    } else {
        gluOrtho2D(player.pos[0] - 5, player.pos[0] + 5, 
                   player.pos[1] - 5, player.pos[1] + 5);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    drawMaze();
    drawPlayer();
    drawLights();
    /*
    When the game starts, the player's ball is copied from the start screen's ball (with green and blue lights except a red light) and the walls are as green as how the ball in the start screen is. The game only allows the player to throw one white light.
    */
    if (game_over) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextGame(player.pos[0] - 2.0f, player.pos[1] + 2.0f, "YOU WIN! Maze Complete");
        glEnable(GL_LIGHTING);
    }
    
    glutSwapBuffers();
}

void throwLight() {
    if (player.remaining_lights <= -1) return;

    for (int i = 0; i < lights.size(); i++) {
        if (lights[i].active && !lights[i].thrown) {
            lights[i].thrown = true;
            lights[i].pos[0] = player.pos[0]; // Start at the player's position
            lights[i].pos[1] = player.pos[1];
            lights[i].dir[0] = light_direction[0]; // Set the direction
            lights[i].dir[1] = light_direction[1];
            lights[i].distance = 0.0f;
            lights[i].hit_wall = false;
            player.remaining_lights--;

            printf("White light is being thrown!\n");
            break;
        }
    }
}

void updateLights() {
    for (auto &light : lights) {
        if (light.thrown && !light.hit_wall) {
            // Move the light
            light.pos[0] += light.dir[0] * 0.05f; // Slower movement
            light.pos[1] += light.dir[1] * 0.05f;
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
        
        if (sqrt(pow(player.pos[0] - end_point.first, 2) + pow(player.pos[1] - end_point.second, 2) < 1.0f)) {
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
