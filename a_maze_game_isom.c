//Codigo referente ao jogo

// Inclusão de bibliotecas necessárias
#include <GL/glut.h> // Biblioteca para gráficos OpenGL
#include "amaze_menu.h" // Arquivo de cabeçalho para o menu do jogo
#include <stdlib.h> // Biblioteca padrão para funções gerais
#include <math.h> // Biblioteca para funções matemáticas
#include <vector> // Biblioteca para uso de vetores (arrays dinâmicos)
#include <cmath> // Biblioteca para funções matemáticas em C++
#include <string> // Biblioteca para manipulação de strings
#include <algorithm> // Biblioteca para algoritmos como remove, sort, etc.
#include <stack> // Biblioteca para usar pilha

//Estados de jogo
bool game_start = false; // Indica se o jogo começou
bool game_over = false; // Indica se o jogo terminou
bool zoom_out = false; // Indica se a câmera está em modo zoom out

//Estruturas de luz e jogador
struct Light {
    bool active; // Indica se a luz está ativa
    float pos[3]; // Posição da luz (x, y, z)
    float cor[3]; // Cor da luz (R, G, B)
    float dre[2]; // Direção da luz (x, y)
    bool thrown; // Indica se a luz foi lançada
    float distance; // Distância percorrida pela luz
    float max_distance; // Distância máxima que a luz pode percorrer
    bool hit_wall; // Indica se a luz colidiu com uma parede
};

struct Player {
    float pos[2]; // Posição do jogador (x, y)
    int remaining_lights; // Número de luzes restantes
};

// Inicialização do jogador e outras variáveis globais
Player player = {{0.0f, 0.0f}, 10}; // Jogador começa na posição (0.0, 0.0) com 10 luzes
std::vector<Light> lights; // Vetor para armazenar as luzes
std::vector<std::pair<float, float>> maze_walls; // Vetor para armazenar as paredes do labirinto
std::pair<float, float> end_point = {0.0f, 0.0f}; // Ponto de saída do labirinto

//Configurações da câmera isométrica
float camera_distance = 5.0f; // Distância da câmera
float spin = 0.0f; // Rotação da câmera
float light_direction[2] = {1.0f, 0.0f}; // Direção inicial da luz
float camera_angle = 45.0f; // Ângulo da câmera
float camera_height = 10.0f; // Altura da câmera

//Função para inicializar o labirinto
void initMaze() {
    const float cell_size = 1.0f; // Tamanho de cada célula
    maze_walls.clear(); // Limpa as paredes existentes

    // Define um layout simples de labirinto (grade 10x10)
    // 1 representa uma parede, 0 representa um espaço vazio
    int maze_layout[16][16] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1},
        {1,0,1,0,0,0,1,0,0,0,1,0,0,1,0,1},
        {1,0,1,0,1,0,1,1,1,1,1,0,1,1,0,1},
        {1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,1},
        {1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,1},
        {1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1},
        {1,0,1,0,0,0,1,0,0,1,0,1,0,1,0,1},
        {1,0,1,1,1,0,1,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Converte o layout em coordenadas de paredes
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if (maze_layout[y][x] == 1) {
                maze_walls.push_back({x * cell_size, y * cell_size});
            }
        }
    }

    // Define o ponto de saída no canto inferior direito
    end_point = {7.0f, 13.5f}; // Centro da célula (8, 1)

    // Define o ponto de spawn do jogador no canto superior esquerdo
    player.pos[0] = 1.5f; // Centro da célula (1, 1)
    player.pos[1] = 1.5f; // Centro da célula (1, 1)
}

//Função para inicializar as luzes
void initLights() {
    glEnable(GL_LIGHTING); // Habilita iluminação
    glEnable(GL_DEPTH_TEST); // Habilita teste de profundidade

    // Configuração da luz ambiente global
    GLfloat global_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Luz ambiente suave
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // Ordem das luzes: RGB três vezes ; Branco
    lights = {
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz vermelha
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz verde
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz azul
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz vermelha
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz verde
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz azul
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz vermelha
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz verde
        {true, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}, // Luz azul
        {true, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, false, 0.0f, 10.0f, false}  // Luz branca
    };
}

//Função para desenhar texto na tela
void drawTextGame(float x, float y, const std::string &text) {
    glRasterPos2f(x, y); // Define a posição do texto
    for (char c : text) { // Loop para desenhar cada caractere
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Desenha o caractere
    }
}

//Função para desenhar o labirinto
void drawMaze() {
    for (auto &wall : maze_walls) { // Loop pelas paredes do labirinto
        //Se a parede não for iluminada, ela estará preta
        GLfloat wall_color[] = {0.0f, 0.0f, 0.0f, 1.0f};

        //Checando contato de luz
        for (auto &light : lights) { // Loop pelas luzes
            if (light.thrown && !light.hit_wall) { // Se a luz foi lançada e não atingiu uma parede
                //Configurando a cor da parede referente à luz dominante
                wall_color[0] = light.cor[0]; // Componente R
                wall_color[1] = light.cor[1]; // Componente G
                wall_color[2] = light.cor[2]; // Componente B
                break;
            }
        }

        //Aplicando propriedades da parede
        glMaterialfv(GL_FRONT, GL_AMBIENT, wall_color); // Cor ambiente
        glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_color); // Cor difusa
        glMaterialfv(GL_FRONT, GL_SPECULAR, wall_color); // Cor especular

        GLfloat shininess[] = {50.0f}; // Brilho
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

        //Desenhando a parede
        glPushMatrix(); // Salva a matriz atual
        glTranslatef(wall.first, wall.second, 0.0f); // Centraliza a parede
        glScalef(1.0f, 1.0f, 3.0f); // Maior escala para paredes de borda
        glutSolidCube(1.0f); // Desenha um cubo esticado (parede)
        glPopMatrix(); // Restaura a matriz salva
    }
}

//Função para desenhar o ponto de saída do labirinto (esfera verde)
void drawEndPoint() {
    //Propriedades do fim do labirinto
    GLfloat end_ambient[] = {0.0f, 0.5f, 0.0f, 1.0f}; // Cor ambiente
    GLfloat end_diffuse[] = {0.0f, 1.0f, 0.0f, 1.0f}; // Cor difusa
    GLfloat end_specular[] = {0.5f, 1.0f, 0.5f, 1.0f}; // Cor especular
    GLfloat end_shininess[] = {50.0f}; // Brilho

    glMaterialfv(GL_FRONT, GL_AMBIENT, end_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, end_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, end_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, end_shininess);

    glPushMatrix(); // Salva a matriz atual
    glTranslatef(end_point.first, end_point.second, 0.5f); // Posiciona a esfera
    glutSolidSphere(0.5f, 20, 20); // Desenha a esfera
    glPopMatrix(); // Restaura a matriz salva
}

//Função para desenhar o jogador (esfera reluzente)
void drawPlayer() {
    GLfloat sphere_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f}; // Cor ambiente
    GLfloat sphere_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f}; // Cor difusa
    GLfloat sphere_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Cor especular
    GLfloat sphere_shininess[] = {128.0f}; // Brilho

    glMaterialfv(GL_FRONT, GL_AMBIENT, sphere_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sphere_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, sphere_shininess);

    glPushMatrix(); // Salva a matriz atual
    glTranslatef(player.pos[0], player.pos[1], 0.0f); // Posiciona o jogador
    glutSolidSphere(0.5f, 20, 20); // Desenha a esfera
    glPopMatrix(); // Restaura a matriz salva
}

//Função para desenhar as luzes
void drawLights() {
    for (int i = 0; i < lights.size(); i++) { // Loop pelas luzes
        if (!lights[i].active) continue; // Se a luz não estiver ativa, pula para a próxima

        if (lights[i].thrown) { // Se a luz foi lançada
            glEnable(GL_LIGHT0 + i); // Habilita a luz

            //Posicionando a luz no eixo Z = 0
            GLfloat light_position[] = {lights[i].pos[0], lights[i].pos[1], 0.0f, 1.0f};
            
            //Modificando cor da luz referente ao estoque
            GLfloat light_diffuse[] = {lights[i].cor[0], lights[i].cor[1], lights[i].cor[2], 1.0f};
            GLfloat light_specular[] = {lights[i].cor[0], lights[i].cor[1], lights[i].cor[2], 1.0f};
            GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

            glLightfv(GL_LIGHT0 + i, GL_POSITION, light_position);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light_specular);
            glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light_ambient);

            //Desenhando wireframe da luz
            glPushMatrix(); // Salva a matriz atual
            glTranslatef(lights[i].pos[0], lights[i].pos[1], 0.0f); // Posiciona a luz
            glColor3f(lights[i].cor[0], lights[i].cor[1], lights[i].cor[2]); // Define a cor
            glutWireSphere(0.3f, 16, 16); // Desenha a esfera wireframe
            glPopMatrix(); // Restaura a matriz salva
        } else {
            glDisable(GL_LIGHT0 + i); // Desabilita a luz se não foi lançada
        }
    }
}

//Função principal de exibição do jogo
void gameDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa os buffers
    
    //Configurando perspectiva
    glMatrixMode(GL_PROJECTION); // Define a matriz de projeção
    glLoadIdentity(); // Carrega a matriz identidade
    gluPerspective(60.0, 1.0, 0.1, 100.0); // Define a perspectiva
    
    glMatrixMode(GL_MODELVIEW); // Define a matriz de modelo/visualização
    glLoadIdentity(); // Carrega a matriz identidade
    
    //Posicionando ângulo da câmera
    if (zoom_out) {
        //Modo zoom out: mostra o labirinto inteiro
        gluLookAt(7.5, 7.5, camera_height,  // Posição da câmera
                  7.5, 7.5, 0.0,            // Ponto de foco
                  0.0, 1.0, 0.0);           // Vetor de orientação
    } else {
        //Perspectiva normal isométrica: foca no jogador
        gluLookAt(player.pos[0] - 3.0, player.pos[1] - 3.0, camera_height, // Posição da câmera
                  player.pos[0], player.pos[1], 0.0, // Ponto de foco
                  0.0, 0.0, 1.0); // Vetor de orientação
    }
    
    //Desenha os elementos do jogo
    drawMaze(); // Labirinto
    drawEndPoint(); // Ponto de saída
    drawPlayer(); // Jogador
    drawLights(); // Luzes
    
    //Condição de game over
    if (game_over) {
        glDisable(GL_LIGHTING); // Desabilita iluminação para o texto
        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para o texto
        glMatrixMode(GL_PROJECTION); // Define a matriz de projeção
        glLoadIdentity(); // Carrega a matriz identidade
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT)); // Projeção ortogonal 2D
        glMatrixMode(GL_MODELVIEW); // Define a matriz de modelo/visualização
        glLoadIdentity(); // Carrega a matriz identidade
        drawTextGame(glutGet(GLUT_WINDOW_WIDTH)/2 - 100, glutGet(GLUT_WINDOW_HEIGHT)/2, "YOU WIN! Press spacebar twice to start NG+ (harder difficulty)"); // Desenha o texto
        glEnable(GL_LIGHTING); // Reabilita iluminação
    }
    
    glutSwapBuffers(); // Troca os buffers para exibir a cena
}

//Função para lançar uma luz
void throwLight() {
    if (player.remaining_lights <= -1) return; // Se não houver luzes restantes, retorna

    //Lançando as luzes
    for (int i = 0; i < lights.size(); i++) { // Loop pelas luzes
        if (lights[i].active && !lights[i].thrown) { // Se a luz está ativa e não foi lançada
            lights[i].thrown = true; // Marca como lançada
            lights[i].pos[0] = player.pos[0]; // Posição x inicial
            lights[i].pos[1] = player.pos[1]; // Posição y inicial
            
            //Configurando direção de lançamento
            lights[i].dre[0] = light_direction[0]; // Direção x
            lights[i].dre[1] = light_direction[1]; // Direção y
            
            //Configurando que a luz não bateu na parede
            lights[i].distance = 0.0f; // Distância inicial
            lights[i].hit_wall = false; // Não atingiu parede
            break;
        }
    }
}

//Função para atualizar as luzes
void updateLights() {
    for (auto &light : lights) { // Loop pelas luzes
        if (light.thrown && !light.hit_wall) { // Se a luz foi lançada e não atingiu parede
            //Movendo a luz
            light.pos[0] += light.dre[0] * 0.05f; // Atualiza posição x
            light.pos[1] += light.dre[1] * 0.05f; // Atualiza posição y
            light.distance += 0.05f; // Atualiza distância percorrida

            //Checando colisão com paredes
            for (auto &wall : maze_walls) { // Loop pelas paredes
                if (fabs(light.pos[0]- wall.first) < 0.5f && fabs(light.pos[1] - wall.second) < 0.5f) { // Verifica colisão
                    light.hit_wall = true; // Marca como atingiu parede
                    break;
                }
            }

            //Ou fazendo a luz parar se chegou à distância máxima
            if (light.distance >= light.max_distance) {
                light.hit_wall = true; // Marca como atingiu parede
            }
        }
    }
}

//Função para atualizar o estado do jogo
void update(int value) {
    if (game_start && !game_over) { // Se o jogo começou e não terminou
        spin += 0.1f; // Atualiza rotação
        if (spin > 360.0f) spin -= 360.0f; // Mantém o ângulo entre 0 e 360
        
        updateLights(); // Atualiza as luzes
        
        //Checando se o jogador alcançou o ponto de saída
        if (sqrt(pow(player.pos[0] - end_point.first, 2) + pow(player.pos[1] - end_point.second, 2)) < 1.0f) {
            game_over = true; // Marca o jogo como terminado
            zoom_out = true; // Ativa o zoom out
        }
    }
    
    glutPostRedisplay(); // Solicita redesenho da tela
    glutTimerFunc(16, update, 0); // Configura o próximo update
}

//Função para tratar teclas pressionadas
void gameKeyboard(unsigned char key, int x, int y) {
    float move_speed = 0.2f; // Velocidade de movimento
    float new_x = player.pos[0]; // Nova posição x
    float new_y = player.pos[1]; // Nova posição y

    switch (key) {
        //Reiniciar jogo ou lançar luz
        case ' ':
            if (game_over) { // Se o jogo terminou
                game_start = false; // Reinicia o jogo
                game_over = false;
                zoom_out = false;
                player = {{1.0f, 8.0f}, 3}; // Reseta o jogador
                initMaze(); // Reinicia o labirinto
                glutDisplayFunc(menuDisplay); // Volta para o menu
                glutKeyboardFunc(menuKeyboard);
                glutPostRedisplay();
            } else {
                throwLight(); // Lança uma luz
            }
            break;
        case 'a': // Movimento para a esquerda
            if (!game_over) new_y += move_speed;
            light_direction[0] = 0.0f;
            light_direction[1] = 1.0f;
            break;
        case 'd': // Movimento para a direita
            if (!game_over) new_y -= move_speed;
            light_direction[0] = 0.0f;
            light_direction[1] = -1.0f;
            break;
        case 's': // Movimento para baixo
            if (!game_over) new_x -= move_speed;
            light_direction[0] = -1.0f;
            light_direction[1] = 0.0f;
            break;
        case 'w': // Movimento para cima
            if (!game_over) new_x += move_speed;
            light_direction[0] = 1.0f;
            light_direction[1] = 0.0f;
            break;
        case 27: // Tecla ESC: sai do jogo
            exit(0);
            break;
    }

    //Detectando colisão com paredes
    bool can_move = true; // Flag para movimento permitido
    for (auto &wall : maze_walls) { // Loop pelas paredes
        if (fabs(new_x - wall.first) < 0.5f && fabs(new_y - wall.second) < 0.5f) {
            can_move = false; // Colisão detectada
            break;
        }
    }

    if (can_move) { // Se não houve colisão
        player.pos[0] = new_x; // Atualiza posição x
        player.pos[1] = new_y; // Atualiza posição y
    }

    glutPostRedisplay(); // Solicita redesenho da tela
}

//Função para tratar teclas especiais (setas)
void gameSpecialKeyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: // Seta para cima
            light_direction[0] = 1.0f;
            light_direction[1] = 0.0f;
            break;
        case GLUT_KEY_DOWN: // Seta para baixo
            light_direction[0] = -1.0f;
            light_direction[1] = 0.0f;
            break;
        case GLUT_KEY_LEFT: // Seta para esquerda
            light_direction[0] = 0.0f;
            light_direction[1] = 1.0f;
            break;
        case GLUT_KEY_RIGHT: // Seta para direita
            light_direction[0] = 0.0f;
            light_direction[1] = -1.0f;
            break;
    }
}

//Função para alternar para o modo de jogo
void switchToGameMode() {
    //Inicializando jogo
    initLights(); // Configura as luzes
    initMaze(); // Configura o labirinto
    
    //Resetando jogo
    player = {{1.0f, 1.0f}, 10}; // Posição inicial do jogador
    game_over = false; // Reseta estado de game over
    zoom_out = false; // Desativa zoom out
    
    //Configurando funções de callback
    glutDisplayFunc([]() {
        if (game_start) {
            gameDisplay(); // Exibe o jogo
        } else {
            menuDisplay(); // Exibe o menu
        }
    });
    glutKeyboardFunc(gameKeyboard); // Teclado normal
    glutSpecialFunc(gameSpecialKeyboard); // Teclas especiais
    glutTimerFunc(0, update, 0); // Configura o update
    
    game_start = true; // Marca o jogo como iniciado
    glutPostRedisplay(); // Solicita redesenho da tela
}

//Função principal
int main(int argc, char **argv) {
    glutInit(&argc, argv); // Inicializa GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Modo de exibição
    glutInitWindowSize(1200, 1200); // Tamanho da janela
    glutInitWindowPosition(100, 100); // Posição da janela
    glutCreateWindow("Maze Ball Game"); // Cria a janela
    
    initMenuLights(); // Inicializa luzes do menu
    
    //Configurando callbacks do menu
    glutDisplayFunc(menuDisplay); // Exibição
    glutReshapeFunc(menuReshape); // Redimensionamento
    glutKeyboardFunc(menuKeyboard); // Teclado
    glutTimerFunc(0, updateMenu, 0); // Update
    
    glutMainLoop(); // Loop principal
    return 0; // Retorno padrão
}
