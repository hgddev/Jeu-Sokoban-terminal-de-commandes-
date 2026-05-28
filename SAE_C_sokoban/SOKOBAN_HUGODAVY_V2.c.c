#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

// Constantes fixes  lues  à la compilation
#define NB_LIGNES 12
#define NB_COLONNES 12
#define MAX_DEP 1000
//décaration des tableaux
typedef char t_Plateau[NB_LIGNES][NB_COLONNES];
typedef char t_tabDeplacement[MAX_DEP];

//décaration des fonctions//proceedures

void initialiser_partie(t_Plateau grille, t_tabDeplacement dep, int *cptdep,
                        int *compteur, char niveau[],
                        int *ligne, int *colonne, int *zoom);
void mettre_a_jour_affichage(t_Plateau grille, char niveau[], int compteur,
                             int ligne, int colonne, int zoom);
char lire_touche();
void gerer_zoom(char touche, int *zoom);
void gerer_deplacement(char touche, t_Plateau grille, int *compteur,
                       int *ligne, int *colonne,
                       t_tabDeplacement dep, int *cptdep, char niveau[]);

void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau, char fichier[], int ligne, 
    int colonne);
void afficher_entete(int compteur, char fichier[]);
void lecture(int compteur, t_Plateau plateau, char fichier[]);
void deplacer(t_Plateau plateau, char touche, int *compteur, int *ligne, 
    int *colonne, t_tabDeplacement dep, int *cptdep);
bool gagne(t_Plateau plateau);
void position(t_Plateau plateau, int *ligne, int *colonne);
void gerer_caisse(t_Plateau plateau, int *ligne, int *colonne, 
    int deltaLigne, int deltaColonne, bool joueurSurCible, 
    int *compteur, t_tabDeplacement dep, int *cptdep);
bool gerer_abandon(t_Plateau plateau, t_tabDeplacement dep, 
    int cptdep, char fic[]);
bool gerer_recommencer(t_Plateau plateau, char sauvegarde[], int *compteur);
void afficher_plateau_agrandi(t_Plateau plateau, int zoom);
void enregistrerDeplacements(t_tabDeplacement dep, int cptdep, char fic[]);
bool afficher_victoire(t_tabDeplacement dep, int cptdep, char fic[]);
void undo(t_Plateau plateau, int *ligne, int *colonne, 
    t_tabDeplacement dep, int *cptdep);
void normal_undo(t_Plateau plateau, int *ligne, int *colonne, char mouv);
void caisse_undo(t_Plateau plateau, int *ligne, int *colonne, char mouv);
int kbhit();

//décaration des Constantes

const char AVANCER = 'z', RECULER = 's', GAUCHE = 'q';
const char DROITE = 'd', ABANDONNER = 'x', RECOMMENCER = 'r';
const char ENREGISTRER = 'b', CONTINUER = 'c';
const char AUGMENTER = '+', DIMINUER = '-';
const char SOKO = '@', CAISSE = '$', CAISSECIBLE = '*';
const char SOKOCIBLE = '+', CIBLE = '.';
const char EDROITE = 'd', EGAUCHE = 'g', EAVANCER = 'h';
const char ERECULER = 'b', ECDROITE = 'D', ECGAUCHE = 'G';
const char ECAVANCER = 'H', ECRECULER = 'B';
const char ENREDEP = 'e';
const int ABANDON = 0;
const int MAX_ZOOM = 3;

int main()
{
    t_Plateau grille;
    t_tabDeplacement dep;
    int cptdep, compteur, ligne, colonne, zoom;
    char touche = '\0';
    char niveau[21];

    initialiser_partie(grille, dep, &cptdep, &compteur, niveau, 
        &ligne, &colonne, &zoom);

    while (touche != ABANDONNER)
    {
        touche = lire_touche();

        if (touche == ABANDONNER && gerer_abandon(grille, dep, cptdep, niveau))
        {
            return 0;
        }

        if (touche == 'u')
        {
            undo(grille, &ligne, &colonne, dep, &cptdep);
            if (compteur > 0)
            {
                compteur--;
            }
        }

        gerer_zoom(touche, &zoom);

        gerer_deplacement(touche, grille, &compteur, &ligne, 
            &colonne, dep, &cptdep, niveau);

        if (touche == RECOMMENCER && gerer_recommencer(grille, 
            niveau, &compteur))
        {
            position(grille, &ligne, &colonne);
        }

        mettre_a_jour_affichage(grille, niveau, compteur, 
            ligne, colonne, zoom);
    }

    return 0;
}
//initialisation de la partie 
void initialiser_partie(t_Plateau grille, t_tabDeplacement dep, int *cptdep,
                        int *compteur, char niveau[],
                        int *ligne, int *colonne, int *zoom)
{
    *cptdep = 0;
    *compteur = 0;
    *ligne = 0;
    *colonne = 0;
    *zoom = 1;

    lecture(*compteur, grille, niveau);
    afficher_entete(*compteur, niveau);
    afficher_plateau(grille, niveau, *ligne, *colonne);
    afficher_plateau_agrandi(grille, *zoom);
    position(grille, ligne, colonne);
}

//lecture touche 
char lire_touche()
{
    while (kbhit() == 0)
    {
    }
    return getchar();
}


//recuperer le sens du zoom
void gerer_zoom(char touche, int *zoom)
{
    if (touche == AUGMENTER && *zoom < MAX_ZOOM)
    {
        (*zoom)++;
    }
    else if (touche == DIMINUER && *zoom > 1)
    {
        (*zoom)--;
    }
}

//recuperer les  touches déplacements
void gerer_deplacement(char touche, t_Plateau grille, int *compteur,
                       int *ligne, int *colonne,
                       t_tabDeplacement dep, int *cptdep, char niveau[])
{
    if (touche == AVANCER || touche == RECULER || touche == GAUCHE || 
        touche == DROITE)
    {
        deplacer(grille, touche, compteur, ligne, colonne, dep, cptdep);
        if (gagne(grille))
        {
            if (afficher_victoire(dep, *cptdep, niveau))
            {
                exit(0); // quitte tout le programme immédiatement
            }
        }
    }
}


void mettre_a_jour_affichage(t_Plateau grille, char niveau[], int compteur,
                             int ligne, int colonne, int zoom)
{
    afficher_entete(compteur, niveau);
    if (zoom == 1)
    {
        afficher_plateau(grille, niveau, ligne, colonne);
    }
    else
    {
        afficher_plateau_agrandi(grille, zoom);
    }
}

//action quand abandon du joueur
bool gerer_abandon(t_Plateau plateau, t_tabDeplacement dep, 
    int cptdep, char fic[])
{
    char touche;
    char sauvegarde[21];
    char deplacement[21];
    system("clear");
    printf("Niveaux déjà enregistrés :\n");
    system("ls *.sok\n");
    printf("Voulez-vous enregistrer la partie ?\n");
    printf("b pour enregistrer le plateau | ");
    printf("e pour les deplacements | x pour quitter : \n");

    do
    {
        while (kbhit() == 0)
        {
        }
        touche = getchar();

        if (touche == ENREGISTRER)
        {
            printf("\nEntrez un nom pour sauvegarder la partie (xxx.sok) : ");
            scanf("%20s", sauvegarde);
            enregistrer_partie(plateau, sauvegarde);
            printf("Partie enregistrée dans %s\n", sauvegarde);
            return true;
        }
        else if (touche == ENREDEP)
        {
            printf("entrez un nom de sauvegarde des déplacements ");
            printf("(xxx.dep) :   \n");
            scanf("%20s", deplacement);
            enregistrerDeplacements(dep, cptdep, deplacement);
            printf("\n");
            printf("déplacements enregistrés dans %s\n", deplacement);
            return true;
        }
        else if (touche == ABANDONNER)
        {
            return true;
        }

    } while (touche != ENREGISTRER && touche != ABANDONNER);

    return false;
}


//recuperer  actions recommencer
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
        charger_partie(plateau, nomFichier);
        return true;
    }

    return false;
}
// affichage victoite
bool afficher_victoire(t_tabDeplacement dep, int cptdep, char fic[])
{
    char touche = '\0';
    char deplacement[21];
    bool res;
    system("clear");
    printf("\n\n\nBravo vous avez gagné la partie !\n\n");
    printf("Appuyez sur e pour enregistrer les deplacements ");
    printf("| x pour quitter.\n");

    do
    {
        while (kbhit() == 0)
        {
        }
        touche = getchar();

        if (touche == ENREDEP)
        {
            printf("entrez un nom de sauvegarde des déplacements "); 
            printf("(xxx.dep) :   ");
            scanf("%20s", deplacement);
            enregistrerDeplacements(dep, cptdep, deplacement);
            printf("\n");
            printf("déplacements enregistrés dans %s\n", deplacement);
            res = true;
        }
        else if (touche == ABANDONNER)
        {
            res = true;
        }

    } while (touche != ENREDEP && touche != ABANDONNER);

    return res;
}

// affichage regles du jeu
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
    printf("presser la touche u pour annuler le dernier mouvement\n");
    printf("presser la touche x pour zoomer\n");
    printf("presser la touche - pour dézoomer\n");
    printf("\nnombre de déplacements : %d\n\n", compteur);
}

bool gagne(t_Plateau plateau)
{
    for (int i = 0; i < NB_LIGNES; i++)
    {
        for (int j = 0; j < NB_COLONNES; j++)
        {
            if (plateau[i][j] == CIBLE || plateau[i][j] == SOKOCIBLE)
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
    charger_partie(plateau, fichier);
    afficher_plateau(plateau, fichier, 0, 0);
}

// recherche position joueur
void position(t_Plateau plateau, int *ligne, int *colonne)
{
    *ligne = -1;
    *colonne = -1;
    for (int i = 0; i < NB_LIGNES; i++)
    {
        for (int j = 0; j < NB_COLONNES; j++)
        {
            if (plateau[i][j] == SOKO || plateau[i][j] == SOKOCIBLE)
            {
                *ligne = i;
                *colonne = j;
                return;
            }
        }
    }
}
// deplacement classique
void deplacer(t_Plateau plateau, char touche, int *compteur, int *ligne,
              int *colonne, t_tabDeplacement dep, int *cptdep)
{
    char mouv;
    int deltaLigne = 0;
    int deltaColonne = 0;

    if (touche == AVANCER)
    {
        deltaLigne = -1;
    }
    else if (touche == RECULER)
    {
        deltaLigne = 1;
    }
    else if (touche == GAUCHE)
    {
        deltaColonne = -1;
    }
    else if (touche == DROITE)
    {
        deltaColonne = 1;
    }
    else
    {
        return;
    }

    int ligneSuivante = *ligne + deltaLigne;
    int colonneSuivante = *colonne + deltaColonne;

    if (ligneSuivante < 0 || ligneSuivante >= NB_LIGNES ||
        colonneSuivante < 0 || colonneSuivante >= NB_COLONNES)
    {
        return;
    }

    bool joueurSurCible = false;
    if (plateau[*ligne][*colonne] == SOKOCIBLE)
    {
        joueurSurCible = true;
    }

    if (plateau[ligneSuivante][colonneSuivante] == CAISSE ||
        plateau[ligneSuivante][colonneSuivante] == CAISSECIBLE)
    {

        gerer_caisse(plateau, ligne, colonne, deltaLigne, deltaColonne,
                     joueurSurCible, compteur, dep, cptdep);
        return;
    }

    if (plateau[ligneSuivante][colonneSuivante] == ' ' ||
        plateau[ligneSuivante][colonneSuivante] == CIBLE)
    {
        if (plateau[ligneSuivante][colonneSuivante] == CIBLE)
        {
            plateau[ligneSuivante][colonneSuivante] = SOKOCIBLE;
        }
        else
        {
            plateau[ligneSuivante][colonneSuivante] = SOKO;
        }

        if (joueurSurCible)
        {
            plateau[*ligne][*colonne] = CIBLE;
        }
        else
        {
            plateau[*ligne][*colonne] = ' ';
        }

        *ligne = ligneSuivante;
        *colonne = colonneSuivante;

        (*compteur)++;

        if (touche == DROITE)
        {
            mouv = EDROITE;
        }
        else if (touche == GAUCHE)
        {
            mouv = EGAUCHE;
        }
        else if (touche == AVANCER)
        {
            mouv = EAVANCER;
        }
        else
        {
            mouv = ERECULER;
        }
        dep[*cptdep] = mouv;
        (*cptdep)++;
    }
}

// deplacement lié a une caisse
void gerer_caisse(t_Plateau plateau, int *ligne, int *colonne,
                  int deltaLigne, int deltaColonne, bool joueurSurCible,
                  int *compteur, t_tabDeplacement dep, int *cptdep)
{
    char mouv;
    int ligneSuivante = *ligne + deltaLigne;
    int colonneSuivante = *colonne + deltaColonne;

    int ligneCaisse = ligneSuivante + deltaLigne;
    int colonneCaisse = colonneSuivante + deltaColonne;

    if (ligneCaisse < 0 || ligneCaisse >= NB_LIGNES ||
        colonneCaisse < 0 || colonneCaisse >= NB_COLONNES)
    {
        return;
    }

    char caseCible = plateau[ligneCaisse][colonneCaisse];
    if (caseCible == '#' || caseCible == CAISSE || caseCible == CAISSECIBLE)
    {
        return;
    }

    if (caseCible == CIBLE)
    {
        plateau[ligneCaisse][colonneCaisse] = CAISSECIBLE;
    }
    else
    {
        plateau[ligneCaisse][colonneCaisse] = CAISSE;
    }

    if (plateau[ligneSuivante][colonneSuivante] == CAISSECIBLE)
    {
        plateau[ligneSuivante][colonneSuivante] = SOKOCIBLE;
    }
    else
    {
        plateau[ligneSuivante][colonneSuivante] = SOKO;
    }

    if (joueurSurCible)
    {
        plateau[*ligne][*colonne] = CIBLE;
    }
    else
    {
        plateau[*ligne][*colonne] = ' ';
    }

    *ligne = ligneSuivante;
    *colonne = colonneSuivante;

    (*compteur)++;

    if (deltaLigne == 0)
    {
        if (deltaColonne == -1)
        {
            mouv = ECGAUCHE;
        }
        else
        {
            mouv = ECDROITE;
        }
    }
    else
    {
        if (deltaLigne == -1)
        {
            mouv = ECAVANCER;
        }
        else
        {
            mouv = ECRECULER;
        }
    }
    dep[*cptdep] = mouv;
    (*cptdep)++;
}

// affichage plateau classique
void afficher_plateau(t_Plateau plateau, char fichier[],
                      int ligne, int colonne)
{
    for (int lig = 0; lig < NB_LIGNES; lig++)
    {
        for (int col = 0; col < NB_COLONNES; col++)
        {
            char c = plateau[lig][col];
            if (c == CAISSECIBLE)
            {
                c = CAISSE;
            }
            if (c == SOKOCIBLE)
            {
                c = SOKO;
            }
            printf("%c", c);
        }
        printf("\n");
    }
}
// affichage du plateau zoomé
void afficher_plateau_agrandi(t_Plateau plateau, int zoom)
{
    int lig, col, i, j;
    char c;
    for (i = 0; i < NB_LIGNES; i++)
    {
        for (col = 0; col < zoom; col++)
        {
            for (j = 0; j < NB_COLONNES; j++)
            {
                c = plateau[i][j];

                if (c == CAISSECIBLE)
                {
                    c = CAISSE;
                }
                if (c == SOKOCIBLE)
                {
                    c = SOKO;
                }

                for (lig = 0; lig < zoom; lig++)
                {
                    printf("%c", c);
                }
            }
            printf("\n");
        }
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
// enregistrtement des deplacements
void enregistrerDeplacements(t_tabDeplacement t, int nb, char fic[])
{
    FILE *f;

    f = fopen(fic, "w");
    fwrite(t, sizeof(char), nb, f);
    fclose(f);
}
// chargement partie
void charger_partie(t_Plateau plateau, char fichier[])
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
// enregistrement de la partie
void enregistrer_partie(t_Plateau plateau, char fichier[])
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


void undo(t_Plateau plateau, int *ligne, int *colonne,
          t_tabDeplacement dep, int *cptdep)
{
    if (*cptdep <= 0)
        return;

    char mouv = dep[*cptdep - 1];
    (*cptdep)--;

    if (mouv == ECDROITE || mouv == ECGAUCHE ||
        mouv == ECAVANCER || mouv == ECRECULER)
    {
        caisse_undo(plateau, ligne, colonne, mouv);
    }
    else
    {
        normal_undo(plateau, ligne, colonne, mouv);
    }
}

// Undo pour le joueur sans caisse
void normal_undo(t_Plateau plateau, int *ligne, int *colonne, char mouv)
{
    int deplig = *ligne;
    int depcol = *colonne;
    int undoLig = deplig;
    int undoCol = depcol;

    bool joueurSurCible = (plateau[deplig][depcol] == SOKOCIBLE);

    if (mouv == EDROITE)
    {
        undoCol = depcol - 1;
    }
    else if (mouv == EGAUCHE)
    {
        undoCol = depcol + 1;
    }
    else if (mouv == EAVANCER)
    {
        undoLig = deplig + 1;
    }
    else if (mouv == ERECULER)
    {
        undoLig = deplig - 1;
    }

    if (joueurSurCible)
    {
        plateau[deplig][depcol] = CIBLE;
    }
    else
    {
        plateau[deplig][depcol] = ' ';
    }

    if (plateau[undoLig][undoCol] == CIBLE)
    {
        plateau[undoLig][undoCol] = SOKOCIBLE;
    }
    else
    {
        plateau[undoLig][undoCol] = SOKO;
    }

    *ligne = undoLig;
    *colonne = undoCol;
}

// retour pour le mouvement d'une caisse
void caisse_undo(t_Plateau plateau, int *ligne, int *colonne, char mouv)
{
    int deplig = *ligne;
    int depcol = *colonne;
    int undoLig = deplig;
    int undoCol = depcol;

    if (mouv == ECDROITE)
    {
        undoCol = depcol - 1;
        if (plateau[deplig][depcol + 1] == CAISSECIBLE)
        {
            plateau[deplig][depcol + 1] = CIBLE;
        }
        else
        {
            plateau[deplig][depcol + 1] = ' ';
        }
        plateau[deplig][depcol] = CAISSE;
    }
    else if (mouv == ECGAUCHE)
    {
        undoCol = depcol + 1;
        if (plateau[deplig][depcol - 1] == CAISSECIBLE)
        {
            plateau[deplig][depcol - 1] = CIBLE;
        }
        else
        {
            plateau[deplig][depcol - 1] = ' ';
        }
        plateau[deplig][depcol] = CAISSE;
    }
    else if (mouv == ECAVANCER)
    {
        undoLig = deplig + 1;
        if (plateau[deplig - 1][depcol] == CAISSECIBLE)
        {
            plateau[deplig - 1][depcol] = CIBLE;
        }
        else
        {
            plateau[deplig - 1][depcol] = ' ';
        }
        plateau[deplig][depcol] = CAISSE;
    }
    else if (mouv == ECRECULER)
    {
        undoLig = deplig - 1;
        if (plateau[deplig + 1][depcol] == CAISSECIBLE)
        {
            plateau[deplig + 1][depcol] = CIBLE;
        }
        else
        {
            plateau[deplig + 1][depcol] = ' ';
        }
        plateau[deplig][depcol] = CAISSE;
    }
    else
    {
        return;
    }

    if (plateau[undoLig][undoCol] == CIBLE)
    {
        plateau[undoLig][undoCol] = SOKOCIBLE;
    }
    else
    {
        plateau[undoLig][undoCol] = SOKO;
    }

    *ligne = undoLig;
    *colonne = undoCol;
}

