//
//  ofxHPGL.h
//  emptyExample
//
//  Created by Nick Hardeman on 2/4/16.
//

#pragma once
#include "ofMain.h"
#include "ofxXmlSettings.h"

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
        PEN,
        CIRCLE,
        RECTANGLE
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
    
    void circle( float ax, float ay, float aradius ) {
        pos.set( ax, ay );
        type    = CIRCLE;
        radius  = aradius;
        
    }
    
    void rectangle( float ax, float ay, float aw, float ah ) {
        pos.set( ax, ay );
        type    = RECTANGLE;
        width   = aw;
        height  = ah;
        
    }
    
    void setPen( int aPen ) {
        type        = PEN;
        penIndex    = aPen;
    }
    
    ofVec2f pos;
    float radius;
    int penIndex;
    float width, height;
    int type;
    ofPolyline polyline;
};

class ofxHPGL {
public:
    
    static bool shouldRemoveSentCommand( const ofxHPGLSerialCommand& ac );
    
    class Settings {
    public:
        enum PaperSize {
            PAPER_SIZE_A3 = 1,
            PAPER_SIZE_A4 = 4
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
    
    bool load( string aFilePath );
    bool save( string aFilePath );
    
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
    void setPaperSize( int aPaperSize );
    
    ofVec2f getPrinterPosFromInput( ofVec2f aInput, ofRectangle aDestRect );
    
    void clear();
    void print();
    bool isPrinting();
    void pause();
    void resume();
    
    ofVec2f getPenPosition();
    int getPenStatus();
    ofRectangle getHardClipLimits();
    
    void addCommand( string astr );
    string getCommand( string aprefix, int ax );
    string getCommand( string aprefix, int ax, int ay );
    
    void sendCommand( string astr );
    
    ofSerial serial;
    
    string message;
    
    int getAvailBufferSize();
    void sendBlockingResponse( ofxHPGLSerialCommand& aCommand );
    
protected:
    
    bool bthreadReceivedPrinterResponse;
    
    vector< ofxHPGLSerialCommand > printerCommands;
    
    vector< ofxHPGLCommand > commands;
    vector< ofPolyline > drawPolys;
    int penIndex;
    Settings _settings;
    string serialIn;
    
    float _inWidth, _inHeight;
    bool bTryToConnectToPrinter;
    unsigned int lastTryToConnectTime;
    bool bPause;
};






