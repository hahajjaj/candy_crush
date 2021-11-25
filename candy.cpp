//Hamza Hajjaj 000461105
//Safouan Ehlalouch matricul
//Projet LDP 2 Candy Crush

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

using namespace std;

const int windowWidth=900;
const int windowHeight=900;
const double refreshPerSecond=30;

struct Point {
    int x,y;
};

class Bonbon{
    Fl_PNG_Image &sprite;
    Point center;
    Fl_Color frameColor;
    Fl_Color fillColor;
    int w,h;
public:
    Bonbon(Point center, int w, int h, Fl_PNG_Image &spri, Fl_Color frameColor = FL_BLACK, Fl_Color fillColor = FL_WHITE);
    void draw();
    bool contains(Point p);
    void setFrameColor(Fl_Color newFrameColor);
    void setFillColor(Fl_Color newFillColor);

    Point get_center();
    void set_center(Point p);

    string get_bonbon();
    void set_bonbon(string bonbon);
};

Bonbon::Bonbon(Point center, int w, int h, Fl_PNG_Image &spri, Fl_Color frameColor, Fl_Color fillColor):center{center},w{w},h{h},sprite{spri}, fillColor{fillColor}, frameColor{frameColor}{}


void Bonbon::draw(){
    fl_draw_box(FL_FLAT_BOX,center.x,center.y,w,h,fillColor); 
    fl_draw_box(FL_BORDER_FRAME,center.x,center.y,w,h,frameColor);
    sprite.draw(center.x, center.y, w, h);
}

bool Bonbon::contains(Point p){
    return p.x>=center.x &&
        p.x<center.x+w &&
        p.y>=center.y &&
        p.y<center.y+h;
}

void Bonbon::setFrameColor(Fl_Color newFrameColor){
    frameColor=newFrameColor;
}

void Bonbon::setFillColor(Fl_Color newFillColor){
    fillColor=newFillColor;
}
 
//#####################################################################################
//#####################################################################################

class Cell {
    vector<Cell *> neighbors;
    Bonbon bonbon;
    bool on=false;
    
public:
    Cell(Point center,int w, int h, Fl_PNG_Image &spri);
    void draw();
    void setNeighbors(const vector<Cell *> newNeighbors);
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    
};

Cell::Cell(Point center,int w, int h, Fl_PNG_Image &spri):bonbon(center,w,h, spri){}

void Cell::draw(){
    bonbon.draw();
}

void Cell::setNeighbors(const vector<Cell *> newNeighbors){
    neighbors = newNeighbors;
}

void Cell::mouseMove(Point mouseLoc){
    if (bonbon.contains(mouseLoc)){
        bonbon.setFrameColor(FL_RED);
    }
    else {
        bonbon.setFrameColor(FL_BLACK);
    }
}
void Cell::mouseClick(Point mouseLoc){
    if (bonbon.contains(mouseLoc)){
        on = !on;
        if (on){
            bonbon.setFillColor(FL_RED);
        }
        else {
            bonbon.setFillColor(FL_WHITE);
        }
    }
}

//#####################################################################################
//#####################################################################################

class Plateau{
    vector<string>bonbons{"sprite/1.png","sprite/2.png","sprite/3.png","sprite/4.png","sprite/5.png","sprite/6.png"};
    vector< vector<Cell> > cells;
public:
    Plateau();
    void draw();
    void initialize_grid();
    void initialize_neighbours();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    void inversion(Cell *cell1, Cell *cell2);
    void keyPressed(int /*keyCode*/) {exit(0);}
};


Plateau::Plateau(){
    initialize_grid();
    initialize_neighbours();
}  

void Plateau::draw() {
    for (auto &v: cells)
        for (auto &c: v)
            c.draw();
}

void Plateau::initialize_grid(){
    for (int x=0; x<9;x++){
        cells.push_back({});
        for (int y=0; y<9; y++){
            int nbr_aleatoire = (rand() % bonbons.size());
            const char * nom_fichier = bonbons[nbr_aleatoire].c_str();
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
            cells[x].push_back(Cell{Point{100*(x),100*(y)},100,100,*sprite});
        }
    }
}

void Plateau::initialize_neighbours(){
    for (int x=0; x<9; x++){
        for (int y=0; y<9; y++){
            vector<Cell *> neighbors;
            for (auto &shift: vector<Point>({
            {-1, 0}, // The 8 neighbors relative to the cell
            {-1, 1},
            { 0, 1},
            { 1, 1},
            { 1, 0},
            { 1, -1},
            { 0, -1},
            {-1, -1},
            })) {
                int neighborx = x+shift.x;
                int neighbory = y+shift.y;
                if (neighborx >= 0 && 
                    neighbory >= 0 && 
                    neighborx < cells.size() & 
                    neighbory < cells[neighborx].size()){
                    neighbors.push_back(&cells[neighborx][neighbory]);
                }
                cells[x][y].setNeighbors(neighbors);
            }
        }
    }
}

void Plateau::mouseMove(Point mouseLoc) {
    for (auto &v: cells)
      for (auto &c: v)
        c.mouseMove(mouseLoc);
}
void Plateau::mouseClick(Point mouseLoc){
    for (auto &v: cells)
      for (auto &c: v)
        c.mouseClick(mouseLoc);       
}

void Plateau::inversion(Cell *cell1, Cell *cell2){
    Cell first_cell = *cell1;
    Cell second_cell = *cell2;

    
}

/* ------ DO NOT EDIT BELOW HERE (FOR NOW) ------ */
class MainWindow : public Fl_Window {
    Plateau plateau;
    public:
    MainWindow() : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush") {
        Fl::add_timeout(1.0/refreshPerSecond, Timer_CB, this);
        resizable(this);
    }
    void draw() override {
        Fl_Window::draw();
        plateau.draw();
    }
    int handle(int event) override {
        switch (event) {
            case FL_MOVE:
                plateau.mouseMove(Point{Fl::event_x(),Fl::event_y()});
                return 1;
            case FL_PUSH:
                plateau.mouseClick(Point{Fl::event_x(),Fl::event_y()});
                return 1;                
            case FL_KEYDOWN:
                plateau.keyPressed(Fl::event_key());
                return 1;
        }
        return 0;
    }
    static void Timer_CB(void *userdata) {
        MainWindow *o = (MainWindow*) userdata;
        o->redraw();
        Fl::repeat_timeout(1.0/refreshPerSecond, Timer_CB, userdata);
    }
};

int main(int argc, char *argv[]) {
    
    MainWindow window;
    window.show(argc, argv);
    return Fl::run();
}