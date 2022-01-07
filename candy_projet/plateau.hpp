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

    vector<string> nom_bonbon_ligne{"sprite_speciaux/lignes/1_h.png", "sprite_speciaux/lignes/2_h.png", "sprite_speciaux/lignes/3_h.png", "sprite_speciaux/lignes/4_h.png", "sprite_speciaux/lignes/5_h.png", "sprite_speciaux/lignes/6_h.png"};
    vector<string> nom_bonbon_vertical{"sprite_speciaux/lignes/1_v.png", "sprite_speciaux/lignes/2_v.png", "sprite_speciaux/lignes/3_v.png", "sprite_speciaux/lignes/4_v.png", "sprite_speciaux/lignes/5_v.png", "sprite_speciaux/lignes/6_v.png"};
    vector<string> nom_bonbon_sachet{"sprite_speciaux/sachets/1_s.png", "sprite_speciaux/sachets/2_s.png", "sprite_speciaux/sachets/3_s.png", "sprite_speciaux/lignes/4_s.png", "sprite_speciaux/sachets/5_s.png", "sprite_speciaux/sachets/6_s.png"};

    ///vecteur de bonbons speciaux
    vector<Fl_PNG_Image *> bonbons_ligne;
    vector<Fl_PNG_Image *> bonbons_vertical;
    vector<Fl_PNG_Image *> bonbons_sachets;

    //vecteur de cells
    vector<vector<Cell>> cells;
    bool coup_valide = false;

    //valeurs booleennes qui permettent de tester si le plateau est crushé et remplie ou non
    bool chute_bonbons = false;
    bool plateau_stable2 = false;

    //Booleen qui permet d'empecher le joueur de jouer tant qu'une animation est en cours
    bool is_anime = false;

    Fichier *fichier;

    string nom_niveau;

    //permet de savoir si le plateau est un niveau chargé ou non
    bool niveau_choix;

    //booleen qui permet de tester si la partie est gagné ou non
    bool partie_gagne = false;

    //le bouton quitter est stocké dans un bonbon par soucis de facilité, mais il ne sert à rien d'autre que quitter
    Bonbon *quitter;

    //image de fond et fond de case
    Fl_PNG_Image *fond_case;
    Fl_PNG_Image *background;

    Fl_PNG_Image *logo_glacage;
    Fl_PNG_Image *logo_cerises;

    Fl_PNG_Image *check;

    //affichage du texte
    Text *affichage_score;
    Text *meilleur_score_text;

    Text *affichage_glacage_restant;
    Text *affichage_bonbons_restant;
    Text *affichage_cerise_restante;
    Text *affichage_coup_restant;

    //bonbon a effacer dans l'objectif bonbon
    int bonbon_pour_objectif = -5;

    //objectifs à atteindre si choix de niveau
    int coup_restant = 0;
    int glacage_restant = 0;
    int bonbon_restant = 0;
    int cerise_restante = 0;
    int objectif_score = 0;

    bool limite_coup = false;

    //les scores
    int score = 0;
    int meilleur_score = 0;

public:
    //pointeurs qui permettent de gérer l'affichage des fenêtres, quitter le niveau ou du choix de niveau
    bool *bool_quitter_partie;
    int *selection_ecran;
    string *file_name;

    Plateau(){};
    void draw();
    void charger_sprite();
    void inisialisation(bool choix_niveau);
    void initialize_neighbours();
    void crush_plateau();
    void affichage_objectifs();
    void charger_niveau();
    void initialize_grid();
    void translation_plateau_vers_bas();
    void selection_bonbon_tombe();
    void transformer_en_bombe();
    void tester_partie_gagne();
    void generer_bonbons();
    bool test_glacage_a_effacer(Cell *cell1, Cell *voisin);
    string detecter_form();
    void afficher_plateau_terminal();
    void initialisation_score();
    void test_ingredient_derniere_ligne();
    void translation_diagonal();
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

void Plateau::charger_sprite()
{
    for (int i = 0; i < 6; i++)
    {
        Fl_PNG_Image *image = new Fl_PNG_Image(nom_bonbon_ligne[i].c_str());
        bonbons_ligne.push_back(image);

        Fl_PNG_Image *image2 = new Fl_PNG_Image(nom_bonbon_vertical[i].c_str());
        bonbons_vertical.push_back(image2);

        Fl_PNG_Image *image3 = new Fl_PNG_Image(nom_bonbon_sachet[i].c_str());
        bonbons_sachets.push_back(image3);
    }
}

void Plateau::charger_niveau()
{
    const char *nom_fichier;
    ifstream myfile;
    myfile.open(("levels/" + *file_name));
    string matrice;
    cells.push_back({});
    if (myfile.is_open())
    {
        int i = 0;
        int j = 0;
        while (myfile.good())
        {
            if (i == 9 && cells.size() < 9)
            {
                i = 0;
                j += 1;
                cells.push_back({});
            }

            myfile >> matrice;
            // ici on rempli la matrice de bonbons, glacage, murs et cerises
            if (cells.size() < 10 && i < 9)
            {
                if (matrice == "/")
                {
                    nom_fichier = "sprite/mur.png";
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, 100};
                    newBonbon->special = true;
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                }
                else if (matrice == "*")
                {
                    nom_fichier = "sprite/glacage.png";
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, 101};
                    newBonbon->special = true;
                    newBonbon->is_glacage = true;
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                    glacage_restant += 1;
                }
                else if (matrice == "c")
                {
                    nom_fichier = "sprite/cerises.png";
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, 102};
                    newBonbon->is_cerises = true;
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                    cerise_restante += 1;
                }
                else if (matrice == "d")
                {
                    nom_fichier = "sprite/glacage_double.png";
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, 103};
                    newBonbon->is_double_glacage = true;
                    newBonbon->special = true;
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                    glacage_restant += 2;
                }
                else
                {
                    nom_fichier = bonbons[stoi(matrice)].c_str();
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, stoi(matrice) + 1};
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                }

                i++;
            }
            //ici on récupère les objectifs de niveau une fois que la matrice 9x9 est remplie
            else
            {
                if (matrice == "r")
                {
                    myfile >> matrice;
                    coup_restant = stoi(matrice);
                    limite_coup = true;
                }
                else if (matrice == "b")
                {
                    myfile >> matrice;
                    bonbon_pour_objectif = stoi(matrice);
                    myfile >> matrice;
                    bonbon_restant = stoi(matrice);
                }
                else if (matrice == "s")
                {
                    myfile >> matrice;
                    objectif_score = stoi(matrice);
                }
            }
        }
    }
}

void Plateau::inisialisation(bool choix_niveau)
{
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
        
    }
    initialisation_score();
    initialize_neighbours();

    charger_sprite();
    if (choix_niveau)
    {
        check = new Fl_PNG_Image("elements_graphique/check2.png");
        affichage_bonbons_restant = new Text(to_string(bonbon_restant), {1050, 100}, 40);
        affichage_glacage_restant = new Text(to_string(glacage_restant), {1080, 450}, 40);
        logo_glacage = new Fl_PNG_Image("sprite/glacage.png");
        logo_cerises = new Fl_PNG_Image("sprite/cerises.png");
        affichage_cerise_restante = new Text(to_string(cerise_restante), {1080, 570}, 40);
        affichage_coup_restant = new Text(to_string(coup_restant), {1080, 690}, 40);

        Fl_PNG_Image *sprite = new Fl_PNG_Image("elements_graphique/exit.png");
        quitter = new Bonbon{*sprite, "exit", 0};
    }
    srand(10);
    fond_case = new Fl_PNG_Image("elements_graphique/case3.png");
    background = new Fl_PNG_Image("elements_graphique/fond_ecran.png");

    niveau_choix = choix_niveau;
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

void Plateau::tester_partie_gagne()
{

    if (limite_coup)
    {
        if (!(score >= objectif_score))
        {
            partie_gagne = false;
        }
        else
        {
            partie_gagne = true;
        }
        if (coup_restant >= 0 && cerise_restante == 0 && glacage_restant == 0)
        {
            partie_gagne = true;
        }
        else
        {
            partie_gagne = false;
        }
    }
    if (coup_restant == 0)
    {
        cout << "fin de partie" << endl;
        if (!partie_gagne)
        {
            cout << "vous avez perdu !" << endl;
        }
    }
    if (partie_gagne)
    {
        cout << "partie gagnée !!" << endl;
    }
}

void Plateau::affichage_objectifs()
{
    if (limite_coup)
    {
        affichage_coup_restant->setString(to_string(coup_restant));
        affichage_coup_restant->draw();
    }

    if (cerise_restante > 0)
    {
        affichage_cerise_restante->setString(to_string(cerise_restante));
        logo_cerises->draw(950, 520, 100, 100);
        affichage_cerise_restante->draw();
    }
    else
    {
        check->draw(1050, 520);
        logo_cerises->draw(950, 520);
    }

    if (glacage_restant > 0)
    {
        affichage_glacage_restant->setString(to_string(glacage_restant));
        logo_glacage->draw(950, 400, 100, 100);
        affichage_glacage_restant->draw();
    }
    else
    {
        check->draw(1050, 400);
        logo_glacage->draw(950, 400);
    }

    if (bonbon_restant > 0)
    {
        affichage_bonbons_restant->setString(("Bonbons restants" + bonbon_restant));
        affichage_bonbons_restant->draw();
    }
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
    // Fl::wait(6.000);
    bool test = false;
    for (auto &v : cells)
    {
        for (auto &c : v)
        {
            for (auto &voisin : c.neighbors)
            {
                c.Inversion(voisin);
                if (!c.tester_coup(voisin))
                {
                    c.Inversion(voisin);
                    // c.Inversion(voisin);
                    // c.Inversion(voisin);
                    cout << "coup valide" << endl;
                    test = true;
                    break;
                }
            }
        }
    }
    // if (!test)
    // {
    //     for (auto &v : cells)
    //     {
    //         for (auto &c : v)
    //         {
    //             Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image(nullptr);
    //             Bonbon *newBonbon = new Bonbon{*sprite_explosion, "vide", 0};
    //             c.setBonbon(newBonbon);
    //         }
    //     }
    //     rendre_plateau_stable();
    // }
}

void Plateau::test_ingredient_derniere_ligne()
{
    for (int x = 0; x < cells.size(); x++)
    {
        if (cells[cells.size() - 1][x].bonbon->is_cerises)
        {
            chute_bonbons = true;
            cells[cells.size() - 1][x].bonbon->id = -1;
            cerise_restante -= 1;
        }
    }
}

void Plateau::selection_bonbon_tombe()
{
    chute_bonbons = false;

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
                        if (cells[q][x].bonbon->special)
                        {
                            break;
                        }
                        cells[q][x].tombe = true;
                        chute_bonbons = true;
                    }
                }
            }
        }
    }
    // afficher_plateau_terminal();
    translation_plateau_vers_bas();
    translation_diagonal();
}

void Plateau::generer_bonbons()
{
    for (int x = 0; x < cells[0].size(); x++)
    {
        if (cells[0][x].bonbon->id == 0)
        {

            int nbr_aleatoire = (rand() % bonbons.size());
            const char *nom_fichier = bonbons[nbr_aleatoire].c_str();
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
            Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, nbr_aleatoire + 1};
            cells[0][x].setBonbon(newBonbon);
            chute_bonbons = true;
        }
    }
}

void Plateau::translation_diagonal()
{
    int i = 0;
    while (i < 100)
    {
        for (int y = 0; y < cells.size() - 1; y++)
        {
            for (int x = 0; x < cells[0].size(); x++)
            {
                // Si la cell actuelle n'est pas vide et qu'elle n'est pas un element special
                if (cells[y][x].bonbon->id != 0 && !cells[y][x].bonbon->special)
                {
                    // si on est pas sur la dernière colonne et que la diagonal droite est vide
                    if (x != cells.size() - 1 && cells[y + 1][x + 1].bonbon->id == 0)
                    {
                        if (cells[y][x + 1].bonbon->special)
                        {
                            cells[y + 1][x + 1].is_selectionne = true;
                            cells[y][x].bonbon->diagonal = true;
                            cells[y][x].center.y += 1;
                            cells[y][x].center.x += 1;

                            Fl::wait(0.00016);
                        }
                    }

                    // Si on est pas sur la première colonne
                    else if (x != 0 && cells[y + 1][x - 1].bonbon->id == 0)
                    {
                        if (cells[y][x - 1].bonbon->special && !cells[y + 1][x - 1].is_selectionne)
                        {
                            cells[y][x].bonbon->diagonal = true;
                            cells[y][x].bonbon->gauche = true;
                            cells[y][x].center.y += 1;
                            cells[y][x].center.x -= 1;
                            Fl::wait(0.00016);
                        }
                    }
                }
            }
        }
        i++;
    }

    for (int y = 0; y < cells.size() - 1; y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            cells[y][x].is_selectionne = false;
            if (cells[y][x].bonbon->diagonal)
            {
                if (cells[y][x].bonbon->gauche)
                {
                    cells[y][x].bonbon->gauche = false;
                    cells[y][x].bonbon->diagonal = false;
                    cells[y + 1][x - 1].setBonbon(cells[y][x].bonbon);
                    cells[y][x].center.y -= 100;
                    cells[y][x].center.x += 100;
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
                    Bonbon *newBonbon = new Bonbon{*sprite, "vide", 0};
                    cells[y][x].setBonbon(newBonbon);
                    chute_bonbons = true;
                }
                else
                {
                    cells[y][x].bonbon->diagonal = false;
                    cells[y + 1][x + 1].setBonbon(cells[y][x].bonbon);
                    cells[y][x].center.y -= 100;
                    cells[y][x].center.x -= 100;
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
                    Bonbon *newBonbon = new Bonbon{*sprite, "vide", 0};
                    cells[y][x].setBonbon(newBonbon);
                    chute_bonbons = true;
                }
            }
        }
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
            if (cells[y - 1][x].bonbon->id == 0 && !cells[y - 2][x].bonbon->special && cells[y - 2][x].bonbon->id != 0)
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

bool Plateau::test_glacage_a_effacer(Cell *cell1, Cell *voisin)
{
    vector<Point> vecteur_points{Point{-1, 0}, Point{1, 0}, Point{0, 1}, Point{0, -1}};
    for (auto &d : vecteur_points)
    {

        if ((cell1->center.x) + (100 * d.x) == voisin->center.x && (cell1->center.y) + (100 * d.y) == voisin->center.y)
        {
            return true;
        }
    }
    return false;
}

void Plateau::crush_plateau()
{
    vector<vector<Point>> vecteur_points{{Point{-1, 0}, Point{1, 0}}, {Point{0, 1}, Point{0, -1}}};
    // Cell *cell1 = cell_p;
    plateau_stable2 = true;
    vector<Cell *> cells_a_effacer;

    for (auto &cell : cells)
    {
        for (auto &cell1 : cell)
        {
            if (!cell1.bonbon->is_cerises && !cell1.bonbon->is_glacage && !cell1.bonbon->special)
            {
                int iteration_cote = 0; // Permet de savoir si on est horizontal ou vertical
                for (auto &p : vecteur_points)
                {
                    vector<Cell *> cell_provisoir;
                    for (auto &d : p)
                    {

                        iteration_cote++;
                        Cell *cell_actuelle = &cell1;
                        bool finished = false;
                        while (!finished)
                        {
                            bool trouve = false;
                            for (auto &voisin : cell_actuelle->neighbors)
                            {
                                if ((cell_actuelle->center.x) + (100 * d.x) == voisin->center.x && (cell_actuelle->center.y) + (100 * d.y) == voisin->center.y)
                                {
                                    if (cell_actuelle->bonbon->nom_sprite == voisin->bonbon->nom_sprite) // si même bonbon
                                    {
                                        if (cell_provisoir.size() == 0) // si le bonbon cliqué n'est pas encore dans le vecteur
                                        {
                                            cell_provisoir.push_back(cell_actuelle);
                                        }
                                        cell_provisoir.push_back(voisin);
                                        cell_actuelle = voisin;
                                        trouve = true;
                                    }
                                }
                            }
                            if (trouve == false) // si on a pas trouvé de bonbon
                                finished = true;
                        }
                    }

                    if (cell_provisoir.size() >= 3) // si au minimum 3 bonbons identiques
                    {
                        for (auto &c : cell_provisoir)
                        {
                            if (!(find(cells_a_effacer.begin(), cells_a_effacer.end(), c) != cells_a_effacer.end())) // ne pas mettre le bonbon cliqué deux fois
                            {
                                cells_a_effacer.push_back(c);
                                plateau_stable2 = false;
                                chute_bonbons = true;
                                coup_valide = true;
                            }
                        }
                    }
                    cell_provisoir.clear(); // clear le vecteur à la prochaine itération
                }
            }
        }
    }

    for (auto &c : cells_a_effacer)
    {
        c->bonbon->id = -c->bonbon->id;
        for (auto &n : c->neighbors)
        {
            if (n->bonbon->is_glacage && test_glacage_a_effacer(c, n))
            {
                n->bonbon->id = -1;
                n->bonbon->is_glacage = false;
                n->bonbon->special = false;
                glacage_restant -= 1;
            }
        }
    }

    for (auto &c : cells_a_effacer)
    {

        for (auto &n : c->neighbors)
        {
            if (n->bonbon->is_double_glacage && test_glacage_a_effacer(c, n))
            {
                const char *nom_fichier = "sprite/glacage.png";
                Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier, 101};
                newBonbon->special = true;
                newBonbon->is_glacage = true;
                n->setBonbon(newBonbon);
                glacage_restant -= 1;
                score += 40;
            }
        }
    }
}

// void Plateau::crush_plateau()
// {
//     bool done = true;

//     //inspiré d'un exercice sur leetcode après avoir fait une première version personnelle et fonctionnelle mais qui n'était pas opti.

//     // pour crush les lignes
//     for (int r = 0; r < cells.size(); r++)
//     {
//         for (int c = 0; c < (cells[r].size()) - 2; c++)
//         {
//             int num1 = abs(cells[r][c].bonbon->id);
//             int num2 = abs(cells[r][c + 1].bonbon->id);
//             int num3 = abs(cells[r][c + 2].bonbon->id);
//             //ici dans le test, l'id 102 représente celui de la cerise
//             if (num1 == num2 && num2 == num3 && num1 != 0 && num1 != 102)
//             {
//                 vector<Cell> cells_a_teste;

//                 cells[r][c].bonbon->id = -num1;
//                 cells_a_teste.push_back(cells[r][c]);
//                 cells[r][c + 1].bonbon->id = -num2;
//                 cells_a_teste.push_back(cells[r][c + 1]);
//                 cells[r][c + 2].bonbon->id = -num3;
//                 cells_a_teste.push_back(cells[r][c + 2]);

//                 done = false;
//                 plateau_stable2 = false;
//                 chute_bonbons = true;
//                 coup_valide = true;
//                 for (auto &c : cells_a_teste)
//                 {
//                     for (auto &n : c.neighbors)
//                     {
//                         if (n->bonbon->is_glacage && test_glacage_a_effacer(&c, n))
//                         {
//                             n->bonbon->id = -1;
//                             n->bonbon->is_glacage = false;
//                             n->bonbon->special = false;
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     //pour crush les colonnes
//     for (int c = 0; c < cells[0].size(); c++)
//     {
//         for (int r = 0; r < (cells.size()) - 2; r++)
//         {
//             int num1 = abs(cells[r][c].bonbon->id);
//             int num2 = abs(cells[r + 1][c].bonbon->id);
//             int num3 = abs(cells[r + 2][c].bonbon->id);
//             if (num1 == num2 && num2 == num3 && num1 != 0 && num1 != 102)
//             {
//                 vector<Cell> cells_a_teste;

//                 cells[r][c].bonbon->id = -num1;
//                 cells_a_teste.push_back(cells[r][c]);
//                 cells[r + 1][c].bonbon->id = -num2;
//                 cells_a_teste.push_back(cells[r + 1][c]);
//                 cells[r + 2][c].bonbon->id = -num3;
//                 cells_a_teste.push_back(cells[r + 2][c]);
//                 done = false;
//                 plateau_stable2 = false;
//                 chute_bonbons = true;
//                 coup_valide = true;

//                 for (auto &c : cells_a_teste)
//                 {
//                     for (auto &n : c.neighbors)
//                     {
//                         if (n->bonbon->is_glacage && test_glacage_a_effacer(&c, n))
//                         {
//                             n->bonbon->id = -1;
//                             n->bonbon->is_glacage = false;
//                             n->bonbon->special = false;
//                         }
//                     }
//                 }
//             }
//         }
//     }
//     if (done)
//     {
//         plateau_stable2 = true;
//     }
// }

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

void Plateau::transformer_en_bombe()
{
    bool explosion = false;
    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y][x].bonbon->id < 0)
            {
                score += 20;
                if (cells[y][x].bonbon->is_glacage)
                {
                    score += 40;
                }
                else if (cells[y][x].bonbon->is_cerises)
                {
                    score += 60;
                }
                Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image("sprite/explosion2.png");
                Bonbon *newBonbon = new Bonbon{*sprite_explosion, "explosion", -6};
                cells[y][x].setBonbon(newBonbon);

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

void Plateau::rendre_plateau_stable()
{
    plateau_stable2 = false;
    while (!plateau_stable2)
    {
        is_anime = true;
        crush_plateau();
        test_ingredient_derniere_ligne();
        transformer_en_bombe();
        while (chute_bonbons)
        {
            selection_bonbon_tombe();
            generer_bonbons();
        }
    }
    is_anime = false;
    proposition_de_coup();
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

    gestion_de_score();

    if (niveau_choix)
    {
        affichage_objectifs();
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
            *bool_quitter_partie = true;
            if (niveau_choix)
            {
                *selection_ecran = 3;
            }
            else
            {

                *selection_ecran = 1;
            }
        }
        for (auto &v : cells)
        {
            for (auto &c : v)
            {
                if (!c.bonbon->special)
                {

                    c.mouseClick(mouseLoc, coup_restant);
                }
            }
        }
        rendre_plateau_stable();
        tester_partie_gagne();
        
    }
}