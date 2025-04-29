//Codigo referente ao jogo

#include <GL/glut.h>
#include "amaze_menu.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

//Estados de jogo
bool game_start = false;
bool game_over = false;
bool zoom_out = false;

//Estruturas de luz e jogador
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

Player player = {{1.0f, 1.0f}, 3};
std::vector<Light> lights;
std::vector<std::pair<float, float>> maze_walls;
std::pair<float, float> end_point = {15.0f, 15.0f};

//Camera isometrica
float camera_distance = 5.0f;
float spin = 0.0f;
float light_direction[2] = {0.0f, 1.0f};
float camera_angle = 45.0f;
float camera_height = 10.0f;

//Iniciando parametros do labirinto
void initMaze() {
    maze_walls = {
        // Bordas externas
        {0.0f, 0.0f}, {15.0f, 0.0f},
        {0.0f, 15.0f}, {15.0f, 15.0f},
        {0.0f, 0.0f}, {0.0f, 15.0f},
        {15.0f, 0.0f}, {15.0f, 15.0f},

        // Paredes internas baseadas no desenho:

        // Região superior
        {1.0f, 0.0f}, {1.0f, 3.0f},
        {3.0f, 0.0f}, {3.0f, 1.0f},
        {4.0f, 1.0f}, {7.0f, 1.0f},
        {6.0f, 0.0f}, {6.0f, 1.0f},
        {8.0f, 0.0f}, {8.0f, 4.0f},
        {10.0f, 2.0f}, {14.0f, 2.0f},
        {12.0f, 0.0f}, {12.0f, 2.0f},

        // Região central
        {3.0f, 5.0f}, {2.0f, 7.0f},
        {4.0f, 3.0f}, {6.0f, 3.0f},
        {5.0f, 3.0f}, {5.0f, 5.0f},
        {7.0f, 4.0f}, {9.0f, 4.0f},
        {9.0f, 4.0f}, {9.0f, 8.0f},
        {11.0f, 5.0f}, {13.0f, 5.0f},
        {11.0f, 5.0f}, {11.0f, 8.0f},
        {13.0f, 5.0f}, {13.0f, 7.0f},

        // Região inferior
        {1.0f, 8.0f}, {5.0f, 8.0f},
        {3.0f, 8.0f}, {3.0f, 12.0f},
        {5.0f, 9.0f}, {7.0f, 9.0f},
        {7.0f, 9.0f}, {7.0f, 13.0f},
        {9.0f, 10.0f}, {11.0f, 10.0f},
        {9.0f, 10.0f}, {9.0f, 14.0f},
        {11.0f, 11.0f}, {13.0f, 11.0f},
        {13.0f, 11.0f}, {13.0f, 13.0f},

        // Pequenas paredes no canto inferior
        {1.0f, 14.0f}, {6.0f, 14.0f},
        {8.0f, 14.0f}, {14.0f, 14.0f},
    };

    //Removendo secao para saida
    maze_walls.erase(std::remove(maze_walls.begin(), maze_walls.end(), std::make_pair(15.0f, 15.0f)), maze_walls.end());
}

//Iniciando parametros de luz
void initLights() {
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    GLfloat global_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    //Ordem das luzes: RGB tres vezes ; Branco
    lights = {
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false},
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false} 
    };
}

void drawTextGame(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

//Gerando labirinto
void drawMaze() {
    for (auto &wall : maze_walls) {
        //Se a parede nao for iluminada, ela estara preta
        GLfloat wall_color[] = {0.0f, 0.0f, 0.0f, 1.0f};

        //Checando contato de luz
        for (auto &light : lights) {
            if (light.thrown && !light.hit_wall) {
                //E configurando a cor da parede referente a luz dominante
                wall_color[0] = light.cor[0];
                wall_color[1] = light.cor[1];
                wall_color[2] = light.cor[2];
                break;
            }
        }

        //Aplicando propriedades
        glMaterialfv(GL_FRONT, GL_AMBIENT, wall_color);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_color);
        glMaterialfv(GL_FRONT, GL_SPECULAR, wall_color);

        GLfloat shininess[] = {50.0f};
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

        //Checando borda
        bool is_boundary_wall = 
            (wall.first == 0.0f || wall.first == 15.0f || 
             wall.second == 0.0f || wall.second == 15.0f);

        //E desenhando a parade
        glPushMatrix();
        glTranslatef(wall.first, wall.second + (is_boundary_wall ? 2.5f : 2.0f), 0.0f); // Centralizando parede
        if (is_boundary_wall) {
            glScalef(1.5f, 5.0f, 3.0f); //Maior escala para paredes de borda
        } else {
            glScalef(1.0f, 4.0f, 3.0f);
        }
        glutSolidCube(1.0f); //Desenhando cubo esticado (parede)
        glPopMatrix();
    }
}

//Desenhando fim do labirinto - esfera verde
void drawEndPoint() {
    //Propriedades do fim do labirinto
    GLfloat end_ambient[] = {0.0f, 0.5f, 0.0f, 1.0f};
    GLfloat end_diffuse[] = {0.0f, 1.0f, 0.0f, 1.0f};
    GLfloat end_specular[] = {0.5f, 1.0f, 0.5f, 1.0f}; 
    GLfloat end_shininess[] = {50.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, end_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, end_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, end_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, end_shininess);

    glPushMatrix();
    glTranslatef(end_point.first, end_point.second, 0.5f);
    glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();
}

//Desenhando jogador: uma esfera reluzente
void drawPlayer() {
    GLfloat sphere_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat sphere_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat sphere_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat sphere_shininess[] = {128.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, sphere_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sphere_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, sphere_shininess);

    glPushMatrix();
    glTranslatef(player.pos[0], player.pos[1], 0.0f);
    glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();
}

//Desenho de luzes
void drawLights() {
    for (int i = 0; i < lights.size(); i++) {
        if (!lights[i].active) continue;

        if (lights[i].thrown) {
            glEnable(GL_LIGHT0 + i);

            //Posicionando a luz num eixo Z = 0
            GLfloat light_position[] = {lights[i].pos[0], lights[i].pos[1], 0.0f, 1.0f};
            
            //Modificando cor da luz referente ao estoque
            GLfloat light_diffuse[] = {lights[i].cor[0], lights[i].cor[1], lights[i].cor[2], 1.0f};
            GLfloat light_specular[] = {lights[i].cor[0], lights[i].cor[1], lights[i].cor[2], 1.0f};
            GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

            glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular);
            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient);

            //Desenhando wireframe
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
    
    //Configurando perspectiva
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //Posicionando angulo da camera

    if (zoom_out) {
        //Quando o jogo era top down, essa configuracao atual mostrava o labirinto inteiro no fim do jogo
        //Agora so mostra o meio do labirinto
        gluLookAt(7.5, 7.5, camera_height,  
                  7.5, 7.5, 0.0,            
                  0.0, 1.0, 0.0);
    } else {
        //Perspectiva normal isometrica
        gluLookAt(player.pos[0] - 3.0, player.pos[1] - 3.0, camera_height,
                  player.pos[0], player.pos[1], 0.0,
                  0.0, 0.0, 1.0);
    }
    
    drawMaze();
    drawEndPoint();
    drawPlayer();
    drawLights();
    
    //Condicao de gameover
    if (game_over) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        drawTextGame(glutGet(GLUT_WINDOW_WIDTH)/2 - 100, glutGet(GLUT_WINDOW_HEIGHT)/2, "YOU WIN! Press spacebar to start NG+ (harder difficulty)");
        glEnable(GL_LIGHTING);
    }
    
    glutSwapBuffers();
}

void throwLight() {
    if (player.remaining_lights <= -1) return;

    //Jogando as 14 luzes aqui
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i].active && !lights[i].thrown) {
            lights[i].thrown = true;
            lights[i].pos[0] = player.pos[0];
            lights[i].pos[1] = player.pos[1];
            
            //Configurando direcao de jogar
            lights[i].dre[0] = light_direction[0];
            lights[i].dre[1] = light_direction[1];
            /*lights[i].dir[0] = light_direction[0];
            lights[i].dir[1] = light_direction[1];*/
            
            //Configurando que a luz nao bateu a parede
            lights[i].distance = 0.0f;
            lights[i].hit_wall = false;
            //player.remaining_lights--; 

            break;
        }
    }
}

//Atualizando a luz relativo a sua distancia
void updateLights() {
    for (auto &light : lights) {
        if (light.thrown && !light.hit_wall) {
            //Movendo a luz
            light.pos[0] += light.dre[0] * 0.05f;
            light.pos[1] += light.dre[1] * 0.05f;
            light.distance += 0.05f;

            //Checando a colisao
            for (auto &wall : maze_walls) {
                if (fabs(light.pos[0] - wall.first) < 0.3f && 
                    light.pos[1] >= wall.second && 
                    light.pos[1] <= wall.second + 4.2f) {
                    light.hit_wall = true;
                    break;
                }
            }

            //Ou fazendo a luz parar se chegou sua distancia maxima
            if (light.distance >= light.max_distance) {
                light.hit_wall = true;
            }
        }
    }
}

//Checando se o jogo terminou
void update(int value) {
    if (game_start && !game_over) {
        spin += 0.1f;
        if (spin > 360.0f) spin -= 360.0f;
        
        updateLights();
        
        //Checando endpoint
        if (sqrt(pow(player.pos[0] - end_point.first, 2) + pow(player.pos[1] - end_point.second, 2)) < 1.0f) {
            game_over = true;
            zoom_out = true;
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

//Controles
void gameKeyboard(unsigned char key, int x, int y) {
    float move_speed = 0.2f;
    float new_x = player.pos[0];
    float new_y = player.pos[1];

    switch (key) {
        //Reiniciar jogo
        case ' ':
            if (game_over) {
                game_start = false;
                game_over = false;
                zoom_out = false;
                player = {{2.0f, 2.0f}, 3};
                initMaze();
                glutDisplayFunc(menuDisplay);
                glutKeyboardFunc(menuKeyboard);
                glutPostRedisplay();
            } else {
                throwLight();
            }
            break;
        case 'a': //Esquerda
            if (!game_over) new_y += move_speed;
            light_direction[0] = 0.0f;
            light_direction[1] = 1.0f;
            break;
        case 'd': //Direita
            if (!game_over) new_y -= move_speed;
            light_direction[0] = 0.0f;
            light_direction[1] = -1.0f;
            break;
        case 's': //Baixo
            if (!game_over) new_x -= move_speed;
            light_direction[0] = -1.0f;
            light_direction[1] = 0.0f;
            break;
        case 'w': //Cima
            if (!game_over) new_x += move_speed;
            light_direction[0] = 1.0f;
            light_direction[1] = 0.0f;
            break;
        case 27: //ESC = Sair do jogo
            exit(0);
            break;
    }

    //Detectando se o jogador colidiu com uma parede
    //Se sim, nao pode se movimentar a essa direcao
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
        case GLUT_KEY_UP: 
            light_direction[0] = 1.0f;
            light_direction[1] = 0.0f;
            break;
        case GLUT_KEY_DOWN: 
            light_direction[0] = -1.0f;
            light_direction[1] = 0.0f;
            break;
        case GLUT_KEY_LEFT: 
            light_direction[0] = 0.0f;
            light_direction[1] = 1.0f;
            break;
        case GLUT_KEY_RIGHT: 
            light_direction[0] = 0.0f;
            light_direction[1] = -1.0f;
            break;
    }
}

void switchToGameMode() {
    //Iniciando jogo
    initLights();
    initMaze();
    
    //Resetando jogo
    player = {{2.0f, 2.0f}, 3};
    game_over = false;
    zoom_out = false;
    
    //Jogo primeiro inicia no menu
    //Se no menu, o jogador apertar para iniciar, o codigo do jogo comeca
    glutDisplayFunc([]() {
        if (game_start) {
            gameDisplay();
        } else {
            menuDisplay();
        }
    });
    glutKeyboardFunc(gameKeyboard);
    glutSpecialFunc(gameSpecialKeyboard);
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
    
    //Comecando menu
    glutDisplayFunc(menuDisplay);
    glutReshapeFunc(menuReshape);
    glutKeyboardFunc(menuKeyboard);
    glutTimerFunc(0, updateMenu, 0);
    
    glutMainLoop();
    return 0;
}
