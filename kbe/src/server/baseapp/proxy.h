﻿/*
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


#ifndef KBE_PROXY_H
#define KBE_PROXY_H
	
#include "base.h"
#include "data_downloads.h"
#include "common/common.h"
#include "helper/debug_helper.h"
#include "network/address.h"
#include "network/message_handler.h"
	
namespace KBEngine{


namespace Network
{
class Channel;
}

class ProxyForwarder;

#define LOG_ON_REJECT  0
#define LOG_ON_ACCEPT  1
#define LOG_ON_WAIT_FOR_DESTROY 2

class Proxy : public Base
{
	/** 子类化将一些py操作填充进派生类 */
	BASE_SCRIPT_HREADER(Proxy, Base)	

public:
	Proxy(ENTITY_ID id, const ScriptDefModule* pScriptModule);
	~Proxy();
	
	INLINE void addr(const Network::Address& address);
	INLINE const Network::Address& addr() const;

	typedef std::vector<Network::Bundle*> Bundles;
	bool pushBundle(Network::Bundle* pBundle);

	/**
		向witness客户端推送一条消息
	*/
	bool sendToClient(const Network::MessageHandler& msgHandler, Network::Bundle* pBundle);
	bool sendToClient(Network::Bundle* pBundle);
	bool sendToClient(bool expectData = true);

	/** 
		脚本请求获取连接的rtt值
	*/
	double getRoundTripTime() const;
	DECLARE_PY_GET_MOTHOD(pyGetRoundTripTime);

	/** 
		This is the number of seconds since a packet from the client was last received. 
	*/
	double getTimeSinceHeardFromClient() const;
	DECLARE_PY_GET_MOTHOD(pyGetTimeSinceHeardFromClient);

	/** 
		脚本请求获取是否有client绑定到proxy上
	*/
	bool hasClient() const;
	DECLARE_PY_GET_MOTHOD(pyHasClient);

	/** 
		脚本请求获取client地址
	*/
	DECLARE_PY_GET_MOTHOD(pyClientAddr);

	/** 
		实体是否可用
	*/
	INLINE bool entitiesEnabled() const;
	DECLARE_PY_GET_MOTHOD(pyGetEntitiesEnabled);

	/**
		这个entity被激活了, 在客户端初始化好对应的entity后， 这个方法被调用
	*/
	void onEntitiesEnabled(void);
	
	/**
		一个数据下载任务完成
	*/
	void onStreamComplete(int16 id, bool success);

	/**
		登陆尝试， 当正常的登陆失败之后， 调用这个接口再进行尝试 
	*/
	int32 onLogOnAttempt(const char* addr, uint32 port, const char* password);
	
	/**
		初始化客户端proxy的属性
	*/
	void initClientBasePropertys();
	void initClientCellPropertys();

	/** 
		当察觉这个entity对应的客户端socket断开时被调用 
	*/
	void onClientDeath(void);
	
	/** 网络接口
		当客户端所关联的这个entity的cell被创建时，被调用 
	*/
	void onClientGetCell(Network::Channel* pChannel, COMPONENT_ID componentID);

	/**
		获取前端类别
	*/
	INLINE COMPONENT_CLIENT_TYPE getClientType() const;
	INLINE void setClientType(COMPONENT_CLIENT_TYPE ctype);
	DECLARE_PY_MOTHOD_ARG0(pyGetClientType);

	/**
		获取前端附带数据
	*/
	INLINE const std::string& getClientDatas();
	INLINE void setClientDatas(const std::string& datas);
	DECLARE_PY_MOTHOD_ARG0(pyGetClientDatas);

	/**
		每个proxy创建之后都会由系统产生一个uuid， 提供前端重登陆时用作身份识别
	*/
	INLINE uint64 rndUUID() const;
	INLINE void rndUUID(uint64 uid);

	/** 
		将其自身所关联的客户端转给另一个proxy去关联 
	*/
	void giveClientTo(Proxy* proxy);
	void onGiveClientTo(Network::Channel* lpChannel);
	void onGiveClientToFailure();
	DECLARE_PY_MOTHOD_ARG1(pyGiveClientTo, PyObject_ptr);

	/**
		文件流数据下载
	*/
	static PyObject* __py_pyStreamFileToClient(PyObject* self, PyObject* args);
	int16 streamFileToClient(PyObjectPtr objptr, 
		const std::string& descr = "", int16 id = -1);

	/**
		字符串流数据下载
	*/
	static PyObject* __py_pyStreamStringToClient(PyObject* self, PyObject* args);
	int16 streamStringToClient(PyObjectPtr objptr, 
		const std::string& descr = "", int16 id = -1);

	/**
		绑定了witness
	*/
	void onGetWitness();

	/**
		将客户端从服务器踢出
	*/
	void kick();

	/**
		获得这个proxy的客户端连接对象
	*/
	Network::Channel* pChannel();

protected:
	uint64 rndUUID_;
	Network::Address addr_;
	DataDownloads dataDownloads_;

	bool entitiesEnabled_;

	// 限制客户端每秒所能使用的带宽
	int32 bandwidthPerSecond_;

	// 通信加密key 默认blowfish
	std::string encryptionKey;

	ProxyForwarder* pProxyForwarder_;

	COMPONENT_CLIENT_TYPE clientComponentType_;
	std::string clientDatas_;
};

}


#ifdef CODE_INLINE
#include "proxy.inl"
#endif

#endif // KBE_PROXY_H
