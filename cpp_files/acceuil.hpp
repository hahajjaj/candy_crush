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

using namespace std;

class Accueil{
    
    
    public:
    Accueil(){};
    void draw();
};

void Accueil::draw(){
    Fl_PNG_Image image_accueil("sprite/1.png");
    image_accueil.draw(500,500,400,400);
};