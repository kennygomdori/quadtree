// File:        pa3.cpp
// Author:      cs221 instructors
// Date:        2021-03-03
// Description: Partial test of PA3 functionality
//              Reads Point data from external files
//              Produces PNG images of the point sets

#include "sqtree.h"
#include <string.h>

int main() {

  // read in image
  // PNG orig;
  
  // orig.readFromFile("images/orig/small1.png");
  //  orig.readFromFile("images/orig/rocky_bullwinkle.png");
  // orig.readFromFile("images/orig/stanley-totem-poles.png");
  /*
  orig.readFromFile("images/orig/small2.png");
  orig.readFromFile("images/orig/pacman.png");
  orig.readFromFile("images/orig/rocky.png");
  orig.readFromFile("images/orig/rocky_bullwinkle.png");
  orig.readFromFile("images/orig/colour.png");
  orig.readFromFile("images/orig/klee-flora-on-sand.png");
  orig.readFromFile("images/orig/stanley-totem-poles.png");
  */
  
  std::string files[] = {"4x4.png", "small1.png", "rocky_bullwinkle.png", "stanley-totem-poles.png"};
  // use it to build a sqtree (try other tolerances)

  // copy the tree (to show you can)
  // SQtree tCopy(t);
  for (auto file: files) {
    PNG orig;
    orig.readFromFile("images/orig/" + file);
    
    SQtree t(orig, 1655360);
    PNG pic = t.render();
    cout << "Size: " << t.size() << endl;
    pic.writeToFile("images/out/" + file);
  }
  
  return 0;
}
