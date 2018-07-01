#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro/mouse.h>
#define LINES 30
#define COLUMS 40
typedef struct personagem {
    float x;
    float y;
    int armadilhas;
    int isKilled;
    float vx;
    float vy;
}Personagem;

typedef struct monster {
    int vidas;
    int x;
    int y;
    float rx;
    float ry;
    float vx;
    float vy;
    BITMAP* bm;
    int dir;
    int tipo;
    int isVivo;
}Monstro;

typedef struct evento {
    struct evento* next;
    int tipo;
    int valor;
    int indice;
    int direcao;
    Monstro* monstro;

}Evento;

typedef struct node {
    int i; // coords x e y da imagem , como contadores
    int j;
    struct node* next;
}Queue;

typedef struct bomba {
    int x;
    int y;
    struct bomba *next;
    int explodida;
    int ticada;
    BITMAP* explosao;
    BITMAP* normal;
}Bomba;

/// 0 parado , 1 esquerda , 2 direita , 3 cima , 4 baixo
void moveLogico (int onq,Personagem* player,char matriz[LINES][COLUMS]){


    int x,y;
    x = (int) player->x/20;
    y = (int) player->y/20;
    switch (onq){
case 4:
    if (matriz[y][x+1] != '*'){
        matriz[y][x] = ' ';
        matriz[y][x+1] = 'p';
        player->vx = 0.5;

    }
    break;
case 1:
    if (matriz[y-1][x] != '*'){
        matriz[y][x] = ' ';
        matriz[y-1][x] = 'p';
        player->vy = -0.5;

    }
    break;
case 2:
    if (matriz[y+1][x] != '*'){
        matriz[y][x] = ' ';
        matriz[y+1][x] = 'p';
        player->vy = 0.5;
    }
    break;


case 3:
    if (matriz[y][x-1] != '*'){
        matriz[y][x] = ' ';
        matriz[y][x-1] = 'p';
        player->vx = -0.5;

    }
    break;
}
}

void moveMonstro (Monstro* monstro,int dir,char maze[LINES][COLUMS]){
    if (dir == 3){
        monstro->y-=1;
        monstro->vy = -0.5 - monstro->tipo*0.5;
        monstro->dir = 1;
        }
    if (dir == 4){
        monstro->y+=1;
        monstro->vy = 0.5 + monstro->tipo*0.5;
        monstro->dir = 2;
        }
    if (dir == 1){
        monstro->x-=1;
        monstro->vx = -0.5 - monstro->tipo*0.5;
        monstro->dir = 3;
        }
    if (dir == 2){
        monstro->x+=1;
        monstro->vx = 0.5 + monstro->tipo*0.5;
        monstro->dir = 0;
        }

}

void setaDraw (Monstro** monstros){
        int i;
        for (i=0;i<5;i++){
            monstros[i]->rx = monstros[i]->x *20;
            monstros[i]->ry = monstros[i]->y *20;
            monstros[i]->dir = 0;
            monstros[i]->vx=0;
            monstros[i]->vy=0;
        }
}

void Morreu (Monstro** monstros,BITMAP* burntA,BITMAP* burntB){
    int i;
    for (i=0;i<5;i++){
        if (monstros[i]->vidas <= 0 && monstros[i]->isVivo == 1){
            monstros[i]->isVivo = 0;
            if (monstros[i]->tipo == 0)
                monstros[i]->bm = burntB;
            else if (monstros[i]->tipo == 1){
                monstros[i]->bm = burntA;
            }
        }
    }

}
int checaCaminho (char mat[LINES][COLUMS], int x,int y,int dir,int* yu){ /// resolvedor de labirintos , mostra uma das saidas visualmente (pela matriz) se encontrar um caminh
    int vetor [5] = {0,0,0,0,0}; /// vetor de direcao , comeca numa posicao zerada e invalida (pos 5)
    int i,cont=0,max=0;                   /// o vetor de direcao tem informacoes a respeito de todos os lados de uma dada celular da matriz
    if (x >=LINES-1 || y>= COLUMS-1)
        return 0; /// se estourar , returna 0
    if (mat[x][y] == 'p' || mat[x-1][y] == 'p' || mat[x+1][y] == 'p' || mat[x][y-1] == 'p' || mat[x][y+1] == 'p'){
        mat[x][y] = 'v';
        return 2;
        } /// se encontrar a saida , entao returna 2;
    if (mat[x-1][y] == ' ') /// se for espaco  em qualquer uma direcao o vetor de direcao naquela direcao vale 1.
        vetor[0] = 1;
    if (mat[x+1][y] == ' ')
        vetor[1] = 1;
    if (mat[x][y-1] == ' ')
        vetor[2] = 1;
    if (mat[x][y+1] == ' ')
        vetor[3] = 1;
    //vetor [dir] = 0;
    if (vetor[0] == 1){
        if (mat[x][y] != 'p')
            mat[x][y] = 'a';
        *yu ++;
        vetor[0] = checaCaminho(mat,x-1,y,1,&yu);
        if (vetor[0] == 2)
            if (mat[x][y] != 'p')
                mat[x][y] = 'v';
    }
    if (vetor[1] == 1){
        if (mat[x][y] != 'p')
            mat[x][y] = 'a';
        *yu ++;
        vetor[1] =  checaCaminho(mat,x+1,y,0,&yu);
        if (vetor[1] == 2)
            if (mat[x][y] != 'p')
                mat[x][y] = 'v';
    }
    if (vetor[2] == 1){
        if (mat[x][y] != 'p')
            mat[x][y] = 'a';
        *yu ++;
        vetor[2] =  checaCaminho(mat,x,y-1,3,&yu);
        if (vetor[2] == 2)
            if (mat[x][y] != 'p')
                mat[x][y] = 'v';
    }
    if (vetor[3] == 1){
        if (mat[x][y] != 'p')
            mat[x][y] = 'a';
        *yu ++;
        vetor[3] =  checaCaminho(mat,x,y+1,2,&yu);
        if (vetor[3] == 2)
            if (mat[x][y] != 'p')
                mat[x][y] = 'v';
    }
    for (i=0;i<4;i++)
        cont+= vetor[i]; ///se a soma dos vetores for 0 , o labirinto eh invalido
    if (cont == 0) /// valida a celula como com saida ou sem.
        return 0;
    else if (cont >= 2){
        if (mat[x][y] != 'p')
            mat[x][y] = 'v';
        return 2;
    }
    else
        return 0;
    } /// para a primeira celular ser validada todas as outras tem que ser validadas.


void limpaCaminho (char maze[LINES][COLUMS]){
    int i,j;
    for (i=0;i<LINES;i++){
        for (j=0;j<COLUMS;j++){
            if (maze[i][j] == 'v')
                maze[i][j] = ' ';
            if (maze[i][j] == 'a')
                maze[i][j] = ' ';
            if (maze[i][j] == 'p')
                maze[i][j] = ' ';
        }
    }

}

char **leituraArquivo(char maze[LINES][COLUMS],Personagem* player, BITMAP* monstroa,BITMAP* monstrob,int* xsaida,int* ysaida){ //LC* o arquivo, passando as coordenadas do player e inimigos por parametro. TambC)m cria o labirinto.

	int i, j, k = 0,aux;

	Monstro ** monstros = (Monstro**) malloc (sizeof(Monstro*)*5);
	for(i=0;i<5;i++)
        monstros[i] = (Monstro*) malloc(sizeof(Monstro));

	FILE *arquivo = fopen("maze_H_1.txt", "r");
	if (arquivo == NULL){
		printf("Arquivo nao encontrado\n");
		return 0;
	}

	while (k<7)
	{
		fscanf(arquivo, "%d %d", &i, &j);
		if (k == 0) {
			player->y = i*20;
			player->x = j*20;
			player->armadilhas = 0;
			player->isKilled = 0;
			k++;
			continue;
		}
		if (k == 1) {
			monstros[0]->x = i;
			monstros[0]->y = j;
			monstros[0]->tipo = 1;
			monstros[0]->isVivo = 1;
			monstros[0]->vidas=3;
			monstros[0]->bm=monstroa;
			monstros[0]->dir = 0;
			k++;
			continue;
		}
		if (k == 6) {
        *xsaida = i;
        *ysaida = j;
			k++;
			break;
		}
		monstros[k-1]->x = i;
		monstros[k-1]->y = j;
		monstros[k-1]->tipo = 0;
		monstros[k-1]->isVivo = 1;
		monstros[k-1]->bm = monstrob;
		monstros[k-1]->vidas=1;
		monstros[0]->dir = 0;
		k++;
	}

	while (fscanf(arquivo,"%d %d %d",&i,&j,&aux)!=EOF){
        if(aux==0)
            maze[i][j] = ' ';
        else maze[i][j] = '*';
	}
    maze[*xsaida][*ysaida] = 's';
	fclose(arquivo);
	return monstros;
}
Evento* criarEvento(){
    return NULL;
}
Evento* inserirEvento(Evento* fila,int indice,int tipo,int valor,int direcao,Monstro* monstro){
    Evento* novo = (Evento*) malloc(sizeof(Evento));
    novo->tipo = tipo;
    novo->valor = valor;
    novo->indice = indice;
    novo->next = fila;
    novo->direcao = direcao;
    novo->monstro = monstro;
    return novo;
}

Evento* EventQueuer (Monstro* teste,Evento* lista,Personagem* player,char maze[LINES][COLUMS]){
    int xp,yp,xm,ym,dist=999,dir=0,fk;
    xp = (int) player->x/20;
    yp = (int) player->y/20;
    xm = (int) teste->x;
    ym = (int) teste->y;
    maze[yp][xp] = 'p';
    checaCaminho(maze,xm,ym,5,fk);
    dist = compConexo(maze,ym,xm,&dir);
    limpaCaminho(maze);
    if (dist <= 5+teste->tipo*3){
        printf("%d ", dir);
        lista = inserirEvento(lista,0,1,1,dir,teste);
    }

    return lista;
}

int face (Evento* lista){
    int x;
    x=lista->direcao;
    printf("x = %d", x);
    if (x==1)
        return 3;
}

int EmptyEvento (Evento* fila){
    return (fila == NULL);
}

Evento* movProcessor (Evento* lista,char maze[LINES][COLUMS]){
    Evento* aux;
    for (aux = lista ; aux != NULL;aux = aux->next){
        if (aux->tipo == 1){
            moveMonstro(aux->monstro,aux->direcao,maze);
        }
    }
    free(lista);
    return NULL;

}

Queue* create_queue () {
   return NULL;
}


Queue* enqueue (Queue *q, int i,int j) {
   /*Criando novo elemento: */
   Queue *n = (Queue *)malloc(sizeof(Queue));
   n->i = i;
   n->j = j;
   n->next = NULL; /*último elemento da lista não tem próximo.*/
   /*Localizando a cauda da lista encadeda: */
   Queue *aux, *end = NULL;
   for (aux = q; aux != NULL; aux = aux->next) {
      end = aux;
   }
   /*Se a lista não exister vazia a cabeça da lista se mantém.*/
   if (end != NULL) {
      end->next = n; /*conectando o novo elemento na cauda da lista.*/
      return q; /*retorne a cabeça da lista encadeada.*/
   }
   else {
      return n; /*A lista estava vazia, logo n é a cabeça da lista.*/
   }
}

Queue* dequeue (Queue *q,int* i,int* j) {
   if (!Empty(q)) {
      Queue *aux = q->next; /*Guarda endereço do próximo elemento.*/
      *i = q->i; // associa os valores que foram retirados a variaveis passada por referencia , pois precisamos destes valores
      *j = q->j;
      free(q); /*Removendo nodo da lista.*/
      return aux; /*Nova cabeça da lista!*/
   }
   else {
      return NULL; /*Remoção em uma lista vazia.*/
   }
}

int Empty (Queue *q) {
   return (q == NULL);
}

int compConexo (char mat[LINES][COLUMS], int x,int y, int* dir){
    Queue* fila = create_queue();
    int iaux,jaux,contador=0;
    if (mat[y-1][x] == 'v' || mat[y-1][x] == 'p'){
        fila = enqueue(fila,y-1,x);
        *dir = 1;
    }
    if (mat[y+1][x] == 'v' || mat[y+1][x] == 'p'){
        fila = enqueue(fila,y+1,x);
        *dir = 2;
    }
    if (mat[y][x-1] == 'v' || mat[y][x-1] == 'p'){
        fila = enqueue(fila,y,x-1);
        *dir = 3;
    }
    if (mat[y][x+1] == 'v' || mat[y][x+1] == 'p'){
        fila = enqueue(fila,y,x+1);
        *dir = 4;
    }
    while (!Empty(fila)){
        fila = dequeue(fila,&jaux,&iaux);
        contador++;
        mat[jaux][iaux] = ' ';
        if (mat[jaux-1][iaux] == 'v'){
            mat[jaux-1][iaux] = ' ';
            fila = enqueue(fila,jaux-1,iaux);
        }
        if (mat[jaux+1][iaux] == 'v'){
            mat[jaux+1][iaux] = ' ';
            fila = enqueue(fila,jaux+1,iaux);
        }
        if (mat[jaux][iaux-1] == 'v'){
            mat[jaux][iaux-1] = ' ';
            fila = enqueue(fila,jaux,iaux-1);
        }
        if (mat[jaux][iaux+1] == 'v'){
            mat[jaux][iaux+1] = ' ';
            fila = enqueue(fila,jaux,iaux+1);
        }
    }
    return contador;

}

Bomba* criarBomba (){
    return NULL;
}
Bomba* inserirBomba (Bomba* lista,int x , int y,Personagem* player,BITMAP* normal){
    Bomba* novo = (Bomba*) malloc(sizeof(Bomba));
    novo->x=x;
    novo->y=y;
    novo->next=lista;
    novo->explodida = 0;
    novo->ticada = 0;
    novo->normal = normal;
    player->armadilhas++;
    return novo;
}

int conta_nos (Bomba* lista) {
   int elementos = 0;
   Bomba *aux;
   for (aux = lista; aux != NULL; aux = aux->next) {
      elementos++;
   }
   return elementos;
}

int detectaBomba(Bomba* bombas,Monstro* monstros){
    if((bombas->x == monstros->y)&&(bombas->y == monstros->x))
        return 1;
    else
        return 0;

}

Bomba* processaBombas (Monstro** monstros,Bomba* lista,Personagem* player){
    int i,j;
    Bomba* aux;
    for (aux=lista;aux!=NULL;aux=aux->next){
        for (i=0;i<5;i++){
            if (detectaBomba(aux,monstros[i])){
                monstros[i]->vidas --;
                aux->explodida = 1;
                player->armadilhas--;
            }
        }
    }
}
Bomba* removeB (Bomba *lista, int elem) {
   Bomba *prv = NULL; Bomba *v = lista;
    int i=0;
   while ((v != NULL) && (i != elem)) {
        i++;
      prv = v;
      v = v->next;
   }
   if (v == NULL) { return lista; }
   if (prv == NULL) { lista = v->next; }
   else { prv->next = v->next; }
   free (v);
   return lista;
}
Bomba* desligaBombas (Bomba* lista){
    int vetor[] = {0,0,0};
    Bomba* aux;
    int i=0,ls=0;
    for (aux = lista;aux!= NULL;aux=aux->next){
        if (aux->ticada == 1){
            vetor[i] = 1;
            ls++;
        }
        i++;
    }
    if (vetor[2] == 1)
        lista = removeB(lista,2);
    if (vetor[1] == 1)
        lista = removeB(lista,1);
    if (vetor[0] == 1)
        lista = removeB(lista,0);
    return lista;


}
Bomba* animaBombas (Bomba* lista,BITMAP* explosao){
    Bomba* aux;
    for (aux = lista ; aux != NULL ; aux=aux->next){
        if (aux->explodida == 1){
            aux->normal = explosao;
            aux->ticada = 1;
        }
    }
}
int checaWin (int xsaida,int ysaida, Personagem* player){
    if ((int)player->x/20 == ysaida && (int)player->y/20 == xsaida)
        return 1;
    else return 0;
}
///timer para controlar tempo de atualização da cena
volatile int ticks = 0;

void ticks_counter()
{
    ticks++;
}
END_OF_FUNCTION(tick_counter)

///timer que controla a troca de frame dos sprites
volatile int frame_update = 0;

void frame_upadate_counter()
{
    frame_update++;
}
END_OF_FUNCTION(frame_upadate_counter())

//exibe a matriz que representa o labirinto
void printMaze(char mat[LINES][COLUMS])
{
    int i, j;

    for(i = 0; i < LINES; i++)
    {
        for(j = 0; j < COLUMS; j++)
        {
            printf("%c", mat[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    int i,j,anima=0,mov=0,onq,zero=0,moveu=0,jm=0,colocou=0,xsaida,ysaida,ganhou=0;
    printf("Bem vindo \nuso : \nsetinhas do teclado para controle\nespaco para bomba\nrecomenda-se jogar pressionando\n");

    /// monolito de declaracoes de variaveis , starts do allegro e starts de structs.

    ///inicializa allegro
    allegro_init();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0); /// se tiver em windowed mult de 320,240
    install_keyboard();
    install_mouse();
    show_mouse(screen);
    install_timer();

    ///inicializa timers
    LOCK_FUNCTION(ticks_counter);
    LOCK_VARIABLE(ticks);
    install_int_ex(ticks_counter, BPS_TO_TIMER(120)); //incrementa 60 vezes em um segundo
    LOCK_FUNCTION(frame_upadate_counter);
    LOCK_VARIABLE(frame_update);
    install_int_ex(frame_upadate_counter, MSEC_TO_TIMER(100));


    ///inicializa as texturas
    BITMAP *parede = load_bmp("texture/wall.bmp", NULL);
    BITMAP *chao = load_bmp("texture/path.bmp", NULL);
    BITMAP *saida = load_bmp ("texture/door.bmp", NULL);
    BITMAP *player = load_bitmap("texture/player.bmp", NULL);
    BITMAP *monstroa = load_bitmap("texture/monster_A.bmp",NULL);
    BITMAP *monstrob = load_bitmap("texture/monster_B.bmp",NULL);
    BITMAP *explosao = load_bitmap("texture/bomba.bmp", NULL);
    BITMAP *normal = load_bitmap ("texture/trap.bmp", NULL);
    BITMAP *burntA = load_bitmap ("texture/burnt_a.bmp", NULL);
    BITMAP *burntB = load_bitmap ("texture/burnt_b.bmp", NULL);

    Personagem* jogador = (Personagem*) calloc(sizeof(Personagem),1);
    int frame_size_h = (player->h)/4;
    int frame_size_w = (player->w)/4;

    ///buffers
    BITMAP *buffer = create_bitmap(COLUMS*20,LINES*20);
    BITMAP *labirinto = create_bitmap(COLUMS*20,LINES*20);

    char maze[LINES][COLUMS];
    Monstro **monstros= leituraArquivo(maze,jogador,monstroa,monstrob,&xsaida,&ysaida);

    ///salva o labirinto uma vez antes de executar
    for (i=0;i<LINES;i++){
        for (j=0;j<COLUMS;j++){
            if (maze[i][j] == '*')
                draw_sprite(labirinto,parede,j*20,i*20);
            else if (maze[i][j] == 's')
                draw_sprite (labirinto,saida,j*20,i*20);
            else if (maze [i][j] == ' ')
                draw_sprite(labirinto,chao,j*20,i*20);
        }
    }
    draw_sprite (buffer,labirinto,0,0);
    Evento* listadeEventos=criarEvento();
    maze[(int)jogador->y/20][(int)jogador->x/20] = 'p';
    masked_blit (player, buffer, 0 * frame_size_w, 0 * frame_size_h, jogador->x, jogador->y, frame_size_w, frame_size_h);
    int k;
    setaDraw(monstros);
    for (k =0 ;k<5;k++){
                    if (monstros[k]->vx != 0 || monstros[k]->vy != 0)
                        jm= j;
                    else
                        jm =0;
                    printf("%d ", monstros[k]->tipo);
                    masked_blit(monstros[k]->bm,buffer,0 * frame_size_w,0 * frame_size_w,monstros[k]->ry,monstros[k]->rx,frame_size_w,frame_size_h);
                    if (((int)monstros[k]->rx == (int)jogador->y && (int)monstros[k]->ry == (int)jogador->x)&& monstros[k]->isVivo == 1)
                        jogador->isKilled = 1;
                }
    draw_sprite (screen,buffer,0,0);
    Bomba* listadeBomba = criarBomba();
    while(!key[KEY_ESC] && !jogador->isKilled){
        while(ticks>0){
            if (mov == 0){
                anima++;
                i=0;
                if (key[KEY_SPACE] && colocou == 0 && jogador->armadilhas < 3){
                    listadeBomba=inserirBomba(listadeBomba,(int)jogador->x/20,(int)jogador->y/20,jogador,normal);
                    colocou=1;
                }
                if (key[KEY_UP]){
                    onq=1;
                    moveLogico (onq,jogador,maze);
                    mov=2;
                    i=3;
                    anima=0;
                }
                else if (key[KEY_DOWN]){
                    onq= 2;
                    moveLogico (onq,jogador,maze);
                    mov=2;
                    i=0;
                    anima=0;
                }
                else if (key[KEY_LEFT]){
                    onq = 3;
                    moveLogico (onq,jogador,maze);
                    mov=2;
                    i=1;
                    anima=0;
                }
                else if (key[KEY_RIGHT]){
                    onq=4;
                    moveLogico(onq,jogador,maze);
                    mov=2;
                    i=2;
                    anima=0;
                }
                else if (anima == 2){
                    anima=0;
                    mov=2;
                    moveu=1;
                }
                Morreu(monstros,burntA,burntB);

            }
            else{
                anima++;
                if (!moveu)
                    j = frame_update % 4;
                else
                    j=0;
                if (anima == 41){
                    mov =0;
                    anima = 0;
                    onq=0;
                    jogador->vx=0;
                    jogador->vy=0;
                    mov=0;
                    moveu=0;
                    int k;
                    for (k=0;k<5;k++){
                    monstros[k]->vx=0;
                    monstros[k]->vy=0;
                    colocou=0;
                    }
                    processaBombas(monstros,listadeBomba,jogador);
                    listadeBomba=desligaBombas(listadeBomba);
                }
                if (anima == 1){
                    animaBombas(listadeBomba,explosao);
                }
                clear(buffer);
                draw_sprite (buffer,labirinto,0,0);
                jogador->x+=jogador->vx;
                jogador->y+=jogador->vy;
                int k;
                for (k=0;k<5;k++){
                    if (monstros[k]->tipo == 1 && anima == 21 && (monstros[k]->vx != 0 || monstros[k]->vy != 0)){
                        monstros[k]->vx=0;
                        monstros[k]->vy=0;
                        int xp,yp,xm,ym,dist=999,dir=0,fk;
                        xp = (int) jogador->x/20;
                        yp = (int) jogador->y/20;
                        xm = (int) monstros[k]->x;
                        ym = (int) monstros[k]->y;
                        maze[yp][xp] = 'p';
                        checaCaminho(maze,xm,ym,5,fk);
                        dist = compConexo(maze,ym,xm,&dir);
                        moveMonstro(monstros[k],dir,maze);
                }
                    monstros[k]->rx+=monstros[k]->vx;
                    monstros[k]->ry+=monstros[k]->vy;
                }

                masked_blit (player, buffer, j * frame_size_w, i * frame_size_w, jogador->x, jogador->y, frame_size_w, frame_size_h);
                Bomba* aux;
                for (aux = listadeBomba ; aux != NULL ;aux=aux->next){
                    draw_sprite(buffer,aux->normal,aux->x*20,aux->y*20);
                }

                for (k =0 ;k<5;k++){
                    if (monstros[k]->vx != 0 || monstros[k]->vy != 0)
                        jm= frame_update % 4;
                    else
                        jm =0;


                    masked_blit(monstros[k]->bm,buffer,jm * frame_size_w,monstros[k]->dir * frame_size_w,monstros[k]->ry,monstros[k]->rx,frame_size_w,frame_size_h);
                    if (((int)monstros[k]->rx == (int)jogador->y && (int)monstros[k]->ry == (int)jogador->x)&& monstros[k]->isVivo == 1)
                        jogador->isKilled = 1;
                }
                draw_sprite(screen,buffer,0,0);



            }
            if (mov == 2){
                    int k;
                for (k =0;k<5;k++){
                    if (monstros[k]->isVivo == 1)
                        listadeEventos = EventQueuer(monstros[k],listadeEventos,jogador,maze);
                }
                mov = 1;
                if (!EmptyEvento(listadeEventos)){
                   listadeEventos = movProcessor (listadeEventos,maze);


                }

            }
            ticks--;
        }
        if (checaWin(xsaida,ysaida,jogador)){
            ganhou=1;
            break;
        }
    }
    if (ganhou == 1){
        printf("\nwinner winner chicken dinner\n");
    }
    else
        printf("\nse fodeu\n");
    system("PAUSE");
    return 0;
}
END_OF_MAIN()
