//
//  ofxHPGL.h
//  emptyExample
//
//  Created by Nick Hardeman on 2/4/16.
//

#pragma once
#include "ofMain.h"

class ofxHPGLSerialCommand {
public:
    ofxHPGLSerialCommand() {
        command                 = "";
        printerResponse         = "";
        bSent                   = false;
        timeout                 = 3000;
        bDidReceiveResponse     = false;
    }
    
    void sent() {
        timeSent = ofGetElapsedTimeMillis();
        bSent = true;
    }
    void received( string aresponse ) {
        timeReceived    = ofGetElapsedTimeMillis();
        printerResponse = aresponse;
        bDidReceiveResponse = true;
//        cout << "Setting the printer response: " << printerResponse << endl;
    }
    bool didReceiveResponse() {
        return ( bDidReceiveResponse );
    }
    
    bool bSent;
    string command;
    string printerResponse;
    unsigned int timeSent;
    unsigned int timeReceived;
    unsigned int timeout;
protected:
    bool bDidReceiveResponse;
};

class ofxHPGLCommand {
public:
    enum CommandType {
        SHAPE = 0,
        PEN
    };
    
    ofxHPGLCommand() {
        penIndex    = -1;
        type        = SHAPE;
    }
    ofxHPGLCommand( ofPolyline aline ) {
        penIndex    = -1;
        type        = SHAPE;
        polyline    = aline;
    }
    
    void setPen( int aPen ) {
        type = PEN;
        penIndex = aPen;
    }
    
    int penIndex;
    int type;
    ofPolyline polyline;
};

class ofxHPGL {
public:
    
    class Settings {
    public:
        enum PaperSize {
            PAPER_SIZE_A3 = 0,
            PAPER_SIZE_A4
        };
        
        Settings();
        
        PaperSize paperSize;
        string serialDevicePath;
        int baud;
    };
    
    ofxHPGL();
    ~ofxHPGL();
    
    void setup( int aDeviceIndedex );
    void setup( string aPortName );
    void setup( Settings asettings );
    
    void setInputWidth( float aw );
    void setInputHeight( float ah );
    
    bool start();
    void stop();
    vector<ofSerialDeviceInfo> getSerialDevices();
    
    bool isConnected();
    
    void update();
    void draw();
    
    void circle( float ax, float ay, float aradius );
    void rectangle( ofRectangle arect );
    void rectangle( float ax, float ay, float awidth, float aheight );
    void line( float ax, float ay, float ax2, float ay2 );
    void triangle( float ax, float ay, float ax2, float ay2, float ax3, float ay3 );
    void triangle( ofVec2f ap1, ofVec2f ap2, ofVec2f ap3 );
    void polyline( ofPolyline aline );
    
    void setPen( int aPenIndex );
    
    void clear();
    void print();
    
    ofVec2f getPenPosition();
    int getPenStatus();
    ofRectangle getHardClipLimits();
    
    void addCommand( string astr );
    string getCommand( string aprefix, int ax );
    string getCommand( string aprefix, int ax, int ay );
    
    ofSerial serial;
    
    string message;
    
protected:
//    void scalePoly( ofPolyline& apoly );
    void sendCommand( string astr );
    bool bthreadReceivedPrinterResponse;
    
    void sendBlockingResponse( ofxHPGLSerialCommand& aCommand );
    
    vector< ofxHPGLSerialCommand > printerCommands;
    
    vector< ofxHPGLCommand > commands;
    vector< ofPolyline > drawPolys;
    int penIndex;
    Settings _settings;
    string serialIn;
    
    float _inWidth, _inHeight;
//    ofVec2f _scale;
    
};






