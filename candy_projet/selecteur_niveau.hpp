/*
Nom et prénom : Ehlalouch Safouan 000514145, Hamza Hajjaj 000461105
Classe qui représente le selecteur de niveau
*/
#include <iostream>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>

#include <FL/Fl_Button.H>

using namespace std;

struct button_niveau
{

    Point center;
    Fl_PNG_Image *image_accueil;
    int ecran;
    string file_name;
    Text *numero_du_niveau;

    bool contains(Point p)
    {
        return p.x >= center.x && p.x < center.x + image_accueil->w() && p.y >= center.y && p.y < center.y + image_accueil->h();
    }
};

class selecteur_niveau
{
    vector<button_niveau> buttons;
    Fl_PNG_Image *background_menu;

    string *file_name;
    int *selection_ecran;
public:
    selecteur_niveau();
    void draw();
    void init_background();
    void init_button();
    void set_filename(string* new_filename);
    void set_selection_ecran(int* new_selection);
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    buttonn contains(Point p);
};

selecteur_niveau::selecteur_niveau()
{
    init_background();
    init_button();
}

void selecteur_niveau::set_selection_ecran(int* new_selection){
    selection_ecran = new_selection;
}

void selecteur_niveau::set_filename(string* new_filename){
    file_name = new_filename;
}

void selecteur_niveau::init_background()
{
    background_menu = new Fl_PNG_Image("elements_graphique/fond_ecran_menu.png");
}

void selecteur_niveau::init_button()
{
    ifstream myfile;
    myfile.open("levels/liste_of_levels.txt");
    string nom_niveau;
    int i = 0;
    int j = 0;
    if (myfile.is_open())
    {
        int numero_niveau = 0;
        while (myfile.good())

        {
            numero_niveau += 1;
            if (i == 4)
            {
                i = 0;
                j += 1;
            }
            Text *nombre_du_niveau = new Text(to_string(numero_niveau), {325 + (i * 160), 490 + (j * 160)}, 70);
            myfile >> nom_niveau;
            Fl_PNG_Image *niveau = new Fl_PNG_Image("elements_graphique/case_niveau.png");
            button_niveau case_niveau{{250 + (i * 160), 420 + (j * 160)}, niveau, 4, nom_niveau, nombre_du_niveau};
            buttons.push_back(case_niveau);
            i++;
        }
    }

    Fl_PNG_Image *quitter = new Fl_PNG_Image("elements_graphique/exit.png");
    button_niveau boutton_quitter{{820, 917}, quitter, 1, "rien"};
    buttons.push_back(boutton_quitter);
}

void selecteur_niveau::draw()
{
    background_menu->draw(0, 0);
    for (auto &b : buttons)
    {
        b.image_accueil->draw(b.center.x, b.center.y);
        if (b.file_name != "rien")
        {
            b.numero_du_niveau->draw();
        }
    }
}

void selecteur_niveau::mouseMove(Point mouseLoc)
{
}

void selecteur_niveau::mouseClick(Point mouseLoc)
{
    for (int i = 0; i < buttons.size(); i++)
    {
        bool ok = buttons[i].contains(mouseLoc);
        if (ok)
        {
            if (buttons[i].file_name != "rien")
            {
                *file_name = buttons[i].file_name;
            }
            *selection_ecran = buttons[i].ecran; // boutton clické
        }
    }
}