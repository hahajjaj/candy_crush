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
    
    public:
    int *selection_ecran;
    menu_principal();
    void draw();
    void init_button();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    buttonn contains(Point p);
};


menu_principal::menu_principal(){

    init_button();
}

void menu_principal::init_button(){
    for(int i = 0; i < 4; i++){
        Fl_PNG_Image *image_accueil = new Fl_PNG_Image("sprite/1.png");
        buttonn b{{300, i * 200}, image_accueil, i+1};
        buttons.push_back(b);
    }
}

void menu_principal::draw(){
    for (auto &b: buttons){
        b.image_accueil->draw(b.center.x, b.center.y);
    }
}

void menu_principal::mouseMove(Point mouseLoc){

}

void menu_principal::mouseClick(Point mouseLoc){
    for (int i = 0; i < 4; i++){
        bool ok = buttons[i].contains(mouseLoc);
        cout << ok << endl;
        if (ok){
            
            *selection_ecran = buttons[i].ecran; // boutton clické
            cout << "clické " << endl;
        }
    
    }

    
}


