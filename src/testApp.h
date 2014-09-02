#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxAccelerometer.h"
#include "AppCore.h"

class testApp : public ofxAndroidApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		void destroy();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();

		float value;
		ofColor backColor;

		// variable needed for import value from java
		jclass javaClass;
		jobject javaObject;

		// CAMERA
		ofVideoGrabber* grabber;
		int cam_selected;
		int one_second_time;
		int camera_fps;
		int frames_one_sec;
		bool isCamStarted;

		ofImage vidFiltered;

		// ACCELEROMETER
		//ofTrueTypeFont font;
		ofVec3f accel, normAccel;
		string messages[3];

		//ofxPD
		//AppCore core;
		void audioReceived(float * input, int bufferSize, int nChannels);
		void audioRequested(float * output, int bufferSize, int nChannels);

		float roll, pitch, yaw;
		float getRoll();
		float getPitch();
		float getYaw();

		ofImage img;
		ofImage myImage;
		float cRed;
		float cGreen;
		float cBlue;

		int w;
		int h;
		int bpp;
		unsigned char * pixels;

		bool showValues;
		int returnShowValues;

		int vidFilter;
};
