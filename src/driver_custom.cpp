
//········································································································//
//········FILE BASED ON "driver_sample.h" FROM Valve Corporation·········································//
//········https://github.com/ValveSoftware/openvr/blob/master/samples/driver_sample/driver_sample.cpp···//
//········Edited/Made by rattata2me····································································//
//····································································································//

#include <thread>
#include <chrono>

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
	}
	
	virtual ~CustomDeviceDriver(){
	}
}