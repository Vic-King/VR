#pragma once

#include "ofMain.h"

class Material
{
 public:
  Material(string filename);
  virtual ~Material();
  void bind();
  void unbind();

 protected:
  void loadMipMapTexture(string inPath, float inAnisotropy);

 private:
  ofTexture _tex;
};
