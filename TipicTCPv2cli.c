/**************************************************************************/
/*                                                                        */
/* Client TCP típic (v2: socket/sí fa bind/connect/read,write/close)      */
/* L'@socket del S (@IP,#portTCP) està fixada en el codi                  */
/* El C es connecta al S (s'estableix una connexió TCP)                   */
/* L'usuari entra una línia via teclat, i el C l'envia al S               */
/* El client tanca el socket, i per tant la connexió TCP, i s'acaba       */
/*                                                                        */
/* Codi extret de "La interfície de sockets de C a UNIX", al Moodle       */
/*                                                                        */
/**************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

void AskIpAddr(char* ip) {
    printf("Entra l'@IP remota:\n");
    scanf("%15s", ip);
}

void AskPort(int* port) {
    printf("Entra el port remot:\n");
    scanf("%d", port);
}

bool strIsEqual(char s1[], char s2[]) {
	return strcmp(s1, s2) == 0;
}

int main(int argc,char *argv[])
{
 int scon, i;
 int bytes_llegits, bytes_escrits;
 char buffer[200];
 struct sockaddr_in adrloc, adrrem;
 char iploc[16], iprem[16];
 int portloc, portrem;
 
 /* 1) Crida socket()                                                                    */
 /* Es crea el socket TCP scon del client (el socket "local"), que de moment no té       */
 /* adreça (@IP i #port TCP) assignada.                                                  */
 if((scon=socket(AF_INET,SOCK_STREAM,0))==-1)
 {
  perror("Error en socket");
  exit(-1);
 }

 /* 2) Crida bind(), que en el client és opcional                                        */
 /* Per assignar adreça a scon (@IP i #port TCP) el programador ho pot fer de 2 maneres: */
 /*  - explícitament, és a dir, assignant uns valors concrets en el codi;                */
 /*  - implícitament, és a dir, deixant que els assigni el S.O., que en el cas de l’@IP, */
 /*    n’escull una qualsevol de la màquina, i en el cas del #port TCP, un de lliure.    */
 /* L’assignació explícita es fa amb bind(), amb els valors d’@IP i #port TCP desitjats. */
 /* L’assignació implícita es pot fer de dues maneres:                                   */
 /*  - fent bind(): en el cas de l’@IP, fent bind() amb @IP 0.0.0.0 (que vol dir         */
 /*    qualsevol, és a dir, totes les @IP de la màquina), i en el cas del #port TCP,     */
 /*    fent bind() amb #port TCP 0; el S.O., quan farà aquesta assignació implícita?     */
 /*    en el cas del #port TCP, un cop fet el bind(), el S.O. haurà assignat un #port    */
 /*    TCP concret, i en el cas de l’@IP, després del connect(), el S.O. haurà assignat  */
 /*    una @IP concreta de la màquina (la de la interfície de sortida de la petició de   */
 /*    connexió TCP);                                                                    */
 /*  - no fent bind(): en el client, per tant, bind() és opcional; el S.O., quan farà    */
 /*    aquesta assignació implícita? després del connect(), el S.O. haurà assignat un    */
 /*    #port TCP concret i una @IP concreta (la de la interfície de sortida de la        */
 /*    petició de connexió TCP).                                                         */
 /* Aquí es fa bind amb @IP 10.0.0.2 (explícita) i #port TCP 2000 (explícita).           */
 /*strcpy(iploc,"0.0.0.0");
 portloc = 2000;

 adrloc.sin_family=AF_INET;
 adrloc.sin_port=htons(portloc);
 adrloc.sin_addr.s_addr=inet_addr(iploc);
 for(i=0;i<8;i++){adrloc.sin_zero[i]='\0';}

 if((bind(scon,(struct sockaddr*)&adrloc,sizeof(adrloc)))==-1)
 {
  perror("Error en bind");
  close(scon);
  exit(-1);
 }*/
 
 while (!(strIsEqual(buffer, "n\n"))) {

    /* 3) Crida connect()                                                                   */
    /* Es connecta scon al socket del servidor (el socket “remot”). Primer s’omple adrrem   */
    /* amb l’adreça del socket remot (@IP i #port TCP) i després es fa connect(). A més,    */
    /* com que abans no s’ha fet bind(), després de connect() el S.O. assignarà a scon una  */
    /* adreça (@IP i #port TCP; @IP de la interfície de sortida i un #port TCP lliure).     */
    /* L'adreca del socket remot és @IP 10.0.0.3 i #port TCP 3000.                          */

    AskIpAddr(iprem);
    AskPort(&portrem);

    adrrem.sin_family=AF_INET;
    adrrem.sin_port=htons(portrem);
    adrrem.sin_addr.s_addr= inet_addr(iprem);
    for(i=0;i<8;i++){adrrem.sin_zero[i]='\0';}

    if((connect(scon, (struct sockaddr*)&adrrem, sizeof(adrrem)))==-1)
    {
    perror("Error en connect");
    close(scon);
    exit(-1);
    }

    /* Un cop fet connect() es diu que el socket scon està "connectat" al socket remot.     */
    /* Com que és un socket TCP això també vol dir que s'ha establert una connexió TCP.     */

    printf("Entra frases (per desconnectar-te del servidor remot entra FI):\n");

    /* 4) Crida write()                                                                     */ 
    /* S'envia pel socket connectat scon el que es llegeix del teclat                       */
    while (!(strIsEqual(buffer, "FI\n"))) {
        if((bytes_llegits=read(0,buffer,200))==-1) // lectura de la seguent frase.
        {
        perror("Error en read");
        close(scon);
        exit(-1);
        }

        buffer[bytes_llegits] = '\0'; // inserció del caràcter null al buffer per poder comparar-lo

        if((bytes_escrits=write(scon,buffer,bytes_llegits))==-1)
        {
        perror("Error en write");
        close(scon);
        exit(-1);
        }

        if (!(strIsEqual(buffer, "FI\n"))) {
            if((bytes_llegits=read(scon,buffer,bytes_llegits))==-1)
            {
            perror("Error en read ECO");
            close(scon);
            exit(-1);
            }
            if((bytes_escrits=write(1,buffer,bytes_llegits))==-1)
            {
            perror("Error en write ECO");
            close(scon);
            exit(-1);
            }
        }
    }

    // demana a l'usuari si vol continuar amb la situació inicial (tornar-se a connectar a un altre servidor).
    printf("Vols tornar a connectar-te a un altre servidor (s|n)?\n");

    if((bytes_llegits=read(0,buffer,200))==-1)
    {
    perror("Error en read");
    close(scon);
    exit(-1);
    }

    buffer[bytes_llegits] = '\0';
 }

 /* 5) Crida close()                                                                     */ 
 /* Es tanca el socket scon, que com que és un socket TCP, també vol dir que es tanca la */
 /* connexió TCP establerta.                                                             */
 if(close(scon)==-1)
 {
  perror("Error en close");
  exit(-1);
 }

 exit(0);
}
