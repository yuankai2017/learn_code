#include "lte.h"
#include "types.h"

XSMART_ANT_API_T XSMART_ANT_API = {
    /* �ṩ��xml����Ĭ�ϵ�ant����*/
    smart_ant_config_default_work_mode,
    smart_ant_config_default_dl_ant,
    smart_ant_config_default_tdd_ul_ant,
    smart_ant_config_default_mask_ant,
    smart_ant_config_debug_flag,
    smart_ant_config_auto_idle_max_time_cnt,
    smart_ant_config_sample_max_time_cnt,
    smart_ant_config_wait_max_time_cnt,
    smart_ant_config_crc_rate_min_threshold,
    smart_ant_config_max_crc_rate_percent,
    smart_ant_config_low_crc_rate_cnt,
    smart_ant_config_max_over_threshold_cnt,

    /******************************************/

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};


char gLteInitFilename[32] = "/dev/shm/lte_init.xml";

//一.文件锁
/*1.
文件锁资源被占用，导致多线程（或多进程）之间程序卡死（程序在某个文件锁操作中卡死）
2.现象：程序A中有操作到文件锁F，程序B中也有操作到文件锁F,
如果文件锁资源分配不当就会导致         死锁
3.解决方法：给每个锁操作带上参数，如下：
*/
static char gParseLockPath[] = "/dev/shm/pa/common/parse_lock";

//上锁
static inline void PARSE_LOCK(int *pParseLockFd)
{
    *pParseLockFd = open(gParseLockPath, O_WRONLY);

    if(*pParseLockFd != -1)
        flock(*pParseLockFd, LOCK_EX);
    else
        printf("[waring]wcdma_control can't find the file \"/dev/shm/pa/common/parse_lock  !!\" \n");
}
//解锁
static inline void PARSE_UNLOCK(int *pParseLockFd)
{
    if(*pParseLockFd != -1)
    {
        flock(*pParseLockFd, LOCK_UN);
        close(*pParseLockFd);
    }
}



/*功能: 取出当前系统时间*/
char *timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    char localtime_str[32];
    static char timestamp_str[64];

    strftime(localtime_str, sizeof(localtime_str), "%T", localtime(&tv.tv_sec));
    sprintf(timestamp_str, "%s.%03ld", localtime_str, tv.tv_usec / 1000);

    return timestamp_str;
}

/**/
char *timeStr(uint32 time)
{
    time_t mytime = time;
    int year, mon, day, hour, min, sec;
    struct tm *result;
    static char timestamp_str[64];

	result = localtime(&mytime);

    year = result->tm_year + 1900;
    mon = result->tm_mon + 1;
    day = result->tm_mday;
    hour = result->tm_hour;
    min = result->tm_min;
	sec = result->tm_sec;

    sprintf(timestamp_str, "%02d:%02d:%02d", hour, min, sec);
	return timestamp_str;
}

/**/
char *secend2hms(uint64 secends)
{
    static char  hms_str[64];
    uint32 hour = 0;
    uint32 minute = 0;
    uint32 secend = 0;
    hour = secends / 3600;
    minute = (secends / 60) % 60;
    secend = secends % 60;

    if(hour != 0)
    {
        sprintf(hms_str, "%dh%dm%ds", hour, minute, secend);
    }
    else if(minute != 0)
    {
        sprintf(hms_str, "%dm%ds", minute, secend);
    }
    else
    {
        sprintf(hms_str, "%ds", secend);
    }

    return hms_str;
}

/**/
uint32 get_system_time()
{
    return time(NULL);
}

/**/
uint64 get_system_time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}



void hitPrintf(uint8 flag, char *fmt, ...)
{
    va_list argptr;
    char buffer[1024];
    unsigned int len = 0;
    va_start(argptr, fmt);
    len = vsnprintf(buffer, 1024 - 1, fmt, argptr);
    buffer[len + 1] = '\0';
    va_end(argptr);

    if(flag == HIT)
        printf("\033[0;32m%s\033[0m", buffer);
    else
        printf("%s", buffer);
}

void greenPrintf(char *fmt, ...)
{
    va_list argptr;
    char buffer[1024];
    unsigned int len = 0;
    va_start(argptr, fmt);
    len = vsnprintf(buffer, 1024 - 1, fmt, argptr);
    buffer[len + 1] = '\0';
    va_end(argptr);

    printf("\033[0;32m%s\033[0m", buffer);
}

void yellow_printf(char *fmt, ...)
{
    va_list argptr;
    char buffer[1024];
    unsigned int len = 0;
    va_start(argptr, fmt);
    len = vsnprintf(buffer, 1024 - 1, fmt, argptr);
    buffer[len + 1] = '\0';
    va_end(argptr);

    printf("\033[0;33m%s\033[0m", buffer);
}

void err_printf(char *fmt, ...)
{
    va_list argptr;
    char buffer[1024];
    unsigned int len = 0;

    va_start(argptr, fmt);
    len = vsnprintf(buffer, 1024 - 1, fmt, argptr);
    buffer[len + 1] = '\0';
    va_end(argptr);
    printf("\033[0;31m%s\033[0m", buffer);
}



void engPrintf(char *fmt, ...)
{
    va_list argptr;
    char buffer[1024];
    unsigned int len = 0;
    va_start(argptr, fmt);
    len = vsnprintf(buffer, 1024 - 1, fmt, argptr);
    buffer[len + 1] = '\0';
    va_end(argptr);
    printf("\033[0;31m%s\033[0m", buffer);
}


void configPrintf(char *fmt, ...)
{
    va_list argptr;
    char buffer[1024];
    unsigned int len = 0;
    va_start(argptr, fmt);
    len = vsnprintf(buffer, 1024 - 1, fmt, argptr);
    buffer[len + 1] = '\0';
    va_end(argptr);
    printf("\033[1;34m%s\033[0m", buffer);
}



void debugPrintData(uint8 *data, uint16 len)
{
    uint32 i = 0;

    printf("data:0x");

    for(i = 0; i < len; i++)
    {
        printf("%02x-", data[i]);

        if(31 == i % 32)
            printf("\n       ");
    }

    printf("\n");
}
int debug_print1(char *fmt, ...)
{
    static FILE *fd = NULL;
    static char printFileName[64] = "/dev/shm/debuginfo1.txt";
    static unsigned long fileLen = 0;
    struct stat statBuff;
    char temp_buf[1024] = {0};
    unsigned int len = 0;
	if(!(gPrintValue & 0x04))
    {
		return 0;
    }

    if(NULL == fd)
    {
        if(stat(printFileName, &statBuff) >= 0)
        {
            fileLen = statBuff.st_size;
        }

        if(fileLen > MAX_FILE_LEN / 4)
        {
            fd = fopen(printFileName, "w+");

            if(NULL == fd)
            {
                return -1;
            }

            fileLen = 0;
        }
        else
        {
            fd = fopen(printFileName, "a+");

            if(NULL == fd)
            {
                return -1;
            }
        }

        fprintf(fd, "[%s]----------------------------system reboot--------------------------------\n", timestamp());
    }

    if(fileLen > MAX_FILE_LEN / 4)
    {
        fileLen = 0;

        fclose(fd);

        fd = fopen(printFileName, "w+");

        if(NULL == fd)
        {
            return -1;
        }

        fprintf(fd, "[%s]----------------------------rewrite--------------------------------\n", timestamp());
    }

    va_list argp;
    va_start(argp, fmt);

    len += vsnprintf(temp_buf, 1024 - 1, fmt, argp);
    temp_buf[len + 1] = '\0';
    va_end(argp);

    if(len > 0)
    {
        if(len > 1 && (temp_buf[len - 1] == '\n' || temp_buf[len - 1] == '\r'))
        {
            fprintf(fd, "[%s]  %s", timestamp(), temp_buf);
        }
        else
        {
            fprintf(fd, "%s", temp_buf);
        }

        fileLen += len;
    }

    fflush(fd);

    return 0;
}

int smart_ant_config_default_work_mode(uint32 mode)
{
    gSmartAntXmlConfig.defaultWorkMode = mode;

    return 0;
}

int smart_ant_config_default_dl_ant(uint32 ant)
{
    gSmartAntXmlConfig.defaultDlAnt = ant;

    return 0;
}

int smart_ant_config_default_tdd_ul_ant(uint32 ant)
{
    gSmartAntXmlConfig.defaultTddUlAnt = ant;

    return 0;
}

int smart_ant_config_default_mask_ant(uint32 mask)
{
    gSmartAntXmlConfig.defaultMaskAnt = mask;

    return 0;
}

int smart_ant_config_debug_flag(uint32 debug)
{
    gSmartAntXmlConfig.debugFlag = debug;

    return 0;
}

int smart_ant_config_auto_idle_max_time_cnt(uint32 cnt)
{
    gSmartAntXmlConfig.autoIdleMaxTimeCnt = cnt / 10;

    return 0;
}

int smart_ant_config_sample_max_time_cnt(uint32 cnt)
{
    gSmartAntXmlConfig.sampleMaxTimeCnt = cnt / 10;

    return 0;
}

int smart_ant_config_wait_max_time_cnt(uint32 cnt)
{
    gSmartAntXmlConfig.waitMaxTimeCnt = cnt / 10;

    return 0;
}

int smart_ant_config_crc_rate_min_threshold(uint16 threshold)
{
    if (threshold > 100)
    {
        return -1;
    }

    gSmartAntXmlConfig.crcRateMinThreshold = threshold;

    return 0;
}

int smart_ant_config_max_crc_rate_percent(uint16 percent)
{
    if (percent > 100)
    {
        return -1;
    }

    gSmartAntXmlConfig.maxCrcRatePercent = percent;

    return 0;
}

int smart_ant_config_low_crc_rate_cnt(uint16 cnt)
{
    gSmartAntXmlConfig.lowCrcRateCnt = cnt;

    return 0;
}

int smart_ant_config_max_over_threshold_cnt(uint16 cnt)
{
    gSmartAntXmlConfig.maxOverThresholdCnt = cnt;

    return 0;
}



//三.xml文件
//(对于那些可能经常被改动的参数，可以xml文件的方式存放，程序只需初始化的时候读取xml文件即可)
int read_control_config_file(char *file)
{
    xmlDocPtr doc = NULL;
    xmlChar *content = NULL;
    xmlNodePtr rootNode = NULL, node1 = NULL, node2 = NULL, node3 = NULL, node4 = NULL, node5 = NULL;
    uint16 ctryIdx = 0, opIdx = 0, freqIdx = 0, mainNetWorkBandIdx = 0;
    int i, j, k;

    char filepath[32];
    char filename[32];
    uint32 sms_mode = 0;
    uint16 range = 0;

    memset(filepath, 0, sizeof(filepath));
    memset(filename, 0, sizeof(filename));
    memset(&gFreqAssign, 0, sizeof(gFreqAssign));

    doc = xmlReadFile(file, "UTF-8", XML_PARSE_RECOVER);
    if(doc == NULL)
    {
        printf("[Error] Read lte XML File Error.\n");
        return -1;
    }
    rootNode = xmlDocGetRootElement(doc);
    if(xmlStrcmp(rootNode->name, BAD_CAST "LTE"))
    {
        printf("[Error] Document Of The Wrong Type, Root Node != LTE.\n");
        xmlFreeDoc(doc);
        return -1;
    }
    node1 = rootNode->children;
    printf("=============[读取lte_init.xml文件成功]=============\n");
    while(node1 != NULL)
    {
        if(xmlStrstr(node1->name, BAD_CAST "CONFIG_FILE"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "FILE_PATH"))
                {
                    content = xmlNodeGetContent(node2);
                    strcpy(filepath, (char *)content);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "FILE_NAME"))
                {
                    content = xmlNodeGetContent(node2);
                    strcpy(filename, (char *)content);
                    xmlFree(content);
                }

                else
                    ;
                node2 = node2->next;
            }
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "WPPL_WORK_MODE"))
        {
            content = xmlNodeGetContent(node1);
            gWpplWorkMode = strtoul((char *)content, NULL, 10);
            xmlFree(content);
            print_info_2_file("gWpplWorkMode=%d\n", gWpplWorkMode);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "SUPPORT_SWITCH_ANT_RF_HW_VER"))
        {
            content = xmlNodeGetContent(node1);
            record_rf_hardware_version((const char*)content);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "USER_TIMING_SUPPORT"))
        {
            content = xmlNodeGetContent(node1);
            gForceTimingMode = strtoul((char *)content, NULL, 10);
            xmlFree(content);
            print_info_2_file("gForceTimingMode=%d\n", gForceTimingMode);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "USER_SWITCH_ANT"))
        {
            content = xmlNodeGetContent(node1);
            gForceSwitchAnt = strtoul((char *)content, NULL, 10);
            xmlFree(content);
            print_info_2_file("gForceSwitchAnt=%d\n", gForceSwitchAnt);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "INIT_INFO_CHECK"))
        {
            content = xmlNodeGetContent(node1);
            set_smart_info_check_flg_xml_config(strtoul((char *)content, NULL, 10));
            xmlFree(content);
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "ACTIVE_IDENTIFY"))
        {
        	node2 = node1->children;
            while(node2 != NULL)
            {
				if(!xmlStrcmp(node2->name, BAD_CAST "ACTIVE_IDENTIFY_FLAG"))
				{
					content = xmlNodeGetContent(node2);
					g_activeIdentifyFlag = strtoul((char *)content, NULL, 10);
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACTIVE_IDENTIFY_PAGING_NUM"))
				{
					content = xmlNodeGetContent(node2);
					g_activePagingNum = strtoul((char *)content, NULL, 10);
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACTIVE_IDENTIFY_RNTI_NUM"))
				{
					content = xmlNodeGetContent(node2);
					g_activeRntiNum = strtoul((char *)content, NULL, 10);
					xmlFree(content);
				}
				else
                    ;
                node2 = node2->next;
			}
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "ENG_EQU"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "WPPL"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(xmlStrEqual(node3->name, BAD_CAST "UL_SUB"))
                        {
                            content = xmlNodeGetContent(node3);
                            gUlEngFactor[WPPL].subFactor = atof((char *)content);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "UL_MULT"))
                        {
                            content = xmlNodeGetContent(node3);
                            gUlEngFactor[WPPL].multFactor= atof((char *)content);
                            xmlFree(content);
                        }
                        else
                        {
                            ;
                        }
                        node3 = node3->next;
                    }
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "WLA3B3100"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(xmlStrEqual(node3->name, BAD_CAST "UL_SUB"))
                        {
                            content = xmlNodeGetContent(node3);
                            gUlEngFactor[WLA3100].subFactor = atof((char *)content);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "UL_MULT"))
                        {
                            content = xmlNodeGetContent(node3);
                            gUlEngFactor[WLA3100].multFactor= atof((char *)content);
                            xmlFree(content);
                        }
                        else
                        {
                            ;
                        }
                        node3 = node3->next;
                    }
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "WLA3B3000"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(xmlStrEqual(node3->name, BAD_CAST "UL_SUB"))
                        {
                            content = xmlNodeGetContent(node3);
                            gUlEngFactor[WLA3000].subFactor = atof((char *)content);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "UL_MULT"))
                        {
                            content = xmlNodeGetContent(node3);
                            gUlEngFactor[WLA3000].multFactor= atof((char *)content);
                            xmlFree(content);
                        }
                        else
                        {
                            ;
                        }
                        node3 = node3->next;
                    }
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "UL_CIC_TRUNC"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaTrunc.ulCic = strtoul((char *)content, NULL, 16);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "UL_FIR_TRUNC"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaTrunc.ulFir = strtoul((char *)content, NULL, 16);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_CIC_TRUNC"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaTrunc.dlCic = strtoul((char *)content, NULL, 16);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_FIR_TRUNC"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaTrunc.dlFir = strtoul((char *)content, NULL, 16);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_STANDARD_ADJUST"))
                {
                    content = xmlNodeGetContent(node2);
                    gDlEngAdjust.standardAdjust = strtol((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_MS_ADJUST"))
                {
                    content = xmlNodeGetContent(node2);
                    gDlEngAdjust.msAdjust = strtol((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_THRESHOULD_LO"))
                {
                    content = xmlNodeGetContent(node2);
                    gDlEngAdjust.thresholdLow = strtod((char *)content, NULL);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_THRESHOULD_HI"))
                {
                    content = xmlNodeGetContent(node2);
                    gDlEngAdjust.thresholdHi = strtod((char *)content, NULL);
                    xmlFree(content);
                }
                else
                    ;
                node2 = node2->next;
            }
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "SCHEDULE_TIME_GAP"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "SCHEDULE_FREQ_TIME_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    gScheduleTimeGap.schedule_freq_time_gap = strtoul((char *)content, NULL, 10);
                    smart_sche_config_max_scan_freq_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                if(!xmlStrcmp(node2->name, BAD_CAST "SCHEDULE_FREQ_SHORT_TIME_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    gScheduleTimeGap.schedule_freq_short_time_gap = strtoul((char *)content, NULL, 10);
                    smart_sche_config_max_scan_freq_short_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "SCHEDULE_CHNL_TIME_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    gScheduleTimeGap.schedule_chnl_time_gap = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "SCHEDULE_UL_INVALID_TIME_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sche_config_max_ul_invalid_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "RNTI_ENABLE_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    gScheduleTimeGap.rnti_enable_gap = strtoul((char *)content, NULL, 10);
                    gScheduleTimeGapMCRD.rnti_enable_gap = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else
                    ;
                node2 = node2->next;
            }
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "FPGA_IRQ"))
        {
            content = xmlNodeGetContent(node1);
            gFPGA_IRQ = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        /**********************Report system status************************/
        else  if(xmlStrstr(node1->name, BAD_CAST "REPORT_SYS_STATE"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "REPORT_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    sys_state_config_send_flag((uint8)strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "REPORT_INTERVAL"))
                {
                    content = xmlNodeGetContent(node2);
                    sys_state_config_send_interval((uint8)strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "CONFIG_FILENAME"))
                {
                    content = xmlNodeGetContent(node2);
                    sys_state_config_file_name((char *)content, sizeof((char *)content));
                    xmlFree(content);
                }
                else
                    ;
                node2 = node2->next;
            }
        }
        /***********************Report system status***********************/
        /*********************SMART_WEAKEN***********************/
        else if(xmlStrstr(node1->name, BAD_CAST "SMART_WEAKEN"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT_WORK_MODE"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_default_work_mode(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_AUTO_WEAKEN_SWITCH"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_dl_auto_weaken_switch(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "UL_AUTO_WEAKEN_SWITCH"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_ul_auto_weaken_switch(strtoul((char *)content, NULL, 10));
					if(auto_dl_weaken_judgement())
					{
						smart_weaken_config_ul_auto_weaken_switch(0);
					}
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "FPGA_WORK_MODE"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_fpga_work_mode(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DEBUG_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_debug_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "UL_WEAKEN_UPPER"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_ul_weaken_upper( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "UL_WEAKEN_LOWER"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_ul_weaken_lower( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_WEAKEN_UPPER"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_dl_weaken_upper( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DL_WEAKEN_LOWER"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_dl_weaken_lower( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "PULL_UP_UL_STEP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_pull_up_ul_weaken_step( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "PULL_DOWN_UL_STEP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_pull_down_ul_weaken_step( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "PULL_UP_DL_STEP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_pull_up_dl_weaken_step( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "PULL_DOWN_DL_STEP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_pull_down_dl_weaken_step( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "SET_WEAKEN_INTERVAL"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_set_weaken_interval( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ENG_STABLY_TIME"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_eng_stably_time( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "MIN_SAMPLE_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_set_min_sample_cnt( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "MIN_SAMPLE_CNT2"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_set_min_sample_cnt2( strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "UL_WEAKEN_EFFECTIVE_TIME"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_ul_weaken_effective_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "PULL_DOWN_UL_WEAKEN_ON_TIME_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_pull_down_ul_weaken_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "MAX_SAMPLE_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_set_max_sample_interval(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "MIN_JUDGE_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_set_min_judge_interval(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "DL_ENG_STABLY_TIME"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_weaken_config_set_dl_eng_stably_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "DL_WEAKEN_PARAM"))
				{
                	node3 = node2->children;
					while(node3 != NULL)
					{
						StrSmartWeakenDlBandParam param;
						memset(&param, 0, sizeof(StrSmartWeakenDlBandParam));
						if(xmlStrstr(node3->name, BAD_CAST "TYPE"))
						{
							node4 = node3->children;
							while(node4 != NULL)
							{
								if(!xmlStrcmp(node4->name, BAD_CAST "BAND_TYPE"))
								{
									content = xmlNodeGetContent(node4);
									param.bandType = strtoul((char *)content, NULL, 10);
									xmlFree(content);
								}
								else if(!xmlStrcmp(node4->name, BAD_CAST "OFFSET_VALUE"))
								{
									content = xmlNodeGetContent(node4);
									param.offset = strtol((char *)content, NULL, 10);
									xmlFree(content);
								}
								else if(!xmlStrcmp(node4->name, BAD_CAST "DL_WEAKEN_UPPER"))
								{
									content = xmlNodeGetContent(node4);
									param.dlWeakenUpper = -strtoul((char *)content, NULL, 10);
									xmlFree(content);
								}
								else if(!xmlStrcmp(node4->name, BAD_CAST "DL_WEAKEN_LOWER"))
								{
									content = xmlNodeGetContent(node4);
									param.dlWeakenLower = -strtoul((char *)content, NULL, 10);
									xmlFree(content);
								}
								else
									;
								node4 = node4->next;
							}
							smart_weaken_config_dl_weaken_band_param(&param);
						}
						node3 = node3->next;
					}
                }
                else
                    ;

                node2 = node2->next;
            }
        }
        /*********************SMART_WEAKEN***********************/
        
        else if(!xmlStrcmp(node1->name, BAD_CAST "ADC_MASK_ENABLE"))
        {
            content = xmlNodeGetContent(node1);
            gAdcMaskEnable = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "ADC_MASK"))
        {
            content = xmlNodeGetContent(node1);
            gAdcMask = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }

        else if(!xmlStrcmp(node1->name, BAD_CAST "STATISTIC_MODE"))
        {
            content = xmlNodeGetContent(node1);
            gStatisticMode = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "TMSI_MASK"))/* 移动、电信Mask */
        {
            content = xmlNodeGetContent(node1);
            gTmsiMask = strtoul((char *)content, NULL, 16);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "TMSI_MASK_1"))/* 联通Mask */
        {
            content = xmlNodeGetContent(node1);
            gTmsiMask_1 = strtoul((char *)content, NULL, 16);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "TMSI_MASK_2"))/* 联通补充Mask */
        {
            content = xmlNodeGetContent(node1);
            gTmsiMask_2 = strtoul((char *)content, NULL, 16);
            xmlFree(content);
        }

        /* RNTI Table 1参数 */
        else if(!xmlStrcmp(node1->name, BAD_CAST "MAX_RNTI_TMSI_LEN"))
        {
            content = xmlNodeGetContent(node1);
            MAX_RNTI_TMSI_LEN = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "RNTI_TMSI_OVER_TIME_FRAME_NO"))
        {
            content = xmlNodeGetContent(node1);
            RNTI_TMSI_OVER_TIME_FRAME_NO = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        /* RNTI Table 2 参数 */
        else if(!xmlStrcmp(node1->name, BAD_CAST "MAX_RNTI_TABLE_LEN"))
        {
            content = xmlNodeGetContent(node1);
            MAX_RNTI_TABLE_LEN = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "RNTI_OVER_TIME_FRAME_NO"))
        {
            content = xmlNodeGetContent(node1);
            RNTI_OVER_TIME_FRAME_NO = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "RNTI_CNT_GAP"))
        {
            content = xmlNodeGetContent(node1);
            RNTI_CNT_GAP = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "RNTI_CNT_THRESHOLD"))
        {
            content = xmlNodeGetContent(node1);
            RNTI_CNT_THRESHOLD = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
  		/***************************SMART_ENG************************************/
		else if(!xmlStrcmp(node1->name, BAD_CAST "SMART_ENG"))
        { 
            /* 能量配置 */
            node2 = node1->children;
			while(node2 != NULL)
            {
				if(!xmlStrcmp(node2->name, BAD_CAST "DEBUG_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_eng_set_debug_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
		        else if(!xmlStrcmp(node2->name, BAD_CAST "ENG_REF_PERCENT"))
		        {
		            content = xmlNodeGetContent(node2);
					smart_eng_set_min_ref_eng_persent(atof((char *)content));
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "MIN_ENG_CNT"))
                {
                    content = xmlNodeGetContent(node2);
					smart_eng_set_max_valid_eng_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
		        else if(!xmlStrcmp(node2->name, BAD_CAST "POS_OK_PERCENT"))
		        {
		            content = xmlNodeGetContent(node2);
					smart_eng_set_min_pos_ok_persent(atof((char *)content));
		            xmlFree(content);
		        }
		        else if(!xmlStrcmp(node2->name, BAD_CAST "POS_WRONG_ENG_CNT"))
		        {
		            content = xmlNodeGetContent(node2);
		            smart_eng_set_min_pos_error_cnt(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
                else if(!xmlStrcmp(node2->name, BAD_CAST "TIME_M"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_eng_set_max_sample_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                /* 等待转身的时间 */
                else if(!xmlStrcmp(node2->name, BAD_CAST "TIME_N"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_eng_set_max_wait_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
		        else if(!xmlStrcmp(node2->name, BAD_CAST "DELTA_T"))
		        {
		            content = xmlNodeGetContent(node2);
		            gDeltaT = strtoul((char *)content, NULL, 10);
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "SINR_X"))
		        {
		            content = xmlNodeGetContent(node2);
					smart_eng_set_min_sinr(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
			   	else if(!xmlStrcmp(node2->name, BAD_CAST "ENG_REF_TTL"))
		        {
		            content = xmlNodeGetContent(node2);
		            smart_eng_set_max_ttl_time(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACC_ENG_COMPENSATE"))
		        {
		            content = xmlNodeGetContent(node2);
		            smart_eng_set_acc_eng_compensate(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACC_ENG_REPORT_DELAY_TIME"))
		        {
		            content = xmlNodeGetContent(node2);
		            smart_eng_set_acc_eng_report_delay_time(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACC_ENG_REPORT_MODE"))
		        {
		            content = xmlNodeGetContent(node2);
		            smart_eng_set_acc_eng_report_mode(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACC_ENG_VALID_TIME"))
		        {
		            content = xmlNodeGetContent(node2);
		            gAccEngValidTime = strtoul((char *)content, NULL, 10);
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACC_ENG_THROW_TIME"))
		        {
		            content = xmlNodeGetContent(node2);
		            gAccEngThrowTime = strtoul((char *)content, NULL, 10);
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "ENG_FILTER_TYPE"))
		        {
		            content = xmlNodeGetContent(node2);
					smart_eng_set_eng_filter_type(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
				else if(!xmlStrcmp(node2->name, BAD_CAST "ACC_ENG_POS_FILTER_FLG"))
		        {
		            content = xmlNodeGetContent(node2);
		            smart_eng_set_rach_eng_pos_filter_flg(strtoul((char *)content, NULL, 10));
		            xmlFree(content);
		        }
		        node2 = node2->next;
            }
		}
        // 普通短信匹配长度参数
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_001"))
        {
            content = xmlNodeGetContent(node1);
            gSmsRlcLen = get_mode_from_xml((char *)content);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_002"))
        {
            content = xmlNodeGetContent(node1);
            gSmsRlcLen_ack = get_mode_from_xml((char *)content);
            xmlFree(content);
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_003"))
        {
            content = xmlNodeGetContent(node1);
            gVolteCall = get_mode_from_xml((char *)content);
			if(0 == gVolteCall)
			{
				gVolteCall = gVolteCall_default;
			}
            xmlFree(content);
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_004"))
        {
            content = xmlNodeGetContent(node1);
            g_Range_vt[1][0] = get_mode_from_xml((char *)content);
            if(0 == g_Range_vt[1][0])
            {
				g_Range_vt[1][0] = g_Range_vt_default[1][0];
			}
            xmlFree(content);
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_005"))
        {
            content = xmlNodeGetContent(node1);
            g_Range_vt[0][1] = get_mode_from_xml((char *)content);
            if(0 == g_Range_vt[0][1])
            {
				g_Range_vt[0][1] = g_Range_vt_default[0][1];
			}
            xmlFree(content);
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_006"))
        {
            content = xmlNodeGetContent(node1);
            g_Range_vt[0][0] = get_mode_from_xml((char *)content);
			g_Range_vt[1][1] = g_Range_vt[0][0];
            if(0 == g_Range_vt[0][0])
            {
				g_Range_vt[0][0] = g_Range_vt_default[0][0];
				g_Range_vt[1][1] = g_Range_vt_default[1][1];
			}
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_007")) //防误匹配开关
        {
            content = xmlNodeGetContent(node1);
            gLCID2_MANUAL = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_008"))
        {
            content = xmlNodeGetContent(node1);
            g_oneUE_or_allUE = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_009"))
        {
            content = xmlNodeGetContent(node1);
            g_Difficulty = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_010"))
        {
            content = xmlNodeGetContent(node1);
            algorithm.volte_sms_interval = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_011"))
        {
            content = xmlNodeGetContent(node1);
            algorithm.volte_call_interval = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_012"))
        {
            content = xmlNodeGetContent(node1);
            stdMeetAccess.maxVarThre = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_013"))
		{
			content = xmlNodeGetContent(node1);
            algorithm.algorithm_auto_flag = strtoul((char *)content, NULL, 10);
            xmlFree(content);
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_014"))
		{
			content = xmlNodeGetContent(node1);
            algorithm.featurex_detal = get_mode_from_xml((char *)content);
			if(0 == algorithm.featurex_detal)
            {
				algorithm.featurex_detal = 150;
			}
            xmlFree(content);
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_015"))
		{
			content = xmlNodeGetContent(node1);
            algorithm.featurex_timeout = get_mode_from_xml((char *)content);
			if(0 == algorithm.featurex_timeout)
            {
				algorithm.featurex_detal = FEATURE_TIMEOUT;
			}
            xmlFree(content);
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_016"))
		{
			content = xmlNodeGetContent(node1);
            algorithm.volte_sms_interval_group = strtoul((char *)content, NULL, 10);
            xmlFree(content);
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "PARAMETER_017"))
		{
			content = xmlNodeGetContent(node1);
            gflag_open_neighbour_earfcn = strtoul((char *)content, NULL, 10);
            xmlFree(content);
		}
        // Add by xn @2016-3-10 VoLTE SMS 参数
        else if(!xmlStrcmp(node1->name, BAD_CAST "LTE_PARAMETER"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER"))
                {
                    if(NUM_OBJ_LCID <= g_lcid_num)
                    {
                        break;
                    }
                    else
                    {
                        content = xmlNodeGetContent(node2);
                        gFeatureXml[g_lcid_num].obj_lcid = strtoul((char *)content, NULL, 10);
                        xmlFree(content);
                        g_lcid_num++;
                    }
                }
                else
                    ;
                node2 = node2->next;
            }
        }
        // Add by xn @2016-9-10 VoLTE ALGORITHM 参数
        else if(!xmlStrcmp(node1->name, BAD_CAST "ALG_SETUP"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "ALG_OR"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_origin.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_origin.alg_th = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }
                }
                if(!xmlStrcmp(node2->name, BAD_CAST "ALG_A"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_a.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_a.alg_th = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ALG_B"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_b.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_b.alg_th = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ALG_C"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_c.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_c.alg_th = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }

                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ALG_D"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_d.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_d.alg_th = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }

                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ALG_E"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_e.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_e.alg_th = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }

                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ALG_F"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_call.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_call.alg_th = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }

                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ALG_G"))
                {
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(!xmlStrcmp(node3->name, BAD_CAST "ON_OFF"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_access.on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "ALGTH"))
                        {
                            content = xmlNodeGetContent(node3);
                            stdMeetAccess.aveframeSubThre = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "FILTER1"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_access.filter1_on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(!xmlStrcmp(node3->name, BAD_CAST "FILTER2"))
                        {
                            content = xmlNodeGetContent(node3);
                            algorithm.alg_access.filter2_on_off = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else
                            ;
                        node3 = node3->next;
                    }

                }
                else
                    ;
                node2 = node2->next;
            }
        }
        /* 短信匹配长度参数 */
        else if(!xmlStrcmp(node1->name, BAD_CAST "SMS_MODE"))
        {
            content = xmlNodeGetContent(node1);
            sms_mode = strtoul((char *)content, NULL, 16);
            xmlFree(content);
        }
        /* 短信模式探测半径 */
        else if(!xmlStrcmp(node1->name, BAD_CAST "SMS_MODE_RANGE"))
        {
            content = xmlNodeGetContent(node1);
            range = strtoul((char *)content, NULL, 10);
            xmlFree(content);
        }
        else if(xmlStrstr(node1->name, BAD_CAST "SMART_SYNC"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "GPS_DEBUG_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_debug_gps_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "GPS_SYNC_MIN_TIME"))
                {
                    content = xmlNodeGetContent(node2);
					smart_sync_config_set_gps_sync_min_time(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "SET_TARINFO_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_set_tar_info_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "BTS_CALIBRATE_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_set_bts_calibrate_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "SET_REF_BTS_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_set_ref_bts_Interval(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "BTS_MIN_UPDATE_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_set_bts_min_update_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "BTS_PARSE_STATUS"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_set_bts_parse_status(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "MAX_CHIPOFFSET_PER_SEC"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_max_chipoffset_per_sec(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "REF_CHIP_UPDATE_GAP"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_ref_chip_update_gap(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "TAR_BTS_CALIBRATE_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    smart_sync_config_tar_bts_calibrate_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                node2 = node2->next;
            }
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "CHNL_TIMEOUT"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "TARGET_LOSE"))
                {
                    content = xmlNodeGetContent(node2);

                    gChnlTimeout.exchangeTimeout = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "TARGET_CHNL_DESTRUCTION"))
                {
                    content = xmlNodeGetContent(node2);
                    gChnlTimeout.tarDesTimeout = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "MAX_PHASE_UNFIX_TIMES"))
                {
                    content = xmlNodeGetContent(node2);
                    gChnlTimeout.maxPhaseUnfixCnt = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "MAX_PHASE_UNFIX_THRESHOLD"))
                {
                    content = xmlNodeGetContent(node2);
                    gChnlTimeout.maxPhaseUnfixThershold = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else
                    ;
                node2 = node2->next;
            }
        }
#if 0
        else if(!xmlStrcmp(node1->name, BAD_CAST "CHNL_PDCCH_PARSE_MODE"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT"))
                {
                    content = xmlNodeGetContent(node2);
                    gDefaultChnlParsePdcchMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gDefaultChnlParsePdcchMode=%d\n", gDefaultChnlParsePdcchMode);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gHitChnlParsePdcchMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gHitChnlParsePdcchMode=%d\n", gHitChnlParsePdcchMode);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "BROTHER_HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gBrotherHitParsePdcchMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gBrotherHitParsePdcchMode=%d\n", gBrotherHitParsePdcchMode);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "LOW_ENERGY"))
                {
                    content = xmlNodeGetContent(node2);
                    gLowEnergyParsePdcchMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gLowEnergyParsePdcchMode=%d\n", gLowEnergyParsePdcchMode);
                }
                node2 = node2->next;
            }
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "CHNL_PDSCH_PARSE_MODE"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT"))
                {
                    content = xmlNodeGetContent(node2);
                    gDefaultChnlParsePdschMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gDefaultChnlParsePdschMode=%d\n", gDefaultChnlParsePdschMode);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gHitChnlParsePdschMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gHitChnlParsePdschMode=%d\n", gHitChnlParsePdschMode);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "BROTHER_HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gBrotherHitParsePdschMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gBrotherHitParsePdschMode=%d\n", gBrotherHitParsePdschMode);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "LOW_ENERGY"))
                {
                    content = xmlNodeGetContent(node2);
                    gLowEnergyParsePdschMode = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gLowEnergyParsePdschMode=%d\n", gLowEnergyParsePdschMode);
                }

                node2 = node2->next;
            }
        }
#else
        else if(!xmlStrcmp(node1->name, BAD_CAST "CHNL_PDCCH_PARSE_MODE_1"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT"))
                {
                    content = xmlNodeGetContent(node2);
                    gDefaultChnlParsePdcchMode[0] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gDefaultChnlParsePdcchMode[0] =%d\n", gDefaultChnlParsePdcchMode[0]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gHitChnlParsePdcchMode[0] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gHitChnlParsePdcchMode[0]=%d\n", gHitChnlParsePdcchMode[0]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "BROTHER_HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gBrotherHitParsePdcchMode[0] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gBrotherHitParsePdcchMode[0]=%d\n", gBrotherHitParsePdcchMode[0]);
                }
                node2 = node2->next;
            }
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "CHNL_PDSCH_PARSE_MODE_1"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT"))
                {
                    content = xmlNodeGetContent(node2);
                    gDefaultChnlParsePdschMode[0] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gDefaultChnlParsePdschMode[0]=%d\n", gDefaultChnlParsePdschMode[0]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gHitChnlParsePdschMode[0] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gHitChnlParsePdschMode[0]=%d\n", gHitChnlParsePdschMode[0]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "BROTHER_HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gBrotherHitParsePdschMode[0] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gBrotherHitParsePdschMode[0]=%d\n", gBrotherHitParsePdschMode[0]);
                }
                node2 = node2->next;
            }
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "CHNL_PDCCH_PARSE_MODE_2"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT"))
                {
                    content = xmlNodeGetContent(node2);
                    gDefaultChnlParsePdcchMode[1] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gDefaultChnlParsePdcchMode[1]=%d\n", gDefaultChnlParsePdcchMode[1]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gHitChnlParsePdcchMode[1] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gHitChnlParsePdcchMode[1]=%d\n", gHitChnlParsePdcchMode[1]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "BROTHER_HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gBrotherHitParsePdcchMode[1] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gBrotherHitParsePdcchMode[1]=%d\n", gBrotherHitParsePdcchMode[1]);
                }
                node2 = node2->next;
            }
        }
		else if(!xmlStrcmp(node1->name, BAD_CAST "FPGA_REGISTER_MONITOR"))
		{
			node2 = node1->children;
			while(node2 != NULL)
			{
				if(!xmlStrcmp(node2->name, BAD_CAST "MONITOR_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.monitorFlag = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("monitorFlag-%d\n",gFpgaRegMonInfo.monitorFlag);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "MONITOR_INTERVAL_TIME"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.monitorIntervalTime = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("monitorIntervalTime-%d\n",gFpgaRegMonInfo.monitorIntervalTime);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "CHANNEL_EFFECTIVE_LENGTH"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.channelEffectiveLen = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("channelEffectiveLen-%d\n",gFpgaRegMonInfo.channelEffectiveLen);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "CHNLINVALIDRATE"))
				{
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.chnlInvalidRate = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("chnlInvalidRate-%d\n",gFpgaRegMonInfo.chnlInvalidRate);
                }	
				else if(!xmlStrcmp(node2->name, BAD_CAST "REGISTER_DST"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.registerDstId = strtoul((char *)content, NULL, 16);
                    xmlFree(content);
					debug_print_mib_sib_register_state("registerDstId-%x\n",gFpgaRegMonInfo.registerDstId);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "MONITOR_REG_JUDGE_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.regJudgeFlag = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("regJudgeFlag-%d\n",gFpgaRegMonInfo.regJudgeFlag);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "CHANNEL_MIB_OK_STANDART"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.mibOkStand = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("mibOkStand-%d\n",gFpgaRegMonInfo.mibOkStand);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "CHANNEL_SIB_OK_STANDART"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.sibOkStand = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("sibOkStand-%d\n",gFpgaRegMonInfo.sibOkStand);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "CHANNEL_CNT_OFFSET"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.chnlCntOffset = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("chnlCntOffset-%d\n",gFpgaRegMonInfo.chnlCntOffset);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "RESET_MONITOR_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.resetMonitorCnt = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("resetMonitorCnt-%d\n",gFpgaRegMonInfo.resetMonitorCnt);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "CHANNEL_RESET_OPEN_MIBOK_RATA"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaRegMonInfo.chnlReOpenMibRate = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
					debug_print_mib_sib_register_state("chnlReOpenMibRate-%d\n",gFpgaRegMonInfo.chnlReOpenMibRate);
                }
				node2 = node2->next;
			}
		}
		else if(xmlStrstr(node1->name, BAD_CAST "RESET_FPGA_CONFIG"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "RESET_FPGA_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    gResetFpgaFlag = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                if(!xmlStrcmp(node2->name, BAD_CAST "RESET_FPGA_TIMEOUT"))
                {
                    content = xmlNodeGetContent(node2);
                    gFpgaTimeOut = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else
                    ;

                node2 = node2->next;
            }
        }
        else if(!xmlStrcmp(node1->name, BAD_CAST "CHNL_PDSCH_PARSE_MODE_2"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT"))
                {
                    content = xmlNodeGetContent(node2);
                    gDefaultChnlParsePdschMode[1] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gDefaultChnlParsePdschMode[1]=%d\n", gDefaultChnlParsePdschMode[1]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gHitChnlParsePdschMode[1] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gHitChnlParsePdschMode[1]=%d\n", gHitChnlParsePdschMode[1]);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "BROTHER_HIT_TAR"))
                {
                    content = xmlNodeGetContent(node2);
                    gBrotherHitParsePdschMode[1] = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                    print_info_2_file("gBrotherHitParsePdschMode[1]=%d\n", gBrotherHitParsePdschMode[1]);
                }
                node2 = node2->next;
            }
        }
#endif
        else if(xmlStrstr(node1->name, BAD_CAST "BOARD_CONFIG"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "BOARD_A_CHANNL_NUM"))
                {
                    content = xmlNodeGetContent(node2);
                    mainBoardChanlNum = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                if(!xmlStrcmp(node2->name, BAD_CAST "BOARD_B_CHANNL_NUM"))
                {
                    content = xmlNodeGetContent(node2);
                    fromBoardChanlNum = strtoul((char *)content, NULL, 10);
                    xmlFree(content);
                }
                else
                    ;

                node2 = node2->next;
            }
        }
        else if(xmlStrstr(node1->name, BAD_CAST "SMART_ANT"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT_WORK_MODE"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_default_work_mode != NULL)
                        XSMART_ANT_API.config_default_work_mode(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT_DL_ANT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_default_dl_ant != NULL)
                        XSMART_ANT_API.config_default_dl_ant(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT_TDD_UL_ANT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_default_tdd_ul_ant != NULL)
                        XSMART_ANT_API.config_default_tdd_ul_ant(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DEFAULT_MASK_ANT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_default_mask_ant != NULL)
                        XSMART_ANT_API.config_default_mask_ant(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "DEBUG_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_debug_flag != NULL)
                        XSMART_ANT_API.config_debug_flag(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "AUTO_IDLE_MAX_TIME_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_auto_idle_max_time_cnt != NULL)
                        XSMART_ANT_API.config_auto_idle_max_time_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "SAMPLE_MAX_TIME_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_sample_max_time_cnt != NULL)
                        XSMART_ANT_API.config_sample_max_time_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "WAIT_MAX_TIME_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_wait_max_time_cnt != NULL)
                        XSMART_ANT_API.config_wait_max_time_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "CRC_RATE_MIN_THRESHOLD"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_crc_rate_min_threshold != NULL)
                        XSMART_ANT_API.config_crc_rate_min_threshold(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "MAX_CRC_RATE_PERCENT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_max_crc_rate_percent != NULL)
                        XSMART_ANT_API.config_max_crc_rate_percent(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "LOW_CRC_RATE_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_low_crc_rate_cnt != NULL)
                        XSMART_ANT_API.config_low_crc_rate_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "MAX_OVER_THRESHOLD_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    if(XSMART_ANT_API.config_max_over_threshold_cnt != NULL)
                        XSMART_ANT_API.config_max_over_threshold_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else
                    ;

                node2 = node2->next;
            }
        }
        else if(xmlStrstr(node1->name, BAD_CAST "AUTO_ADJUST_PHASE"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(!xmlStrcmp(node2->name, BAD_CAST "DISCRETE_SAMPLE_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_discrete_sample_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "SAMPLE_TIME_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_sample_time_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "CHANGE_VOLT_WAIT_TIME_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_change_volt_wait_time_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "VOLT_CHANGE_DEGREE"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_volt_change_degree(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                else if(!xmlStrcmp(node2->name, BAD_CAST "ADJUST_MAX_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_adjust_max_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "SWITCH_FLAG"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_adjust_switch(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "DELAY_TIME_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_delay_time_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "SAMPLE_PHASE_CNT"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_sample_phase_cnt(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "SAVE_VOLT_SWITCH"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_save_volt_switch(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "SAVE_FILE_SWITCH"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_save_file_switch(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
				else if(!xmlStrcmp(node2->name, BAD_CAST "MIB_OK_RATE"))
                {
                    content = xmlNodeGetContent(node2);
                    aap_config_mib_ok_rate(strtoul((char *)content, NULL, 10));
                    xmlFree(content);
                }
                node2 = node2->next;
            }
        }

		else if(!xmlStrcmp(node1->name, BAD_CAST "CHECK_FPGA_ID"))
		{
			node2 = node1->children;
			while(node2 != NULL)
			{
				if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER1"))
				{
					content = xmlNodeGetContent(node2);
					gDebugFpgaIdFlg = strtoul((char *)content, NULL, 10);
					xmlFree(content);
				}
				node2 = node2->next;
			}
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "SIB_DELAY_TIME"))
		{
			content = xmlNodeGetContent(node1);
			gSibDelayTime = strtoul((char *)content, NULL, 10);
			xmlFree(content);
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "SIB5_DELAY_TIME"))
		{
			content = xmlNodeGetContent(node1);
			gSib5DelayTime = strtoul((char *)content, NULL, 10);
			xmlFree(content);
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "AUTO_SCAN_FREQ"))
		{
			node2 = node1->children;
			while(node2 != NULL)
			{
				if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER1"))
				{
					content = xmlNodeGetContent(node2);
					gAutoScanFreqXmlConfig.switchFlg = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER2"))
				{
					content = xmlNodeGetContent(node2);
					gAutoScanFreqXmlConfig.reportPeriod = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER3"))
				{
					content = xmlNodeGetContent(node2);
					gAutoScanFreqXmlConfig.neighborFreqTimeout = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER4"))
				{
					content = xmlNodeGetContent(node2);
					gAutoScanFreqXmlConfig.defaultMode = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER5"))
				{
					content = xmlNodeGetContent(node2);
					gAutoScanFreqXmlConfig.defaultMixFlg = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER6"))
				{
					content = xmlNodeGetContent(node2);
					gAutoScanFreqXmlConfig.freqDereplicationSwtich = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "PARAMETER7"))
				{
					content = xmlNodeGetContent(node2);
					gAutoScanFreqXmlConfig.freqDereplicationOrder = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
				}
				node2 = node2->next;
			}
		}
		else if(!xmlStrcmp(node1->name, BAD_CAST "POS_ADJUST_PARAM"))
		{
			node2 = node1->children;
			while(node2 != NULL)
			{
				if(!xmlStrcmp(node2->name, BAD_CAST "REF_POS"))
				{
					content = xmlNodeGetContent(node2);
					gPosAdjustParam.refPos = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
					xlog_pos_adjust("refPos=%d \n",gPosAdjustParam.refPos);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "POS_ADJUST_DIFF_VALUE"))
				{
					content = xmlNodeGetContent(node2);
					gPosAdjustParam.posAdjustDiffTH = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
					xlog_pos_adjust("posAdjustDiffTH=%d \n",gPosAdjustParam.posAdjustDiffTH);
				}
				else if(!xmlStrcmp(node2->name, BAD_CAST "POS_ADJUST_SWITCH"))
				{
					content = xmlNodeGetContent(node2);
					gPosAdjustParam.posAdjustSwitch = (strtoul((char *)content, NULL, 10));
					xmlFree(content);
					xlog_pos_adjust("posAdjustSwitch=%d \n",gPosAdjustParam.posAdjustSwitch);
				}
				node2 = node2->next;
			}
		}
        else if(xmlStrstr(node1->name, BAD_CAST "FREQUENCE_ASSIGN"))
        {
            node2 = node1->children;
            while(node2 != NULL)
            {
                if(xmlStrstr(node2->name, BAD_CAST "COUNTRY_"))
                {
                    ctryIdx = gFreqAssign.countryCnt;
                    node3 = node2->children;
                    while(node3 != NULL)
                    {
                        if(xmlStrEqual(node3->name, BAD_CAST "NAME"))
                        {
                            content = xmlNodeGetContent(node3);
                            strcpy(gFreqAssign.countryCfg[ctryIdx].name, (char *)content);
                            xmlFree(content);
                        }
                        else if(xmlStrEqual(node3->name, BAD_CAST "MCC"))
                        {
                            content = xmlNodeGetContent(node3);
                            gFreqAssign.countryCfg[ctryIdx].MCC = strtoul((char *)content, NULL, 10);
                            xmlFree(content);
                        }
                        else if(xmlStrstr(node3->name, BAD_CAST "OPERATOR_"))
                        {
                            node4 = node3->children;
                            opIdx = gFreqAssign.countryCfg[ctryIdx].operatorCnt;
                            while(node4 != NULL)
                            {
                                if(xmlStrEqual(node4->name, BAD_CAST "NAME"))
                                {
                                    content = xmlNodeGetContent(node4);
                                    strcpy(gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].name, (char *)content);
                                    xmlFree(content);
                                }
                                else if(xmlStrEqual(node4->name, BAD_CAST "MNC"))
                                {
                                    content = xmlNodeGetContent(node4);
                                    gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].MNC = strtoul((char *)content, 
NULL, 10);
                                    xmlFree(content);
                                }
								else if (xmlStrEqual(node4->name, BAD_CAST "MAIN_NETWORK_BAND"))
								{
									content = xmlNodeGetContent(node4);
									mainNetWorkBandIdx = gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].mainNetWorkBandCnt;
									gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].mainNetWorkBand[mainNetWorkBandIdx] = strtoul(
										(char *)content,
										NULL,
										10);
									xmlFree(content);
									gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].mainNetWorkBandCnt++;
								}
                                else if(xmlStrstr(node4->name, BAD_CAST "FREQ_"))
                                {
                                    node5 = node4->children;
                                    freqIdx = gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqCnt;
                                    while(node5 != NULL)
                                    {
                                        if(xmlStrEqual(node5->name, BAD_CAST "ADDR"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].addr 
= strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "FREQ_NUM"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
freqNo = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "LTE_WORK_MODE"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
lteWorkMode = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        else if(xmlStrEqual(node5->name, BAD_CAST "BAND_NUM"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
BandNo = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        else if(xmlStrEqual(node5->name, BAD_CAST "FRQ_BAND_NUM"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
freqBandNo = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "LINK_NUM"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
linkNo = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "FPGA_LINK_DL"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
fpgaLinkDl = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "FPGA_LINK_DL_1"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
fpgaLinkDl_1 = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "FPGA_LINK_UL"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
fpgaLinkUl = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "FPGA_LINK_UL_1"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
fpgaLinkUl_1 = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "SWITCH_DL"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
switchDl = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        if(xmlStrEqual(node5->name, BAD_CAST "SWITCH_UL"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
switchUl = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }

                                        else if(xmlStrEqual(node5->name, BAD_CAST "MID_FREQ"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
midFreq = strtoul((char *)content, NULL, 10);
                                            xmlFree(content);
                                        }
                                        else if(xmlStrEqual(node5->name, BAD_CAST "DL_NCO"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
dlNco = strtoul((char *)content, NULL, 16);
                                            xmlFree(content);
                                        }
                                        else if(xmlStrEqual(node5->name, BAD_CAST "UL_NCO"))
                                        {
                                            content = xmlNodeGetContent(node5);
                                            gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqInfo[freqIdx].
ulNco = strtoul((char *)content, NULL, 16);
                                            xmlFree(content);
                                        }
                                        node5 = node5->next;
                                    }
                                    gFreqAssign.countryCfg[ctryIdx].operatorCfg[opIdx].freqCnt++;
                                }
                                node4 = node4->next;
                            }
                            gFreqAssign.countryCfg[ctryIdx].operatorCnt++;
                        }
                        node3 = node3->next;
                    }
                    gFreqAssign.countryCnt++;
                }
                node2 = node2->next;
            }
        }
        node1 = node1->next;
    }
    xmlFreeDoc(doc);

    g_lcid_num = (g_lcid_num <= NUM_OBJ_LCID) ? g_lcid_num : NUM_OBJ_LCID;

    if(0 == g_oneUE_or_allUE)
    {
        print_info_2_file("命中系数 %d,SMS间隔 %ds,CALL间隔 %ds,解析指定UE，需要界面手动设置m-
TMSI\n",
                          g_Difficulty, algorithm.volte_sms_interval, algorithm.volte_call_interval);
    }
    else
    {
        print_info_2_file("命中系数 %d,SMS间隔 %ds,CALL间隔 %ds,解析全部UE\n", g_Difficulty, algorithm.
volte_sms_interval, algorithm.volte_call_interval);
    }

    print_info_2_file("F1=0x%08x\n", sms_mode);
    //print_info_2_file("F1:%d\n",gSmsRlcLen);
    gSmsRlcLen -= 2;
	gSmsRlcLen_ack -= 2;
	gSmsRlcLen_default = gSmsRlcLen;
	gSmsRlcLen_ack_default = gSmsRlcLen_ack;
//  print_info_2_file("SMS Rlc Len:%d\n",gSmsRlcLen);

    print_info_2_file("FPGA IRQ = %d\n", gFPGA_IRQ);

    if(0 == gLCID2_MANUAL)
    {
        print_info_2_file("SMS: %d\n", gSmsRlcLen);
    }
    else
    {
        print_info_2_file("SMS: %d %d\n", gSmsRlcLen, gSmsRlcLen_ack);
    }


    print_info_2_file("[LTE Parameter]\n");
    for(i = 0; i < g_lcid_num; i++)
    {
        print_info_2_file("Parameter =0x%x\n", gFeatureXml[i].obj_lcid);
    }
    for(i = 0; i < 2; i++)
    {
        print_info_2_file("Parameter RA_RB = [%d,%d]\n", g_Range_vt[i][0], g_Range_vt[i][1]);
    }
    print_info_2_file("Parameter CALL = %d\n", gVolteCall);
    // add 2016-9-5 15:08:55
    f_print_alg_info();
    /* add in 2016-04-27 将时间换算成帧号*/
    gDeltaT *= 100;

    if(strlen(filepath) != 0 && strlen(filename) != 0)
        sprintf(gSaveConfigFile, "%s%s", filepath, filename);
    printf("SaveConfigFilePath: %s\n", gSaveConfigFile);

    printf("----------LTE Freq Config----------\n");
    for(i = 0; i < gFreqAssign.countryCnt; i++)
    {
        printf("country: %s  MCC:%d\n", gFreqAssign.countryCfg[i].name, gFreqAssign.countryCfg[i].MCC);
        for(j = 0; j < gFreqAssign.countryCfg[i].operatorCnt; j++)
        {
            printf("    operator: %s  MNC:%d\n", gFreqAssign.countryCfg[i].operatorCfg[j].name, gFreqAssign.countryCfg
[i].operatorCfg[j].MNC);
            for (k = 0; k < gFreqAssign.countryCfg[i].operatorCfg[j].mainNetWorkBandCnt; k++)
            {
                printf("            mainNetWorkBand: %d\n",
                    gFreqAssign.countryCfg[i].operatorCfg[j].mainNetWorkBand[k]);
            }
            for(k = 0; k < gFreqAssign.countryCfg[i].operatorCfg[j].freqCnt; k++)
            {
                printf("            FREQ_%d\n", k);
                printf("                FreqNo: %d\n", gFreqAssign.countryCfg[i].operatorCfg[j].freqInfo[k].freqNo);
//                printf("                BandId: %d\n", gFreqAssign.countryCfg[i].operatorCfg[j].freqInfo[k].BandNo);
//                printf("                MidFreq: %d\n", gFreqAssign.countryCfg[i].operatorCfg[j].freqInfo[k].midFreq
);
//                printf("                DlNco  : %08x\n", gFreqAssign.countryCfg[i].operatorCfg[j].freqInfo[k].dlNco
);
//                printf("                UlNco  : %08x\n", gFreqAssign.countryCfg[i].operatorCfg[j].freqInfo[k].ulNco
);
            }
            printf("\n");
        }
    }

    printf("        Board A IP:%s\n", mainBoardIp);
    printf("      Board A Port:%d\n", mainRxFromPort);
    printf("Board A Channl Num:%d\n", mainBoardChanlNum);
    printf("        Board B IP:%s\n", fromBoardIp);
    printf("      Board B Port:%d\n", fromRxMainPort);
    printf("Board B Channl Num:%d\n", fromBoardChanlNum);
	printf("gResetFpgaFlag:%d\n", gResetFpgaFlag);
	printf("gFpgaTimeOut:%d\n", gFpgaTimeOut);
    printf("=================================================\n");

    return 0;
}

void init_smart_ant_api(uint8 mode)
{
    XSMART_ANT_API.sample_add_value = NULL;
    XSMART_ANT_API.config = NULL;
    XSMART_ANT_API.process = NULL;
    XSMART_ANT_API.init = NULL;
    XSMART_ANT_API.ul_process = NULL;
    XSMART_ANT_API.dl_process = NULL;

    if (mode == 1)
    {
        XSMART_ANT_API.sample_add_value = smart_ant_sample_add_value;
        XSMART_ANT_API.config = smart_ant_config;
        XSMART_ANT_API.process = smart_ant_process;
        XSMART_ANT_API.init = smart_ant_init;
        XSMART_ANT_API.ul_process = smart_ant_ul_process;
        XSMART_ANT_API.dl_process = smart_ant_dl_process;
    }
}


