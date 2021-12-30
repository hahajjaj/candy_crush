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

#include "cell.hpp"
#include "fichier.hpp"
// #include "text_class.hpp"

using namespace std;

class Plateau
{
    vector<string> bonbons{"sprite/1.png", "sprite/2.png", "sprite/3.png", "sprite/4.png", "sprite/5.png", "sprite/6.png"};
    // vector<Cell> cells;
    vector<vector<Cell>> cells;
    bool coup_valide = false;
    int score = 0;
    bool var = true;
    bool plateau_stable = false;
    bool plateau_stable2 = false;
    bool is_anime = false;

public:
    Plateau();
    void draw();
    void initialize_neighbours();
    void crush_plateau();
    void initialize_grid();
    void translation_plateau_vers_bas();
    void selection_bonbon_tombe();
    void transformer_en_bombe();
    void generer_bonbons();
    void afficher_plateau_terminal();
    void gestion_de_score();
    void proposition_de_coup();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    void rendre_plateau_stable();
    void keyPressed(int /*keyCode*/) { exit(0); }
};

Plateau::Plateau()
{
    initialize_grid();
    initialize_neighbours();
    // ici on crush
}

void Plateau::gestion_de_score()
{
    string score_string = to_string(score);
    Text affichage_score(score_string, {425, 940});

    
    affichage_score.setFontSize(40);
    
    Fichier *fichier = new Fichier();
    string meilleur_score = fichier->lire_fichier();
    
    Text meilleur_score_text(meilleur_score, {600, 940});
    meilleur_score_text.setFontSize(40);
    int meilleure_score_int = stoi(meilleur_score); // reconverti le score string en int

    if (score > meilleure_score_int){
        meilleur_score_text.setString(score_string);
        fichier->ecrire_fichier(score_string);
        
    }

    affichage_score.draw();
    meilleur_score_text.draw();
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
}

void Plateau::proposition_de_coup()
{
    for (auto &v : cells)
        for (auto &c : v)
        {
            for (auto &voisin : c.neighbors)
            {
                if (c.tester_coup(voisin))
                {
                    c.Inversion(voisin);
                    c.Inversion(voisin);
                    break;
                }
            }
        }
}

void Plateau::selection_bonbon_tombe()
{
    int test = 0;
    for (int k = 0; k < 10; k++)
    {
        for (int y = 0; y < cells.size(); y++)
        {
            for (int x = 0; x < cells[0].size(); x++)
            {
                if (cells[y][x].bonbon->id == 0)
                {
                    for (int q = y; q != -1; q--)
                    {
                        if (cells[q][x].bonbon->id != 0)
                        {
                            cells[q][x].tombe = true;
                            test++;
                        }
                    }
                }
            }
        }
    }
    translation_plateau_vers_bas();
}

void Plateau::generer_bonbons()
{
    bool test = false;
    for (int x = 0; x < cells[0].size(); x++)
    {
        if (cells[0][x].bonbon->id == 0)
        {

            int nbr_aleatoire = (rand() % bonbons.size());
            const char *nom_fichier = bonbons[nbr_aleatoire].c_str();
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
            Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, nbr_aleatoire + 1};
            cells[0][x].setBonbon(newBonbon);
            test = true;
        }
    }
    if (test)
    {
        plateau_stable = false;
    }
    else
    {
        plateau_stable = true;
    }
}

void Plateau::translation_plateau_vers_bas()
{
    int i = 0;
    while (i < 100)
    {
        for (int y = 0; y < cells.size(); y++)
        {
            for (int x = 0; x < cells[0].size(); x++)
            {
                if (cells[y][x].tombe == true)
                {
                    cells[y][x].center.y += 1;
                    Fl::wait(0.00016);
                }
            }
        }
        i++;
    }
    //cas de la derniere ligne
    for (int x = 0; x < cells[0].size(); x++)
    {
        if (cells[cells.size() - 1][x].bonbon->id == 0 && cells[cells.size() - 2][x].bonbon->id != 0)
        {
            cells[cells.size() - 1][x].setBonbon(cells[cells.size() - 2][x].bonbon);
            cells[cells.size() - 2][x].center.y -= 100;

            // cells[y-2][x].bonbon->id = 0;
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
            Bonbon *newBonbon = new Bonbon{*sprite, "vide", 0};
            cells[cells.size() - 2][x].setBonbon(newBonbon);
        }
    }

    //cas de toutes les autres lignes
    for (int y = cells.size() - 1; y > 1; y--)
    {

        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y - 1][x].bonbon->id == 0 && cells[y - 2][x].bonbon->id != 0)
            {

                cells[y - 1][x].setBonbon(cells[y - 2][x].bonbon);
                cells[y - 2][x].center.y -= 100;

                // cells[y-2][x].bonbon->id = 0;
                Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
                Bonbon *newBonbon = new Bonbon{*sprite, "vide", 0};
                cells[y - 2][x].setBonbon(newBonbon);
            }
        }
    }

    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            cells[y][x].tombe = false;
        }
    }
}

void Plateau::crush_plateau()
{
    bool done = true;

    // pour crush les lignes
    for (int r = 0; r < cells.size(); r++)
    {
        for (int c = 0; c < (cells[r].size()) - 2; c++)
        {
            int num1 = abs(cells[r][c].bonbon->id);
            int num2 = abs(cells[r][c + 1].bonbon->id);
            int num3 = abs(cells[r][c + 2].bonbon->id);
            if (num1 == num2 && num2 == num3 && num1 != 0)
            {
                cells[r][c].bonbon->id = -num1;
                cells[r][c + 1].bonbon->id = -num2;
                cells[r][c + 2].bonbon->id = -num3;
                done = false;
                plateau_stable2 = false;
                plateau_stable = false;
                coup_valide = true;
            }
        }
    }

    //pour crush les colonnes
    for (int c = 0; c < cells[0].size(); c++)
    {
        for (int r = 0; r < (cells.size()) - 2; r++)
        {
            int num1 = abs(cells[r][c].bonbon->id);
            int num2 = abs(cells[r + 1][c].bonbon->id);
            int num3 = abs(cells[r + 2][c].bonbon->id);
            if (num1 == num2 && num2 == num3 && num1 != 0)
            {
                cells[r][c].bonbon->id = -num1;
                cells[r + 1][c].bonbon->id = -num2;
                cells[r + 2][c].bonbon->id = -num3;
                done = false;
                plateau_stable2 = false;
                plateau_stable = false;
                coup_valide = true;
            }
        }
    }
    if (done)
    {
        plateau_stable2 = true;
    }
}

void Plateau::afficher_plateau_terminal()
{
    cout << "#####################################################" << endl;
    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            cout << cells[y][x].bonbon->id << " ";
        }
        cout << endl;
    }
}

void Plateau::transformer_en_bombe()
{
    bool explosion = false;
    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y][x].bonbon->id < 0)
            {
                Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image("sprite/explosion2.png");
                Bonbon *newBonbon = new Bonbon{*sprite_explosion, "explosion", -6};
                cells[y][x].setBonbon(newBonbon);
                score += 20;
                explosion = true;
                Fl::wait();
            }
        }
    }
    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y][x].bonbon->id < 0)
            {
                Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image(nullptr);
                Bonbon *newBonbon = new Bonbon{*sprite_explosion, "vide", 0};
                cells[y][x].setBonbon(newBonbon);
            }
        }
    }
}

void Plateau::initialize_neighbours()
{
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
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

void Plateau::rendre_plateau_stable()
{
    plateau_stable2 = false;
    ;
    while (!plateau_stable2)
    {
        is_anime = true;
        crush_plateau(); // il met plateau stable 2 a false
        transformer_en_bombe();
        while (!plateau_stable)
        {
            selection_bonbon_tombe();
            generer_bonbons();
        }
    }
    is_anime = false;
}

void Plateau::draw()
{
    for (auto &v : cells)
        for (auto &c : v)
        {
            c.draw();
        }
    gestion_de_score();
}

void Plateau::mouseMove(Point mouseLoc)
{

    for (auto &v : cells)
        for (auto &c : v)
            c.mouseMove(mouseLoc);
}

void Plateau::mouseClick(Point mouseLoc)
{
    if(!is_anime){
for (auto &v : cells)
    {
        for (auto &c : v)
        {
            c.mouseClick(mouseLoc);
        }
    }
    rendre_plateau_stable();
    }
    
    
}