#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>
#include <string>
#include <math.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <fstream>

#include "cell.hpp"

using namespace std;

class Plateau
{
    //vecteur de cells
    vector<vector<Cell>> cells;

    string nom_niveau;

    //le bouton quitter est stocké dans un bonbon par soucis de facilité, mais il ne sert à rien d'autre que quitter
    Bonbon *quitter;

    //image de fond et fond de case
    Fl_PNG_Image *fond_case;
    Fl_PNG_Image *background;

    Fl_PNG_Image *logo_glacage;
    Fl_PNG_Image *logo_cerises;

    //affichage du texte
    Text *affichage_score;
    Text *meilleur_score_text;

    Text *affichage_glacage_restant;
    Text *affichage_bonbons_restant;
    Text *affichage_cerise_restante;
    Text *affichage_coup_restant;

public:
    //pointeurs qui permettent de gérer l'affichage des fenêtres, quitter le niveau ou du choix de niveau
    bool *bool_quitter_partie;
    int *selection_ecran;
    string *file_name;

    Plateau(){};
    void draw();
    void charger_sprite();
    void inisialisation();
    void initialize_neighbours();
    void charger_niveau();
    void initialize_grid();
    void afficher_plateau_terminal();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    void keyPressed(int /*keyCode*/) { exit(0); }
};

// Plateau::Plateau()
// {
//     initialize_grid();
//     initialize_neighbours();
//     fichier = new Fichier();
//     string meilleur_score_fichier = fichier->lire_fichier();
//     meilleur_score = stoi(meilleur_score_fichier);
//     initialisation_score();
// }

void Plateau::charger_sprite()
{
    
}

void Plateau::charger_niveau()
{
}

void Plateau::inisialisation()
{
}

void Plateau::initialize_grid()
{
    for (int x = 0; x < 9; x++)
    {
        cells.push_back({});
        for (int y = 0; y < 9; y++)
        {
            int nbr_aleatoire = (rand() % bonbons.size());
            const char *nom_fichier = bonbons[nbr_aleatoire].c_str();
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
            Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, nbr_aleatoire + 1};
            cells[x].push_back(Cell{Point{100 * (y), 100 * (x)}, 100, 100, newBonbon});
        }
    }
    Fl_PNG_Image *sprite = new Fl_PNG_Image("elements_graphique/exit.png");
    quitter = new Bonbon{*sprite, "exit", 0};
}

void Plateau::afficher_plateau_terminal()
{
    cout << "#####################################################" << endl;
    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            cout << cells[y][x].tombe << "  ";
        }
        cout << endl;
    }
}

void Plateau::initialize_neighbours()
{
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            // code repris dans le tp du démineur
            vector<Cell *> neighbors;
            for (auto &shift : vector<Point>({
                     {-1, 0}, // gauche // The 8 neighbors relative to the cell
                     {1, 0},  // droite
                     {0, 1},  // bas
                     {0, -1}, // haut
                     {-1, 1},
                     {1, 1},
                     {1, -1},
                     {-1, -1},
                 }))
            {
                int neighborx = x + shift.x;
                int neighbory = y + shift.y;
                if (neighborx >= 0 &&
                    neighbory >= 0 &&
                    neighborx < cells.size() &
                        neighbory < cells[neighborx].size())
                {
                    neighbors.push_back(&cells[neighborx][neighbory]);
                }
                cells[x][y].setNeighbors(neighbors);
            }
        }
    }
}


void Plateau::draw()
{

    background->draw(0, 0);
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            fond_case->draw(100 * y, 100 * x);
        }
    }

    for (auto &v : cells)
        for (auto &c : v)
        {
            c.draw();
        }
    quitter->sprite.draw(820, 917);
}

void Plateau::mouseMove(Point mouseLoc)
{

    for (auto &v : cells)
        for (auto &c : v)
            c.mouseMove(mouseLoc);
}

void Plateau::mouseClick(Point mouseLoc)
{
        for (auto &v : cells)
        {
            for (auto &c : v)
            {
              

                    c.mouseClick(mouseLoc);
                
            }
        }
        
    
}