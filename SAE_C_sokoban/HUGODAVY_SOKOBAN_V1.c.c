#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#define NB_LIGNES 12
#define NB_COLONNES 12

typedef char t_Plateau[NB_LIGNES][NB_COLONNES];

void chargerPartie(t_Plateau plateau, char fichier[]);
void enregistrerPartie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau, char fichier[], int ligne, int colonne);
void afficher_entete(int compteur, char fichier[]);
void lecture(int compteur, t_Plateau plateau, char fichier[]);
void deplacer(t_Plateau plateau, char touche, int *compteur);
bool gagne(t_Plateau plateau);
void position(t_Plateau plateau, int *ligne, int *colonne);
void gerer_caisse(t_Plateau plateau, int ligne, int colonne, int deltaLigne, int deltaColonne, bool joueurSurCible, int *compteur);
bool gerer_abandon(t_Plateau plateau);
bool gerer_recommencer(t_Plateau plateau, char sauvegarde[], int *compteur);
void afficher_victoire();
int kbhit();

const char AVANCER = 'z', RECULER = 's', GAUCHE = 'q';
const char DROITE = 'd', ABANDONNER = 'x', RECOMMENCER = 'r';
const char ENREGISTRER = 'b', CONTINUER = 'c';
const int ABANDON = 0;

int main()
{
    t_Plateau grille;
    int compteur = 0;
    char touche = '\0';
    char niveau[21];
    int ligne = 0, colonne = 0;

    lecture(compteur, grille, niveau);

    afficher_entete(compteur, niveau);
    afficher_plateau(grille, niveau, ligne, colonne);

    while (touche != ABANDONNER)
    {
        while (kbhit() == 0)
        {
        }
        touche = getchar();

        if (touche == ABANDONNER)
        {
            if (gerer_abandon(grille))
            {
                return 0;
            }
        }

        position(grille, &ligne, &colonne);

        if (touche == AVANCER || touche == RECULER ||
            touche == GAUCHE || touche == DROITE)
        {

            deplacer(grille, touche, &compteur);

            if (gagne(grille))
            {
                afficher_victoire();
                return 0;
            }
        }

        if (touche == RECOMMENCER)
        {
            if (gerer_recommencer(grille, niveau, &compteur))
            {
                ligne = 0;
                colonne = 0;
            }
        }

        afficher_entete(compteur, niveau);
        afficher_plateau(grille, niveau, ligne, colonne);
    }

    return 0;
}

bool gerer_abandon(t_Plateau plateau)
{
    char touche;
    system("clear");
    printf("Niveaux déjà enregistrés :\n");
    system("ls *.sok\n");
    printf("Voulez-vous enregistrer la partie ?\n");
    printf("b pour enregistrer | x pour quitter : ");

    do
    {
        while (kbhit() == 0)
        {
        }
        touche = getchar();

        if (touche == ENREGISTRER)
        {
            char sauvegarde[21];
            printf("\nEntrez un nom pour sauvegarder la partie (xxx.sok) : ");
            scanf("%20s", sauvegarde);
            enregistrerPartie(plateau, sauvegarde);
            printf("Partie enregistrée dans %s.\n", sauvegarde);
            return true;
        }
        else if (touche == ABANDONNER)
        {
            return true;
        }

    } while (touche != ENREGISTRER && touche != ABANDONNER);

    return false;
}

bool gerer_recommencer(t_Plateau plateau, char nomFichier[], int *compteur)
{
    char touche;

    system("clear");
    printf("Êtes-vous sûr de vouloir recommencer ?\n");
    printf("r pour recommencer ou c pour continuer : ");

    do
    {
        while (kbhit() == 0)
        {
        }
        touche = getchar();
    } while (touche != RECOMMENCER && touche != CONTINUER);

    if (touche == RECOMMENCER)
    {
        *compteur = 0;
        chargerPartie(plateau, nomFichier);
        return true;
    }

    return false;
}

void afficher_victoire()
{
    char touche = '\0';
    system("clear");
    printf("\n\n\nBravo vous avez gagné la partie !\n\n");
    printf("Appuyez sur x pour quitter.\n");

    while (touche != ABANDONNER)
    {
        while (kbhit() == 0)
        {
        }
        touche = getchar();
    }
}
void afficher_entete(int compteur, char fichier[])
{
    system("clear");
    printf("vous êtes au niveau : %s\n\n", fichier);
    printf("presser la touche q pour aller à gauche\n");
    printf("presser la touche d pour aller à droite\n");
    printf("presser la touche z pour avancer\n");
    printf("presser la touche s pour reculer\n");
    printf("presser la touche x pour abandonner\n");
    printf("presser la touche r pour recommencer le niveau\n");
    printf("\nnombre de déplacements : %d\n\n", compteur);
}

bool gagne(t_Plateau plateau)
{
    for (int i = 0; i < NB_LIGNES; i++)
    {
        for (int j = 0; j < NB_COLONNES; j++)
        {
            if (plateau[i][j] == '.' || plateau[i][j] == '+')
                return false;
        }
    }
    return true;
}

void lecture(int compteur, t_Plateau plateau, char fichier[])
{
    printf("niveaux disponibles :\n");
    system("ls *.sok");
    printf("saisir un fichier en .sok (ex: niveau1.sok) : ");
    scanf("%20s", fichier);
    afficher_entete(compteur, fichier);
    chargerPartie(plateau, fichier);
    afficher_plateau(plateau, fichier, 0, 0);
}

void position(t_Plateau plateau, int *ligne, int *colonne)
{
    *ligne = -1;
    *colonne = -1;
    for (int i = 0; i < NB_LIGNES; i++)
    {
        for (int j = 0; j < NB_COLONNES; j++)
        {
            if (plateau[i][j] == '@' || plateau[i][j] == '+')
            {
                *ligne = i;
                *colonne = j;
                return;
            }
        }
    }
}

void deplacer(t_Plateau plateau, char touche, int *compteur)
{
    int ligne, colonne;
    position(plateau, &ligne, &colonne);

    int deltaLigne = 0;
    int deltaColonne = 0;

    if (touche == 'z')
    {
        deltaLigne = -1;
    }
    else if (touche == 's')
    {
        deltaLigne = 1;
    }
    else if (touche == 'q')
    {
        deltaColonne = -1;
    }
    else if (touche == 'd')
    {
        deltaColonne = 1;
    }
    else
    {
        return;
    }

    int ligneSuivante = ligne + deltaLigne;
    int colonneSuivante = colonne + deltaColonne;

    if (ligneSuivante < 0 || ligneSuivante >= NB_LIGNES ||
        colonneSuivante < 0 || colonneSuivante >= NB_COLONNES)
    {
        return;
    }

    bool joueurSurCible = false;
    if (plateau[ligne][colonne] == '+')
    {
        joueurSurCible = true;
    }

    if (plateau[ligneSuivante][colonneSuivante] == '$' ||
        plateau[ligneSuivante][colonneSuivante] == '*')
    {
        gerer_caisse(plateau, ligne, colonne, deltaLigne, deltaColonne, joueurSurCible, compteur);
        return;
    }

    if (plateau[ligneSuivante][colonneSuivante] == ' ' ||
        plateau[ligneSuivante][colonneSuivante] == '.')
    {
        if (plateau[ligneSuivante][colonneSuivante] == '.')
        {
            plateau[ligneSuivante][colonneSuivante] = '+';
        }
        else
        {
            plateau[ligneSuivante][colonneSuivante] = '@';
        }

        if (joueurSurCible)
        {
            plateau[ligne][colonne] = '.';
        }
        else
        {
            plateau[ligne][colonne] = ' ';
        }

        (*compteur)++;
    }
}

void gerer_caisse(t_Plateau plateau, int ligne, int colonne,
                  int deltaLigne, int deltaColonne, bool joueurSurCible, int *compteur)
{
    int ligneSuivante = ligne + deltaLigne;
    int colonneSuivante = colonne + deltaColonne;

    int ligneCaisse = ligneSuivante + deltaLigne;
    int colonneCaisse = colonneSuivante + deltaColonne;

    if (ligneCaisse < 0 || ligneCaisse >= NB_LIGNES ||
        colonneCaisse < 0 || colonneCaisse >= NB_COLONNES)
    {
        return;
    }

    char caseCible = plateau[ligneCaisse][colonneCaisse];
    if (caseCible == '#' || caseCible == '$' || caseCible == '*')
    {
        return;
    }

    if (caseCible == '.')
    {
        plateau[ligneCaisse][colonneCaisse] = '*';
    }
    else
    {
        plateau[ligneCaisse][colonneCaisse] = '$';
    }

    if (plateau[ligneSuivante][colonneSuivante] == '*')
    {
        plateau[ligneSuivante][colonneSuivante] = '+';
    }
    else
    {
        plateau[ligneSuivante][colonneSuivante] = '@';
    }

    if (joueurSurCible)
    {
        plateau[ligne][colonne] = '.';
    }
    else
    {
        plateau[ligne][colonne] = ' ';
    }

    (*compteur)++;
}

void afficher_plateau(t_Plateau plateau, char fichier[], int ligne, int colonne)
{
    for (int lig = 0; lig < NB_LIGNES; lig++)
    {
        for (int col = 0; col < NB_COLONNES; col++)
        {
            char c = plateau[lig][col];
            if (c == '*')
                c = '$';
            if (c == '+')
                c = '@';
            printf("%c", c);
        }
        printf("\n");
    }
}

int kbhit()
{
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }

    return unCaractere;
}

void chargerPartie(t_Plateau plateau, char fichier[])
{
    FILE *f;
    char finDeLigne;
    f = fopen(fichier, "r");

    if (f == NULL)
    {
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    }
    else
    {
        for (int ligne = 0; ligne < NB_LIGNES; ligne++)
        {
            for (int colonne = 0; colonne < NB_COLONNES; colonne++)
            {
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

void enregistrerPartie(t_Plateau plateau, char fichier[])
{
    FILE *f;
    char finDeLigne = '\n';
    f = fopen(fichier, "w");

    for (int ligne = 0; ligne < NB_LIGNES; ligne++)
    {
        for (int colonne = 0; colonne < NB_COLONNES; colonne++)
        {
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }

    fclose(f);
}
