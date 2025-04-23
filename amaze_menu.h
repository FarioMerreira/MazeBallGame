#ifndef AMAZE_MENU_H
#define AMAZE_MENU_H

#include <GL/glut.h>
#include <string>

extern bool game_start_menu;
extern int red, green, blue;
extern float spin_menu;

void initMenuLights();
void drawMenu();
void drawScene(bool showMenu);
void menuKeyboard(unsigned char key, int x, int y);
void updateMenu(int value);
void menuReshape(int w, int h);
void menuDisplay();
void switchToGameMode();
void gameDisplay();
void gameKeyboard(unsigned char key, int x, int y);

#endif
