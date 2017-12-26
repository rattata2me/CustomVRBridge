#include <thread>
#include <chrono>
#include <cmath>

#include <openvr_driver.h>
#include "driverlog.h"


using namespace vr;


// Keys for use with the "PERFECT" settings API
static const char * const k_pch_Sample_Section = "driver_custom"; // Maybe if I repeat it 1000 times it sounds better...
static const char * const k_pch_Sample_SerialNumber_String = "serialNumber";
static const char * const k_pch_Sample_ModelNumber_String = "modelNumber";
static const char * const k_pch_Sample_WindowX_Int32 = "windowX";
static const char * const k_pch_Sample_WindowY_Int32 = "windowY";
static const char * const k_pch_Sample_WindowWidth_Int32 = "windowWidth";
static const char * const k_pch_Sample_WindowHeight_Int32 = "windowHeight";
static const char * const k_pch_Sample_RenderWidth_Int32 = "renderWidth";
static const char * const k_pch_Sample_RenderHeight_Int32 = "renderHeight";
static const char * const k_pch_Sample_SecondsFromVsyncToPhotons_Float = "secondsFromVsyncToPhotons";
static const char * const k_pch_Sample_DisplayFrequency_Float = "displayFrequency";
static const char * const k_pch_Sample_DistortionK1_Float = "DistortionK1";
static const char * const k_pch_Sample_DistortionK2_Float = "DistortionK2";
static const char * const k_pch_Sample_ZoomWidth_Float = "ZoomWidth";
static const char * const k_pch_Sample_ZoomHeight_Float = "ZoomHeight";
static const char * const k_pch_Sample_DebugMode_Bool = "DebugMode";


class CustomDeviceDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent{

public:
	
	//Construction method
	CustomDeviceDriver();
	
	virtual ~CustomDeviceDriver();
	
	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId);
	
	virtual void Deactivate();
	
	//TODO Do something
	virtual void EnterStandby();
	
	
	void *GetComponent(const char *pchComponentNameAndVersion);
	
	virtual void PowerOff(){}
	
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);
	
	virtual void GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight );
	
	virtual bool IsDisplayOnDesktop();
	
	virtual bool IsDisplayRealDisplay();
	
	virtual void GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight );
	
	virtual void GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight);
	
	virtual void GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom);
	
	//Taken from https://github.com/HelenXR/openvr_survivor/blob/master/src/head_mount_display_device.cc
	virtual DistortionCoordinates_t ComputeDistortion( EVREye eEye, float fU, float fV );
	
	virtual DriverPose_t GetPose();

	void RunFrame();
	
	std::string GetSerialNumber() const;
	
	
private:
	
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	int32_t m_nWindowX;
	int32_t m_nWindowY;
	int32_t m_nWindowWidth;
	int32_t m_nWindowHeight;
	int32_t m_nRenderWidth;
	int32_t m_nRenderHeight;
	float m_flSecondsFromVsyncToPhotons;
	float m_flDisplayFrequency;
	float m_flIPD;
	float m_fDistortionK1;
	float m_fDistortionK2;
	float m_fZoomWidth;
	float m_fZoomHeight;
	bool m_bDebugMode;
	float h1;
	
};