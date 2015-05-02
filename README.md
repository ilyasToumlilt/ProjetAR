#ProjetAR - Content-Adressable Network
######version 1.0

** Participants : 2 **

   MABILLOT Paul  - 3100385 - <paul.mabillot@etu.upmc.fr>
   TOUMLILT Ilyas - 3261538 - <ilyas.toumlilt@etu.upmc.fr>

** a propos **
   Pour cette version de soumission on a une librairie complète avec les 4 étapes demandées
   qui compile mais qui ne marche pas pour tous les cas, nous essayerons de débuguer l'execution
   et d'avoir une code totalement fonctionnel pour la soutenance du Lundi. Cependant le code
   soumis est clair, documenté et décrit le fonctionnement global des algos choisis.
   
** Fichiers inclus **

   Primitives:
   -> src/CAN.c include/CAN.h : bibliohtèque globale
   -> src/CANsearchData.c(.h) : coté recherche de data de l'API
   -> src/CANinsertData.c(.h) : coté insertion de data de l'aPI
   
   Bibliothèque d'affichage:
   -> include/display.h
   -> src/display.c
   
   Le reste sont des primitives
   -> include/*.h : headers
   -> src/*.c : implementations

**== Directives de 'makefile' ==**

   all : monsieur fait tout.

   can : produit la lib de l'API

   directories : création des répertoires temporaires s'ils n'existent pas déjà
   clean : Suppression de tous les fichiers binaires, objet et bibliothèques.
   cleanall : Suppression des répertoires et fichiers temporaires

**== Etat d'avancement ==**

   Algos           - OK
   API d'affichage - OK
   Tests           - KO
   Main            - KO
   

**Soumission**
    le 30 avril
