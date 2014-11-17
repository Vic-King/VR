#pragma once

#include "ofMain.h"

#include "Material.h"

class Cube : public ofNode
{
 public:
  Cube(Material* m);
  Cube(const ofFloatColor &c);
  virtual ~Cube();
  virtual void customDraw();
  ofVec3f getColor ();
  void setColor (ofVec3f color);
  void setIsFbo (bool b);
  int getNbLife ();
  void lostLife();

 private:
  ofVec3f _color;
  Material* _m;
  ofVboMesh _mesh;
  bool _isFbo;
  int life;
};

