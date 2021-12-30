#include <string>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <fstream>
#include <iostream>

using namespace std;

class Fichier
{
    string score;

public:
    string lire_fichier()
    {
        ifstream fichier("score.txt");

        if (fichier)
        {
            getline(fichier, score);
        }

        else
        {
            cout << "Impossible d'ouvvrir le fichier" << endl;
        }

        cout << " le score : " << endl;

        return score;
    }

    void ecrire_fichier(string nouveauscore)
    {
        string const nom_fichier("score.txt");
        ofstream fich(nom_fichier.c_str());

        if (fich)
        {
            fich << nouveauscore << endl;
        }
        else
        {
            cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
        }
    }
};
