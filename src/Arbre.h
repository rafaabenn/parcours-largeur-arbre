#ifndef ARBRE_H
#define ARBRE_H

#include <vector>

// ===================== Structure de l'arbre n-aire (commune a tous les programmes) =====================
// Chaque noeud a un nombre QUELCONQUE d'enfants, ranges de gauche a droite dans un vector.
struct Noeud {
    int valeur;
    std::vector<Noeud*> enfants;
    bool visite;            // true des que le noeud a ete decouvert (mis dans la file).
                            // Dans un arbre, un noeud n'a qu'un parent : il ne peut pas etre
                            // decouvert deux fois. Ce marqueur devient indispensable dans un GRAPHE.

    Noeud(int v) : valeur(v), visite(false) {}
};

// Cree un nouveau noeud, l'accroche comme dernier enfant de "parent" et le renvoie
inline Noeud* ajouterEnfant(Noeud* parent, int v)
{
    Noeud* enfant = new Noeud(v);
    parent->enfants.push_back(enfant);
    return enfant;
}

// Construit l'arbre de test (12 noeuds, 4 niveaux, des noeuds a 0, 1, 2 ou 4 enfants) :
//
//                  7                        niveau 0
//         +--------+--------+
//         9        67       1               niveau 1
//       +-+-+         +---+-+-+----+
//      28   8        10   29  2   57        niveau 2
//      |             |
//      40            6                      niveau 3
inline Noeud* construireArbre()
{
    Noeud* racine = new Noeud(7);

    Noeud* n9  = ajouterEnfant(racine, 9);
    ajouterEnfant(racine, 67);                 // feuille
    Noeud* n1  = ajouterEnfant(racine, 1);

    Noeud* n28 = ajouterEnfant(n9, 28);
    ajouterEnfant(n9, 8);                      // feuille

    Noeud* n10 = ajouterEnfant(n1, 10);
    ajouterEnfant(n1, 29);                     // feuilles
    ajouterEnfant(n1, 2);
    ajouterEnfant(n1, 57);

    ajouterEnfant(n28, 40);                    // feuille
    ajouterEnfant(n10, 6);                     // feuille

    return racine;
}

// Remet tous les marqueurs "visite" a false.
// A appeler au debut de chaque parcours : sinon un 2e parcours du meme arbre
// trouverait tous les noeuds deja marques et s'arreterait a la racine.
inline void reinitialiser(Noeud* n)
{
    if (n == nullptr) return;
    n->visite = false;
    for (Noeud* e : n->enfants) reinitialiser(e);
}

// Libere tout l'arbre : les enfants d'abord, puis le noeud
inline void detruire(Noeud* n)
{
    if (n == nullptr) return;
    for (Noeud* e : n->enfants) detruire(e);
    delete n;
}

#endif
