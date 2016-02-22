#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    hp.setup( "/dev/tty.usbserial-A10172HG" );
//    hp.start();
    hp.load("hpgl.xml");
}

//--------------------------------------------------------------
void ofApp::update() {
    
    hp.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    hp.draw();
    
    polyline.draw();
//    for( int i = 0; i < polylines.size(); i++ ) {
//        polylines[i].draw();
//    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if( key == 'p' ) {
        hp.clear();
//        for( int i = 0; i < polylines.size(); i++ ) {
//            hp.polyline( polylines[i] );
//        }
        hp.print();
//        hp.addCommand( "OH;", true );
//        cout << hp.getHardClipLimits() << endl;
    }
    if( key == 127 ) {
        polyline.clear();
        hp.clear();
    }
    if( key == 's' ) {
//        hp.save( ofGetTimestampString()+".xml" );
        hp.save( "hpgl.xml" );
    }
//    if( key == 'a' ) {
//        cout << hp.getPenPosition() << endl;
//    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    polyline.addVertex( x, y );
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
//    polylines.push_back( ofPolyline() );
    polyline.clear();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if( polyline.getPerimeter() > 10 ) {
        polyline = polyline.getResampledBySpacing( 8 );
        
        hp.polyline( polyline );
        polyline.clear();
    }
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
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}