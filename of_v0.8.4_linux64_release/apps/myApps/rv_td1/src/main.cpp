#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( )
{
  ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
  ofSetupOpenGL(800,600,OF_WINDOW);
  ofSetWindowTitle("RV TD1");

  ofRunApp(new ofApp());
}
