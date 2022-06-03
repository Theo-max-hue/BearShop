#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 6001
#define MAX_BUFFER 2000

const char *EXIT = "exit";

void saisirCommande(char tampon[]) {
    printf("Veuillez saisir votre commande :\n");
    fgets(tampon, MAX_BUFFER, stdin);
    strtok(tampon, "\n");
}

int testQuitter(char tampon[]) {
    return strcmp(tampon, EXIT) == 0;
}


int main(int argc , char const *argv[]) {
    int fdSocket;
    int nbRecu;
    struct sockaddr_in coordonneesServeur;
    int longueurAdresse;
    char tampon[MAX_BUFFER];

    fdSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (fdSocket < 0) {
        printf("socket incorrecte\n");
        exit(EXIT_FAILURE);
    }

    // On prépare les coordonnées du serveur
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);

    // connexion de type TCP
    coordonneesServeur.sin_family = PF_INET;
    // adresse du serveur
    inet_aton("127.0.0.1", &coordonneesServeur.sin_addr);
    // le port d'écoute du serveur
    coordonneesServeur.sin_port = htons(PORT);

    if (connect(fdSocket, (struct sockaddr *) &coordonneesServeur, sizeof(coordonneesServeur)) == -1) {
        printf("connexion impossible\n");
        exit(EXIT_FAILURE);
    }

    printf("connexion ok\n");

    //récupération de l'affichage de l'acceuil
    nbRecu = recv(fdSocket, tampon, MAX_BUFFER, 0);
    //affichage de l'acceuil
    if (nbRecu > 0) {
        tampon[nbRecu] = 0;
        printf("%s\n", tampon);
    }

    //envoi de la commande au serveur
    saisirCommande(tampon);
    send(fdSocket, tampon, strlen(tampon), 0);

    //récup msg quantité
    nbRecu = recv(fdSocket, tampon, MAX_BUFFER, 0);
    //affichage du msg
    if (nbRecu > 0) {
        tampon[nbRecu] = 0;
        printf("%s\n", tampon);
    }

    //Envoi de la quantité
    fgets(tampon, MAX_BUFFER, stdin);
    strtok(tampon, "\n");
    send(fdSocket, tampon, strlen(tampon), 0);

    //récup msg facture
    nbRecu = recv(fdSocket, tampon, MAX_BUFFER, 0);
    //affichage du msg
    if (nbRecu > 0) {
        tampon[nbRecu] = 0;
        printf("%s\n", tampon);
    }


    //fermeture
    close(fdSocket);
    return EXIT_SUCCESS;
}
/*
    while (1) {
        lireMessage(tampon);

        if (testQuitter(tampon)) {
            send(fdSocket, tampon, strlen(tampon), 0);
            break; // on quitte la boucle
        }

        // on envoie le message au serveur
        send(fdSocket, tampon, strlen(tampon), 0);

        // on attend la réponse du serveur
        nbRecu = recv(fdSocket, tampon, MAX_BUFFER, 0);

        if (nbRecu > 0) {
            tampon[nbRecu] = 0;
            printf("Recu : %s\n", tampon);

            if (testQuitter(tampon)) {
                break; // on quitte la boucle
            }
        }
    }


}

*/