/*
Nom et prénom : Ehlalouch Safouan 000514145, Hamza Hajjaj 000461105
Classe qui représente le menu d'acceuil du jeu
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

using namespace std;

class Accueil
{

public:
    Accueil(){};
    void draw();
};

void Accueil::draw()
{
    Fl_PNG_Image image_accueil("sprite/ecran_accueil_candy.png");
    image_accueil.draw(0, 0);
    
};
