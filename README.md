Jeu SOKOBAN


J'ai programmé le jeu sokoban avec tout ses mouvement et ses fonctionnalités.

Le but du jeu est de pousser des caisses sur des emplacements cibles dans un entrepôt avec un nombre minimal de déplacements.



Fonctionnalités :

* **Gameplay Classique :** Déplacements fluides du joueur (haut, bas, gauche, droite) avec gestion stricte des collisions (murs et caisses).
* **Moteur de Jeu Robuste :** Gestion des conditions de victoire (toutes les caisses sur les cibles).
* **Historique & Undo :** Possibilité d'annuler ses coups pour corriger une erreur (via une structure de pile).
* **Lecture de Niveaux Externes :** Chargement dynamique des cartes à partir de fichiers textes (`.txt`).
* **Compteur de Coups :** Affichage en temps réel des pas effectués et des caisses bien placées.




Ce projet a permis de valider plusieurs livrables essentiels des blocs de compétences de première année :

- Conception d'une architecture de code propre, modularité des fonctions et gestion de la mémoire.
- Manipulation de matrices (tableaux 2D) pour la grille, structures de données complexes et algorithmes de vérification d'états.




Aperçu des Symboles du Jeu :

| Symbole | Élément |
| :---: | :--- |
| `#` | Mur |
| `.` | Zone de stockage (Cible) |
| `$` | Caisse |
| `@` | Joueur |
| `*` | Caisse sur une zone de stockage |
| `+` | Joueur sur une zone de stockage |




Installation et Lancement du jeu :

Prérequis pour jouer
- Avoir un compilateur installé (comme `gcc` pour le C ou `g++` pour le C++).
- Avoir un OS sous Linux ou similaire.




  
Téléchargez et compilez le projet à l'aide de la commande bash suivante :


cd sokoban
gcc main.c -o sokoban # Modifie selon la structure exacte de tes fichiers (.c ou .cpp)
