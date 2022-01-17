/*
Nom et prénom : Ehlalouch Safouan 000514145, Hamza Hajjaj 000461105
Classe qui permet l'affichage de texte
*/
#include <string>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>

#include "point.hpp"

using namespace std;

class Text {
    Point center;
    string s;
    int fontSize;
    Fl_Color color;
    public:
    //Constructor
    Text(string s, Point center, int fontSize = 10, Fl_Color color = FL_BLACK):
        s{s}, center{center}, fontSize{fontSize}, color{color} {}

    //Draw
    void draw();

    //Setters and getters
    string getString() {
        return s;
    }
    void setString(const string &newString) {
        s = newString;
    }
    int getFontSize() {
        return fontSize;
    }
    void setFontSize(int newFontSize) {
        fontSize = newFontSize;
    }
    Point getCenter() {
        return center;
    }
    void setCenter(Point newCenter) {
        center = newCenter;
  }
};

void Text::draw() {
  fl_color(color);
  fl_font(FL_HELVETICA, fontSize);
  int width, height;
  fl_measure(s.c_str(), width, height, false);
  fl_draw(s.c_str(), center.x-width/2, center.y-fl_descent()+height/2);
}