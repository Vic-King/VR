#include "ofApp.h"
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

struct _data{
    ofVec3f* posCam;
    float zDist;
};

//--------------------------------------------------------------
void ofApp::setup(){
    screenResolution = ofVec2f(1440,900);
    screenSize = ofVec2f(285, 180);

    baseDist = 500.f;
    camPos = ofVec3f(0, 0, baseDist);
    halfEyeDist = 8;

    nearPlane = 10;
    farPlane = 2000;

    ofBackground(0,0,0);
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofEnableAntiAliasing();
    ofSetSmoothLighting(true);
    dirLight.setDirectional();
    dirLight.setOrientation(ofQuaternion(90.f, ofVec3f(0, -1, 0)));
    dirLight.setAmbientColor(ofFloatColor(.5,.5,.5));
    dirLight.setDiffuseColor(ofFloatColor(.5,.5,.5));

    material.setAmbientColor(ofFloatColor(1.f,0.f,0.f));
    material.setDiffuseColor(ofFloatColor(1.f,0.f,0.f));

    rx = 0.f;
    ry = 0.f;
    tz = 0.f;
    sign = 1;

    init = false;

    // Open connection
    {
        m_TrackerRemote = new vrpn_Tracker_Remote("Tracker0@localhost");
        float data[4] = {camPos.x, camPos.y, camPos.z, baseDist};
        struct _data* d = new struct _data;
        d->posCam = &camPos;
        d->zDist = baseDist;
        m_TrackerRemote->register_change_handler(d, handle_tracker,0);
    }


}

void ofApp::handle_tracker(void *userdata, const vrpn_TRACKERCB t)
{
  //this function gets called when the tracker's POSITION xform is updated

  //you can change what this callback function is called for by changing
  //the type of t in the function prototype above.
  //Options are:
  //   vrpn_TRACKERCB              position
  //   vrpn_TRACKERVELCB           velocity
  //   vrpn_TRACKERACCCB           acceleration
  //   vrpn_TRACKERTRACKER2ROOMCB  tracker2room transform
  //                                 (comes from local or remote
  //                                  vrpn_Tracker.cfg file)
  //   vrpn_TRACKERUNIT2SENSORCB   unit2sensor transform (see above comment)
  //   vrpn_TRACKERWORKSPACECB     workspace bounding box (extent of tracker)

  // userdata is whatever you passed into the register_change_handler function.
  // vrpn sucks it up and spits it back out at you. It's not used by vrpn internally

    struct _data* d = (struct _data*)userdata;
    d->posCam->set(-t.pos[0], -t.pos[1], d->zDist + t.pos[2]);
    std::cerr << "pos[0] = " << t.pos[2] << std::endl;
}

//--------------------------------------------------------------
void ofApp::update(){
    ry += 1.f;
    rx += 1.f;
    if (abs(tz) == 100)
        sign = -sign;
    tz = tz + sign;
    m_TrackerRemote->mainloop();
}

//--------------------------------------------------------------
void ofApp::draw(){
    drawLeft();
    glClear( GL_DEPTH_BUFFER_BIT );
    drawRight();
    glClear( GL_DEPTH_BUFFER_BIT );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE) ;
    //drawMono();
}

void ofApp::drawMono()
{
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE) ;
    setCamera(camPos);
    drawScene();
}

void ofApp::drawLeft()
{
    glColorMask( GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE) ;
    ofVec3f newPos = camPos;
    newPos.x -= halfEyeDist;
    setCamera(newPos);
    drawScene();
}

void ofApp::drawRight()
{
    glColorMask( GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE) ;
    ofVec3f newPos = camPos;
    newPos.x += halfEyeDist;
    setCamera(newPos);
    drawScene();
}

void ofApp::setCamera(const ofVec3f& cameraPos)
{
    float nearFactor = nearPlane / cameraPos.z;
    float left   = nearFactor * (- screenSize.x / 2.f - cameraPos.x);
    float right  = nearFactor * (  screenSize.x / 2.f - cameraPos.x);
    float top    = nearFactor * (  screenSize.y / 2.f - cameraPos.y);
    float bottom = nearFactor * (- screenSize.y / 2.f - cameraPos.y);

    ofMatrix4x4 projection;
    projection.makeFrustumMatrix(left,right,bottom,top,nearPlane,farPlane);

    ofSetMatrixMode(OF_MATRIX_PROJECTION);
    ofLoadMatrix(projection);

    ofMatrix4x4 view;
    view.makeLookAtViewMatrix(cameraPos,ofVec3f(cameraPos.x,cameraPos.y,0.f),ofVec3f(0.f,1.f,0.f));

    ofSetMatrixMode(OF_MATRIX_MODELVIEW);
    ofLoadMatrix(view);
}

void ofApp::drawScene()
{
    ofEnableLighting();
    dirLight.enable();

    // Draw a rotating cube
    ofPushMatrix();
    ofTranslate(0, 0, tz);
    ofRotateY(ry);
    ofRotateX(rx);

    ofSetColor(255,255,255);

    material.begin();
    ofFill();
    // cube de 5cm
    ofScale(50.f,50.f,50.f);
    ofDrawBox(1);
    material.end();

    ofPopMatrix();

    dirLight.disable();
    ofDisableLighting();

    // Draw a 3D grid
    ofNoFill();
    ofSetLineWidth(1.5f);

    int numLines = 10;
    float lineDist = 40;
    int intensStep = 255 / numLines;

    // Horizontal lines
    float stepSize = screenSize.x / numLines;

    for (int i = 0; i <  numLines; i++) {
        ofSetColor(255 - i * intensStep);

        // Bottom
        ofLine(-screenSize.x/2.f, -screenSize.y/2.f, -i * lineDist,
               screenSize.x/2.f, -screenSize.y/2.f, -i * lineDist);

        // Top
        ofLine(-screenSize.x/2.f, screenSize.y/2.f, -i * lineDist,
               screenSize.x/2.f, screenSize.y/2.f, -i * lineDist);

        // Line which goes to Z
        // Bottom
        for (float j = -screenSize.x/2.f; j <  screenSize.x/2.f ; j+= stepSize) {
            ofLine(j, -screenSize.y/2.f, -i * lineDist,
                   j, -screenSize.y/2.f, -i * lineDist - lineDist);
        }

        // Top
        for (float j = -screenSize.x/2.f; j <  screenSize.x/2.f ; j+= stepSize) {
            ofLine(j, screenSize.y/2.f, -i * lineDist,
                   j, screenSize.y/2.f, -i * lineDist - lineDist);
        }
    }

    stepSize = screenSize.y / numLines;
    // Vertical lines
    for (int i = 0; i <  numLines; i++) {
        ofSetColor(255 - i * intensStep);

        // left
        ofLine(-screenSize.x/2.f, screenSize.y/2.f, -i * lineDist,
               -screenSize.x/2.f, -screenSize.y/2.f, -i * lineDist);

        // right
        ofLine(screenSize.x/2.f, -screenSize.y/2.f, -i * lineDist,
               screenSize.x/2.f, screenSize.y/2.f, -i * lineDist);

        // Line which goes to Z
        // left
        for (float j = -screenSize.y/2.f; j <  screenSize.y/2.f ; j+= stepSize) {
            ofLine(-screenSize.x/2.f, j, -i * lineDist,
                   -screenSize.x/2.f, j, -i * lineDist - lineDist);
        }

        // right
        for (float j = -screenSize.y/2.f; j <=  screenSize.y/2.f ; j+= stepSize) {
            ofLine(screenSize.x/2.f, j, -i * lineDist,
                   screenSize.x/2.f, j, -i * lineDist - lineDist);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'm'){
        lastX = mouseX;
        lastY = mouseY;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    if(!init){
        lastX = x;
        lastY = y;
        init = true;
    }
    int deltaX = x - lastX;
    int deltaY = y - lastY;
    //std::cerr << "delta x : " << deltaX << std::endl;
    ofVec3f newPos = camPos;
    newPos.x += deltaX;
    newPos.y += deltaY;
    setCamera(newPos);
    camPos = newPos;
    lastX = x;
    lastY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
