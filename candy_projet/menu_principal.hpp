//
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

struct buttonn{
    Point center;
    Fl_PNG_Image *image_accueil;
    int ecran;

    bool contains(Point p){
    return p.x >= center.x && p.x < center.x + image_accueil->w() && p.y >= center.y && p.y < center.y + image_accueil->h();
}
};



class menu_principal
{
    vector <buttonn> buttons;
    Fl_PNG_Image *background_menu;
    public:
    int *selection_ecran;
    menu_principal();
    void draw();
    void init_background();
    void init_button();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    buttonn contains(Point p);
};


menu_principal::menu_principal(){
    init_background();
    init_button();
}

void menu_principal::init_background(){
    background_menu = new Fl_PNG_Image("elements_graphique/fond_ecran_menu.png");
}

void menu_principal::init_button(){

    //boutton lancement partie
    Fl_PNG_Image *nv_partie = new Fl_PNG_Image("elements_graphique/1.png");
    buttonn nouvelle_partie{{400, 420}, nv_partie, 2};
    buttons.push_back(nouvelle_partie);

    //boutton charger un niveau
    Fl_PNG_Image *selec_niveau = new Fl_PNG_Image("elements_graphique/2.png");
    buttonn selection_niveau{{400, 589}, selec_niveau, 3};
    buttons.push_back(selection_niveau);

    //boutton editeur de niveau
    Fl_PNG_Image *edit_niveau = new Fl_PNG_Image("elements_graphique/4.png");
    buttonn editeur_niveau_niveau{{400, 727}, edit_niveau, 2};
    buttons.push_back(editeur_niveau_niveau);
}

void menu_principal::draw(){
    background_menu->draw(0,0);
    for (auto &b: buttons){
        b.image_accueil->draw(b.center.x, b.center.y);
    }
}

void menu_principal::mouseMove(Point mouseLoc){

}

void menu_principal::mouseClick(Point mouseLoc){
    for (int i = 0; i < buttons.size(); i++){
        bool ok = buttons[i].contains(mouseLoc);
        if (ok){
            
            *selection_ecran = buttons[i].ecran; // boutton clické
        }
    
    }

    
}


