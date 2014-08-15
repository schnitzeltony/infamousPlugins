// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef stuck_ui_h
#define stuck_ui_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include"ffffltk.h"
#include"lv2/lv2plug.in/ns/extensions/ui/ui.h"
using namespace ffffltk;
#include"stuck.h"
#include"draw_stuckbg.h"
#include"draw_toggleSwitch.h"
#include"draw_blueLED.h"
#include"draw_tabDial.h"

class StuckUI {
public:
  Fl_Double_Window* show();
  Fl_Double_Window *ui;
  Aspect_Group *bg;
  Button *stickit;
private:
  void cb_stickit_i(Button*, void*);
  static void cb_stickit(Button*, void*);
public:
  Button *led;
  Dial *volume;
  Dial *time;
  LV2UI_Write_Function write_function; 
  LV2UI_Controller controller; 
};
#endif
