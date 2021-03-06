#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 6000
#define MAX_BUFFER 2000
#define MAX_CLIENTS 3
#define PCB_GO 10
#define PCB_OI 15
#define PCB_OC 20
#define GO "Gros Ourson"
#define OI "Ours Infirmière"
#define OC "Ours Cupidon"
#define MSG_STOCK "Le stock de cet article est actuellement épuisé, veuillez patienter le temps qu'un vendeur refasse le stock\n"

typedef struct {
    int stockGrosOurson;
    int prixGrosOurson;
    double tvaGrosOurson;
    int stockOursInfirmiere;
    int prixOursInfirmiere;
    double tvaOursInfirmiere;
    int stockOursCupidon;
    int prixOursCupidon;
    double tvaOursCupidon;
}Magasin;

void ouvrirMemoirePartagee();
void fermerMemoirePartagee();
void *fonctionVendeur(void *arg);

int id_memoire;
Magasin *magasin;

void initialiserMagasin(){
    magasin->stockGrosOurson = 10;
    magasin->stockOursInfirmiere = 10;
    magasin->stockOursCupidon = 10;
    magasin->prixGrosOurson = 25;
    magasin->prixOursCupidon = 69;
    magasin->prixOursInfirmiere = 15;
    magasin->tvaGrosOurson = 0.2;
    magasin->tvaOursCupidon = 0.5;
    magasin->tvaOursInfirmiere = 1.8;
}

int main(int argc, char const *argv[]) {
    ouvrirMemoirePartagee();
    initialiserMagasin();

    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordonneesServeur;
    struct sockaddr_in coordonneesAppelant;
    char tampon[MAX_BUFFER];
    int nbRecu;
    int longueurAdresse;
    int pid[MAX_CLIENTS];
    int facture;

    fdSocketAttente = socket(PF_INET, SOCK_STREAM, 0);

    pthread_t vendeur;
    pthread_create(&vendeur,NULL, fonctionVendeur, &magasin);

    if (fdSocketAttente < 0) {
        printf("socket incorrecte\n");
        exit(EXIT_FAILURE);
    }

    // On prépare l’adresse d’attachement locale
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);

    // connexion de type TCP
    coordonneesServeur.sin_family = PF_INET;
    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_addr.s_addr = htonl(INADDR_ANY);
    // le port d'écoute
    coordonneesServeur.sin_port = htons(PORT);

    if (bind(fdSocketAttente, (struct sockaddr *) &coordonneesServeur, sizeof(coordonneesServeur)) == -1) {
        printf("erreur de bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(fdSocketAttente, 5) == -1) {
        printf("erreur de listen\n");
        exit(EXIT_FAILURE);
    }

    socklen_t tailleCoord = sizeof(coordonneesAppelant);

    int nbClients = 0;

    while (nbClients < MAX_CLIENTS) {
        if ((fdSocketCommunication = accept(fdSocketAttente, (struct sockaddr *) &coordonneesAppelant,
                                            &tailleCoord)) == -1) {
            printf("erreur de accept\n");
            exit(EXIT_FAILURE);
        }
        nbClients++;
        printf("Client connecté - %s:%d\n",
               inet_ntoa(coordonneesAppelant.sin_addr),
               ntohs(coordonneesAppelant.sin_port));

        if ((pid[nbClients] = fork()) == 0) {
            close(fdSocketAttente);

            //Premier affichage a envoyer au client, acceuil
            strcpy(tampon, "______                    _____ _                 \n"
                           "| ___ \\                  /  ___| |                \n"
                           "| |_/ / ___  __ _ _ __   \\ `--.| |__   ___  _ __  \n"
                           "| ___ \\/ _ \\/ _` | '__|   `--. \\ '_ \\ / _ \\| '_ \\ \n"
                           "| |_/ /  __/ (_| | |     /\\__/ / | | | (_) | |_) |\n"
                           "\\____/ \\___|\\__,_|_|     \\____/|_| |_|\\___/| .__/ \n"
                           "                                           | |    \n"
                           "                                           |_|    \n"
                           "                      ,-._____,-.\n"
                           "                     (_c       c_)\n"
                           "                      /  e-o-e  \\\n"
                           "                     (  (._|_,)  )\n"
                           "                      >._`---'_,<\n"
                           "                    ,'/  `---'  \\`.\n"
                           "                  ,' /           \\ `.\n"
                           "                 (  (             )  )\n"
                           "                  `-'\\           /`-'\n"
                           "                     |`-._____.-'|\n"
                           "                     |     Y     |\n"
                           "                     /     |     \\\n"
                           "                    (      |      )\n"
                           "                     `-----^-----'   \n"
                           "                      Gros Ourson \n"
                           "                             \n"
                           "(\\     (\\                           ___    \n"
                           " (\"\\  _ (\"\\ _                    (o|_+_|o)    \n"
                           "((/ \\(o\\---/o)                    ( . . )      \n"
                           "((/  \\( . . )  /|                _( (Y) )_  \n"
                           "((/  _( (T) )-/-l_              / /,---.\\ \\ \n"
                           " (/ (  \\__ (_/_(__)            / / | + | \\ \\   \n"
                            "  \\/ \\    `-|   |              \\_)-""     ""-(_/   \n"
                           "  (   `---.__)--+--|>            |_______| \n"
                           "   \\    \\  \\ \\  |                _)  |  (_  \n"
                           "    )    )  ) \\_|               (___,'.___)     \n"
                           "   (    (  (                  Ours Infirmière                  \n"
                           "    \\    )  )`                                    \n"
                           "     `--^`--^                                      \n"
                           "   Ours Cupidon                            \n\n"

            );
            send(fdSocketCommunication, tampon, strlen(tampon), 0);

            bool commande = false;

            while (!commande) { //Tant que la commande n'est pas finie, on boucle

                //Demande au client ce qu'il souhaite commander
                strcpy(tampon, "Quel Article souhaitez-vous commander ?");
                send(fdSocketCommunication, tampon, strlen(tampon), 0);

                //On récup ensuite la commande que le client rentre
                nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);
                if (nbRecu > 0) {
                    tampon[nbRecu] = 0;
                    if (strcmp(tampon, GO) == 0) { //Si commande Gros Ourson
                        sprintf(tampon, "Il reste en stock : %d articles\nQuelle Quantité souhaitez-vous commander ?",
                                magasin->stockGrosOurson);
                        send(fdSocketCommunication, tampon, strlen(tampon), 0);

                        //récup quantité que le client demande
                        nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);
                        tampon[nbRecu] = 0;
                        int nbreAchats = atoi(tampon);

                        //Test si qté demandée est dispo en stock
                        if (magasin->stockGrosOurson < nbreAchats) {
                            send(fdSocketCommunication, MSG_STOCK, strlen(MSG_STOCK), 0);
                            commande = false;
                        } else {
                            magasin->stockGrosOurson -= nbreAchats;
                            //Calcul et envoi de la facture au client
                            facture = magasin->prixGrosOurson * nbreAchats;
                            int tva = facture * magasin->tvaGrosOurson;
                            facture += tva;
                            sprintf(tampon,
                                    "Merci pour votre commande !\nVotre facture est de : %d €\nAvec une TVA de : %d €",
                                    facture, tva);
                            send(fdSocketCommunication, tampon, strlen(tampon), 0);
                            commande = true;
                        }
                    } else if (strcmp(tampon, OI) == 0) { //Si commande Ours Infirmière
                        sprintf(tampon, "Il reste en stock : %d articles\nQuelle Quantité souhaitez-vous commander ?",
                                magasin->stockOursInfirmiere);
                        send(fdSocketCommunication, tampon, strlen(tampon), 0);

                        nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);
                        tampon[nbRecu] = 0;
                        int nbreAchats = atoi(tampon);

                        if (magasin->stockOursInfirmiere < nbreAchats) {
                            send(fdSocketCommunication, MSG_STOCK, strlen(MSG_STOCK), 0);
                            commande = false;
                        } else {
                            magasin->stockOursInfirmiere -= nbreAchats;
                            facture = magasin->prixOursInfirmiere * nbreAchats;
                            int tva = facture * magasin->tvaOursInfirmiere;
                            facture += tva;
                            sprintf(tampon,
                                    "Merci pour votre commande !\nVotre facture est de : %d €\nAvec une TVA de : %d €",
                                    facture, tva);
                            send(fdSocketCommunication, tampon, strlen(tampon), 0);
                            commande = true;
                        }
                    } else if (strcmp(tampon, OC) == 0) { //Si commande Ours Cupidon
                        sprintf(tampon, "Il reste en stock : %d articles\nQuelle Quantité souhaitez-vous commander ?",
                                magasin->stockOursCupidon);
                        send(fdSocketCommunication, tampon, strlen(tampon), 0);

                        nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);
                        tampon[nbRecu] = 0;
                        int nbreAchats = atoi(tampon);

                        if (magasin->stockOursCupidon < nbreAchats) {
                            send(fdSocketCommunication, MSG_STOCK, strlen(MSG_STOCK), 0);
                            commande = false;
                        } else {
                            magasin->stockOursCupidon -= nbreAchats;
                            facture = magasin->prixOursCupidon * nbreAchats;
                            int tva = facture * magasin->tvaOursCupidon;
                            facture += tva;
                            sprintf(tampon,
                                    "Merci pour votre commande !\nVotre facture est de : %d €\nAvec une TVA de : %d €",
                                    facture, tva);
                            send(fdSocketCommunication, tampon, strlen(tampon), 0);
                            commande = true;
                        }
                    } else {
                        strcpy(tampon, "Vous n'avez pas rentré un nom d'article valide\n");
                        send(fdSocketCommunication, tampon, strlen(tampon), 0);
                    }
                }
            }
            nbClients--;
            printf("Déconnexion du client %s:%d\n",
                   inet_ntoa(coordonneesAppelant.sin_addr),
                   ntohs(coordonneesAppelant.sin_port));
            fermerMemoirePartagee();
            pthread_cancel(vendeur);
            exit(EXIT_SUCCESS);
            }
        }
    close(fdSocketCommunication);
    close(fdSocketAttente);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        wait(&pid[i]);
    }

    printf("Fin du programme.\n");
    return EXIT_SUCCESS;

}

void ouvrirMemoirePartagee() {
    id_memoire = shmget(IPC_PRIVATE, sizeof(Magasin), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    magasin = (Magasin *) shmat(id_memoire, 0, 0);
}

void fermerMemoirePartagee() {
    shmdt(magasin);
    shmctl(id_memoire, IPC_RMID, 0);
}

void *fonctionVendeur(void *arg) {

    while (1) {
        sleep(10);
        magasin->stockGrosOurson += PCB_GO;
        magasin->stockOursCupidon += PCB_OC;
        magasin->stockOursInfirmiere += PCB_OI;
    }
}
