/*******************************************************************************/
/*                                                                             */
/* L'aplicació ECO amb sockets TCP/IP                                          */
/* Fitxer cliECO.c que implementa el client ECO sobre la capa de               */
/* transport TCP (fent crides a la "nova" interfície de la capa de             */
/* transport o "nova" interfície de sockets).                                  */
/*                                                                             */
/* Autors: Juan José Gómez Villegas, u1987338, u1987338@campus.udg.edu, GEINF  */
/*         Martí Valverde Rodríguez, u1994928, u1994928@campus.udg.edu, GEINF  */
/* Data: 10 d'octubre de 2025                                                  */
/*                                                                             */
/*******************************************************************************/

/* Inclusió de llibreries, p.e. #include <stdio.h> o #include "meu.h"     */

#include "p1-tTCP.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Definició de constants, p.e.,                                          */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */
bool strIsEqual(char s1[], char s2[]);
void AskIpAddr(char* ip);
void AskPort(int* port);
void Tanca(int Sck);
void exitError(int codiRes);

int main(int argc,char *argv[])
{
    /* Declaració de variables, p.e., int n;                                 */
    bool errorS;
    int i, scon;
    int bytes_llegits, bytes_escrits;
    char buffer[200];
    char iprem[16];
    int portrem;
    char iploc[16] = "0.0.0.0";
    int portloc = 0;

    /* Expressions, estructures de control, crides a funcions, etc.          */

    /* Es crea el socket scon del client (el socket "local")                 */
    /* Amb una @IP i un #Port assignats.                                     */
    if ((scon = TCP_CreaSockClient(iploc, portloc)) == -1) {
        exitError(scon);
    }

    while (!(strIsEqual(buffer, "n\n"))) {

        /* Demana a l'usuari l'adreça IP i Port del servidor al que es vol connectar.  */
        AskIpAddr(iprem);
        AskPort(&portrem);

        /* Es connecta scon al socket del servidor (el socket “remot”).                */
        if ((i = TCP_DemanaConnexio(scon, iprem, portrem)) == -1) {
            Tanca(scon);
            exitError(i);
        }

        /* Obté i mostra l'adreça del socket local                                          */
        if ((i = TCP_TrobaAdrSockLoc(scon, iploc, &portloc)) == -1) {
            Tanca(scon);
            exitError(i);
        }
        printf("\nsocket local: IP=%s;Port=%d\n", iploc, portloc);
        /* Obté i mostra l'adreça del socket remot                                          */
        if ((i = TCP_TrobaAdrSockRem(scon, iprem, &portrem)) == -1) {
            Tanca(scon);
            exitError(i);
        }
        printf("socket remot: IP=%s;Port=%d\n", iprem, portrem);

        /* Un cop s'ha establert la connexió TCP entre el C i el S, l'usuari entra frases   */
        /* que s'enviaran al S, i després el C rebrà l'ECO que es mostrarà per pantalla.    */
        /* L'entrada de frases i la connexió amb el S acabarà amb la paraula: FI.           */
        printf("\nEntra frases (per desconnectar-te del servidor remot entra FI):\n");

        errorS = false;
        while (!(strIsEqual(buffer, "FI\n")) && !(errorS)) {
            /* L'usuari entra una frase per pantalla.                                        */
            if ((bytes_llegits = read(0, buffer, 200)) == -1) {
                Tanca(scon);
                exitError(bytes_llegits);
            }

            buffer[bytes_llegits] = '\0'; // inserció del caràcter null al buffer per poder comparar-lo.

            if (!(strIsEqual(buffer, "FI\n"))) { // si l'usuari no ha entrat FI
                /* El C envia la frase al S, a través del socket scon.                        */
                if ((bytes_escrits = TCP_Envia(scon, buffer, bytes_llegits)) == -1) {
                    Tanca(scon);
                    exitError(bytes_escrits);
                }

                /* Es mostren els bytes enviats.                                               */
                printf("bytes enviats: %d\n", bytes_escrits);
                
                /* A continuació, el C rep l'ECO del S i el mostra per pantalla.               */
                if ((bytes_llegits = TCP_Rep(scon, buffer, bytes_escrits)) == -1) {
                    Tanca(scon);
                    exitError(bytes_llegits);
                } else if (bytes_llegits == 0) { // Si el C rep la desconnexió del servidor surt del bucle.
                    errorS = true;
                    Tanca(scon);
                    printf("\nS desconnectat\n");
                }

                if ((bytes_escrits = write(1, buffer, bytes_llegits)) == -1) {
                    Tanca(scon);
                    exitError(bytes_escrits);
                }
            } else { // Si l'usuari ha entrat FI, el C es desconnecta (surt del bucle).
                Tanca(scon);
            }
        }

        /* Demana a l'usuari si vol continuar amb la situació inicial, és a dir,                 */
        /* si es vol tornar a connectar a un altre servidor. L'usuari pot respondre:             */
        /*    s -> l'usuari vol tornar-se a connnectar i torna a demanar la ip i port remots.    */
        /*    n -> l'usuari no vol tornar-se a connnectar, i s'atura el programa.                */
        printf("\nVols tornar a connectar-te a un altre servidor (s|n)?\n");

        if ((bytes_llegits = read(0, buffer, 200)) == -1) {
            exitError(bytes_llegits);
        }

        buffer[bytes_llegits] = '\0'; // inserció del caràcter null al buffer per poder comparar-lo.
    }
    
    exit(0);
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* funció simple que serveix per comparar dues cadenes de caràcters.      */
/*                                                                        */
/* Retorna:                                                               */
/*  true, si s1 == s2, per tant, comparació == 0                          */
/*  false, altrament, si s1 != s2, per tant, comparació != 0              */
bool strIsEqual(char s1[], char s2[]) {
	return strcmp(s1, s2) == 0;
}

/* funció que demana l'adreça IP del socket a l'usuari.                   */
/*                                                                        */
/* Retorna:                                                               */
/*  void                                                                  */
void AskIpAddr(char* ip) {
    printf("Entra l'@IP remota:\n");
    scanf("%15s", ip);
}

/* funció que demana el port del socket a l'usuari.                       */
/*                                                                        */
/* Retorna:                                                               */
/*  void                                                                  */
void AskPort(int* port) {
    printf("Entra el port remot:\n");
    scanf("%d", port);
}

/* funció que tanca un socket entrat per paràmetre.                       */
/*                                                                        */
/* Retorna:                                                               */
/*  void                                                                  */
void Tanca(int Sck) {
    int i;

    if ((i = TCP_TancaSock(Sck)) == -1) {
        exitError(i);
    }
}

/* funció que mostra l'error recuperat amb la funció T_ObteTextRes,       */
/* de la nova llibreria de sockets TCP. I atura l'execució amb errors.    */
/*                                                                        */
/* Retorna:                                                               */
/*  void                                                                  */
void exitError(int codiRes) {
    printf("Error: %s\n", T_ObteTextRes(&codiRes));
    exit(-1);
}
