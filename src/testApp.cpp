#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    //Clear all keys
    memset(keys, 0, sizeof(*keys));
    
    doPause = false;
    debug = false;
    drone.connect();
    
    // setup command history lengths for debugging and dumping onscreen (OPTIONAL)
    //drone.controller.commandHistory.setMaxLength(30);
    //drone.dataReceiver.commandHistory.setMaxLength(30);}
    
    //-----------------------------
    //TCP stuff
    //have we typed?
    typed = false;
    
    //our typing position
    pos = 0;
    
    //send and receive strings
    msgTx = "";
    msgRx = "";
    
    //are we connected to the server - if this fails we
	//will check every few seconds to see if the server exists
	weConnected = tcpClient.setup("192.168.1.1", 5555);
	//optionally set the delimiter to something else.  The delimter in the client and the server have to be the same
	tcpClient.setMessageDelimiter("\n");
	
	connectTime = 0;
	deltaTime = 0;
    
	tcpClient.setVerbose(true);
    
    //--------------------------------
    
    colorImg.allocate(50, 52);
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    //DRONE STUFF-----------------------------------------
    
    if(doPause) return;
    
    {
        float s = 0.02;
        
        if(keys[OF_KEY_UP]) drone.controller.pitchAmount -= s;
        else if(keys[OF_KEY_DOWN]) drone.controller.pitchAmount += s;
        
        if(keys['a']) drone.controller.rollAmount -= s;
        else if(keys['d']) drone.controller.rollAmount += s;
        
        if(keys['w']) drone.controller.liftSpeed += s;
        else if(keys['s']) drone.controller.liftSpeed -= s;
        
        if(keys[OF_KEY_LEFT]) drone.controller.spinSpeed -= s;
        else if(keys[OF_KEY_RIGHT]) drone.controller.spinSpeed += s;
        
    }
    
    // update the drone (process and send queued commands to drone, receive commands from drone and update state
    drone.update();
    
    
    
    
    //TCP STUFF--------------------------------------------
    
    bool dataRecd = false;
    unsigned char buffer[32];
    int recd = 32;
    int totalReceived = 0;
    int messageSize = 0;
    while(recd > 0) {
        
        if(recd > messageSize  ){
            tcpClient.receiveRawBytes( (char*) &buffer[totalReceived], messageSize);
            recd -= messageSize;
            totalReceived += messageSize;
        }
        else{
            tcpClient.receiveRawBytes( (char*) &buffer[totalReceived], recd);
            totalReceived += recd;
            recd = 0;
            dataRecd = true;
        }
    }
    
    if(dataRecd){
        
        colorImg.setFromPixels( &buffer[0], 50, 52); //,OF_IMAGE_COLOR);
    }
    
    
    /*
     //we are connected - lets send our text and check what we get back
     if(weConnected){
     if(tcpClient.send(msgTx)){
     
     //if data has been sent lets update our text
     string str = tcpClient.receive();
     if( str.length() > 0 ){
     msgRx = str;
     ofBuffer databuffer = str;
     
     }
     }else if(!tcpClient.isConnected()){
     weConnected = false;
     }
     }else{
     //if we are not connected lets try and reconnect every 5 seconds
     deltaTime = ofGetElapsedTimeMillis() - connectTime;
     
     if( deltaTime > 5000 ){
     weConnected = tcpClient.setup("192.161.1.1", 5555);
     connectTime = ofGetElapsedTimeMillis();
     }
     
     }
     
     
     */
    
}



//--------------------------------------------------------------
void testApp::draw(){
    
    
    ofBackground(0);
    ofEnableAlphaBlending();
    glEnable(GL_DEPTH_TEST);
    
    colorImg.draw(100, 100);
    
    
    
    //DRONE DEBUG
    
    // draw debug strings
    string controllerString = "fps: " + ofToString(ofGetFrameRate()) + "\n";
    controllerString += "millisSinceLastSend: " + ofToString(drone.controller.getMillisSinceLastSend()) + "\n";
    controllerString += "\n";
    controllerString += "takeOff (t)\n";
    controllerString += "land (l)\n";
    controllerString += "calibrateHorizontal (c)\n";
    controllerString += "calibrateMagnetometer (m)\n";
    controllerString += "EMERGENCY (E)\n";
    controllerString += "\n";
    controllerString += "roll (a/d)        : " + ofToString(drone.controller.rollAmount) + "\n";
    controllerString += "pitch (up/down)   : " + ofToString(drone.controller.pitchAmount) + "\n";
    controllerString += "lift (w/s)        : " + ofToString(drone.controller.liftSpeed) + "\n";
    controllerString += "spin (left/right) : " + ofToString(drone.controller.spinSpeed) + "\n";
    controllerString += "\n";
    controllerString += "reset droneSimulator (r)\n";
    controllerString += "debug history (h)\n";
    controllerString += "fullscreen (f)\n";
    controllerString += "PAUSE (p)\n";
    
    ofxARDrone::State &state = drone.state;
    string stateString = "";
    stateString += "isFlying : " + ofToString(state.isFlying()) + "\n";
    stateString += "isTakingOff : " + ofToString(state.isTakingOff()) + ", " + ofToString(state.isTakingOffMillis()) + "\n";
    stateString += "isLanding : " + ofToString(state.isLanding()) + ", " + ofToString(state.isLandingMillis()) + "\n";
    stateString += "isCalibratingHorizontal : " + ofToString(state.isCalibratingHorizontal()) + ", " + ofToString(state.isCalibratingHorizontalMillis()) + "\n";
    stateString += "isCalibratingMagnetometer : " + ofToString(state.isCalibratingMagnetometer()) + ", " + ofToString(state.isCalibratingMagnetometerMillis()) + "\n";
    
    
    stateString += "\n\nisConnected: " + ofToString(state.isConnected()) + ", " + ofToString(state.isCalibratingMagnetometerMillis()) + "\n";
    stateString += "altitude: "+ ofToString(state.getAltitude())+"\n";
    stateString += "emergency state: "+ ofToString(state.inEmergencyMode())+"\n";
    stateString += "battery level: "+ ofToString(state.getBatteryPercentage())+"%\n";
    stateString += "vx: "+ ofToString(state.getVx())+" vy: "+ ofToString(state.getVy())+" vz: "+ ofToString(state.getVz())+"\n";
    
    
    //TCP String
    string TCPstring = "";
    TCPstring += "TCP Status\n";
    TCPstring += "Connected : " + ofToString(weConnected) + "\n";
    TCPstring += "Sending : " + msgTx + "\n";
    TCPstring += "From Server : " + msgRx + "\n";
    
    
    
    if(debug == true){
        ofSetColor(0, 200, 0);
        ofDrawBitmapString(controllerString, 10, 30);
        ofDrawBitmapString(stateString, ofGetWidth()-300, 30);
        ofDrawBitmapString(TCPstring, 10, 400);
        
        ofDrawBitmapString(drone.controller.commandHistory.getAsString(), 10, 280);
        ofDrawBitmapString(drone.dataReceiver.commandHistory.getAsString("\n"), ofGetWidth()-300, 280);
        
        
        
        
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    switch(key) {
        case '1': drone.controller.exitBootstrap(); break;
        case '2': drone.controller.sendAck(); break;
        case '3': drone.dataReceiver.sendDummyPacket(); break;
        case '0': drone.controller.resetCommunicationWatchdog(); break;
            
        case 't': drone.controller.takeOff(!drone.state.isTakingOff(), 3000); break;
        case 'l': drone.controller.land(!drone.state.isLanding(), 3000); break;
        case 'c': drone.controller.calibrateHorizontal(!drone.state.isCalibratingHorizontal(), 3000); break;
        case 'm': drone.controller.calibrateMagnetometer(!drone.state.isCalibratingMagnetometer(), 3000); break;
        case 'p': doPause ^= true; break;
            
        case 'e': drone.controller.emergency(0); break;
        case 'E': drone.controller.emergency(1); break;
            
        case 'R': drone.resetSequenceNumber(); break;
            
        case 'h':
            drone.controller.commandHistory.setMaxLength(drone.controller.commandHistory.getMaxLength() ? 0 : (ofGetHeight()-280)/14);
            drone.dataReceiver.commandHistory.setMaxLength(drone.controller.commandHistory.getMaxLength());
            break;
            
        case 'f': ofToggleFullscreen(); break;
            
        case ' ': debug = !debug; break;
    }
    
    keys[key] = true;
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
    keys[key] = false;
    
    switch(key) {
        case OF_KEY_UP:
        case OF_KEY_DOWN:
            drone.controller.pitchAmount = 0;
            break;
            
        case OF_KEY_LEFT:
        case OF_KEY_RIGHT:
            drone.controller.spinSpeed = 0;
            break;
            
        case 'w':
        case 's':
            drone.controller.liftSpeed = 0;
            break;
            
        case 'a':
        case 'd':
            drone.controller.rollAmount = 0;
            break;
            
    }
    
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    mousePressed(x, y, button);
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
    if(drone.controller.commandHistory.getMaxLength()) {
        drone.controller.commandHistory.setMaxLength((ofGetHeight()-280)/14);
        drone.dataReceiver.commandHistory.setMaxLength(drone.controller.commandHistory.getMaxLength());
    }
    
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}