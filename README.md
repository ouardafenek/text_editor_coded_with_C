## Projet de CProj : VIlain - (un) editeur de texte
Repository pour le projet de CProj

## Membres
	- FENEK Ouarda
	- TREU Marc
	- ZHOU Eric

## Réalisation

### Projet
* Interface utilisateur
  - Une interface avec ncurses
* Buffer
  - La gestion des caractères d'un ficher
* Chargement et sauvegarde de fichiers
  - Pour le chargement et la sauvegarde de fichier
* Defilement
  - Dans le cas où le fichier est trop long pour être affiché sur un terminal
* Curseur, Position, Edition
  - La gestion des déplacement des curseurs
  - La position des curseurs
  - L'édition a l'emplacement du curseur
* Selection
  - La selection d'une partie du texte
* Copier/Coller
  - Copier un contenue pour le coller ailleurs
* Raccourcie clavier
  - Des raccourcies claviers pour faciliter l'usage
* Configuration
  - Un éditeur configurable
* Line Wrapping
  - La gestion des lignes trop longues
* Recherche de chaine
  - La recherche de chaines dans tous le fichier
* Undo/Redo
  - Annuler une erreur et corriger la correction
* Récupération
  - Au cas où l'utilisateur a oublié de sauvegarder

### Autour du projet
* Git
  - Pour le travail de groupe
  - Pour faire du versionning
* Diagramme d'architecture
  - L'architecture de notre logiciel
  - Dans un fichier .dot
  - Pour générer le diagramme en png, faire : `make graph` dans le dossier du projet
* Journal
  - Pour garder une évolution du projet
* Makefile
  - Pour une compilation simplifié et automatiser quelques tâches
* Check
  - Pour tester quelques fonction importantes

## Utilisation
Après avoir éxecuté le make (`make`), le programme est situé dans le dossier bin et s'appelle Editeur (cproj_projet/bin/Editeur). 

Lors du lancement (`./Editeur` à partir du dossier bin), le terminal affiche une zone vierge qui est l'éditeur de texte. Il est possible de donner un fichier ne parametre pour l'ouvrir directement. 

En bas de l'éditeur se trouve une ligne d'information, notament consernant le numéro de la ligne et le numéro de la colonne du curseur. On trouvera dans cette barre quelques autre informations (voir plus loin)

La saisie du texte se fait par le clavier. Les déplacements du curseur avec les touches fléchées du clavier. La souris n'est pas encore géré. 

La sélection du texte est effectuée par pression sur shift et les touches fléchées. La zone de selection est surlignée en blanc.

**Toutes les actions possibles sont disponibles dans le menu accessible via la conbinaison ctrl+n.**

Grace à la selection, il est possible de faire des copier/coller avec les raccourcies définies (ctrl+c pour copier et ctrl+v pour coller par défaut). Il est de même possible de couper du texte (ctrl+x par défaut)

Il est également de faire de la recherche de texte, il suffit de faire la combinaison ctrl+f (valeur par défaut) et inscrire le texte à chercher. Ctrl+q ou échap permet de terminer la recherche.

Notre éditeur supporte également d'annuler et refaire un modification grace aux combinaison ctrl+z et ctrl+y respectivement (valeur par défaut)

Une fois les modification de textes éffectuées, la sauvegarde est effectuée avec ctrl+s (par défaut). Notez que la sauvegarde est effectuée automatiquement après un certaint nombre de modification effectuée.
De même, l'ouverture d'in fichier se fait avec la combinaison ctrl+o (par défaut).

Pour quitter l'application, faites ctrl+q, il serait alors possible de sauvegarder ou non le fichier modifié et si le nom du fichier n'est pas connu, il sera demandé.

La combinaison de touche ctrl+m, permet de formater le fichier (auto-fill)

Il est également possible de desactiver le line wrapping


La modification des raccourcies se fait avec la combinaison ctrl+n et le menu Modify accesssible avec les touches fléchées.


