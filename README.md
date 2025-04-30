# MazeBallGame
"MazeBallGame" é um jogo 3D original (criado por nós, André Luís e Mário) que brinca com estruturas da disciplina de computação gráfica.

O jogador controla uma esfera espelhada que se move por um labirinto no escuro total, mas pode lançar luzes que se apagam rapidamente. O objetivo é chegar ao fim do labirinto antes que todas as luzes se apaguem. Para um desafio adicional, os jogadores podem iniciar um modo NG+ ao completar o objetivo, com luzes que se apagam de modo mais rápido.

O jogo conta com duas perspectivas de câmera: isométrica (de cima) e em primeira pessoa. O modo FPS foi balançeado para adicionar mais luzes ao jogador.

# Imagens
![Tela de inicio](https://i.imgur.com/WkDoRmz.png)

![Jogo em execução](https://i.imgur.com/McIO9RN.png)

![Fim](https://i.imgur.com/IDcC9AH.png)

![Modo FPS](https://i.imgur.com/9dEjoLe.png)

# Jogabilidade
Usando WASD e as setas do teclado, o jogador move a esfera e desvia das paredes do labirinto para encontrar a saída.

# Conceitos 
O jogo usa estruturas como luz, reflexão, colisão, sombras e cores para demonstrar aplicação dos conceitos da disciplina.

# Como abrir - modo isométrico (Linux)
Digite "g++ a_maze_game_isom.c amaze_menu.c -o a_maze_game_isom -lGL -lGLU -lglut && ./a_maze_game_isom" (sem as aspas) no terminal do Linux.

# Como abrir - modo FPS (Linux)
Digite "g++ a_maze_game_fps.c amaze_menu.c -o a_maze_game_fps -lGL -lGLU -lglut && ./a_maze_game_fps" (sem as aspas) no terminal do Linux.

# Principais problemas encontrados:
Jogo era para ser em uma perspectiva top-down, mas desse jeito, o aplicativo não mostrava as paredes. Tivemos que mudar a vista para isométrica.

Um "oversight" no código fez com que, inicialmente, a direção das luzes jogadas determinasse a cor delas.

Problmas na geração de paredes fez com que o layout do mapa fosse inicialmente bagunçado, fazendo o jogo se parecer mais como uma "caça ao tesouro" do que um desafio de labirinto.

# O que pode ser melhorado:
Fazer o jogo ser mais completo com inimigos e colecionáveis.

# O que cada integrante fez:
André: Programação da tela de início, movimento e design geral

Mário: Programação da lógica da luz, debugging e playtester
