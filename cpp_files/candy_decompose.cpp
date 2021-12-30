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


using namespace std;

const int windowWidth = 900;
const int windowHeight = 1000;
const double refreshPerSecond = 30;

 

/* ------ DO NOT EDIT BELOW HERE (FOR NOW) ------ */
class MainWindow : public Fl_Window
{
    Plateau plateau;
    Accueil acceuil_a_afficher;
    int selection_ecran=1;
    bool crushed = true;


public:
    bool affichage_ecran_acceuil = true;
    MainWindow() : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush")
    {
        Fl::add_timeout(1.0 / refreshPerSecond, Timer_CB, this);
        resizable(this);
    }
    void draw() override
    {
        
        Fl_Window::draw();
        if(affichage_ecran_acceuil){
            acceuil_a_afficher.draw();
            affichage_ecran_acceuil = false;
        }else{
            switch(selection_ecran){
                case 1:
                    if (crushed){
                        crushed = false;
                        plateau.draw();
                        plateau.rendre_plateau_stable();
                        cout << "test" << endl;
                        
                    
                    }
                    else{
                       plateau.draw(); 
                    }
                    break;
            }
            
        }

    }
    int handle(int event) override
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
        return 0;
    }
    static void Timer_CB(void *userdata)
    {
        MainWindow *o = (MainWindow *)userdata;
        if (o->affichage_ecran_acceuil){
            o->redraw();
            Fl::repeat_timeout(2, Timer_CB, userdata); // refaire chaque 2 seconde
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
