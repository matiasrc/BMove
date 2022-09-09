#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    //ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    //----------------- XML -------------------
    loadSettings();
    
    //----------------- CAM -------------------
    
    //viewCam            = false;  // en el XML
    camWidth            = 640;
    camHeight           = 480;

    //deviceID            = 1;
    deviceName          = "NO DEVICE AVAILABLE";

    //hMirror             = false; // en el XML
    //vMirror             = false; // en el XML
       
    isOneDeviceAvailable =  false;
    
    setupCam(deviceID);
       
    promedioDeMovimiento = 0.0f;
    //----------------- GUI -------------------
    //required call
    gui.setup();
    
    ImGui::GetIO().MouseDrawCursor = false;

    //show_plotline = false; //en el XML
    
    //----------------- OSC -------------------
    
    sender.setup(host, puerto);
}

void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {

		// take the absolute difference of prev and cam and save it inside diff
		absdiff(cam, previous, diff);
		diff.update();
		
		// like ofSetPixels, but more concise and cross-toolkit
		copy(cam, previous);
		
		// mean() returns a Scalar. it's a cv:: function so we have to pass a Mat
		diffMean = mean(toCv(diff));
		
		// you can only do math between Scalars,
		// but it's easy to make a Scalar from an int (shown here)
		diffMean *= Scalar(3);
        
        // use the [] operator to get elements from a Scalar
        float diffRed = diffMean[0];
        float diffGreen = diffMean[1];
        float diffBlue = diffMean[2];
        
        promedioDeMovimiento = ofClamp(ofMap((( diffRed + diffGreen + diffBlue ) / 3) , 0, 255, 0, 1), 0.0f, 1.0f);
        
        if(enviarMOVIMIENTO) enviarOsc(etiquetaMOVIMIENTO, promedioDeMovimiento);
	}
}

void ofApp::draw() {
    
    float x = 0;
    float y = 0;
    float ancho = cam.getWidth();
    float alto = cam.getHeight();
    
    if(hMirror){
        x = cam.getWidth();
        ancho = -cam.getWidth();
    }
    if(vMirror){
        y = cam.getHeight();
        alto = - cam.getHeight();
    }
    
    if(viewCam){
        cam.draw(x, y, ancho, alto);
    }else{
        diff.draw(x, y, ancho, alto);
    }
	
    ofSetColor(255);
    ofDrawRectangle(0, 10, ofMap(promedioDeMovimiento, 0, 1, 0, ofGetWidth()), 10);
    drawGui();
}

void ofApp::setupCam(int devID){
    
    wdevices = cam.listDevices();
    for(int i=0;i<static_cast<int>(wdevices.size());i++){
        if(wdevices[i].bAvailable){
            isOneDeviceAvailable = true;
            devicesVector.push_back(wdevices[i].deviceName);
            devicesID.push_back(i);

            for(size_t f=0;f<wdevices[i].formats.size();f++){
                ofLog(OF_LOG_NOTICE,"Capture Device format vailable: %ix%i",wdevices[i].formats.at(f).width,wdevices[i].formats.at(f).height);
            }
        }
    }
    
    cam.setDeviceID(devID);
    cam.setup(camWidth, camHeight);
}
void ofApp::resetCameraSettings(int devID){
    if(devID!=deviceID){
        ofLog(OF_LOG_NOTICE,"Changing Device to: %s",devicesVector[devID].c_str());

        deviceID = devID;
        deviceName = devicesVector[deviceID];
    }
    
    if(cam.isInitialized()){
        cam.close();

        //cam = new ofVideoGrabber();
        cam.setDeviceID(deviceID);
        cam.setup(camWidth, camHeight);
    }
}

//--------------------------------------------------------------
void ofApp::enviarOsc(string etiqueta, float valor){
    ofxOscMessage m;
    m.setAddress(etiqueta);
    m.addFloatArg(valor);
    sender.sendMessage(m, false);
}
//--------------------------------------------------------------
void ofApp::enviarOsc(string etiqueta, vector<float> valores){
    ofxOscMessage m;
    m.setAddress(etiqueta);
    for( int i=0; i<valores.size(); i++){
        m.addFloatArg(valores[i]);
    }
    sender.sendMessage(m, false);
    
}
//--------------------------------------------------------------
void ofApp::loadSettings(){
    //-----------
    //the string is printed at the top of the app
    //to give the user some feedback
    xmlMessage = "loading mySettings.xml";

    //we load our settings file
    //if it doesn't exist we can still make one
    //by hitting the 's' key
    if( XML.loadFile("mySettings.xml") ){
        xmlMessage = "mySettings.xml loaded!";
    }else{
        xmlMessage = "unable to load mySettings.xml check data/ folder";
    }

    //read the colors from XML
    //if the settings file doesn't exist we assigns default values (170, 190, 240)
    //red        = XML.getValue("BACKGROUND:COLOR:RED", 170);
    
    
    puerto = XML.getValue("OSC:PUERTO", 3333);
    host = XML.getValue("OSC:HOST", "127.0.0.1");
    
    etiquetaMOVIMIENTO = XML.getValue("OSC:ETIQUETA:MOVIMIENTO", "/bmove/movimiento");
    
    enviarMOVIMIENTO = XML.getValue("OSC:ENVIARMOVIMIENTO", true);
    
    
    deviceID = XML.getValue("CAM:DEVICEID", 0);
    hMirror = XML.getValue("CAM:HMIRROR", false);
    vMirror = XML.getValue("CAM:VMIRROR", false);
    
    show_plotline = XML.getValue("GUI:VISUALIZADOR", false);
    
    viewCam = XML.getValue("CAM:CAMARA", false);
    
    ofLog(OF_LOG_NOTICE,xmlMessage);
}
//--------------------------------------------------------------
void ofApp::saveSettings(){
    //XML.setValue("BACKGROUND:COLOR:RED", red);
    XML.clear();

    XML.setValue("OSC:PUERTO", puerto);
    XML.setValue("OSC:HOST", host);
    
    XML.setValue("OSC:ETIQUETA:MOVIMIENTO", etiquetaMOVIMIENTO);

    XML.setValue("OSC:ENVIARMOVIMIENTO", enviarMOVIMIENTO);
    
    
    XML.setValue("CAM:DEVICEID", deviceID);
    XML.setValue("CAM:HMIRROR", hMirror);
    XML.setValue("CAM:VMIRROR", vMirror);
    
    XML.setValue("GUI:VISUALIZADOR", show_plotline);
    
    XML.setValue("CAM:CAMARA", viewCam);
    
    XML.saveFile("mySettings.xml");
    xmlMessage ="settings saved to xml!";
    ofLog(OF_LOG_NOTICE,xmlMessage);

}

//--------------------------------------------------------------
void ofApp::keyPressed(ofKeyEventArgs& e){
    //no data gets saved unless you hit the s key
    if(e.key == 's' && e.hasModifier(OF_KEY_COMMAND)){
        saveSettings();
    }
}
