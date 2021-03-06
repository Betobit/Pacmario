/*======================================= ARCHIVOS ========================================
ayuda.txt     |Texto que se muestra al dar clic en ayuda.
               Se usa en ayuda().

escenario.txt |Contiene una serie de numeros que indican la posicion del color que se
               dibujara un cubo.
               *Se puede usar escenario.src.txt como archivo auxiliar y as� poder editar el
               escenario de una manera m�s "humana", copiarlo en escenario.txt y despu�s
               remover los saltos de linea con tu editor de texto favorito.
               Cada 7 renglones es una matriz.
               Se usa en crea_contenedor().

records.dat   |Binario que almacena un arreglo de estructuras del tipo Registro.
               Se usa en guardarRegistro() e imprimeRegistro().
==========================================================================================*/

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 25
#define PROF (TAM/2)
#define PROFX 3*PROF/6
#define R 7 // Renglones de altura
#define TAM 21
#define TOP 10 // Numero maximo del top.

typedef char String [100];

typedef struct{
    int x,y;
    int color;
    int e;
    int comida;
}TCubo;

typedef struct{
    int m,r,c,color;
}TJugador;

typedef struct {
    String nombre;
    int puntos;
} Registro;

// Funciones del juego
void crea_contenedor(int x, int y,  TCubo cont[N][R][N]);
void cubo(int x, int y, int color);
void dibujaVidas(int vidas);
void girar(TCubo cubo[N][R][N], TJugador enemigos[3], TJugador *pac, int derecha);
void guardarRegistro(Registro registro);
void imprimeTiempo(int x1, int y1, int x2, int y2, clock_t inicio);
void intextxy(int x, int y, int bkcolor, String texto);
void juego(int nivel, int puntos, int vidas);
void pinta_ambiente(int nivel, int puntos, clock_t ini);
void pinta_contenedor(TCubo cont[N][R][N]);
void popup(int puntos);
int validaPosicion(TCubo contenedor[N][R][N], TJugador *jug, TJugador bots[3]);

// Funciones de portada.
void animarPac(int tam, int altura);
void atras();
void ayuda(String nombre, int x1, int y1, int x2, int y2);
void creditos();
void menu();
void ocultar();
void portada();
void imprimeRegistro();

// Variables globales.
int maxx, maxy, // Evita repetir la funcion getmaxN() cada vez que se llama en un ciclo.
    suelo[8]; // Coordenadas de la plataforma de juego, comenzado en la izquierda-arriba, izquierda-abajo ...
void *coin,
     *colina,
     *nubes,
     *pasto;

/******************************************************************************************
                             Funciones del juego
******************************************************************************************/
int main()
{
    initwindow(1024,700,"Pacmario");
    maxx = getmaxx();
    maxy = getmaxy();

    readimagefile("img/coin.gif",100,100,110,110);
    readimagefile("img/colina.gif",maxx-324,maxy-149,maxx,maxy);
    readimagefile("img/nubes.jpg", maxx/2+200, 50, maxx/2+500, 236);
    readimagefile("img/pasto.jpg",0, maxy-192, 192, maxy);

    coin  = malloc(imagesize(100,100,110,110));
    colina= malloc(imagesize(maxx-324,maxy-149,maxx,maxy));
    nubes = malloc(imagesize(maxx/2+200, 50, maxx/2+500, 236));
    pasto = malloc(imagesize(0, maxy-192, 192, maxy));

    getimage(100,100,110,110, coin);
    getimage(maxx-324,maxy-149,maxx,maxy, colina);
    getimage(maxx/2+200, 50, maxx/2+500, 236, nubes);
    getimage(0, maxy-192, 192, maxy,pasto);

    portada();
    getch();
    closegraph();
    return(0);
}
void crea_contenedor(int x, int y, TCubo cont[N][R][N])
{
    int i,j,k,
    colores[] = {
        0x3F3122, // NADA
        0x0861EB, // CAF�
        0xFFCC00, // AZUL
        0x0653C6, // PUERTA
        0x0656CE, // MAS CAF�
        0x6AB100, // VERDE 1
        0x71CC2E, // VERDE 2
        0xCECECE, // IMAGEN
        0x71CC2E, // IMAGEN
    };
    FILE *campo;
    campo = fopen("escenario.txt", "r");
    int c;
    for(i=0;i<N;i++)
        for(j=0;j<R;j++)
            for(k=0;k<N;k++)
            {
                c = fgetc(campo)-48;
                cont[i][j][k].x = x+k*TAM-i*PROFX;
                cont[i][j][k].y = y-j*TAM+i*PROF;
                cont[i][j][k].e = (c==0 || c==9)?0:1;
                cont[i][j][k].color = colores[c];
                cont[i][j][k].comida = c==9?1:0;
            }

    suelo[0] = cont[0][0][0].x+PROFX;
    suelo[1] = cont[0][0][0].y+TAM-PROF;
    suelo[2] = cont[N-1][0][0].x;
    suelo[3] = cont[N-1][0][0].y+TAM;
    suelo[4] = cont[N-1][0][N-1].x+TAM;
    suelo[5] = cont[N-1][0][N-1].y+TAM;
    suelo[6] = cont[0][0][N-1].x+TAM+PROFX;
    suelo[7] = cont[0][0][N-1].y+TAM-PROF;
}

void cubo(int x, int y, int color)
{
    int puntos[8];

    setcolor(0x503E2C);
    setfillstyle(1,color);
    //Cara frontal
    bar(x,y,x+TAM,y+TAM);
    rectangle(x,y,x+TAM,y+TAM);
    //Cara lateral
    puntos[0]= x+TAM;
    puntos[1]= y;
    puntos[2]= x+TAM;
    puntos[3]= y+TAM;
    puntos[4]= x+TAM+PROFX;
    puntos[5]= y-PROF+TAM;
    puntos[6]= x+TAM+PROFX;
    puntos[7]= y-PROF;
    fillpoly(4,puntos);
    //Cara Superior
    puntos[0]= x;
    puntos[1]= y;
    puntos[2]= x+TAM;
    puntos[3]= y;
    puntos[4]= x+TAM+PROFX;
    puntos[5]= y-PROF;
    puntos[6]= x+PROFX;
    puntos[7]= y-PROF;
    fillpoly(4,puntos);
}

void dibujaVidas(int vidas)
{
    int tam = 30, i;
    setcolor(YELLOW);
    setfillstyle(1, YELLOW);
    for(i=0; i<vidas; i++)
        pieslice(maxx-tam-tam*i, maxy-tam, 40, -40, tam/2);
}
void girar(TCubo cubo[N][R][N], TJugador enemigos[3], TJugador *pac, int derecha)
{
    int aux[N][R][N],  // Auxiliar para .existe
        auxC[N][R][N], // Auxiliar para .color
        auxCom[N][R][N], // Auxiliar para .comida
        m, r, c, auxJ; // Aux para TJugador

    if(derecha)
    {
        auxJ = pac->m;
        pac->m = (N-1)-(pac->c);
        pac->c = auxJ;

        for(m=0;m<3;m++)
        {
            auxJ = enemigos[m].m;
            enemigos[m].m = (N-1)-(enemigos[m].c);
            enemigos[m].c = auxJ;
        }

        for(m=0;m<N;m++)
            for(r=0;r<R;r++)
                for(c=0;c<N;c++)
                {
                    aux[m][r][c] = cubo[c][r][N-1-m].e;
                    auxC[m][r][c] = cubo[c][r][N-1-m].color;
                    auxCom[m][r][c] = cubo[c][r][N-1-m].comida;
                }
    } else {
        auxJ = pac->m;
        pac->m = pac->c;
        pac->c = N-1-auxJ;

        for(m=0;m<3;m++)
        {
            auxJ = enemigos[m].m;
            enemigos[m].m = enemigos[m].c;
            enemigos[m].c = N-1-auxJ;
        }
        for(m=0;m<N;m++)
            for(r=0;r<R;r++)
                for(c=0;c<N;c++)
                {
                    aux[m][r][c] = cubo[N-1-c][r][m].e;
                    auxC[m][r][c] = cubo[N-1-c][r][m].color;
                    auxCom[m][r][c] = cubo[N-1-c][r][m].comida;
                }
    }

    for(m=0;m<N;m++)
        for(r=0;r<R;r++)
            for(c=0;c<N;c++)
            {
                cubo[m][r][c].e = aux[m][r][c];
                cubo[m][r][c].color = auxC[m][r][c];
                cubo[m][r][c].comida = auxCom[m][r][c];
            }
}

void guardarRegistro(Registro registro)
{
    FILE *f;
    Registro jugadores[TOP], j = {"-------", 0};
    int i;
    String nombre = "records.dat";
    f = fopen(nombre, "rb+");
    if(f== NULL)
    {
        f = fopen(nombre, "wb");
        jugadores[0] = registro;
        for(i=1; i<TOP; i++)
            jugadores[i] = j;
        fwrite(jugadores, sizeof(Registro), TOP, f);
    } else {
        fread(jugadores, sizeof(Registro),TOP, f);
        for(i=TOP-1; i>=0; i--)
            if(registro.puntos >= jugadores[i].puntos)
            {
                jugadores[i+1] = jugadores[i];
                jugadores[i] = registro;
            }
        fseek(f, 0, SEEK_SET);
        fwrite(jugadores, sizeof(Registro), TOP, f);
    }
    fclose(f);
}

void intextxy(int x, int y, int bkcolor, String texto)
{
    int xi = x;
    int i = 0, tecla;
    char letra[2];
    settextstyle(8, HORIZ_DIR, 4);

    do
    {
        do
        {
            setbkcolor(bkcolor);
            setcolor(BLACK);
            outtextxy(xi, y, "_");
            delay(100);
            setcolor(bkcolor);
            outtextxy(xi, y, "_");
            delay(100);
        }while(!kbhit());

        tecla = getch();
        if(tecla!=13 && tecla!='\b')
        {
            letra[0] = tecla;
            letra[1] = '\0';
            setcolor(BLACK);
            outtextxy(xi, y, letra);
            xi+=textwidth(letra);
            texto[i++] = letra[0];

        } else {
            if(tecla=='\b' && i>0)
            {
                letra[0] = texto[--i];
                letra[1] = '\0';
                xi-=textwidth(letra);
                setcolor(bkcolor);
                outtextxy(xi, y, letra);
            }
        }
    }while(tecla!=13 && i<7);
    texto[i] = '\0';
}
void imprimeTiempo(int x1, int y1, int x2, int y2, clock_t inicio)
{
    clock_t actual;
    int seg;
    char texto[10];

    actual = clock();
    seg = (actual-inicio)/CLK_TCK;
    sprintf(texto, "%d:%02d", seg/60, seg%60);
    bar(x1, y1, x2,y2);
    outtextxy(x1+5, y1+5, texto);
}
void juego(int nivel, int puntos, int vidas)
{
    int derecha = 0, // Giro: 0 izquierda, 1 derecha.
        //hongoCatch = 0, // Bandera para hongo.
        np = 0, // Paginacion.
        i = 0,
        puntosNivel = 0, // Puntos de cada nivel.
        retraso = 0, // Crear un retraso en los bots.
        tecla;
    clock_t ini = clock();
    srand(time(NULL));

    TCubo contenedor[N][R][N];
    TJugador jug = {N/2+7,0,N/2,0x00ffff};
    TJugador bots[3];
    TJugador hongo = {N/2,0,0,0x0000ff};

    bots[0] = {5, 0, N-1, 0x532FFF}; // Rosa
    bots[1] = {N/2, 0, 0, 0xA9C903}; // Azul
    bots[2] = {N-1, 0, 0, 0x00ff00}; // Tortuga

    crea_contenedor(maxx/2-maxy/4,maxy/8*3,contenedor);

    setvisualpage(np);
    do
    {
        // Encender bots y salir cuando toquen al pac.
        for(i=0;i<2; i++)
        {
            // Variable para crear un retraso en los bots.
            if((retraso>10000?0:retraso++)%5==0)
            {
                if((bots[i].c - jug.c > 0) && !(contenedor[bots[i].m][bots[i].r][bots[i].c-1].e))
                    bots[i].c--;
                else if((bots[i].c - jug.c < 0) && !(contenedor[bots[i].m][bots[i].r][bots[i].c+1].e))
                    bots[i].c++;
                else if((bots[i].m - jug.m > 0) && !(contenedor[bots[i].m-1][bots[i].r][bots[i].c].e))
                    bots[i].m--;
                else if((bots[i].m - jug.m < 0) && !(contenedor[bots[i].m+1][bots[i].r][bots[i].c].e))
                    bots[i].m++;
            }

            contenedor[bots[i].m][bots[i].r][bots[i].c].e=1;
            contenedor[bots[i].m][bots[i].r][bots[i].c].color=bots[i].color;

            if(bots[i].m == jug.m && bots[i].c == jug.c )
            {
                vidas -= validaPosicion(contenedor, &jug, bots);
                break;
            }

        }

        // Validar tortuga
        if(bots[2].m == jug.m && bots[2].c == jug.c )
            vidas -= validaPosicion(contenedor, &jug, bots);
        // Enciende tortuga
        if((retraso>10000?0:retraso++)%(nivel==1?5:4)==0)
        {
            if((bots[2].m - jug.m > 0) && !(contenedor[bots[2].m-1][bots[2].r][bots[2].c].e))
                bots[2].m--;
            else if((bots[2].m - jug.m < 0)&& !(contenedor[bots[2].m+1][bots[2].r][bots[2].c].e))
                bots[2].m++;
            else if((bots[2].c - jug.c > 0) && !(contenedor[bots[2].m][bots[2].r][bots[2].c-1].e))
                bots[2].c--;
            else if((bots[2].c - jug.c < 0) && !(contenedor[bots[2].m][bots[2].r][bots[2].c+1].e))
                bots[2].c++;
        };
        // Enciende jugador y tortuga
        contenedor[jug.m][jug.r][jug.c].e = 1;
        contenedor[jug.m][jug.r][jug.c].color =0x00ffff;
        contenedor[bots[2].m][bots[2].r][bots[2].c].e = 1;
        contenedor[bots[2].m][bots[2].r][bots[2].c].color=bots[2].color;

        setactivepage(np=!np);

        pinta_contenedor(contenedor);
        pinta_ambiente(nivel, puntos+puntosNivel, ini);
        dibujaVidas(vidas);


        setvisualpage(np);
        contenedor[jug.m][jug.r][jug.c].e=0;  // Apaga jugador
        contenedor[hongo.m][hongo.r][hongo.c].e=0;  // Apaga hongo

        for(i=0; i<3; i++)
            contenedor[bots[i].m][bots[i].r][bots[i].c].e=0;  // Apaga bots

        if(kbhit()!=0)
        {
            switch(tecla = getch())
            {
                case 71: // G
                case 103: // g
                    girar(contenedor, bots, &jug, derecha = !derecha);
                    break;
                case 72: //Arriba
                    if(jug.m>0 && !(contenedor[jug.m-1][jug.r][jug.c].e))
                    {
                        if(contenedor[jug.m-1][jug.r][jug.c].comida)
                        {
                            puntosNivel+=10;
                            contenedor[jug.m-1][jug.r][jug.c].comida = 0;
                        }
                        jug.m--;
                    } else if (contenedor[jug.m-1][jug.r][jug.c].color==0x000000)
                        {
                            nivel++;
                            juego(nivel++, puntos+puntosNivel, vidas);
                        }
                    break;
                case 75: // Izquierda
                    if(jug.c>0 && !(contenedor[jug.m][jug.r][jug.c-1].e))
                    {
                        if(contenedor[jug.m][jug.r][jug.c-1].comida)
                        {
                            puntosNivel+=10;
                            contenedor[jug.m][jug.r][jug.c-1].comida = 0;
                        }
                        jug.c--;
                    }
                    break;
                case 77: // Derecha
                    if(jug.c<N-1 && !(contenedor[jug.m][jug.r][jug.c+1].e))
                    {
                        if(contenedor[jug.m][jug.r][jug.c+1].comida)
                        {
                            puntosNivel+=10;
                            contenedor[jug.m][jug.r][jug.c+1].comida = 0;
                        }
                        jug.c++;
                    }
                    break;
                case 80: // Abajo
                    if(jug.m<N-1 && !(contenedor[jug.m+1][jug.r][jug.c].e))
                    {
                        if(contenedor[jug.m+1][jug.r][jug.c].comida)
                        {
                            puntosNivel+=10;
                            contenedor[jug.m+1][jug.r][jug.c].comida = 0;
                        }
                        jug.m++;
                    }
                    break;
            }
        }
        // Cuando termine de comer, abrir la puerta del castillo
        if(puntosNivel%2130==0 && puntosNivel!=0)
            contenedor[4][0][12].color = 0x000000;
    }while(tecla!=27 && vidas>=0);

    if(tecla == 27)
        portada();
    else
        popup(puntos+puntosNivel);
}

void pinta_ambiente(int nivel, int puntos, clock_t ini)
{
    setbkcolor(0xFF9900);
    setfillstyle(1, 0xFF9900);
    settextstyle(3,HORIZ_DIR, 2);

    setcolor(WHITE);
    String niv;
    sprintf(niv, "Nivel %d", nivel);
    outtextxy(textwidth(niv), 10, niv);

    String sPuntos;
    sprintf(sPuntos, "Puntuaci�n: %d", puntos);
    outtextxy(maxx-textwidth("Puntos: 1000")*2, 10, sPuntos);

    imprimeTiempo(maxx/2, 10, maxx/2+100, 100, ini);
}

void pinta_contenedor(TCubo cont[N][R][N])
{
    int i,j,k;
    // Cielo
    setfillstyle(1, 0xFF9900);
    bar(0, 0, maxx+1, suelo[1]);
    putimage(maxx/2+200, 50, nubes, COPY_PUT);

    // Pasto
    setcolor(0x31D301);
    setfillstyle(1, 0x31D301);
    bar(0, suelo[1], maxx+1, maxy+1);
    putimage(10, suelo[1]-150, colina, COPY_PUT);

    // Zona de juego
    setcolor(0x31D301);
    setfillstyle(1, 0x2DC501);
    fillpoly(4,suelo);

    for(i=0;i<N;i++)
        for(j=0;j<R;j++)
            for(k=0;k<N;k++)
                if(cont[i][j][k].e)
                    cubo(cont[i][j][k].x,cont[i][j][k].y,cont[i][j][k].color);
                else if(cont[i][j][k].comida)
                      putimage(cont[i][j][k].x+10,cont[i][j][k].y-5,coin,OR_PUT);
}
void popup(int puntos)
{
    int x1,x2,y1,y2, i,
        nRec = 13; // Numero de rectangulos
    x1 = x2 = (maxx/2);
    y1 = y2 = (maxy/2);
    String pts;
    sprintf(pts, "%d", puntos);

    setfillstyle(1,0x00CCFF);
    for(i=0; i<nRec; x1-=15, y1-=10, x2+=15, y2+=10, i++)
    {
        bar(x1, y1, x2, y2);
        delay(15);
    }

    // �PERDISTE!
    setbkcolor(0x00CCFF);
    setcolor(0x0054D3);
    settextstyle(3, HORIZ_DIR, 6);
    outtextxy(maxx/2-textwidth("�PERDISTE!")/2, y1+textheight("P")/2, "�PERDISTE!");

    // Puntuacion
    setcolor(0x4b78F2);
    settextstyle(3, HORIZ_DIR, 3);
    outtextxy(maxx/2-textwidth("Puntuaci�n:")/2, y1+textheight("P")*4, "Puntuaci�n:");

    // Puntos
    settextstyle(3, HORIZ_DIR, 6);
    outtextxy(maxx/2-textwidth(pts)/2, maxy/2, pts);

    // Nombre
    fflush(stdin);
    String nombre;
    setfillstyle(1, 0x5ADEFF);
    bar(maxx/2-textwidth("Puntuaci�n:")/3+15, maxy/2+65, maxx/2+textwidth("Puntuacion:")/3-15, maxy/2+100);
    intextxy(maxx/2-60, maxy/2+65, 0x5ADEFF, nombre);

    Registro r = {"", 0};
    strcpy(r.nombre, nombre);
    r.puntos = puntos;
    guardarRegistro(r);
    juego(1, 0, 3);
}
int validaPosicion(TCubo contenedor[N][R][N], TJugador *jug, TJugador bots[3])
{
    int resta = 0;
    if(contenedor[N-1][0][0].e) // Contenedor girado
    {
        contenedor[bots[0].m][0][bots[0].c].e = 0;
        contenedor[bots[1].m][0][bots[1].c].e = 0;
        contenedor[bots[2].m][0][bots[2].c].e = 0;
        bots[0].m = 0;
        bots[0].c = 5;
        bots[1].m = N-1;
        bots[1].c = N/2;
        bots[2].m = N-1;
        bots[2].c = N-1;
        jug->m = N/2;
        jug->c = N/2+7;
        resta = 1;
    } else { // Contenedor de frente.
        contenedor[bots[0].m][0][bots[0].c].e = 0;
        contenedor[bots[1].m][0][bots[1].c].e = 0;
        contenedor[bots[2].m][0][bots[2].c].e = 0;
        bots[0].m = 5;
        bots[0].c = N-1;
        bots[1].m = N/2;
        bots[1].c = 0;
        bots[2].m = N-1;
        bots[2].c = 0;
        jug->m = N/2+7;
        jug->c = N/2;
        resta = 1;
    }
    return resta;
}
/******************************************************************************************
                             Funciones de la portada
******************************************************************************************/
void animarPac(int tam, int altura)
{
    int x, ang, aux = 1;

    for(x=0, ang=0; x<maxx+tam*2; x+=2, ang += aux)
    {
        if(ang==60)
            aux = -aux;
        else if(ang == 0)
            aux = abs(aux);

        setfillstyle(1,0xFF9900);
        setcolor(0xFF9900);
        bar(0, maxy-altura-tam, x, maxy-altura+tam);

        setfillstyle(1,0x00CCFF);
        setcolor(0x00CCFF);
        pieslice(x, maxy-altura, ang, 360-ang, tam);

        delay(3);
    }
}
void atras()
{
    int x1 = maxx/2-260,
        y1 = maxy/2-60,
        x2 = maxx/2-200,
        y2 = maxy/2, salir = 0, xm, ym;
    readimagefile("img/atras.gif", x1, y1, x2, y2);
    do
    {
        while(!ismouseclick(WM_LBUTTONDOWN));
        getmouseclick(WM_LBUTTONDOWN, xm, ym);
        if(xm>x1 && xm<x2 && ym>y1 && ym<y2)
            salir = 1;
    }while(!salir);
    menu();
}
void ayuda(String nombre, int x1, int y1, int x2, int y2)
{
    ocultar();
    FILE *f;
    String texto;
    int x, y;

    setbkcolor(0xFF9900);
    setcolor(BLACK);
    settextstyle(3, HORIZ_DIR, 2);

    f = fopen(nombre, "r");

    if(f == NULL)
    {
        outtextxy(x1+((x2-x1)-textwidth("No se encuentra el archivo")/2), (y1-y2)/2, "No se encontr� archivo.");
        getch();getch();
    } else {
        y = y1;
        settextstyle(3, HORIZ_DIR, 1);
        while(!feof(f))
        {
            fgets(texto, 100, f);
            y += textheight(texto);
            x = x1 + (((x2-1)-textwidth(texto))/2);
            outtextxy(x, y, texto);
        }

        // OBJETIVO Y CONTROLES
        settextstyle(2, HORIZ_DIR, 8);
        setcolor(0x660066);
        outtextxy(maxx/2-textwidth("OBJETIVO")/2, y1-textheight("A")/2, "OBJETIVO");
        setcolor(0x00FF99);
        readimagefile("img/ayuda.gif", maxx/2-216, y1+textheight("A")*6,  maxx/2+216, y1+textheight("A")*6+170);

        atras();
        fclose(f);
    }
}
void creditos()
{
    ocultar();
    readimagefile("img/creditos.gif", maxx/2-300, maxy/2-90, maxx/2+300, maxy/2+210);
    atras();
}
void dibujo()
{
    int tam = 20, // Tama�o del Pac
        altura = 140, // Altura del suelo
        i;

    // Cielo
    setfillstyle(1,0xFF9900);
    bar(0, 0, maxx+1, maxy+1);
    setbkcolor(0xFF9900);

    // Titulo
    readimagefile("img/logo.gif", maxx/2-384, 50, maxx/2+384, 250);

    // Pasto
    for(i=0;i<maxx;i+=192)
        putimage(i, maxy-192, pasto, COPY_PUT);

}
void imprimeRegistro()
{
    ocultar();
    int i;
    String nombre = "records.dat",
           indiceJ, nomJ, ptsJ;
    Registro jugadores[TOP];

    // Barra naranja
    setcolor(0x005bb6);
    setfillstyle(1, 0x0075ea);
    setbkcolor(0x0075ea);
    bar(maxx/2-130, maxy/2-60, maxx/2+150, maxy-160);
    settextstyle(4, HORIZ_DIR, 2);

    // Lee archivo
    FILE *f; f = fopen(nombre, "rb");
    if(f== NULL)
    {
        outtextxy(maxx/2-textwidth("A�n no hay registros")/2, maxy/2+40, "A�n no hay registros,");
        outtextxy(maxx/2-textwidth("s� el primero.")/2, maxy/2+textheight("A")+44, "s� el primero.");
    } else {
        fread(jugadores, sizeof(Registro),TOP, f);
        settextstyle(3, HORIZ_DIR, 2);
        outtextxy(maxx/2-textwidth("Pos      Nombre        Puntos")/2+20, maxy/2-50, "Pos      Nombre        Puntos");
        settextstyle(4, HORIZ_DIR, 2);
        for(i=0; i<TOP; i++)
        {
            sprintf(indiceJ, "%d", i+1);
            sprintf(nomJ, "%s", jugadores[i].nombre);
            sprintf(ptsJ, "%d", jugadores[i].puntos);
            setcolor(0x00ffff);
            outtextxy(maxx/2-104, maxy/2+i*20-25, indiceJ);
            outtextxy(maxx/2-textwidth(nomJ)/2, maxy/2+i*20-25, nomJ);
            outtextxy(maxx/2+70, maxy/2+i*20-25, ptsJ);
        }
    }
    atras();
    fclose(f);
}
void menu()
{
    ocultar();
    int op=-1, xm, ym, i, margin = 10;
    String opciones[] = {"Jugar", "Top 10", "Ayuda", "Acerca De"};
    setbkcolor(0x0066F4);
    setfillstyle(1, 0x0066F4);
    setcolor(0x003988);
    settextstyle(2, HORIZ_DIR, 8);

    for(i=0; i<4; i++, margin+=textheight("A")*2)
    {
        bar(maxx/2-textwidth(opciones[3])/2-5, maxy/2+margin-10, maxx/2+textwidth(opciones[3])/2+5, maxy/2+margin+textheight(opciones[3])+10);
        outtextxy(maxx/2-textwidth(opciones[i])/2, maxy/2+margin, opciones[i]);
    }

    do
    {
        while(!ismouseclick(WM_LBUTTONDOWN));
        getmouseclick(WM_LBUTTONDOWN, xm, ym);
        margin = 10;
        for(i=0; i<4; i++, margin+=textheight("A")*2)
            if(xm>maxx/2-textwidth(opciones[3])/2-5
               && xm<maxx/2+textwidth(opciones[0])/2-10
               && ym>maxy/2+margin-10
               && ym<maxy/2+margin+textheight(opciones[3])+10)
                {
                    op = i;
                    break;
                }
    }while(op==-1);
    switch(op)
    {
        case 0:
            juego(1, 0, 3);
            break;
        case 1:
            imprimeRegistro();
            break;
        case 2:
            ayuda("ayuda.txt", 0, maxy/2-100, maxx, maxy);
            break;
        case 3:
            creditos();
            break;
    }
}
void ocultar()
{
    // Dibujar una barra para ocultar elementos anteriores
    setfillstyle(1, 0xFF9900);
    bar(0, maxy/2-140, maxx, maxy-128);
}
void portada()
{
    int tam = 20, // Tama�o del Pac
        altura = 140; // Altura del suelo

    dibujo();
    menu();

    /*while(kbhit()==0)
    {
        time_t tiempo = time(NULL);
        struct tm *tm;
        tm = localtime (&tiempo);
        int seg = tm->tm_sec;
        if(seg == 15 || seg == 45 || seg == 30 || seg == 0)
        {
            animarPac(tam, altura);;
        }
        delay(200);
    }*/
}
