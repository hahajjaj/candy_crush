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
#include "fichier.hpp"
// #include "text_class.hpp"

using namespace std;

class Plateau
{   
    //vecteur de fichiers
    vector<string> bonbons{"sprite/1.png", "sprite/2.png", "sprite/3.png", "sprite/4.png", "sprite/5.png", "sprite/6.png"};

    vector<string> nom_bonbon_ligne{"sprite_speciaux/lignes/1_h.png","sprite_speciaux/lignes/2_h.png","sprite_speciaux/lignes/3_h.png","sprite_speciaux/lignes/4_h.png", "sprite_speciaux/lignes/5_h.png","sprite_speciaux/lignes/6_h.png"};
    vector<string> nom_bonbon_vertical{"sprite_speciaux/lignes/1_v.png","sprite_speciaux/lignes/2_v.png","sprite_speciaux/lignes/3_v.png", "sprite_speciaux/lignes/4_v.png","sprite_speciaux/lignes/5_v.png","sprite_speciaux/lignes/6_v.png"};
    vector<string> nom_bonbon_sachet{"sprite_speciaux/sachets/1_s.png","sprite_speciaux/sachets/2_s.png","sprite_speciaux/sachets/3_s.png", "sprite_speciaux/lignes/4_s.png","sprite_speciaux/sachets/5_s.png","sprite_speciaux/sachets/6_s.png"};

    ///vecteur de bonbons speciaux
    vector<Fl_PNG_Image*> bonbons_ligne;
    vector<Fl_PNG_Image*> bonbons_vertical;
    vector<Fl_PNG_Image*> bonbons_sachets;

    //png pour le glacage
    Fl_PNG_Image *glacage;

    //vecteur de cells
    vector<vector<Cell>> cells;
    bool coup_valide = false;
    int score = 0;
    int meilleur_score = 0;
    bool plateau_stable = false;
    bool plateau_stable2 = false;
    bool is_anime = false;
    Text *affichage_score;
    Text *meilleur_score_text;
    Fichier *fichier;
    string nom_niveau;
    bool niveau_choix;
    Bonbon *quitter;
    Fl_PNG_Image *fond_case;
    Fl_PNG_Image *background;

public:
    bool *test;
    int *selection_ecran;

    Plateau(){};
    void draw();
    void charger_sprite();
    void inisialisation(bool choix_niveau);
    void initialize_neighbours();
    void crush_plateau();
    void charger_niveau();
    void initialize_grid();
    void translation_plateau_vers_bas();
    void selection_bonbon_tombe();
    void transformer_en_bombe();
    void generer_bonbons();
    string detecter_form();
    void afficher_plateau_terminal();
    void initialisation_score();
    void gestion_de_score();
    void proposition_de_coup();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    void rendre_plateau_stable();
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


string detecter_form(){
    vector <vector <Cell>> ligne_negatif;

    for (int i = 0; i < cells.size(); i++){
        for (int col = 0; i < cells[i].size();){
            if (cells[j][col].bonbon->id < 0){ 
            ligne_negatif.push_back(cells[i]);}

        }
    }

    for (int i = 0; i < ligne_negatif.size())

}

void Plateau::charger_sprite(){
    for (int i = 0; i < 6; i++){
        Fl_PNG_Image *image = new Fl_PNG_Image(nom_bonbon_ligne[i].c_str());
        bonbons_ligne.push_back(image);

        Fl_PNG_Image *image2 = new Fl_PNG_Image(nom_bonbon_vertical[i].c_str());
        bonbons_vertical.push_back(image2);

        Fl_PNG_Image *image3 = new Fl_PNG_Image(nom_bonbon_sachet[i].c_str());
        bonbons_sachets.push_back(image3);
    }

    glacage = new Fl_PNG_Image("sprite/glacage.png");
}

void Plateau::charger_niveau()
{
    ifstream myfile;
    myfile.open("levels/niveau1.txt");
    nom_niveau = "niveau1.txt";
    string matrice;
    cells.push_back({});
    if (myfile.is_open())
    {
        int i = 0;
        int j = 0;
        while (myfile.good())
        {
            if (i == 9)
            {
                i = 0;
                j += 1;
                cells.push_back({});
            }
            myfile >> matrice;
            if(matrice != "/"){
                const char *nom_fichier = bonbons[stoi(matrice)].c_str();
            }else{
                const char *nom_fichier = "sprite/glacage.png";
            }
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
            Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, stoi(matrice) + 1};
            cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
            i++;
        }
    }
    Fl_PNG_Image *sprite = new Fl_PNG_Image("elements_graphique/exit.png");
    quitter = new Bonbon{*sprite, "exit", 0};
}

void Plateau::inisialisation(bool choix_niveau)
{
    charger_sprite();
    srand(10);
    fond_case = new Fl_PNG_Image("elements_graphique/case3.png");
    background = new Fl_PNG_Image("elements_graphique/background_partie.png");

    niveau_choix = choix_niveau;
    if (choix_niveau)
    {
        charger_niveau();
    }
    else
    {
        initialize_grid();
        fichier = new Fichier();
        string meilleur_score_fichier = fichier->lire_fichier();
        meilleur_score = stoi(meilleur_score_fichier);
        initialisation_score();
    }
    initialize_neighbours();
}

void Plateau::initialisation_score()
{

    affichage_score = new Text("0", {150, 920});
    affichage_score->setFontSize(30);

    meilleur_score_text = new Text(("Meilleur score : " + to_string(meilleur_score)), {510, 920});
    meilleur_score_text->setFontSize(30);
}

void Plateau::gestion_de_score()
{
    //affichage du score actuel
    affichage_score->setString(("score : " + to_string(score)));

    //affichage du meilleur score

    if (score >= meilleur_score)
    {
        meilleur_score = score;
        meilleur_score_text->setString("Meilleur score : " + to_string(meilleur_score));
        fichier->ecrire_fichier(to_string(score));
    }

    affichage_score->draw();
    meilleur_score_text->draw();
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

void Plateau::proposition_de_coup()
{
    bool test = false;
    for (auto &v : cells)
    {
        for (auto &c : v)
        {
            for (auto &voisin : c.neighbors)
            {
                if (c.tester_coup(voisin))
                {
                    c.Inversion(voisin);
                    c.Inversion(voisin);
                    test = true;
                    break;
                }
            }
        }
    }
    if (!test)
    {
        for (auto &v : cells)
        {
            for (auto &c : v)
            {
                Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image(nullptr);
                Bonbon *newBonbon = new Bonbon{*sprite_explosion, "vide", 0};
                c.setBonbon(newBonbon);
            }
        }
        rendre_plateau_stable();
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
                Fl::wait(1);
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
    if (!niveau_choix)
    {

        gestion_de_score();
    }
}

void Plateau::mouseMove(Point mouseLoc)
{

    for (auto &v : cells)
        for (auto &c : v)
            c.mouseMove(mouseLoc);
}

void Plateau::mouseClick(Point mouseLoc)
{

    if (!is_anime)
    {
        if (mouseLoc.x >= 820 && mouseLoc.x < 820 + quitter->sprite.w() && mouseLoc.y >= 917 && mouseLoc.y < 917 + quitter->sprite.h())
        {
            *test = true;
            *selection_ecran = 1;
        }
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