/**************************************************************************/
/*                                                                        */
/* L'aplicació ECO amb sockets TCP/IP                                     */
/* Fitxer serECO.c que implementa el servidor ECO sobre la capa de        */
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

int main(int argc,char *argv[])
{
    /* Declaració de variables, p.e., int n;                                 */
    int sesc, scon, i;
    int bytes_llegits, bytes_escrits;
    char buffer[200];
    char iploc[16] = "192.168.0.17";
    int portloc = 3000;

    /* Expressions, estructures de control, crides a funcions, etc.          */
    if ((sesc = TCP_CreaSockServidor(iploc, portloc)) == -1) {
        exit(-1);
    }

    for (;;) {
        if ((scon = TCP_AcceptaConnexio(sesc, iploc, portloc)) == -1) {
            exit(-1);
        }

        while (!(strIsEqual(buffer, "FI\n"))) {
            if ((bytes_llegits=TCP_Rep(scon, buffer, 200)) == -1) {
                perror("Error en read");
                exit(-1);
            }

            buffer[bytes_llegits] = '\0'; // inserció del caràcter null al buffer per poder comparar-lo
            
            if (!(strIsEqual(buffer, "FI\n"))) {
                if ((bytes_escrits = write(1, buffer, bytes_llegits)) == -1) {
                    perror("Error en write");
                    exit(-1);
                }
                if ((bytes_escrits = TCP_Envia(scon, buffer, bytes_llegits)) == -1) {
                    perror("Error en write ECO");
                    exit(-1);
                }                
            }
        }
    }

    if (TCP_TancaSock(sesc) == -1) {
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
