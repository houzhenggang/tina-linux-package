#include "bluetooth_socket.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

extern "C"
{
#include "bluetooth_interface.h"
#include "bluetooth_params.h"
}

c_bt::c_bt(){
    bt_wd[0] = '.';
    bt_wd[1] = '\0';
    bt_on_status = 0;
}

c_bt::~c_bt(){

}

int c_bt::bt_on(char *bt_addr){

    char cmd[512] = {0};
    struct bluetooth_params bt_params;

    printf("do bt cmd bt on bt_addr path %s\n", bt_addr);
    if(this->bt_on_status == 1){
        printf("bt already on\n");
        return 0;
    }

    /* get bt config */
    get_params_from_config("/etc/config/bluetooth", &bt_params);

    /* start bt server */
    snprintf(cmd, 512, "/etc/bluetooth/btenable.sh on %s", bt_params.device);
    system(cmd);
    usleep(1000000);

    /*start bluetooth app*/
    bluetooth_start(this,bt_addr);

    /*start app avk*/
    start_app_avk();

    this->bt_on_status = 1;
    return 0;
}

int c_bt::bt_off(){
    printf("do bt cmd bt off\n");

    if(this->bt_on_status == 0){
        printf("bt is not on\n");
        return 0;
    }

    /*stop app avk*/
    stop_app_avk();

    /* stop bluetooth app */
    bluetooth_stop();
    usleep(1000*1000);

    /* stop bt server */
    system("/etc/bluetooth/btenable.sh off");
    usleep(500*1000);
    this->bt_on_status = 0;
    return 0;
}

int c_bt::set_bt_name(const char *name)
{
    if(!name || !name[0]){
        printf("Error: set bt name is NULL!\n");
        return -1;
    }

    printf("do bt cmd set bt name %s\n", name);

    s_set_bt_name(name);
    return 0;
}

int c_bt::set_dev_discoverable(int enable)
{
    printf("do bt cmd set dev discoverable %d\n", enable);
    s_set_discoverable(enable);
    return 0;
}

int c_bt::set_dev_connectable(int enable)
{
    printf("do bt cmd set dev connectable %d\n", enable);
    s_set_connectable(enable);
    return 0;
}

int c_bt::start_discovery(int time)
{
    printf("do bt cmd start discovery\n");
    s_start_discovery(time);
    return 0;
}

int c_bt::get_disc_results(char *disc_results, int *len)
{
    printf("do bt cmd get discover results\n");
    return s_get_disc_results(disc_results, len);
}

int c_bt::connect_auto()
{
    printf("do bt cmd connect_auto\n");

    return s_connect_auto();
}

int c_bt::disconnect()
{
    printf("do bt cmd disconnect\n");
    s_disconnect();
    return 0;
}

int c_bt::reset_avk_status()
{
    printf("do bt cmd reset avk status\n");
    if(this->bt_on_status == 0){
        printf("bt status is off\n");
        return 0;
    }

    /* stop app avk */
    stop_app_avk();
    usleep(200*1000);

    /* start app avk */
    start_app_avk();
    usleep(200*1000);
    return 0;
}

int c_bt::avk_play()
{
    printf("do bt cmd avk play\n");

    s_avk_play();
    return 0;
}

int c_bt::avk_pause()
{
    printf("do bt cmd avk pause\n");
    s_avk_pause();
    return 0;
}

int c_bt::avk_previous()
{
    printf("do bt cmd avk previous\n");
    s_avk_play_previous();
    return 0;
}

int c_bt::avk_next()
{
    printf("do bt cmd avk next\n");
    s_avk_play_next();
    return 0;
}

int c_bt::hs_pick_up()
{
    s_hs_pick_up();
    return 0;
}

int c_bt::hs_hung_up()
{
    s_hs_hung_up();
    return 0;
}

void c_bt::set_callback(tBtCallback *pCb)
{
    pBtCb = pCb;
}

void c_bt::event_callback(BT_EVENT bt_event)
{
    printf("received bt event 0x%x\n", bt_event);
    if(pBtCb){
        pBtCb(bt_event);
    }
}

void c_bt::do_test()
{
    ;
}

extern "C" void bt_event_transact(void *p, APP_BT_EVENT event, char *reply, int *len)
{
    c_bt *p_c_bt = (c_bt *)p;
    switch(event)
    {
          case APP_AVK_CONNECTED_EVT:
          {
              p_c_bt->event_callback(BT_AVK_CONNECTED_EVT);
              break;
          }

          case APP_AVK_DISCONNECTED_EVT:
          {
              p_c_bt->event_callback(BT_AVK_DISCONNECTED_EVT);
              break;
          }

        case APP_AVK_CONNECT_COMPLETED_EVT:
        {
            p_c_bt->event_callback(BT_AVK_CONNECT_COMPLETED_EVT);
            break;
        }

        case APP_AVK_START_EVT:
        {
            p_c_bt->event_callback(BT_AVK_START_EVT);
            break;
        }

        case APP_AVK_STOP_EVT:
        {
            p_c_bt->event_callback(BT_AVK_STOP_EVT);
            break;
        }

        default:
                ;
    }
}
