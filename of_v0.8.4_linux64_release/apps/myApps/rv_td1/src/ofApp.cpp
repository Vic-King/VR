#include "ofApp.h"
#include "ctime"

//--------------------------------------------------------------

void ofApp::setup()
{
  int width = ofGetWidth();
  int height = ofGetHeight();

  cam.init(width, height);

  init = true;

  fbo.allocate(width, height, GL_RGBA);

  pixels.allocate(width, height, OF_IMAGE_COLOR_ALPHA);

  image.allocate(width, height, OF_IMAGE_COLOR_ALPHA);

  cpt = 0;

  ofBackground(0,0,0);
  ofEnableDepthTest();
  ofEnableNormalizedTexCoords();
  ofEnableAntiAliasing();
  ofDisableArbTex();

  ofHideCursor();

  ofTrueTypeFont::setGlobalDpi(72);
  verdana.loadFont("verdana.ttf", 18, true, true, true);
  verdana.setLineHeight(18.0f);
  verdana.setLetterSpacing(1.037);

  height = 0;
  up = true;

  srand(time(NULL));
  int r;

  // Create cubes and their materials
  Material* sand = new Material("sand.png");
  materials.push_back(sand);

  Material* stonebrick = new Material("stonebrick.png");
  materials.push_back(stonebrick);

  Material* stonebrick_mossy = new Material("stonebrick_mossy.png");
  materials.push_back(stonebrick_mossy);

  Cube* c;

  currentColor.set(0, 0, 0);

  for (int i=0; i < 2; i++) {
    for (int j=-10; j < 10; j++) {
      for (int k=-10; k < 10; k++) {
	if (i==1){
	  c = new Cube(sand);
	}else{
	  r = rand() % 2;
	  if (r == 0)
	    c = new Cube(stonebrick);
	  else
	    c = new Cube(stonebrick_mossy);
	}

        currentColor = nextCurrentColor(currentColor);
	c->setColor(currentColor);

	c->move(j, i+1, k);
	cubes.push_back(c);
      }
    }
  }

  Material* tnt = new Material("tnt.png");
  materials.push_back(tnt);
  
  c = new Cube(tnt);

  c->move(2, 0, -1);

  cubes.push_back(c);
}

ofVec3f ofApp::nextCurrentColor(ofVec3f currentColor) {
  ofVec3f newColor;

  newColor.x = currentColor.x;
  newColor.y = currentColor.y;
  newColor.z = currentColor.z;

  newColor.x = (((int)(newColor.x)) + 1) % 256;
  if (newColor.x == 0) {
    newColor.y = (((int)(newColor.y)) + 1) % 256;
    if (newColor.z == 0) {
      newColor.z = (((int)(newColor.z)) + 1) % 256;
    }
  }

  return newColor;
}

//--------------------------------------------------------------
void ofApp::exit()
{
  for (unsigned int i=0; i<cubes.size(); i++) {
    delete cubes[i];
  }
  for (unsigned int i=0; i<materials.size(); i++) {
    delete materials[i];
  }
}

//--------------------------------------------------------------
void ofApp::update()
{
  // Constrain mouse cursor inside window
  if(init){
    moveMouse(ofGetWidth()/2, ofGetHeight()/2);
    init = false;
  }else{
    int x = ofGetMouseX();
    int y = ofGetMouseY();
    if(x >= ofGetWindowWidth()-1)
      moveMouse(1,y);
    if(x <= 0)
      moveMouse(ofGetWindowWidth()-2,y);
    if(y <= 0)
      moveMouse(x,ofGetHeight()-2);
    if(y >= ofGetHeight()-1)
      moveMouse(x,1);
  }

  cam.translateCamera();
}

//--------------------------------------------------------------
void ofApp::draw()
{
  //ofBackground(0);

  ofPushMatrix();
  cam.applyMatrix();
  drawAxes();

  ofPushMatrix();
  ofScale(100,100,100);

  if (up && height <= -3.0f)
    up = false;
  else if (!up && height >= 0.0f)
    up = true;

  height += up ? -0.1f : 0.1f;

  displacement.makeTranslationMatrix(0.0f, height, 0.0f);
  cubes[cubes.size() - 1]->setTransformMatrix(displacement);

  for (unsigned int i=0; i<cubes.size(); i++) {
    cubes[i]->setIsFbo(false);
    cubes[i]->draw();
  }
  
  fbo.begin();
  ofClear(255);
  
  /*for (int i = 0; i < cubes.size(); i++) {
    cubes[i]->setIsFbo(true);
    cubes[i]->draw();
    }*/

  for (int i = 0; i < ofGetWidth(); ++i) {
    for (int j = 0; i < ofGetHeight(); ++j) {
      ofRect(i, j, 1, 1);
      ofSetColor(i, j, 0, 255);
    }
  }
      
  fbo.end();

  fbo.readToPixels(pixels);
  image.setFromPixels(pixels);

  fbo.draw(0,0);

  ofPopMatrix();
  ofPopMatrix();

  cam.drawAim();
}

//--------------------------------------------------------------
void ofApp::drawAxes()
{
  ofPushMatrix();
  ofSetLineWidth(2);
  ofSetColor(255, 0, 0);
  ofLine(0, 0, 0, 20, 0, 0);
  verdana.drawStringAsShapes("x", 25, 0);
  ofSetColor(0, 255, 0);
  ofLine(0, 0, 0, 0, 20, 0);
  verdana.drawStringAsShapes("y", 5, 25);
  ofSetColor(0, 0, 255);
  ofLine(0, 0, 0, 0, 0, 20);
  ofTranslate(0,0,22);
  verdana.drawStringAsShapes("z", -5, -5);
  ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::moveMouse(int x, int y)
{
#if defined(__APPLE__)
  CGWarpMouseCursorPosition(CGPointMake(ofGetWindowPositionX()+x,ofGetWindowPositionY()+y));
#elif defined(_WIN32)
  SetCursorPos(x,y); // not tested
#else // xlib
  Display *display = XOpenDisplay(0);
  Window window;
  int state;
  XGetInputFocus(display,&window,&state);
  XWarpPointer(display, None, window, 0, 0, 0, 0, x, y);
  XCloseDisplay(display);
#endif
  cam.mousePressed(x, y);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if(key == 'f')
    ofToggleFullscreen();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
  cam.moveView(x, y);
  moveMouse(ofGetWidth()/2, ofGetHeight()/2);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  if(button==0)
    cam.update(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  if((button==0 || button ==2) && x>0 && y>0) {
    //cam.mousePressed(x, y);
    
    ofColor tmpColor = image.getColor(x, y);

    for (int i = 0; i < ofGetHeight(); ++i) {
      for (int j = 0; j < ofGetWidth(); ++j) {
	if (image.getColor(j, i).r != 255)
	  std::cout << pixels.getColor(j, i) << std::endl;
      }
    }

    std::cout << tmpColor << std::endl;

    if (button == 0) { // Clic gauche
      for (int i = 0; i < cubes.size(); ++i) {
	if (cubes[i]->getColor().x == tmpColor.r && cubes[i]->getColor().y == tmpColor.g && cubes[i]->getColor().z == tmpColor.b) {
	  //cubes[i]->setColor(newColor);

	  cubes[i]->lostLife();
	  if (cubes[i]->getNbLife() < 0) {
	    cubes.erase(cubes.begin() + i);
	  }

	  std::cout << "-1 <3 !" << std::endl;
	  break;
	}
      }
    }
    else { // Clic droit
      for (int i = 0; i < cubes.size(); ++i) {
	if (cubes[i]->getColor().x == tmpColor.r && cubes[i]->getColor().y == tmpColor.g && cubes[i]->getColor().z == tmpColor.b) {

	  Material* tex = new Material("stonebrick.png");
	  materials.push_back(tex);

	  Cube* c = new Cube(tex);
	  ofVec3f newColor;

	  currentColor = nextCurrentColor(currentColor);
	  c->setColor(currentColor);
	 
	  c->move(cubes[i]->getX(), cubes[i]->getY() - 1, cubes[i]->getZ());
	  cubes.push_back(c);

	  std::cout << "Cube ! x = " << cubes[i]->getX() << " y = " << cubes[i]->getY() << " z = " << cubes[i]->getZ() << std::endl;
	  std::cout << "new Cube ! x = " << c->getX() << " y = " << c->getY() << " z = " << c->getZ() << std::endl;
	  break;
	}
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
  cam.init(ofGetWidth(),ofGetHeight());
}
