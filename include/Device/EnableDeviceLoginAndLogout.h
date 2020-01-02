// Copyright (c) 2019, *** Inc.
// All rights reserved.
//
// Author : ������
// E-mail : wangkw531@icloud.com
//
// Abstract base class of device for logging in and out.
//

#ifndef ENABLE_DEVICE_LOGIN_AND_LOGOUT_H
#define ENABLE_DEVICE_LOGIN_AND_LOGOUT_H

#include "predef.h"

NS_BEGIN(device, 1)

class EnableDeviceLoginAndLogout
{
protected:
	EnableDeviceLoginAndLogout(void) {}
	virtual ~EnableDeviceLoginAndLogout(void) {}

protected:
	//The asynchronous login status, UserID, and device information are returned by the call-back function(fLoginResultCallBack) 
	//which is configured in the structure of NET_DVR_USER_LOGIN_INFO. For asynchronous login, the returned value of -1 
	//indicates login failed, and other returned value indicates the user ID. The userID is unique, and is indispensable for operating 
	//the device.
	virtual int loginDevice(void) = 0;
	virtual int logoutDevice(void) = 0;
};//class EnableDeviceLoginAndLogout

NS_END

#endif//ENABLE_DEVICE_LOGIN_AND_LOGOUT_H