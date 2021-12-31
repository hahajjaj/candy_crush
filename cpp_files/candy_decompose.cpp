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

using namespace std;

const int windowWidth = 900;
const int windowHeight = 1000;
const double refreshPerSecond = 30;

/* ------ DO NOT EDIT BELOW HERE (FOR NOW) ------ */
class MainWindow : public Fl_Window
{
    Plateau plateau;
    Accueil acceuil_a_afficher;
    
    int selection_ecran = 1;
    menu_principal menu;
    bool crushed = true;

public:
    bool affichage_ecran_acceuil = true;
    MainWindow() : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush")
    {
        Fl::add_timeout(1.0 / refreshPerSecond, Timer_CB, this);
        resizable(this);
        menu.selection_ecran = &selection_ecran;
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
                if (crushed)
                {
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
                plateau.mouseMove(Point{Fl::event_x(), Fl::event_y()});
                return 1;
            case FL_PUSH:
                plateau.mouseClick(Point{Fl::event_x(), Fl::event_y()});
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
