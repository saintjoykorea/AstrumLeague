#pragma once

#import <Foundation/Foundation.h>

#define QAV_RESULT(label, value) QAV_Result_##label = value

/*!
 @discussion    错误码，方法的返回值和异步回调请参考该错误码
 */
typedef NS_ENUM(NSInteger, QAVResult) {

    QAV_OK = 0,       ///< QAV_OK 成功操作。
///QAV_ERR_FAIL = 1,  如果需要对所有错误码统一处理，请使用!AV_OK；
///如果需要单独处理每一类错误，请关注接口返回的错误类型。
///此错误码没有明确含义，且2.4以后版本不再返回，故删除。
///---------------------------------------------------------------------------------------
/// @name 客户端错误
///---------------------------------------------------------------------------------------
  

    QAV_ERR_REPETITIVE_OPERATION = 1001,    ///< 重复操作。已经在进行某种操作，再次去做同样的操作，则返回这个错误。
    QAV_ERR_EXCLUSIVE_OPERATION  = 1002,    ///< 互斥操作。已经在进行某种操作，再次去做同类型的其他操作，则返回这个错误。
    QAV_ERR_HAS_IN_THE_STATE     = 1003,    ///< 已经处于所要状态，无需再操作。如设备已经打开，再次去打开，就返回这个错误码。
    QAV_ERR_INVALID_ARGUMENT     = 1004,    ///< 错误参数。
    QAV_ERR_TIMEOUT              = 1005,    ///< 操作超时。
    QAV_ERR_NOT_IMPLEMENTED      = 1006,    ///< 功能未实现。
    QAV_ERR_NOT_IN_MAIN_THREAD   = 1007,    ///< 不在主线程中执行操作

    QAV_ERR_CONTEXT_NOT_START    = 1101,    ///< AVContext没有启动。
    QAV_ERR_ROOM_NOT_EXIST       = 1201,    ///< 房间不存在。

    QAV_ERR_DEVICE_NOT_EXIST     = 1301,    ///< 设备不存在。

    QAV_ERR_SERVER_FAILED                    = 10001,   ///< 服务器返回一般错误
    QAV_ERR_SERVER_NO_PERMISSION             = 10003,   ///< 没有权限
    QAV_ERR_SERVER_REQUEST_ROOM_ADDRESS_FAIL = 10004,   ///< 进房间获取房间地址失败
    QAV_ERR_SERVER_CONNECT_ROOM_FAIL_INFO    = 10005,   ///< 进房间连接房间失败
    QAV_ERR_SERVER_FREE_FLOW_AUTH_FAIL       = 10006,   ///< 免流情况下，免流签名校验失败，导致进房获取房间地址失败
    QAV_ERR_SERVER_ROOM_DISSOLVED            = 10007,   ///< 游戏应用房间超过90分钟，强制下线

    QAV_ERR_IMSDK_FAIL                                     = 6999,          ///< IMSDK失败
    QAV_ERR_IMSDK_TIMEOUT                                  = 7000,          ///< IMSDK超时
    QAV_ERR_HTTP_REQ_FAIL                                   = 7001,
    QAV_ERR_3DVOICE_ERR_FILE_DAMAGED                        = 7002,//3d voice model file is damaged.
    QAV_ERR_3DVOICE_ERR_NOT_INITED                          = 7003,//should call InitSpatializer first
    QAV_ERR_UNKNOWN                                        = 65536,         ///< 无效值

    QAV_ERR_NET_REQUEST_FALLED = 7004,
    QAV_ERR_CHARGE_OVERDUE     = 7005,
    QAV_ERR_AUTH_FIALD         = 7006,
    QAV_ERR_IN_OTHER_ROOM      = 7007,
    QAV_ERR_DISSOLVED_OVERUSER = 7008,
    QAV_ERR_NO_PERMISSION      = 7009,
    QAV_ERR_FILE_CANNOT_ACCESS = 7010,
    QAV_ERR_FILE_DAMAGED       = 7011,
    QAV_ERR_SERVICE_NOT_OPENED = 7012,
///---------------------------------------------------------------------------------------
/// @name 实时语音伴奏相关错误
///---------------------------------------------------------------------------------------
    QAV_ERR_ACC_OPENFILE_FAILED            = 4001,        ///< 打开文件失败
    QAV_ERR_ACC_FILE_FORAMT_NOTSUPPORT     = 4002,        ///< 不支持的文件格式
    QAV_ERR_ACC_DECODER_FAILED             = 4003,        ///< 解码失败
    QAV_ERR_ACC_BAD_PARAM                  = 4004,        ///< 参数错误
    QAV_ERR_ACC_MEMORY_ALLOC_FAILED        = 4005,        ///< 内存分配失败
    QAV_ERR_ACC_CREATE_THREAD_FAILED       = 4006,        ///< 创建线程失败
    QAV_ERR_ACC_STATE_ILLIGAL              = 4007,        ///< 状态非法

///---------------------------------------------------------------------------------------
/// @name 实时语音音效相关错误
///---------------------------------------------------------------------------------------
    QAV_ERR_EFFECT_OPENFILE_FAILED         = 4051,        ///< 打开文件失败
    QAV_ERR_EFFECT_FILE_FORAMT_NOTSUPPORT  = 4052,        ///< 不支持的文件格式
    QAV_ERR_EFFECT_DECODER_FAILED          = 4053,        ///< 解码失败
    QAV_ERR_EFFECT_BAD_PARAM               = 4054,       ///< 参数错误
    QAV_ERR_EFFECT_MEMORY_ALLOC_FAILED     = 4055,       ///< 内存分配失败
    QAV_ERR_EFFECT_CREATE_THREAD_FAILED    = 4056,        ///< 创建线程失败
    QAV_ERR_EFFECT_STATE_ILLIGAL           = 4057,        ///< 状态非法
    
    //for recorder
    QAV_ERR_VOICE_RECORDER_PARAM_NULL                    = 0x1001,
    QAV_ERR_VOICE_RECORDER_INIT_ERROR                    = 0x1002,
    QAV_ERR_VOICE_RECORDER_RECORDING_ERROR               = 0x1003,
    QAV_ERR_VOICE_RECORDER_NO_AUDIO_DATA_WARN            = 0x1004,
    QAV_ERR_VOICE_RECORDER_OPENFILE_ERROR                = 0x1005,
    QAV_ERR_VOICE_RECORDER_MIC_PERMISSION_ERROR          = 0x1006,
    QAV_ERR_VOICE_RECORDER_VOICE_RECORD_TOO_SHORT        = 0x1007,
    QAV_ERR_VOICE_RECORD_NOT_START                       = 0x1008,
};


