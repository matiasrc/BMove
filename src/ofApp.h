#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxImGui.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(ofKeyEventArgs& e);
    void setupCam(int devID);
    void resetCameraSettings(int devID);
	
    //----------------- CV -------------------
    ofVideoGrabber          cam;
    vector<ofVideoDevice>   wdevices;
    vector<string>          devicesVector;
    vector<int>             devicesID;
    int                     camWidth, camHeight;
    string                  deviceName;
    int                     deviceID;
    bool                    needReset;
    bool                    isOneDeviceAvailable;
    bool                    hMirror, vMirror;
    
    ofPixels previous;
    ofImage diff;
	
    // a scalar is like an ofVec4f but normally used for storing color information
    cv::Scalar diffMean;
    
    float promedioDeMovimiento;

    //----------------- GUI -------------------
    void drawGui();
    ofxImGui::Gui gui;
    bool show_plotline;
        
    //----------------- OSC -------------------
    ofxOscSender sender;
    int puerto;
    string host;
    string etiquetaMOVIMIENTO;
    bool enviarMOVIMIENTO;
    
    void enviarOsc(string etiqueda, float valor);
    void enviarOsc(string etiqueda, vector<float> valores);
    
    
    
    //----------------- XML -------------------
    ofxXmlSettings XML;
    void saveSettings();
    void loadSettings();
    string xmlMessage;
    bool viewCam;
    
};
