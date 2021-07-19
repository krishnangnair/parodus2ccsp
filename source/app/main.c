/**
 * @file main.c
 *
 * @description This file defines WebPA's main function
 *
 * Copyright (c) 2015  Comcast
 */
#include "stdlib.h"
#include "signal.h"
#include "webpa_adapter.h"
#include "libpd.h"
#include "webpa_rbus.h"
#include "rbus.h"
#ifdef FEATURE_SUPPORT_WEBCONFIG
#include <curl/curl.h>
#endif
#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif

//struct _rbusHandle_t;
///  @brief     An RBus handle which identifies an opened component
//typedef struct _rbusHandle_t* rbusHandle_t;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
#ifndef INCLUDE_BREAKPAD
static void sig_handler(int sig);
#endif

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
#define     TotalParams   4
rbusHandle_t rbus_handle;
char const*     paramNames[TotalParams] = {
    "Device.DeviceInfo.ModelName",
    "Device.DeviceInfo.X_CISCO_COM_FirmwareName",
    "Device.DeviceInfo.ProductClass",
    "Device.DeviceInfo.SerialNumber"   
};

rbusHandle_t get_global_rbus_handle(void)
{
     return rbus_handle;
}

int main()
{
        //int ret = -1;
        int rc = 0;
        int value = 0; 
        int count =4;
#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();
#else
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	signal(SIGSEGV, sig_handler);
	signal(SIGBUS, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGFPE, sig_handler);
	signal(SIGILL, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGALRM, sig_handler);
#endif
	const char *pComponentName = WEBPA_COMPONENT_NAME;
	WalInfo("********** Starting component: %s **********\n ", pComponentName); 
        drop_root_privilege();
	/* Backend Manager for Webpa Creation and Initilization 
    CosaWebpaBEManagerCreate( );*/
	WalInfo("B4 msgBusInit\n");
	if(isRbusEnabled())
	{
		WalInfo("daemonize Webpa\n");
		daemonize();
		WalInfo("webpaRbusInit\n");
		webpaRbusInit(pComponentName);
		system("touch /tmp/webpa_initialized");		               
	}
	else
	{
		msgBusInit(pComponentName); //ccsp init
	}
	WalInfo("After BusInit\n");
	//ret = waitForOperationalReadyCondition();
	libpd_client_mgr();
	WalInfo("Syncing backend manager with DB....\n");
	//CosaWebpaSyncDB();
	WalInfo("Webpa banckend manager is in sync with DB\n");
	rbusHandle_t webcfg_rbus_handle = get_global_rbus_handle();
        for(count=0; count < TotalParams; count++)
    	{
         WalInfo("calling rbus get for [%s]\n", paramNames[count]);
         rc = rbus_get(webcfg_rbus_handle, paramNames[count], &value);
         WalInfo("rbus_get succeded  [%s] with  [%d]\n", paramNames[count], rc);
         if(rc != RBUS_ERROR_SUCCESS)
        {
            WalInfo("rbus_get failed for [%s] with error [%d]\n", paramNames[count], rc);
            continue;
        }
	WalInfo("rbus_get succeded for [%s] with  [%s]\n", paramNames[count], value);
        switch(count)
        {
            case 0:
                WalInfo("Model name = [%s]\n", rbusValue_GetString(value, NULL));
                break;
            case 1:
                WalInfo("The value is = [%s]\n", rbusValue_GetString(value, NULL));
                break;
            case 4:
                WalInfo("The value is = [%s]\n", rbusValue_GetString(value, NULL));
                break;
            case 3:
                WalInfo("The value is = [%s]\n", rbusValue_GetString(value, NULL));
                break;
        }
}
	//initComponentCaching(ret);
	// Initialize Apply WiFi Settings handler
	//initApplyWiFiSettings();
	//initNotifyTask(ret);
#ifdef FEATURE_SUPPORT_WEBCONFIG
	curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
	parodus_receive_wait();
#ifdef FEATURE_SUPPORT_WEBCONFIG
curl_global_cleanup();
#endif
	WalInfo("Exiting webpa main thread!!\n");
	return 1;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
#ifndef INCLUDE_BREAKPAD
static void sig_handler(int sig)
{

	if ( sig == SIGINT ) 
	{
		signal(SIGINT, sig_handler); /* reset it to this function */
		WalInfo("SIGINT received!\n");
	}
	else if ( sig == SIGUSR1 ) 
	{
		signal(SIGUSR1, sig_handler); /* reset it to this function */
		WalInfo("SIGUSR1 received!\n");
	}
	else if ( sig == SIGUSR2 ) 
	{
		WalInfo("SIGUSR2 received!\n");
	}
	else if ( sig == SIGCHLD ) 
	{
		signal(SIGCHLD, sig_handler); /* reset it to this function */
		WalInfo("SIGHLD received!\n");
	}
	else if ( sig == SIGPIPE ) 
	{
		signal(SIGPIPE, sig_handler); /* reset it to this function */
		WalInfo("SIGPIPE received!\n");
	}
	else if ( sig == SIGALRM ) 
	{
		signal(SIGALRM, sig_handler); /* reset it to this function */
		WalInfo("SIGALRM received!\n");
	}
	else 
	{
		WalInfo("Signal %d received!\n", sig);
		OnboardLog("Signal %d received!\n", sig);
		exit(0);
	}
	
}
#endif
