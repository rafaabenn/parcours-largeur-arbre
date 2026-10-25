// Programme 1 : parcours en largeur avec std::queue, affiche pas a pas
#include <iostream>
#include <vector>
#include <queue>      // la file de la bibliothèque standard : std::queue (sur deque par défaut)
#include "Arbre.h"    // struct Noeud, construireArbre(), reinitialiser(), detruire()
using namespace std;

// Affiche le contenu de la file sans la modifier :
// on reçoit une COPIE (passage par valeur) et on la vide.
void afficherFile(queue<Noeud*> copie)
{
    cout << "[ ";
    while (!copie.empty()) {
        cout << copie.front()->valeur << " ";  // valeur du nœud en tête de file
        copie.pop();                           // on passe au suivant
    }
    cout << "]";
}

// Affiche une liste de valeurs
void afficherListe(const vector<int>& liste)
{
    cout << "{ ";
    for (vector<int>::const_iterator it = liste.begin(); it != liste.end(); ++it)
        cout << *it << " ";
    cout << "}";
}

void parcoursLargeur(Noeud* racine)
{
    if (racine == nullptr) return;    // arbre vide : rien à parcourir
    reinitialiser(racine);            // aucun nœud n'est encore visité

    queue<Noeud*> file;              // elle contient des POINTEURS vers les nœuds
    vector<int> visites;              // nœuds VISITES (découverts), dans l'ordre
    vector<int> affiches;             // nœuds AFFICHES (sortis de la file), dans l'ordre

    // On découvre la racine : elle est VISITEE et entre dans la file
    racine->visite = true;
    visites.push_back(racine->valeur);
    file.push(racine);

    cout << "Depart\n";
    cout << "  visites  : "; afficherListe(visites);  cout << "\n";
    cout << "  file     : "; afficherFile(file);      cout << "\n\n";

    int etape = 1;
    // Tant qu'il reste des nœuds à traiter...
    while (!file.empty()) {
        // 1. On retire le premier nœud de la file (le plus ancien)
        Noeud* courant = file.front();
        file.pop();

        // 2. On l'AFFICHE (on le traite)
        affiches.push_back(courant->valeur);
        cout << "Etape " << etape++ << " : on affiche " << courant->valeur << "\n";

        // 3. On met TOUS ses enfants à la FIN de la file, de gauche à droite :
        //    ils seront traités après les nœuds du même niveau déjà en attente
        cout << "  nouveaux visites : ";
        for (Noeud* e : courant->enfants) {
            if (e != nullptr && !e->visite) {
                e->visite = true;              // VISITE au moment où il entre dans la file
                visites.push_back(e->valeur);
                file.push(e);
                cout << e->valeur << " ";
            }
        }
        if (courant->enfants.empty()) cout << "aucun (feuille)";
        cout << "\n";

        cout << "  visites  : "; afficherListe(visites);  cout << "\n";
        cout << "  affiches : "; afficherListe(affiches); cout << "\n";
        cout << "  file     : "; afficherFile(file);      cout << "\n\n";
    }

    cout << "Parcours en largeur : ";
    for (vector<int>::iterator it = affiches.begin(); it != affiches.end(); ++it)
        cout << *it << " ";
    cout << endl;
}

// Affiche l'arbre niveau par niveau.
// Astuce : au début de chaque niveau, la file contient EXACTEMENT les nœuds de ce niveau,
// donc file.size() nous dit combien de nœuds retirer avant de passer au niveau suivant.
void afficherParNiveau(Noeud* racine)
{
    if (racine == nullptr) return;

    queue<Noeud*> file;
    file.push(racine);
    int niveau = 0;

    while (!file.empty()) {
        size_t nbNoeuds = file.size();          // nombre de nœuds du niveau courant
        cout << "Niveau " << niveau++ << " (" << nbNoeuds << " noeud(s)) : ";

        for (size_t k = 0; k < nbNoeuds; k++) {
            Noeud* courant = file.front();
            file.pop();
            cout << courant->valeur << " ";
            for (Noeud* e : courant->enfants)   // ses enfants appartiennent au niveau suivant
                file.push(e);
        }
        cout << "\n";
    }
}

int main()
{
    Noeud* racine = construireArbre();
    parcoursLargeur(racine);

    cout << "\n===== Affichage par niveau =====\n";
    afficherParNiveau(racine);

    // 2e parcours du même arbre : fonctionne grâce à reinitialiser()
    cout << "\n===== Deuxieme parcours du meme arbre =====\n";
    parcoursLargeur(racine);

    detruire(racine);
    return 0;
}
