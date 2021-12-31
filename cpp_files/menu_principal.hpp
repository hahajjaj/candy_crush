//
#include <iostream>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>

#include <FL/Fl_Button.H>

using namespace std;

class menu_principal
{
    public:
    // void button_cb(Fl_Widget *, void *);
    menu_principal(){
    }
    static void button_cb(Fl_Widget *obj, void *);
    void show();
};

void menu_principal::show()
{
    Fl_PNG_Image *img = new Fl_PNG_Image("sprite/1.png");
    Fl_Button *partie_simple = new Fl_Button(0, 0, img->w(), img->h(),"nouvelle partie");
    partie_simple->image(img);
    partie_simple->callback(button_cb);
    // partie_simple->redraw();

}

void menu_principal::button_cb(Fl_Widget *obj, void *)

{
    obj->label("OFF");
    // obj->redraw();

    //  obj->resize( 0,0,40,40 );
}
