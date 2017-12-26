
#include <thread>
#include <chrono>
#include <cmath>

#include <openvr_driver.h>
#include "driverlog.h"

#include "CustomDeviceDriver.h"

using namespace vr;

class Custom_Watchdog : public IVRWatchdogProvider{
	
	
public:
		Custom_Watchdog();
		
		virtual EVRInitError Init(vr::IVRDriverContext *pDriverContext);
		
		virtual void Cleanup();
	
	
private:
		std::thread *m_pWatchdogThread;
};


class CustomServerDriver: public IServerTrackedDeviceProvider{
 
public:
	CustomServerDriver();
	virtual EVRInitError Init( vr::IVRDriverContext *pDriverContext ) ;
	virtual void Cleanup();
	virtual const char * const *GetInterfaceVersions();
	virtual void RunFrame();
	virtual bool ShouldBlockStandbyMode();
	virtual void EnterStandby();
	virtual void LeaveStandby(); 
	
	
private:
	CustomDeviceDriver *m_pNullHmdLatest;
	bool m_bEnableNullDriver;
};