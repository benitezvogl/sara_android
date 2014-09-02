#include "testApp.h"
#include "ofxAndroidVideoGrabber.h"
#include <jni.h>
//--------------------------------------------------------------
void testApp::setup() {
	backColor.set(0, 0, 0);
	value = -1;
	//ofSleepMillis(1000);

	JNIEnv *env = ofGetJNIEnv();
	jclass localClass = env->FindClass("cc/openframeworks/androidUiExample/OFActivity");
	javaClass = (jclass) env->NewGlobalRef(localClass);
	if (!javaClass) {
		ofLog() << "javaClass not found!" << endl;
	}

	javaObject = ofGetOFActivityObject();
	javaObject = (jobject) env->NewGlobalRef(javaObject);
	if (!javaObject) {
		ofLog() << "javaObject not found!" << endl;
	}

	//CAMERA
	grabber = new ofVideoGrabber();
	grabber->listDevices();
	isCamStarted = false;
	cam_selected = -1;
	//grabber->setDeviceID(cam_selected);
	//grabber->initGrabber(960,720);
	one_second_time = ofGetElapsedTimeMillis();
	camera_fps = 0;
	frames_one_sec = 0;
	w = 960;
	h = 720;

	//ACCELEROMETER
	ofxAccelerometer.setup();

	ofLog() << "Test 1: " << ofFilePath::getCurrentWorkingDirectory();
	ofLog() << "Test 2: " << ofFilePath::getUserHomeDir();
	ofLog() << "Test 3: " << ofFilePath::getPathForDirectory("/");

	roll = pitch = yaw = cRed = cBlue = cGreen = 0.0f;
	vidFilter = 0;
}

//--------------------------------------------------------------
void testApp::update() {

	//CAMERA
	if(grabber->isInitialized() && isCamStarted){
		grabber->update();
		if(grabber->isFrameNew()){
			frames_one_sec++;
			if( ofGetElapsedTimeMillis() - one_second_time >= 1000){
				camera_fps = frames_one_sec;
				frames_one_sec = 0;
				one_second_time = ofGetElapsedTimeMillis();
			}
		}

		if(vidFilter != 0){
			vidFiltered.setFromPixels(grabber->getPixelsRef());
			pixels = vidFiltered.getPixels();

			for (int i = 0; i < w; i++){
				for (int j = 0; j < h; j++){
					if(vidFilter == 1){
						int r = pixels[(j*w+i)*bpp+0];
						int g = pixels[(j*w+i)*bpp+1];
						int b = pixels[(j*w+i)*bpp+2];
						int grey = (r+b+g)/3;
						pixels[(j*w+i)*bpp+0] = grey;
						pixels[(j*w+i)*bpp+1] = grey;
						pixels[(j*w+i)*bpp+2] = grey;
					}
					if(vidFilter == 2){
						int r = 255 - pixels[(j*w+i)*bpp+0];
						int g = 255 - pixels[(j*w+i)*bpp+1];
						int b = 255 - pixels[(j*w+i)*bpp+2];
						pixels[(j*w+i)*bpp+0] = r;
						pixels[(j*w+i)*bpp+1] = g;
						pixels[(j*w+i)*bpp+2] = b;
					}
				}
			}

			vidFiltered.setFromPixels(pixels, w, h, OF_IMAGE_COLOR);
			myImage.setFromPixels(vidFiltered.getPixelsRef());
			//myImage.mirror(0,1);
		}else{
			myImage.setFromPixels(grabber->getPixelsRef());
		}

		//w = myImage.width;
		//h = myImage.height;
		//int type = myImage.type;
		bpp = myImage.bpp / 8;
		pixels = myImage.getPixels();
		cRed = 0;
		cGreen = 0;
		cBlue = 0;
		int cnt = 0;
		for (int i = 0; i < w; i++){
			for (int j = 0; j < h; j++){
				if(bpp >= 3){
					cRed += pixels[(j*w+i)*bpp+0];
					cGreen += pixels[(j*w+i)*bpp+1];
					cBlue += pixels[(j*w+i)*bpp+2];
					if(bpp >= 4){
						//float cAlpha = pixels[(j*w+i)*bpp+3];
					}
				}
				else if(bpp == 1){
					//float cGray = pixels[(j*w+i)];
				}
				cnt++;
			}
		}
		cRed = ((cRed / (float)cnt) / 255.0f) * 180.0f;
		cGreen = ((cGreen / (float)cnt) / 255.0f) * 180.0f;
		cBlue = ((cBlue / (float)cnt) / 255.0f) * 180.0f;

	}

	//core.update();

	//ACCELEROMETER
	accel = ofxAccelerometer.getForce();
	normAccel = accel.getNormalized();

	roll = accel.x * 180.0;
	pitch = accel.y * 180.0;
	yaw = accel.z * 180.0;

	messages[0] = ofToString(roll,2);
	messages[1] = ofToString(pitch,2);
	messages[2] = ofToString(yaw,2);


	JNIEnv *envVideo = ofGetJNIEnv();
	jmethodID javaReturnMethodVideo = envVideo->GetMethodID(javaClass,"returnValueVideo","()I");
	if(javaReturnMethodVideo){
		vidFilter = envVideo->CallIntMethod(javaObject, javaReturnMethodVideo);
	}


	JNIEnv *envDisplay = ofGetJNIEnv();
	jmethodID javaReturnMethodDisplay = envDisplay->GetMethodID(javaClass,"returnValueDisplay","()I");
	if(javaReturnMethodDisplay){
		showValues=(bool)envDisplay->CallIntMethod(javaObject, javaReturnMethodDisplay);
	}

	//SEND AND RECEIVE VARIABLES TO/FROM oF
	JNIEnv *env = ofGetJNIEnv();
	jmethodID javaReturnMethod = env->GetMethodID(javaClass,"returnValue","(FFFFFF)F");

	if(!javaReturnMethod){
		ofLog() << "javaReturnMethod not found!" << endl;
	}else{
		value=env->CallFloatMethod(javaObject,javaReturnMethod, roll, pitch, yaw, cRed, cGreen, cBlue);

		if((int)value != (int)cam_selected){
			//isCamStarted = false;
		}

		cam_selected = (int)value;

		if (!isCamStarted && cam_selected == 0) {
			ofLog() << "switch to back" << endl;
			grabber->close();
			grabber->setDeviceID(0);
			grabber->initGrabber(960,720);
			isCamStarted = true;
			w = grabber->getWidth();
			h = grabber->getHeight();

		} else if (!isCamStarted && cam_selected == 1) {
			ofLog() << "switch to front" << endl;
			grabber->close();
			grabber->setDeviceID(1);
			grabber->initGrabber(960,720);
			isCamStarted = true;
			w = grabber->getWidth();
			h = grabber->getHeight();

		}

	}


}

float testApp::getRoll() {
	return roll;
}
float testApp::getPitch() {
	return pitch;
}
float testApp::getYaw() {
	return yaw;
}

//--------------------------------------------------------------
void testApp::draw() {
	//UI Example
	ofBackground(backColor);
	//ofDrawBitmapString(ofToString(ofGetFrameRate(),2),10,10);

	//CAMERA
	ofSetHexColor(0xFFFFFF);
	if(grabber->isInitialized() && (vidFilter == 0))
		grabber->draw(0,0,ofGetScreenWidth(),ofGetScreenHeight());

	if(grabber->isInitialized() && (vidFilter == 1 || vidFilter == 2))
		myImage.draw(0,0,ofGetScreenWidth(),ofGetScreenHeight());

	//img.draw(0,0);


	if(vidFilter == 3){
		ofSetColor(0);
		ofRect(0,0,w,h);
		int size = 20;
		int step = 20;

		for (int y=0; y<h; y+=step) {
			for (int x=0; x<w; x+=step) {

				//get pixel color from camera
				int i = (y*w+x)*3;
				int r = grabber->getPixels()[i+0];
				int g = grabber->getPixels()[i+1];
				int b = grabber->getPixels()[i+2];

				//calculate brightness
				//float bri = (r+g+b) / 765.0;

				//draw black circles
				ofSetColor(r,g,b);
				ofRect(x,y,size,size);
			}
		}

	}

	if(!showValues){
		return;
	}

	ofSetHexColor(0x000000);
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()),20,350);
	ofDrawBitmapString("camera fps: " + ofToString(camera_fps),20,370);

	//ACCELEROMETER
	ofSetColor(255,255,0);
	ofDrawBitmapString("x: " + ofToString(messages[0]),20,390);
	ofSetColor(255,0,255);
	ofDrawBitmapString("y: " + ofToString(messages[1]),20,410);
	ofSetColor(0,255,255);
	ofDrawBitmapString("z: " + ofToString(messages[2]),20,430);
	ofSetColor(255,0,0);
	ofDrawBitmapString("red: " + ofToString(cRed),20,480);
	ofSetColor(0,255,0);
	ofDrawBitmapString("green: " + ofToString(cGreen),20,500);
	ofSetColor(0,0,255);
	ofDrawBitmapString("blue: " + ofToString(cBlue),20,520);
	ofSetColor(255,255,255);
	ofDrawBitmapString("camera: " + ofToString(cam_selected),20,550);

}

// this is where the openframeworks sound stream connects to ofxPd
// it's also where the audio processing is done
//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels) {
	//core.audioReceived(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
	//core.audioRequested(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void testApp::touchDown(int x, int y, int id) {

}

//--------------------------------------------------------------
void testApp::touchMoved(int x, int y, int id) {

}

//--------------------------------------------------------------
void testApp::touchUp(int x, int y, int id) {

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(int x, int y, int id) {

}

//--------------------------------------------------------------
void testApp::touchCancelled(int x, int y, int id) {

}

//--------------------------------------------------------------
void testApp::swipe(ofxAndroidSwipeDir swipeDir, int id) {

}

//--------------------------------------------------------------
void testApp::pause() {

}

//--------------------------------------------------------------
void testApp::destroy() {
	grabber->close();
	delete grabber;
}

//--------------------------------------------------------------
void testApp::stop() {
	grabber->close();
	delete grabber;
}

//--------------------------------------------------------------
void testApp::resume() {

}

//--------------------------------------------------------------
void testApp::reloadTextures() {

}

//--------------------------------------------------------------
bool testApp::backPressed() {
	return false;
}

//--------------------------------------------------------------
void testApp::okPressed() {

}

//--------------------------------------------------------------
void testApp::cancelPressed() {

}
