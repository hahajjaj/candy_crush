/*
Nom et prénom : Ehlalouch Safouan 000514145, Hamza Hajjaj 000461105
Classe qui représente le plateau de jeu
*/


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
    bool plateau_stable = false;

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

    Bonbon *restart_score;

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

    bool *bool_quitter_partie;

    //permet de savoir quel ecran afficher
    int *selection_ecran;

    //permet de savoir quel fichier traiter par les autres ecrans
    string *file_name;

public:
  
    Plateau(){};

    void set_selection_ecran(int *new_ecran);
    void set_quitter_partie(bool *changement);
    void set_filename(string *new_name);

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
    void ecrire_nouveau_meilleur_score(bool restart = false);
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

void Plateau::set_selection_ecran(int *new_ecran){
    selection_ecran = new_ecran;
}

void Plateau::set_quitter_partie(bool *changement){
    bool_quitter_partie = changement;
}

void Plateau::set_filename(string *new_name){
    file_name = new_name;
}

//méthode qui permet de chatger les sprites principaux du jeu ainsi que les boutons
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
    quitter = new Bonbon{"exit", 0};
    quitter->set_sprite(sprite);


    fond_case = new Fl_PNG_Image("elements_graphique/case.png");
    background = new Fl_PNG_Image("elements_graphique/fond_ecran.png");

    Fl_PNG_Image *sprite_restart = new Fl_PNG_Image("elements_graphique/delete.png");
    restart_score = new Bonbon{"restart_score", 0};
    restart_score->set_sprite(sprite_restart);

    image_partie_gagne = new Fl_PNG_Image("elements_graphique/partie_gagne.png");
    image_partie_perdu = new Fl_PNG_Image("elements_graphique/game_over.png");
    image_aucun_coup = new Fl_PNG_Image("elements_graphiques/aucun_mouvement.png");
    next_level = new Fl_PNG_Image("elements_graphique/check_image.png");
}

//methode qui permet de charger un niveau depuis un fichier
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
                    Bonbon *newBonbon = new Bonbon{nom_fichier, 100};
                    newBonbon->set_sprite(sprite);

                    newBonbon->set_special(true);
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                }
                else if (matrice == "*")
                {
                    nom_fichier = "sprite/glacage.png";
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{nom_fichier, 101};
                    newBonbon->set_sprite(sprite);

                    newBonbon->set_special(true);
                    newBonbon->set_glacage(true);
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                    glacage_restant += 1;
                }
                else if (matrice == "c")
                {
                    nom_fichier = "sprite/cerises.png";
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{nom_fichier, 102};
                    newBonbon->set_sprite(sprite);
                    newBonbon->set_cerise(true);
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                    cerise_restante += 1;
                    cerise = true;
                }
                else if (matrice == "d")
                {
                    nom_fichier = "sprite/glacage_double.png";
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{nom_fichier, 103};
                    newBonbon->set_sprite(sprite);
                    newBonbon->set_double_glacage(true);
                    newBonbon->set_special(true);
                    cells[j].push_back(Cell{Point{100 * (i), 100 * (j)}, 100, 100, newBonbon});
                    glacage_restant += 2;
                    glacage = true;
                }
                else
                {
                    nom_fichier = bonbons[stoi(matrice)].c_str();
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                    Bonbon *newBonbon = new Bonbon{nom_fichier, stoi(matrice) + 1};
                    newBonbon->set_sprite(sprite);
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

//methode qui permet d'écrire le meilleur score dans un fichier de niveau existant
//Un bool par defaut est fournis pour savoir si on remet à 0 le meilleur score du fichier ou non
void Plateau::ecrire_nouveau_meilleur_score(bool restart)
{
    if(restart){
        meilleur_score = 0;
    }
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

//methode qui initialise le score et meilleur score pour affichage
void Plateau::initialisation_score()
{

    affichage_score = new Text("0", {150, 920});
    affichage_score->setFontSize(40);

    meilleur_score_text = new Text(("Meilleur score : " + to_string(meilleur_score)), {510, 920});
    meilleur_score_text->setFontSize(30);
}

//methode qui permet la gestion du meilleur score au cours de la partie
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

//methode qui teste a chaque coup si la partie est gagne ou perdu
void Plateau::tester_partie_gagne()
{
    if(niveau_choix){
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
    
}

//methode qui affiche à l'écran les objectifs si le niveau est chargé depuis un fichier
void Plateau::affichage_objectifs()
{
    if(niveau_choix){
        affichage_coup_restant->setString(to_string(coup_restant));
        affichage_coup_restant->draw();
    }
    

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

// méthode qui initialise la matrice de jeu 
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
            Bonbon *newBonbon = new Bonbon{nom_fichier, nbr_aleatoire + 1};
            newBonbon->set_sprite(sprite);
            cells[x].push_back(Cell{Point{100 * (y), 100 * (x)}, 100, 100, newBonbon});
        }
    }
    Fl_PNG_Image *sprite = new Fl_PNG_Image("elements_graphique/exit.png");
    quitter = new Bonbon{"exit", 0};
    quitter->set_sprite(sprite);
}

//methode qui propose un coup après un certain laps de temps si le joueur ne fait pas de coup
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
            for (auto &voisin : c.getNeighbors())
            {
                if (!c.getBonbon()->get_special() && !voisin->getBonbon()->get_special() && voisin_valide(&c, voisin) && test_pour_proposition_coup(&c, voisin))
                {
                    for (int i = 0; i < 20; i++)
                    {
                        c.get_center()->y -= 1;
                        Fl::wait(0.01);
                    }
                    for (int i = 0; i < 20; i++)
                    {
                        c.get_center()->y += 1;
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
                Bonbon *newBonbon = new Bonbon{"vide", 0};
                newBonbon->set_sprite(sprite_explosion);

                c.setBonbon(newBonbon);
            }
        }
        rendre_plateau_stable();
    }
    is_anime = false;
}

//methode qui teste le plateau pour trouver un coup à proposer au joueur
bool Plateau::test_pour_proposition_coup(Cell *cell1, Cell *voisin)
{
    Bonbon *bonbon1 = cell1->getBonbon();
    Bonbon *bonbon2 = voisin->getBonbon();

    bool test_coup_valide = false;

    cell1->setBonbon(bonbon2);
    voisin->setBonbon(bonbon1);

    if (voisin->tester_coup(voisin))
    {
        test_coup_valide = true;
    }

    cell1->setBonbon(bonbon1);
    voisin->setBonbon(bonbon2);

    return test_coup_valide;
}

//methode qui teste si on a reussi à faire decendre un ingredient en bas du plateau
void Plateau::test_ingredient_derniere_ligne()
{
    for (int x = 0; x < cells.size(); x++)
    {
        if (cells[cells.size() - 1][x].getBonbon()->get_cerise())
        {
            chute_bonbons = true;
            cells[cells.size() - 1][x].getBonbon()->set_id(-1);
            cerise_restante -= 1;
        }
    }
}

//methode qui permet de selectionné tout les bonbons qui vont tomber avec une animation
void Plateau::selection_bonbon_tombe()
{
    chute_bonbons = false;

    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y][x].getBonbon()->get_id() == 0)
            {
                for (int q = y; q != -1; q--)
                {
                    if (cells[q][x].getBonbon()->get_id() != 0)
                    {
                        if (cells[q][x].getBonbon()->get_special())
                        {
                            break;
                        }
                        cells[q][x].set_tombe(true);
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

//methode qui permet de generer des bonbons sur la premiere ligne si une case est vide
void Plateau::generer_bonbons()
{
    for (int x = 0; x < cells[0].size(); x++)
    {
        if (cells[0][x].getBonbon()->get_id() == 0)
        {

            int nbr_aleatoire = (rand() % bonbons.size());
            const char *nom_fichier = bonbons[nbr_aleatoire].c_str();
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
            Bonbon *newBonbon = new Bonbon{nom_fichier, nbr_aleatoire + 1};
            newBonbon->set_sprite(sprite);

            cells[0][x].setBonbon(newBonbon);
            chute_bonbons = true;
        }
    }
}

//methode qui permet la translation d'un bonbon en diagonal
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
                if (cells[y][x].getBonbon()->get_id() != 0 && !cells[y][x].getBonbon()->get_special())
                {
                    // si on est pas sur la dernière colonne et que la diagonal droite est vide
                    if (x != cells.size() - 1 && cells[y + 1][x + 1].getBonbon()->get_id() == 0)
                    {
                        if (cells[y][x + 1].getBonbon()->get_special())
                        {
                            cells[y + 1][x + 1].set_selectionne(true);
                            cells[y][x].getBonbon()->set_diagonal(true);
                            cells[y][x].get_center()->y += 1;
                            cells[y][x].get_center()->x += 1;

                            Fl::wait(0.00016);
                        }
                    }

                    // Si on est pas sur la première colonne
                    else if (x != 0 && cells[y + 1][x - 1].getBonbon()->get_id() == 0)
                    {
                        if (cells[y][x - 1].getBonbon()->get_special() && !cells[y + 1][x - 1].get_selectionne())
                        {
                            cells[y][x].getBonbon()->set_diagonal(true);
                            cells[y][x].getBonbon()->set_gauche(true);
                            cells[y][x].get_center()->y += 1;
                            cells[y][x].get_center()->x -= 1;
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
            cells[y][x].set_selectionne(false);
            if (cells[y][x].getBonbon()->get_diagonal())
            {
                if (cells[y][x].getBonbon()->get_gauche())
                {
                    cells[y][x].getBonbon()->set_gauche(false);
                    cells[y][x].getBonbon()->set_diagonal(false);
                    cells[y + 1][x - 1].setBonbon(cells[y][x].getBonbon());
                    cells[y][x].get_center()->y -= 100;
                    cells[y][x].get_center()->x += 100;
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
                    Bonbon *newBonbon = new Bonbon{"vide", 0};
                    newBonbon->set_sprite(sprite);
                    cells[y][x].setBonbon(newBonbon);
                    chute_bonbons = true;
                }
                else
                {
                    cells[y][x].getBonbon()->set_diagonal(false);
                    cells[y + 1][x + 1].setBonbon(cells[y][x].getBonbon());
                    cells[y][x].get_center()->y -= 100;
                    cells[y][x].get_center()->x -= 100;
                    Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
                    Bonbon *newBonbon = new Bonbon{"vide", 0};
                    newBonbon->set_sprite(sprite);
                    cells[y][x].setBonbon(newBonbon);
                    chute_bonbons = true;
                }
            }
        }
    }
}

//methode qui permet la translation des bonbons vers le bas après que ceux ci ai été selectionné par "selection_bonbon_tombe"
void Plateau::translation_plateau_vers_bas()
{
    int i = 0;
    while (i < 100)
    {
        for (int y = 0; y < cells.size(); y++)
        {
            for (int x = 0; x < cells[0].size(); x++)
            {
                if (cells[y][x].get_tombe() == true)
                {
                    cells[y][x].get_center()->y += 1;
                    Fl::wait(0.00016);
                }
            }
        }
        i++;
    }
    //cas de la derniere ligne
    for (int x = 0; x < cells[0].size(); x++)
    {
        if (cells[cells.size() - 1][x].getBonbon()->get_id() == 0 && cells[cells.size() - 2][x].getBonbon()->get_id() != 0)
        {
            cells[cells.size() - 1][x].setBonbon(cells[cells.size() - 2][x].getBonbon());
            cells[cells.size() - 2][x].get_center()->y -= 100;

            // cells[y-2][x].bonbon->id = 0;
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
            Bonbon *newBonbon = new Bonbon{"vide", 0};
            newBonbon->set_sprite(sprite);
            cells[cells.size() - 2][x].setBonbon(newBonbon);
        }
    }

    //cas de toutes les autres lignes
    for (int y = cells.size() - 1; y > 1; y--)
    {

        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y - 1][x].getBonbon()->get_id() == 0 && !cells[y - 2][x].getBonbon()->get_special() && cells[y - 2][x].getBonbon()->get_id() != 0)
            {

                cells[y - 1][x].setBonbon(cells[y - 2][x].getBonbon());
                cells[y - 2][x].get_center()->y -= 100;

                // cells[y-2][x].bonbon->id = 0;
                Fl_PNG_Image *sprite = new Fl_PNG_Image(nullptr);
                Bonbon *newBonbon = new Bonbon{"vide", 0};
                    newBonbon->set_sprite(sprite);
                cells[y - 2][x].setBonbon(newBonbon);
            }
        }
    }

    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            cells[y][x].set_tombe(false);
        }
    }
}

//methode qui permet de savoir si le deuxieme bonbon est bien dans l'un des 4 coté du premier bonbon
bool Plateau::voisin_valide(Cell *cell1, Cell *voisin)
{
    vector<Point> vecteur_points{Point{-1, 0}, Point{1, 0}, Point{0, 1}, Point{0, -1}};
    for (auto &d : vecteur_points)
    {

        if ((cell1->get_center()->x) + (100 * d.x) == voisin->get_center()->x && (cell1->get_center()->y) + (100 * d.y) == voisin->get_center()->y)
        {
            return true;
        }
    }
    return false;
}

//methode qui permet d'effacer tous les bonbons qui forment un coup gagnant
void Plateau::crush_plateau()
{
    vector<vector<Point>> vecteur_points{{Point{-1, 0}, Point{1, 0}}, {Point{0, 1}, Point{0, -1}}};
    // Cell *cell1 = cell_p;
    plateau_stable = true;
    vector<Cell *> cells_a_effacer;

    for (auto &cell : cells)
    {
        for (auto &cell1 : cell)
        {
            if (!cell1.getBonbon()->get_cerise() && !cell1.getBonbon()->get_glacage() && !cell1.getBonbon()->get_special())
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
                            for (auto &voisin : cell_actuelle->getNeighbors())
                            {
                                if ((cell_actuelle->get_center()->x) + (100 * d.x) == voisin->get_center()->x && (cell_actuelle->get_center()->y) + (100 * d.y) == voisin->get_center()->y)
                                {
                                    if (cell_actuelle->getBonbon()->get_nom_sprite() == voisin->getBonbon()->get_nom_sprite()) // si même bonbon
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
                                plateau_stable = false;
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
        if (bonbon_objectif && bonbon_restant > 0 && c->getBonbon()->get_id() == bonbon_pour_objectif + 1)
        {
            bonbon_restant -= 1;
        }
        c->getBonbon()->set_id(-c->getBonbon()->get_id());
        for (auto &n : c->getNeighbors())
        {
            if (n->getBonbon()->get_glacage() && voisin_valide(c, n))
            {
                n->getBonbon()->set_id(-1);
                n->getBonbon()->set_glacage(false);
                n->getBonbon()->set_special(false);
                glacage_restant -= 1;
            }
        }
    }

    for (auto &c : cells_a_effacer)
    {

        for (auto &n : c->getNeighbors())
        {
            if (n->getBonbon()->get_double_glacage() && voisin_valide(c, n))
            {
                const char *nom_fichier = "sprite/glacage.png";
                Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
                Bonbon *newBonbon = new Bonbon{nom_fichier, 101};
                    newBonbon->set_sprite(sprite);
                newBonbon->set_special(true);
                newBonbon->set_glacage(true);
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
            cout << cells[y][x].get_tombe() << "  ";
        }
        cout << endl;
    }
}

//methode qui transforme les coups gagnant en bombe puis en case vide
void Plateau::transformer_en_bombe()
{
    bool explosion = false;
    int nombre_bonbon_crushe = 0;
    for (int y = 0; y < cells.size(); y++)
    {
        for (int x = 0; x < cells[0].size(); x++)
        {
            if (cells[y][x].getBonbon()->get_id() < 0)
            {
                if (!cells[y][x].getBonbon()->get_special())
                {
                    score += (20 + nombre_bonbon_crushe * 0.5);
                }
                if (cells[y][x].getBonbon()->get_glacage())
                {
                    score += 40;
                }
                else if (cells[y][x].getBonbon()->get_cerise())
                {
                    score += 60;
                }
                Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image("sprite/explosion.png");
                Bonbon *newBonbon = new Bonbon{"explosion", -6};
                newBonbon->set_sprite(sprite_explosion);
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
            if (cells[y][x].getBonbon()->get_id() < 0)
            {
                Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image(nullptr);
                Bonbon *newBonbon = new Bonbon{"vide", 0};
                    newBonbon->set_sprite(sprite_explosion);

                cells[y][x].setBonbon(newBonbon);
            }
        }
    }
}

// méthode qui initalise les voisins de chaque cellule
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

//methode qui permet de rendre un plateau à l'état stable après chaque coup d'un joueur.
void Plateau::rendre_plateau_stable()
{
    plateau_stable = false;
    while (!plateau_stable)
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

// dessine le plateau de jeu
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
    quitter->get_sprite()->draw(820, 917);
    restart_score->get_sprite()->draw(1100, 917);
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

        if (mouseLoc.x >= 1100 && mouseLoc.x < 1100 + restart_score->get_sprite()->w() && mouseLoc.y >= 917 && mouseLoc.y < 917 + restart_score->get_sprite()->h())
        {
            ecrire_nouveau_meilleur_score(true);
        }

        if (mouseLoc.x >= 820 && mouseLoc.x < 820 + quitter->get_sprite()->w() && mouseLoc.y >= 917 && mouseLoc.y < 917 + quitter->get_sprite()->h())
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
                    if (!c.getBonbon()->get_special())
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