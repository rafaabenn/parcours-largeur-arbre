#ifndef FILE_H
#define FILE_H

#include <vector>
#include <stdexcept>  // std::out_of_range
#include "Arbre.h"

// ===================== Conteneur adaptateur (commun aux programmes 2 et 3) =====================
// Une file (premier entre, premier sorti) construite sur un vector.
// Le vector est cache (private) : on ne montre que les operations d'une file.
class File {
private:
    std::vector<Noeud*> elements;   // le conteneur cache a l'interieur
    size_t debut = 0;               // indice du premier element encore dans la file

public:
    void push(Noeud* n)  { elements.push_back(n); }    // ajouter a la fin

    Noeud* front() const                                // lire le premier
    {
        if (empty()) throw std::out_of_range("File::front() sur une file vide");
        return elements[debut];
    }

    void pop()                                          // retirer le premier (on avance l'indice)
    {
        if (empty()) throw std::out_of_range("File::pop() sur une file vide");
        debut++;
    }

    bool empty() const   { return debut == elements.size(); }
    size_t size() const  { return elements.size() - debut; }
};

#endif
