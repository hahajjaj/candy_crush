#include <FL/Fl_PNG_Image.H>
#include <string>
#include <algorithm>

using namespace std;

class Bonbon
{

public:
    string nom_sprite;
    Fl_PNG_Image &sprite;
    int id;
    bool special = false;
    bool diagonal = false;
    bool gauche = false;
    Bonbon(Fl_PNG_Image &spri, string nom_sprite, int id);
};

Bonbon::Bonbon(Fl_PNG_Image &spri, string nom_sprite, int id) : sprite{spri}, nom_sprite{nom_sprite}, id{id} {}

