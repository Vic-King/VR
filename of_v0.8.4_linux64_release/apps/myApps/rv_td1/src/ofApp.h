#pragma once

#include <vector>
#include "ofMain.h"

#include "Camera.h"
#include "Material.h"
#include "Cube.h"

class ofApp : public ofBaseApp{

 public:
  void setup();
  void exit();
  void update();
  void draw();
  void drawAxes();
  void moveMouse(int x, int y);
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  ofVec3f nextCurrentColor(ofVec3f c);
  ofTrueTypeFont verdana;

  Camera cam;

  std::vector<Cube*> cubes;
  std::vector<Material*> materials;

  bool init;

 private:
  ofMatrix4x4 displacement;
  bool up;
  float height;
  ofVec3f currentColor;
  ofFbo fbo;
  ofPixels pixels;
  ofImage image;
  int cpt;
};
