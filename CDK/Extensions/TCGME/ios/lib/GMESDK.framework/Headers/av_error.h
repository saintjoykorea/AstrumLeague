#pragma once

namespace tencent {
namespace av {
const int AV_OK = 0;
//const int ERR_FAIL = 0x1;如果需要对所有错误码统一处理，请使用!AV_OK；
//如果需要单独处理每一类错误，请关注接口返回的错误类型。
//此错误码没有明确含义，且2.4以后版本不再返回，故删除。
const int AV_ERR_REPETITIVE_OPERATION = 1001;
static const char AV_ERR_INFO_REPETITIVE_OPERATION[] = "repetitive operation";
const int AV_ERR_EXCLUSIVE_OPERATION = 1002;
static const char AV_ERR_EXCLUSIVE_OPERATION_INFO[] = "exclusive operation";
const int AV_ERR_HAS_IN_THE_STATE = 1003;
static const char AV_ERR_HAS_IN_THE_STATE_INFO[] = "just in the state";
const int AV_ERR_INVALID_ARGUMENT = 1004;
static const char AV_ERR_INVALID_ARGUMENT_INFO[] = "invalid argument";
const int AV_ERR_TIMEOUT = 1005;
static const char AV_ERR_TIMEOUT_INFO[] = "waiting timeout, please check your network";
const int AV_ERR_NOT_IMPLEMENTED = 1006;
static const char AV_ERR_NOT_IMPLEMENTED_INFO[] = "function not implemented";
const int AV_ERR_NOT_ON_MAIN_THREAD = 1007;
static const char AV_ERR_INFO_NOT_ON_MAIN_THREAD[] = "not on the main thread";
const int  AV_ERR_CONTEXT_NOT_START = 1101;
static const char AV_ERR_INFO_CONTEXT_NOT_START[] = "AVContext did not start";
const int  AV_ERR_ROOM_NOT_EXIST = 1201;
static const char AV_ERR_ROOM_NOT_EXIST_INFO[] = "room not exist";
const int  AV_ERR_ROOM_NOT_EXITED = 1202;
static const char AV_ERR_ROOM_NOT_EXITED_INFO[] = "room not exited";
const int  AV_ERR_DEVICE_NOT_EXIST = 1301;
static const char AV_ERR_DEVICE_NOT_EXIST_INFO[] = "device not exist";
const int  AV_ERR_SERVER_FAIL = 10001;
static const char AV_ERR_SERVER_FAIL_INFO[] = "server response error";
const int  AV_ERR_SERVER_NO_PERMISSION = 10003;
static const char AV_ERR_SERVER_NO_PERMISSION_INFO[] = "server refused because of no permission";
const int  AV_ERR_SERVER_REQUEST_ROOM_ADDRESS_FAIL = 10004;
static const char AV_ERR_SERVER_REQUEST_ROOM_ADDRESS_FAIL_INFO[] = "request room server address failed";
const int  AV_ERR_SERVER_CONNECT_ROOM_FAIL = 10005;
static const char AV_ERR_SERVER_CONNECT_ROOM_FAIL_INFO[] = "connect room server response error, mostly arguments wrong.";
const int  AV_ERR_SERVER_ROOM_DISSOLVED = 10007;
static const char AV_ERR_SERVER_ROOM_DISSOLVED_INFO[] = "room dissolved because of overuse";
const int  AV_ERR_IMSDK_FAIL  = 6999;
static const char AV_ERR_IMSDK_FAIL_INFO[] = "imsdk return failed";
const int  AV_ERR_IMSDK_TIMEOUT  = 7000;
static const char AV_ERR_IMSDK_TIMEOUT_INFO[] = "imsdk waiting timeout";
const int  AV_ERR_HTTP_REQ_FAIL  = 7001;
static const char AV_ERR_HTTP_REQ_FAIL_INFO[] = "http request failed";

const int AV_ERR_3DVOICE_ERR_FILE_DAMAGED     = 7002;//3d voice model file is damaged.
const int AV_ERR_3DVOICE_ERR_NOT_INITED       = 7003;//should call InitSpatializer first


const int AV_ERR_NET_REQUEST_FALLED = 7004;
const int AV_ERR_CHARGE_OVERDUE     = 7005;
const int AV_ERR_AUTH_FIALD         = 7006;
const int AV_ERR_IN_OTHER_ROOM      = 7007;
const int AV_ERR_DISSOLVED_OVERUSER = 7008;
const int AV_ERR_NO_PERMISSION      = 7009;
const int AV_ERR_FILE_CANNOT_ACCESS = 7010;
const int AV_ERR_FILE_DAMAGED       = 7011;
const int AV_ERR_SERVICE_NOT_OPENED = 7012;

/*
 ---------------------------------------------------------------------------------------
 @name 实时语音伴奏相关错误
---------------------------------------------------------------------------------------
 */
const int AV_ERR_ACC_OPENFILE_FAILED         = 4001;        ///< 打开文件失败
const int AV_ERR_ACC_FILE_FORAMT_NOTSUPPORT  = 4002;        ///< 不支持的文件格式
const int AV_ERR_ACC_DECODER_FAILED          = 4003;        ///< 解码失败
const int AV_ERR_ACC_BAD_PARAM               = 4004;       ///< 参数错误
const int AV_ERR_ACC_MEMORY_ALLOC_FAILED     = 4005;       ///< 内存分配失败
const int AV_ERR_ACC_CREATE_THREAD_FAILED    = 4006;        ///< 创建线程失败
const int AV_ERR_ACC_STATE_ILLIGAL           = 4007;        ///< 状态非法

/*
---------------------------------------------------------------------------------------
 @name 实时语音音效相关错误
---------------------------------------------------------------------------------------
*/
const int AV_ERR_EFFECT_OPENFILE_FAILED         = 4051;        ///< 打开文件失败
const int AV_ERR_EFFECT_FILE_FORAMT_NOTSUPPORT  = 4052;        ///< 不支持的文件格式
const int AV_ERR_EFFECT_DECODER_FAILED          = 4053;        ///< 解码失败
const int AV_ERR_EFFECT_BAD_PARAM               = 4054;       ///< 参数错误
const int AV_ERR_EFFECT_MEMORY_ALLOC_FAILED     = 4055;       ///< 内存分配失败
const int AV_ERR_EFFECT_CREATE_THREAD_FAILED    = 4056;        ///< 创建线程失败
const int AV_ERR_EFFECT_STATE_ILLIGAL           = 4057;        ///< 状态非法
    
const int  AV_ERR_UNKNOWN = 65536;
static const char AV_ERR_INFO_UNKNOWN[] = "unknown error";

const int VOICE_RECORD_AUDIO_TOO_SHORT		  = 0x1007;

const int VOICE_UPLOAD_FILE_ACCESSERROR       = 0x2001;
const int VOICE_UPLOAD_SIGN_CHECK_FAIL        = 0x2002;
const int VOICE_UPLOAD_NETWORK_FAIL           = 0x2003;
const int VOICE_UPLOAD_GET_TOKEN_NETWORK_FAIL = 0x2004;
const int VOICE_UPLOAD_GET_TOKEN_RESP_NULL    = 0x2005;
const int VOICE_UPLOAD_GET_TOKEN_RESP_INVALID = 0x2006;
const int VOICE_UPLOAD_TOKEN_CHECK_EXPIRED    = 0x2007;
const int VOICE_UPLOAD_APPINFO_UNSET          = 0x2008;

const int VOICE_DOWNLOAD_FILE_ACCESS_ERROR             = 0x3001;       //12289 写文件失败
const int VOICE_DOWNLOAD_SIGN_CHECK_FAIL               = 0x3002;       //12290 TLS签名校验失败，可以尝试重新获取
const int VOICE_DOWNLOAD_COS_INTERNAL_FAIL             = 0x3003;       //12291 COS存储系统内部失败，也可能是访问COS网络超时
const int VOICE_DOWNLOAD_REMOTEFILE_ACCESS_ERROR       = 0x3004;       //12292 服务器文件系统错误
const int VOICE_DOWNLOAD_GET_SIGN_NETWORK_FAIL         = 0x3005;       //12293 获取下载参数过程中，http网络失败
const int VOICE_DOWNLOAD_GET_SIGN_RSP_DATA_NULL        = 0x3006;       //12294 获取下载参数过程中，回包数据为空
const int VOICE_DOWNLOAD_GET_SIGN_RSP_DATA_DECODE_FAIL = 0x3007;       //12295 获取下载参数过程中，回包解包失败
const int VOICE_DOWNLOAD_SIGN_CHECK_EXPIRED            = 0x3008;       //12296 TLS签名校验明确过期，需要重新申请TLS签名
const int VOICE_DOWNLOAD_APPINFO_UNSET                 = 0x3009;       //12297 app info为空
    

const int VOICE_PLAY_INIT_ERROR 			  = 0x5001;	

const int VOICE_ERR_VOICE_S2T_NETWORK_FAIL             = 0x8002;
const int VOICE_ERR_VOICE_STREAMIN_RECORD_SUC_REC_FAIL = 0x8007; // Failed while Uploading, but recorded
const int VOICE_ERR_VOICE_V2T_SIGN_CHECK_FAIL          = 0x8008; // AuthBuffer Check Failed
const int VOICE_ERR_VOICE_STREAMIN_UPLOADANDRECORD_SUC_REC_FAIL = 0x8009; // Failed while converting, but uploaded and recorded
} // namespace av
} // namespace tencent
