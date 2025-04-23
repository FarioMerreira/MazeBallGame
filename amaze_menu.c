#include <GL/glut.h>
#include "amaze_menu.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <cmath>
#include <string>

bool game_start_menu = false;
int red = 1, green = 1, blue = 1; 
float spin_menu = 0.0f;

void initMenuLights(void)
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
    GLfloat mat_shininess[] = { 50.0 };               

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    //red
    GLfloat light0_ambient[] = { 0.0, 0.0, 0.0, 1.0 };  
    GLfloat light0_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };  
    GLfloat light0_specular[] = { 1.0, 0.0, 0.0, 1.0 }; 
    GLfloat light0_position[] = { 0.0, 1.5, 0.0, 1.0 }; 
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    //green
    GLfloat light1_ambient[] = { 0.0, 0.0, 0.0, 1.0 };  
    GLfloat light1_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };  
    GLfloat light1_specular[] = { 0.0, 1.0, 0.0, 1.0 }; 
    GLfloat light1_position[] = { 1.5, 0.0, 0.0, 1.0 }; 
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    
    //blue
    GLfloat light2_ambient[] = { 0.0, 0.0, 0.0, 1.0 };  
    GLfloat light2_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };  
    GLfloat light2_specular[] = { 0.0, 0.0, 1.0, 1.0 }; 
    GLfloat light2_position[] = { -1.5, 0.0, 0.0, 1.0 }; 

    glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_DEPTH_TEST);
}

void drawText(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawMenu() {
    // Switch to 2D orthographic projection for text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable lighting for text
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Draw menu text
    glColor3f(1, 1, 1);
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    // Draw title
    drawText(windowWidth/2 - 75, windowHeight - 600, "MAZE BALL GAME");
    
    drawText(windowWidth/2 - 50, windowHeight - 750, "CONTROLS:");
    drawText(windowWidth/2 - 90, windowHeight - 775, "WASD  -   Move Player");
    drawText(windowWidth/2 - 90, windowHeight - 800, "<^v>     - Aim Light");
    drawText(windowWidth/2 - 90, windowHeight - 825, "SPACE - Throw Light");
    
    
    // Draw instructions
    drawText(windowWidth/2 - 110, windowHeight - 975, "Press SPACE to start game");
    drawText(windowWidth/2 - 75, windowHeight - 1000, "Press ESC to exit");
    
    // Restore state
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawScene(bool showMenu) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Update light positions based on spin_menu
    GLfloat light0_position[] = { 0.0f, 1.5f * static_cast<GLfloat>(cos(spin_menu)), 1.5f * static_cast<GLfloat>(sin(spin_menu)), 0.0f };
    GLfloat light1_position[] = { 1.5f * static_cast<GLfloat>(cos(spin_menu)), 0.0f, 1.5f * static_cast<GLfloat>(sin(spin_menu)), 0.0f };
    GLfloat light2_position[] = { 1.5f * static_cast<GLfloat>(sin(spin_menu)), 1.5f * static_cast<GLfloat>(cos(spin_menu)), 0.0f, 0.0f };

    // Set light positions
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position); 

    // Draw lights
    // Red light
    glPushMatrix();
    glRotated(spin_menu, 1.0, 0.0, 0.0);
    glTranslated(0.0, 1.5, 0.0);
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 0.0, 0.0);
    //glutWireSphere(0.1, 20, 10);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Green light
    glPushMatrix();
    glRotated(spin_menu, 0.0, 1.0, 0.0);
    glTranslated(1.5, 0.0, 0.0);
    glDisable(GL_LIGHTING);
    glColor3f(0.0, 1.0, 0.0);
    //glutWireSphere(0.1, 20, 10);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Blue light
    glPushMatrix();
    glRotated(spin_menu, 0.0, 0.0, 1.0);
    glTranslated(-1.5, 0.0, 0.0);
    glDisable(GL_LIGHTING);
    glColor3f(0.0, 0.0, 1.0);
    //glutWireSphere(0.1, 20, 10);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    
    // Main sphere
    glutSolidSphere(1.0, 50, 50);
    
    glPopMatrix();
    
    // Draw menu if requested
    if (showMenu) {
        drawMenu();
    }
}

void menuDisplay(void)
{
    drawScene(!game_start_menu); // Show menu when game is not started
    glutSwapBuffers();
}

void menuReshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void menuKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ' ': // SPACE key
        game_start_menu = true;
        switchToGameMode();
        return;
        
    case 'r':
        red = !red;
        if (red)
            glEnable(GL_LIGHT0);
        else
            glDisable(GL_LIGHT0);
        break;
        
    case 'g': 
        green = !green;
        if (green)
            glEnable(GL_LIGHT1);
        else
            glDisable(GL_LIGHT1);
        break;
        
    case 'b': 
        blue = !blue;
        if (blue)
            glEnable(GL_LIGHT2);
        else
            glDisable(GL_LIGHT2);
        break;
        
    case 27: // ESC key
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void updateMenu(int value) {
    spin_menu += 0.1f;
    if(spin_menu > 360.0f) spin_menu -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, updateMenu, 0); // ~60 FPS
}


