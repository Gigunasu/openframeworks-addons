#ifndef __OFX_RSSDK_H__
#define __OFX_RSSDK_H__

#include "ofMain.h"
#include "pxcsensemanager.h"
#include "pxcprojection.h"
#include "ofxBase3DVideo.h"


/// \class ofxRSSDK
///
/// Base3dVideo implementation of RealSense SDK
/// based on ofxKinect
///
class ofxRSSDK : public ofxBase3DVideo, protected ofThread {

public:

	ofxRSSDK();
	virtual ~ofxRSSDK();

	bool init(bool pGrabVideo=true, bool pUseTexture=true);
	void clear();
	void setRegistration(bool bUseRegistration = false);
	void close();

	bool isConnected();
	bool isFrameNew();
	void update();

	ofVec3f getWorldCoordinateAt(int cx, int cy);
	ofVec3f getWorldCoordinateAt(float cx, float cy, float wz);
	ofColor getColorAt(int x, int y);
	ofColor getColorAt(const ofPoint & p);

	unsigned char* getPixels();

	ofPixels & getPixelsRef();
	ofPixels & getDepthPixelsRef();       	///< grayscale values
	ofShortPixels & getRawDepthPixelsRef();	///< raw 11 bit values

	ofTexture& getTextureReference();

	void setUseTexture(bool bUse);
	void setCalcCameraPoints(bool pCalc){ bCalcCameraPoints=pCalc; }
	void draw(float x, float y, float w, float h);
	void draw(float x, float y);
	void draw(const ofPoint& point);
	void draw(const ofRectangle& rect);

	const static int width = 1280;
	const static int height = 720;
	float getHeight();
	float getWidth();

	//TODO: Move this stuff to ofxRSSDKContext?
	ofPoint getColorFOV();

	void DrawLandmark( PXCFaceData::Face* trackedFace );
protected:
	bool bUseTexture;
	ofTexture videoTex; ///< the RGB texture
	bool bGrabberInited;

	ofPixels videoPixels;

	// for auto connect tries
	float timeSinceOpen;
	int lastDeviceId;
	bool bGotData;
	int tryCount;

public:

	bool bIsFrameNew;
	bool bNeedsUpdate;
	bool bUpdateTex;
	bool bGrabVideo;
	bool bUseRegistration;
	bool bNearWhite;
	bool bCalcCameraPoints;
	float nearClipping, farClipping;

	bool bIsVideoInfrared;  ///< is the video image infrared or RGB?
	int videoBytesPerPixel; ///< how many bytes per pixel in the video image

	PXCSenseManager* mRealSenseDevice;      ///< kinect device handle
	PXCFaceData* mOutput;
	
	PXCFaceData::LandmarkPoint* mLandmarkPoints;
	int landmarks;
};
#endif