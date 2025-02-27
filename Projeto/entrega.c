#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define HEIGHT 50
#define WIDTH 200
#define MAX_INIMIGOS 10
#define VISION_RANGE 8
#define SCALE_FACTOR 2


typedef struct {
    int x;
    int y;
    int vida;
    int ataque;
    int velocidade;
} Inimigo;

typedef struct {
    int x;
    int y;
    int vida;
    int ataque;
} Jogador;

void bresenham(int x0, int y0, int x1, int y1, char map[HEIGHT][WIDTH], char visible_map[HEIGHT][WIDTH], char discovered_map[HEIGHT][WIDTH]) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        visible_map[y0][x0] = map[y0][x0];
        discovered_map[y0][x0] = 1;

        if (x0 == x1 && y0 == y1) {
            break;
        }

        if (map[y0][x0] == '#') {
            break;
        }

        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}




void create_map(char map[HEIGHT][WIDTH]) {
    int x, y;
    
    // Cria um mapa vazio com espaços em branco
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            map[y][x] = ' ';
        }
    }
    
    // Adiciona as paredes ao redor do mapa
    for (x = 0; x < WIDTH; x++) {
        map[0][x] = '#';
        map[HEIGHT-1][x] = '#';
    }
    for (y = 0; y < HEIGHT; y++) {
        map[y][0] = '#';
        map[y][WIDTH-1] = '#';
    }
    
    // Adiciona paredes aleatórias no meio do mapa
    srand(time(NULL));
    for (int i = 0; i < (HEIGHT * WIDTH) / 10; i++) {
        x = rand() % (WIDTH-2) + 1;
        y = rand() % (HEIGHT-2) + 1;
        map[y][x] = '#';
    }
}


void draw_map(char map[HEIGHT][WIDTH], char visible_map[HEIGHT][WIDTH], char discovered_map[HEIGHT][WIDTH], Jogador* jogador, Inimigo inimigo[], int n_inimigos) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char ch = map[y][x];
            // Desenhe o jogador
            if (jogador->x == x && jogador->y == y) {
                ch = '@';
            } else {
                // Desenhe os inimigos
                for (int i = 0; i < n_inimigos; i++) {
                    if (inimigo[i].x == x && inimigo[i].y == y) {
                        ch = 'M';
                        break;
                    }
                }
            }
            if (visible_map[y][x] == 0) {
                if (discovered_map[y][x] != 0) {
                    attron(COLOR_PAIR(1));
                    mvaddch(y, x, discovered_map[y][x] == ' ' ? '.' : discovered_map[y][x]);
                    attroff(COLOR_PAIR(1));
                }
            } else {
                discovered_map[y][x] = map[y][x];
                mvaddch(y, x, ch == ' ' ? '.' : ch);
            }
        }
    }
}

void update_visible_map(char map[HEIGHT][WIDTH], char visible_map[HEIGHT][WIDTH], char discovered_map[HEIGHT][WIDTH], Jogador* jogador) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            visible_map[y][x] = 0;
        }
    }

    for (int angle = 0; angle < 360; angle++) {
        double rad = angle * M_PI / 180;
        int player_x = jogador->x;
        int player_y = jogador->y;
        int x1 = player_x + cos(rad) * VISION_RANGE * SCALE_FACTOR;
        int y1 = player_y + sin(rad) * VISION_RANGE;
        bresenham(player_x, player_y, x1, y1, map, visible_map, discovered_map);
    }
}


int coincide (int x, int y, Inimigo inimigo[], int n_inimigos) {
    for (int i = 0; i < n_inimigos; i++) {
        if (x == inimigo[i].x && y == inimigo[i].y) {
            return 1;
        }
    }
    return 0;
}

void move_inimigo (Inimigo* inimigo, int jogadorx, int jogadory, char map[HEIGHT][WIDTH]) {
    int dx = jogadorx - inimigo->x;
    int dy = jogadory - inimigo->y;
    int distancia = sqrt(dx*dx + dy*dy);
    
    if (distancia < 10) {
        if (dx == 0 && dy == 0) return;
        
        
        int prox_x = inimigo->x;
        int prox_y = inimigo->y;

        if(abs(dx) > abs(dy)) {
            if (dx > 0 && map[prox_y][prox_x + 1] != '#' && prox_x + 1 != jogadorx) {
                prox_x++;
            } else if (dx < 0 && map[prox_y][prox_x - 1] != '#' && prox_x - 1 != jogadorx) {
                prox_x--;
            }
            } else {
                if (dy > 0 && map[prox_y + 1][prox_x] != '#' && prox_y + 1 != jogadory) {
                prox_y++;
            } else if (dy < 0 && map[prox_y - 1][prox_x] != '#' && prox_y - 1 != jogadory) {
                prox_y--;
            }
        }

        inimigo->x = prox_x;
        inimigo->y = prox_y;   
    }
}

void cria_inimigo (Inimigo inimigo[], int n, int jogadorx, int jogadory, char map[HEIGHT][WIDTH]) {
    int x, y;
    for (int i = 0; i < n; i++) {
        do {
            x = rand() % (WIDTH - 2) + 1;
            y = rand() % (HEIGHT - 2) + 1;
    
        } while (x == jogadorx || y == jogadory || map[y][x] == '#' || coincide(x,y,inimigo,i));
        
        inimigo[i].x = x;
        inimigo[i].y = y;
        inimigo[i].vida = rand() % 11 + 20;
        inimigo[i].ataque = rand() % 2 + 3;
        inimigo[i].velocidade = 2;
    }
}

void atacar (Inimigo* inimigo, int jogadorx, int jogadory, Jogador* jogador) {
    int dx = abs(jogadorx - inimigo->x);
    int dy = abs(jogadory - inimigo->y);
    
    if ((((dx == 1) && (dy == 0)) || ((dx == 0) && (dy == 1)))) {
        jogador->vida -= inimigo->ataque;
    }
}

void desenha_inimigo (Inimigo* inimigo, int n, char visible_map[HEIGHT][WIDTH]) {
    for (int i = 0; i < n; i++) {
        if (visible_map[inimigo[i].y][inimigo[i].x]) {
            mvaddch (inimigo[i].y, inimigo[i].x, 'M');
        }
    }
}



    void move_player(char map[HEIGHT][WIDTH], int key, Inimigo inimigo[], int n_inimigos, Jogador* jogador) {
    int novo_x = jogador->x;
    int novo_y = jogador->y;

    switch(key) {
        case KEY_UP:
        case 'k':
        case '8':
            novo_y--;
            break;
        case KEY_DOWN:
        case 'j':
        case '2':
            novo_y++;
            break;
        case KEY_LEFT:
        case 'h':
        case '4':
            novo_x--;
            break;
        case KEY_RIGHT:
        case 'l':
        case '6':
            novo_x++;
            break;
    }

    if (map[novo_y][novo_x] != '#') {
        for (int i = 0; i < n_inimigos; i++) {
            if (novo_x == inimigo[i].x && novo_y == inimigo[i].y) {
                inimigo[i].vida -= jogador->ataque;
                if (inimigo[i].vida <= 0) {
                    for(int j = i; j < n_inimigos - 1; j++) {
                        inimigo[j] = inimigo[j+1];
                    }
                    n_inimigos--;
                }
                return;
            }
        }

        jogador->x = novo_x;
        jogador->y = novo_y;

    }
}
void desenha_jogador (int x, int y) {
    mvaddch (y, x, '@');
}






int main() {
    char map[HEIGHT][WIDTH];
    char visible_map[HEIGHT][WIDTH];
    char discovered_map[HEIGHT][WIDTH];

    Inimigo inimigo[MAX_INIMIGOS];
    int n_inimigos = 0;
    Jogador jogador = {WIDTH / 2, HEIGHT / 2, 100, 8};

    // Inicia a biblioteca ncurses
    initscr();
    curs_set(0);  // 1 visível, 2 muito visível
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);

    // Gera o mapa aleatório
    srand(time(NULL));
    create_map(map);

    // Inicializa o mapa descoberto com 0s
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            discovered_map[y][x] = 0;
        }
    }

    update_visible_map(map, visible_map, discovered_map, &jogador);
    draw_map(map, visible_map, discovered_map, &jogador, inimigo, n_inimigos);

    // inimigos
    cria_inimigo (inimigo, MAX_INIMIGOS, jogador.x, jogador.y, map);
    n_inimigos = MAX_INIMIGOS;
    
    // Loop principal
    while (1) {
        // Lê a entrada do usuário
        int key = getch();
         if (key == 'q') break;
        desenha_jogador(jogador.x, jogador.y);
        desenha_inimigo(inimigo, n_inimigos, visible_map);

    
        for (int i = 0; i < n_inimigos; i++) {
            move_inimigo(&inimigo[i], jogador.x, jogador.y, map);
            atacar(&inimigo[i], jogador.x, jogador.y, &jogador);
        }

        mvprintw(HEIGHT+1, 0, "Health: %d", jogador.vida);
        refresh();

        for (int i = 0; i < n_inimigos; i++) {
            if (inimigo[i].vida <= 0) {
                for (int j = 0; j < n_inimigos - 1; j++) {
                    inimigo[j] = inimigo[j + 1];
                }
                n_inimigos--;
            }
        }

        if (jogador.vida <= 0) {
            clear();
            mvprintw(HEIGHT/2, WIDTH/2-10, "Game over!");
            refresh();
            getch();
            break;
        }

        // Move o jogador e redesenha o mapa
        
         // Move o jogador
        move_player(map, key, inimigo, n_inimigos, &jogador);

        // Checa se o jogador colidiu com um inimigo
        for (int i = 0; i < n_inimigos; i++) {
            if (jogador.x == inimigo[i].x && jogador.y == inimigo[i].y) {
                inimigo[i].vida -= jogador.ataque;
                if (inimigo[i].vida <= 0) {
                    for (int j = i; j < n_inimigos - 1; j++) {
                        inimigo[j] = inimigo[j + 1];
                    }
                    n_inimigos--;
                }
                break;
            }
        }
    

        update_visible_map(map, visible_map, discovered_map, &jogador);
        draw_map(map, visible_map, discovered_map, &jogador, inimigo, n_inimigos);



    }

    // Encerra a biblioteca ncurses
    endwin();
    return 0;
}
