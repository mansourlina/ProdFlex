#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

// Structure for raw material
typedef struct {
    char nom[MAX_NAME_LENGTH];
    float prix;
    int quantite;
} MatierePremiere;

// Structure for product
typedef struct {
    char nom[MAX_NAME_LENGTH];
    float prix;
    float prix_vente;
    MatierePremiere* matieres;
    int nb_matieres;
    int temps_execution;
    int quantite;
} Produit;

// Node structure for the queue
typedef struct Noeud {
    Produit val;
    struct Noeud* suiv;
} Noeud;

// Queue structure

typedef struct {
    char nomClient[MAX_NAME_LENGTH];
    Produit produit; // Suppose que 'Produit' est d�j� d�fini dans votre code
    int quantite;
    float prixTotal;
    struct Commande* suiv; // Pour g�rer une liste cha�n�e de commandes
} Commande;

typedef struct {
    Noeud* tete;  // Liste chaînée de produits
    Noeud* queue; // Queue of products
    Commande* commandes; // Liste chaînée de commandes (ajouté ici)
} File;
// Function to initialize the queue
void initialiserFile(File* file) {
    file->tete = NULL;
    file->queue = NULL;
}

// Check if the queue is empty
int estVide(File* file) {
    return file->tete == NULL;
}

// Enqueue a product
void enfiler(File* file, Produit p) {
    Noeud* nouveau = (Noeud*)malloc(sizeof(Noeud));
    nouveau->val = p;
    nouveau->suiv = NULL;

    if (file->queue == NULL) {
        file->tete = nouveau;
        file->queue = nouveau;
    } else {
        file->queue->suiv = nouveau;
        file->queue = nouveau;
    }
}

// Dequeue a product
Produit defiler(File* file) {
    Noeud* temp;
    Produit produit;

    if (estVide(file)) {
        printf("La file est vide!\n");
        produit.quantite = -1; // Indicate an invalid product
        return produit;
    }

    temp = file->tete;
    produit = temp->val;

    if (file->tete == file->queue) {
        file->tete = NULL;
        file->queue = NULL;
    } else {
        file->tete = file->tete->suiv;
    }

    free(temp);
    return produit;
}

// Display all products in the queue
void afficherFile(File* file) {
    printf("\n=== Liste des produits dans la file ===\n");
    Noeud* courant = file->tete;
    int i = 1;

    while (courant) {
        printf("\nProduit %d :\n", i++);
        printf("Nom : %s\n", courant->val.nom);
        printf("Co�t total : %.2f\n", courant->val.prix);
        printf("Prix de vente : %.2f\n", courant->val.prix_vente);
        printf("Temps d'ex�cution : %d minutes\n", courant->val.temps_execution);
        printf("Quantit� produite : %d\n", courant->val.quantite);
        printf("Mati�res premi�res utilis�es :\n");
        for (int j = 0; j < courant->val.nb_matieres; j++) {
            printf("- %s : %.2f � %d = %.2f\n",
                   courant->val.matieres[j].nom,
                   courant->val.matieres[j].prix,
                   courant->val.matieres[j].quantite,
                   courant->val.matieres[j].prix * courant->val.matieres[j].quantite);
        }
        courant = courant->suiv;
    }
}
// Calculate the total cost of raw materials
float calcul_prix(MatierePremiere* matieres, int nb_matieres) {
    float prix_total = 0.0;
    for (int i = 0; i < nb_matieres; i++) {
        prix_total += matieres[i].prix * matieres[i].quantite;
    }
    return prix_total;
}

// Add products to the queue
void Production(File* file, float gain) {
    int n;
    printf("Combien de produits voulez-vous ajouter ? ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        Produit p;
        printf("\nProduit %d :\n", i + 1);
        printf("Nom du produit : ");
        scanf(" %s", p.nom);

        printf("Combien de mati�res contient le produit ? ");
        scanf("%d", &p.nb_matieres);
        p.matieres = (MatierePremiere*)malloc(p.nb_matieres * sizeof(MatierePremiere));

        for (int j = 0; j < p.nb_matieres; j++) {
            printf("\nMati�re %d :\n", j + 1);
            printf("Nom : ");
            scanf(" %s", p.matieres[j].nom);
            printf("Prix unitaire : ");
            scanf("%f", &p.matieres[j].prix);
            printf("Quantit� utilis�e : ");
            scanf("%d", &p.matieres[j].quantite);
        }

        p.prix = calcul_prix(p.matieres, p.nb_matieres);
        p.prix_vente = p.prix + (p.prix * gain / 100);

        printf("Temps d'ex�cution (en minutes) : ");
        scanf("%d", &p.temps_execution);
        printf("Quantit� produite : ");
        scanf("%d", &p.quantite);

        enfiler(file, p);
    }
}

// Buy a product by reducing its quantity
void acheterProduit(File* file, const char* nomProduit) {
    Noeud* courant = file->tete;

    while (courant) {
        if (strcmp(courant->val.nom, nomProduit) == 0) {
            if (courant->val.quantite > 0) {
                courant->val.quantite--;
                setColor(GREEN);
                printf("\nProduit '%s' achet� avec succ�s! Nouvelle quantit� : %d\n",
                       courant->val.nom, courant->val.quantite);
                resetColor();

                if (courant->val.quantite == 0) {
                    printf("Rupture de stock pour le produit '%s'.\n", courant->val.nom);
                }
            } else {
                printf("\nProduit '%s' en rupture de stock!\n", courant->val.nom);
            }
            return;
        }
        courant = courant->suiv;
    }
    printf("\nProduit '%s' non trouv� dans la file!\n", nomProduit);
}

// Free allocated memory for the queue
void libererFile(File* file) {
    while (!estVide(file)) {
        Produit p = defiler(file);
        free(p.matieres);
    }
}
void getCurrentDate(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d", t);
}

void livraison(const char* nomClient, const char* nomProduit) {
    char fichierNom[MAX_NAME_LENGTH + 10];
    snprintf(fichierNom, sizeof(fichierNom), "%s.txt", nomClient);

    FILE* fichier = fopen(fichierNom, "a");
    if (!fichier) {
        printf("Erreur lors de l'ouverture du fichier %s.\n", fichierNom);
        return;
    }

    char emplacement[MAX_NAME_LENGTH];
    char dateAchat[11];

    printf("Entrez l'emplacement pour la livraison : ");
    scanf(" %s", emplacement);
    getCurrentDate(dateAchat, sizeof(dateAchat));

    fprintf(fichier, "Produit: %s\nEmplacement: %s\nDate d'achat: %s\n\n", nomProduit, emplacement, dateAchat);
    fclose(fichier);

    printf("Livraison enregistr�e pour le client '%s' (Produit: %s).\n", nomClient, nomProduit);
}
void garantie(const char* nomProduit, const char* dateAchat) {
    // Obtenir la date actuelle
    char dateActuelle[11];
    getCurrentDate(dateActuelle, sizeof(dateActuelle));

    // Variables pour d�composer les dates
    int anneeAchat, moisAchat, jourAchat;
    int anneeActuelle, moisActuelle, jourActuelle;

    // Extraction des composants des dates
    if (sscanf(dateAchat, "%d-%d-%d", &anneeAchat, &moisAchat, &jourAchat) != 3) {
        printf("Erreur : format de date d'achat invalide. Utilisez YYYY-MM-DD.\n");
        return;
    }

    if (sscanf(dateActuelle, "%d-%d-%d", &anneeActuelle, &moisActuelle, &jourActuelle) != 3) {
        printf("Erreur lors de la r�cup�ration de la date actuelle.\n");
        return;
    }

    // V�rification de la garantie
    if (anneeActuelle - anneeAchat > 1 ||
        (anneeActuelle - anneeAchat == 1 &&
         (moisActuelle > moisAchat ||
          (moisActuelle == moisAchat && jourActuelle > jourAchat)))) {
        setColor(RED);
        printf("Le produit '%s' ne peut plus �tre �chang� : garantie expir�e.\n", nomProduit);
        resetColor();
    } else {
        printf("Le produit '%s' peut �tre �chang� sous garantie.\n", nomProduit);
    }
}


// Fonction pour tester la fid�lit� d'un client
void testerFidelite(const char* nomC) {
    char clientfidele[MAX_NAME_LENGTH + 10];
    snprintf(clientfidele, sizeof(clientfidele), "%s.txt", nomC);

    FILE* fichier = fopen(clientfidele, "r");
    if (!fichier) {
        printf("Aucun achat trouv� pour le client '%s'.\n", nomC);
        return;
    }

    int nbAchats = 0;
    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strncmp(ligne, "Produit:", 8) == 0) {
            nbAchats++;
        }
    }
    fclose(fichier);

    int pourcentageReduction = 0;
    if (nbAchats >= 5 && nbAchats < 10) {
        pourcentageReduction = 10;
    } else if (nbAchats >= 10 && nbAchats < 20) {
        pourcentageReduction = 20;
    } else if (nbAchats >= 20) {
        pourcentageReduction = 30;
    }

if (pourcentageReduction > 0) {
        setColor(GREEN);  // Affiche en vert pour un client fid�le
        printf("F�licitations ! En tant que client fid�le avec %d achats, vous b�n�ficiez de %d%% de r�duction sur votre prochain achat !\n",
               nbAchats, pourcentageReduction);
        resetColor();
    } else {
        setColor(YELLOW);  // Affiche en jaune si le client n'est pas encore tr�s fid�le
        printf("Vous avez effectu� %d achats. Continuez pour d�bloquer des r�ductions fid�lit� !\n", nbAchats);
        resetColor();
    }
}
void ajouterCommande(File* file, const char* nomClient, Produit produit, int quantite) {
    Commande* nouvelleCommande = (Commande*)malloc(sizeof(Commande));
    if (!nouvelleCommande) {
        printf("Erreur : Allocation de m�moire pour la commande �chou�e.\n");
        return;
    }

    strcpy(nouvelleCommande->nomClient, nomClient);
    nouvelleCommande->produit = produit;
    nouvelleCommande->quantite = quantite;
    nouvelleCommande->prixTotal = produit.prix_vente * quantite;
    nouvelleCommande->suiv = file->commandes;
    file->commandes = nouvelleCommande;
}

void sauvegarderDonnees(File* file, const char* nomFichierProduits, const char* nomFichierCommandes) {
    // Sauvegarder les produits dans un fichier distinct
    FILE* fichierProduits = fopen(nomFichierProduits, "w");
    if (!fichierProduits) {
        printf("Erreur : impossible d'ouvrir le fichier %s pour l'\u00e9criture.\n", nomFichierProduits);
        return;
    }

    Noeud* courant = file->tete;
    while (courant) {
        Produit* produit = &courant->val;

        fprintf(fichierProduits, "Produit: %s\n", produit->nom);
        fprintf(fichierProduits, "Prix: %.2f\n", produit->prix);
        fprintf(fichierProduits, "PrixVente: %.2f\n", produit->prix_vente);
        fprintf(fichierProduits, "Temps: %d\n", produit->temps_execution);
        fprintf(fichierProduits, "Quantite: %d\n", produit->quantite);

        for (int i = 0; i < produit->nb_matieres; i++) {
            fprintf(fichierProduits, "Matiere: %s %.2f %d\n",
                    produit->matieres[i].nom,
                    produit->matieres[i].prix,
                    produit->matieres[i].quantite);
        }

        fprintf(fichierProduits, "\n"); // S�parer les produits par une ligne vide
        courant = courant->suiv;
    }

    fclose(fichierProduits);
    printf("Produits sauvegard�s avec succ�s dans le fichier %s.\n", nomFichierProduits);

    // Sauvegarder les commandes dans un fichier distinct
    FILE* fichierCommandes = fopen(nomFichierCommandes, "w");
    if (!fichierCommandes) {
        printf("Erreur : impossible d'ouvrir le fichier %s pour l'\u00e9criture.\n", nomFichierCommandes);
        return;
    }

    Commande* commandeCourante = file-> commandes; // Suppose qu'il y a une liste de commandes dans File
    while (commandeCourante) {
        fprintf(fichierCommandes, "Client: %s\n", commandeCourante->nomClient);
        fprintf(fichierCommandes, "Produit: %s\n", commandeCourante->produit.nom);
        fprintf(fichierCommandes, "Quantite: %d\n", commandeCourante->quantite);
        fprintf(fichierCommandes, "Prix Total: %.2f\n", commandeCourante->prixTotal);
        fprintf(fichierCommandes, "\n"); // S�parer les commandes par une ligne vide

        commandeCourante = commandeCourante->suiv;
    }

    fclose(fichierCommandes);
    printf("Commandes sauvegard�es avec succ�s dans le fichier %s.\n", nomFichierCommandes);
}


// D�finitions des couleurs
void statistiquesVentes(File* file) {
    if (estVide(file)) {
        printf("Aucun produit dans la file pour générer des statistiques.\n");
        return;
    }

    Produit* produitPlusVendu = NULL;
    int totalQuantiteVendue = 0;
    float totalVentes = 0.0;

    Noeud* courant = file->tete;
    while (courant) {
        Produit* produit = &courant->val;
        int quantiteVendue = produit->quantite;
        totalQuantiteVendue += quantiteVendue;
        totalVentes += (produit->prix_vente * quantiteVendue);

        if (!produitPlusVendu || quantiteVendue > produitPlusVendu->quantite) {
            produitPlusVendu = produit;
        }

        courant = courant->suiv;
    }

    printf("\n--- Statistiques des Ventes ---\n");
    printf("Quantité totale vendue : %d\n", totalQuantiteVendue);
    printf("Total des ventes : %.2f\n", totalVentes);
    if (produitPlusVendu) {
        printf("Produit le plus vendu : %s (Quantité : %d, Prix de vente : %.2f)\n",
               produitPlusVendu->nom,
               produitPlusVendu->quantite,
               produitPlusVendu->prix_vente);
    }
    printf("---------------------------------\n");
}
void gestionStocksCritiques(File* file, int seuil) {
    if (estVide(file)) {
        printf("Aucun produit dans la file pour vérifier les stocks.\n");
        return;
    }

    printf("\n--- Produits avec un stock critique (<= %d) ---\n", seuil);
    int trouve = 0;

    Noeud* courant = file->tete;
    while (courant) {
        Produit* produit = &courant->val;
        if (produit->quantite <= seuil) {
            trouve = 1;
            printf("Nom : %s\n", produit->nom);
            printf("Quantité restante : %d\n", produit->quantite);
            printf("Prix de vente : %.2f\n", produit->prix_vente);
            printf("-----------------------------\n");
        }
        courant = courant->suiv;
    }

    if (!trouve) {
        printf("Aucun produit avec un stock critique.\n");
    }
}
void printInBox(const char *text) {
    int len = strlen(text);
    int width = len + 4;

    // Bord sup�rieur
    for (int i = 0; i < width; i++) {
        printf("*");
    }
    printf("\n");

    // Ligne contenant le texte
    printf("* %s *\n", text);

    // Bord inf�rieur
    for (int i = 0; i < width; i++) {
        printf("*");
    }
    printf("\n");
}
void setColor(const char *color) {
    printf("%s", color);
}

void resetColor() {
    printf("\033[0m");
}

int main() {

    File file;

    initialiserFile(&file);

    initialiserFile(&file);
    #define MAX_COMMANDES 100
    Commande commandes[MAX_COMMANDES];
    int nbCommandes = 0;
    int choix;
    float gain;
    setColor(BLUE);
    printf("Entrez le pourcentage de gain souhait� pour le prix de vente : ");
    scanf("%f", &gain);
    resetColor();

    while (1) {
        setColor(MAGENTA);
        printf("\n--- MENU GESTION �LECTRONIQUE ---\n");
        resetColor();
        setColor(RED);
        printf("1. Ajouter un Produit\n");
        resetColor();
        setColor(YELLOW);
        printf("2. Ajouter des Mati�res Premi�res\n");
        resetColor();
        setColor(CYAN);
        printf("3. Cr�er une Commande\n");
        resetColor();
        setColor(WHITE);
        printf("4. Afficher Produits\n");
        resetColor();
        setColor(BLUE);
        printf("5. Afficher Commandes\n");
        resetColor();
        setColor(RED);
        printf("6. Tester Fid�lit� Client\n");
        resetColor();
        setColor(YELLOW);
        printf("7. Sauvegarder Donn�es\n");
        resetColor();
        setColor(MAGENTA);
        printf("8. Vérifier Garantie\n");
        resetColor();
        printf("9. Afficher Statistiques des Ventes\n");
        printf("10. Vérifier Stocks Critiques\n");
        printf("11. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        resetColor();

        switch (choix) {
            case 1: {
                // Ajouter un produit
                Production(&file, gain);
                break;
            }
            case 2: {
                // Ajouter des mati�res premi�res � un produit
                char nomProduit[MAX_NAME_LENGTH];
                printf("Entrez le nom du produit auquel ajouter des mati�res premi�res : ");
                scanf("%s", nomProduit);

                Noeud* courant = file.tete;
                while (courant && strcmp(courant->val.nom, nomProduit) != 0) {
                    courant = courant->suiv;
                }

                if (courant) {
                    Produit* produit = &courant->val;
                    int nb_matieres;
                    printf("Nombre de mati�res premi�res � ajouter : ");
                    scanf("%d", &nb_matieres);

                    produit->matieres = realloc(produit->matieres,
                        (produit->nb_matieres + nb_matieres) * sizeof(MatierePremiere));

                    for (int i = 0; i < nb_matieres; i++) {
                        MatierePremiere* matiere = &produit->matieres[produit->nb_matieres + i];
                        printf("\nMati�re %d :\n", produit->nb_matieres + i + 1);
                        printf("Nom : ");
                        scanf(" %s", matiere->nom);
                        printf("Prix unitaire : ");
                        scanf("%f", &matiere->prix);
                        printf("Quantit� utilis�e : ");
                        scanf("%d", &matiere->quantite);
                    }

                    produit->nb_matieres += nb_matieres;
                    produit->prix = calcul_prix(produit->matieres, produit->nb_matieres);
                    produit->prix_vente = produit->prix + (produit->prix * gain / 100);

                    printf("Mati�res premi�res ajout�es avec succ�s au produit '%s'.\n", produit->nom);
                } else {
                    printf("Produit '%s' introuvable.\n", nomProduit);
                }
                break;
            }
            case 3: {

                // Créer une commande
                char nomProduit[MAX_NAME_LENGTH], nomClient[MAX_NAME_LENGTH];
                printf("Entrez le nom du produit à commander : ");
                scanf("%s", nomProduit);
                printf("Entrez le nom du client : ");
                scanf("%s", nomClient);

                Noeud* courant = file.tete;
                while (courant && strcmp(courant->val.nom, nomProduit) != 0) {
                    courant = courant->suiv;
                }

                if (courant) {
                    Produit* produit = &courant->val;
                    if (produit->quantite > 0) {
                        produit->quantite--;

                        // Ajouter la commande à la liste des commandes
                        ajouterCommande(&file, nomClient, *produit, 1); // 1 est la quantité de la commande (ajustez si nécessaire)

                        livraison(nomClient, produit->nom);
                        printf("Commande pour le produit '%s' créée avec succès pour le client '%s'.\n", produit->nom, nomClient);
                    } else {
                        printf("Rupture de stock pour le produit '%s'.\n", produit->nom);
                    }
                } else {
                    printf("Produit '%s' introuvable.\n", nomProduit);
                }
                break;
            }

            case 4: {
                // Afficher les produits
                afficherFile(&file);
                break;
            }
            case 5: {
                // Afficher les commandes
                printf("Commandes non encore sauvegard�es dans une liste s�par�e.\n");
                break;
            }
            case 6: {
                // Tester fid�lit� client
                char nomClient[MAX_NAME_LENGTH];
                printf("Entrez le nom du client : ");
                scanf("%s", nomClient);
                testerFidelite(nomClient);
                break;
            }
            case 7: {
                char fichierProduits[MAX_NAME_LENGTH];
                char fichierCommandes[MAX_NAME_LENGTH];

                printf("Entrez le nom du fichier pour les produits : ");
                scanf("%s", fichierProduits);
                printf("Entrez le nom du fichier pour les commandes : ");
                scanf("%s", fichierCommandes);
                sauvegarderDonnees(&file, fichierProduits, fichierCommandes);
                break;
            }
            case 8: {
                // Vérifier la garantie
                char nomProduit[MAX_NAME_LENGTH];
                char dateAchat[11]; // Format YYYY-MM-DD

                printf("Entrez le nom du produit : ");
                scanf(" %s", nomProduit);

                printf("Entrez la date d'achat (format YYYY-MM-DD) : ");
                scanf(" %s", dateAchat);

                garantie(nomProduit, dateAchat); // Appel à la fonction garantie
                break;
            }

            case 9: {
                statistiquesVentes(&file);
                break;
            }
            case 10: {
                int seuil;
                printf("Entrez le seuil de stock critique : ");
                scanf("%d", &seuil);
                gestionStocksCritiques(&file, seuil);
                break;
}
            case 11:
                // Quitter le programme
                libererFile(&file);
                printf("Programme termin�.\n");
                return 0;
            default:
                printf("Choix invalide. Veuillez r�essayer.\n");
        }
    }
}


