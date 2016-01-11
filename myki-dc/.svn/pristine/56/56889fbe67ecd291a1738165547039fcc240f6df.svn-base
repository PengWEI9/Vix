/***************************************************************************
 * Copyright (c) 2014 Vix Technology. All rights reserved
 *
 * Module name   : CCEEventProcessingThread.cpp
 * Module type   : 
 * Compiler(s)   : ANSI C
 * Environment(s): LINUX
 *
 * Description:
 *
 *       
 *
 * Contents:
 *
     
 *

 * Version   Who      Date       Description
   1.0       EAH      14/07/2014 Created
   1.01      ANT      24/02/2015 MTU-4: Implemented AVL notification
 *
 * $Id:  $
 * $HeadURL: $
 *.........................................................................*/
 
#include "dc.h"

static uint32_t pollTimer   = 0;


extern "C" void* CCEEventProcessingThread(void* arg)
{
	CceRv_t Rv;
    CceEcConn_t     EcConn=0;
    CceEc_t Ec = 0;    
    CceFilter_t     Filter;
    CceEvent_t      Event;
    int             Finished = FALSE;
   
    CceEvFilterPass(&Filter, CCE_EV_ALL); 
    // Exclude the following event types from the event loop
    CceEvFilterBlock(&Filter, CCE_EV_SYS_UDDRAIN); 
    CceEvFilterBlock(&Filter, CCE_EV_SYS_ARDRAIN);
    CceEvFilterBlock(&Filter, CCE_EV_SYS_TIMESYNC);

    while (!Finished)
    {
        if(EcConn==0)
        {
            Ec= 0; // Reopen event channel
            Rv = CceEvConnect(&EcConn, NULL, 0);
            if (CceMsFail(Rv))
            {
                CsErrx("Could not open RPC to event server: %s", CceMsErrStr(Rv, NULL, 0));
                CsSleep(1, 0);
                continue;
            }    
        }
        
        if(Ec==0)
        {
            Rv = CceEvChanOpen(&Ec, EcConn, &Filter);
            if (CceMsFail(Rv))
            {
                CsErrx("Could not open event channel: %s", CceMsErrStr(Rv, NULL, 0));
                Ec= 0; // Reopen event channel
                // Try to reconnect again.
                CceEvDisconnect(EcConn);
                EcConn = 0;
                
                CsSleep(1, 0); // Wait one second
                continue;
            }       
            else
            {
				requestStatusToAll();
            }            
        }

        memset(&Event, 0, sizeof(CceEvent_t));
        Rv = CceEvRead(Ec, &Event, -1);  // TODO define timeout period (to allow for clean exit of thread)
        if (CceMsFail(Rv))
        {
            if((Rv&CCE_ERR_APITIMEOUT)==0) // Not a timeout failure
            {
                CsErrx("Couldn't read event channel: %s", CceMsErrStr(Rv, NULL, 0));
                
                CceEvChanClose(Ec);
                Ec = 0;
            }
        }
        else 
        {
            // Process the received event.
            switch(Event.EvType)
            {
                case CCE_EV_SYS_VAROP:
                    CsDebug(3, (3, "Event: CCE_EV_SYS_VAROP %s op: %d id: %d value: %u", Event.DevAddr.LogicalNm, 
                                    Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_op,
                                    (unsigned int)Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_id,
                                    (unsigned int)Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_value));   
                    if(Event.DevData.Type==CCE_EVDET_VAROP)
                    {
                        if(Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_op == VAROP_SET || Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_op == VAROP_GET)
                        {
                            switch(Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_id)
                            {
                             case VO_DEVICE_STATUS:
                                setDeviceStatus(Event.DevAddr.LogicalNm, Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_value, "", true, false);
                                break;
                             case VO_READER_ENABLED:
                                setDeviceOpStatus(Event.DevAddr.LogicalNm, Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_value, "", true, false);
                                break;
                             case VO_READER_MODE:
                                setDeviceReaderMode(Event.DevAddr.LogicalNm, Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_value, "Mode:", true, false);
                                break;
                             case VO_READER_ESN:
                                setDeviceReaderESN(Event.DevAddr.LogicalNm, Event.DevData.CceEvDetDevData_t_u.VarOp.Rsp.var_value, "Reader ESN:", true, false);
                                break;
                            }
                        }        
                    }
                    break;
                case CCE_EV_SYS_ERROR:
                    CsDebug(2, (2, "Event: CCE_EV_SYS_ERROR %s Error: %d Desc: %s", Event.DevAddr.LogicalNm, Event.CceErrVal, Event.CceErrDesc));       
                    break;
                case CCE_EV_SYS_CSCKEYSETXFER:
                    CsDebug(4, (4, "Event: CCE_EV_SYS_CSCKEYSETXFER %s", Event.DevAddr.LogicalNm));   
                    break;
                case CCE_EV_SYS_CSCEODXFER:
                    CsDebug(4, (4, "Event: CCE_EV_SYS_CSCEODXFER %s", Event.DevAddr.LogicalNm));   
                    break;
                case CCE_EV_DEV_EVENTNOTIFY:
                    CsDebug(4, (4, "Event: CCE_EV_DEV_EVENTNOTIFY %s", Event.DevAddr.LogicalNm));   
                    if(Event.DevData.Type==CCE_EVDET_DEVEVENT)
                    {
                        CsDebug(2, (2, "Event: CCE_EV_DEV_EVENTNOTIFY %d  - %s", Event.DevData.CceEvDetDevData_t_u.devEvent.Event.Event, Event.DevData.CceEvDetDevData_t_u.devEvent.Event.jsonstring));  

                        // Create an event on the queue.
                        IPC_CCEEvent_t m = { {IPC_CCE_EVENT, (TaskID_t)DC_CMD_TASK }};
                        strncpy(m.jsonstring, (char*) Event.DevData.CceEvDetDevData_t_u.devEvent.Event.jsonstring, MAX_JSONSTRING_SIZE);
						strncpy(m.source, Event.DevAddr.LogicalNm, MAX_HOSTNAME_SIZE);
                        IPCSend(appQueueId, &m, sizeof m);  
                    }
                    break;
                case CCE_EV_SYS_DEVLOCATED:

                    if(Event.DevData.Type==CCE_EVDET_LOCATED)
                    {
                        CsDebug(2, (2, "Event: CCE_EV_SYS_DEVLOCATED %s DaUsed: %u PhysicalId: %u IP: %s Channel: %s Arming %u", Event.DevAddr.LogicalNm, 
                                        Event.DevAddr.DaUsed,
                                        Event.DevAddr.PhysicalId,
                                        Event.DevAddr.IpNm,
                                        Event.DevData.CceEvDetDevData_t_u.Locate.Channel,
                                        Event.DevData.CceEvDetDevData_t_u.Locate.Arming));                          
                                               
                    }                   
                    break;
                case CCE_EV_SYS_DCROPEN:
                    CsDebug(2, (2, "Event: CCE_EV_SYS_DCROPEN %s", Event.DevAddr.LogicalNm)); 
                    break;
                case CCE_EV_SYS_DCRCLOSE:
                    CsDebug(2, (2, "Event: CCE_EV_SYS_DCRCLOSE %s", Event.DevAddr.LogicalNm)); 
                    break;                    
                case CCE_EV_SYS_STATUS:
                    CsDebug(3, (3, "Event: CCE_EV_SYS_STATUS %s", Event.DevAddr.LogicalNm));    
					if(Event.DevData.Type == CCE_EVDET_STATUS)
					{
                        // TODO add setDeviceStatus.
//						setGateStatus(Event.DevAddr.LogicalNm, &Event.DevData.CceEvDetDevData_t_u.Status);
                        //setDeviceStatus(Event.DevAddr.LogicalNm, DEVICE_STATUS_ERROR, "", DEVICE_OPSTATUS_OUT_OF_SERVICE); 
					}						
                    break;
                case CCE_EV_SYS_CONN:
                    CsDebug(1, (1, "Event: CCE_EV_SYS_CONN %s", Event.DevAddr.LogicalNm));       
                    // Request the gate state and current ticket usage indicator config
                    sendVarop(Event.DevAddr.LogicalNm, VAROP_GET, VO_DEVICE_STATUS,     0);
                    sendVarop(Event.DevAddr.LogicalNm, VAROP_GET, VO_READER_ENABLED,     0);
                    sendVarop(Event.DevAddr.LogicalNm, VAROP_GET, VO_READER_MODE,     0);
                    sendVarop(Event.DevAddr.LogicalNm, VAROP_GET, VO_READER_ESN,     0);
                     
                    // Request varops for that device.

                    break;                       
                case CCE_EV_SYS_DISCONN:
                    CsDebug(1, (1, "Event: CCE_EV_SYS_DISCONN %s", Event.DevAddr.LogicalNm));       
                    {
                    // Update the device state to disconnected
                    bool changed = setDeviceStatus(Event.DevAddr.LogicalNm, DEVICE_STATUS_ERROR, "Disconnected", false, false); 
                    setDeviceOpStatus(Event.DevAddr.LogicalNm, DEVICE_OPSTATUS_DISCONNECTED, "Disconnected", changed, false);
                    }
                    break;    
                case CCE_EV_SYS_BOOTP:
                case CCE_EV_SYS_TFTP:
                case CCE_EV_SYS_START:
                case CCE_EV_SYS_END:
                case CCE_EV_SYS_DSM:                 
                    // Ignore
                    break;
                default:
                    CsDebug(1, (1, "Event: %d", Event.EvType));                
                    break;
            }
        }
    }
    CceEvChanClose(Ec);
    CceEvDisconnect(EcConn);
	
	return 0;
}


std::string findLogicalName(std::string terminalId)
{
    std::string returnValue = terminalId;
    
    // Perform any required translations here.
  
    return returnValue;
}


CceRv_t getStatus(const char* logicalId, CceDcrStatus_t* pStatus)
{
	CceCc_t channel = 0;
    CceRv_t Rv = CceCtlOpen(&channel, NULL, NULL);
    
    std::string logicalName = findLogicalName(logicalId);
        
    if (CceMsOk(Rv))
    {
		CsDebug(4, (4, "CCE Opened channel %d", channel));
        CceDevAddr_t deviceAddress;
        
        // Construct a device address that sends a varop to this logical terminal Id.
        CceMsLogicalNm(&deviceAddress, logicalName.c_str());
        
        Rv = CceintDcrStatusGet (channel, &deviceAddress, pStatus);
        if (CceMsOk(Rv))
        {
            //CmdLogWrite( LOG_ALWAYS, "S+ Device has a status of 0x%x\n", Status);
            CsDebug(1, (1, "S+ Device has a status of 0x%x\n", *pStatus));
            
        }
		CceCtlClose(&channel);

    }    
    else
    {
        CsErrx("CceCtlOpen() failed %s", CceMsErrStr(Rv, NULL, 0));
    }
    
    return Rv;
}

/*==========================================================================*
**
**  cceSendLocation
**
**  Description     :
**      Sends VO_CURRENT_LOCATION, current location varop, to FPDm.
**
**  Parameters      :
**      routeId         [I]     route id    [0-65535]
**      stopId          [I]     stop id     [0-255]
**      isAvailable     [I]     TRUE if location available; FALSE otherwise
**
**  Returns         :
**      0                       successful
**      Else                    failed
**
**  Notes           :
**
**
**==========================================================================*/

int cceSendLocation( int32_t routeId, int32_t stopId, int32_t isAvailable )
{
    int32_t         nLocation   = 0;
    CceRv_t         Rv;

    CsDebug( 4, ( 4, "CCE cceSendLocation %d %d %s",
        routeId, stopId, isAvailable != FALSE ? "Available" : "Unavailable" ) );

    nLocation   = (   routeId              & 0x0000ffff ) |
                  ( ( stopId << 16 )       & 0x00ff0000 ) |
                  ( ( isAvailable != FALSE ? 0x80000000 : 0x00000000 ) );
    Rv  = sendVaropToAll( VO_CURRENT_LOCATION, (unsigned long)nLocation );
  

    return  CceMsOk( Rv ) ? 0 : -1;
}   /*  cceSendLocation( ) */


CceRv_t sendVaropToAll(  unsigned long varId,  unsigned long varValue)
{
	CceCc_t channel = 0;
    CceRv_t Rv = CceCtlOpen(&channel, NULL, NULL);
    
    unsigned long varOp = VAROP_SET;
      
    if (CceMsOk(Rv))
    {
		CsDebug(4, (4, "CCE Opened channel %d", channel));
        CceDevAddr_t deviceAddress;
        // Construct a device address that sends a varop request to all devices
        CceMsAllDevices(&deviceAddress);
                
        CsDebug(3, (3, "sendVaropToAll CCE VAROP op:%u id:%u val:%u", varOp, varId, varValue));
        Rv = CceDevVarop( channel,  &deviceAddress, varOp, varId, varValue);
		if (!CceMsOk(Rv))
		{
            if(Rv!=0x8000000D ) // Device not found
                CsErrx("ERROR: sendVaropToAll() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0));
            else
                CsDebug(4, (4, "CceDevVarop() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0)));             
		}

		CceCtlClose(&channel);
    }    
    else
    {
        CsErrx("CceCtlOpen() failed %s", CceMsErrStr(Rv, NULL, 0));
    }
    return Rv;
}

CceRv_t sendVarop(const char* logicalId,  unsigned long varOp, unsigned long varId, unsigned long varValue)
{
	CceCc_t channel = 0;
    CceRv_t Rv = CceCtlOpen(&channel, NULL, NULL);
    
    std::string logicalName = findLogicalName(logicalId);
    
    if (CceMsOk(Rv))
    {
		CsDebug(4, (4, "CCE Opened channel %d", channel));
        CceDevAddr_t deviceAddress;
        
        // Construct a device address that sends a varop to this physical terminal Id.
        CceMsLogicalNm(&deviceAddress, logicalName.c_str());
        
        CsDebug(2, (2, "sendVarop CCE VAROP op:%d id:%d val:%d", varOp, varId, varValue));
        Rv = CceDevVarop( channel,  &deviceAddress, varOp, varId, varValue);
		if (!CceMsOk(Rv))
		{
            if(Rv!=0x8000000D ) // Device not found
                CsErrx("sendVarop ERROR: CceDevVarop() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0));
            else
                CsDebug(4, (4, "CceDevVarop() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0))); 
		}

		CceCtlClose(&channel);
    }    
    else
    {
        CsErrx("CceCtlOpen() failed %s", CceMsErrStr(Rv, NULL, 0));
    }
    return Rv;
}

CceRv_t requestStatusToAll()
{
	CceCc_t channel = 0;
    CceRv_t Rv = CceCtlOpen(&channel, NULL, NULL);

    if (CceMsOk(Rv))
    {
		CsDebug(6, (6, "CCE Opened channel %d", channel));
        CceDevAddr_t deviceAddress;
        // Construct a device address that sends a varop request to all devices
        CceMsAllDevices(&deviceAddress);
        
        CsDebug(6, (6, "requestStatusToAll"));
		Rv = CceDevStatusReq(channel, &deviceAddress);

		if (!CceMsOk(Rv))
		{
			CsErrx("CceDevStatusReq() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0));
		}

		CceCtlClose(&channel);
    }    
    else
    {
        CsErrx("CceDevStatusReq() failed %s", CceMsErrStr(Rv, NULL, 0));
    }
    
    return Rv;
}

CceRv_t requestVaropToAll(unsigned long varId)
{
	CceCc_t channel = 0;
    CceRv_t Rv = CceCtlOpen(&channel, NULL, NULL);
    unsigned long varOp = VAROP_GET;
    unsigned long varValue  = 0;
    
    if (CceMsOk(Rv))
    {
		CsDebug(6, (6, "CCE Opened channel %d", channel));
        CceDevAddr_t deviceAddress;
        // Construct a device address that sends a varop request to all devices
        CceMsAllDevices(&deviceAddress);
        
        CsDebug(6, (6, "requestVaropToAll CCE VAROP op:%d id:%d val:%d", varOp, varId, varValue));
        Rv = CceDevVarop( channel,  &deviceAddress, varOp, varId, varValue);

		if (!CceMsOk(Rv))
		{
            if(Rv!=0x8000000D ) // Device not found
                CsErrx("requestVaropToAll() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0));
            else
                CsDebug(4, (4, "CceDevVarop() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0))); 
		}

		CceCtlClose(&channel);
    }    
    else
    {
        CsErrx("CceCtlOpen() failed %s", CceMsErrStr(Rv, NULL, 0));
    }
    return Rv;
}

int openCCE() // opens a connection and reports version and connected devices
{
	CceCc_t channel = 0;
    CceRv_t Rv = CceCtlOpen(&channel, NULL, NULL);
    int returnValue = -1;

    if (CceMsOk(Rv))
    {
		unsigned long version =0;
		char verString[256];
		verString[0]='\0';
 
		CsDebug(1, (1, "CCE Opened channel %d", channel));
		  
		Rv = CceCtlVersion(channel, &version, verString, 256);

		if (CceMsOk(Rv))
		{
			CsDebug(1, (1, "CCE Version %d %s", version, verString));


            CceDevList_t	DevList;
            unsigned int i;
            Rv = CceCtlDevList(channel, &DevList);

            if ( CceMsOk(Rv) )
            {
                CsDebug(1, (1, "S+ The CCE knows of %d device%s"
                         "S+ %-16s%-16s%-8s%-17s",
                         DevList.NumDevices,
                         DevList.NumDevices==1 ? "" : "s",
                         "Logical",
                         "Group",
                         "PhysId",
                         "IP"));

                for (i = 0; i < DevList.NumDevices; i++)
                {
                    CsDebug(1, (1, "S+ %-16s%-16s%06x  %-17s",
                            DevList.List[i].LogicalName,
                            DevList.List[i].GroupName,
                            DevList.List[i].PhysicalId,
                            DevList.List[i].IpAddress));
                }
            }
            else
            {
                CsErrx("CceCtlDevList() failed %u", Rv);
            }
            
            returnValue = 0; // success
		}
		else
		{
			CsErrx("CceCtlVersion() failed %u", Rv);
		}
		CceCtlClose(&channel);
        
        
    }    
    else
    {
        CsErrx("CceCtlOpen() failed %u : %s", Rv, CceMsErrStr(Rv, NULL, 0));
    }
    
	return returnValue;
}

int LoadDeviceList()
{
	CceDevTbl_t *pCceDevTbl = NULL;
    int groupId =0;
	PilError_et ret=ePILE_NOERR;
	if ( (ret = PilShmConnect((void **)&pCceDevTbl, CCEDEVTBLSHM, PIL_READ)) != ePILE_NOERR )
	{
		CsErrx("Error connecting to CCEDEVTBLSHM: %s", PilErrorStr(ret));
		return -1;
	}
	else
	{
        std::map<std::string, Json::Value> deviceMap;
        unsigned int count =0;
        
		CsDebug(1, (1, "Device list %d", pCceDevTbl->Size));
		for(int i = 0; i < pCceDevTbl->Size; i++ )
		{
			CceDevTblEntry_t *pEntry = &pCceDevTbl->Entry[i];
			CsDebug(1, (1, "Device %d: Name: %s, Active: %d, IP: %s, Mode %d, group %s, type %d", i, pEntry->Name, pEntry->Active, pEntry->IpAddress, pEntry->Mode, pEntry->GroupNm, pEntry->AppType)); 

            std::string groupNm = "Default";
            if(pEntry->GroupNm[0][0]!='\0')
            {
               groupNm = pEntry->GroupNm[0];
            }   

            if(deviceMap.count(groupNm)==0)
            {               
                CsDebug(2, (2, "Add array %s", groupNm.c_str()));
                deviceMap[groupNm]          = Json::Value();
                deviceMap[groupNm]["id"]    = ++groupId;
                deviceMap[groupNm]["title"] = groupNm;
            }
            
           // char *id = &pEntry->GroupNm[1][3];

            Json::Value device;
            
            std::string id  = pEntry->Name;
            std::string terminalid  = id;
            
            if(terminalid.length()>8)
            {
                terminalid = terminalid.substr(terminalid.length()-8, 8);
            }
            
            device["id"]                      = id;
            device["terminalid"]              = terminalid;
            device["status"]                  = getDeviceStatusName(DEVICE_STATUS_ERROR);
            device["opStatus"]                = "unknown";
            device["mode"]                    = "";
            
            device["log"]                     = Json::Value(Json::arrayValue);
            device["number"] = "E"; // E for error
            device["ESN"] = "unknown";
           
            switch(pEntry->AppType)
            {
                case eDT_MERG:
                    device["type"]                    = "DC";
                    device["deviceType"]              = (strcmp(g_terminalType, "BDC") == 0) ? "BDC" : "TDC";
                    // If this is the same device as the one that hosts the dc application then it is a primary.
                    device["number"]                  = strncmp(g_ipAddress, pEntry->IpAddress, CSFINI_MAXVALUE)==0 ? "P" : "S";

                    if(strncmp(g_ipAddress, pEntry->IpAddress, CSFINI_MAXVALUE)==0)
                    {
                        //Update primary with the known ESN.
                        const size_t BUF_SIZE = 21;
                        char buf[BUF_SIZE + 1] = { 0 };
                        snprintf(buf, BUF_SIZE, "%u", getESN());
                        device["ESN"] = buf;
                    }
                    break;
                case eDT_PCP_ENTRY:
                    device["type"]                    = "Entry Reader";
                    break;   
                case eDT_PCP_EXIT:
                    device["type"]                    = "Exit Reader";
                    break;   
                 #ifdef eDT_PCP_MOBILE
                case eDT_PCP_MOBILE:
                    device["type"]                    = "Mobile Reader";
                    break;
                #endif
                #ifdef eDT_PCP_STATIC
                case eDT_PCP_STATIC:
                    device["type"]                    = "Static Reader";
                    break;           
                #endif                            
                default:
                    device["type"]                    = "unknown";
                    break;
            }
            
            device["ip"] = pEntry->IpAddress;
            

            if(pEntry->AppType!=eDT_MERG)
            {
                device["deviceType"] = "FPDm";
                in_addr addr;
                if(inet_aton(pEntry->IpAddress, &addr)!=0)
                {
                    // The number of the FPDm is the last octet of the ip address
                    device["number"] = addr.s_addr & 0xFF; 
                }
            }
            
            s_deviceList["devices"].append(device);
            
          // Assume that each DC (should only be one) has a printer
            if(pEntry->AppType==eDT_MERG && strcmp(g_terminalType, "BDC") == 0)
            {
                device["id"]                      = "PRINTER";
                device["terminalid"]              = "PRINTER";
                device["status"]                  = getDeviceStatusName(DEVICE_STATUS_ERROR);
                device["opStatus"]                = "unknown";
                device["mode"]                    = "";
                device["log"]                     = Json::Value(Json::arrayValue);              
                device["deviceType"]              = "PRINTER";
                device["number"]                  = "PRINTER";                
                device["ESN"]                     = "N/A";
                device["type"]                    = "Printer";
                device["ip"]                      = "";
                s_deviceList["devices"].append(device);
                count++;
            }               
            count++;
        }

        
       

        CsDebug(1, (1, "deviceList loaded with %d entries", count));//s_deviceList.size()));
	}
	PilShmDisconnect( pCceDevTbl ); // Frees resource
	return 0;
}

void VaropPollTimer(uint32_t timer)
{
    CsDebug(8, (8, "VaropPollTimer")); 
    requestVaropToAll(VO_DEVICE_STATUS);
    requestVaropToAll(VO_READER_ENABLED);
    requestVaropToAll(VO_READER_MODE);    
    requestVaropToAll(VO_READER_ESN); 

    // TODO: Expand bitmask of allowed status information
    // e.g. updates in progress, TDC critical error
    s_dcStatus = s_driverBreakLockAll?1:0;
    sendVaropToAll(VO_DRIVER_BREAK, s_dcStatus);
  
}


void InitialiseCCEEventThread()
{
	CsDebug(5, (5, "InitialiseCCEEventThread"));
	CsThread_t threadId;
    CsThrCreate(NULL, 0, CCEEventProcessingThread, 0, CSTHR_BOUND, &threadId);
}


int initCCE()
{
    int result = -1;
    
    result = openCCE();
	if(result==0)
	{
		if(LoadDeviceList()==0)
        {
            // TODO eliminate race condition between this request and thread startup (Push event onto message queue)
            InitialiseCCEEventThread();
            
            CsfTimerCreate2(VaropPollTimer, "VaropPollTimer", &pollTimer);
            TimerSpec_t ts = { { 0, 0 }, { 0, 0 } };
            ts.it_interval.tm_msecs = g_pollDelayMS;       
            ts.it_value.tm_secs    = 1;
            CsfTimerSet(pollTimer, TMR_RELTIME, &ts);    
            return 0;
        }
        else
        {
            CsErrx("Failed to load device list.");
            result = -1;
        }
	}
    else
    {
        CsErrx("openCCE failed to connect to CCE");
    }
	
    return result;
}
