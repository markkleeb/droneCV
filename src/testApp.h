#pragma once


#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARDrone.h"


class testApp : public ofBaseApp{
    
public:
    
    
    
    //drone variables
    ofxARDrone::Drone drone;
    bool doPause;
    bool keys[65535];
    
    //TCP variables
    ofxTCPClient tcpClient;
    string msgTx, msgRx;
    ofTrueTypeFont  mono;
    ofTrueTypeFont  monosm;
    float counter;
    int connectTime;
    int deltaTime;
    bool weConnected;
    int size;
    int pos;
    bool typed;
    
    
    //OpenCV Stuff
    
    ofxCvColorImage colorImg;
    ofImage img;
    ofPixels pixels;
    
    bool debug;
    
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
};
