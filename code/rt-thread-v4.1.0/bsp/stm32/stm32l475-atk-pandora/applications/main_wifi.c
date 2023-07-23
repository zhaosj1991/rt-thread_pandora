/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "wifi_config.h"
#include "drv_wlan.h"
#include "msh.h"

#define DBG_LVL 3
#include "rtdbg.h"

#define WLAN_SSID "ChinaNet-1273"
#define WLAN_PASSWORD "3xc5z74v"
#define NET_READY_TIME_OUT (rt_tick_from_millisecond(15 * 1000))

static void print_wifi_info(struct rt_wlan_info* info);
static void print_scan_result(struct rt_wlan_scan_result *scan_result);

static struct rt_semaphore net_ready;

void wlan_ready_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    rt_sem_release(&net_ready);
}

/* 断开连接回调函数 */
void wlan_station_disconnect_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    LOG_I("disconnect from the network!");
}

int main(void)
{
    int ret = RT_EOK;
    struct rt_wlan_info info;
    struct rt_wlan_scan_result *scan_result;
    
    //wlan初始化，等待500ms
    rt_hw_wlan_wait_init_done(500);
    
    LOG_D("start scan...");

    scan_result = rt_wlan_scan_sync(); //同步扫描
    if (scan_result)
    {
        LOG_D("scan complete...");
        print_scan_result(scan_result);
        rt_wlan_scan_result_clean();
    }
    else
    {
        LOG_E("not found ap infomation");
        return -1;
    }

    LOG_D("connect to %s", WLAN_SSID);

    rt_sem_init(&net_ready, "net_ready", 0, RT_IPC_FLAG_FIFO);
    
    //注册wlan ready回调
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, wlan_ready_handler, RT_NULL);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_DISCONNECTED, 
        wlan_station_disconnect_handler, RT_NULL);
    
    rt_err_t res = rt_wlan_connect(WLAN_SSID, WLAN_PASSWORD);
    if (RT_EOK == res)
    {
        struct rt_wlan_info info;
        rt_memset(&info, 0, sizeof(info));
        rt_wlan_get_info(&info);
        ret = rt_sem_take(&net_ready, NET_READY_TIME_OUT);
        if (RT_EOK == ret)
        {
            LOG_D("network ready");
            msh_exec("ifconfig", rt_strlen("ifconfig"));
        }
        else
        {
            LOG_D("connect wifi timeout");
        }
    }
    else
    {
        LOG_E("connect failed..., errnum: %d", res);
    }

}

static void print_wifi_info(struct rt_wlan_info* info)
{
    LOG_D("SSID : %-.32s", &info->ssid.val[0]);
    LOG_D("MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x", info->bssid[0],
          info->bssid[1],
          info->bssid[2],
          info->bssid[3],
          info->bssid[4],
          info->bssid[5]);
    LOG_D("Channel: %d", info->channel);
    LOG_D("DataRate: %dMbps", info->datarate / 1000000);
    LOG_D("RSSI: %d", info->rssi);
}

static void print_scan_result(struct rt_wlan_scan_result *scan_result)
{
    char *security;
    int index, num;

    num = scan_result->num;
    /* 有规则的排列扫描到的热点 */
    rt_kprintf("             SSID                      MAC            security    rssi chn Mbps\n");
    rt_kprintf("------------------------------- -----------------  -------------- ---- --- ----\n");
    for (index = 0; index < num; index++)
    {
        rt_kprintf("%-32.32s", &scan_result->info[index].ssid.val[0]);
        rt_kprintf("%02x:%02x:%02x:%02x:%02x:%02x  ",
                   scan_result->info[index].bssid[0],
                   scan_result->info[index].bssid[1],
                   scan_result->info[index].bssid[2],
                   scan_result->info[index].bssid[3],
                   scan_result->info[index].bssid[4],
                   scan_result->info[index].bssid[5]);
        switch (scan_result->info[index].security)
        {
        case SECURITY_OPEN:
            security = "OPEN";
            break;
        case SECURITY_WEP_PSK:
            security = "WEP_PSK";
            break;
        case SECURITY_WEP_SHARED:
            security = "WEP_SHARED";
            break;
        case SECURITY_WPA_TKIP_PSK:
            security = "WPA_TKIP_PSK";
            break;
        case SECURITY_WPA_AES_PSK:
            security = "WPA_AES_PSK";
            break;
        case SECURITY_WPA2_AES_PSK:
            security = "WPA2_AES_PSK";
            break;
        case SECURITY_WPA2_TKIP_PSK:
            security = "WPA2_TKIP_PSK";
            break;
        case SECURITY_WPA2_MIXED_PSK:
            security = "WPA2_MIXED_PSK";
            break;
        case SECURITY_WPS_OPEN:
            security = "WPS_OPEN";
            break;
        case SECURITY_WPS_SECURE:
            security = "WPS_SECURE";
            break;
        default:
            security = "UNKNOWN";
            break;
        }
        rt_kprintf("%-14.14s ", security);
        rt_kprintf("%-4d ", scan_result->info[index].rssi);
        rt_kprintf("%3d ", scan_result->info[index].channel);
        rt_kprintf("%4d\n", scan_result->info[index].datarate / 1000000);
    }
    rt_kprintf("\n");
}
