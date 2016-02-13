//
//  ofxHPGL.cpp
//  emptyExample
//
//  Created by Nick Hardeman on 2/4/16.
//

#include "ofxHPGL.h"

//--------------------------------------------------------------
ofxHPGL::Settings::Settings() {
    paperSize           = PAPER_SIZE_A3;
    serialDevicePath    = "";
    baud = 9600;
}

//--------------------------------------------------------------
ofxHPGL::ofxHPGL() {
    penIndex = -1;
    _inWidth = -1;
    _inHeight = -1;
}

//--------------------------------------------------------------
ofxHPGL::~ofxHPGL() {
    stop();
}

//--------------------------------------------------------------
void ofxHPGL::setup( int aDeviceIndedex ) {
    setup( getSerialDevices()[aDeviceIndedex].getDevicePath() );
}

//--------------------------------------------------------------
void ofxHPGL::setup( string aPortName ) {
    ofxHPGL::Settings tsettings;
    tsettings.serialDevicePath = aPortName;
    setup( tsettings );
}

//--------------------------------------------------------------
void ofxHPGL::setup( Settings asettings ) {
    _settings = asettings;
    if( _settings.serialDevicePath == "" ) {
        _settings.serialDevicePath = getSerialDevices()[0].getDevicePath();
    }
    start();
    if( _inWidth < 10 ) {
        setInputWidth( ofGetWidth() );
    }
    if( _inHeight < 10 ) {
        setInputHeight( ofGetHeight() );
    }
}

//--------------------------------------------------------------
bool ofxHPGL::load( string aFilePath ) {
    ofxXmlSettings txml;
    bool bOk = txml.load( aFilePath );
    if( bOk ) {
        clear();
        float inw = txml.getAttribute("screen", "width", 0.0 );
        float inh = txml.getAttribute("screen", "height", 0.0 );
        setInputWidth( inw );
        setInputHeight( inh );
        string tcname = "command";
        int numTags = txml.getNumTags(tcname);
        for( int i = 0; i < numTags; i++ ) {
            int ttype = txml.getAttribute(tcname, "type", -1, i );
            if( ttype == ofxHPGLCommand::PEN ) {
                int penIndex = txml.getAttribute(tcname, "penIndex", 1, i );
                setPen( penIndex );
            } else if(ttype == ofxHPGLCommand::RECTANGLE ) {
                ofRectangle trect;
                trect.x = txml.getAttribute(tcname, "x", 0.0, i );
                trect.y = txml.getAttribute(tcname, "y", 0.0, i );
                trect.width = txml.getAttribute(tcname, "width", 0.0, i );
                trect.height = txml.getAttribute(tcname, "height", 0.0, i );
                rectangle( trect.x, trect.y, trect.width, trect.height );
            } else if( ttype == ofxHPGLCommand::CIRCLE ) {
                circle(txml.getAttribute(tcname, "x", 0.0, i ),
                          txml.getAttribute(tcname, "y", 0.0, i ),
                          txml.getAttribute(tcname, "radius", 0.0, i ));
            } else if( ttype == ofxHPGLCommand::SHAPE ) {
                if( txml.pushTag(tcname, i )) {
                    int numPTags = txml.getNumTags( "point" );
                    ofPolyline tpoly;
                    for( int j = 0; j < numPTags; j++ ) {
                        tpoly.addVertex(
                                        txml.getAttribute("point", "x", 0.0, j ),
                                        txml.getAttribute("point", "y", 0.0, j )
                        );
                    }
                    
                    polyline( tpoly );
                    
                    txml.popTag();
                }
            }
        }
    }
    return bOk;
}

//--------------------------------------------------------------
bool ofxHPGL::save( string aFilePath ) {
    
    ofFile tfile( aFilePath );
    string filepath = aFilePath;
    if( tfile.getExtension() != "xml" ) {
        filepath = tfile.getEnclosingDirectory()+"/"+tfile.getBaseName()+".xml";
    }
    
    cout << "ofxHPGL :: save : " << filepath << endl;
    
    ofxXmlSettings txml;
    
    txml.addTag("screen");
    txml.addAttribute("screen", "width", _inWidth, 0 );
    txml.addAttribute("screen", "height", _inHeight, 0 );
    
    for( int i = 0; i < commands.size(); i++ ) {
        ofxHPGLCommand& tc = commands[i];
        string tname = "command";
        int ttag = txml.addTag(tname);
        txml.addAttribute(tname, "type", tc.type, ttag );
        
        if( tc.type == ofxHPGLCommand::PEN ) {
            txml.addAttribute(tname, "penIndex", tc.penIndex, ttag );
        } else if( tc.type == ofxHPGLCommand::RECTANGLE ) {
            txml.addAttribute(tname, "x", tc.pos.x, ttag );
            txml.addAttribute(tname, "y", tc.pos.y, ttag );
            txml.addAttribute(tname, "width", tc.width, ttag );
            txml.addAttribute(tname, "height", tc.height, ttag );
        } else if( tc.type == ofxHPGLCommand::CIRCLE ) {
            txml.addAttribute(tname, "x", tc.pos.x, ttag );
            txml.addAttribute(tname, "y", tc.pos.y, ttag );
            txml.addAttribute(tname, "radius", tc.radius, ttag );
            
        } else if( tc.type == ofxHPGLCommand::SHAPE ) {
            if( txml.pushTag( tname, i )) {
                for( int j = 0; j < tc.polyline.size(); j++ ) {
                    int ptag = txml.addTag("point");
                    txml.addAttribute("point", "x", tc.polyline[j].x, ptag );
                    txml.addAttribute("point", "y", tc.polyline[j].y, ptag );
                }
            } txml.popTag();
        }
    }
    return txml.save( filepath );
}

//--------------------------------------------------------------
void ofxHPGL::setInputWidth( float aw ) {
    _inWidth = aw;
}

//--------------------------------------------------------------
void ofxHPGL::setInputHeight( float ah ) {
    _inHeight = ah;
}

//--------------------------------------------------------------
bool ofxHPGL::start() {
    bool bOk = false;
    if( serial.isInitialized() ) {
        serial.close();
    }
    bOk = serial.setup( _settings.serialDevicePath, _settings.baud );
    return bOk;
}

//--------------------------------------------------------------
void ofxHPGL::stop() {
    if( serial.isInitialized() ) {
        serial.close();
    }
}

//--------------------------------------------------------------
vector<ofSerialDeviceInfo> ofxHPGL::getSerialDevices() {
    return serial.getDeviceList();
}

//--------------------------------------------------------------
bool ofxHPGL::isConnected() {
    return serial.available();
}

//--------------------------------------------------------------
void ofxHPGL::update() {
        
    if( printerCommands.size() ) {
        if( !printerCommands[0].bSent ) {
            printerCommands[0].sent();
            cout << "sending command " << printerCommands[0].command << " | " << ofGetFrameNum() << endl;
            sendCommand( printerCommands[0].command );
            
            printerCommands.erase( printerCommands.begin() );
        }
        ofSleepMillis( 50 );
    }
////        bool bErasePCommand = true;
////        if( printerCommands[0].bWaitForPrinterResponse ) {
////            if( ofGetElapsedTimeMillis() - printerCommands[0].timeSent < printerCommands[0].timeout ) {
////                bErasePCommand = false;
////            }
////            if( printerCommands[0].printerResponse != "" ) {
////                bErasePCommand = true;
////                cout << "printer command received: " << printerCommands[0].printerResponse << " | " << ofGetFrameNum() << endl;
////            }
////        }
////        
////        if( bErasePCommand ) {
////            printerCommands.erase( printerCommands.begin() );
////        }
//    }
    
//    if( serial.available() ) {
//        
//        unsigned char buffer[1];
//        while( serial.readBytes( buffer, 1) > 0){
//            if (buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == '\t') {
//                //                    message = serialIn;
//                if( printerCommands.size() ) {
//                    if( printerCommands[0].bWaitForPrinterResponse ) {
//                        printerCommands[0].received( serialIn );
//                        if( printerCommands[0].command == "OH;" ) {
//                            //                                setHardClipLimits( serialIn );
//                        }
//                    }
//                }
//                serialIn = "";
//                break;
//            }
//            serialIn.push_back( buffer[0] );
//        };
//    }
    
};

//--------------------------------------------------------------
void ofxHPGL::draw() {
    // digest the lines //
//    vector< ofPolyline > lines = polylines;
//    for( int i = 0; i < commands.size(); i++ ) {
//        ofxHPGLCommand& com = commands[i];
//        if( com.type == ofxHPGLCommand::SHAPE ) {
//            ofPolyline& cpoly = com.polyline;
//            vector< ofPoint > verts = cpoly.getVertices();
//            for( int j = 0; j < verts.size()-1; j++ ) {
//                ofDrawLine( verts[j].x, verts[j].y, verts[j+1].x, verts[j+1].y );
//            }
//        } else if( com.type == ofxHPGLCommand::PEN ) {
//            cout << i << " - Setting the pen to " << com.penIndex << endl;
//        }
//    }
    
    for( int i = 0; i < drawPolys.size(); i++ ) {
        drawPolys[i].draw();
    }
    
    ofDrawBitmapStringHighlight( "m: "+message, 40, 40 );
}

#pragma mark - Drawing Commands 
//--------------------------------------------------------------
//void ofxHPGL::scalePoly( ofPolyline& apoly ) {
//    vector< ofPoint >& tpts = apoly.getVertices();
//    for( int i = 0; i < tpts.size(); i++ ) {
////        tpts[i].x *= _scale.x;
////        tpts[i].y *= _scale.y;
//        
//    }
//}

//--------------------------------------------------------------
void ofxHPGL::circle( float ax, float ay, float aradius ) {
    ofPolyline tpoly;
    tpoly.arc( ofPoint( ax, ay, 0), aradius, aradius, 0, 360, 60 );
    
    drawPolys.push_back( tpoly );
//    scalePoly( tpoly );
    ofxHPGLCommand tc;
    tc.circle( ax, ay, aradius );
    commands.push_back( tc );
}

//--------------------------------------------------------------
void ofxHPGL::rectangle( ofRectangle arect ) {
    ofPolyline tpoly;
    
    tpoly.addVertex( arect.getTopLeft() );
    tpoly.addVertex( arect.getTopRight() );
    tpoly.addVertex( arect.getBottomRight() );
    tpoly.addVertex( arect.getBottomLeft() );
    tpoly.addVertex( arect.getTopLeft() );
    
    drawPolys.push_back( tpoly );
//    scalePoly( tpoly );
    
    ofxHPGLCommand tc;
    tc.rectangle( arect.x, arect.y, arect.width, arect.height );
    commands.push_back( tc );
}

//--------------------------------------------------------------
void ofxHPGL::rectangle( float ax, float ay, float awidth, float aheight ) {
    rectangle( ofRectangle(ax, ay, awidth, aheight ));
}

//--------------------------------------------------------------
void ofxHPGL::line( float ax, float ay, float ax2, float ay2 ) {
    ofPolyline tpoly;
    tpoly.addVertex( ax, ay );
    tpoly.addVertex( ax2, ay2 );
    
    drawPolys.push_back( tpoly );
//    scalePoly( tpoly );
    
    commands.push_back( ofxHPGLCommand(tpoly) );
}

//--------------------------------------------------------------
void ofxHPGL::triangle( float ax, float ay, float ax2, float ay2, float ax3, float ay3 ) {
    triangle( ofVec2f(ax, ay), ofVec2f(ax2,ay2), ofVec2f(ax3,ay3) );
}

//--------------------------------------------------------------
void ofxHPGL::triangle( ofVec2f ap1, ofVec2f ap2, ofVec2f ap3 ) {
    ofPolyline tpoly;
    tpoly.addVertex( ap1 );
    tpoly.addVertex( ap2 );
    tpoly.addVertex( ap3 );
    tpoly.addVertex( ap1 );
    
    drawPolys.push_back( tpoly );
//    scalePoly( tpoly );
    
    commands.push_back( ofxHPGLCommand(tpoly) );
}

//--------------------------------------------------------------
void ofxHPGL::polyline( ofPolyline aline ) {
    if( !aline.size() ) return;
    
    ofPolyline tpoly = aline;
    
    drawPolys.push_back( tpoly );
//    scalePoly( tpoly );
    
    commands.push_back( ofxHPGLCommand(tpoly) );
}

//--------------------------------------------------------------
void ofxHPGL::setPen( int aPenIndex ) {
    if( aPenIndex < 1 || aPenIndex > 6 ) {
        ofLogWarning("ofxHPGL :: setPen : pen index ( "+ofToString( aPenIndex,0)+" ) is out of range (0-6)" );
        return;
    }
    
    if( aPenIndex != penIndex ) {
        penIndex = aPenIndex;
        ofxHPGLCommand com;
        com.setPen( penIndex );
        commands.push_back( com );
    }
}

//--------------------------------------------------------------
ofVec2f ofxHPGL::getPrinterPosFromInput( ofVec2f aInput, ofRectangle aDestRect ) {
    ofVec2f nvert;
    nvert.x = ofMap( aInput.x, 0, _inWidth, 0, aDestRect.width, true );
    nvert.y = ofMap( aInput.y, 0, _inHeight, aDestRect.height, 0, true );
    return nvert;
}

//--------------------------------------------------------------
void ofxHPGL::clear() {
    penIndex = -1;
    commands.clear();
    printerCommands.clear();
    drawPolys.clear();
}

// http://www.piclist.com/techref/language/hpgl/commands.htm
//--------------------------------------------------------------
void ofxHPGL::print() {
    if( !serial.isInitialized() ) {
        ofLogWarning("ofxHPGL :: print : printer not available.");
        return;
    }
    
    cout << "Sending to print : " << commands.size() << " | " << ofGetFrameNum() << endl;
    
    // clear the incoming commands to the printer //
    addCommand("IN;");
//    addCommand("DF;");
//    addCommand("PU;");
//    addCommand("SC;");
    addCommand("PA;");
    
    // make sure to set the pen //
    // add in a command for that //
    if( penIndex < 1 ) {
        setPen( 1 );
    }
    
    ofRectangle destRect = getHardClipLimits();
    float dscalex = destRect.width / _inWidth;
    float dscaley = destRect.height / _inHeight;
//    addCommand( getCommand("SP", penIndex ));
    
    for( int i = 0; i < commands.size(); i++ ) {
        ofxHPGLCommand& com = commands[i];
        if( com.type == ofxHPGLCommand::SHAPE ) {
            ofPolyline& cpoly = com.polyline;
            vector< ofPoint > verts = cpoly.getVertices();
            
            for( int j = 0; j < verts.size(); j++ ) {
                // move the verts into the right place for the plotter //
//                ofVec2f nvert;
//                nvert.x = ofMap( verts[j].x, 0, _inWidth, destRect.width, 0, true );
//                nvert.x = ofMap( verts[j].x, 0, _inWidth, 0, destRect.width, true );
//                nvert.y = ofMap( verts[j].y, 0, _inHeight, destRect.height, 0, true );
                
                ofVec2f nvert = getPrinterPosFromInput( verts[j], destRect );
                
                if( j == 0 ) {
                    addCommand(getCommand("PU", (int)nvert.x, (int)nvert.y ));
                }
                addCommand(getCommand("PD", (int)nvert.x, (int)nvert.y ));
//                ofDrawLine( verts[j].x, verts[j].y, verts[j+1].x, verts[j+1].y );
            }
            addCommand("PU;");
        } else if( com.type == ofxHPGLCommand::PEN ) {
            cout << i << " - Setting the pen to " << com.penIndex << endl;
            addCommand( getCommand("SP", com.penIndex ));
        } else if( com.type == ofxHPGLCommand::CIRCLE ) {
//            PU1500,1500;
//            CI500;
            ofVec2f nvert = getPrinterPosFromInput( ofVec2f(com.pos.x, com.pos.y), destRect );
            addCommand( getCommand("PU", nvert.x, nvert.y ));
            addCommand( getCommand("CI", com.radius*dscalex ));
            addCommand("PU;");
        } else if( com.type == ofxHPGLCommand::RECTANGLE ) {
//            RA (Rectangle Absolute - Filled, from current position to diagonal x,y):
//            EA (rEctangle Absolute - Unfilled, from current position to diagonal x,y):
//            EA x, y;
            ofVec2f nvert = getPrinterPosFromInput( ofVec2f(com.pos.x, com.pos.y), destRect );
            addCommand( getCommand("PU", nvert.x, nvert.y ));
            addCommand( getCommand("EA", nvert.x+com.width*dscalex, nvert.y+com.height*dscaley ));
            addCommand( "PU;" );
        }
    }
    
    addCommand("PU;");
    
    string pstring = "";
    for( int i = 0; i < printerCommands.size(); i++ ) {
        pstring += printerCommands[i].command;
    }
    
//    printerCommands.clear();
//    
//    cout << " PRINTING about to send string " << endl;
//    cout << pstring << endl;
//    cout << "---------------------------- " << endl;
//    
//    sendCommand( pstring );

    
    
//    sendCommand( getCommand("SP", penIndex ));
    
    // a4 size - A Size
    // p1 250, 596
    // p2 10250, 7796
    // xaxis max 0 to 10365
    // yaxis max 0 to 7962
    
    // a3 size - B Size
    // p1 522, 259
    // p2 15722, 10259
    // xaxis max 0 to 16640
    // yaxis max 0 to 10365
    
    // fill type
//    FT type, spacing and angle
    
    // input window
    // IW xlo, ylo, x hi, y hi
    
    // Set line type(i) and pattern length(d)
    // LT pattern #, pattern length
    
    // user -unit scaling(i)
    // xmin, xmax, ymin, ymax
    
}

//--------------------------------------------------------------
ofVec2f ofxHPGL::getPenPosition() {
    ofVec2f tpos;
    ofxHPGLSerialCommand com;
    com.command = "OA;";
    sendBlockingResponse( com );
    if( com.didReceiveResponse() ) {
        vector< string > tstrings = ofSplitString( com.printerResponse, "," );
        if( tstrings.size() == 3 ) {
            tpos.set( ofToInt(tstrings[0]), ofToInt(tstrings[1]) );
        }
    }
    return tpos;
}

//--------------------------------------------------------------
int ofxHPGL::getPenStatus() {
    int tstatus = -1;
    ofxHPGLSerialCommand com;
    com.command = "OA;";
    sendBlockingResponse( com );
    if( com.didReceiveResponse() ) {
        vector< string > tstrings = ofSplitString( com.printerResponse, "," );
        if( tstrings.size() == 3 ) {
            tstatus = ofToInt( tstrings[2] );
        }
    }
    return tstatus;
}

//--------------------------------------------------------------
ofRectangle ofxHPGL::getHardClipLimits() {
    ofRectangle trect;
    ofxHPGLSerialCommand com;
    com.command = "OH;";
    sendBlockingResponse( com );
    if( com.didReceiveResponse() ) {
        vector< string > tstrings = ofSplitString( com.printerResponse, "," );
        if( tstrings.size() == 4 ) {
            trect.set( ofToInt(tstrings[0]), ofToInt(tstrings[1]), ofToInt(tstrings[2]), ofToInt(tstrings[3]) );
        }
    }
    return trect;
}

//--------------------------------------------------------------
void ofxHPGL::addCommand( string astr ) {
    
    ofxHPGLSerialCommand com;
    com.command = astr;
    printerCommands.push_back( com );
    
}

//--------------------------------------------------------------
void ofxHPGL::sendBlockingResponse( ofxHPGLSerialCommand& aCommand ) {
    aCommand.sent();
    sendCommand( aCommand.command );
    int tnumCountTries = 0;
    string tserialIn = "";
    
    while ( tnumCountTries < 1000 && !aCommand.didReceiveResponse() ) {
        if( serial.available() ) {
            
            unsigned char buffer[1];
            while( serial.readBytes( buffer, 1) > 0){
                if (buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == '\t') {
                    aCommand.received( tserialIn );
                    tserialIn = "";
                    break;
                }
                tserialIn.push_back( buffer[0] );
            };
        }
        tnumCountTries++;
        ofSleepMillis( 100 );
    }
}

//--------------------------------------------------------------
string ofxHPGL::getCommand( string aprefix, int ax ) {
    string outstr = aprefix;
    outstr += ofToString( ax, 0 );
    outstr += ";";
    outstr += '\n';
    return outstr;
}

//--------------------------------------------------------------
string ofxHPGL::getCommand( string aprefix, int ax, int ay ) {
    string outstr = aprefix;
    outstr += ofToString( ax, 0 );
    outstr += ",";
    outstr += ofToString( ay, 0 );
    outstr += ";";
    outstr += '\n';
    return outstr;
}


// protected
//--------------------------------------------------------------
void ofxHPGL::sendCommand( string astr ) {
    if( serial.isInitialized() ) {
        unsigned char buf[ (int)astr.size() ];
        for( int i = 0; i < astr.size(); i++ ) {
            buf[ i ] = astr[i];
        }
        serial.writeBytes(buf, (int)astr.size() );
    } else {
        cout << "serial not initialized " << endl;
    }
    
}




