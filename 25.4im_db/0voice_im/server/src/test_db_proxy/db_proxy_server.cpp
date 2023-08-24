/*
 * db_proxy_server.cpp
 *
 *  Created on: 2014年7月21日
 *      Author: ziteng
 */

#include "netlib.h"
#include "ConfigFileReader.h"
#include "version.h"
#include "ThreadPool.h"
#include "DBPool.h"
#include "CachePool.h"
#include "ProxyConn.h"
#include "HttpClient.h"
#include "EncDec.h"
#include "business/AudioModel.h"
#include "business/MessageModel.h"
#include "business/SessionModel.h"
#include "business/RelationModel.h"
#include "business/UserModel.h"
#include "business/GroupModel.h"
#include "business/GroupMessageModel.h"
#include "business/FileModel.h"
#include "SyncCenter.h"

string strAudioEnc;
// this callback will be replaced by imconn_callback() in OnConnect()
void proxy_serv_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CProxyConn* pConn = new CProxyConn();
		pConn->OnConnect(handle);
	}
	else
	{
		log("!!!error msg: %d", msg);
	}
}

#define random(a,b) (rand()%(b-a+1)+a)
// 注册
static bool insertUser(DBUserInfo_t& cUser)
{
    bool bRet = false;
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
    if (pDBConn)
    {
    	string strSql;
    	if(0 == cUser.nId)
			strSql = "insert into IMUser(`salt`,`sex`,`nick`,`password`,`domain`,`name`,`phone`,`email`,`avatar`,`sign_info`,`departId`,`status`,`created`,`updated`) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
		else
			strSql = "insert into IMUser(`id`,`salt`,`sex`,`nick`,`password`,`domain`,`name`,`phone`,`email`,`avatar`,`sign_info`,`departId`,`status`,`created`,`updated`) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

		CPrepareStatement* stmt = new CPrepareStatement();
        if (stmt->Init(pDBConn->GetMysql(), strSql))
        {
            srand((unsigned)time(NULL));
	        uint32_t nSalt = random(1000,10000);
            uint32_t nNow = (uint32_t) time(NULL);
            uint32_t index = 0;
			string strOutPass = "";
			string strSalt = "";
            uint32_t nGender = cUser.nSex;
            uint32_t nStatus = 0;

            string strUserPass = "123456";
            char strUserPassMd5[33];
        	CMd5::MD5_Calculate(strUserPass.c_str(), strUserPass.length(), strUserPassMd5); // 计算MD5后的用户密码
            
			string strInPass = strUserPassMd5 + int2string(nSalt);  // 计算MD5后的用户密码 + 混淆码
        	char szMd5[33];
        	CMd5::MD5_Calculate(strInPass.c_str(), strInPass.length(), szMd5);
        	strOutPass = szMd5;
			strSalt = int2string(nSalt);


			if(0 != cUser.nId)
				stmt->SetParam(index++, cUser.nId);
			
            stmt->SetParam(index++, strSalt);
            stmt->SetParam(index++, nGender);
            stmt->SetParam(index++, cUser.strNick);
			stmt->SetParam(index++, strOutPass);
            stmt->SetParam(index++, cUser.strDomain);
            stmt->SetParam(index++, cUser.strName);
            stmt->SetParam(index++, cUser.strTel);
            stmt->SetParam(index++, cUser.strEmail);
            stmt->SetParam(index++, cUser.strAvatar);
            stmt->SetParam(index++, cUser.sign_info);
            stmt->SetParam(index++, cUser.nDeptId);
            stmt->SetParam(index++, nStatus);
            stmt->SetParam(index++, nNow);
            stmt->SetParam(index++, nNow);
            bRet = stmt->ExecuteUpdate();
            
            if (!bRet)
            {
                printf("insert user failed: %s\n", strSql.c_str());
                sleep(1);
            }
			else
			{
				cUser.nId = stmt->GetInsertId();
			}
        }else {
             printf("smt Init failed\n");
        }
        delete stmt;
        pDBManager->RelDBConn(pDBConn);
    }
    else
    {
        printf("no db connection!\n");
    }
    return bRet;
}


// 登录验证

static bool doLogin(const std::string &strName, const std::string &strPass)
{
    bool bRet = false;
    CDBManager* pDBManger = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManger->GetDBConn("teamtalk_slave");
    if (pDBConn) {
        string strSql = "select * from IMUser where name='" + strName + "' and status=0";
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            bRet = true;
            string strResult, strSalt;
            uint32_t nId, nGender, nDeptId, nStatus;
            string strNick, strAvatar, strEmail, strRealName, strTel, strDomain,strSignInfo;
            while (pResultSet->Next()) {
                nId = pResultSet->GetInt("id");
                strResult = pResultSet->GetString("password");
                strSalt = pResultSet->GetString("salt");
                
                strNick = pResultSet->GetString("nick");
                nGender = pResultSet->GetInt("sex");
                strRealName = pResultSet->GetString("name");
                strDomain = pResultSet->GetString("domain");
                strTel = pResultSet->GetString("phone");
                strEmail = pResultSet->GetString("email");
                strAvatar = pResultSet->GetString("avatar");
                nDeptId = pResultSet->GetInt("departId");
                nStatus = pResultSet->GetInt("status");
                strSignInfo = pResultSet->GetString("sign_info");

            }

            string strInPass = strPass + strSalt;
            char szMd5[33];
            CMd5::MD5_Calculate(strInPass.c_str(), strInPass.length(), szMd5);

            delete  pResultSet;
        }else {
             printf("ResultSet failed\n");
        }
        pDBManger->RelDBConn(pDBConn);
    } else {
        printf("no db connection!\n");
    }
    return bRet;
}

static void sendMessage(uint32_t from_user_id, uint32_t to_session_id, IM::BaseDefine::MsgType msg_type, string &msg_data)
{
    uint32_t nFromId = from_user_id;
    uint32_t nToId = to_session_id;
    uint32_t nCreateTime = (uint32_t) time(NULL);
    IM::BaseDefine::MsgType nMsgType = msg_type;
    uint32_t nMsgLen = msg_data.size();
    
    uint32_t nNow = (uint32_t)time(NULL);
    if (IM::BaseDefine::MsgType_IsValid(nMsgType))
    {
        if(nMsgLen != 0)
        {
            CImPdu* pPduResp = new CImPdu;

            uint32_t nMsgId = INVALID_VALUE;
            uint32_t nSessionId = INVALID_VALUE;
            uint32_t nPeerSessionId = INVALID_VALUE;

            CMessageModel* pMsgModel = CMessageModel::getInstance();
            CGroupMessageModel* pGroupMsgModel = CGroupMessageModel::getInstance();
            if(nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_TEXT) {
                CGroupModel* pGroupModel = CGroupModel::getInstance();
                if (pGroupModel->isValidateGroupId(nToId) && pGroupModel->isInGroup(nFromId, nToId))
                {
                    nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
                    if (INVALID_VALUE == nSessionId) {
                        nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
                    }
                    if(nSessionId != INVALID_VALUE)
                    {
                        nMsgId = pGroupMsgModel->getMsgId(nToId);
                        if (nMsgId != INVALID_VALUE) {
                            pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg_data);
                            CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                        }
                    }
                }
                else
                {
                    log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
                    delete pPduResp;
                    return;
                }
            } else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_AUDIO) {
                CGroupModel* pGroupModel = CGroupModel::getInstance();
                if (pGroupModel->isValidateGroupId(nToId)&& pGroupModel->isInGroup(nFromId, nToId))
                {
                    nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
                    if (INVALID_VALUE == nSessionId) {
                        nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
                    }
                    if(nSessionId != INVALID_VALUE)
                    {
                        nMsgId = pGroupMsgModel->getMsgId(nToId);
                        if(nMsgId != INVALID_VALUE)
                        {
                            pGroupMsgModel->sendAudioMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg_data.c_str(), nMsgLen);
                            CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                        }
                    }
                }
                else
                {
                    log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
                    delete pPduResp;
                    return;
                }
            } else if(nMsgType== IM::BaseDefine::MSG_TYPE_SINGLE_TEXT) {
                if (nFromId != nToId) {
                    nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                    if (INVALID_VALUE == nSessionId) {
                        nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                    }
                    nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                    if(INVALID_VALUE ==  nPeerSessionId)
                    {
                        nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                    }
                    uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
                    if(nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
                    {
                        nMsgId = pMsgModel->getMsgId(nRelateId);
                        if(nMsgId != INVALID_VALUE)
                        {
                            pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg_data);
                            CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                            CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
                        }
                        else
                        {
                            log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                        }
                    }
                    else{
                        log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                    }
                }
                else
                {
                    log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
                }
                
            } else if(nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_AUDIO) {
                
                if(nFromId != nToId)
                {
                    nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                    if (INVALID_VALUE == nSessionId) {
                        nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                    }
                    nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                    if(INVALID_VALUE ==  nPeerSessionId)
                    {
                        nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                    }
                    uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
                    if(nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
                    {
                        nMsgId = pMsgModel->getMsgId(nRelateId);
                        if(nMsgId != INVALID_VALUE) {
                            pMsgModel->sendAudioMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg_data.c_str(), nMsgLen);
                            CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                            CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
                        }
                        else {
                            log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                        }
                    }
                    else {
                        log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                    }
                }
                else
                {
                    log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
                }
            }

            log("fromId=%u, toId=%u, type=%u, msgId=%u, sessionId=%u", nFromId, nToId, nMsgType, nMsgId, nSessionId);
        }
        else
        {
            log("msgLen error. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
        }
    }
    else
    {
        log("invalid msgType.fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
    }
}

static void getLatestMsgId(uint32_t user_id, uint32_t session_id, IM::BaseDefine::SessionType session_type)
{
    uint32_t nUserId = user_id;
    IM::BaseDefine::SessionType nType = session_type;
    uint32_t nPeerId = session_id;
    if (IM::BaseDefine::SessionType_IsValid(nType)) {
        CImPdu* pPduResp = new CImPdu;
        uint32_t nMsgId = INVALID_VALUE;
        if(IM::BaseDefine::SESSION_TYPE_SINGLE == nType)
        {
            string strMsg;
            IM::BaseDefine::MsgType nMsgType;
            CMessageModel::getInstance()->getLastMsg(nUserId, nPeerId, nMsgId, strMsg, nMsgType, 1);
        }
        else
        {
            string strMsg;
            IM::BaseDefine::MsgType nMsgType;
            uint32_t nFromId = INVALID_VALUE;
            CGroupMessageModel::getInstance()->getLastMsg(nPeerId, nMsgId, strMsg, nMsgType, nFromId);
        }
        log("userId=%u, peerId=%u, sessionType=%u, msgId=%u", nUserId, nPeerId, nType,nMsgId);

    }
    else
    {
        log("invalid sessionType. userId=%u, peerId=%u, sessionType=%u", nUserId, nPeerId, nType);
    }
}



int main(int argc, char* argv[])
{
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: DBProxyServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}

	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));

	CacheManager* pCacheManager = CacheManager::getInstance();
	if (!pCacheManager) {
		log("CacheManager init failed");
		return -1;
	}

	CDBManager* pDBManager = CDBManager::getInstance();
	if (!pDBManager) {
		log("DBManager init failed");
		return -1;
	}
    puts("db init success");
	// 主线程初始化单例，不然在工作线程可能会出现多次初始化
	if (!CAudioModel::getInstance()) {
		return -1;
	}
    
    if (!CGroupMessageModel::getInstance()) {
        return -1;
    }
    
    if (!CGroupModel::getInstance()) {
        return -1;
    }
    
    if (!CMessageModel::getInstance()) {
        return -1;
    }

	if (!CSessionModel::getInstance()) {
		return -1;
	}
    
    if(!CRelationModel::getInstance())
    {
        return -1;
    }
    
    if (!CUserModel::getInstance()) {
        return -1;
    }
    
    if (!CFileModel::getInstance()) {
        return -1;
    }


	CConfigFileReader config_file("dbproxyserver.conf");

	char* listen_ip = config_file.GetConfigName("ListenIP");
	char* str_listen_port = config_file.GetConfigName("ListenPort");
	char* str_thread_num = config_file.GetConfigName("ThreadNum");
    char* str_file_site = config_file.GetConfigName("MsfsSite");
    char* str_aes_key = config_file.GetConfigName("aesKey");

	if (!listen_ip || !str_listen_port || !str_thread_num || !str_file_site || !str_aes_key) {
		log("missing ListenIP/ListenPort/ThreadNum/MsfsSite/aesKey, exit...");
		return -1;
	}
    
    if(strlen(str_aes_key) != 32)
    {
        log("aes key is invalied");
        return -2;
    }
    string strAesKey(str_aes_key, 32);
    CAes cAes = CAes(strAesKey);
    string strAudio = "[语音]";
    char* pAudioEnc;
    uint32_t nOutLen;
    if(cAes.Encrypt(strAudio.c_str(), strAudio.length(), &pAudioEnc, nOutLen) == 0)
    {
        strAudioEnc.clear();
        strAudioEnc.append(pAudioEnc, nOutLen);
        cAes.Free(pAudioEnc);
    }

	uint16_t listen_port = atoi(str_listen_port);
	uint32_t thread_num = atoi(str_thread_num);
    
    string strFileSite(str_file_site);
    CAudioModel::getInstance()->setUrl(strFileSite);

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;
    
    /// yunfan add 2014.9.28
    // for 603 push
    curl_global_init(CURL_GLOBAL_ALL);
    /// yunfan add end

	init_proxy_conn(thread_num);
    CSyncCenter::getInstance()->init();
    CSyncCenter::getInstance()->startSync();

	CStrExplode listen_ip_list(listen_ip, ';');
	for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(listen_ip_list.GetItem(i), listen_port, proxy_serv_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}

	printf("server start listen on: %s:%d\n", listen_ip,  listen_port);
	printf("now enter the event loop...\n");
    writePid();
	//netlib_eventloop(10);

    uint64_t big_for_start_time = get_tick_count();
    uint64_t big_for_end_time  = get_tick_count();
    uint64_t small_for_start_time = get_tick_count();
    uint64_t small_for_end_time  = get_tick_count();

    #define REGISTER_TEST 1 // 1测试 0不测试
    #define REGISTER_BIG_FOR 2
    #define REGISTER_SMALL_FOR 10000

    #define LOGIN_TEST 1 // 1测试 0不测试
    #define LOGIN_BIG_FOR 2
    #define LOGIN_SMALL_FOR 10000

    #define MSG_TEST 1 // 1测试 0不测试
    #define MSG_BIG_FOR 1
    #define MSG_SMALL_FOR 10000


    #define GET_LAST_MSG_TEST 1 // 1测试 0不测试
    #define GET_LAST_MSG_BIG_FOR 1
    #define GET_LAST_MSG_SMALL_FOR 10000
    
    uint32_t user_count = 0;
    
#if REGISTER_TEST 
    printf("start test register user\n");
    // 1. 测试注册性能
    big_for_start_time = get_tick_count();
    for(int i = 0; i < REGISTER_BIG_FOR; i++) {
        bool ret = true;
        small_for_start_time = get_tick_count();
        for(int j = 0; j < REGISTER_SMALL_FOR; j++) {
            user_count++;
            DBUserInfo_t user_info;
            memset(&user_info, 0, sizeof(DBUserInfo_t));
            user_info.nSex = 1;
            user_info.nStatus = 0;
            user_info.nDeptId = 0;
            string name = "lqf" + int2string(user_count);
            string domain = "lqf" + int2string(user_count);
            string phone =  int2string(user_count);
            user_info.strNick =  "0";
            user_info.strDomain = domain;
            user_info.strName =  name;
            user_info.strTel =  "18570368134";
            user_info.strEmail = "326873713@qq.com";
            user_info.strAvatar = "http";
            user_info.sign_info = "一切只为渴望更优秀的你";
            //printf("insertUser %d\n", user_count);
            ret = insertUser(user_info);
            if(!ret) {
                printf("insertUser small for  failed\n");
                //break;
            }
            if(j % 500 == 0)
            {
                small_for_end_time  = get_tick_count();
                printf("%d %d register user need time:%lums\n", j, REGISTER_SMALL_FOR, small_for_end_time - small_for_start_time);
            }
        }
        small_for_end_time  = get_tick_count();
        printf("%d %d register user need time:%lums\n", i, REGISTER_SMALL_FOR, small_for_end_time - small_for_start_time);
        if(!ret) {
            printf("insertUser big for failed\n");
            //break;
        }
    }
    
    big_for_end_time  = get_tick_count();
    printf("total %d register user need time:%lums\n\n", REGISTER_BIG_FOR * REGISTER_SMALL_FOR, big_for_end_time - big_for_start_time);
#endif

    // 查询
#if LOGIN_TEST    
    printf("start test login user\n");
    // 2. 测试登录查询性能
    big_for_start_time = get_tick_count();
    user_count = 0;
    for(int i = 0; i < LOGIN_BIG_FOR; i++) {
        bool ret = true;
        small_for_start_time = get_tick_count();
        for(int j = 0; j < LOGIN_SMALL_FOR; j++) {
            user_count++;
           
            string name = "0voice" + int2string(user_count);
            string password = "123456";
            char passwordMd5[33];
        	CMd5::MD5_Calculate(password.c_str(), password.length(), passwordMd5);
            ret = doLogin(name, passwordMd5);
            if(!ret) {
                printf("doLogin failed\n");
              //  break;
            }
        }
        small_for_end_time  = get_tick_count();
        printf("%d %d register login need time:%lums\n", i, LOGIN_SMALL_FOR, small_for_end_time - small_for_start_time);
        if(!ret) {
            printf("insertUser failed\n");
            //break;
        }
    }
    
    big_for_end_time  = get_tick_count();
    printf("total %d register user need time:%lums\n", LOGIN_BIG_FOR * LOGIN_SMALL_FOR, big_for_end_time - big_for_start_time);
#endif

#if MSG_TEST
    // 发送消息
    printf("start test send msg\n");
     big_for_start_time = get_tick_count();
    user_count = 0;
    for(int i = 0; i < MSG_BIG_FOR; i++) {
        bool ret = true;
        small_for_start_time = get_tick_count();
        for(int j = 0; j < MSG_SMALL_FOR; j++) {
            user_count++;
            string msg_data = "测试发送消息";
        	sendMessage(user_count, 1, IM::BaseDefine::MSG_TYPE_SINGLE_TEXT, msg_data);
        }
        small_for_end_time  = get_tick_count();
        printf("%d %d test send need time:%lums\n", i, MSG_SMALL_FOR, small_for_end_time - small_for_start_time);
    }
    
    big_for_end_time  = get_tick_count();
    printf("total %d test send need time:%lums\n\n", MSG_BIG_FOR * MSG_SMALL_FOR, big_for_end_time - big_for_start_time);
#endif    

#if GET_LAST_MSG_TEST
        // 发送消息
        printf("start test get last msg\n");
         big_for_start_time = get_tick_count();
        user_count = 0;
        for(int i = 0; i < GET_LAST_MSG_BIG_FOR; i++) {
            bool ret = true;
            small_for_start_time = get_tick_count();
            for(int j = 0; j < GET_LAST_MSG_SMALL_FOR; j++) {
                user_count++;
                string msg_data = "测试发送消息";
                getLatestMsgId(user_count, 1, IM::BaseDefine::SESSION_TYPE_SINGLE);
            }
            small_for_end_time  = get_tick_count();
            printf("%d %d get last msg need time:%lums\n", i, GET_LAST_MSG_SMALL_FOR, small_for_end_time - small_for_start_time);
        }
        
        big_for_end_time  = get_tick_count();
        printf("total %d get last need time:%lums\n\n", GET_LAST_MSG_BIG_FOR * GET_LAST_MSG_SMALL_FOR, big_for_end_time - big_for_start_time);
#endif 


	return 0;
}


