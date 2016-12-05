/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef KBE_LOGINAPP_H
#define KBE_LOGINAPP_H
	
// common include	
#include "server/kbemain.h"
#include "server/serverapp.h"
#include "server/idallocate.h"
#include "server/serverconfig.h"
#include "server/pendingLoginmgr.h"
#include "server/python_app.h"
#include "common/timer.h"
#include "network/endpoint.h"
	
namespace KBEngine{

class HTTPCBHandler;
class TelnetServer;

class Loginapp :	public PythonApp, 
					public Singleton<Loginapp>
{
public:
	enum TimeOutType
	{
		TIMEOUT_TICK = TIMEOUT_PYTHONAPP_MAX + 1
	};

	Loginapp(Network::EventDispatcher& dispatcher, 
		Network::NetworkInterface& ninterface, 
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Loginapp();
	
	bool run();
	
	virtual void onChannelDeregister(Network::Channel * pChannel);

	virtual void handleTimeout(TimerHandle handle, void * arg);
	void handleMainTick();

	/* Initializing interface */
	bool initializeBegin();
	bool inInitialize();
	bool initializeEnd();
	void finalise();
	void onInstallPyModules();
	
	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

	virtual void onHello(Network::Channel* pChannel, 
		const std::string& verInfo, 
		const std::string& scriptVerInfo, 
		const std::string& encryptedKey);

	/** Network interface
		A client app inform is active.
	*/
	void onClientActiveTick(Network::Channel* pChannel);

	/** Network interface
		Create an account
	*/
	bool _createAccount(Network::Channel* pChannel, std::string& accountName, 
		std::string& password, std::string& datas, ACCOUNT_TYPE type = ACCOUNT_TYPE_NORMAL);
	void reqCreateAccount(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		Create email accounts
	*/
	void reqCreateMailAccount(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		Create an account
	*/
	void onReqCreateAccountResult(Network::Channel* pChannel, MemoryStream& s);
	void onReqCreateMailAccountResult(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		Reset account password application (forgot password?)
	*/
	void reqAccountResetPassword(Network::Channel* pChannel, std::string& accountName);
	void onReqAccountResetPasswordCB(Network::Channel* pChannel, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

	/** Network interface
		DbMgr returns account activation
	*/
	void onAccountActivated(Network::Channel* pChannel, std::string& code, bool success);

	/** Network interface
		DbMgr account bound email return
	*/
	void onAccountBindedEmail(Network::Channel* pChannel, std::string& code, bool success);

	/** Network interface
		DbMgr return account to reset the password
	*/
	void onAccountResetPassword(Network::Channel* pChannel, std::string& code, bool success);

	/** Network interface
		User logins to the server
		clientType[COMPONENT_CLIENT_TYPE]: Front-end category (mobile, Web, pcexe end)
		clientData[str]: Front end with data (can be anything, like with phone type, browser type, etc)
		accountName[str]: The account name
		password[str]: Password
	*/
	void login(Network::Channel* pChannel, MemoryStream& s);

	/*
		Logon failure
		failedcode: Failure return codes NETWORK_ERR_SRV_NO_READY:Server is not ready, 
									NETWORK_ERR_SRV_OVERLOAD:Server is overloaded, 
									NETWORK_ERR_NAME_PASSWORD:User name or password is not correct
	*/
	void _loginFailed(Network::Channel* pChannel, std::string& loginName, 
		SERVER_ERROR_CODE failedcode, std::string& datas, bool force = false);
	
	/** Network interface
		dbmgr Returns the login test results
	*/
	void onLoginAccountQueryResultFromDbmgr(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		baseappmgr Returns the login gateway address
	*/
	void onLoginAccountQueryBaseappAddrFromBaseappmgr(Network::Channel* pChannel, std::string& loginName, 
		std::string& accountName, std::string& addr, uint16 port);


	/** Network interface
		dbmgr Initial information sent
		startGlobalOrder: Global startup sequence including a variety of different components
		startGroupOrder: Set boot sequence, as in all baseapp several starts.
	*/
	void onDbmgrInitCompleted(Network::Channel* pChannel, COMPONENT_ORDER startGlobalOrder, 
		COMPONENT_ORDER startGroupOrder, const std::string& digest);

	/** Network interface
		Client agreements exporting
	*/
	void importClientMessages(Network::Channel* pChannel);

	/** Network interface
		Error code that describes the exported
	*/
	void importServerErrorsDescr(Network::Channel* pChannel);

	// Engine versions do not match
	virtual void onVersionNotMatch(Network::Channel* pChannel);

	// Engine does not match the script version
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel);

	/** Network interface
		baseappS ynchronize your own initialization information
		startGlobalOrder: Global startup sequence including a variety of different components
		startGroupOrder: Set boot sequence, as in all baseapp several starts.
	*/
	void onBaseappInitProgress(Network::Channel* pChannel, float progress);

protected:
	TimerHandle							mainProcessTimer_;

	// Log requests registered account has not been logged
	PendingLoginMgr						pendingCreateMgr_;

	// Record log on to the server but not yet processed account
	PendingLoginMgr						pendingLoginMgr_;

	std::string							digest_;

	HTTPCBHandler*						pHttpCBHandler;

	float								initProgress_;
	
	TelnetServer*						pTelnetServer_;
};

}

#endif // KBE_LOGINAPP_H
