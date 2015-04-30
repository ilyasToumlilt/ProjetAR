#ProjetAR - Content-Adressable Network
######version 1.0

**== Participants : 2 ==**

   MABILLOT Paul  -         - <paul.mabillot@etu.upmc.fr>
   TOUMLILT Ilyas - 3261538 - <toumlilt.ilyas@gmail.com>
   
**== Fichiers inclus ==**

   Primitives:
   -> include/point.h :
   -> src/point.c :
   -> include/space.h :
   -> src/space.c :
   -> include/node.h :
   -> src/node.c :
   -> include/list_node.h :
   -> src/list_node.c :

   API publique :
   -> include/CAN.h :
   -> src/CAN.c :

**== Directives de 'makefile' ==**

   all : monsieur fait tout.

   can : produit la lib de l'API

   directories : création des répertoires temporaires s'ils n'existent pas déjà
   clean : Suppression de tous les fichiers binaires, objet et bibliothèques.
   cleanall : Suppression des répertoires et fichiers temporaires

**== Etat d'avancement ==**

   Primitives - OK
   Insertion  - KO
   

**== Réponses ==**

   

**Soumission**
    le 30 avril

**todo**
OK marche en mono
KO revoir l'attribution des voisins p
KO insert MPI i
OK .c .h primitives des données p

KO inserer ses données i
KO recherche MPI i
KO suppression i
KO test case p
KO sortie pour affichage p
OK affichage p

**test case**
une première ligne avec :
<nbNodes> <nbData>
nbNodes lignes avec :
id x y
nbDatas lignes avec
data x y