// Programme 3 : mesure du temps du parcours en largeur pour 3 files differentes,
// SANS aucun affichage pendant la mesure
#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <chrono>
#include <iomanip>    // setprecision
#include "Arbre.h"    // struct Noeud, construireArbre(), detruire()
#include "File.h"     // notre classe File (la meme que dans LargeurFileMaison.cpp)
using namespace std;

// Construit un grand arbre n-aire de n noeuds ou chaque noeud a (au plus) "k" enfants
Noeud* construireGrandArbre(int n, int k)
{
    vector<Noeud*> noeuds(n);
    for (int i = 0; i < n; i++) noeuds[i] = new Noeud(i % 100);
    for (int i = 0; i < n; i++)
        for (int j = 1; j <= k && k * i + j < n; j++)
            noeuds[i]->enfants.push_back(noeuds[k * i + j]);
    return noeuds[0];
}

// Parcours en largeur de l'arbre.
// AUCUN affichage ici : on mesure uniquement l'algorithme.
// Le type de file est un parametre : on peut tester deque, list ou notre File.
template <typename TypeFile>
long long parcoursLargeur(Noeud* racine)
{
    TypeFile file;
    long long somme = 0;              // on "traite" chaque noeud en l'ajoutant a la somme

    file.push(racine);
    while (!file.empty()) {
        Noeud* courant = file.front();
        file.pop();
        somme += courant->valeur;

        for (Noeud* e : courant->enfants)
            file.push(e);
    }
    return somme;                     // renvoyee pour que le compilateur ne supprime pas la boucle
}

// Lance le parcours "repetitions" fois et renvoie le temps MOYEN d'un parcours en nanosecondes
template <typename TypeFile>
double mesurer(Noeud* racine, int repetitions, long long& controle)
{
    auto debut = chrono::steady_clock::now();          // top depart
    for (int r = 0; r < repetitions; r++)
        controle += parcoursLargeur<TypeFile>(racine);
    auto fin = chrono::steady_clock::now();            // top arrivee

    double total = chrono::duration<double, nano>(fin - debut).count();
    return total / repetitions;
}

void comparer(Noeud* racine, int nbNoeuds, int repetitions)
{
    long long controle = 0;
    double tDeque  = mesurer<queue<Noeud*, deque<Noeud*>>>(racine, repetitions, controle);
    double tList   = mesurer<queue<Noeud*, list<Noeud*>>>(racine, repetitions, controle);
    double tMaison = mesurer<File>(racine, repetitions, controle);

    // l'affichage se fait seulement ICI, apres toutes les mesures
    cout << fixed << setprecision(3);
    cout << "Arbre de " << nbNoeuds << " noeuds, " << repetitions << " repetitions\n";
    cout << "  queue<Noeud*, deque<Noeud*>> : " << tDeque  / 1000 << " us / parcours\n";
    cout << "  queue<Noeud*, list<Noeud*>>  : " << tList   / 1000 << " us / parcours\n";
    cout << "  File maison (vector)         : " << tMaison / 1000 << " us / parcours\n";
    cout << "  (controle : " << controle << ")\n\n";
}

int main()
{
    // 1) Le meme arbre que les autres programmes (12 noeuds)
    Noeud* petit = construireArbre();
    comparer(petit, 12, 100000);
    detruire(petit);

    // 2) Un grand arbre (1 000 000 de noeuds, 3 enfants par noeud)
    const int N = 1000000;
    Noeud* grand = construireGrandArbre(N, 3);
    comparer(grand, N, 20);
    detruire(grand);

    return 0;
}
