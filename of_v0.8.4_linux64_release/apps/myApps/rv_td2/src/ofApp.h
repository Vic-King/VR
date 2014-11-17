#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxCvContourFinder.h"

#include <vrpn_Connection.h>
#include <vrpn_Tracker.h>
#include <vrpn_Tracker_Fastrak.h>

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:
    void seuillage(ofColor c, int min, int max);

    ofColor rgbToHsv(ofColor crgb);

    ofVideoGrabber cam;

    ofColor targetColor;

    int img_width, img_height;
    ofxCvColorImage img;
    ofxCvGrayscaleImage imgSeuillee;

    ofxCvContourFinder contours;
    ofxCvHaarFinder faceRecognizer;

    bool drawImgSeuil;

    std::vector<double> m_CameraMatrix;
    std::vector<double> m_DistCoeff;
    vrpn_Connection* m_Connec;
    vrpn_Tracker_Server* m_Tracker;
};
