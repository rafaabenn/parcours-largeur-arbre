# Parcours en largeur d'un arbre n-aire — `std::queue` et file maison (C++17)

**Module :** ILISI 2 — C++ / STL
**Thème :** conteneurs adaptateurs (`std::queue`) et conteneurs séquentiels (`deque`, `list`, `vector`)

## Résumé

Ce travail étudie le parcours en largeur (BFS) d'un arbre n-aire de 12 nœuds à travers trois programmes C++. L'arbre est une vraie structure chaînée : chaque `Noeud` garde ses enfants dans un `vector<Noeud*>`, ce qui lui permet d'en avoir un nombre quelconque. Le premier programme montre le parcours pas à pas avec `std::queue`. Le deuxième refait le même parcours avec une file écrite à la main sur un `vector`, pour comprendre le principe d'un adaptateur. Le troisième mesure le temps du parcours avec trois files différentes (`deque`, `list`, file maison), **sans aucun affichage pendant la mesure**.

Résultats principaux :
- les deux programmes pas à pas donnent le même ordre : `7 9 67 1 28 8 10 29 2 57 40 6` ;
- `std::list` est nettement la file la plus lente (≈ 4 à 6× sur 12 nœuds, ≈ 7 à 9× sur 1 000 000 de nœuds) ;
- `std::deque`, le choix par défaut de `std::queue`, et la file maison sur `vector` ont des performances proches ;
- tous les nœuds sont libérés à la fin, aucun programme ne fuit la mémoire.

## Sommaire

1. [Objectifs](#1-objectifs)
2. [Rappels théoriques](#2-rappels-théoriques)
3. [Organisation du projet](#3-organisation-du-projet)
4. [Environnement et compilation](#4-environnement-et-compilation)
5. [Les programmes](#5-les-programmes)
6. [Résultats](#6-résultats)
7. [Analyse](#7-analyse)
8. [Conclusion](#8-conclusion)
9. [Annexe — sorties du terminal](#9-annexe--sorties-du-terminal)

---

## 1. Objectifs

| # | Objectif |
|---|----------|
| 1 | Représenter un arbre n-aire par une structure chaînée (`struct Noeud` + `vector<Noeud*>`) |
| 2 | Implémenter un parcours en largeur correct avec `std::queue` |
| 3 | Visualiser l'état de la file à chaque étape pour comprendre le principe FIFO |
| 4 | Distinguer un nœud **visité** (découvert, entré dans la file) d'un nœud **affiché** (sorti de la file) |
| 5 | Comprendre ce qu'est un **adaptateur** en construisant soi-même une file sur un `vector` |
| 6 | Mesurer correctement, sans affichage, l'impact du conteneur sous-jacent sur les performances |

## 2. Rappels théoriques

### 2.1 Parcours en largeur

Le parcours en largeur visite l'arbre **niveau par niveau**, de gauche à droite. Il utilise une **file** (premier entré, premier sorti) :

```text
mettre la racine dans la file
tant que la file n'est pas vide :
    retirer le nœud en tête  -> le traiter (l'afficher)
    ajouter tous ses enfants, de gauche à droite, à la fin de la file
```

Les enfants sont ajoutés **à la fin** de la file. Ils sont donc traités seulement après tous les nœuds du niveau courant déjà en attente.

| Propriété | Valeur |
|-----------|--------|
| Temps | O(n) : chaque nœud entre et sort une fois de la file |
| Mémoire | O(largeur maximale de l'arbre) |

### 2.2 Arbre n-aire chaîné

Dans un arbre **n-aire**, un nœud peut avoir 0, 1, 2… ou n'importe quel nombre d'enfants. On ne peut donc pas prévoir des champs fixes comme `gauche` et `droit`. On range les enfants dans un `vector<Noeud*>`, de gauche à droite. Un nœud dont le vector est vide est une **feuille**. La file du parcours contient des **pointeurs** `Noeud*` : on y range les nœuds eux-mêmes, ce qui permet d'atteindre leurs enfants quand ils en sortent.

### 2.3 `std::queue` est un adaptateur

`std::queue` n'est pas un conteneur à part entière. Elle **enveloppe** un autre conteneur et n'en montre que les opérations d'une file :

| Opération de la file | Appel sur le conteneur caché |
|----------------------|------------------------------|
| `push(x)`  | `push_back(x)` |
| `pop()`    | `pop_front()` |
| `front()`  | `front()` |
| `empty()`  | `empty()` |

Le conteneur par défaut est `std::deque`. On peut le changer : `queue<Noeud*, list<Noeud*>>`. En revanche, `vector` ne convient pas, car il n'a pas de `pop_front()`. C'est pour cette raison que notre file maison utilise un indice `debut` qui avance, au lieu d'effacer le premier élément.

## 3. Organisation du projet

```
arbre/
├── README.md                  # ce rapport
├── Makefile                   # compilation des 3 programmes
├── .gitignore
├── src/
│   ├── Arbre.h                # struct Noeud, construireArbre(), reinitialiser(), detruire() — commun aux 3
│   ├── File.h                 # classe File maison (adaptateur sur vector) — programmes 2 et 3
│   ├── LargeurQueue.cpp       # programme 1 : std::queue, affichage pas à pas + par niveau
│   ├── LargeurFileMaison.cpp  # programme 2 : file maison sur vector, affichage pas à pas
│   └── LargeurPerf.cpp        # programme 3 : mesure deque / list / file maison, sans affichage
└── build/                     # exécutables générés
```

## 4. Environnement et compilation

| Élément | Valeur |
|---------|--------|
| Machine | HP EliteBook 840 G5 |
| Processeur | Intel Core i7-8550U @ 1.80 GHz |
| Système | Windows 11 Pro |
| Compilateur | g++ 15.2.0 (MinGW-W64) |
| Standard | C++17 |

```bash
mingw32-make          # compile les 3 programmes dans build/   (sous Linux : make)
mingw32-make run      # exécute les 3 programmes
mingw32-make clean    # supprime les exécutables
```

Ou un seul programme à la main :

```bash
g++ -std=c++17 -O2 -Wall -Wextra src/LargeurQueue.cpp -o build/LargeurQueue
.\build\LargeurQueue.exe
```

Les options : `-std=c++17` pour le standard, `-O2` pour l'optimisation (indispensable pour des mesures significatives), `-Wall -Wextra` pour les avertissements. Les 3 programmes compilent **sans aucun avertissement**.

## 5. Les programmes

### 5.1 L'arbre de test : `Arbre.h`

```text
                  7                          niveau 0
         ┌────────┼────────┐
         9        67       1                 niveau 1
       ┌─┴─┐         ┌───┬─┴─┬────┐
      28   8        10   29  2   57          niveau 2
      │             │
      40            6                        niveau 3
```

L'arbre compte 12 nœuds et 4 niveaux. Il est volontairement **irrégulier** : la racine a 3 enfants, `9` en a 2, `1` en a 4, `28` et `10` en ont 1, et `67`, `8`, `29`, `2`, `57`, `40`, `6` sont des feuilles. On vérifie ainsi que le parcours fonctionne quel que soit le nombre d'enfants.

Les trois programmes incluent le même en-tête, et l'arbre est donc défini **une seule fois** :

```cpp
struct Noeud {
    int valeur;
    std::vector<Noeud*> enfants;   // nombre quelconque d'enfants, de gauche à droite
    bool visite;                   // true dès que le nœud a été découvert
    Noeud(int v) : valeur(v), visite(false) {}
};

// crée un nœud, l'accroche comme dernier enfant de "parent" et le renvoie
inline Noeud* ajouterEnfant(Noeud* parent, int v);

inline Noeud* construireArbre()
{
    Noeud* racine = new Noeud(7);
    Noeud* n9  = ajouterEnfant(racine, 9);
    ajouterEnfant(racine, 67);
    Noeud* n1  = ajouterEnfant(racine, 1);
    // ...
    return racine;
}

inline void reinitialiser(Noeud* n)  // remet tous les marqueurs "visite" à false
{
    if (n == nullptr) return;
    n->visite = false;
    for (Noeud* e : n->enfants) reinitialiser(e);
}

inline void detruire(Noeud* n)     // libère tout l'arbre, les enfants avant le parent
{
    if (n == nullptr) return;
    for (Noeud* e : n->enfants) detruire(e);
    delete n;
}
```

**Pourquoi `reinitialiser()` ?** Le marqueur `visite` est rangé dans le nœud lui-même. Sans remise à zéro, un 2ᵉ parcours du même arbre trouverait tous les enfants déjà marqués et n'afficherait que la racine. Chaque `parcoursLargeur()` commence donc par appeler `reinitialiser()`. `LargeurQueue.cpp` le vérifie en parcourant l'arbre deux fois de suite.

**Le marqueur est-il nécessaire ?** Dans un **arbre**, non : un nœud n'a qu'un seul parent, donc il ne peut pas être découvert deux fois. On le garde pour illustrer la notion de nœud *visité* et parce qu'il devient indispensable dans un **graphe**, où plusieurs chemins mènent au même nœud.

Le `main()` de chaque programme tient en trois lignes :

```cpp
Noeud* racine = construireArbre();
parcoursLargeur(racine);
detruire(racine);
```

### 5.2 Les trois programmes

| Programme | File utilisée | Ce que le programme montre |
|-----------|---------------|----------------------------|
| `LargeurQueue.cpp` | `std::queue<Noeud*>` (sur `deque`) | à chaque étape : le nœud affiché, les nouveaux visités, les listes *visités* / *affichés* et le contenu de la file ; puis l'arbre **niveau par niveau** ; puis un 2ᵉ parcours du même arbre |
| `LargeurFileMaison.cpp` | classe `File` maison (sur `vector`) | le même parcours sur une ligne par étape, avec la taille de la file ; puis un test de lecture sur une file vide |
| `LargeurPerf.cpp` | `deque`, `list` et `File` maison | le temps moyen d'un parcours pour chacune, sans affichage |

### 5.3 Points clés du code

**Parcourir un nœud n-aire.** On ne connaît pas à l'avance le nombre d'enfants, donc on parcourt le vector avec une boucle :

```cpp
for (Noeud* e : courant->enfants) {
    if (e != nullptr && !e->visite) {
        e->visite = true;          // VISITÉ au moment où il entre dans la file
        file.push(e);
    }
}
if (courant->enfants.empty()) cout << "aucun (feuille)";
```

**Visité ou affiché.** Un nœud est marqué *visité* **au moment où il entre dans la file**, pas quand il en sort. Il ne peut donc jamais être ajouté deux fois. Dans un arbre, c'est une simple précaution ; dans un graphe, ce serait indispensable.

**Afficher la file sans la vider.** La fonction reçoit une **copie** (passage par valeur), qu'elle vide pour l'afficher. La vraie file n'est pas modifiée :

```cpp
void afficherFile(queue<Noeud*> copie)
{
    cout << "[ ";
    while (!copie.empty()) {
        cout << copie.front()->valeur << " ";
        copie.pop();
    }
    cout << "]";
}
```

**Construire son propre adaptateur (`File.h`).** Le `vector` est privé. On n'expose que les opérations d'une file, et `pop()` se contente d'avancer un indice, ce qui coûte O(1) au lieu d'effacer le premier élément (O(n)). La classe est écrite une seule fois et partagée par les programmes 2 et 3 :

```cpp
class File {
private:
    std::vector<Noeud*> elements;
    size_t debut = 0;
public:
    void push(Noeud* n)  { elements.push_back(n); }
    Noeud* front() const
    {
        if (empty()) throw std::out_of_range("File::front() sur une file vide");
        return elements[debut];
    }
    void pop()
    {
        if (empty()) throw std::out_of_range("File::pop() sur une file vide");
        debut++;
    }
    bool empty() const   { return debut == elements.size(); }
    size_t size() const  { return elements.size() - debut; }
};
```

**Protéger contre la file vide.** Sur une `std::queue` vide, `front()` et `pop()` ont un *comportement indéfini* : le programme peut planter ou lire n'importe quoi. Notre `File` vérifie et lance une exception `std::out_of_range`. `LargeurFileMaison.cpp` le teste à la fin :

```text
Test file vide -> erreur attrapee : File::front() sur une file vide
```

**Afficher par niveau.** Au début de chaque niveau, la file contient **exactement** les nœuds de ce niveau. `file.size()` donne donc le nombre de nœuds à retirer avant de passer au niveau suivant :

```cpp
while (!file.empty()) {
    size_t nbNoeuds = file.size();          // nombre de nœuds du niveau courant
    cout << "Niveau " << niveau++ << " : ";
    for (size_t k = 0; k < nbNoeuds; k++) {
        Noeud* courant = file.front();
        file.pop();
        cout << courant->valeur << " ";
        for (Noeud* e : courant->enfants)   // ses enfants appartiennent au niveau suivant
            file.push(e);
    }
}
```

### 5.4 Comment on mesure sans affichage (`LargeurPerf.cpp`)

Un `cout` prend beaucoup plus de temps que de retirer un nœud de la file. Si on chronométrait un parcours qui affiche, on mesurerait surtout l'affichage. Le programme sépare donc strictement **le calcul** et **l'affichage**, en quatre étapes.

**1. Un parcours qui n'affiche rien.** Au lieu d'afficher chaque nœud, on additionne sa valeur dans une variable `somme`, que la fonction renvoie :

```cpp
template <typename TypeFile>
long long parcoursLargeur(Noeud* racine)
{
    TypeFile file;
    long long somme = 0;
    file.push(racine);
    while (!file.empty()) {
        Noeud* courant = file.front();
        file.pop();
        somme += courant->valeur;          // le "traitement", sans cout
        for (Noeud* e : courant->enfants)
            file.push(e);
    }
    return somme;
}
```

La somme ne sert à rien en elle-même. Elle est là parce qu'en `-O2`, le compilateur supprime un calcul dont personne n'utilise le résultat : on mesurerait alors… rien. Comme la somme est renvoyée puis affichée à la fin (« controle »), la boucle est obligatoirement exécutée.

**2. Le chronomètre n'entoure que la boucle de parcours.** On prend l'heure juste avant et juste après, avec `steady_clock`, une horloge qui avance toujours régulièrement et convient donc pour mesurer des durées :

```cpp
auto debut = chrono::steady_clock::now();          // top départ
for (int r = 0; r < repetitions; r++)
    controle += parcoursLargeur<TypeFile>(racine);
auto fin = chrono::steady_clock::now();            // top arrivée
```

**3. On répète, puis on fait la moyenne.** Un seul parcours de 12 nœuds dure moins d'une microseconde, ce qui est trop court pour être mesuré précisément. On le répète 100 000 fois, puis on divise le temps total par 100 000. Pour le grand arbre d'un million de nœuds, 20 répétitions suffisent.

**4. L'affichage vient seulement après toutes les mesures.** Les `cout` se trouvent dans `comparer()`, une fois les trois chronomètres arrêtés.

Enfin, la file est un paramètre `template`. Le **même code** de parcours est donc mesuré avec les trois files, et seule la file change :

```cpp
mesurer<queue<Noeud*, deque<Noeud*>>>(racine, repetitions, controle);
mesurer<queue<Noeud*, list<Noeud*>>>(racine, repetitions, controle);
mesurer<File>(racine, repetitions, controle);
```

Le grand arbre est construit par `construireGrandArbre(1000000, 3)` : un arbre n-aire où chaque nœud a 3 enfants.

## 6. Résultats

### 6.1 Trace du parcours

Trace produite par les deux programmes pas à pas :

| Étape | Nœud affiché | Enfants ajoutés | File après l'étape | Taille |
|-------|--------------|-----------------|--------------------|--------|
| départ | — | — | [ 7 ] | 1 |
| 1 | 7 | 9, 67, 1 | [ 9 67 1 ] | 3 |
| 2 | 9 | 28, 8 | [ 67 1 28 8 ] | 4 |
| 3 | 67 | aucun (feuille) | [ 1 28 8 ] | 3 |
| 4 | 1 | 10, 29, 2, 57 | [ 28 8 10 29 2 57 ] | 6 |
| 5 | 28 | 40 | [ 8 10 29 2 57 40 ] | 6 |
| 6 | 8 | aucun (feuille) | [ 10 29 2 57 40 ] | 5 |
| 7 | 10 | 6 | [ 29 2 57 40 6 ] | 5 |
| 8 | 29 | aucun (feuille) | [ 2 57 40 6 ] | 4 |
| 9 | 2 | aucun (feuille) | [ 57 40 6 ] | 3 |
| 10 | 57 | aucun (feuille) | [ 40 6 ] | 2 |
| 11 | 40 | aucun (feuille) | [ 6 ] | 1 |
| 12 | 6 | aucun (feuille) | [ ] | 0 |

La file contient au plus **6 éléments** (étapes 4 et 5). C'est exactement la largeur du niveau 2, l'étage le plus large de l'arbre.

### 6.2 Sortie finale

Les deux programmes affichent la même ligne :

```text
Parcours en largeur : 7 9 67 1 28 8 10 29 2 57 40 6
```

### 6.3 Affichage par niveau

```text
Niveau 0 (1 noeud(s)) : 7
Niveau 1 (3 noeud(s)) : 9 67 1
Niveau 2 (6 noeud(s)) : 28 8 10 29 2 57
Niveau 3 (2 noeud(s)) : 40 6
```

On retrouve exactement les niveaux du dessin. Le niveau le plus large (6 nœuds) correspond à la taille maximale atteinte par la file dans la trace.

### 6.4 Mesures de performance (`LargeurPerf.cpp`)

Programme exécuté 3 fois. Les valeurs sont des temps moyens pour **un** parcours, compilé en `-O2`.

**Arbre de 12 nœuds (100 000 répétitions)**

| File | Essai 1 | Essai 2 | Essai 3 |
|------|---------|---------|---------|
| `queue<Noeud*, deque<Noeud*>>` | 0,288 µs | 0,401 µs | 0,333 µs |
| `queue<Noeud*, list<Noeud*>>`  | 1,654 µs | 2,079 µs | 1,676 µs |
| `File` maison (`vector`) | 0,668 µs | 0,603 µs | 0,678 µs |

**Arbre de 1 000 000 de nœuds, 3 enfants par nœud (20 répétitions)**

| File | Essai 1 | Essai 2 | Essai 3 |
|------|---------|---------|---------|
| `queue<Noeud*, deque<Noeud*>>` | 24,7 ms | 21,7 ms | 16,2 ms |
| `queue<Noeud*, list<Noeud*>>`  | 244,0 ms | 148,2 ms | 156,9 ms |
| `File` maison (`vector`) | 19,9 ms | 16,5 ms | 39,9 ms |

Les temps varient d'un essai à l'autre, car la machine fait d'autres tâches en même temps (la valeur de 39,9 ms en est un exemple). Il faut donc comparer les files **au sein d'un même essai**, et sur plusieurs essais.

## 7. Analyse

### 7.1 Correction

- L'ordre obtenu correspond exactement aux niveaux de l'arbre : le parcours est correct.
- Le résultat ne dépend pas de la file utilisée (`std::queue` ou classe maison). Seul le principe FIFO compte.
- Les cas particuliers sont gérés : feuilles (vector `enfants` vide), nœuds à 1, 2, 3 ou 4 enfants, arbre vide (`parcoursLargeur` retourne immédiatement si la racine est `nullptr`).
- Tous les nœuds alloués avec `new` sont libérés par `detruire()` : aucun programme ne fuit.
- Le même arbre peut être parcouru plusieurs fois grâce à `reinitialiser()`.
- Notre `File` refuse proprement une lecture sur file vide, là où `std::queue` a un comportement indéfini.

### 7.2 Pourquoi `list` est la plus lente

Avec `std::list`, **chaque `push` fait une allocation mémoire** (un nouveau maillon) et chaque `pop` une libération. De plus, les maillons sont dispersés en mémoire, ce qui exploite mal le cache du processeur. `deque` alloue par **blocs** de plusieurs éléments contigus, et le `vector` de la file maison n'alloue que lorsqu'il doit grandir. Résultat : `list` est environ 5 fois plus lente sur 12 nœuds, et 7 à 10 fois plus lente sur un million.

### 7.3 `deque` face à la file maison

Sur le petit arbre, `deque` est environ deux fois plus rapide. Sur le grand arbre, les deux sont au coude à coude, et la file maison est même un peu plus rapide dans deux essais sur trois. Les vérifications « file vide » ajoutées dans `front()` et `pop()` ne coûtent presque rien : le processeur prédit qu'elles sont toujours fausses. Notre file maison a toutefois un défaut : `pop()` n'efface rien. Le `vector` garde donc **tous** les éléments passés par la file jusqu'à la fin, soit O(n) en mémoire au lieu de O(largeur). `deque` libère ses blocs au fur et à mesure. La file maison convient donc pour comprendre le principe d'un adaptateur ou pour un parcours unique ; pour un usage général, `std::queue` (sur `deque`) reste le bon choix.

### 7.4 Ce que montre l'affichage pas à pas

La trace de `LargeurQueue.cpp` montre que la liste *visités* est toujours **en avance** sur la liste *affichés*. La différence entre les deux correspond exactement au contenu de la file : ce sont les nœuds découverts mais pas encore traités.

### 7.5 Quand utiliser (ou non) un parcours en largeur

| Situation | Choix |
|-----------|-------|
| Afficher ou traiter un arbre niveau par niveau | BFS avec `std::queue` |
| Plus court chemin en nombre d'arêtes (graphe non pondéré) | BFS |
| Arbre très large (la file devient énorme) | parcours en profondeur avec `std::stack` |
| Graphe pondéré | Dijkstra avec `std::priority_queue` |

## 8. Conclusion

- L'arbre est une vraie structure chaînée n-aire (`struct Noeud` avec `vector<Noeud*> enfants`), définie une seule fois dans `Arbre.h` et partagée par les trois programmes.
- Le parcours en largeur repose entièrement sur le comportement **FIFO** de la file : on obtient le même ordre avec `std::queue` et avec notre classe `File`.
- `std::queue` est un **adaptateur** : elle cache un conteneur et n'en garde que `push`, `pop`, `front` et `empty`. Écrire notre classe `File` sur un `vector` nous a montré concrètement ce mécanisme.
- Le **conteneur sous-jacent** compte pour les performances. `list` est nettement plus lente à cause d'une allocation par élément. `deque`, le choix par défaut, est le bon compromis entre vitesse et mémoire.
- Une mesure de temps n'a de sens que **sans affichage**, **répétée** et compilée **avec optimisation**.

**Pistes d'amélioration :** libérer la mémoire de la file maison, par exemple avec un buffer circulaire. Ajouter un parcours en profondeur avec `std::stack` pour comparer les ordres de visite et l'usage mémoire.

## 9. Annexe — sorties du terminal

<details>
<summary><code>.\build\LargeurQueue.exe</code> (extrait)</summary>

```text
Depart
  visites  : { 7 }
  file     : [ 7 ]

Etape 1 : on affiche 7
  nouveaux visites : 9 67 1
  visites  : { 7 9 67 1 }
  affiches : { 7 }
  file     : [ 9 67 1 ]

Etape 2 : on affiche 9
  nouveaux visites : 28 8
  visites  : { 7 9 67 1 28 8 }
  affiches : { 7 9 }
  file     : [ 67 1 28 8 ]

Etape 3 : on affiche 67
  nouveaux visites : aucun (feuille)
  visites  : { 7 9 67 1 28 8 }
  affiches : { 7 9 67 }
  file     : [ 1 28 8 ]

Etape 4 : on affiche 1
  nouveaux visites : 10 29 2 57
  visites  : { 7 9 67 1 28 8 10 29 2 57 }
  affiches : { 7 9 67 1 }
  file     : [ 28 8 10 29 2 57 ]
...
Etape 12 : on affiche 6
  nouveaux visites : aucun (feuille)
  visites  : { 7 9 67 1 28 8 10 29 2 57 40 6 }
  affiches : { 7 9 67 1 28 8 10 29 2 57 40 6 }
  file     : [ ]

Parcours en largeur : 7 9 67 1 28 8 10 29 2 57 40 6

===== Affichage par niveau =====
Niveau 0 (1 noeud(s)) : 7
Niveau 1 (3 noeud(s)) : 9 67 1
Niveau 2 (6 noeud(s)) : 28 8 10 29 2 57
Niveau 3 (2 noeud(s)) : 40 6

===== Deuxieme parcours du meme arbre =====
...
Parcours en largeur : 7 9 67 1 28 8 10 29 2 57 40 6
```
</details>

<details>
<summary><code>.\build\LargeurFileMaison.exe</code></summary>

```text
Depart | file : [ 7 ]
affiche 7 | enfants ajoutes : 9 67 1 | file (3) : [ 9 67 1 ]
affiche 9 | enfants ajoutes : 28 8 | file (4) : [ 67 1 28 8 ]
affiche 67 | enfants ajoutes : aucun | file (3) : [ 1 28 8 ]
affiche 1 | enfants ajoutes : 10 29 2 57 | file (6) : [ 28 8 10 29 2 57 ]
affiche 28 | enfants ajoutes : 40 | file (6) : [ 8 10 29 2 57 40 ]
affiche 8 | enfants ajoutes : aucun | file (5) : [ 10 29 2 57 40 ]
affiche 10 | enfants ajoutes : 6 | file (5) : [ 29 2 57 40 6 ]
affiche 29 | enfants ajoutes : aucun | file (4) : [ 2 57 40 6 ]
affiche 2 | enfants ajoutes : aucun | file (3) : [ 57 40 6 ]
affiche 57 | enfants ajoutes : aucun | file (2) : [ 40 6 ]
affiche 40 | enfants ajoutes : aucun | file (1) : [ 6 ]
affiche 6 | enfants ajoutes : aucun | file (0) : [ ]

Parcours en largeur : 7 9 67 1 28 8 10 29 2 57 40 6

Test file vide -> erreur attrapee : File::front() sur une file vide
```
</details>

<details>
<summary><code>.\build\LargeurPerf.exe</code> (essai 1)</summary>

```text
Arbre de 12 noeuds, 100000 repetitions
  queue<Noeud*, deque<Noeud*>> : 0.288 us / parcours
  queue<Noeud*, list<Noeud*>>  : 1.654 us / parcours
  File maison (vector)         : 0.668 us / parcours

Arbre de 1000000 noeuds, 20 repetitions
  queue<Noeud*, deque<Noeud*>> : 24676.425 us / parcours
  queue<Noeud*, list<Noeud*>>  : 243983.580 us / parcours
  File maison (vector)         : 19860.205 us / parcours
```
</details>
