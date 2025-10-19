/*******************************************************************************/
/*                                                                             */
/* L'aplicació ECO amb sockets TCP/IP                                          */
/* Fitxer serECO.c que implementa el servidor ECO sobre la capa de             */
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
#include <signal.h>

/* Definició de constants                                                 */

/* Definició de variables globals                                         */
int sesc, scon;

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */
void aturadaS(int signal);
bool strIsEqual(char s1[], char s2[]);
void AskPort(int* port);
void Tanca(int Sck);
void exitError(int codiRes);

int main(int argc,char *argv[])
{
    /* Senyals.                                                              */
    signal(SIGINT, aturadaS);

    /* Declaració de variables, p.e., int n;                                 */
    int i;
    int bytes_llegits, bytes_escrits;
    char buffer[200];
    char iploc[16] = "0.0.0.0";
    char iprem[16];
    int portloc, portrem;

    /* Demana el port des d'on escoltar peticions a l'usuari                 */
    AskPort(&portloc);

    /* Expressions, estructures de control, crides a funcions, etc.          */

    /* Es crea el socket sesc del servidor (el socket "local")               */
    /* Amb una @IP i un #Port assignats, i ja en estat de LISTEN (escolta)   */
    if ((sesc = TCP_CreaSockServidor(iploc, portloc)) == -1) {
        Tanca(sesc);
        exitError(sesc);
    }

    for (;;) {
        /* Ara espera una petició de connexió TCP d’un client, i un cop rebuda, l'accepta.  */
        /* El descriptor de socket retornat, scon, serà un nou identificador del socket     */
        /* local per a la connexió que s'acaba d'establir. Ambdós sockets tenen la mateixa  */
        /* adreça (@IP i #port TCP), però sesc és un socket “d’escolta” i scon és un socket */
        /*"connectat".                                                                      */
        if ((scon = TCP_AcceptaConnexio(sesc, iploc, &portloc)) == -1) {
            Tanca(sesc);
            exitError(scon);
        }

        /* Obté i mostra l'adreça del socket local scon que és la mateixa que sesc.        */
        if ((i = TCP_TrobaAdrSockLoc(scon, iploc, &portloc)) == -1) {
            Tanca(sesc);
            Tanca(scon);
            exitError(i);
        }
        printf("\nsocket local: IP=%s;Port=%d\n", iploc, portloc);
        /* Obté i mostra l'adreça del socket remot                                          */
        if ((i = TCP_TrobaAdrSockRem(scon, iprem, &portrem)) == -1) {
            Tanca(sesc);
            Tanca(scon);
            exitError(i);
        }
        printf("socket remot: IP=%s;Port=%d\n\n", iprem, portrem);

        /* A continuació, s'escriu a la pantalla el que rep per scon, i l'envia a C          */
        bytes_llegits = 1;
        while (bytes_llegits > 0) {
            /* rep una frase del C                                                           */
            if ((bytes_llegits = TCP_Rep(scon, buffer, 200)) == -1) {
                Tanca(sesc);
                Tanca(scon);
                exitError(bytes_llegits);
            }

            if (bytes_llegits > 0) {
                /* Si el C encara respon, es mostra la frase per pantalla                     */
                if ((bytes_escrits = write(1, buffer, bytes_llegits)) == -1) {
                    Tanca(sesc);
                    Tanca(scon);
                    exitError(bytes_escrits);
                }

                /* Es mostren els bytes rebuts                                                 */
                printf("bytes rebuts: %d\n", bytes_llegits);

                /* Es propaga l'ECO de la frase rebuda cap el C, fent servir el socket scon    */
                if ((bytes_escrits = TCP_Envia(scon, buffer, bytes_llegits)) == -1) {
                    Tanca(sesc);
                    Tanca(scon);
                    exitError(bytes_escrits);
                }
            } else {
                /* Si el C es desconnecta, el S ho detecta i torna a escoltar altres peticions  */
                printf("\nC desconnectat\n");
            }
        }
    }

    /* Finalment es tanca el socket d'escolta                                                    */
    Tanca(sesc);

    exit(0);
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* funció que alliberarà els recursos oberts quan es detecti el signal    */
/* CTRL+C.                                                                */
/*                                                                        */
/* Retorna:                                                               */
/*  void                                                                  */
void aturadaS(int signal) {
    int i;

    printf("\nS'ha detectat el senyal Control+c. Espera un moment...\n"); // informa a l'usuari per pantalla

    if (sesc >= 0) { // Si sesc està obert el tanca
        if ((i == TCP_TancaSock(sesc)) == -1) {
            exitError(i);
        }

        printf("socket d'escolta tancat correctament...\n"); // informa a l'usuari per pantalla
    }

    if (scon >= 0) { // Si scon està obert el tanca
        if ((i == TCP_TancaSock(scon)) == -1) {
            exitError(i);
        }

        printf("socket de connexió TCP tancat correctament...\n"); // informa a l'usuari per pantalla
    }

    printf("\nFI del programa.\n"); // informa a l'usuari per pantalla de què s'ha aturat l'execució del S
    
    exit(0);
}

/* funció simple que serveix per comparar dues cadenes de caràcters.      */
/*                                                                        */
/* Retorna:                                                               */
/*  true, si s1 == s2, per tant, comparació == 0                          */
/*  false, altrament, si s1 != s2, per tant, comparació != 0              */
bool strIsEqual(char s1[], char s2[]) {
	return strcmp(s1, s2) == 0;
}

/* funció que demana el port del socket a l'usuari.                       */
/*                                                                        */
/* Retorna:                                                               */
/*  void                                                                  */
void AskPort(int* port) {
    printf("Entra el port des d'on escoltar peticions:\n");
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
