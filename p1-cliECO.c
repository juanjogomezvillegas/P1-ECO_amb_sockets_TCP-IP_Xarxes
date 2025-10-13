/**************************************************************************/
/*                                                                        */
/* L'aplicació ECO amb sockets TCP/IP                                     */
/* Fitxer cliECO.c que implementa el client ECO sobre la capa de          */
/* transport TCP (fent crides a la "nova" interfície de la capa de        */
/* transport o "nova" interfície de sockets).                             */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <stdio.h> o #include "meu.h"     */

#include "p1-tTCP.h"
#include <stdbool.h>

/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */
bool strIsEqual(char s1[], char s2[]);
void AskIpAddr(char* ip);
void AskPort(int* port);

int main(int argc,char *argv[])
{
    /* Declaració de variables, p.e., int n;                                 */
    int scon, i;
    int bytes_llegits, bytes_escrits;
    char buffer[200];
    char iprem[16];
    int portrem;

    /* Expressions, estructures de control, crides a funcions, etc.          */
    AskIpAddr(iprem);
    AskPort(&portrem);

    if ((scon = TCP_CreaSockClient(iprem, portrem)) == -1) {
        exit(-1);
    }

    exit(0);
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/*int FuncioInterna(arg1, arg2...)
{
	
} */
bool strIsEqual(char s1[], char s2[]) {
	return strcmp(s1, s2) == 0;
}

void AskIpAddr(char* ip) {
    printf("Entra l'@IP remota:\n");
    scanf("%15s", ip);
}

void AskPort(int* port) {
    printf("Entra el port remot:\n");
    scanf("%d", port);
}
