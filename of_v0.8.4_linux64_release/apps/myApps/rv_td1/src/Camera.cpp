#include "Camera.h"
#include "ofMain.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::init(int windowWidth, int windowHeight)
{
    width = windowWidth;
    height = windowHeight;
    aim_dist = 400;
    rotX = 0;
    rotY = 0;
    lastX = width/2;
    lastY = height/2;
    posX = 0;
    posY = 0;
    speed = 10.0f;
    speedView = 0.2f;
}


 void Camera::applyMatrix()
 {
    ofLoadIdentityMatrix();
   
    ofRotateX(rotX);
    ofRotateY(rotY);
    
    ofTranslate(posX, 0, posY);
 }

void Camera::update(int x, int y)
{
    float rateX = 180.0/float(height);
    float rateY = 180.0/float(width);
    float diffX = x - lastX;
    float diffY = lastY - y;
    lastX = x;
    lastY = y;
    rotX += diffY * rateY;
    rotY += diffX * rateX;
}

void Camera::translateCamera() {

  if (ofGetKeyPressed('d')) {
    posY += speed * sin((rotY + 180) * M_PI / 180.0f);
    posX += speed * cos((rotY + 180) * M_PI / 180.0f);
  }

  if (ofGetKeyPressed('q')) {
    posY += speed * sin(rotY * M_PI / 180.0f);
    posX += speed * cos(rotY * M_PI / 180.0f);
  }

  if (ofGetKeyPressed('z')) {
    posY += speed * sin((rotY + 90) * M_PI / 180.0f);
    posX += speed * cos((rotY + 90) * M_PI / 180.0f);
  }

  if (ofGetKeyPressed('s')) {
    posY += speed * sin((rotY + 270) * M_PI / 180.0f);
    posX += speed * cos((rotY + 270) * M_PI / 180.0f);
  }
}

void Camera::moveView(int x, int y) {

  rotY -= speedView * (width / 2 - x);
  rotX += speedView * (height / 2 - y);

  // rot modulo 360 ? 

  if (rotX < -80.0f)
    rotX = -80.0f;
  else if (rotX > 80.0f)
    rotX = 80.0f;

}

void Camera::mousePressed(int x, int y)
{
    lastX = x;
    lastY = y;
}

void Camera::drawAim()
{
    ofDisableDepthTest();

    ofPushMatrix();
    ofTranslate(width/2, height/2);
    ofSetLineWidth(4);
    ofSetColor(100, 100, 100);
    ofLine(-10, 0, 10, 0);
    ofLine(0, -10, 0, 10);
    ofPopMatrix();

    ofEnableDepthTest();
}
