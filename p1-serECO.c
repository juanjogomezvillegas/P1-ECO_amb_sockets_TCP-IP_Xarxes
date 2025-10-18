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

/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

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
    int i, sesc, scon;
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

        /* Obté i mostra l'adreça del socket local                                          */
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
void aturadaS(int signal) {
    printf("\nS'ha detectat el senyal Control+c. Espera un moment...\n");

    printf("\nFI del programa.\n");
    
    exit(0);
}

bool strIsEqual(char s1[], char s2[]) {
	return strcmp(s1, s2) == 0;
}

void AskPort(int* port) {
    printf("Entra el port des d'on escoltar peticions:\n");
    scanf("%d", port);
}

void Tanca(int Sck) {
    int i;

    if ((i = TCP_TancaSock(Sck)) == -1) {
        exitError(i);
    }
}

void exitError(int codiRes) {
    printf("Error: %s\n", T_ObteTextRes(&codiRes));
    exit(-1);
}
