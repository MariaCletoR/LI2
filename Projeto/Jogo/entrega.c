#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ALTURA 50
#define LARGURA 200
#define MAX_INIMIGOS 10
#define ALCANCE_VISAO 8
#define ESCALA 2


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

void desenhaMenu() {
    clear();


    // Desenha o título
    attron(A_BOLD);
        mvprintw(14, 83, "EEEEE    SSSS   CCCCC    AAAA    PPPP     AAAA");
        mvprintw(15, 83, "E       S       C       A    A   P   P   A    A");
        mvprintw(16, 83, "EEEE    SSSS    C       AAAAAA   PPPP    AAAAAA");
        mvprintw(17, 83, "E           S   C       A    A   P       A    A");
        mvprintw(18, 83, "EEEEE   SSSS    CCCCC   A    A   P       A    A");
    attroff(A_BOLD);


    // Desenha o quadrado de opções
attron(A_BOLD);  // Ativa a formatação em negrito
    mvprintw(24, 95  , "+----------------------+");  // Linha superior
    mvprintw(25 + 4, 96 - 1, "+----------------------+");  // Linha inferior
    for (int i = 0; i < 4; i++) {
        mvprintw(25 + i, 96 - 1, "|");  // Lateral esquerda
        mvprintw(25 + i, 96 + 22, "|"); // Lateral direita
    }
    mvprintw(25 + 1, 96 + 6, "1. Jogar");
    mvprintw(25 + 2, 96 + 6, "2. Sair");
    attroff(A_BOLD);  // Desativa a formatação em negrito




    // Atualiza a tela
    refresh();
}

void desenhaH() {
    clear();
    attron(A_BOLD);
    mvprintw(12, 95, "História do Jogo:");
    attroff(A_BOLD);

    mvprintw(18, 30, "Lucky, um explorador e escritor de renome de 38 anos, estava em mais uma das suas aventuras para finalizar o seu grande livro *A Civilização Antiga*.");
    mvprintw(20, 20, "Após ouvir relatos sobre os rituais macabros que a tribo Kujalank do Budapeste realizava em outros humanos, Lucky decidiu ver por ele mesmo o local onde ocorriam estes atos.");
    mvprintw(22, 20, "Após chegar a Budapeste, enquanto explorava uma masmorra subterrânea ancestral, inundada por uma atmosfera pesada e arrepiante, ocorreu um abalo sísmico.");
    mvprintw(24, 30, "Agora, uma porta com uma ranhura estranha ,que antes se encontrava selada, fora aberta e de lá, inúmeros monstros saíram.");
    mvprintw(26, 30, "Encontra a chave para colocar na ranhura antes que os monstros te devorem.");
    attron(A_BOLD);
    mvprintw(34, 50, "Pressione qualquer tecla para continuar...");
    attroff(A_BOLD);

   attron(A_BOLD);
    mvprintw(45, 140, "Mova os personagens com as setas direcionais.");
    mvprintw(47, 140, "Caso queira sair do jogo pressione a tecla 'q'.");
   attroff(A_BOLD);
}

// funcao que desenha uma linha reta no mapa utilizando o algoritmo de Bresenham
void bresenham(int x0, int y0, int x1, int y1, char mapa[ALTURA][LARGURA], char mapa_visivel[ALTURA][LARGURA], char mapa_descoberto[ALTURA][LARGURA]) {

    // distancia entre as coordenadas finais e iniciais, para ser possivel determinar a direcao da linha que se vai desenhar
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    // indicam se devemos incrementar ou decrementar as coordenadas x e y a medida que se desenha a linha
    int sx = x0 < x1 ? 1 : -1;   // se x1 maior que x0 entao sx = 1 senao sx = -1 
    int sy = y0 < y1 ? 1 : -1;

    // err - medida do erro na posicao atual da linha, que e para ajudar a decidir se devemos mover na direcao x ou y para desenhar a prox linha
    int err = dx - dy;

    while (1) {// continua ate se chegar ao ponto final da linha (quando x0=x1 e y0=y1)
        // atualiza os mapas para marcar que um ponto esta visivel e ja foi descoberto
        mapa_visivel[y0][x0] = mapa[y0][x0];
        mapa_descoberto[y0][x0] = 1;

        if (x0 == x1 && y0 == y1) { //ponto final
            break;
        }

        if (mapa[y0][x0] == '#') { // encontra se uma parede
            break;
        }

        int e2 = err * 2; // dobro do erro atual
        if (e2 > -dy) {  // se o valor do erro e maior que a diferenca vertical entre as coordenadas y, o proximo passo sera na direcao x
            err -= dy;   // diminuir o valor do erro em dy
            x0 += sx;   // adicionar x0 em sx
        }
        if (e2 < dx) {  // diferenca horizontal entre as coordenadas x 
            err += dx;
            y0 += sy;
        }
    }
}

void cria_mapa(char mapa[ALTURA][LARGURA], int chave_x, int chave_y) {
    int x, y;   // usadas para percorrer o mapa
    
    // Cria um mapa vazio com espaços em branco
    for (y = 0; y < ALTURA; y++) {
        for (x = 0; x < LARGURA; x++) {
            mapa[y][x] = ' ';
        }
    }
    
    // Adiciona as paredes ao redor do mapa
    for (x = 0; x < LARGURA; x++) {
        mapa[0][x] = '#';
        mapa[ALTURA-1][x] = '#';
    }
    for (y = 0; y < ALTURA; y++) {
        mapa[y][0] = '#';
        mapa[y][LARGURA-1] = '#';
    }
    
    // Adiciona paredes aleatórias no meio do mapa
    srand(time(NULL));
    for (int i = 0; i < (ALTURA * LARGURA) / 10; i++) {   // o numero de paredes e um decimo do tamanho do mapa
        x = rand() % (LARGURA-2) + 1;      // [1, LARGURA - 2]
        y = rand() % (ALTURA-2) + 1;
        mapa[y][x] = '#';
    }

    mapa[chave_y][chave_x] = '&';
}

void desenha_mapa(char mapa[ALTURA][LARGURA], char mapa_visivel[ALTURA][LARGURA], char mapa_descoberto[ALTURA][LARGURA], Jogador* jogador, Inimigo inimigo[], int n_inimigos, int chave_x, int chave_y) {
    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            char ch = mapa[y][x];
            
            // Desenhe o jogador
            if (jogador->x == x && jogador->y == y) {  // caractere que corresponde a posicao atual do mapa
                ch = '@';
            } else {
                // Se houver encontrar algum inimigo naquela posicao desenha-o
                for (int i = 0; i < n_inimigos; i++) {
                    if (inimigo[i].x == x && inimigo[i].y == y) {
                        ch = 'M';
                        break;
                    }
                }
            }

            if (mapa_visivel[y][x] == 0) { // o mapa ainda nao foi descoberto pelo jogador nessa parte
                if (mapa_descoberto[y][x] != 0) { // verificar se ja foi descoberto anteriormente
                    attron(COLOR_PAIR(1));
                    mvaddch(y, x, mapa_descoberto[y][x] == ' ' ? '.' : mapa_descoberto[y][x]);
                    attroff(COLOR_PAIR(1));
                }
            } else { // a posicao atual e visivel para o jogador
                mapa_descoberto[y][x] = mapa[y][x];

                //  desenha a chave
                if (chave_x == x && chave_y == y) {
                    attron(COLOR_PAIR(2));
                    mvaddch (y, x, '&');
                    attroff(COLOR_PAIR(2));

                    // desenha um "."
                } else {
                   mvaddch(y, x, ch == ' ' ? '.' : ch); 
                }   
            }
        }
    }
}



void atualiza_mapa_visivel(char mapa[ALTURA][LARGURA], char mapa_visivel[ALTURA][LARGURA], char mapa_descoberto[ALTURA][LARGURA], Jogador* jogador) { 

 // percorremos todas as posições e definimos como 0 (nenhuma é visivel)

    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            mapa_visivel[y][x] = 0; 
        }
    }

  // basicamente, com esta parte, estamos a percorrer a área ao redor do jogador de ângulo em ângulo

    for (int angle = 0; angle < 360; angle++) {
        double rad = angle * M_PI / 180;  // converter o ângulo atual em radianos porque as funções em C funcionam com radianos  (angulo * Pi a dividir por 180)
        int player_x = jogador->x;   //  vamos buscar as coordenadas ao player
        int player_y = jogador->y;
        int x1 = player_x + cos(rad) * ALCANCE_VISAO * ESCALA;  // calcula o deslocamento horizontal a partir do jogador (x), temos de multiplicar pela distância máxima para definir o alcance da visão e pelo valor de escala para ficar mais bonito
        int y1 = player_y + sin(rad) * ALCANCE_VISAO;  // calcula o vertical (y)
        bresenham(player_x, player_y, x1, y1, mapa, mapa_visivel, mapa_descoberto);  // neste caso, se encontrar um # o loop vai ser interrompido
    }
}

// basicamente com esta função estamos a definir a área toda que está à volta do jogador, cos para a x , sen para o y, depois chamamos a bresenham para ver onde estão os cardinais e assim impedir a visão a partir desse


// com esta função, queremos saber se a posição do jogador coincide com a posição do inimigo, se tal ocorrer devolve 1.
int coincide (int x, int y, Inimigo inimigo[], int n_inimigos) {
    for (int i = 0; i < n_inimigos; i++) {
        if (x == inimigo[i].x && y == inimigo[i].y) {
            return 1;
        }
    }
    return 0;
}



void move_inimigo (Inimigo* inimigo, int jogadorx, int jogadory, char mapa[ALTURA][LARGURA]) {
    int dx = jogadorx - inimigo->x;     // distância horizontal do player ao inimigo
    int dy = jogadory - inimigo->y;     // distância vertical.. 
    int distancia = sqrt(dx*dx + dy*dy); // teorema de pitágoras para saber a distância exata
    
    if (distancia < 10) {     // se menor que 10 ele começa a movimentar-se
        if (dx == 0 && dy == 0) return;   // se for igual a 0, ele acaba a função
        
        
        int prox_x = inimigo->x;
        int prox_y = inimigo->y;

        if(abs(dx) > abs(dy)) {   // se a distância horizontal for maior, e se for possível mover o inimigo, ele move-se na horizontal
            if (dx > 0 && mapa[prox_y][prox_x + 1] != '#' && prox_x + 1 != jogadorx) {
                prox_x++;   // direita se estiver no lado esquerdo do jogador
            } else if (dx < 0 && mapa[prox_y][prox_x - 1] != '#' && prox_x - 1 != jogadorx) {
                prox_x--;   // esquerda se estiver no lado direito do jogador  
            }
            } else {
                if (dy > 0 && mapa[prox_y + 1][prox_x] != '#' && prox_y + 1 != jogadory) {
                prox_y++;  // aqui é igual, se estiver em baixo move-se para cima
            } else if (dy < 0 && mapa[prox_y - 1][prox_x] != '#' && prox_y - 1 != jogadory) {
                prox_y--;  // se estiver em cima move-se para baixo
            }
        }

        inimigo->x = prox_x;  // no final atualizamos as posições do inimigo
        inimigo->y = prox_y;   
    }
}

void cria_inimigo (Inimigo inimigo[], int n, int jogadorx, int jogadory, char mapa[ALTURA][LARGURA]) {
    int x, y;
    for (int i = 0; i < n; i++) {   // cria n inimigos
        do {
            x = rand() % (LARGURA - 2) + 1;  // -2 por causa das bordas,  e soma-se 1 para que não dê 0 o que dá um número entre [1,LARGURA-2]
            y = rand() % (ALTURA - 2) + 1;
    
        } while (x == jogadorx || y == jogadory || mapa[y][x] == '#' || coincide(x,y,inimigo,i));  // gera novas coordenadas enquanto nenhuma dessas se comprovar
        
        inimigo[i].x = x;
        inimigo[i].y = y;
        inimigo[i].vida = rand() % 11 + 20;   // produz entre 0 e 10 + 20 = [20,30]
        inimigo[i].ataque = rand() % 2 + 3;   // mesma coisa
        inimigo[i].velocidade = 2;
    }
}

void atacar (Inimigo* inimigo, int jogadorx, int jogadory, Jogador* jogador) {
    int dx = abs(jogadorx - inimigo->x);
    int dy = abs(jogadory - inimigo->y);
    
    if ((((dx == 1) && (dy == 0)) || ((dx == 0) && (dy == 1)))) { // se a distância entre o inimigo e o jogador for 1
        jogador->vida -= inimigo->ataque;   // o jogador perde vida igual ao ataque do inimigo
    }
}

void desenha_inimigo (Inimigo* inimigo, int n, char mapa_visivel[ALTURA][LARGURA]) {
    for (int i = 0; i < n; i++) {     // percorre inimigo por inimigo
        if (mapa_visivel[inimigo[i].y][inimigo[i].x]) {     // se essas coordenadas estiverem visiveis
            mvaddch (inimigo[i].y, inimigo[i].x, 'M');  // é desenhado o inimigo no mapa
        }
    }
}


void move_jogador(char mapa[ALTURA][LARGURA], int key, Inimigo inimigo[], int n_inimigos, Jogador* jogador) {

    // coordenadas do jogador após o movimento
    int novo_x = jogador->x;
    int novo_y = jogador->y;

    switch(key) {

        case KEY_UP:
            novo_y--;
            break;
        case KEY_DOWN:
            novo_y++;
            break;
        case KEY_LEFT:
            novo_x--;
            break;
        case KEY_RIGHT:
            novo_x++;
            break;
    }

    if (mapa[novo_y][novo_x] != '#') {  // Verifica se as novas coordenadas não são uma parede
        for (int i = 0; i < n_inimigos; i++) { // Avaliar todos os inimigos
            if (novo_x == inimigo[i].x && novo_y == inimigo[i].y) { // Verificar se as coordenadas que o utilizador toma são as mesmas de um inimigo.
                inimigo[i].vida -= jogador->ataque; // Se forem, diminui a vida do inimigo
                if (inimigo[i].vida <= 0) { // Se o inimigo for derrotado é eliminado do array 
                    for(int j = i; j < n_inimigos - 1; j++) {
                        inimigo[j] = inimigo[j+1]; //  copia o inimigo seguinte para a posição atual. Isso move cada inimigo uma posição para trás no array, substituindo o inimigo derrotado.
                                                   // Após o loop, o inimigo derrotado foi "removido" do array porque todos os inimigos seguintes foram movidos uma posição para trás. 
                    
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
    char mapa[ALTURA][LARGURA];
    char mapa_visivel[ALTURA][LARGURA];
    char mapa_descoberto[ALTURA][LARGURA];

    Inimigo inimigo[MAX_INIMIGOS];
    int n_inimigos = 0;

    Jogador jogador = {LARGURA / 2, ALTURA / 2, 100, 8}; // Dividido por 2 para ele começar no centro do mapa.

    int chave_x;
    int chave_y;
    
    // Inicia a biblioteca ncurses
    initscr(); // Inicializa o modo de tela
    curs_set(0);  // mexe no cursor, 0 não visível, 1 visível, 2 muito visível
    cbreak(); // Permite receber caracteres individuais no programa, em vez de esperar por uma linha inteira de entrada.
    noecho(); // Faz com que os caracteres digitados pelo usuário não sejam exibidos na tela
    keypad(stdscr, TRUE); // Habilita a captura de teclas especiais, como as teclas de seta
 

    int opcao;

        while (1) { 
        desenhaMenu(); 
        refresh(); // atualiza a tela após a exibição do menu.
        opcao = getch(); 

        if (opcao == '1') { // passa para o jogo
            desenhaH();
            getch(); // Aguarda o jogador pressionar uma tecla para continuar
            break;
        } else if (opcao == '2') {   // sai do jogo
            endwin();   
            exit(0);
        }
    }

    clear();  // limpa a tela
    start_color();  // inicia o uso de cores
    init_pair(1, COLOR_BLUE, COLOR_BLACK); // cor do texto e a cor do fundo.
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // cor amarela para a chave

    // Gera o mapa aleatório
    srand(time(NULL)); // Essa função inicializa a semente para geração de números aleatórios usando a função 'time(NULL)' como argumento.
                       // Ao passar esse valor como semente para a função srand(), garante-se que a sequência de números aleatórios gerada seja diferente em cada execução do programa.

    chave_x = rand() % (LARGURA - 2) + 1;   
    chave_y = rand() % (ALTURA - 2) + 1;
    
    

    // Inicializa o mapa descoberto com 0s
    for (int y = 0; y < ALTURA; y++) {  
        for (int x = 0; x < LARGURA; x++) { 
            mapa_descoberto[y][x] = 0;  // Atribui o valor zero (0) a cada posição do mapa descoberto.  (ou seja, nenhum foi descoberto)
        }
    }

    cria_mapa(mapa, chave_x, chave_y); 
    atualiza_mapa_visivel(mapa, mapa_visivel, mapa_descoberto, &jogador); 
    desenha_mapa(mapa, mapa_visivel, mapa_descoberto, &jogador, inimigo, n_inimigos, chave_x, chave_y); 

    // inimigos
    cria_inimigo (inimigo, MAX_INIMIGOS, jogador.x, jogador.y, mapa); 
    n_inimigos = MAX_INIMIGOS; 
    
    // Loop principal
    while (1) {
        int key = getch();
        if (key == 'q') break;  // o jogo termina

        desenha_jogador(jogador.x, jogador.y); 
        desenha_inimigo(inimigo, n_inimigos, mapa_visivel); 

    
        for (int i = 0; i < n_inimigos; i++) { 
            move_inimigo(&inimigo[i], jogador.x, jogador.y, mapa); 
            atacar(&inimigo[i], jogador.x, jogador.y, &jogador); 
            }

        mvprintw(ALTURA+1, 0, "Health: %d ", jogador.vida); 
        refresh(); 


        if (jogador.vida <= 0) { 
            clear(); 
            attron(A_BOLD);
                    mvprintw(23, 58, "GGGGGG      AAAA      MM        MM     EEEEE            OOOOOOO    VV     VV     EEEEE     RRRRR      ");
                    mvprintw(24, 58, "G          A    A     M M      M M     E                OO   OO    VV     VV     E         R   RR  ");
                    mvprintw(25, 58, "G   GG     AAAAAA     M  M    M  M     EEEE    ======   OO   OO     VV   VV      EEEE      RRRRR  ");
                    mvprintw(26, 58, "G    G     A    A     M   M  M   M     E                OO   OO     VV   VV      E         R  RR   ");
                    mvprintw(27, 58, "GGGGGG     A    A     M    MM    M     EEEEE            OOOOOOO       VVV        EEEEE     R   RR     ");
            attroff(A_BOLD);
            refresh();
            getch();
            break;
        }

        move_jogador(mapa, key, inimigo, n_inimigos, &jogador);


        if (jogador.x == chave_x && jogador.y == chave_y) { // verifica se a posição atual do jogador é igual à posição da chave - alcançou a chave (caso seja o código dentro do bloco é executado)
            clear();
               attron(A_BOLD);
        mvprintw(23, 70, "GGGGGG     AAAA     NN    N    H    H     AAAA      SSSS   TTTTTT   EEEEE     ");
        mvprintw(24, 70, "G         A    A    N N   N    H    H    A    A    S         TT     E         ");
        mvprintw(25, 70, "G   GG    AAAAAA    N  N  N    HHHHHH    AAAAAA    SSSS      TT     EEEE      ");
        mvprintw(26, 70, "G    G    A    A    N   N N    H    H    A    A        S     TT     E         ");
        mvprintw(27, 70, "GGGGGG    A    A    N    NN    H    H    A    A    SSSS      TT     EEEEE     ");
                 attroff(A_BOLD);

            refresh();
            getch();
            break; // se pressionar qualquer o loop principal é interrompido
        }
    
        atualiza_mapa_visivel(mapa, mapa_visivel, mapa_descoberto, &jogador);   
        desenha_mapa(mapa, mapa_visivel, mapa_descoberto, &jogador, inimigo, n_inimigos, chave_x, chave_y); 
    }

    // Encerra a biblioteca ncurses
    endwin();
    return 0;
}
