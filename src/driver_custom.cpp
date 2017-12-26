
//········································································································//
//········FILE BASED ON "driver_sample.h" FROM Valve Corporation·········································//
//········https://github.com/ValveSoftware/openvr/blob/master/samples/driver_sample/driver_sample.cpp···//
//········Edited/Made by rattata2me····································································//
//·······························································thx to @r57zone······················//

#include "driver_custom.h"



#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif





//=====================================================//
//······ The World's most Useless WatchDog ···········//
//===================================================//

Custom_Watchdog watchDog;

bool watchDogExit = false;


Custom_Watchdog::Custom_Watchdog(){
	
			m_pWatchdogThread = nullptr;
			
}


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
//·········CUSTOM Server driver········in a nutshell···························//
//============================================================================//

CustomServerDriver customDriverNull;


CustomServerDriver::CustomServerDriver()

		: m_pNullHmdLatest(NULL)
		, m_bEnableNullDriver(false){
	  
}

const char * const *CustomServerDriver::GetInterfaceVersions(){ 
	
	return vr::k_InterfaceVersions; 
	
}

bool CustomServerDriver::ShouldBlockStandbyMode(){ 
	
	return false; 
	
}

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

void CustomServerDriver::EnterStandby(){
	
}

void CustomServerDriver::LeaveStandby(){
	
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