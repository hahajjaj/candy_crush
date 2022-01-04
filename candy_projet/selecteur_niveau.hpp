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

struct buttonnn
{
    
    Point center;
    Fl_PNG_Image *image_accueil;
    int ecran;
    string file_name;

    bool contains(Point p)
    {
        return p.x >= center.x && p.x < center.x + image_accueil->w() && p.y >= center.y && p.y < center.y + image_accueil->h();
    }
};

class selecteur_niveau
{
    vector<buttonnn> buttons;
    Fl_PNG_Image *background_menu;

public:
    int *selection_ecran;
    string *file_name;
    selecteur_niveau();
    void draw();
    void init_background();
    void init_button();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    buttonn contains(Point p);
};

selecteur_niveau::selecteur_niveau()
{
    init_background();
    init_button();
}

void selecteur_niveau::init_background()
{
    background_menu = new Fl_PNG_Image("elements_graphique/background_menu.png");
}

void selecteur_niveau::init_button()
{

    //boutton lancement partie
    Fl_PNG_Image *niveau1 = new Fl_PNG_Image("sprite/1.png");
    buttonnn nouvelle_partie{{250, 420}, niveau1, 4, "niveau1.txt"};
    buttons.push_back(nouvelle_partie);

    //boutton charger un niveau
    Fl_PNG_Image *niveau2 = new Fl_PNG_Image("sprite/2.png");
    buttonnn selection_niveau{{250, 589}, niveau2, 4, "niveau2.txt"};
    buttons.push_back(selection_niveau);

    //boutton editeur de niveau
    Fl_PNG_Image *niveau3 = new Fl_PNG_Image("sprite/4.png");
    buttonnn editeur_niveau_niveau{{250, 727}, niveau3, 4, "niveau3.txt"};
    buttons.push_back(editeur_niveau_niveau);

    Fl_PNG_Image *quitter = new Fl_PNG_Image("elements_graphique/exit.png");
    buttonnn boutton_quitter{{820, 917}, quitter, 1, "rien"};
    buttons.push_back(boutton_quitter);

}

void selecteur_niveau::draw()
{
    background_menu->draw(0, 0);
    for (auto &b : buttons)
    {
        b.image_accueil->draw(b.center.x, b.center.y);
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
        cout << ok << endl;
        if (ok)
        {
            if(buttons[i].file_name != "rien"){
                *file_name = buttons[i].file_name;
            }
            *selection_ecran = buttons[i].ecran; // boutton clické
            cout << "clické " << endl;
        }
    }
}