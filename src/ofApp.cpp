#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
// shader
#ifdef TARGET_OPENGLES
    shader.load("shadersES2/shader");
    /// shader reload
    shaderPath = ofFilePath::getAbsolutePath("shadersES2/shader.frag",true);

#else
    if(ofIsGLProgrammableRenderer()){
        shader.load("shadersGL3/shader");
        /// shader reload
        shaderPath = ofFilePath::getAbsolutePath("shadersGL3/shader.frag",true);
    }else{
        shader.load("shadersGL2/shader2.vert","shadersGL2/shader2.frag");
        /// shader reload
        shaderPath = ofFilePath::getAbsolutePath("shadersGL2/shader2.frag",true);
    }
#endif
   

    stat(shaderPath.c_str(),&stat_buf);
    last_file_time = stat_buf.st_mtime;
    
    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(0, 0, 0);
    
    soundStream.printDeviceList();
    
    int bufferSize = 256;
//    int bufferSize = 512;

    // shader mode
    shaderMode = 0;
    

// -------------------- Audio section
    sampleRate 			= 44100;
    phase 				= 0;
    phaseAdder 			= 0.0f;
    phaseAdderTarget 	= 1.0f;
    volume				= 0.1f;
    bNoise 				= false;
    bSin                = false;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);
    
    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    // you can also get devices for an specific api
    // auto devices = soundStream.getDevicesByApi(ofSoundDevice::Api::PULSE);
    // settings.device = devices[0];
    
    // or get the default device for an specific api:
    // settings.api = ofSoundDevice::Api::PULSE;
    
    // or by name
    
    auto devices = soundStream.getDeviceList();
    
//    devices = soundStream.getMatchingDevices("default"); // if slect default audio device
    
    //    settings.device = devices[0];
    if(!devices.empty()){
//        settings.setInDevice(devices[1]); // mic in
        settings.setInDevice(devices[3]); // sound flower
        settings.setOutDevice(devices[2]); // built in out put
    }
    
    settings.setInListener(this); // important
    settings.setOutListener(this);// important
    settings.sampleRate = 44100;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    soundStream.start();

// ------------- GUI section
    parameters.setName("settings");
    parameters.add(vSync.set("vSync",true));
    //vSync.addListener(this, &ofApp::vSyncChanged);
    gui.setup(parameters);
    
    gui.loadFromFile("settings.xml");


    
}

//--------------------------------------------------------------
void ofApp::update(){
    //lets scale the vol up to a 0-1 range
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    //lets record the volume into an array
    volHistory.push_back( scaledVol );
    
    //if we are bigger the the size we want to record - lets drop the oldest value
    if( volHistory.size() >= 400 ){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
    
    stat(shaderPath.c_str(),&stat_buf);

    if (stat_buf.st_mtime != last_file_time) {
        // 更新チェック
//        cout << "shader changed " << endl;
//        cout << "current_file_time : " + ofToString(ctime(&stat_buf.st_mtime)) << endl;        
        last_file_time = stat_buf.st_mtime;
        shader.load("shader2.frag",shaderPath);
    }
    
//    cout << "last_file_time :" + ofToString(ctime(&last_file_time)) << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(255);
    
    float volL;
    float volR;
    
    for (unsigned int i = 0; i < left.size(); i++){
        //        volL = 100 -left[i]*180.0f;
        volL = 1 -left[i]*180.0f;
    }
    
    for (unsigned int i = 0; i < right.size(); i++){
        volR = 1 -right[i]*180.0f;
    }
    
//    cout << volL << endl;
//    cout << " " << endl;
//    cout << volR << endl;
    
    shader.begin();
    shader.setUniform1f("time",ofGetElapsedTimef());
    shader.setUniform1f("volL",volL);
    shader.setUniform1f("volR",volR);
    shader.setUniform2f("resolution",ofGetWidth(),ofGetHeight());
    shader.setUniform1f("shadermode",shaderMode);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    shader.end();
    
    ofSetColor(19);
    ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio\n 'n' is noise\n 'm' is sign wave\n 'z' is no effect.", 31, 92);
    
    ofDrawBitmapString(targetFrequency, 31, 579);
    ofDrawBitmapString(volL, 31, 589);
    ofDrawBitmapString(volR, 31, 600);
        
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (size_t i = 0; i < input.getNumFrames(); i++){
        left[i]		= input[i*2];
        right[i]	= input[i*2+1];
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    switch(key) {
        case 's':
            soundStream.start();
            ofSerialize(xmlSettings,parameters);
            xmlSettings.save("settings.xml");
            break;
            
        case 'e':
            xmlSettings.load("settings.xml");
            ofDeserialize(xmlSettings, parameters);
            soundStream.stop();
            break;
            
        case 'n':
            if (bNoise == false) {
                bNoise = true;
            } else {
                bNoise = false;
            }
            break;
            
        case 'm':
            if (bSin == false) {
                bSin = true;
            } else {
                bSin = false;
            }
            break;
            
        case 'z':
            bNoise = false;
            bSin = false;
            break;
            
        case 'r':
            shader.load("shadersGL2/shader2");
            break;
        
        // shader mode
        case '0':
            shaderMode = 0;
            break;
        case '1':
            shaderMode = 1;
            break;
        case '2':
            shaderMode=2;
            break;
        case '3':
            shaderMode=3;
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    int width = ofGetWidth();
    pan = (float)x / (float)width;
    float height = (float)ofGetHeight();
    float heightPct = ((height-y)/height);
    targetFrequency = 2000.0f * heightPct;
    phaseAdderTarget = (targetFrequency/(float) sampleRate) * TWO_PI;
    //cout << targetFrequency << endl;
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    int width = ofGetWidth();
    pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//    bNoise = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
//    bNoise = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------

//void ofApp::audioOut( float * output, int bufferSize, int nChannels ) {
void ofApp::audioOut(ofSoundBuffer & buffer) {
    pan = 0.5f;
    float leftScale = 1 -pan;
    float rightScale = pan;
    
    while (phase > TWO_PI) {
        phase -= TWO_PI;
    }
    
    phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
    
    // want to change case
    
    
    if ( bNoise == true) {
        for (size_t i = 0; i < buffer.getNumFrames(); i++) {
            left[i] = buffer[i*buffer.getNumChannels() ] = ofRandom((0,1) * volume *left[i]);
            right[i] = buffer[i*buffer.getNumChannels() + 1 ] = ofRandom((0,1) * volume * right[i]);
        }
    } else if(bSin == true) {
        for (size_t i = 0; i < buffer.getNumFrames(); i++ ){
            phase += phaseAdder;
            float sample = sin(phase);
            left[i] = buffer[i*buffer.getNumChannels()    ] = sample * volume * leftScale * left[i];
            right[i] = buffer[i*buffer.getNumChannels() + 1] = sample * volume * rightScale * right[i];
        }
    } else {
        for (size_t i = 0; i < buffer.getNumFrames(); i++ ){ // no effect

            left[i] = buffer[i*buffer.getNumChannels()    ] = volume  * left[i];
            right[i] = buffer[i*buffer.getNumChannels() + 1] =  volume  * right[i];
        }
    }
    
}


//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

