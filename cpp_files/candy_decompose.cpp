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

#include "plateau.hpp"


using namespace std;

const int windowWidth = 900;
const int windowHeight = 900;
const double refreshPerSecond = 30;

/* ------ DO NOT EDIT BELOW HERE (FOR NOW) ------ */
class MainWindow : public Fl_Window
{
    Plateau plateau;

public:
    MainWindow() : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush")
    {
        Fl::add_timeout(1.0 / refreshPerSecond, Timer_CB, this);
        resizable(this);
    }
    void draw() override
    {
        Fl_Window::draw();
        plateau.draw();
    }
    int handle(int event) override
    {
        switch (event)
        {
        case FL_MOVE:
            plateau.mouseMove(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        case FL_PUSH:
            plateau.mouseClick(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        case FL_KEYDOWN:
            plateau.keyPressed(Fl::event_key());
            return 1;
        }
        return 0;
    }
    static void Timer_CB(void *userdata)
    {
        MainWindow *o = (MainWindow *)userdata;
        o->redraw();
        Fl::repeat_timeout(1.0 / refreshPerSecond, Timer_CB, userdata);
    }
};

int main(int argc, char *argv[])
{

    MainWindow window;
    window.show(argc, argv);
    return Fl::run();
}
