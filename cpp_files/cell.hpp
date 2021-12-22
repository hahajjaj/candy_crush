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

#include "point.hpp"
#include "bonbon.hpp"

class Animation;

class Cell
{
public:
    Animation *anim;
    bool on = false;

    Fl_Color frameColor;
    Fl_Color fillColor;
    int w, h;

    Point center;
    vector<Cell *> neighbors;
    Bonbon *bonbon;
    Cell(Point center, int w, int h, Bonbon *bonbon, Fl_Color frameColor = FL_BLACK, Fl_Color fillColor = FL_WHITE);
    void draw();
    bool tester_plateau();
    void finaliser_plateau();
    void setNeighbors(const vector<Cell *> newNeighbors);
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    bool contains(Point p);
    void setFrameColor(Fl_Color newFrameColor);
    void setFillColor(Fl_Color newFillColor);
    void Inversion(Cell *cell);
    void setBonbon(Bonbon *newBonbon);
};

class Animation
{
public:
    Cell *c;
    Animation(Cell *cell) : c{cell} {}
    void translation_bonbon_vers_le_bas(Cell *cell2);
    void translation_bidirectionnelle(Cell *cell2);
    string test_voisins_valide(Cell *cell2);
    
};


Cell::Cell(Point center, int w, int h, Bonbon *bonbon, Fl_Color frameColor, Fl_Color fillColor) : center{center}, w{w}, h{h}, bonbon{bonbon}, frameColor{frameColor}, fillColor{fillColor}
{
}

void Cell::draw()
{
    // fl_draw_box(FL_FLAT_BOX,center.x,center.y,w,h,fillColor);
    // fl_draw_box(FL_BORDER_FRAME,center.x,center.y,w,h,frameColor);
    bonbon->sprite.draw(center.x, center.y, w, h);
    // jeu->tester_coup();
}

void Cell::setNeighbors(const vector<Cell *> newNeighbors)
{
    neighbors = newNeighbors;
}

void Cell::mouseMove(Point mouseLoc)
{
    if (contains(mouseLoc))
    {
        setFrameColor(FL_RED);
    }
    else
    {
        setFrameColor(FL_BLACK);
    }
}

bool Cell::contains(Point p)
{
    return p.x >= center.x &&
           p.x < center.x + w &&
           p.y >= center.y &&
           p.y < center.y + h;
}

void Cell::setFrameColor(Fl_Color newFrameColor)
{
    frameColor = newFrameColor;
}

void Cell::setFillColor(Fl_Color newFillColor)
{
    fillColor = newFillColor;
}

void Cell::setBonbon(Bonbon *newBonbon)
{
    bonbon = newBonbon;
}

void Cell::Inversion(Cell *cell)
{
    //récupération des références des bonbon
    Bonbon *bonbon1 = cell->bonbon;
    Bonbon *bonbon2 = bonbon;

    //appel de la translation des deux bonbons
    cell->anim->translation_bidirectionnelle(this);


    //Inversion des references des bonbons dans les cells
    cell->setBonbon(bonbon2);
    setBonbon(bonbon1);

    //Inversion des centres des cells
    Point centre_actuel = this->center;
    this->center = cell->center;
    cell->center = centre_actuel;

    //Suppression de l'instance d'animation
    delete anim;
}

void Cell::mouseClick(Point mouseLoc)
{
    bool reponse_test_plateau = false;
    static int bonbons_clicked;
    static Cell *cell1;
    if (contains(mouseLoc))
    {
        bonbons_clicked++;
        if (bonbons_clicked == 1)
        {
            anim = new Animation{this};
            cell1 = this;
        }
        else if (bonbons_clicked == 2 && cell1 != this)
        {

            Inversion(cell1);
            bonbons_clicked = 0;
        }
        else
        {
            bonbons_clicked = 0;
        }
    }
}



void Animation::translation_bonbon_vers_le_bas(Cell *cell2)
{

    Bonbon *bonbon1 = c->bonbon;
    Bonbon *bonbon2 = cell2->bonbon;

    Point centre1 = c->center;
    Point centre2 = cell2->center;
    // string ret = test_voisins_valide(cell2); // retourne dans quelle direction est la cell2 par rapport à c
    // if (ret == "b") // si voisin du bas 
    // {
    while (c->center.y != centre2.y)
    {
        c->center.y += 1;
        Fl::wait(0.01);
    }

    cell2->setBonbon(bonbon2);
    c->setBonbon(bonbon1);
    
    // }
    // cell2->setBonbon(c->bonbon);

    // c->center = centre2;

    Point centre_actuel = c->center;
    c->center = cell2->center;
    cell2->center = centre_actuel;
}

string Animation::test_voisins_valide(Cell *cell2)
{
    Point test;
    for (auto &p : c->neighbors)
    {
        if (cell2->center.x == p->center.x && cell2->center.y == p->center.y)
        {
            test = p->center;
        }
    };
    int position_voisin_x = (test.x - c->center.x) / 100;
    int position_voisin_y = (test.y - c->center.y) / 100;
    Point test2{position_voisin_x, position_voisin_y};

    if (test2.x == -1 && test2.y == 0)
        return "g";
    else if (test2.x == 1 && test2.y == 0)
        return "d";
    else if (test2.x == 0 && test2.y == -1)
        return "h";
    else if (test2.x == 0 && test2.y == 1)
        return "b";
    else
        return "no";
}

void Animation::translation_bidirectionnelle(Cell *cell2)
{
    Point centre1 = c->center;
    Point centre2 = cell2->center;
    string ret = test_voisins_valide(cell2);
    if (ret == "g")
    {
        while (c->center.x != centre2.x)
        {
            c->center.x -= 1;
            cell2->center.x += 1;
            Fl::wait(0.01);
        }
    }
    else if (ret == "d")
    {
        while (c->center.x != centre2.x)
        {
            c->center.x += 1;
            Fl::wait(0.01);
            cell2->center.x -= 1;
        }
    }
    else if (ret == "h")
    {
        while (c->center.y != centre2.y)
        {
            c->center.y -= 1;
            Fl::wait(0.01);
            cell2->center.y += 1;
        }
    }
    else if (ret == "b")
    {
        while (c->center.y != centre2.y)
        {
            c->center.y += 1;
            Fl::wait(0.01);
            cell2->center.y -= 1;
        }
    }
}