﻿#include "stdafx.h"
#include "../Message/Msg_ID.pb.h"
#include "CommandDef.h"
#include "GameDefine.h"
#include "LoginMsgHandler.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonFunc.h"
#include "Utility/CommonEvent.h"
#include "GameService.h"
#include "Utility/CommonSocket.h"
#include "PacketHeader.h"
#include "Error.h"
#include "../Message/Msg_Login.pb.h"




CLoginMsgHandler::CLoginMsgHandler()
{

}

CLoginMsgHandler::~CLoginMsgHandler()
{

}

BOOL CLoginMsgHandler::Init()
{
	

	return TRUE;
}

BOOL CLoginMsgHandler::Uninit()
{
	

	return TRUE;
}



BOOL CLoginMsgHandler::DispatchPacket(NetPacket *pNetPacket)
{
	switch(pNetPacket->m_dwMsgID)
	{
		PROCESS_MESSAGE_ITEM(MSG_CHECK_VERSION_REQ,	OnMsgCheckVersionReq);
		PROCESS_MESSAGE_ITEM(MSG_ACCOUNT_REG_REQ,	OnMsgAccountRegReq);
 		PROCESS_MESSAGE_ITEM(MSG_ACCOUNT_LOGIN_REQ,	OnMsgAccountLoginReq);

		PROCESS_MESSAGE_ITEM(MSG_ACCOUNT_LOGINREG_REQ,	OnMsgAccountLoginRegReq);
 		PROCESS_MESSAGE_ITEM(MSG_SERVER_LIST_REQ,	OnMsgServerListReq);
 		PROCESS_MESSAGE_ITEM(MSG_SELECT_SERVER_REQ,	OnMsgSelectServerReq);
		PROCESS_MESSAGE_ITEM(MSG_ACCOUNT_REG_ACK,	OnMsgAccountRegAck);
		PROCESS_MESSAGE_ITEM(MSG_ACCOUNT_LOGIN_ACK,	OnMsgAccountLoginAck);
		PROCESS_MESSAGE_ITEM(MSG_LOGIC_REGTO_LOGIN_REQ,	OnMsgLogicSvrRegReq);
		PROCESS_MESSAGE_ITEM(MSG_SELECT_SERVER_ACK,	OnMsgSelectServerAck);
		
		

	default:
		{

		}
		break;
	}

	return TRUE;
}

BOOL CLoginMsgHandler::OnMsgCheckVersionReq(NetPacket *pPacket)
{
	CheckVersionReq Req;
	Req.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());

	CheckVersionAck Ack;
	Ack.set_retcode(E_SUCCESSED);
	return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(pPacket->m_pConnect->GetConnectionID(), MSG_CHECK_VERSION_ACK, 0, 0, Ack);
}

BOOL CLoginMsgHandler::OnMsgAccountRegReq(NetPacket *pPacket )
{
	AccountRegReq Req;
	Req.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();
	
	UINT32 nConnID = pPacket->m_pConnect->GetConnectionID();
	ASSERT(nConnID != 0);

	return CGameService::GetInstancePtr()->SendCmdToAccountConnection(MSG_ACCOUNT_REG_REQ, nConnID, 0, Req);
}

BOOL CLoginMsgHandler::OnMsgAccountLoginReq(NetPacket *pPacket)
{
	AccountLoginReq Req;
	Req.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();

	UINT32 nConnID = pPacket->m_pConnect->GetConnectionID();
	ASSERT(nConnID != 0);

	return CGameService::GetInstancePtr()->SendCmdToAccountConnection(MSG_ACCOUNT_LOGIN_REQ, nConnID, 0, Req);
}


BOOL CLoginMsgHandler::OnMsgAccountLoginRegReq(NetPacket *pPacket)
{
	AccountLoginReq Req;
	Req.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();

	UINT32 nConnID = pPacket->m_pConnect->GetConnectionID();
	ASSERT(nConnID != 0);

	return CGameService::GetInstancePtr()->SendCmdToAccountConnection(MSG_ACCOUNT_LOGINREG_REQ, nConnID, 0, Req);


	return TRUE;
}

BOOL CLoginMsgHandler::OnMsgServerListReq(NetPacket *pPacket)
{
	ClientServerListReq Req;
	Req.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();

	UINT32 nConnID = pPacket->m_pConnect->GetConnectionID();
	ASSERT(nConnID != 0);


	ClientServerListAck Ack;

	return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(pPacket->m_pConnect->GetConnectionID(), MSG_SERVER_LIST_ACK, 0, 0, Ack);
}

BOOL CLoginMsgHandler::OnMsgSelectServerReq(NetPacket *pPacket)
{
	SelectServerReq Req;
	Req.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();

	UINT32 nConnID = pPacket->m_pConnect->GetConnectionID();
	ASSERT(nConnID != 0);

	UINT32 SvrConnID =m_LogicSvrMgr.GetLogicConnID(Req.serverid());
	if(SvrConnID == 0)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(SvrConnID, MSG_SELECT_SERVER_REQ, nConnID, 0, Req);
}

BOOL CLoginMsgHandler::OnMsgAccountRegAck( NetPacket *pPacket )
{
	AccountRegAck Ack;
	Ack.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();

	UINT32 nConnID = pHeader->u64TargetID;
	ASSERT(nConnID != 0);
	
	return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(nConnID, MSG_ACCOUNT_REG_ACK, 0, 0, Ack);
}



BOOL CLoginMsgHandler::OnMsgAccountLoginAck( NetPacket *pPacket )
{
	AccountLoginAck Ack;
	Ack.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();

	UINT32 nConnID = pHeader->u64TargetID;
	ASSERT(nConnID != 0);

	return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(nConnID, MSG_ACCOUNT_LOGIN_ACK, 0, 0, Ack);
}

BOOL CLoginMsgHandler::OnMsgLogicSvrRegReq(NetPacket *pPacket)
{
	RegToLoginSvrReq Req;
	Req.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	pPacket->m_pConnect->SetConnectionData(Req.serverid());

	m_LogicSvrMgr.RegisterLogicServer(pPacket->m_pConnect->GetConnectionID(), Req.serverid());

	RegToLoginSvrAck Ack;
	Ack.set_retcode(E_SUCCESSED);
	return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(pPacket->m_pConnect->GetConnectionID(), MSG_LOGIC_REGTO_LOGIN_ACK, 0, 0, Ack);
}

BOOL CLoginMsgHandler::OnMsgSelectServerAck(NetPacket *pPacket)
{
	SelectServerAck Ack;
	Ack.ParsePartialFromArray(pPacket->m_pDataBuffer->GetData(), pPacket->m_pDataBuffer->GetBodyLenth());
	PacketHeader* pHeader = (PacketHeader*)pPacket->m_pDataBuffer->GetBuffer();

	UINT32 nConnID = pPacket->m_pConnect->GetConnectionID();
	ASSERT(nConnID != 0);

	Ack.set_retcode(E_SUCCESSED);
	Ack.set_serveraddr("11111111");
	Ack.set_serverport(1);
	Ack.set_logincode(1);
	return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(pHeader->u64TargetID, MSG_SELECT_SERVER_ACK, 0, 0, Ack);
}
