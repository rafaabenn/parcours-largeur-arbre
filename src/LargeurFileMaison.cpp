// Programme 2 : le meme parcours, mais avec NOTRE PROPRE file construite sur un vector
// (pour comprendre ce qu'est un conteneur adaptateur comme std::queue)
#include <iostream>
#include <vector>
#include "Arbre.h"    // struct Noeud, construireArbre(), reinitialiser(), detruire()
#include "File.h"     // notre classe File (adaptateur sur un vector)
using namespace std;

// Contenu de la file : on recoit une COPIE et on la vide (la vraie file ne change pas)
void afficherFile(File copie)
{
    cout << "[ ";
    while (!copie.empty()) {
        cout << copie.front()->valeur << " ";
        copie.pop();
    }
    cout << "]";
}

void parcoursLargeur(Noeud* racine)
{
    if (racine == nullptr) return;
    reinitialiser(racine);      // aucun noeud n'est encore visite

    File file;
    vector<int> resultat;       // noeuds AFFICHES, dans l'ordre

    racine->visite = true;
    file.push(racine);

    cout << "Depart | file : "; afficherFile(file); cout << "\n";

    while (!file.empty()) {
        Noeud* courant = file.front();
        file.pop();
        resultat.push_back(courant->valeur);

        cout << "affiche " << courant->valeur << " | enfants ajoutes : ";
        for (Noeud* e : courant->enfants) {
            if (e != nullptr && !e->visite) {
                e->visite = true;
                file.push(e);
                cout << e->valeur << " ";
            }
        }
        if (courant->enfants.empty()) cout << "aucun ";

        cout << "| file (" << file.size() << ") : ";
        afficherFile(file);
        cout << "\n";
    }

    cout << "\nParcours en largeur : ";
    for (vector<int>::iterator it = resultat.begin(); it != resultat.end(); ++it)
        cout << *it << " ";
    cout << endl;
}

int main()
{
    Noeud* racine = construireArbre();
    parcoursLargeur(racine);

    // Notre File protege contre une lecture sur file vide (std::queue, elle, ne verifie rien)
    File vide;
    try {
        vide.front();
    } catch (const out_of_range& e) {
        cout << "\nTest file vide -> erreur attrapee : " << e.what() << endl;
    }

    detruire(racine);
    return 0;
}
