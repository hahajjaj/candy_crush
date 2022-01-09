#include <FL/Fl_PNG_Image.H>
#include <string>
#include <algorithm>

using namespace std;

class Bonbon
{

private:
    string nom_sprite;
    Fl_PNG_Image *sprite;
    int id;
    bool special = false;
    bool diagonal = false;
    bool gauche = false;
    bool is_glacage = false;
    bool is_cerises = false;
    bool is_double_glacage = false;
public:
    Bonbon(string nom_sprite, int id);

    void set_special(bool new_special);
    void set_glacage(bool new_glacage);
    void set_cerise(bool new_cerise);
    void set_double_glacage(bool new_double_glacage);
    void set_diagonal(bool new_diagonal);
    void set_gauche(bool new_gauche);
    void set_sprite(Fl_PNG_Image *new_image);
    void set_nom_sprite(string new_nom_sprite);
    void set_id(int new_id);

    bool get_special();
    bool get_glacage();
    bool get_cerise();
    bool get_double_glacage();
    bool get_diagonal();
    bool get_gauche();
    Fl_PNG_Image* get_sprite();
    string get_nom_sprite();
    int get_id();
    
};

Bonbon::Bonbon(string nom_sprite, int id) : nom_sprite{nom_sprite}, id{id} {}


void Bonbon::set_special(bool new_special){
    special = new_special;
}
void Bonbon::set_glacage(bool new_glacage){
    is_glacage = new_glacage;
}
void Bonbon::set_cerise(bool new_cerise){
    is_cerises = new_cerise;
}
void Bonbon::set_double_glacage(bool new_double_glacage){
    is_double_glacage = new_double_glacage;
}
void Bonbon::set_diagonal(bool new_diagonal){
    diagonal = new_diagonal;
}
void Bonbon::set_gauche(bool new_gauche){
    gauche = new_gauche;
}
void Bonbon::set_sprite(Fl_PNG_Image *new_image){
    sprite = new_image;
}
void Bonbon::set_nom_sprite(string new_nom_sprite){
    nom_sprite = new_nom_sprite;
}
void Bonbon::set_id(int new_id){
    id = new_id;
}


bool Bonbon::get_special(){
    return special;
}
bool Bonbon::get_glacage(){
    return is_glacage;
}
bool Bonbon::get_cerise(){
    return is_cerises;
}
bool Bonbon::get_double_glacage(){
    return is_double_glacage;
}
bool Bonbon::get_diagonal(){
    return diagonal;
}
bool Bonbon::get_gauche(){
    return gauche;
}
Fl_PNG_Image* Bonbon::get_sprite(){
    return sprite;
}
string Bonbon::get_nom_sprite(){
    return nom_sprite;

}
int Bonbon::get_id(){
    return id;
}