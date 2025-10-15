/**************************************************************************/
/*                                                                        */
/* Servidor TCP típic (v1,v2: socket/bind/listen/accept/read,write/close) */
/* El #port TCP del S (l'@IP és la local) està fixat en el codi           */
/* El S escolta peticions de connexió (TCP) del C i n'accepta una         */
/* El S rep una línia del C i la mostra per la pantalla                   */
/* El S tanca el socket, i per tant la connexió TCP, i s'acaba            */
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

bool strIsEqual(char s1[], char s2[]) {
	return strcmp(s1, s2) == 0;
}

void AskPort(int* port) {
    printf("Entra el port local:\n");
    scanf("%d", port);
}

int main(int argc,char *argv[])
{
 int sesc, scon, i;
 int bytes_llegits, bytes_escrits;
 char buffer[200];
 socklen_t long_adrrem;
 struct sockaddr_in adrloc, adrrem;
 char iploc[16];
 int portloc;

 /* 1) Crida socket()                                                                    */
 /* Es crea el socket TCP sesc del servidor (el socket "local"), que de moment no té     */
 /* adreça (@IP i #port TCP) assignada.                                                  */
 if((sesc=socket(AF_INET,SOCK_STREAM,0))==-1)
 {
  perror("Error en socket");
  exit(-1);
 }
 
 /* 2) Crida bind()                                                                      */
 /* Per assignar adreça a sesc (@IP i #port TCP) el programador ho pot fer de 2 maneres: */
 /*  - explícitament, és a dir, assignant uns valors concrets en el codi;                */
 /*  - implícitament, és a dir, deixant que els assigni el S.O., que en el cas de l’@IP, */
 /*    n’escull una qualsevol de la màquina, i en el cas del #port TCP, un de lliure.    */
 /* L’assignació explícita es fa amb bind(), amb els valors d’@IP i #port TCP desitjats. */
 /* L’assignació implícita també es fa amb bind(): en el cas de l’@IP, fent bind() amb   */
 /* @IP 0.0.0.0 (que vol dir qualsevol, és a dir, totes les @IP de la màquina), i en el  */
 /* cas del #port TCP, fent bind() amb #port TCP 0; assignar l’@IP 0.0.0.0 vol dir que   */
 /* el socket queda lligat a totes les @IP de la màquina i que escoltarà peticions de    */
 /* connexió TCP que arribin a qualsevol d’elles (per cert, inet_addr(“0.0.0.0”) és      */
 /* igual a la constant INADDR_ANY); el S.O., quan farà aquesta assignació implícita?    */
 /* en el cas del #port TCP, un cop fet el bind(), el S.O. haurà assignat un #port TCP   */
 /* concret, i en el cas de l’@IP, després de l’accept(), el S.O. haurà assignat una @IP */
 /* concreta de la màquina (la de la interfície d'entrada de la petició de connexió TCP).*/
 /* Aquí es fa bind amb @IP 10.0.0.3 (explícita) i #port TCP 3000 (explícita).           */
 strcpy(iploc,"0.0.0.0");
 AskPort(&portloc);

 adrloc.sin_family=AF_INET;
 adrloc.sin_port=htons(portloc);
 adrloc.sin_addr.s_addr=inet_addr(iploc);
 for(i=0;i<8;i++){adrloc.sin_zero[i]='\0';}

 if((bind(sesc,(struct sockaddr*)&adrloc, sizeof(adrloc)))==-1)
 {
  perror("Error en bind");
  close(sesc);
  exit(-1);
 }

 /* 3) Crida listen()                                                                    */
 /* Es crea una cua per emmagatzemar peticions de connexió pendents.                     */
 /* Un cop fet listen() es diu que sesc és un socket "d'escolta".                        */
 if((listen(sesc,3))==-1)
 {
 perror("Error en listen");
 close(sesc);
 exit(-1);
 }
 
 for (;;)
 {
   /* 4) Crida accept()                                                                    */
   /* Ara espera una petició de connexió TCP d’un client, i un cop rebuda, l'accepta. El   */
   /* descriptor de socket retornat, scon, serà un nou identificador del socket local per  */
   /* a la connexió que s'acaba d'establir. Ambdós sockets tenen la mateixa adreça (@IP i  */
   /* #port TCP), però sesc és un socket “d’escolta” i scon és un socket "connectat". En   */
   /* aquest cas, però, com que sesc no tenia assignada una @IP concreta, sinó qualsevol,  */
   /* és a dir, totes les de la màquina (l'@IP 0.0.0.0), scon tindrà la “mateixa” @IP,     */
   /* però “concretada” a una d’elles (el S.O. li assignarà l’@IP que hagi fet servir el   */
   /* client en la petició de connexió).                                                   */
   long_adrrem=sizeof(adrrem);
   if((scon=accept(sesc,(struct sockaddr*)&adrrem, &long_adrrem))==-1)
   {
   perror("Error en accept");
   close(sesc);
   exit(-1);
   }

   printf("\nSocket local: IP=%s;Port=%d", inet_ntoa(adrloc.sin_addr), ntohs(adrloc.sin_port));
   printf("\nSocket remot: IP=%s;Port=%d\n", inet_ntoa(adrrem.sin_addr), ntohs(adrrem.sin_port));

   /* Ara adrrem conté l'adreça del socket remot (@IP i #port TCP).                        */
   /* 5) Crida read()                                                                      */
   /* S'escriu a pantalla el que arriba pel socket connectat scon                          */
   bytes_llegits = 1;
   while (bytes_llegits > 0) {
      printf("\n");
      if((bytes_llegits=read(scon,buffer,200))==-1) // rep la seguent frase
      {
      perror("Error en read");
      close(scon);
      exit(-1);
      }

      if (bytes_llegits > 0) {
         printf("bytes rebuts: %d\n", bytes_llegits);

         
         if((bytes_escrits=write(1,buffer,bytes_llegits))==-1)
         {
         perror("Error en write");
         close(scon);
         exit(-1);
         }
         if((bytes_escrits=write(scon,buffer,bytes_llegits))==-1)
         {
         perror("Error en write ECO");
         close(scon);
         exit(-1);
         }
      } else {
         printf("C desconnectat\n");
      }
   }

   /* 6) Crida close()                                                                     */ 
   /* Es tanca el socket scon, que com que és un socket TCP, també vol dir que es tanca la */
   /* connexió TCP establerta, i es tanca el socket d'escolta sesc.                        */
   if(close(scon)==-1)
   {
   perror("Error en close");
   exit(-1);
   }
 }

 if(close(sesc)==-1)
 {
  perror("Error en close");
  exit(-1);
 }

 exit(0);
}
