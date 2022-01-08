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

    bool partie_perdu = false;

    //le bouton quitter est stocké dans un bonbon par soucis de facilité, mais il ne sert à rien d'autre que quitter
    Bonbon *quitter;

    //image de fond et fond de case
    Fl_PNG_Image *fond_case;
    Fl_PNG_Image *background;

    Fl_PNG_Image *logo_glacage;
    Fl_PNG_Image *logo_cerises;
    Fl_PNG_Image *logo_bonbon_objectif;

    Fl_PNG_Image *check;

    Fl_PNG_Image *image_partie_gagne;
    Fl_PNG_Image *image_partie_perdu;
    Fl_PNG_Image *image_aucun_coup;
    Fl_PNG_Image *next_level;

    //affichage du texte
    Text *affichage_score;
    Text *meilleur_score_text;

    Text *affichage_glacage_restant;
    Text *affichage_bonbons_restant;
    Text *affichage_cerise_restante;
    Text *affichage_coup_restant;

    //bonbon a effacer dans l'objectif bonbon
    int bonbon_pour_objectif = 0;

    //objectifs à atteindre si choix de niveau
    int coup_restant = 0;

    bool glacage = false;
    int glacage_restant = 0;

    bool bonbon_objectif = false;
    int bonbon_restant = 0;

    bool cerise = false;
    int cerise_restante = 0;

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
    void ecrire_nouveau_meilleur_score();
    bool test_pour_proposition_coup(Cell *cell1, Cell *voisin);
    bool voisin_valide(Cell *cell1, Cell *voisin);
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
    check = new Fl_PNG_Image("elements_graphique/check.png");

    affichage_bonbons_restant = new Text(to_string(bonbon_restant), {1080, 330}, 40);
    const char *nom_fichier = bonbons[bonbon_pour_objectif].c_str();
    logo_bonbon_objectif = new Fl_PNG_Image(nom_fichier);

    affichage_glacage_restant = new Text(to_string(glacage_restant), {1080, 450}, 40);
    logo_glacage = new Fl_PNG_Image("sprite/glacage.png");

    logo_cerises = new Fl_PNG_Image("sprite/cerises.png");
    affichage_cerise_restante = new Text(to_string(cerise_restante), {1080, 570}, 40);

    affichage_coup_restant = new Text(to_string(coup_restant), {1080, 690}, 40);

    Fl_PNG_Image *sprite = new Fl_PNG_Image("elements_graphique/exit.png");
    quitter = new Bonbon{*sprite, "exit", 0};

    fond_case = new Fl_PNG_Image("elements_graphique/case.png");
    background = new Fl_PNG_Image("elements_graphique/fond_ecran.png");

    image_partie_gagne = new Fl_PNG_Image("elements_graphique/partie_gagne.png");
    image_partie_perdu = new Fl_PNG_Image("elements_graphique/game_over.png");
    image_aucun_coup = new Fl_PNG_Image("elements_graphiques/aucun_mouvement.png");
    next_level = new Fl_PNG_Image("elements_graphique/check_image.png");
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
                    cerise = true;
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
                    glacage = true;
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
                }
                else if (matrice == "b")
                {
                    myfile >> matrice;
                    bonbon_pour_objectif = stoi(matrice);
                    myfile >> matrice;
                    bonbon_restant = stoi(matrice);
                    bonbon_objectif = true;
                }
                else if (matrice == "s")
                {
                    myfile >> matrice;
                    meilleur_score = stoi(matrice);
                }
            }
        }
    }
}

void Plateau::ecrire_nouveau_meilleur_score()
{
    ofstream myfile2;
    ifstream myfile;
    myfile.open(("levels/" + *file_name));
    char oldname[] = "levels/temp.txt";
    string newname_temp = ("levels/" + *file_name);

    char newname[newname_temp.size() + 1];
    strcpy(newname, newname_temp.c_str());
    myfile2.open("levels/temp.txt");
    bool test2 = false;
    while (myfile.good())
    {

        string test;
        // myfile >> test;
        test = myfile.get();
        // test+="\n";
        if (test[0] == 's')
        {
            cout << "voila le meilleur score " << meilleur_score << endl;
            myfile2 << ("s " + to_string(meilleur_score));
            test2 = true;
        }
        if (!test2)
        {

            myfile2 << test;
        }
    }
    myfile.close();
    remove(newname);
    rename(oldname, newname);
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

    niveau_choix = choix_niveau;
    srand(10);
}

void Plateau::initialisation_score()
{

    affichage_score = new Text("0", {150, 920});
    affichage_score->setFontSize(40);

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
    if (coup_restant >= 0 && cerise_restante == 0 && glacage_restant == 0 && bonbon_restant == 0)
    {
        
        partie_gagne = true;
        
    }
    else
    {
        partie_gagne = false;
    }

    if (coup_restant == 0)
    {
        if (!partie_gagne)
        {
            partie_perdu = true;
        }
    }
    if (!partie_perdu && !partie_gagne)
    {
        proposition_de_coup();
    }
}

void Plateau::affichage_objectifs()
{
    affichage_coup_restant->setString(to_string(coup_restant));
    affichage_coup_restant->draw();

    //bool qui test si on affiche l'objectif de cerises
    if (cerise)
    {
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
    }

    //bool qui test si on affiche l'objectif de glacage
    if (glacage)
    {
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
    }

    if (bonbon_objectif)
    {
        affichage_bonbons_restant->setString(to_string(bonbon_restant));
        logo_bonbon_objectif->draw(950, 280);
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
    for (int i = 0; i < 100; i++)
    {
        Fl::wait(1);
    }
    is_anime = true;
    int valeur_factice = 0;
    bool test = false;
    vector<vector<Cell>> vect_temp = cells;
    for (auto &v : cells)
    {
        for (auto &c : v)
        {
            for (auto &voisin : c.neighbors)
            {
                if (!c.bonbon->special && !voisin->bonbon->special && voisin_valide(&c, voisin) && test_pour_proposition_coup(&c, voisin))
                {
                    for (int i = 0; i < 20; i++)
                    {
                        c.center.y -= 1;
                        Fl::wait(0.01);
                    }
                    for (int i = 0; i < 20; i++)
                    {
                        c.center.y += 1;
                        Fl::wait(0.01);
                    }
                    test = true;
                    break;
                }
            }
            if (test)
            {
                break;
            }
        }
        if (test)
        {
            break;
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
    is_anime = false;
}

bool Plateau::test_pour_proposition_coup(Cell *cell1, Cell *voisin)
{
    Bonbon *bonbon1 = cell1->bonbon;
    Bonbon *bonbon2 = voisin->bonbon;

    bool test_coup_valide = false;

    cell1->setBonbon(bonbon2);
    voisin->setBonbon(bonbon1);

    if (voisin->tester_coup(voisin))
    {
        test_coup_valide = true;
        cout << "true" << endl;
    }

    cell1->setBonbon(bonbon1);
    voisin->setBonbon(bonbon2);

    return test_coup_valide;
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

bool Plateau::voisin_valide(Cell *cell1, Cell *voisin)
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
        if (bonbon_objectif && bonbon_restant > 0 && c->bonbon->id == bonbon_pour_objectif + 1)
        {
            bonbon_restant -= 1;
        }
        c->bonbon->id = -c->bonbon->id;
        for (auto &n : c->neighbors)
        {
            if (n->bonbon->is_glacage && voisin_valide(c, n))
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
            if (n->bonbon->is_double_glacage && voisin_valide(c, n))
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
    int nombre_bonbon_crushe = 0;
    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y][x].bonbon->id < 0)
            {
                if (!cells[y][x].bonbon->special)
                {
                    score += (20 + nombre_bonbon_crushe * 0.5);
                }
                if (cells[y][x].bonbon->is_glacage)
                {
                    score += 40;
                }
                else if (cells[y][x].bonbon->is_cerises)
                {
                    score += 60;
                }
                Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image("sprite/explosion.png");
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
}

void Plateau::draw()
{
    // affichage de l'image d'arrière plan du jeu
    background->draw(0, 0);

    //affichage du fond de chaque case du plateau
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            fond_case->draw(100 * y, 100 * x);
        }
    }

    //affichage de chaque bonbon stocké dans une cell
    for (auto &v : cells)
        for (auto &c : v)
        {
            c.draw();
        }

    //affichage du bouton pour quitter la partie
    quitter->sprite.draw(820, 917);

    //affichage du score et du meilleur score
    gestion_de_score();

    //affichage des objectifs pour chaque niveau
    affichage_objectifs();

    if (partie_gagne)
    {
        image_partie_gagne->draw(50, 181);
        next_level->draw(680, 548);
    }
    else if (partie_perdu)
    {
        image_partie_perdu->draw(50, 181);
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
        if (partie_gagne || partie_perdu)
        {
            if (mouseLoc.x >= 680 && mouseLoc.x < 680 + next_level->w() && mouseLoc.y >= 548 && mouseLoc.y < 548 + next_level->h())
            {
                ecrire_nouveau_meilleur_score();
                *bool_quitter_partie = true;
                int number_of_level = file_name->at(6) - 48;
                string number_of_next_level = to_string(number_of_level + 1);
                file_name->replace(6, 1, number_of_next_level);
            }
        }
        if (mouseLoc.x >= 820 && mouseLoc.x < 820 + quitter->sprite.w() && mouseLoc.y >= 917 && mouseLoc.y < 917 + quitter->sprite.h())
        {
            *bool_quitter_partie = true;
            if (*selection_ecran == 4)
            {
                *selection_ecran = 3;
            }
            else
            {

                *selection_ecran = 1;
            }
        }
        if (!partie_perdu && !partie_gagne)
        {
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
        }
        else
        {
        }

        rendre_plateau_stable();
        tester_partie_gagne();
    }
}