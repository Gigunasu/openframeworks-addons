#include "ofxRSSDK.h"
#include "ofMain.h"

//--------------------------------------------------------------------
ofxRSSDK::ofxRSSDK() 
{
	ofLog(OF_LOG_VERBOSE, "ofxRSSDK: Creating ofxRSSDK");

	bUseTexture = true;
	bGrabVideo = true;

	// set defaults
	bGrabberInited = false;

	bNeedsUpdate = false;
	bUpdateTex = false;
	bIsFrameNew = false;

	bIsVideoInfrared = false;
	videoBytesPerPixel = 3;

	mRealSenseDevice = NULL;

	lastDeviceId = -1;
	tryCount = 0;
	timeSinceOpen = 0;
	bGotData = false;

	bUseRegistration = false;
	bNearWhite = true;

	bCalcCameraPoints = false;
}

//--------------------------------------------------------------------
ofxRSSDK::~ofxRSSDK()
{
	close();
	clear();
}

//--------------------------------------------------------------------
bool ofxRSSDK::init(bool pGrabVideo, bool pUseTexture)
{
	if (isConnected()) {
		ofLog(OF_LOG_WARNING, "ofxRSSDK: Do not call init while ofxRSSDK is running!");
		return false;
	}

	clear();
	bGrabVideo = pGrabVideo;
	bUseTexture = pUseTexture;
	videoBytesPerPixel = 4;

	videoPixels.allocate(width, height, videoBytesPerPixel);

	videoPixels.set(0);

	if (bUseTexture)
	{
		videoTex.allocate(width, height, GL_RGBA);
	}

	bGrabberInited = true;
	mRealSenseDevice = PXCSenseManager::CreateInstance();

	// デバイスの作成
	if (!mRealSenseDevice)
	{
		bGrabberInited = false;
	}

	// フェイストラッキングの有効化
	/* Set Mode & Source */
	PXCCaptureManager* captureManager = mRealSenseDevice->QueryCaptureManager();
	pxcStatus status = PXC_STATUS_NO_ERROR;

	/* Set Module */
	mRealSenseDevice->EnableFace();

	/* Initialize */
	PXCFaceModule* faceModule = mRealSenseDevice->QueryFace();
	if (faceModule == NULL)
	{
		//assert(faceModule);
		return false;
	}
	PXCFaceConfiguration* config = faceModule->CreateActiveConfiguration();
	if (config == NULL)
	{
		//assert(config);
		return false;
	}
	config->SetTrackingMode( PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR );
	config->detection.isEnabled = true;
	config->landmarks.isEnabled = true;
	config->pose.isEnabled = true;
	config->QueryExpressions()->DisableAllExpressions();
	config->QueryExpressions()->Disable();
	config->ApplyChanges();

	landmarks = config->landmarks.numLandmarks;
	mLandmarkPoints = new PXCFaceData::LandmarkPoint[landmarks];

	PXCCapture::Device::StreamProfileSet set;
	memset(&set, 0, sizeof(set));
	set.color.imageInfo.width = width;
	set.color.imageInfo.height = height;
	captureManager->FilterByStreamProfiles(&set);

	// ビデオのカラーストリームの有効化
	if (bGrabVideo)
	{
		bGrabberInited = mRealSenseDevice->EnableStream(PXCCapture::STREAM_TYPE_COLOR, width, height, 30) >= PXC_STATUS_NO_ERROR;
	}
	bGrabberInited = mRealSenseDevice->Init() >= PXC_STATUS_NO_ERROR;
	// アウトプットを取得
	mOutput = faceModule->CreateOutput();

	return bGrabberInited;
}

//---------------------------------------------------------------------------
void ofxRSSDK::clear()
{
	if (isConnected()) {
		ofLog(OF_LOG_WARNING, "ofxRSSDK: Do not call clear while ofxRSSDK is running!");
		return;
	}

	videoPixels.clear();
	videoTex.clear();

	bGrabberInited = false;
}

//--------------------------------------------------------------------
void ofxRSSDK::setRegistration(bool bUseRegistration) {
	this->bUseRegistration = bUseRegistration;
}

//---------------------------------------------------------------------------
void ofxRSSDK::close()
{
	bIsFrameNew = false;
	bNeedsUpdate = false;
	bUpdateTex = false;
	if (mRealSenseDevice)
		mRealSenseDevice->Close();
}

//---------------------------------------------------------------------------
bool ofxRSSDK::isConnected()
{
	if (mRealSenseDevice!=nullptr)
		return mRealSenseDevice->IsConnected();
	return false;
}

//--------------------------------------------------------------------
bool ofxRSSDK::isFrameNew()
{
	return bIsFrameNew;
}

//----------------------------------------------------------
void ofxRSSDK::update()
{
	if (!bGrabberInited)
	{
		return;
	}

	// 画像の更新
	if( mRealSenseDevice->AcquireFrame( true ) >= PXC_STATUS_NO_ERROR )
	{
		PXCCapture::Sample *cSample = mRealSenseDevice->QuerySample();

		if (!cSample)
			return;
		if (bGrabVideo)
		{
			PXCImage *cRgbImage = cSample->color;
			if (cRgbImage)
			{
				PXCImage::ImageData cRgbData;
				if( cRgbImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &cRgbData) >= PXC_STATUS_NO_ERROR )
				{
					// フェイスの更新
					mOutput->Update();

					uint8_t *cBuffer = cRgbData.planes[0];
					videoPixels.setFromPixels(cBuffer, width, height, 4);

					cRgbImage->ReleaseAccess(&cRgbData);
				}
			}
		}

		mRealSenseDevice->ReleaseFrame();
	}

	videoTex.loadData(videoPixels.getPixels(), width, height, GL_BGRA);
}

//------------------------------------
ofVec3f ofxRSSDK::getWorldCoordinateAt(float cx, float cy, float wz)
{
	PXCPoint3DF32 cDepthPoint;
	cDepthPoint.x = cx; cDepthPoint.y = cy; cDepthPoint.z = wz;
	PXCPoint3DF32 cDepth[1] = {cDepthPoint};
	PXCPoint3DF32 cCamera[1] = {PXCPoint3DF32()};
	//mProjection->ProjectDepthToCamera(1, cDepth, cCamera);
	
	return ofVec3f(cCamera[0].x, cCamera[0].y, cCamera[0].z);
}

//------------------------------------
ofColor ofxRSSDK::getColorAt(int x, int y)
{
	int index = (y * width + x) * videoBytesPerPixel;
	ofColor c;
	c.r = videoPixels[index + 0];
	c.g = videoPixels[index + (videoBytesPerPixel - 1) / 2];
	c.b = videoPixels[index + (videoBytesPerPixel - 1)];
	c.a = 255;

	return c;
}

//------------------------------------
ofColor ofxRSSDK::getColorAt(const ofPoint & p) {
	return getColorAt(p.x, p.y);
}

//---------------------------------------------------------------------------
unsigned char * ofxRSSDK::getPixels() {
	return videoPixels.getPixels();
}

ofPixels & ofxRSSDK::getPixelsRef(){
	return videoPixels;
}

//------------------------------------
ofTexture& ofxRSSDK::getTextureReference()
{
	if (!videoTex.bAllocated()){
		ofLog(OF_LOG_WARNING, "ofxRSSDK: Video texture is not allocated");
	}
	return videoTex;
}

//------------------------------------
void ofxRSSDK::setUseTexture(bool bUse){
	bUseTexture = bUse;
}

//----------------------------------------------------------
void ofxRSSDK::draw(float _x, float _y, float _w, float _h) {
	if (bUseTexture && bGrabVideo) {
		videoTex.draw(_x, _y, _w, _h);
	}
}

//----------------------------------------------------------
void ofxRSSDK::draw(float _x, float _y) {
	draw(_x, _y, (float)width, (float)height);
}

//----------------------------------------------------------
void ofxRSSDK::draw(const ofPoint & point) {
	draw(point.x, point.y);
}

//----------------------------------------------------------
void ofxRSSDK::draw(const ofRectangle & rect) {
	draw(rect.x, rect.y, rect.width, rect.height);
}

//----------------------------------------------------------
float ofxRSSDK::getHeight() {
	return (float)height;
}

//---------------------------------------------------------------------------
float ofxRSSDK::getWidth() {
	return (float)width;
}

ofPoint ofxRSSDK::getColorFOV()
{
	PXCPointF32 cFOV = mRealSenseDevice->QueryCaptureManager()->QueryDevice()->QueryColorFieldOfView();
	return ofPoint(cFOV.x, cFOV.y);
}

void ofxRSSDK::DrawLandmark( PXCFaceData::Face* trackedFace )
{
	const PXCFaceData::LandmarksData* landmarkData = trackedFace->QueryLandmarks();
	if( landmarkData == NULL )
	{
		return;
	}

	pxcI32 numPoints = landmarkData->QueryNumPoints();
	if( numPoints != landmarks )
	{
		return;
	}

	landmarkData->QueryPoints(mLandmarkPoints);
	for (int i = 0; i < landmarks; ++i) 
	{
		int x = (int)mLandmarkPoints[i].image.x + (-3);
		int y = (int)mLandmarkPoints[i].image.y + (-3);

		ofCircle( x, y, 2 );
	}
}