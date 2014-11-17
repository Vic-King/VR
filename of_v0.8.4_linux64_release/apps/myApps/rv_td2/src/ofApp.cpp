#include "ofApp.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdlib>


//--------------------------------------------------------------
void ofApp::setup(){
    img_width = 640;
    img_height = 480;
    cam.setDesiredFrameRate(30.0);
    cam.initGrabber(img_width,img_height);

    img.allocate(img_width,img_height);
    imgSeuillee.allocate(img_width,img_height);

    drawImgSeuil = false;

    //faceRecognizer.setup("haarcascade_frontalface_alt2.xml");

    {
        setlocale(LC_ALL,"C");
        std::string PATH = ofToDataPath("out_camera_data.xml");
        ofFile file;
        file.open(PATH); // open a file
        ofBuffer buffer = file.readToBuffer(); // read to a buffer

        ofXml data;
        data.loadFromBuffer( buffer.getText() );
        data.setTo("opencv_storage");
        std::string camMat = data.getValue("Camera_Matrix/data");
        std::vector<string> camera_matrix = ofXml::tokenize(camMat, " ");
        std::vector<string>::const_iterator it = camera_matrix.begin();
        std::vector<string>::const_iterator end = camera_matrix.end();
        //for(; it != end; ++it){
        for(int i = 0; i < camera_matrix.size(); ++i){
            //FIXTHIS
            std::cerr << "camera_mat = " << camera_matrix[i] << std::endl;
            //m_CameraMatrix.push_back(stod(camera_matrix[i]));
        }


        std::string distCoeff = data.getValue("Distortion_Coefficients/data");
        std::vector<string> dist_coeff = ofXml::tokenize(distCoeff, " ");
        it = dist_coeff.begin()+1;
        end = dist_coeff.end();
        for(; it != end; ++it){
            std::cerr << "dist : " << (*it) << std::endl;
            m_DistCoeff.push_back(stod((*it)));
        }
    }

    //Launch server
    {
        int	bail_on_error = 1;

        m_Connec = vrpn_create_server_connection();
        if ( (m_Tracker =
              new vrpn_Tracker_Server("Tracker0", m_Connec)) == NULL){
            std::cerr << "Can't create new vrpn_Tracker_Server" << std::endl;
            exit();
        }

    }

}

ofColor ofApp::rgbToHsv(ofColor crgb) {
    float r = crgb.r/255.f;
    float g = crgb.g/255.f;
    float b = crgb.b/255.f;
    float cmax = fmaxf(fmaxf(r,g),b);
    float cmin = fminf(fminf(r,g),b);
    float delta = cmax-cmin;
    float h = (cmax == r) ? 60.f*(g-b)/delta : (cmax == g) ? 60.f*((b-r)/delta+2.f) : 60.f*((r-g)/delta+4.f);
    if(h<0)
        h += 360;
    float s = cmax < 1e-4 ? 0 : delta/cmax;
    return ofColor(h/2,s*255,cmax*255);
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if (cam.isFrameNew()){
        img.setFromPixels(cam.getPixelsRef());
        img.mirror(false,true);
        seuillage(targetColor, 50, 50);
        m_Tracker->mainloop();
        // Send and receive all messages
        m_Connec->mainloop();

        //ofxCvColorImage tmp = img;
//        std::vector<unsigned char> imgPixels;
        //cv::Mat imgMat(img.getCvImage());
        //cv::Mat tmp(img.getCvImage());
        //img.undistort(m_CameraMatrix.at(0), m_CameraMatrix.at(4), m_CameraMatrix.at(2), m_CameraMatrix.at(5));
        //cv::undistort(imgMat, tmp, m_CameraMatrix, m_DistCoeff);
        //img.setFromPixels(imgMat.data, img.getWidth(), img.getHeight());
    }
}



//----------------------------------------------------------haarcascade_frontalface_alt2.xml----
void ofApp::draw(){
    ofSetColor(255,255,255);

    if (drawImgSeuil)
       imgSeuillee.draw(0,0);
    else{
       img.draw(0,0);
//       for(int i = 0; i < faceRecognizer.blobs.size(); i++) {
//            ofRect( faceRecognizer.blobs[i].boundingRect );
//         }
       if(contours.nBlobs > 0){
           contours.blobs.at(0).draw();
           float postitHeight = 1.f;
           //25.4 = focal length / 6.35 = sensor height
            float dist = (25.4 * postitHeight * img_height) / (contours.blobs.at(0).boundingRect.getHeight() * 6.35);
            vrpn_float64 pos[3] = {contours.blobs.at(0).boundingRect.getCenter().x - img_width / 2.f, contours.blobs.at(0).boundingRect.getCenter().y - img_height / 2.f, dist};
            vrpn_float64 quat[4] = {0.f,0.f,0.f,0.f};
            timeval t;
            t.tv_sec = 1.0;
            t.tv_usec = 0.0;
            m_Tracker->report_pose(0, t, pos, quat);
            //std::cerr << "Dist : " << dist << std::endl;
            //send dist to TD3
       }

//       for(int i = 0; i < contours.nBlobs; ++i){
//           contours.blobs.at(i).draw();
//       }
    }



    ofTranslate(8, 8);
    ofFill();
    ofSetColor(0);
    ofRect(-3, -3, 64+6, 64+6);
    ofSetColor(targetColor);
    ofRect(0, 0, 64, 64);
}

void ofApp::seuillage(ofColor c, int min, int max) {
    /*img.convertRgbToHsv(); // EN HSV
    float H,S,V;
    c.getHsb(H,S,V);*/
    for (int i = 0; i < img_width; ++i) {
        for (int j = 0; j < img_height; ++j) {
            if (ofInRange(img.getPixelsRef().getColor(i, j).r, c.r - min, c.r + max) && // Model RGB
                ofInRange(img.getPixelsRef().getColor(i, j).g, c.g - min, c.g + max) &&
                ofInRange(img.getPixelsRef().getColor(i, j).b, c.b - min, c.b + max)) {
                imgSeuillee.getPixelsRef().setColor(i, j, 255);//img.getPixelsRef().getColor(i, j)
            }
            else {
                imgSeuillee.getPixelsRef().setColor(i, j, ofColor(0, 0, 0));
            }

            /*if (ofInRange(img.getPixelsRef().getColor(i, j).r, H-10, H + 10)) {

                imgSeuillee.getPixelsRef().setColor(i, j, 255);//img.getPixelsRef().getColor(i, j)
            }
            else {
                imgSeuillee.getPixelsRef().setColor(i, j, ofColor(0, 0, 0));
            }*/
        }
    }
    for(int i = 0; i<  5; ++i){
        imgSeuillee.dilate_3x3();
        imgSeuillee.erode_3x3();
    }

    imgSeuillee.flagImageChanged();

    contours.findContours(imgSeuillee, 30, img_width * img_height * 0.8, 5, true, false);
   // img.convertHsvToRgb();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 's') {
        drawImgSeuil = !drawImgSeuil;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    targetColor = img.getPixelsRef().getColor(x, y);

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
