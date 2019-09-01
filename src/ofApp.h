#pragma once

#include "ofMain.h"
// File kanshi
#include <sys/stat.h>
#include "ofxGui.h"

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
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
				
    void audioIn(ofSoundBuffer & input);
    void audioOut(ofSoundBuffer & buffer);
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    float 	volume;
    
    int 	bufferCounter;
    int     drawCounter;
    
    float smoothedVol;
    float scaledVol;
    
    
    float 	pan;
    int		sampleRate;
    bool 	bNoise;
    bool    bSin;
    
    
    //------------------- for the simple sine wave synthesis
    float 	targetFrequency;
    float 	phase;
    float 	phaseAdder;
    float 	phaseAdderTarget;
    
    ofSoundStream soundStream;
    ofSoundStreamSettings settings;

    
    // ------------------- shader
    ofShader shader;
    
    int shaderMode;
    
    // --- for live coding
    // shader path
    string shaderPath;
    // stat
    struct stat stat_buf;
    time_t last_file_time; //ファイルの最終更新時間
    
    // debug mode
    bool debug;
    
    // GUI
    ofxPanel gui;
    ofParameter<bool> vSync;
    ofParameterGroup parameters;
    ofXml xmlSettings;

};
