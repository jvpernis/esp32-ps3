
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "include/ps3.h"
#include "include/ps3_int.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "stack/gap_api.h"
#include "stack/bt_types.h"
#include "stack/l2c_api.h"
#include "osi/allocator.h"

#define  PS3_TAG "PS3_L2CAP"


#define PS3_L2CAP_ID_HIDC 0x40
#define PS3_L2CAP_ID_HIDI 0x41


/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/

static void ps3_l2cap_init_service( char *name, uint16_t psm, uint8_t security_id);
static void ps3_l2cap_deinit_service( char *name, uint16_t psm );
static void ps3_l2cap_connect_ind_cback (BD_ADDR  bd_addr, uint16_t l2cap_cid, uint16_t psm, uint8_t l2cap_id);
static void ps3_l2cap_connect_cfm_cback (uint16_t l2cap_cid, uint16_t result);
static void ps3_l2cap_config_ind_cback (uint16_t l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void ps3_l2cap_config_cfm_cback (uint16_t l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void ps3_l2cap_disconnect_ind_cback (uint16_t l2cap_cid, bool ack_needed);
static void ps3_l2cap_disconnect_cfm_cback (uint16_t l2cap_cid, uint16_t result);
static void ps3_l2cap_data_ind_cback (uint16_t l2cap_cid, BT_HDR *p_msg);
static void ps3_l2cap_congest_cback (uint16_t cid, bool congested);


/********************************************************************************/
/*                         L O C A L    V A R I A B L E S                       */
/********************************************************************************/

static const tL2CAP_APPL_INFO dyn_info = {
    ps3_l2cap_connect_ind_cback,
    ps3_l2cap_connect_cfm_cback,
    NULL,
    ps3_l2cap_config_ind_cback,
    ps3_l2cap_config_cfm_cback,
    ps3_l2cap_disconnect_ind_cback,
    ps3_l2cap_disconnect_cfm_cback,
    NULL,
    ps3_l2cap_data_ind_cback,
    ps3_l2cap_congest_cback,
    NULL
} ;

static tL2CAP_CFG_INFO ps3_cfg_info;

bool is_connected = false;


/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S                        */
/********************************************************************************/

/*******************************************************************************
**
** Function         ps3_l2cap_init_services
**
** Description      This function initialises the required L2CAP services.
**
** Returns          void
**
*******************************************************************************/
void ps3_l2cap_init_services()
{
    ps3_l2cap_init_service( "PS3-HIDC", BT_PSM_HIDC, BTM_SEC_SERVICE_FIRST_EMPTY   );
    ps3_l2cap_init_service( "PS3-HIDI", BT_PSM_HIDI, BTM_SEC_SERVICE_FIRST_EMPTY+1 );
}

/*******************************************************************************
**
** Function         ps3_l2cap_deinit_services
**
** Description      This function deinitialises the required L2CAP services.
**
** Returns          void
**
*******************************************************************************/
void ps3_l2cap_deinit_services()
{
    ps3_l2cap_deinit_service( "PS3-HIDC", BT_PSM_HIDC );
    ps3_l2cap_deinit_service( "PS3-HIDI", BT_PSM_HIDI );
}


/*******************************************************************************
**
** Function         ps3_l2cap_send_hid
**
** Description      This function sends the HID command using the L2CAP service.
**
** Returns          void
**
*******************************************************************************/
void ps3_l2cap_send_hid( hid_cmd_t *hid_cmd, uint8_t len )
{
    uint8_t result;
    BT_HDR     *p_buf;

    p_buf = (BT_HDR *)osi_malloc(BT_DEFAULT_BUFFER_SIZE);

    if( !p_buf ){
        ESP_LOGE(PS3_TAG, "[%s] allocating buffer for sending the command failed", __func__);
    }

    p_buf->len = len + ( sizeof(*hid_cmd) - sizeof(hid_cmd->data) );
    p_buf->offset = L2CAP_MIN_OFFSET;

    memcpy ((uint8_t *)(p_buf + 1) + p_buf->offset, (uint8_t*)hid_cmd, p_buf->len);

    result = L2CA_DataWrite( PS3_L2CAP_ID_HIDC, p_buf );

    if (result == L2CAP_DW_SUCCESS)
        ESP_LOGI(PS3_TAG, "[%s] sending command: success", __func__);

    if (result == L2CAP_DW_CONGESTED)
        ESP_LOGW(PS3_TAG, "[%s] sending command: congested", __func__);

    if (result == L2CAP_DW_FAILED)
        ESP_LOGE(PS3_TAG, "[%s] sending command: failed", __func__);
}


/********************************************************************************/
/*                      L O C A L    F U N C T I O N S                          */
/********************************************************************************/

/*******************************************************************************
**
** Function         ps3_l2cap_init_service
**
** Description      This registers the specified bluetooth service in order
**                  to listen for incoming connections.
**
** Returns          void
**
*******************************************************************************/
static void ps3_l2cap_init_service( char *name, uint16_t psm, uint8_t security_id)
{
    /* Register the PSM for incoming connections */
    if (!L2CA_Register(psm, (tL2CAP_APPL_INFO *) &dyn_info)) {
        ESP_LOGE(PS3_TAG, "%s Registering service %s failed", __func__, name);
        return;
    }

    /* Register with the Security Manager for our specific security level (none) */
    if (!BTM_SetSecurityLevel (false, name, security_id, 0, psm, 0, 0)) {
        ESP_LOGE (PS3_TAG, "%s Registering security service %s failed", __func__, name);\
        return;
    }

    ESP_LOGI(PS3_TAG, "[%s] Service %s Initialized", __func__, name);
}

/*******************************************************************************
**
** Function         ps3_l2cap_deinit_service
**
** Description      This deregisters the specified bluetooth service.
**
** Returns          void
**
*******************************************************************************/
static void ps3_l2cap_deinit_service( char *name, uint16_t psm )
{
    /* Deregister the PSM from incoming connections */
    L2CA_Deregister(psm);
    ESP_LOGI(PS3_TAG, "[%s] Service %s Deinitialized", __func__, name);
}


/*******************************************************************************
**
** Function         ps3_l2cap_connect_ind_cback
**
** Description      This the L2CAP inbound connection indication callback function.
**
** Returns          void
**
*******************************************************************************/
static void ps3_l2cap_connect_ind_cback (BD_ADDR  bd_addr, uint16_t l2cap_cid, uint16_t psm, uint8_t l2cap_id)
{
    ESP_LOGI(PS3_TAG, "[%s] bd_addr: %s\n  l2cap_cid: 0x%02x\n  psm: %d\n  id: %d", __func__, bd_addr, l2cap_cid, psm, l2cap_id );

    /* Send connection pending response to the L2CAP layer. */
    L2CA_CONNECT_RSP (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_PENDING, L2CAP_CONN_PENDING, NULL, NULL);

    /* Send response to the L2CAP layer. */
    L2CA_CONNECT_RSP (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_OK, L2CAP_CONN_OK, NULL, NULL);

    /* Send a Configuration Request. */
    L2CA_CONFIG_REQ (l2cap_cid, &ps3_cfg_info);
}


/*******************************************************************************
**
** Function         ps3_l2cap_connect_cfm_cback
**
** Description      This is the L2CAP connect confirmation callback function.
**
**
** Returns          void
**
*******************************************************************************/
static void ps3_l2cap_connect_cfm_cback(uint16_t l2cap_cid, uint16_t result)
{
    ESP_LOGI(PS3_TAG, "[%s] l2cap_cid: 0x%02x\n  result: %d", __func__, l2cap_cid, result );
}


/*******************************************************************************
**
** Function         ps3_l2cap_config_cfm_cback
**
** Description      This is the L2CAP config confirmation callback function.
**
**
** Returns          void
**
*******************************************************************************/
void ps3_l2cap_config_cfm_cback(uint16_t l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    ESP_LOGI(PS3_TAG, "[%s] l2cap_cid: 0x%02x\n  p_cfg->result: %d", __func__, l2cap_cid, p_cfg->result );

    /* The PS3 controller is connected after    */
    /* receiving the second config confirmation */
    is_connected = l2cap_cid == PS3_L2CAP_ID_HIDI;

    if(is_connected){
        ps3Enable();
    }
}


/*******************************************************************************
**
** Function         ps3_l2cap_config_ind_cback
**
** Description      This is the L2CAP config indication callback function.
**
**
** Returns          void
**
*******************************************************************************/
void ps3_l2cap_config_ind_cback(uint16_t l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    ESP_LOGI(PS3_TAG, "[%s] l2cap_cid: 0x%02x\n  p_cfg->result: %d\n  p_cfg->mtu_present: %d\n  p_cfg->mtu: %d", __func__, l2cap_cid, p_cfg->result, p_cfg->mtu_present, p_cfg->mtu );

    p_cfg->result = L2CAP_CFG_OK;

    L2CA_ConfigRsp(l2cap_cid, p_cfg);
}


/*******************************************************************************
**
** Function         ps3_l2cap_disconnect_ind_cback
**
** Description      This is the L2CAP disconnect indication callback function.
**
**
** Returns          void
**
*******************************************************************************/
void ps3_l2cap_disconnect_ind_cback(uint16_t l2cap_cid, bool ack_needed)
{
    ESP_LOGI(PS3_TAG, "[%s] l2cap_cid: 0x%02x\n  ack_needed: %d", __func__, l2cap_cid, ack_needed );
}


/*******************************************************************************
**
** Function         ps3_l2cap_disconnect_cfm_cback
**
** Description      This is the L2CAP disconnect confirm callback function.
**
**
** Returns          void
**
*******************************************************************************/
static void ps3_l2cap_disconnect_cfm_cback(uint16_t l2cap_cid, uint16_t result)
{
    ESP_LOGI(PS3_TAG, "[%s] l2cap_cid: 0x%02x\n  result: %d", __func__, l2cap_cid, result );
}


/*******************************************************************************
**
** Function         ps3_l2cap_data_ind_cback
**
** Description      This is the L2CAP data indication callback function.
**
**
** Returns          void
**
*******************************************************************************/
static void ps3_l2cap_data_ind_cback(uint16_t l2cap_cid, BT_HDR *p_buf)
{
    if ( p_buf->len > 2 )
    {
        ps3_parse_packet( p_buf->data );
    }

    osi_free( p_buf );
}


/*******************************************************************************
**
** Function         ps3_l2cap_congest_cback
**
** Description      This is the L2CAP congestion callback function.
**
** Returns          void
**
*******************************************************************************/
static void ps3_l2cap_congest_cback (uint16_t l2cap_cid, bool congested)
{
    ESP_LOGI(PS3_TAG, "[%s] l2cap_cid: 0x%02x\n  congested: %d", __func__, l2cap_cid, congested );
}
