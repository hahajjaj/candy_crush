//Hamza Hajjaj 000461105
//Safouan Ehlalouch matricul
//Projet LDP 2 Candy Crush

/*
Utiliser la méthode tester_coup sur tout le plateau à chaque fois et après avoir rajouter chaque cell dans le vectore verifier si la nouvelle
cell a ajouter est déjà dans le vecteur ou non.
Puis supprimer tout le vecteur d'un coup.
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

using namespace std;

const int windowWidth=900;
const int windowHeight=900;
const double refreshPerSecond=30;




class Animation;
class Jeu;

struct Point {
    int x,y;
};

class Bonbon{
  
public:
    string nom_sprite;
    Fl_PNG_Image &sprite;
    Bonbon(Fl_PNG_Image &spri, string nom_sprite);
    
};

Bonbon::Bonbon(Fl_PNG_Image &spri, string nom_sprite):sprite{spri}, nom_sprite{nom_sprite}{}

class Cell {
    public:
    Jeu *jeu;
    Animation *anim;
    bool on=false;
    
    Fl_Color frameColor;
    Fl_Color fillColor;
    int w,h;

    Point center;
    vector<Cell *> neighbors;
    Bonbon *bonbon;
    Cell(Point center,int w, int h,  Bonbon *bonbon, Fl_Color frameColor = FL_BLACK, Fl_Color fillColor = FL_WHITE);
    void draw();
    void setNeighbors(const vector<Cell *> newNeighbors);
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    bool contains(Point p);
    void setFrameColor(Fl_Color newFrameColor);
    void setFillColor(Fl_Color newFillColor);
    void Inversion(Cell *cell);
    void setBonbon(Bonbon *newBonbon);
    
};

//############################################################

class Jeu{
    public:
        vector<Point> vecteur_points{Point{-1, 0},Point{ 1, 0},Point{ 0, 1},Point{ 0, -1}};
        Cell *cell1;
        Cell *cell2;
        
        vector<Cell*> cells_a_effacer;
        Jeu(Cell *cell1, Cell *cell2);
        void tester_coup();
        void appliquer_coup();
        void effacer_bonbon();
};

Jeu::Jeu(Cell *cell1, Cell *cell2):cell1{cell1},cell2{cell2}{}

void Jeu::effacer_bonbon(){
    
    for (auto &c:cells_a_effacer){
        Fl_PNG_Image *sprite_explosion = new Fl_PNG_Image("sprite/explosion2.png");
        Bonbon *newBonbon = new Bonbon{*sprite_explosion, "explosion"};
        c->setBonbon(newBonbon);
    }
}

void Jeu::tester_coup(){
    // if (cell1->bonbon->nom_sprite == cell2->bonbon->nom_sprite){
    //     cout << "bonbons idéntiques" << endl;
    // }
    
    cell2->Inversion(cell1);
    for (auto &p: vecteur_points){
        Cell *cell_actuelle = cell2;
        bool finished = false;
        while(!finished){
            bool trouve = false;
            for(auto &voisin:cell_actuelle->neighbors){
                if ((cell_actuelle->center.x)+(100*p.x) == voisin->center.x && (cell_actuelle->center.y)+(100*p.y) == voisin->center.y){
                    if(cell_actuelle->bonbon->nom_sprite == voisin->bonbon->nom_sprite){
                        if (cells_a_effacer.size() == 0){
                            cells_a_effacer.push_back(cell_actuelle);
                        }
                        cells_a_effacer.push_back(voisin);
                        cell_actuelle = voisin;
                        trouve = true;
                    }  
                }
            }
            if (trouve == false)
                finished = true;
        }
        
    }
    effacer_bonbon();
    cout << cells_a_effacer.size() << endl;
}


//#############################################################

class Animation{
    public:
    Cell *c;
    Animation(Cell *cell):c{cell}{}
    void translation_bonbon(Cell *cell2);
    string test_voisins_valide(Cell *cell2);
    void suppression_bonbon();
};

string Animation::test_voisins_valide(Cell *cell2){
    Point test;
    for (auto &p: c->neighbors){
        if (cell2->center.x == p->center.x && cell2->center.y == p->center.y){
            test = p->center;
        }
    };
    int position_voisin_x = (test.x - c->center.x)/100;
    int position_voisin_y = (test.y - c->center.y)/100;
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

void Animation::translation_bonbon(Cell *cell2){
    Point centre1 = c->center;
    Point centre2 = cell2->center;
    string ret = test_voisins_valide(cell2);
    if (ret == "g"){
        while (c->center.x != centre2.x){
            c->center.x-=1;
            cell2->center.x+=1;
            Fl::wait(0.01);
        }
    }
    else if (ret == "d"){
        while (c->center.x != centre2.x){
            c->center.x+=1;
            Fl::wait(0.01);
            cell2->center.x-=1;
        }
    }
    else if (ret == "h"){
        while (c->center.y != centre2.y){
            c->center.y-=1;
            Fl::wait(0.01);
            cell2->center.y+=1;
        }
    }
    else if (ret == "b"){
        while (c->center.y != centre2.y){
            c->center.y+=1;
            Fl::wait(0.01);
            cell2->center.y-=1;
        }
    }
}

Cell::Cell(Point center,int w, int h, Bonbon *bonbon, Fl_Color frameColor, Fl_Color fillColor):center{center},w{w},h{h},bonbon{bonbon},anim{nullptr}, frameColor{frameColor}, fillColor{fillColor}{
    
}

void Cell::draw(){
    // fl_draw_box(FL_FLAT_BOX,center.x,center.y,w,h,fillColor); 
    // fl_draw_box(FL_BORDER_FRAME,center.x,center.y,w,h,frameColor);
    bonbon->sprite.draw(center.x, center.y, w, h);
}

void Cell::setNeighbors(const vector<Cell *> newNeighbors){
    neighbors = newNeighbors;
}

void Cell::mouseMove(Point mouseLoc){
    if (contains(mouseLoc)){
        setFrameColor(FL_RED);
    }
    else {
        setFrameColor(FL_BLACK);
    }
}

bool Cell::contains(Point p){
    return p.x>=center.x &&
        p.x<center.x+w &&
        p.y>=center.y &&
        p.y<center.y+h;
}

void Cell::setFrameColor(Fl_Color newFrameColor){
    frameColor=newFrameColor;
}

void Cell::setFillColor(Fl_Color newFillColor){
    fillColor=newFillColor;
}

 void Cell::setBonbon(Bonbon *newBonbon){
     bonbon = newBonbon;
 }

void Cell::Inversion(Cell *cell){
    
    Bonbon *bonbon1 = cell->bonbon;
    Bonbon *bonbon2 = bonbon;
    
    cell->anim->translation_bonbon(this);
    cell->setBonbon(bonbon2);
    setBonbon(bonbon1);
    Point centre_actuel = this->center;
    this->center = cell->center;
    cell->center = centre_actuel;
    delete anim;
}


void Cell::mouseClick(Point mouseLoc){
    
    static int bonbons_clicked;
    static Cell *cell1;
    if (contains(mouseLoc)){
        on = !on;
        if (on){
            setFillColor(FL_RED);
            
            bonbons_clicked++;
              if (bonbons_clicked == 1){
                anim = new Animation{this};
                cell1 = this;
            }
              else if (bonbons_clicked == 2){
                // cell1->anim->translation_bonbon(this);
                jeu = new Jeu(cell1, this);
                jeu->tester_coup();
                // Inversion(cell1);
                // Inversion(cell1);
                bonbons_clicked = 0;
                setFillColor(FL_WHITE);
                cell1->setFillColor(FL_WHITE);
              }
        }
        else {
            setFillColor(FL_WHITE);
        }
    }
}

//###########################################################


//##########################################################
// Code pour le vecteur de voisins pris dans le tp
class Plateau{
    vector<string>bonbons{"sprite/1.png","sprite/2.png","sprite/3.png","sprite/4.png","sprite/5.png","sprite/6.png"};
    // vector<Cell> cells;
    vector< vector<Cell> > cells;
public:
    Plateau();
    void draw();
    void tester_coup_plateau();
    void initialize_neighbours();
    void initialize_grid();
    void load_sprite();
    void mouseMove(Point mouseLoc);
    void mouseClick(Point mouseLoc);
    void keyPressed(int /*keyCode*/) {exit(0);}
};


Plateau::Plateau(){
    initialize_grid();
    initialize_neighbours();
    
}
void Plateau::initialize_grid(){
    for (int x=0; x<9;x++){
        cells.push_back({});
        for (int y=0; y<9; y++){
            int nbr_aleatoire = (rand() % bonbons.size());
            const char * nom_fichier = bonbons[nbr_aleatoire].c_str();
            Fl_PNG_Image *sprite = new Fl_PNG_Image(nom_fichier);
            Bonbon *newBonbon = new Bonbon{*sprite, nom_fichier};
            cells[x].push_back(Cell{Point{100*(x),100*(y)},100,100,newBonbon});
        }
    }
}

void Plateau::initialize_neighbours(){
    for (int x=0; x<9; x++){
        for (int y=0; y<9; y++){
            vector<Cell *> neighbors;
            for (auto &shift: vector<Point>({
            {-1, 0}, // gauche // The 8 neighbors relative to the cell
            { 1, 0}, // droite
            { 0, 1}, // bas
            { 0, -1}, // haut
            {-1, 1},
            { 1, 1},
            { 1, -1},
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

void Plateau::draw() {
    for (auto &v: cells)
        for (auto &c: v)
            c.draw();
}
void Plateau::mouseMove(Point mouseLoc) {
    for (auto &v: cells)
      for (auto &c: v)
        c.mouseMove(mouseLoc);
}
void Plateau::mouseClick(Point mouseLoc){
    for (auto &v: cells){
        for (auto &c: v){
            c.mouseClick(mouseLoc);
            
        }
    }       
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
