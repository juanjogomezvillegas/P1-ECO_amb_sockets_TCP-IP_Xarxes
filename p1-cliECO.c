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
char* printError(int* codiRes);

int main(int argc,char *argv[])
{
    /* Declaració de variables, p.e., int n;                                 */
    int scon, i;
    int bytes_llegits, bytes_escrits;
    char buffer[200];
    char iprem[16];
    int portrem;
    char iploc[16] = "192.168.0.17";
    int portloc = 3000;

    /* Expressions, estructures de control, crides a funcions, etc.          */
    if ((scon = TCP_CreaSockClient(iploc, &portloc)) == -1) {
        TCP_TancaSock(scon);
        exit(-1);
    }

    while (!(strIsEqual(buffer, "n\n"))) {

        AskIpAddr(iprem);
        AskPort(&portrem);

        if (TCP_DemanaConnexio(scon, iprem, &portrem) == -1) {
            TCP_TancaSock(scon);
            exit(-1);
        }

        printf("Entra frases (per desconnectar-te del servidor remot entra FI):\n");

        while (!(strIsEqual(buffer, "FI\n"))) {
            if((bytes_llegits=read(0,buffer,200))==-1) {
                perror("Error en read");
                TCP_TancaSock(scon);
                exit(-1);
            }

            buffer[bytes_llegits] = '\0'; // inserció del caràcter null al buffer per poder comparar-lo

            if((bytes_escrits=TCP_Envia(scon,buffer,bytes_llegits))==-1) {
                perror("Error en write");
                close(scon);
                exit(-1);
            }

            if (!(strIsEqual(buffer, "FI\n"))) {
                if((bytes_llegits=TCP_Rep(scon,buffer,bytes_llegits))==-1) {
                    perror("Error en read ECO");
                    close(scon);
                    exit(-1);
                }
                if((bytes_escrits=write(1,buffer,bytes_llegits))==-1) {
                    perror("Error en write ECO");
                    close(scon);
                    exit(-1);
                }
            }
        }

        // demana a l'usuari si vol continuar amb la situació inicial (tornar-se a connectar a un altre servidor).
        printf("Vols tornar a connectar-te a un altre servidor (s|n)?\n");

        if((bytes_llegits=read(0,buffer,200)) == -1) {
            perror("Error en read");
            close(scon);
            exit(-1);
        }

        buffer[bytes_llegits] = '\0';
    }
    
    if(TCP_TancaSock(scon) == -1) {
        perror("Error en close");
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

char* printError(int* codiRes) {
    return "Error: %s\n", T_ObteTextRes(&codiRes);
}
