#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    hp.setup( "/dev/tty.usbserial-A10172HG" );
//    hp.start();
    hp.setPen( 1 );
    
}

//--------------------------------------------------------------
void ofApp::update() {
    
    hp.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    hp.draw();
    for( int i = 0; i < polylines.size(); i++ ) {
        polylines[i].draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if( key == 'p' ) {
        hp.clear();
        for( int i = 0; i < polylines.size(); i++ ) {
            hp.polyline( polylines[i] );
        }
        hp.print();
//        hp.addCommand( "OH;", true );
//        cout << hp.getHardClipLimits() << endl;
    }
    if( key == 127 ) {
        polylines.clear();
        hp.clear();
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
    if(polylines.size()) polylines.back().addVertex( x, y );
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    polylines.push_back( ofPolyline() );
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if( polylines.size() && polylines.back().getPerimeter() > 10 ) {
        polylines.back() = polylines.back().getResampledBySpacing( 8 );
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