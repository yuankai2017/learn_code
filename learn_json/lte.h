#ifndef __LTE_H__
#define __LTE_H__


//二.宏
//1.宏定义成各种小接口
#define HI_UINT8(a) (((a) >> 4) & 0x0F)      //获取1个byte的高4bits
#define LO_UINT8(a) ((a) & 0x0F)              //获取1个byte的低4bits
#define HI_UINT16(a) (((a) >> 8) & 0xFF)     //获取2个byte的高8bits
#define LO_UINT16(a) ((a) & 0xFF)             //获取2个byte的低8bits
#define HI_UINT32(a) (((a) >> 16) & 0xFFFF)  //获取4个byte的高16bits
#define LO_UINT32(a) ((a) & 0xFFFF)           //获取4个byte的低16bits

/*将两个4bits组拼成一个8位byte*/
#define BUILD_UINT8(loByte, hiByte) \
          ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

/*将2个byte组拼成一个16位的字节*/          
#define BUILD_UINT16(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

/*将4个byte组拼成一个32位的字节*/          
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3)  (uint32)((((Byte3) & 0x000000ff) << 24) | \
                                                           (((Byte2) & 0x00000000ff) << 16) | \
                                                           (((Byte1) & 0x00000000ff) << 8) | \
                                                           ((Byte0) & 0x00000000ff));
/**/
#define CHECK_SUM_UINT32(data)  (uint8)((((data) >> 24) & 0x00000000ff) + \
                                        (((data) >> 16) & 0x00000000ff) + \
                                        (((data) >> 8) & 0x00000000ff) + \
                                        ((data) & 0x00000000ff))

/**/
#define BUILD_UINT16_BIG(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
/**/
#define BUILD_UINT16_SMALL(hiByte, loByte) \
          ((uint16)(((hiByte) & 0x00FF) << 8)) + (((loByte) & 0x00FF))
          
/*将8个字节组拼成一个64位的byte*/
#define BUILD_UINT64(Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7) \
          ((uint64)((uint64)((Byte0) & 0x00FF) \
          + ((uint64)((Byte1) & 0x00FF) << 8) \
          + ((uint64)((Byte2) & 0x00FF) << 16) \
          + ((uint64)((Byte3) & 0x00FF) << 24) \
          + ((uint64)((Byte4) & 0x00FF) << 32) \
          + ((uint64)((Byte5) & 0x00FF) << 40) \
          + ((uint64)((Byte6) & 0x00FF) << 48) \
          + ((uint64)((Byte7) & 0x00FF) << 56)))

#define BUF_TO_UINT08(p)	((p)[0])
#define BUF_TO_UINT16_BIG(p)	BUILD_UINT16_BIG((p)[0], (p)[1])
#define BUF_TO_UINT16_SMALL(p)	BUILD_UINT16_SMALL((p)[0], (p)[1])

#define BUF_TO_UINT32(p)	BUILD_UINT32((p)[0], (p)[1], (p)[2], (p)[3])
#define BUF_TO_UINT48(p)	BUILD_UINT64((p)[0], (p)[1], (p)[2], (p)[3], (p)[4], (p)[5], 0, 0)
#define BUF_TO_UINT64(p)	BUILD_UINT64((p)[0], (p)[1], (p)[2], (p)[3], (p)[4], (p)[5], (p)[6], (p)[7])

#define BUF_TO_UINT08(p)	((p)[0])
#define BUF_TO_UINT16(p)	BUILD_UINT16((p)[0], (p)[1])
#define BUF_TO_UINT32(p)	BUILD_UINT32((p)[0], (p)[1], (p)[2], (p)[3])

#define get_smart_Byte(p)    ((p)[0])
#define get_smart_Word(p)    ((p)[1] | ((p)[0] << 8))
#define get_smart_DWord(p)   ((p)[3] | ((p)[2] << 8) | ((p)[1] << 16) | ((p)[0] << 24))

/*对16位的字节，高8位与低8位互换位置*/
#define  N2H16(p)  ((p & 0xFF00) >> 8)|((p & 0x00FF) << 8)
/*对32位的字节，4与1互换位置，2与3互换位置*/
#define  N2H32(p)  ((p & 0xFF000000) >> 24)|((p & 0x00FF0000) >> 8)|((p & 0x0000FF00) << 8)|((p & 0x000000FF) << 24);

/**/
#define  H2N16(p)  ((p & 0xFF00) >> 8)|((p & 0x00FF) << 8)
/**/
#define  H2N32(p)  ((p & 0xFF000000) >> 24)|((p & 0x00FF0000) >> 8)|((p & 0x0000FF00) << 8)|((p & 0x000000FF) << 24);

#define get_view_Byte(p)    ((p)[0])
#define get_view_Word(p)    ((p)[0] | ((p)[1] << 8))
#define get_view_DWord(p)   ((p)[0] | ((p)[1] << 8) | ((p)[2] << 16) | ((p)[3] << 24))



//2.宏定义成各种脚本文件等
/* PA path */
#define M_PA_CFG_COMMON_DIR 	            "/dev/shm/pa/common"

/* Out file Path*/
#define M_PA_OUT_FILE_PATH		            "/dev/shm/out/"

/* system init */
#define M_TRANSFER_PARSE   		            "/dev/shm/parse -l "
#define M_COMM_FAN_ON_PA 		            "set_fan_on.pa"
#define M_COMM_FAN_OFF_PA 		            "set_fan_off.pa"
#define M_COMM_WPPL_FAN_ON_PA 		        "set_fan_on_wppl.pa"
#define M_COMM_WPPL_FAN_OFF_PA 		        "set_fan_off_wppl.pa"
#define M_COMM_SHOW_ALL_STATUS	            "show_wppl_status.pa"
#define M_COMM_SHOW_ARM_TEMPER	            "show_arm_temper.pa"
#define M_SHOW_NDB_INFO			            "show_ndb_info.pa"
#define M_COMM_SET_SYSTEM_OFF	            "set_system_off.pa"
#define M_COMM_SET_RF_OFF                   "set_rf_down.pa"
#define M_COMM_SET_FPGA_RESET               "reset_fpga.pa"
#define M_COMM_READ_MCU_VERSION             "read_mcu_batt_version.pa"
/* pa out file*/
#define M_PA_SHOW_WPPL_OUT_FILE             "show_wppl_status.out"
#define M_SHOW_NDB_INFO_OUT 	            "show_ndb_info.out"
#define M_SHOW_ARM_TEMPER_OUT 	            "show_arm_temper.out"
#define M_SHOW_MCU_BATT_VERSION_OUT 	    "show_mcu_batt_version.out"

/*checksum*/
#define M_SET_PRODUCT_EEPROM_INFO           "set_product_eeprom_info.pa"
#define M_SHOW_PRODUCT_EEPROM_INFO          "show_product_eeprom_info.pa"
#define M_SET_PRODUCT_EEPROM_CHECKSUM       "set_product_eeprom_checksum"
#define M_SHOW_PRODUCT_EEPROM_CHECKSUM      "show_product_eeprom_checksum.pa"
#define M_SHOW_PRODUCT_EEPROM_CHECKSUM_OUT  "show_product_eeprom_checksum.out" 
#define M_SHOW_PRODUCT_EEPROM_INFO_OUT      "show_product_eeprom_info.out"

//3.对于收到包的类型也定义成宏（方便识别）
/* pkt from view */
#define CDMA1X_RX_VIEW_SET_MODE			    0x02//设置模式
#define CDMA1X_RX_VIEW_SET_FREQ			    0x03//设置频点
#define CDMA1X_RX_VIEW_SET_PCHNL			0x04//设置寻呼信道
#define CDMA1X_RX_VIEW_SET_IMSI_ESN			0x05//设置imsi或esn
#define CDMA1X_RX_VIEW_UNLOCK_IMSI_ESN		0x06//解锁imsi和esn
#define CDMA1X_RX_VIEW_SET_PN				0x07//配置PN
#define CDMA1X_RX_VIEW_SET_CTRY_OPER		0x08//配置国家运营商
#define CDMA1X_RX_VIEW_AUTO_WEAK_SWITCH     0xFD//自动衰减
#define CDMA1X_RX_VIEW_SET_WEAK     		0xFE//设置衰减
#define CDMA1X_RX_VIEW_QUERY_VERSION		0xFF//版本查询
/* end pkt 2 view */

//4.特定文件路径也可定义成宏
#define M_FLASH_COM_CFG_AREA                "/dev/shm/.comcfg"
#define M_COMCFG_PATH                       "/dev/mtd5"

//5.关键词定义成宏
#define EXTERN extern
#define PUBLIC extern 
#define PRIVATE static
#define CONST const

//6.与低层交互（io控制）
#define IOCTL_MSG( code ) _IOWR(FC_MAGIC, code, uint8)

typedef enum _DRIVER_MSGS2
{
    MSG_IOCTL_RW_SYNC,
    MSG_IOCTL_GET_PHY_ADDR,
    MSG_IOCTL_SHM_DONE
} DRIVER_MSGS2;

#define IOCTL_RW_SYNC       IOCTL_MSG(MSG_IOCTL_RW_SYNC)
#define IOCTL_GET_PHY_ADDR  IOCTL_MSG(MSG_IOCTL_GET_PHY_ADDR)
#define IOCTL_SHM_DONE      IOCTL_MSG(MSG_IOCTL_SHM_DONE)



//四.函数指针的妙用
typedef struct
{
    int (*config_default_work_mode)(uint32 mode);
    int (*config_default_dl_ant)(uint32 ant);
    int (*config_default_tdd_ul_ant)(uint32 ant);
    int (*config_default_mask_ant)(uint32 mask);
    int (*config_debug_flag)(uint32 debug);
    int (*config_auto_idle_max_time_cnt)(uint32 cnt);
    int (*config_sample_max_time_cnt)(uint32 cnt);
    int (*config_wait_max_time_cnt)(uint32 cnt);
    int (*config_crc_rate_min_threshold)(uint16 threshold);
    int (*config_max_crc_rate_percent)(uint16 percent);
    int (*config_low_crc_rate_cnt)(uint16 cnt);
    int (*config_max_over_threshold_cnt)(uint16 cnt);

    /*******************************************************/

    int (*sample_add_value)(uint16 chnlNo, uint16 crcOk, uint16 crcErr);
    int (*config)(PStrSmartAntConfig config);
    int (*process)(void);
    int (*init)(void);
    int (*ul_process)(uint8 group, uint8 externalTimingFlg);
    int (*dl_process)(uint8 channel, uint8 group);
}XSMART_ANT_API_T;

extern XSMART_ANT_API_T XSMART_ANT_API;


#endif




