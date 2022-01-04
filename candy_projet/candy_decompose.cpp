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

#include "plateau.hpp"
#include "acceuil.hpp"
#include "menu_principal.hpp"
#include "selecteur_niveau.hpp"

using namespace std;

const int windowWidth = 1200;
const int windowHeight = 1000;
const double refreshPerSecond = 25;

/* ------ DO NOT EDIT BELOW HERE (FOR NOW) ------ */
class MainWindow : public Fl_Window
{
    Plateau plateau;
    Accueil acceuil_a_afficher;
    menu_principal menu;
    selecteur_niveau selec_niveau;

    int selection_ecran = 1;

    string file_name;

    bool crushed = true;
    // bool partie_lance = false;
    bool bool_quitter_partie = false;

public:
    bool affichage_ecran_acceuil = true;
    MainWindow() : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush")
    {
        Fl::add_timeout(1.0 / refreshPerSecond, Timer_CB, this);
        resizable(this);
        menu.selection_ecran = &selection_ecran;
        plateau.selection_ecran = &selection_ecran;
        selec_niveau.selection_ecran = &selection_ecran;
        selec_niveau.file_name = &file_name;
        plateau.bool_quitter_partie = &bool_quitter_partie;
    }

    void remettre_plateau_zero_noniveau()
    {
        if (bool_quitter_partie)
        {
            // partie_lance = false;
            bool_quitter_partie = false;
            crushed = true;
            // (&plateau)->~Plateau();
            // new (&plateau) Plateau();
            plateau = Plateau();
            plateau.selection_ecran = &selection_ecran;
            plateau.bool_quitter_partie = &bool_quitter_partie;
            cout << "click" << endl;
        }
    }

    void draw() override
    {

        Fl_Window::draw();
        if (affichage_ecran_acceuil)
        {
            acceuil_a_afficher.draw();
            affichage_ecran_acceuil = false;
        }
        else
        {
            switch (selection_ecran)
            {
            case 1: // menu principal
                menu.draw();
                break;
            

            case 2:

                if (crushed)
                {
                    // partie_lance = true;
                    plateau.inisialisation(false); // pas de choix de niveau
                    crushed = false;
                    plateau.draw();
                    plateau.rendre_plateau_stable();
                }
                else
                {
                    plateau.draw();
                }
                break;
            
            case 3:
                selec_niveau.draw();
                break;

            case 4:
                if (crushed)
                {
                    // partie_lance = true;
                    plateau.file_name = &file_name;
                    plateau.inisialisation(true); // choix de niveau
                    crushed = false;
                    plateau.draw();
                    plateau.rendre_plateau_stable();
                }
                else
                {
                    plateau.draw();
                }
                break;
            }
        }
    }
    int handle(int event) override
    {
        switch (selection_ecran)
        {
        case 1:
        {
            switch (event)
            {
            case FL_MOVE:
                menu.mouseMove(Point{Fl::event_x(), Fl::event_y()});
                return 1;
            case FL_PUSH:
                menu.mouseClick(Point{Fl::event_x(), Fl::event_y()});
                return 1;
                // case FL_KEYDOWN:
                //     plateau.keyPressed(Fl::event_key());
                //     return 1;
            }
        }
        break;
        case 2:
        {
            switch (event)
            {
            case FL_MOVE:
                plateau.mouseMove(Point{Fl::event_x(), Fl::event_y()});
                return 1;
            case FL_PUSH:
                plateau.mouseClick(Point{Fl::event_x(), Fl::event_y()});
                remettre_plateau_zero_noniveau();
                return 1;
            case FL_KEYDOWN:
                plateau.keyPressed(Fl::event_key());
                return 1;
            }
        }
        break;

        case 3:
        {
            switch (event)
            {
            case FL_MOVE:
                selec_niveau.mouseMove(Point{Fl::event_x(), Fl::event_y()});
                return 1;
            case FL_PUSH:
                selec_niveau.mouseClick(Point{Fl::event_x(), Fl::event_y()});
                return 1;
                // case FL_KEYDOWN:
                //     plateau.keyPressed(Fl::event_key());
                //     return 1;
            }
        }
        break;

        case 4:
        {
            switch (event)
            {
            case FL_MOVE:
                plateau.mouseMove(Point{Fl::event_x(), Fl::event_y()});
                return 1;
            case FL_PUSH:
                plateau.mouseClick(Point{Fl::event_x(), Fl::event_y()});
                remettre_plateau_zero_noniveau();

                return 1;
            case FL_KEYDOWN:
                plateau.keyPressed(Fl::event_key());
                return 1;
            }
        }
        break;
        }
        return 0;
    }

    static void Timer_CB(void *userdata)
    {
        MainWindow *o = (MainWindow *)userdata;
        if (o->affichage_ecran_acceuil)
        {
            o->redraw();
            Fl::repeat_timeout(1, Timer_CB, userdata); // refaire chaque 2 seconde
        }
        else
        {
            o->redraw();
            Fl::repeat_timeout(1.0 / refreshPerSecond, Timer_CB, userdata);
        }
    }

    //o->acceuil_a_afficher.draw();
};

int main(int argc, char *argv[])
{

    MainWindow window;
    window.show(argc, argv);
    return Fl::run();
}
