
//········································································································//
//········FILE BASED ON "driver_sample.h" FROM Valve Corporation·········································//
//········https://github.com/ValveSoftware/openvr/blob/master/samples/driver_sample/driver_sample.cpp···//
//········Edited/Made by rattata2me····································································//
//·······························································thx to @r57zone······················//

#include <thread>
#include <chrono>
#include <cmath>

#include <openvr_driver.h>
#include "driverlog.h"


using namespace vr;

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif


inline HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

inline void HmdMatrix_SetIdentity( HmdMatrix34_t *pMatrix )
{
	pMatrix->m[0][0] = 1.f;
	pMatrix->m[0][1] = 0.f;
	pMatrix->m[0][2] = 0.f;
	pMatrix->m[0][3] = 0.f;
	pMatrix->m[1][0] = 0.f;
	pMatrix->m[1][1] = 1.f;
	pMatrix->m[1][2] = 0.f;
	pMatrix->m[1][3] = 0.f;
	pMatrix->m[2][0] = 0.f;
	pMatrix->m[2][1] = 0.f;
	pMatrix->m[2][2] = 1.f;
	pMatrix->m[2][3] = 0.f;
}


//=====================================================//
//······ The World's most Useless WatchDog ···········//
//===================================================//


//Defining class
class Custom_Watchdog : public IVRWatchdogProvider{
	
	
public:
		Custom_Watchdog(){
			m_pWatchdogThread = nullptr;
		}
		
		virtual EVRInitError Init(vr::IVRDriverContext *pDriverContext);
		
		virtual void Cleanup();
	
	
private:
		std::thread *m_pWatchdogThread;
};


Custom_Watchdog watchDog;

bool watchDogExit = false;



//Watchdog is pointless here TODO add implementation to this watchdog
void WatchdogThreadFunction(){
	
	while(!watchDogExit){
		
		std::this_thread::sleep_for(std::chrono::seconds(6));
		
		}
	
	}



EVRInitError Custom_Watchdog::Init(vr::IVRDriverContext *pDriverContext){
	
	//init watchdog driver
	VR_INIT_WATCHDOG_DRIVER_CONTEXT(pDriverContext);
	InitDriverLog(vr::VRDriverLog());
	
	watchDogExit = false;
	
	m_pWatchdogThread = new std::thread(WatchdogThreadFunction);
	
	if(!m_pWatchdogThread){
		DriverLog("Unable to create the thread for some reason...");
		return VRInitError_Driver_Failed;
	}
	
	return VRInitError_None;
}


void Custom_Watchdog::Cleanup(){
	
	watchDogExit = true;
	
	//Clean up thread from memory
	if(m_pWatchdogThread){
		m_pWatchdogThread->join();
		delete m_pWatchdogThread;
		m_pWatchdogThread = nullptr;
	}
	
	//Maybe cleaning the Log is a good idea idk
	CleanupDriverLog();
	
}


//==============================================================================//
//·········CUSTOM Device driver... does that mean something to you?············//
//============================================================================//

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
	CustomDeviceDriver(){
		
		//Avoid getting null
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
		m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
		
		m_flIPD = vr::VRSettings()->GetFloat(k_pch_SteamVR_Section, k_pch_SteamVR_IPD_Float);
		
		
		char buf[1024]; //My lord isn't it 0x400?
		
		vr::VRSettings()->GetString(k_pch_Sample_Section, k_pch_Sample_SerialNumber_String, buf, sizeof(buf));
		m_sSerialNumber = buf;
		
		vr::VRSettings()->GetString(k_pch_Sample_Section, k_pch_Sample_ModelNumber_String, buf, sizeof(buf));
		m_sModelNumber = buf;
		
		m_nWindowX = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowX_Int32);
		m_nWindowY = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowY_Int32);
		m_nWindowWidth = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowWidth_Int32);
		m_nWindowHeight = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowHeight_Int32);
		m_nRenderWidth = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_RenderWidth_Int32);
		m_nRenderHeight = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_RenderHeight_Int32);
		m_flSecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_SecondsFromVsyncToPhotons_Float);
		m_flDisplayFrequency = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_DisplayFrequency_Float);

		m_fDistortionK1 = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_DistortionK1_Float);
		m_fDistortionK2 = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_DistortionK2_Float);
		m_fZoomWidth = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_ZoomWidth_Float);
		m_fZoomHeight = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_ZoomHeight_Float);
		m_bDebugMode = vr::VRSettings()->GetBool(k_pch_Sample_Section, k_pch_Sample_DebugMode_Bool);
		
		DriverLog("Custom Driver Succesfully set");
	}
	
	virtual ~CustomDeviceDriver(){
	}
	
	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId){
	  
	m_unObjectId = unObjectId;
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);
	
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserIpdMeters_Float, m_flIPD);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserHeadToEyeDepthMeters_Float, 0.f);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_DisplayFrequency_Float, m_flDisplayFrequency);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_SecondsFromVsyncToPhotons_Float, m_flSecondsFromVsyncToPhotons);
	
	vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);
	
	vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false);
	
	return VRInitError_None;
	
	}
	virtual void Deactivate(){
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	  
	}
	
	//TODO Do something
	virtual void EnterStandby()
	{}
	
	void *GetComponent(const char *pchComponentNameAndVersion){
	  if ( !_stricmp( pchComponentNameAndVersion, vr::IVRDisplayComponent_Version ) )
		
		{
			return (vr::IVRDisplayComponent*)this;
		}
		
		return NULL;
	}
	
	virtual void PowerOff(){}
	
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) 
	{
		if(unResponseBufferSize >= 1)
			pchResponseBuffer[0] = 0;
	  
	}
	
	virtual void GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) 
	{
		*pnX = m_nWindowX;
		*pnY = m_nWindowY;
		*pnWidth = m_nWindowWidth;
		*pnHeight = m_nWindowHeight;
	}
	
	virtual bool IsDisplayOnDesktop() 
	{
		return true;
	}
	
	virtual bool IsDisplayRealDisplay() 
	{
		return false;
	}
	
	virtual void GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight ) 
	{
		*pnWidth = m_nRenderWidth;
		*pnHeight = m_nRenderHeight;
	}
	
	virtual void GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight) 
	{
		*pnY = 0;
		*pnWidth = m_nWindowWidth / 2;
		*pnHeight = m_nWindowHeight;
	
		if ( eEye == Eye_Left )
		{
			*pnX = 0;
		}
		else
		{
			*pnX = m_nWindowWidth / 2;
		}
	}
	
	virtual void GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom) 
	{
		*pfLeft = -1.0;
		*pfRight = 1.0;
		*pfTop = -1.0;
		*pfBottom = 1.0;  
	}
	
	//Taken from https://github.com/HelenXR/openvr_survivor/blob/master/src/head_mount_display_device.cc
	virtual DistortionCoordinates_t ComputeDistortion( EVREye eEye, float fU, float fV ) 
	{
		DistortionCoordinates_t coordinates;

		
		float hX;
		float hY;
		double rr;
		double r2;
		double theta;

		rr = sqrt((fU - 0.5f)*(fU - 0.5f) + (fV - 0.5f)*(fV - 0.5f));
		r2 = rr * (1 + m_fDistortionK1*(rr*rr) + m_fDistortionK2*(rr*rr*rr*rr));
		theta = atan2(fU - 0.5f, fV - 0.5f);
		hX = sin(theta)*r2*m_fZoomWidth;
		hY = cos(theta)*r2*m_fZoomHeight;

		coordinates.rfBlue[0] = hX + 0.5f;
		coordinates.rfBlue[1] = hY + 0.5f;
		coordinates.rfGreen[0] = hX + 0.5f;
		coordinates.rfGreen[1] = hY + 0.5f;
		coordinates.rfRed[0] = hX + 0.5f;
		coordinates.rfRed[1] = hY + 0.5f;


		return coordinates;
	}
	
	virtual DriverPose_t GetPose() 
	{
		DriverPose_t pose = { 0 };
		pose.poseIsValid = true;
		pose.result = TrackingResult_Running_OK;
		pose.deviceIsConnected = true;

		pose.qWorldFromDriverRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
		pose.qDriverFromHeadRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
		

		return pose;
	}

	void RunFrame(){
		//TODO finish this
		// In a real driver, this should happen from some pose tracking thread.
		// The RunFrame interval is unspecified and can be very irregular if some other
		// driver blocks it for some periodic task.
		if ( m_unObjectId != vr::k_unTrackedDeviceIndexInvalid )
		{
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated( m_unObjectId, GetPose(), sizeof( DriverPose_t ) );
		}
	}
	
	std::string GetSerialNumber() const { return m_sSerialNumber; }
	
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
};



//==============================================================================//
//·········CUSTOM Server driver················································//
//============================================================================//

class CustomServerDriver: public IServerTrackedDeviceProvider{
 
public:
	CustomServerDriver()
		: m_pNullHmdLatest(NULL)
		, m_bEnableNullDriver(false){
	  
	}
	virtual EVRInitError Init( vr::IVRDriverContext *pDriverContext ) ;
	virtual void Cleanup();
	virtual const char * const *GetInterfaceVersions(){ return vr::k_InterfaceVersions; }
	virtual void RunFrame();
	virtual bool ShouldBlockStandbyMode(){ return false; }
	virtual void EnterStandby(){}
	virtual void LeaveStandby(){} 
	
	
private:
	CustomDeviceDriver *m_pNullHmdLatest;
	bool m_bEnableNullDriver;
};

CustomServerDriver customDriverNull;


EVRInitError CustomServerDriver::Init( vr::IVRDriverContext *pDriverContext )
{
	VR_INIT_SERVER_DRIVER_CONTEXT( pDriverContext );
	InitDriverLog(vr::VRDriverLog());

	m_pNullHmdLatest = new CustomDeviceDriver();
	vr::VRServerDriverHost()->TrackedDeviceAdded( m_pNullHmdLatest->GetSerialNumber().c_str(), vr::TrackedDeviceClass_HMD, m_pNullHmdLatest );
	return VRInitError_None;
}

void CustomServerDriver::Cleanup() 
{
	delete m_pNullHmdLatest;
	m_pNullHmdLatest = NULL;
}


void CustomServerDriver::RunFrame()
{
	if ( m_pNullHmdLatest )
	{
		m_pNullHmdLatest->RunFrame();
	}
}


//Driver function
HMD_DLL_EXPORT void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
	if( 0 == strcmp( IServerTrackedDeviceProvider_Version, pInterfaceName ) )
	{
		return &customDriverNull;
	}
	if( 0 == strcmp( IVRWatchdogProvider_Version, pInterfaceName ) )
	{
		return &watchDog;
	}

	if( pReturnCode )
		*pReturnCode = VRInitError_Init_InterfaceNotFound;

	return NULL;
}