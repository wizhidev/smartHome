#ifndef UDPPROPKT_H
#define UDPPROPKT_H

#include <stdint.h>
#define UDP_PKT_SIZE          56
#define RCU_INFO_SIZE         64
#define WARE_DEV_SIZE         40
#define AIR_SIZE              sizeof(DEV_PRO_AIRCOND)
#define LIGHT_SIZE            sizeof(DEV_PRO_LIGHT)
#define CURTAIN_SIZE          sizeof(DEV_PRO_CURTAIN)
#define LOCK_SIZE             sizeof(DEV_PRO_LOCK)
#define VALUE_SIZE            sizeof(DEV_PRO_VALVE)
#define FRAIR_SIZE            sizeof(DEV_PRO_FRESHAIR)
#define FLOOR_HEAT_SIZE      sizeof(DEV_PRO_FLOOR_HEAT)

#define WARE_AIR_SIZE         (sizeof(WARE_DEV) - 4 + AIR_SIZE)
#define WARE_LGT_SIZE         (sizeof(WARE_DEV) - 4 + LIGHT_SIZE)
#define WARE_CUR_SIZE         (sizeof(WARE_DEV) - 4 + CURTAIN_SIZE)
#define WARE_TV_SIZE          (sizeof(WARE_DEV))
#define WARE_TVUP_SIZE        (sizeof(WARE_DEV))
#define WARE_LOCK_SIZE        (sizeof(WARE_DEV) - 4 + LOCK_SIZE)
#define WARE_VALUE_SIZE       (sizeof(WARE_DEV) - 4 + VALUE_SIZE)
#define WARE_FRAIR_SIZE       (sizeof(WARE_DEV) - 4 + FRAIR_SIZE)
#define WARE_FLOOR_HEAT_SIZE  (sizeof(WARE_DEV) - 4 + FLOOR_HEAT_SIZE)

#define SCENE_SIZE            (16 + 20 * 32)
#define BOARDCHNOUT_SIZE      (28 + 144)
#define BOARDKEYINPUT_SIZE    (28 + 6 * 12 + 6)
#define BOARDWLESSIR_SIZE     28
#define BOARDENVDETECT_SIZE   34
#define KEY_OPITEM_SIZE       16
#define CHN_OPITEM_SIZE       36

enum {
  TYPE_STATE_0,
  TYPE_STATE_1,
  TYPE_STATE_2,
};

//用户传输传输数据帧格式
typedef struct tUDPPROPKT {
  uint8_t head[4];    //固定值，字符串"head"
  uint8_t srcIp[4];   //源IP
  uint8_t dstIp[4];   //目的IP
  uint8_t uidDst[12]; //目的uid地址
  uint8_t pwdDst[8];  //访问目标Rcu密码
  uint8_t uidSrc[12]; //源uid地址
  uint16_t snPkt;      //帧编号，防止重复处理
  uint8_t sumPkt;     //本帧总包数
  uint8_t currPkt;    //当前第几包，从0开始计数
  uint8_t bAck;       //应答属性
  uint8_t datType;    //数据含义分类，取值E_UDP_PRO_DAT枚举
  uint8_t subType1;   //辅助参数
  uint8_t subType2;   //辅助参数
  uint16_t datLen;     //本包数据的dat成员长度
  uint16_t rev;

  uint8_t dat[0];     //数据区,变长
} UDPPROPKT;

typedef enum {
  NOT_ACK,
  IS_ACK,
  NOW_ACK,
} IS_ACK_TYPE;

//设备类型定义
typedef enum {
  e_ware_airCond,
  e_ware_tv,
  e_ware_tvUP,
  e_ware_light,
  e_ware_curtain,
  e_ware_lock,
  e_ware_value,
  e_ware_fresh_air,
  e_ware_multiChn,
  e_ware_floor_heat,
  e_ware_other,
} E_WARE_TYPE;

//设备控制类型
typedef enum {
  e_dev_IR,    //红外控制类
  e_dev_315M,  // 315M控制类
  e_dev_Chn,   //通道控制类
} E_DEV_TYPE;

//网关信息结构体定义
typedef struct rcu_info {
  uint8_t devUnitID[12];  //联网模块的cpuid
  uint8_t devUnitPass[8]; //联网模块的访问密码
  uint8_t name[12];
  uint8_t IpAddr[4];
  uint8_t SubMask[4];
  uint8_t Gateway[4];
  uint8_t centerServ[4];
  uint8_t roomNum[4];      //
  uint8_t macAddr[6];
  uint8_t SoftVersion[2];
  uint8_t HwVersion[2];
  uint8_t bDhcp;
  uint8_t rev2;
} RCU_INFO;

//设备信息结构体定义：
typedef struct t_ware_dev {
  uint8_t canCpuId[12];       //设备的执行端（输出模块）的总线id
  uint8_t devName[12];        //设备所属房间的名称
  uint8_t roomName[12];       //设备所属房间的名称
  uint8_t devType;            //设备类型 取值范围E_WARE_TYPE
  uint8_t devId;              //设备ID
  uint8_t devCtrlType;        //设备类型 取值范围E_DEV_TYPE
  uint8_t datLen;             //设备详细信息数据区长度    40字节的设备头信息
  uint8_t dat[4];             // 设备详细信息数据区  长度可变  对应不同的设备属性数据 但最大长度不能大于255
} WARE_DEV;

//空调设备属性
typedef struct tDevProAircond {
  uint8_t bOnOff;            //开关状态 0关闭  1打开
  uint8_t selMode;           //当前选择的模式
  uint8_t selTemp;           //当前选择的温度
  uint8_t selSpd;            //当前选择的风速
  uint8_t selDirect;         //当前选择的风向
  uint8_t rev1;              //保留未用
  uint16_t powChn;            // 一个uint16_t可以表示0-15号通道 线控中央空调 一般对应5根线  依次定义为 冷 热 大 中 小
} DEV_PRO_AIRCOND;

//空调的模式定义：
typedef enum {
  e_air_auto = 0,
  e_air_hot,
  e_air_cool,
  e_air_dry,
  e_air_wind,
  e_air_mode_total,
} E_AIR_MODE;

//空调的命令定义
typedef enum {
  e_air_pwrOn = 0,
  e_air_pwrOff,
  e_air_spdLow,
  e_air_spdMid,
  e_air_spdHigh,
  e_air_spdAuto,
  e_air_drctUpDn1,     //上下摇摆
  e_air_drctUpDn2,
  e_air_drctUpDn3,
  e_air_drctUpDnAuto,
  e_air_drctLfRt1,     //左右摇摆
  e_air_drctLfRt2,
  e_air_drctLfRt3,
  e_air_drctLfRtAuto,
  e_air_temp14,
  e_air_temp15,
  e_air_temp16,
  e_air_temp17,
  e_air_temp18,
  e_air_temp19,
  e_air_temp20,
  e_air_temp21,
  e_air_temp22,
  e_air_temp23,
  e_air_temp24,
  e_air_temp25,
  e_air_temp26,
  e_air_temp27,
  e_air_temp28,
  e_air_temp29,
  e_air_temp30,
  e_air_cmd_total,
} E_AIR_CMD;

//电视机命令
typedef enum {
  e_tv_offOn = 0,
  e_tv_mute,
  e_tv_numTvAv,

  e_tv_num1,
  e_tv_num2,
  e_tv_num3,

  e_tv_num4,
  e_tv_num5,
  e_tv_num6,

  e_tv_num7,
  e_tv_num8,
  e_tv_num9,

  e_tv_numMenu,
  e_tv_numUp,
  e_tv_num0,

  e_tv_numLf,
  e_tv_enter,
  e_tv_numRt,

  e_tv_numRet,
  e_tv_numDn,
  e_tv_numLookBack,

  e_tv_userDef1,
  e_tv_userDef2,
  e_tv_userDef3,
  e_tv_cmd_total,
} E_TV_CMD;

//机顶盒命令
typedef enum {
  e_tvUP_offOn = 0,        //电源键
  e_tvUP_mute,             //静音
  e_tvUP_numPg,            //主页键

  e_tvUP_num1,
  e_tvUP_num2,
  e_tvUP_num3,

  e_tvUP_num4,
  e_tvUP_num5,
  e_tvUP_num6,

  e_tvUP_num7,
  e_tvUP_num8,
  e_tvUP_num9,

  e_tvUP_numDemand,        //查询键
  e_tvUP_numUp,
  e_tvUP_num0,

  e_tvUP_numLf,
  e_tvUP_enter,
  e_tvUP_numRt,

  e_tvUP_numuint16_teract,
  e_tvUP_numDn,
  e_tvUP_numBack,

  e_tvUP_numVInc,
  e_tvUP_numInfo,
  e_tvUP_numPInc,

  e_tvUP_numVDec,
  e_tvUP_numLive,
  e_tvUP_numPDec,

  e_tvUP_userDef1,
  e_tvUP_userDef2,
  e_tvUP_userDef3,
  e_tvUP_cmd_total,
} E_TVUP_CMD;

//灯光设备属性
typedef struct tDevProLight {
  uint8_t bOnOff;         //开关状态 0关闭  1打开
  uint8_t bTuneEn;        //是否可调 0否   1是
  uint8_t lmVal;          //如果是可调灯光 亮度值 范围0-9
  uint8_t powChn;         //输出模块的通道号
} DEV_PRO_LIGHT;       //整个灯光设备信息字节数44

//灯光控制命令
typedef enum {
  e_lgt_offOn = 0,
  e_lgt_onOff,
  e_lgt_dark,
  e_lgt_bright,
  e_lgt_cmd_total,
} E_LGT_CMD;

//窗帘设备属性
typedef struct tDevProCurtain {
  uint8_t bOnOff;       //开关状态 0关闭  1打开
  uint8_t timRun;       //开关电机运转的时间  超时停止转动  防止有的电机没有行程限制
  uint16_t powChn;       //输出模块的通道号  ?
} DEV_PRO_CURTAIN;

//窗帘控制命令
typedef enum {
  e_curt_offOn = 0,
  e_curt_onOff,
  e_curt_stop,
  e_curt_cmd_total,
} E_CURT_CMD;

//阀门设备属性数据
typedef struct tDevProValve {
  uint8_t bOnOff;           //开关状态 0关闭  1打开
  uint8_t timRun;           //开关电机运转的时间  超时停止转动  防止有的电机没有行程限制
  uint8_t powChnOpen;       //开阀门通道
  uint8_t powChnClose;      //开阀门通道
} DEV_PRO_VALVE;         //整个阀门设备信息字节数44

//阀门控制命令
typedef enum {
  e_valve_offOn = 0,
  e_valve_onOff,
  e_valve_stop,
  e_valve_cmd_total,
} E_VALVE_CMD;

//门锁设备属性数据
typedef struct tDevProLock {
  uint8_t bOnOff;          //开关状态0关闭  1打开
  uint8_t timRun;          //开锁超时 锁打开后必须一段时间之内关上
  uint8_t bLockOut;        //是否反锁  0否  1是
  uint8_t powChnOpen;      //开锁通道
  uint8_t pwd[8];          // 开锁用8位密码
} DEV_PRO_LOCK;         //整个门锁设备信息字节数52

//门锁控制命令
typedef enum {
  e_lock_open = 0,
  e_lock_close,
  e_lock_stop,
  e_lock_lockOut,
  e_lock_cmd_total,
} E_LOCK_CMD;

//新风设备属性数据
typedef struct tDevProFreshAir {
  uint8_t bOnOff;         //开关状态  0关闭  1打开
  uint8_t spdSel;         //新风运行的风速，2，3，4低中高
  uint8_t onOffChn;//新⻛设备的开关通道 如果没有取值0xff
  uint8_t spdLowChn;//新⻛设备的控制通道 低⻛
  uint8_t spdMidChn;//新⻛设备的控制通道 中⻛
  uint8_t spdHighChn;//新⻛设备的控制通道 ⾼⻛
  uint8_t autoRun;//是否根据阀值⾃自动运⾏行行
  uint8_t rev;
  uint16_t valPm10;//⾃自动开启新⻛风设备的颗粒物阀值
  uint16_t valPm25;//⾃自动开启新⻛风设备的颗粒物阀值
} DEV_PRO_FRESHAIR;    //整个新风设备信息字节数44

//新风控制命令
typedef enum {
  e_freshair_open = 0,
  e_freshair_spd_low,
  e_freshair_spd_mid,
  e_freshair_spd_high,
  e_freshair_spd_auto,
  e_freshair_close,
  e_freshair_cmd_total,
} E_FRESH_AIR_CMD;

typedef struct tDevProFLoorHeat {
  uint8_t bOnOff;//开关状态 0关闭 1打开
  uint8_t tempGet;//地暖设备对应区域的当前的室内温度值
  uint8_t tempSet;//地暖设备设定的温度阀值
  uint8_t powChn;//地暖电磁阀对应的开关通道
  uint16_t autoRun;//是否根据阀值⾃自动运⾏行行
  uint16_t rev2;//
} DEV_PRO_FLOOR_HEAT;

typedef enum {
  e_floorHeat_open = 0,
  e_floorHeat_auto,
  e_floorHeat_close,
  e_floorHeat_cmd_total,
} E_FLOOR_HEAT_CMD;

typedef struct tDevMultiItem {
  uint8_t uidBoard[12];  //通道所在的控制板
  uint16_t powChn;
  uint16_t rev;
} DEV_MULTI_ITEM;

//多通道设备属性
typedef struct tDevProMultiChn {
  uint8_t bOnOff;         //开关状态 0关闭  1打开
  uint8_t itemCnt;        // item数组的元素个数
  uint8_t rev2;           //保留未用
  uint8_t rev3;           //保留未用
  DEV_MULTI_ITEM itemAry[12];    //数组长度可变  但是最大不超过12个
} DEV_PRO_MULTICHN;                //整个多通道设备信息字节数

// 多通道控制命令
typedef enum {
  e_multiChn_offOn = 0,
  e_multiChn_onOff,
  e_multiChn_cmd_total,
} E_MULTICHN_CMD;

typedef enum {
  e_udpPro_getRcuInfo = 0,
  e_udpPro_setRcuInfo = 1,
  e_udpPro_handShake = 2,
  e_udpPro_getDevsInfo = 3,
  e_udpPro_ctrlDev = 4,
  e_udpPro_addDev = 5,
  e_udpPro_editDev = 6,
  e_udpPro_delDev = 7,

  e_udpPro_getBoards = 8,
  e_udpPro_editBoards = 9,
  e_udpPro_delBoards = 10,
  e_udpPro_getKeyOpItems = 11,
  e_udpPro_setKeyOpItems = 12,
  e_udpPro_delKeyOpItems = 13,
  e_udpPro_getChnOpItems = 14,
  e_udpPro_setChnOpItems = 15,
  e_udpPro_delChnOpItems = 16,
  e_udpPro_getTimerEvents = 17,
  e_udpPro_addTimerEvents = 18,
  e_udpPro_editTimerEvents = 19,
  e_udpPro_delTimerEvents = 20,
  e_udpPro_exeTimerEvents = 21,

  e_udpPro_getSceneEvents = 22,
  e_udpPro_addSceneEvents = 23,
  e_udpPro_editSceneEvents = 24,
  e_udpPro_delSceneEvents = 25,
  e_udpPro_exeSceneEvents = 26,

  e_udpPro_getEnvEvents = 27,
  e_udpPro_addEnvEvents = 28,
  e_udpPro_editEnvEvents = 29,
  e_udpPro_delEnvEvents = 30,
  e_udpPro_exeEnvEvents = 31,
  e_udpPro_security_info = 32,
  e_udpPro_getRcuInfoNoPwd = 33,
  e_udpPro_pwd_error = 34,
  e_udpPro_chns_status = 35,
  e_udpPro_keyInput_info = 36,

  e_udpPro_getIOSet_input = 37,
  e_udpPro_getIOSet_output = 38,
  e_udpPro_saveIOSet_input = 39,
  e_udpPro_saveIOSet_output = 40,
  e_udpPro_studyIR_cmd = 41,
  e_udpPro_studyIR_cmd_ret = 42,

  e_udpPro_report_output = 43,
  e_udpPro_report_cardInput = 44,

  e_udpPro_irDev_exeRet = 45,
  e_udpPro_quick_setDevKey = 46,
  e_udpPro_quick_delDevKey = 47,
  e_udpPro_get_modulePos = 48,

  e_udpPro_soft_iap_data = 49,
  e_udpPro_get_soft_version = 50,
  e_udpPro_bc_key_ctrl = 51,
  e_udpPro_ctrl_allDevs = 52,
  e_udpPro_hotel_signal = 53,
  e_udpPro_confirm_signal = 54,
  e_udpPro_set_86key_bkLgt = 55,
  e_udpPro_set_IR_bkDat = 56,  //设置红外阵列检测器的背景数据
  e_udpPro_set_IR_param = 57,

  e_udpPro_get_key2scene = 58,
  e_udpPro_set_key2scene = 59,
  e_udpPro_get_keyNameCtrls = 60,                   //60
  e_udpPro_set_keyNameCtrls = 61,
  e_udpPro_keyName_alarm = 62,
  e_udpPro_test_dat = 63,
  e_udpPro_report_io_info = 64,
  e_udpPro_group_operation = 65,
  e_udpPro_secs_trigger = 66,
  e_udpPro_chns_timerEv = 67,
  e_udpPro_report_param = 68,

  e_udpPro_getBroadCast = 80
} E_UDP_PRO_DAT;

typedef enum {
  e_86key_air_power = 0,
  e_86key_air_mode,
  e_86key_air_spd,
  e_86key_air_tempInc,
  e_86key_air_tempDec,
} E_86KEY_AIR_CMD;

typedef enum {
  e_86keyMutex_null = 0,
  e_86keyMutex_on,
  e_86keyMutex_off,
  e_86keyMutex_stop,
  e_86keyMutex_loop,
} E_86KEY_MUTEX_TYPE;

typedef enum {
  e_86keyCtrl_null = 0,
  e_86keyCtrl_offOn,
  e_86keyCtrl_onOff,
  e_86keyCtrl_power,
  e_86keyCtrl_dark,
  e_86keyCtrl_bright,
  e_86keyCtrl_cmd_total,
} E_86KEY_CTRL_TYPE;

typedef enum {
  e_board_chnOut = 0,
  e_board_keyInput,
  e_board_wlessIR,
  e_board_envDetect,
} E_BOARD_TYPE;

//输出模块：
typedef struct BOARD_CHNOUT {
  uint8_t devUnitID[12];      //模块的cpuid
  uint8_t boardName[12];      //模块名称
  uint8_t boardType;          //控制板类型 取值e_board_chnOut
  uint8_t chnCnt;             //通道数
  uint8_t bOnline;            //是否在线 由RCU定时发出握手包检测
  uint8_t rev2;
  uint8_t chnName[12][12];    //目前输出模块最多12个通道   长度根据chnCnt可变
} BOARD_CHNOUT;

//输入模块：
typedef struct BOARD_KEYINPUT {
  uint8_t devUnitID[12];     //模块的cpuid
  uint8_t boardName[12];     //模块名称
  uint8_t boardType;         //控制板类型 取值e_board_keyInput
  uint8_t keyCnt;            //按键数
  uint8_t bResetKey;         //是否是复位按键0复位     1非复位
  uint8_t ledBkType;         //按键的背光灯模式//背光灯   0 随灯状态变化   1 常亮      2 不亮
  uint8_t keyName[8][12];    //目前输入模块最多6个按键
  uint8_t keyAllCtrlType[8]; //总开总关 0 否 1灯总开 2灯总关 3灯总开关 4窗帘类总开 5窗帘类总关 6窗帘类总开关 7全部总开 8 全部总关  9全部总开关
  uint8_t roomName[12];
} BOARD_KEYINPUT;

//无线信号转发模块：
typedef struct BOARD_WLESSIR {
  uint8_t devUnitID[12]; //模块的cpuid
  uint8_t boardName[12]; //模块名称
  uint8_t boardType;     //控制板类型 取值e_board_wlessIR
  uint8_t bOnline;       //是否在线 由RCU定时发出握手包检测
  uint8_t rev2;
  uint8_t rev3;
} BOARD_WLESSIR;

//环境参数探测模块：
typedef struct BOARD_ENVDETECT {
  uint8_t devUnitID[12]; //模块的cpuid
  uint8_t boardName[12]; //模块名称
  uint8_t boardType;     //控制板类型 取值e_board_envDetect,
  uint8_t temp;
  uint8_t humidity;
  uint8_t bOnline;//是否在线 由RCU定时发出握手包检测
  uint16_t pm25;
  uint8_t rev2;
  uint8_t rev3;
} BOARD_ENVDETECT;

//输出模块与输入模块对应关系配置
typedef struct chnop_item {
  uint8_t devUnitID[12];    //按键模块的cpuid
  uint8_t keyDownValid;     //按键板最多6个按键，bit0~bit5 = 1 表示有效
  uint8_t keyUpValid;       //按键板最多6个按键，bit0~bit5 = 1 表示有效
  uint16_t rev1;
  uint8_t keyDownCmd[8];    //每个按键针对本设备的控制命令
  uint16_t rev2;
  uint8_t keyUpCmd[8];      //每个按键针对本设备的控制命令
  uint16_t rev3;
} CHNOP_ITEM;

//输入模块上保存着每个按键与总线上输出模块上的设备的对应关系，数据定义如下：
typedef struct keyop_item {
  uint8_t devUnitID[12];    //输出模块的cpuid
  uint8_t devType;          //设备类型
  uint8_t devId;            //设备Id
  uint8_t keyOpCmd;         //设备操作命令
  uint8_t keyOp;            //按键弹起或是按下   0：按下    1：弹起
} KEYOP_ITEM;

typedef struct {
  uint8_t uid[12];          //设备所在控制板的cpuid
  uint8_t devType;          //设备的类型 E_WARE_TYPE
  uint8_t lmVal;            //保存灯光的亮度值  0-9
  uint8_t rev2;
  uint8_t rev3;
  uint8_t devID;           //设备在控制板上的id
  uint8_t bOnoff;
  uint8_t param1;          //设备定时运行的参数1
  uint8_t param2;          //设备定时运行的参数2
} RUN_DEV_ITEM;

//情景模式
typedef struct {
  uint8_t sceneName[12];//名称
  uint8_t devCnt;//参与事件的设备数目
  uint8_t eventId;//事件id  用于在联网模块上唯一表示事件身份 id0、1的情景定义为在家模式与外出模式 且不可删除
  uint8_t onOff;//   01进入情景    00 退出情景
  uint8_t exeSecu;//关联执行安防的布撤防命令  0--24小时布防  1--在家布防 2--外出布防  0xff--撤防状态
  RUN_DEV_ITEM itemAry[32];//数组长度可变 数目devCnt  最多32个
} SCENE_EVENT;

typedef struct {
  uint16_t state;          //输出模块的通道状态
  uint8_t devUnitID[12];  //输出模块的cpuID
} CHNS_STATUS;

//安防信息,每一个防区对应一个结构体，IP端可以查询\修改
typedef struct {
  uint8_t secName[12];//防区名称
  uint8_t secCode[12];//红外探测器无线信号编码
  uint8_t secId;//防区Id，唯一标识这个防区
  uint8_t secType;//布防类型  0--24小时布防  1--在家布防 2--外出布防 0xff--撤防状态
  uint8_t sceneId;//防区触发情景模式事件id  不触发则为0xff
  uint8_t secDev;//防区是否触发设备动作  0--不触发  1--触发
  uint8_t itemCnt;//
  uint8_t rev1;//
  uint8_t rev2;//
  uint8_t valid;//防区是否有效
  RUN_DEV_ITEM itemAry[4];//
} SEC_INFO;

typedef struct {
  uint8_t secStatus;//防区布撤防状态0--24小时布防  1--在家布防 2--外出布防  0xff--撤防状态
  uint8_t rev1;//
  uint8_t rev2;//
  uint8_t rev3;//
  uint32_t secDat;//防区报警字节  32bit 每一bit位对应一个防区  为0未报警 为1即报警
} SEC_ALRM;

typedef struct {
  uint8_t keyUId[12];  //按键板ID
  uint8_t keyIndex;    //按键板上的键值
  uint8_t eventId;     //事件id  用于在联网模块上唯一表示事件身份 id0、1的情景定义为在家模式与外出模式 且不可删除
  uint8_t valid;
  uint8_t rev3;
} KEY2SCENE_ITEM;
#define _KEY2SCENE_ITEM_MAX_          12

typedef struct {
  KEY2SCENE_ITEM items[_KEY2SCENE_ITEM_MAX_];
  uint8_t flg[4];
} KEY2SCENE;

typedef struct {
  uint8_t timSta[4];//开始时间 时分秒  timSta[3]字节表示星期的选择 bit0~6分别代表星期天~星期六
  uint8_t timEnd[4];//开始时间 时分秒  timEnd[3]字节表示是否重复定时操作    0否 1是
  uint8_t timerName[12];//名称
  uint8_t devCnt;//参与事件的设备数目  0xff表示全部设备参与
  uint8_t eventId;//事件id  用于在联网模块上唯一表示事件身份
  uint8_t valid;//用于表示该定时事件是否有效  0--无效  1--有效
  uint8_t rev3;
  RUN_DEV_ITEM item[4];//
} TIMER_EVENT;

typedef struct {
  uint8_t eventName[12];//名称
  uint8_t uidSrc[12];//环境参数数据来自哪个模块id	全0或者全0xff表示都有效
  uint8_t valid;//本环境触发选项是否有效   0--无效  1--有效
  uint8_t valTh;//触发阀值
  uint8_t thType;//触发方式  0--大于阀值触发    1--小于阀值触发
  uint8_t envType;//触发事件类型  0 温度触发  1湿度触发   2 pm2.5触发
  uint8_t devCnt;
  uint8_t eventId;
  uint16_t rev;
  RUN_DEV_ITEM itemAry[4];// 数目devCnt
} ENV_EVENT;

typedef struct {
  uint8_t triggerName[12];//防区名称
  uint32_t triggerSecs;//组合的防区 32bit 每一bit位对应一个防区  为0未检测到信号 为1即有信号
  uint8_t triggerId;//组合触发事件的ID，唯一标识
  uint8_t valid;//触发器是否启用
  uint8_t reportServ;//是否上报服务器  0--否  1--是
  uint8_t bRun;//是否已经被触发过
  uint8_t rev1;
  uint8_t rev2;
  uint8_t rev3;
  uint8_t itemCnt;//itemAry的有效个数
  RUN_DEV_ITEM itemAry[4];//
} SECS_TRIGGER;

#endif // UDPPROPKT_H
