/**
  Autor: Fabio Lubacheski
  Objetivo: Exemplo carrega uma imagem e desenha na janela utilizando
            a biblioteca SDL
*/
#include <SDL/SDL.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <SDL/SDL_gfxPrimitives.h>	// SDL_gfx Primitives

using namespace std;

// Variaveis globais
SDL_Surface *screen = NULL; //superficie que representa a tela do computador
//SDL_Surface **torreMouse = NULL; //Array de Torre que fica no mouse
SDL_Surface* menu = NULL;
SDL_Surface* caminho = NULL;
SDL_Surface* naoCaminho = NULL;
SDL_Surface* castelo = NULL;
SDL_Surface* torreNoMouse = NULL;
SDL_Surface *torreMenu = NULL; //Imagem da torre no menu
SDL_Rect torreMouse_destino;// onde irei desenhar a torre do mouse
SDL_Rect torreMenu_destino;//onde irei desenhar a torre do menu
SDL_Rect menu_destino;
SDL_Rect **localCenario;
int altura = 700, comprimento = 900;
int qntTorres = 0;
int torre_no_mouse = 0;
int id = 1;
int inicio_do_quadriculadoX; //Onde come o cenario do jogo
int inicio_do_quadriculadoY; //Onde come o cenario do jogo
int dinheiro;//Dinheiro do jogador

struct Cenario{
    int caminho; //0 = caminho.   1 = nao caminho
    int x;
    int y;
    int usada = 0; //0 para nao e 1 para sim
};

struct Torre{
    SDL_Surface* imagem1;
    Torre *prox;
    Torre *anterior;
    SDL_Rect coord;
};

struct PilhaTorres{
    Torre *topo;
};

/*void adicionaTorre(*ListaTorres t, Torre *torre)
{
    t->topo->prox;
}*/

Torre *torreMouse = new Torre;
Cenario ***matriz;
void criaCenario(Cenario ***matriz);
PilhaTorres *criaPilha();
void desenha(PilhaTorres *p);
Cenario ***criaMatriz();
int pegaOrdemDaMatriz();
int ordemDaMatriz = pegaOrdemDaMatriz();
PilhaTorres *pilha;
void imprime(PilhaTorres *p);
void insere(PilhaTorres *p, SDL_Rect rect);


int main(int argc, char *argv[]){
    int auxX;
    dinheiro = 150;
    pilha = criaPilha();
    Torre *t;
    SDL_Event event; //um evento enviado pela SDL
    int JogoAtivo = 1; //Inicializa a condicao para GameLoop do Jogo
    matriz = criaMatriz();

    //- Inicializa a SDL -//
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    // Inicializa a tela
    screen = SDL_SetVideoMode(comprimento ,altura, 32, SDL_SWSURFACE|SDL_DOUBLEBUF );
    if(screen == NULL){
        printf("SDL_SetVideoMode: %s\n", SDL_GetError());
        SDL_Quit(); //encerrar a SDL
        exit(2); //- Configuração de vídeo não foi aceita
    }

    //- título da janela
    SDL_WM_SetCaption("Carregando imagens", NULL);

    // carrega a imagem usando a funcao
    // SDL_Surface *SDL_LoadBMP(const char *file);
    torreMenu = SDL_LoadBMP("torre2.bmp");
    torreNoMouse = SDL_LoadBMP("torre2Peq.bmp");
    menu = SDL_LoadBMP("menu.bmp");
    naoCaminho = SDL_LoadBMP("quadrado1.bmp");
    caminho = SDL_LoadBMP("quadrado2.bmp");
    castelo = SDL_LoadBMP("castelo.bmp");

    //Cria um array para guardar as torres com 100 posicoes
    /*torreMouse = new SDL_Surface*[100];
    for(int i=0;i<100;i++){
        torreMouse[i] = new SDL_Surface();
        torreMouse[i] = SDL_LoadBMP("torre2Peq.bmp");
    }
    torreMouse[1] = SDL_LoadBMP("torre2.bmp");*/

   // int SDL_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key);
   // suface = surface da imagem
   // flag = SDL_SRCCOLORKEY que torna o blit mais rapido
   // key  = fundo da imagem eh mangenta R=225 G=0 B=255
   //SDL_SetColorKey(torreMouse, SDL_SRCCOLORKEY , 0xFF00FF);

    //Local onde estara a torre no menu
    torreMenu_destino.x = 10;
    torreMenu_destino.y = 10;
    //Local onde estara o menu
    menu_destino.x = 0;
    menu_destino.y = 0;

    //Local onde sera criado o primeiro quadrado do cenario
    localCenario = new SDL_Rect*[ordemDaMatriz];
    // aloca as colunas nas linhas
    for(int aux=0;aux<ordemDaMatriz;aux++)
        localCenario[aux]=new SDL_Rect[ordemDaMatriz];
    //le a matriz a atribue para cada elemento do array um local que sera criado o cenario
    for(int i=0;i<ordemDaMatriz;i++){
        for(int j=0;j<ordemDaMatriz;j++){
            localCenario[i][j].x = (comprimento/2+menu->w/2-naoCaminho->w*(ordemDaMatriz/2))+i*(naoCaminho->w);// Ajusta o cenario
            localCenario[i][j].y = (altura/2-naoCaminho->h*(ordemDaMatriz/2))+j*(naoCaminho->h);//Ajusta o cenario no centro da tela
        }
    }
    inicio_do_quadriculadoX = localCenario[0][0].x;
    inicio_do_quadriculadoY = localCenario[0][0].y;

   if( torreMouse->imagem1 == NULL ){
      printf("Nao carreguei a torreMouse\n");
      return 1;
   }

   // Game Loop
   while( JogoAtivo ){
      // Aqui tratamos os comandos
      while(SDL_PollEvent(&event)){
         // Processar o tipo do evento
         switch(event.type){
            case SDL_KEYDOWN:
               printf("Foi pressionada uma tecla!\n");
               break;
                //Seta a imagem da torre para ficar no mouse
             case SDL_MOUSEMOTION:
              // pega o x,y do clique do mouse
              if( torre_no_mouse == 1 || torre_no_mouse == 3){
                 torreMouse_destino.x = event.motion.x -(torreNoMouse->w/2);
                 torreMouse_destino.y = event.motion.y -(torreNoMouse->h/2);
              }
             break;
            //Caso: Mouse apertado
            case SDL_MOUSEBUTTONDOWN:{
                int x = event.motion.x;
                int y = event.motion.y;
                // se a torre ja estiver no mouse
                if(event.button.button==SDL_BUTTON_LEFT
                //fora do menu,  mouse com torre
                && (x<torreMenu_destino.x
                || x>(torreMenu_destino.x+torreMenu->w)
                || y<torreMenu_destino.y
                || y>(torreMenu_destino.y+torreMenu->h))
                ){
                    if(torre_no_mouse == 1){
                        printf("t=1 fora \n");
                        torreMouse_destino.x = event.button.x /*-(torreNoMouse->w/2)*/;
                        torreMouse_destino.y = event.button.y /*-(torreNoMouse->h/2)*/;
                        insere(pilha, torreMouse_destino);
                        torre_no_mouse = 2;
                        dinheiro = dinheiro - 50;
                        break;
                    }
                    if(torre_no_mouse == 3){
                        for(t = pilha->topo; t!= NULL; t = t->prox){
                        if(event.button.x > t->coord.x && event.button.x < t->coord.x+t->imagem1->w
                        && event.button.y > t->coord.y && event.button.y < t->coord.y+t->imagem1->h
                        ){
                           // printf("oiu1");
                            /*auxX = (event.button.x - inicio_do_quadriculadoX)/torreNoMouse->w;
                            t1->coord.x = inicio_do_quadriculadoX + torreNoMouse->w * auxX;
                            int auxY = (event.button.y - inicio_do_quadriculadoY)/torreNoMouse->h;
                            t1->coord.y = inicio_do_quadriculadoY + torreNoMouse->h * auxY;
                            t1->imagem1 = SDL_LoadBMP("torre4Peq.bmp");
                            torre_no_mouse = 2;*/
                        }
                        //printf("oiu2");
                    }

                    for(Torre *t = pilha->topo; t!= NULL; t = t->prox){
                        if(t->imagem1 == NULL){
                            //Ajusta o X e Y da torre para encaixar corretamente
                            auxX = (event.button.x - inicio_do_quadriculadoX)/torreNoMouse->w;
                            t->coord.x = inicio_do_quadriculadoX + torreNoMouse->w * auxX;
                            int auxY = (event.button.y - inicio_do_quadriculadoY)/torreNoMouse->h;
                            t->coord.y = inicio_do_quadriculadoY + torreNoMouse->h * auxY;
                            t->imagem1 = SDL_LoadBMP("torre2Peq.bmp");
                            torre_no_mouse = 2;
                        }
                    }
                    break;
                    }
                }
              if(event.button.button==SDL_BUTTON_LEFT
                  //fora do menu, mouse sem torre
                  && torre_no_mouse == 0
                  && (x<torreMenu_destino.x
                  || x>(torreMenu_destino.x+torreMenu->w)
                  || y<torreMenu_destino.y
                  || y>(torreMenu_destino.y+torreMenu->h))
              ){
                for(Torre *t1 = pilha->topo; t1!= NULL; t1 = t1->prox){
                    //Se o mouse estiver em cima de alguma torre
                    if(event.button.x > t1->coord.x && event.button.x < t1->coord.x+t1->imagem1->w
                    && event.button.y > t1->coord.y && event.button.y < t1->coord.y+t1->imagem1->h
                    ){
                        t1->imagem1 = NULL;
                        torre_no_mouse = 3;
                        printf("t=0, fora, posX: %d, mouseX: %d ", t1->coord.x, event.button.x);
                        printf("t=0, fora, posY: %d, mouseY: %d\n", t1->coord.y, event.button.y);
                    }
                }
                  printf("t=0 fora \n");
                  break;
              }
              //Se a imagem da torre no menu for clicada, sem q a torre esteja no mouse
              if(event.button.button==SDL_BUTTON_LEFT
                 //dentro do menu, mouse sem torre
                  && x>torreMenu_destino.x
                  && x<(torreMenu_destino.x+torreMenu->w)
                  && y>torreMenu_destino.y
                  && y<(torreMenu_destino.y+torreMenu->h)
                  && torre_no_mouse == 0
                  && dinheiro >= 50
              ){
                    printf("t=0: dentro\n");
                    // pega o x,y do clique do mouse
                    torreMouse_destino.x = event.button.x -(torreNoMouse->w/2);
                    torreMouse_destino.y = event.button.y -(torreNoMouse->h/2);
                    torre_no_mouse = 1;
              }
              if(event.button.button==SDL_BUTTON_RIGHT){

              }
              break;
            }
            case SDL_MOUSEBUTTONUP:{
                // estou tratando o clique do mouse
                if(event.button.button==SDL_BUTTON_LEFT && torre_no_mouse == 2){
                    printf("t=2 mouseUP\n");
                    torre_no_mouse = 0;
                }
            }
             break;

            case SDL_QUIT: //fechar a janela?
               JogoAtivo = 0; //sair do loop principal
               break;
            default://Evento não tratado
               printf("Evento nao tratado!\n");
         }
      }

      // aqui fariamos as simulacoes move objetos etc...

      // Desenha a tela
      desenha(pilha);

      //FPS
      //SDL_Delay(100);
  }

  SDL_Quit(); //encerrar a SDL

  return 0;
}
// Função desenha os objetos na tela
void desenha(PilhaTorres *p)
{
    Torre *t;

    // Pinta a janela de branco (limpa tela)
    SDL_FillRect(screen, NULL, 0x005000);

    //Cria Imagem do Menu
    SDL_BlitSurface(menu, 0, screen, &menu_destino);
    //Desenha a torre do menu
    SDL_BlitSurface(torreMenu, NULL, screen, &torreMenu_destino);
    //Titulo do jogo, na tela
    stringColor(screen, comprimento/2, 5, "Tower Defense -- 2048",0xFFFFFFFF);
    //Mostra as moedas
    //stringColor(screen, comprimento/2, 5,  ,0xFFFFFFFF);



       for(int i=0;i<ordemDaMatriz;i++){
           for(int j=0;j<ordemDaMatriz;j++){
               if(matriz[i][j]->caminho == 0)
                   SDL_BlitSurface(naoCaminho, 0, screen, &localCenario[i][j]);
               if(matriz[i][j]->caminho == 1)
                   SDL_BlitSurface(caminho, 0, screen, &localCenario[i][j]);
                if(matriz[i][j]->caminho == 9)
                    SDL_BlitSurface(castelo, 0, screen, &localCenario[i][j]);
            }
        }
        if(torre_no_mouse == 1 || torre_no_mouse == 3)
            SDL_BlitSurface(torreNoMouse, NULL, screen, &torreMouse_destino);
            /*printf("%d\n", t->id);*/
            imprime(p);

    SDL_Flip(screen); //atualizar a tela
}

//Verifica a ordem da matriz, que eh o primeiro numero do arquivo .txt
int pegaOrdemDaMatriz(){
    int a;
    ifstream myfile;
    myfile.open ("dados.txt");
    myfile>>a;
    myfile.close();
    return a;
}
//Cria a Matriz nXn, que eh o ladrilho
Cenario ***criaMatriz() {
    Cenario ***mat;
    int lixo;
    ifstream myfile;
    myfile.open ("dados.txt");
    // Descarta esse numero
    myfile>>lixo;
    // aloca as linhas
    mat = new Cenario**[ordemDaMatriz];
    // aloca as colunas nas linhas
    for(int i=0;i<ordemDaMatriz;i++)
        mat[i]=new Cenario*[ordemDaMatriz];
    //le a matriz
    for(int i=0;i<ordemDaMatriz;i++)
        for(int j=0;j<ordemDaMatriz;j++){
            mat[j][i] = new Cenario();
            myfile>>mat[j][i]->caminho;
        }
    return mat;
}



PilhaTorres *criaPilha(){
    PilhaTorres *p = new PilhaTorres;
    p->topo = NULL;
	return p;
}

void libera(PilhaTorres *p){
    Torre *t, *temp;
    t = p->topo;
    while(t!=NULL){
        temp = t->prox;
        delete t;
        t = temp;
    }
    delete p;
}

void retira(PilhaTorres *p){

}

//Insere uma torre na pilha de torres
void insere(PilhaTorres *p, SDL_Rect rect){
    int auxX, auxY;
    Torre *t = new Torre;
    t->coord = rect;
    t->imagem1 = SDL_LoadBMP("torre2Peq.bmp");
    t->prox = p->topo;
    p->topo = t;

    //Ajusta o X e Y da torre para encaixar corretamente
    auxX = (t->coord.x - inicio_do_quadriculadoX)/t->imagem1->w;
    t->coord.x = inicio_do_quadriculadoX + t->imagem1->w * auxX;
    auxY = (t->coord.y - inicio_do_quadriculadoY)/t->imagem1->h;
    t->coord.y = inicio_do_quadriculadoY + t->imagem1->h * auxY;

    //Verifica se ja existe alguma torre nessa posicao
    for(Torre *t1 = p->topo->prox; t1!= NULL; t1 = t1->prox){
        if(t1->coord.x == t->coord.x && t1->coord.y == t->coord.y){
            t->imagem1 = SDL_LoadBMP("torre4Peq.bmp");
            //delete t1;
            t1->imagem1 = NULL;
        }
    }

}

void moveTorre(Torre *t){

}

//Imprime as torres em suas posicoes corretas
void imprime(PilhaTorres *p){
    Torre *t;
    for(t = p->topo; t!= NULL; t = t->prox){
        SDL_BlitSurface(t->imagem1, NULL, screen, &t->coord);
    }
}
