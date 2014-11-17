#include "Cube.h"

// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

const float _vertices[72] = {
  1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
  1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
  1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
  -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
  -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
  1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back)

const ofIndexType _indices[36]  = {
  0, 1, 2,   2, 3, 0,      // front
  4, 5, 6,   6, 7, 4,      // right
  8, 9,10,  10,11, 8,      // top
  12,13,14,  14,15,12,      // left
  16,17,18,  18,19,16,      // bottom
  20,21,22,  22,23,20 };    // back

// texture ///////////////////////////////////////////////
//  3----2----5----7
//  |    |    |    |
//  |    |    |    |
//  0----1----4----6

const int _uv_indices[36] = {
  2, 3, 0, 1, // front
  3, 0, 1, 2, // right
  4, 5, 2, 1, // top
  2, 3, 0, 1, // left
  4, 6, 7, 5, // bottom
  0, 1, 2, 3  // back
};

const float _uv[24] = {
  0, 0,         1.0/3.0, 0,
  1.0/3.0, 1,   0, 1,
  2.0/3.0, 0,   2.0/3.0, 1,
  1, 0,         1, 1,
};

Cube::Cube(Material* m)
{
  _m = m;

  _mesh.setMode(OF_PRIMITIVE_TRIANGLES);
  for(unsigned int i=0; i<24; i++)
    {
      _mesh.addVertex(ofPoint(_vertices[i*3],_vertices[i*3+1],_vertices[i*3+2]));
    }
  for(unsigned int i=0; i<36; i++)
    {
      _mesh.addIndex(_indices[i]);
    }
  for(unsigned int i=0; i<36; i++)
    {
      _mesh.addTexCoord(ofVec2f(_uv[_uv_indices[i]*2], _uv[_uv_indices[i]*2+1]));
    }

  setScale(0.5);
  _mesh.disableTextures();

  _isFbo = false;

  life = 2;
}

Cube::~Cube()
{
}

ofVec3f Cube::getColor () {
  return _color;
}

void Cube::setColor (ofVec3f color) {
  _color = color;
}

void Cube::setIsFbo (bool b) {
  _isFbo = b;
}

int Cube::getNbLife () {
  return life;
}

void Cube::lostLife() {
  life--;
}

void Cube::customDraw()
{
  if (!_isFbo)
    _m->bind();
  else
    ofSetColor(_color.x, _color.y, _color.z);

  _mesh.draw();

  if (!_isFbo)
    _m->unbind();
}
