// ===================================================================================
// Bluetooth LE Handler for CH573/CH571                                       * v1.0 *
// ===================================================================================
//
// Implements the unified BLE host API (ble_host.h) with the WCH BLE library:
// - GAP peripheral role (advertising, one connection, parameter handling)
// - GATT server with a dynamic attribute table (services, characteristics, CCCDs)
// - read/write callbacks, notifications with flow control
// - TMOS task for messages and periodic RF/LSI calibration
//
// References:
// -----------
// - WCH CH573 EVT: BLE examples (Peripheral, HAL) and CH57xBLE_LIB.h
// - WCH BLE Software Development Reference Manual
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include <string.h>
#include "ble_host.h"
#include "gpio.h"
#include "CH57xBLE_LIB.h"

// ===================================================================================
// Settings and Variables
// ===================================================================================
#define BLE_CONNECTIONS   1                   // number of peripheral connections
#define BLE_EVT_START     0x0001              // TMOS event: start GAP role
#define BLE_EVT_CALIB     0x0002              // TMOS event: calibration

// TX power in dBm -> library value
#if   BLE_TX_POWER <= -20
  #define BLE_TX_LEVEL    LL_TX_POWEER_MINUS_20_DBM
#elif BLE_TX_POWER <= -14
  #define BLE_TX_LEVEL    LL_TX_POWEER_MINUS_14_DBM
#elif BLE_TX_POWER <= -8
  #define BLE_TX_LEVEL    LL_TX_POWEER_MINUS_8_DBM
#elif BLE_TX_POWER <= -4
  #define BLE_TX_LEVEL    LL_TX_POWEER_MINUS_4_DBM
#elif BLE_TX_POWER <= 0
  #define BLE_TX_LEVEL    LL_TX_POWEER_0_DBM
#elif BLE_TX_POWER == 1
  #define BLE_TX_LEVEL    LL_TX_POWEER_1_DBM
#elif BLE_TX_POWER == 2
  #define BLE_TX_LEVEL    LL_TX_POWEER_2_DBM
#elif BLE_TX_POWER == 3
  #define BLE_TX_LEVEL    LL_TX_POWEER_3_DBM
#elif BLE_TX_POWER == 4
  #define BLE_TX_LEVEL    LL_TX_POWEER_4_DBM
#else
  #define BLE_TX_LEVEL    LL_TX_POWEER_5_DBM
#endif

#if (BLE_ATT_MTU < 23) || (BLE_ATT_MTU > 247)
  #error BLE_ATT_MTU must be 23..247!
#endif

// Attribute kinds
#define KIND_SERVICE      1
#define KIND_CHAR         2
#define KIND_VALUE        3
#define KIND_CCCD         4

// Additional information for each attribute of the GATT table
typedef struct {
  uint8_t         kind;                       // attribute kind (see above)
  uint8_t         props;                      // characteristic properties
  uint8_t         uuid[16];                   // UUID (16 or 128 bits, LSB first)
  gattAttrType_t  svc;                        // service UUID (for service attribute)
  uint8_t*        value;                      // value buffer
  uint16_t        len;                        // current length of value
  uint16_t        max;                        // size of value buffer
  BLE_WRITE_CB    cb;                         // write callback
  gattCharCfg_t   ccc[BLE_CONNECTIONS];       // client characteristic configuration
} BLE_ATTR;

static BLE_ATTR        BLE_info[BLE_ATTRIBUTES];  // attribute information
static gattAttribute_t BLE_attr[BLE_ATTRIBUTES];  // attribute table for WCH stack
static uint16_t        BLE_attrCount;             // number of attributes
static uint16_t        BLE_regCount;              // number of registered attributes

__attribute__((aligned(4))) static uint32_t BLE_mem[BLE_MEMHEAP_SIZE / 4];  // BLE heap

static uint8_t  BLE_taskID = INVALID_TASK_ID;     // TMOS task ID
static uint8_t  BLE_started;                      // GAP role started?
static uint8_t  BLE_advOn;                        // advertising wanted?
static uint8_t  BLE_advActive;                    // advertising currently running?
static uint8_t  BLE_advPending;                   // restart after advertising has ended?
static uint8_t  BLE_advType = 0xFF;               // advertising type
static uint16_t BLE_advIvl;                       // advertising interval (0.625ms)
static uint16_t BLE_conn = INVALID_CONNHANDLE;    // connection handle
static uint8_t  BLE_advData[31], BLE_srData[31];  // advertising data
static uint8_t  BLE_name[GAP_DEVICE_NAME_LEN + 1];// device name

// ===================================================================================
// Minimal C Library Functions (the templates are linked without libc)
// ===================================================================================
#define NOLIB __attribute__((weak, used, optimize("no-tree-loop-distribute-patterns")))

NOLIB void* memcpy(void* dst, const void* src, size_t n) {
  uint8_t* d = (uint8_t*)dst; const uint8_t* s = (const uint8_t*)src;
  while(n--) *d++ = *s++;
  return dst;
}

NOLIB void* memset(void* dst, int c, size_t n) {
  uint8_t* d = (uint8_t*)dst;
  while(n--) *d++ = (uint8_t)c;
  return dst;
}

NOLIB int memcmp(const void* a, const void* b, size_t n) {
  const uint8_t* x = (const uint8_t*)a; const uint8_t* y = (const uint8_t*)b;
  for(; n; n--, x++, y++) if(*x != *y) return *x - *y;
  return 0;
}

NOLIB size_t strlen(const char* s) {
  size_t n = 0;
  while(*s++) n++;
  return n;
}

NOLIB int strncmp(const char* a, const char* b, size_t n) {
  for(; n; n--, a++, b++) {
    if(*a != *b) return (uint8_t)*a - (uint8_t)*b;
    if(!*a) break;
  }
  return 0;
}

// ===================================================================================
// Interrupt Handlers (forwarded to the BLE library)
// ===================================================================================
void BB_IRQHandler(void) __attribute__((interrupt));
void BB_IRQHandler(void) {
  BB_IRQLibHandler();
}

void LLE_IRQHandler(void) __attribute__((interrupt));
void LLE_IRQHandler(void) {
  LLE_IRQLibHandler();
}

// ===================================================================================
// Callbacks for the BLE Library
// ===================================================================================

// Random seed: SysTick counter
static uint32_t BLE_srandCB(void) {
  return STK->CNTL;
}

// Raw value of temperature sensor (for RF calibration)
static uint16_t BLE_tempCB(void) {
  return ADC_read_TEMP_raw();
}

// Calibration of internal 32kHz oscillator
#if SYS_CLK32K == 0
static void BLE_calibrateCB(void) {
  LSI_calibrate();
  TMOS_Set32KTuneValue(LSI_tune, LSI_tune);
}
#endif

// ===================================================================================
// GATT Server Callbacks
// ===================================================================================

// Read attribute value
static uint8_t BLE_readCB(uint16_t connHandle, gattAttribute_t* pAttr, uint8_t* pValue,
                          uint16_t* pLen, uint16_t offset, uint16_t maxLen, uint8_t method) {
  BLE_ATTR* a = &BLE_info[pAttr - BLE_attr];
  uint16_t  n;
  (void)method;
  *pLen = 0;
  if(a->kind == KIND_CCCD) {                    // client characteristic configuration
    uint16_t v = GATTServApp_ReadCharCfg(connHandle, a->ccc);
    if(offset) return ATT_ERR_ATTR_NOT_LONG;
    pValue[0] = (uint8_t)v; pValue[1] = (uint8_t)(v >> 8);
    *pLen = 2;
    return SUCCESS;
  }
  if(a->kind != KIND_VALUE) return ATT_ERR_ATTR_NOT_FOUND;
  if(!(a->props & BLE_READ)) return ATT_ERR_READ_NOT_PERMITTED;
  if(offset > a->len) return ATT_ERR_INVALID_OFFSET;
  n = a->len - offset;
  if(n > maxLen) n = maxLen;
  if(n) memcpy(pValue, a->value + offset, n);
  *pLen = n;
  return SUCCESS;
}

// Write attribute value
static uint8_t BLE_writeCB(uint16_t connHandle, gattAttribute_t* pAttr, uint8_t* pValue,
                           uint16_t len, uint16_t offset, uint8_t method) {
  uint16_t  idx = pAttr - BLE_attr;
  BLE_ATTR* a = &BLE_info[idx];
  (void)method;
  if(a->kind == KIND_CCCD)                      // enable/disable notifications
    return GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset,
                                          GATT_CLIENT_CFG_NOTIFY);
  if(a->kind != KIND_VALUE) return ATT_ERR_ATTR_NOT_FOUND;
  if(!(a->props & (BLE_WRITE | BLE_WRITE_NORESP))) return ATT_ERR_WRITE_NOT_PERMITTED;
  if(offset) return ATT_ERR_ATTR_NOT_LONG;
  if(len > a->max) return ATT_ERR_INVALID_VALUE_SIZE;
  if(len && a->value) memcpy(a->value, pValue, len);
  a->len = len;
  if(a->cb) a->cb(idx + 1, a->value, len);      // call write callback
  return SUCCESS;
}

static gattServiceCBs_t BLE_serviceCBs = {BLE_readCB, BLE_writeCB, NULL};

// ===================================================================================
// GAP Role Callbacks and Event Handling
// ===================================================================================

// Optional user event function
__attribute__((weak)) void BLE_EVENT_callback(uint8_t event) {(void)event;}

// Reset client characteristic configurations (notifications off)
static void BLE_resetCCC(void) {
  for(uint16_t i=0; i<BLE_attrCount; i++) {
    if(BLE_info[i].kind == KIND_CCCD)
      GATTServApp_InitCharCfg(INVALID_CONNHANDLE, BLE_info[i].ccc);
  }
}

// Enable/disable advertising in the GAP role
static void BLE_advEnable(uint8_t on) {
  GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(on), &on);
}

// (Re)start advertising with current settings
static void BLE_advRestart(void) {
  if(!BLE_started || !BLE_advOn || (BLE_conn != INVALID_CONNHANDLE)) return;
  BLE_advEnable(TRUE);
}

// State changes of the GAP peripheral role
static void BLE_stateCB(gapRole_States_t newState, gapRoleEvent_t* pEvent) {
  switch(pEvent->gap.opcode) {
    case GAP_MAKE_DISCOVERABLE_DONE_EVENT:      // advertising has started
      BLE_advActive = 1;
      break;
    case GAP_END_DISCOVERABLE_DONE_EVENT:       // advertising has ended
      BLE_advActive = 0;
      if(BLE_advPending) {                      // restart with new settings
        BLE_advPending = 0;
        BLE_advRestart();
      }
      break;
    case GAP_LINK_ESTABLISHED_EVENT:
      BLE_advActive = 0;
      if(pEvent->gap.hdr.status != SUCCESS) {
        BLE_advRestart();
        break;
      }
      if(BLE_conn != INVALID_CONNHANDLE) {      // only one connection
        GAPRole_TerminateLink(pEvent->linkCmpl.connectionHandle);
        break;
      }
      BLE_conn = pEvent->linkCmpl.connectionHandle;
      BLE_EVENT_callback(BLE_EVENT_CONNECTED);
      break;
    case GAP_LINK_TERMINATED_EVENT:
      if(pEvent->linkTerminate.connectionHandle != BLE_conn) break;
      BLE_conn = INVALID_CONNHANDLE;
      BLE_resetCCC();
      BLE_EVENT_callback(BLE_EVENT_DISCONNECTED);
      BLE_advRestart();                         // continue advertising
      break;
    default:
      break;
  }
  (void)newState;
}

static gapRolesCBs_t BLE_roleCBs = {BLE_stateCB, NULL, NULL};
static gapBondCBs_t  BLE_bondCBs = {NULL, NULL};

// TMOS task: process messages and events
static uint16_t BLE_processEvent(uint8_t task_id, uint16_t events) {
  if(events & SYS_EVENT_MSG) {
    uint8_t* msg = tmos_msg_receive(task_id);
    if(msg) {
      gattMsgEvent_t* evt = (gattMsgEvent_t*)msg;
      if((evt->hdr.event == GATT_MSG_EVENT) && (evt->method == ATT_MTU_UPDATED_EVENT))
        BLE_EVENT_callback(BLE_EVENT_MTU_CHANGED);
      tmos_msg_deallocate(msg);
    }
    return events ^ SYS_EVENT_MSG;
  }
  if(events & BLE_EVT_START) {
    GAPRole_PeripheralStartDevice(BLE_taskID, &BLE_bondCBs, &BLE_roleCBs);
    return events ^ BLE_EVT_START;
  }
  if(events & BLE_EVT_CALIB) {
    #if BLE_CALIBRATION > 0
    BLE_RegInit();                              // calibrate RF
    #if SYS_CLK32K == 0
    BLE_calibrateCB();                          // calibrate LSI
    #endif
    tmos_start_task(BLE_taskID, BLE_EVT_CALIB, MS1_TO_SYSTEM_TIME(BLE_CALIBRATION));
    #endif
    return events ^ BLE_EVT_CALIB;
  }
  return 0;
}

// ===================================================================================
// Init Function
// ===================================================================================
uint8_t BLE_init(void) {
  bleConfig_t cfg;
  uint8_t mac[6];
  uint16_t appearance = BLE_APPEARANCE;

  // Check library version
  if(!tmos_memcmp(VER_LIB, VER_FILE, strlen(VER_FILE))) return 0;

  // Configure and init BLE library
  memset(&cfg, 0, sizeof(cfg));
  ESIG_getMAC(mac);                             // factory MAC address (LSB first)
  memcpy(cfg.MacAddr, mac, 6);
  cfg.MEMAddr       = (uint32_t)BLE_mem;
  cfg.MEMLen        = BLE_MEMHEAP_SIZE;
  cfg.BufMaxLen     = BLE_ATT_MTU + 4;
  cfg.BufNumber     = BLE_BUFF_NUM;
  cfg.TxNumEvent    = 1;
  cfg.TxPower       = BLE_TX_LEVEL;
  cfg.ConnectNumber = BLE_CONNECTIONS;          // peripheral only
  cfg.srandCB       = BLE_srandCB;
  cfg.tsCB          = BLE_tempCB;
  #if SYS_CLK32K > 0
  cfg.SelRTCClock   = 0;                        // LSE 32768Hz
  #else
  cfg.SelRTCClock   = 2;                        // LSI calibrated to 32768Hz
  cfg.rcCB          = BLE_calibrateCB;
  #endif
  if(BLE_LibInit(&cfg)) return 0;

  // Init 32kHz clock and TMOS timer
  CLK32K_init();
  #if SYS_CLK32K == 0
  TMOS_Set32KTuneValue(LSI_tune, LSI_tune);
  #endif
  TMOS_TimerInit(0);

  // Init GAP peripheral role and TMOS task
  GAPRole_PeripheralInit();
  BLE_taskID = TMOS_ProcessEventRegister(BLE_processEvent);
  #if BLE_CALIBRATION > 0
  tmos_start_task(BLE_taskID, BLE_EVT_CALIB, MS1_TO_SYSTEM_TIME(BLE_CALIBRATION));
  #endif

  // GAP parameters
  {
    uint8_t  off  = FALSE;
    uint16_t cmin = BLE_CONN_INT_MIN, cmax = BLE_CONN_INT_MAX;
    uint8_t  pair = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8_t  io   = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(off), &off);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(cmin), &cmin);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(cmax), &cmax);
    GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(pair), &pair);
    GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(io), &io);
    GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(off), &off);
    GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(off), &off);
  }

  // GAP and GATT services, device name and appearance
  GGS_AddService(GATT_ALL_SERVICES);
  GATTServApp_AddService(GATT_ALL_SERVICES);
  memset(BLE_name, 0, sizeof(BLE_name));
  for(uint8_t i=0; (i < GAP_DEVICE_NAME_LEN) && BLE_NAME[i]; i++) BLE_name[i] = BLE_NAME[i];
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, strlen((char*)BLE_name), BLE_name);
  GGS_SetParameter(GGS_APPEARANCE_ATT, sizeof(appearance), &appearance);

  BLE_attrCount = 0;
  BLE_regCount  = 0;
  BLE_started   = 0;
  BLE_conn      = INVALID_CONNHANDLE;
  BLE_poll();
  return 1;
}

// ===================================================================================
// General Functions
// ===================================================================================

// Process events and data (call regularly!)
void BLE_poll(void) {
  TMOS_SystemProcess();
}

// Check if a central is connected
uint8_t BLE_connected(void) {
  return BLE_conn != INVALID_CONNHANDLE;
}

// Terminate connection
void BLE_disconnect(void) {
  if(BLE_conn != INVALID_CONNHANDLE) GAPRole_TerminateLink(BLE_conn);
}

// Get negotiated ATT MTU
uint16_t BLE_getMTU(void) {
  uint16_t mtu;
  if(BLE_conn == INVALID_CONNHANDLE) return ATT_MTU_SIZE;
  mtu = ATT_GetMTU(BLE_conn);
  if(mtu < ATT_MTU_SIZE) mtu = ATT_MTU_SIZE;
  if(mtu > BLE_ATT_MTU)  mtu = BLE_ATT_MTU;
  return mtu;
}

// Get Bluetooth device address (MSB first)
void BLE_getMAC(uint8_t* mac) {
  uint8_t m[6];
  ESIG_getMAC(m);
  for(uint8_t i=0; i<6; i++) mac[i] = m[5 - i];
}

// Milliseconds (TMOS system clock in units of 625us)
uint32_t BLE_millis(void) {
  return (uint32_t)(((uint64_t)TMOS_GetSystemClock() * SYSTEM_TIME_MICROSEN) / 1000);
}

// Wait n milliseconds, BLE stack keeps running
void BLE_delay(uint32_t ms) {
  while(ms--) {
    uint32_t start = STK_getTicks();
    while((STK_getTicks() - start) < DLY_MS_TIME) BLE_poll();
  }
}

// 32-bit pseudo random number
uint32_t BLE_random(void) {
  return tmos_rand();
}

// ===================================================================================
// Advertising Functions
// ===================================================================================

// Register GATT services and start GAP role (once, at first advertising)
static void BLE_start(void) {
  uint16_t start = BLE_regCount;
  if(BLE_started) return;
  for(uint16_t i=BLE_regCount; i<=BLE_attrCount; i++) {
    if((i == BLE_attrCount) || ((BLE_info[i].kind == KIND_SERVICE) && (i > start))) {
      if(i > start) GATTServApp_RegisterService(&BLE_attr[start], i - start,
                                                GATT_MAX_ENCRYPT_KEY_SIZE, &BLE_serviceCBs);
      start = i;
    }
  }
  BLE_regCount = BLE_attrCount;
  BLE_started  = 1;
  tmos_set_event(BLE_taskID, BLE_EVT_START);
}

// Start advertising with given data, interval (ms) and type
uint8_t BLE_advertise(const uint8_t* adv, uint8_t len, const uint8_t* sr, uint8_t srlen,
                      uint16_t interval, uint8_t type) {
  uint16_t ivl;
  if(BLE_taskID == INVALID_TASK_ID) return 0;
  if(len   > 31) len   = 31;
  if(srlen > 31) srlen = 31;
  if(adv) memcpy(BLE_advData, adv, len);
  if(sr)  memcpy(BLE_srData,  sr,  srlen);
  ivl = (uint32_t)interval * 8 / 5;             // ms -> units of 0.625ms
  if(ivl < 32)     ivl = 32;                    // 20ms minimum
  if(ivl > 0x4000) ivl = 0x4000;                // 10.24s maximum

  // Advertising and scan response data (updated immediately if advertising)
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, len, BLE_advData);
  GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, srlen, BLE_srData);

  // Type and interval (take effect when advertising is (re)started)
  if((type != BLE_advType) || (ivl != BLE_advIvl) || !BLE_advOn) {
    BLE_advType = type;
    BLE_advIvl  = ivl;
    GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(BLE_advType), &BLE_advType);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, ivl);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, ivl);
    BLE_advOn = 1;
    if(!BLE_started) {                          // first start: with GAP role
      BLE_advEnable(TRUE);
      BLE_start();
    }
    else if(BLE_advActive) {                    // running: stop and restart later
      BLE_advPending = 1;
      BLE_advEnable(FALSE);
    }
    else BLE_advRestart();
  }
  BLE_poll();
  return 1;
}

// Start connectable advertising with flags, name and 128-bit service UUID
uint8_t BLE_advertiseName(const char* name, const uint8_t* uuid128) {
  uint8_t adv[31], sr[31], n = 0, s = 0;
  uint8_t nl = strlen(name);
  memset(BLE_name, 0, sizeof(BLE_name));
  for(uint8_t i=0; (i < GAP_DEVICE_NAME_LEN) && name[i]; i++) BLE_name[i] = name[i];
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, strlen((char*)BLE_name), BLE_name);
  adv[n++] = 2; adv[n++] = 0x01; adv[n++] = 0x06;   // flags: general, no BR/EDR
  if(uuid128) {
    sr[s++] = 17; sr[s++] = 0x07;                   // complete 128-bit UUID list
    memcpy(sr + s, uuid128, 16); s += 16;
  }
  if(nl > 31 - n - 2) nl = 31 - n - 2;
  adv[n++] = nl + 1; adv[n++] = 0x09;               // complete local name
  memcpy(adv + n, name, nl); n += nl;
  return BLE_advertise(adv, n, sr, s, 100, BLE_ADV_CONNECTABLE);
}

// Stop advertising
void BLE_advStop(void) {
  BLE_advOn      = 0;
  BLE_advPending = 0;
  if(BLE_started) BLE_advEnable(FALSE);
  BLE_poll();
}

// ===================================================================================
// GATT Server Functions
// ===================================================================================

// Add attribute to table
static uint16_t BLE_addAttr(uint8_t kind, const uint8_t* uuid, uint8_t ulen) {
  BLE_ATTR*        a;
  gattAttribute_t* t;
  if(BLE_started || (BLE_attrCount >= BLE_ATTRIBUTES)) return 0;
  a = &BLE_info[BLE_attrCount];
  t = &BLE_attr[BLE_attrCount];
  memset(a, 0, sizeof(BLE_ATTR));
  a->kind = kind;
  if(uuid) memcpy(a->uuid, uuid, ulen);
  t->handle = 0;
  switch(kind) {
    case KIND_SERVICE:                          // primary service declaration
      a->svc.len    = ulen;
      a->svc.uuid   = a->uuid;
      t->type.len   = ATT_BT_UUID_SIZE;
      t->type.uuid  = primaryServiceUUID;
      t->permissions= GATT_PERMIT_READ;
      t->pValue     = (uint8_t*)&a->svc;
      break;
    case KIND_CHAR:                             // characteristic declaration
      t->type.len   = ATT_BT_UUID_SIZE;
      t->type.uuid  = characterUUID;
      t->permissions= GATT_PERMIT_READ;
      t->pValue     = &a->props;
      break;
    case KIND_VALUE:                            // characteristic value
      t->type.len   = ulen;
      t->type.uuid  = a->uuid;
      t->permissions= 0;
      t->pValue     = 0;
      break;
    default:                                    // client characteristic configuration
      GATTServApp_InitCharCfg(INVALID_CONNHANDLE, a->ccc);
      t->type.len   = ATT_BT_UUID_SIZE;
      t->type.uuid  = clientCharCfgUUID;
      t->permissions= GATT_PERMIT_READ | GATT_PERMIT_WRITE;
      t->pValue     = (uint8_t*)a->ccc;
      break;
  }
  return ++BLE_attrCount;
}

// Add primary service with 16-bit UUID
uint16_t BLE_addService16(uint16_t uuid) {
  uint8_t u[2] = {(uint8_t)uuid, (uint8_t)(uuid >> 8)};
  return BLE_addAttr(KIND_SERVICE, u, 2);
}

// Add primary service with 128-bit UUID (LSB first)
uint16_t BLE_addService128(const uint8_t* uuid) {
  return BLE_addAttr(KIND_SERVICE, uuid, 16);
}

// Add characteristic (declaration, value and optional CCCD)
static uint16_t BLE_addChar(const uint8_t* uuid, uint8_t ulen, uint8_t props, uint8_t* buf,
                            uint16_t len, uint16_t max, BLE_WRITE_CB cb) {
  uint16_t d, h;
  BLE_ATTR* a;
  if(BLE_attrCount + ((props & BLE_NOTIFY) ? 3 : 2) > BLE_ATTRIBUTES) return 0;
  d = BLE_addAttr(KIND_CHAR, uuid, ulen);
  if(!d) return 0;
  BLE_info[d - 1].props = props;
  h = BLE_addAttr(KIND_VALUE, uuid, ulen);
  a = &BLE_info[h - 1];
  a->props = props; a->value = buf; a->len = (len > max) ? max : len; a->max = max; a->cb = cb;
  BLE_attr[h - 1].permissions = ((props & BLE_READ) ? GATT_PERMIT_READ : 0)
                              | ((props & (BLE_WRITE | BLE_WRITE_NORESP)) ? GATT_PERMIT_WRITE : 0);
  if(props & BLE_NOTIFY) BLE_addAttr(KIND_CCCD, 0, 2);
  return h;
}

// Add characteristic with 16-bit UUID
uint16_t BLE_addChar16(uint16_t uuid, uint8_t props, uint8_t* buf, uint16_t len,
                       uint16_t max, BLE_WRITE_CB cb) {
  uint8_t u[2] = {(uint8_t)uuid, (uint8_t)(uuid >> 8)};
  return BLE_addChar(u, 2, props, buf, len, max, cb);
}

// Add characteristic with 128-bit UUID (LSB first)
uint16_t BLE_addChar128(const uint8_t* uuid, uint8_t props, uint8_t* buf, uint16_t len,
                        uint16_t max, BLE_WRITE_CB cb) {
  return BLE_addChar(uuid, 16, props, buf, len, max, cb);
}

// Change value of characteristic
void BLE_setValue(uint16_t handle, const void* data, uint16_t len) {
  BLE_ATTR* a;
  if(!handle || (handle > BLE_attrCount)) return;
  a = &BLE_info[handle - 1];
  if(len > a->max) len = a->max;
  if(a->value && len) memcpy(a->value, data, len);
  a->len = len;
}

// Check if client has enabled notifications
uint8_t BLE_notifyEnabled(uint16_t handle) {
  if(!handle || (handle >= BLE_attrCount) || (BLE_conn == INVALID_CONNHANDLE)) return 0;
  if(BLE_info[handle].kind != KIND_CCCD) return 0;
  return (GATTServApp_ReadCharCfg(BLE_conn, BLE_info[handle].ccc) & GATT_CLIENT_CFG_NOTIFY) != 0;
}

// Send notification, returns 1 if sent (waits up to 200ms for free buffers)
uint8_t BLE_notify(uint16_t handle, const void* data, uint16_t len) {
  attHandleValueNoti_t noti;
  uint8_t  status;
  uint16_t tries = 200;
  BLE_poll();
  if(!BLE_notifyEnabled(handle)) return 0;
  if(len > BLE_getMTU() - 3) len = BLE_getMTU() - 3;
  while(tries--) {
    noti.handle = BLE_attr[handle - 1].handle;  // ATT handle assigned by the stack
    noti.len    = len;
    noti.pValue = GATT_bm_alloc(BLE_conn, ATT_HANDLE_VALUE_NOTI, len, NULL, 0);
    if(noti.pValue) {
      memcpy(noti.pValue, data, len);
      status = GATT_Notification(BLE_conn, &noti, FALSE);
      if(status == SUCCESS) return 1;
      GATT_bm_free((gattMsg_t*)&noti, ATT_HANDLE_VALUE_NOTI);
      if((status != MSG_BUFFER_NOT_AVAIL) && (status != bleNoResources)
         && (status != blePending) && (status != bleMemAllocError)) return 0;
    }
    BLE_delay(1);                               // wait for free buffers
    if(!BLE_notifyEnabled(handle)) return 0;
  }
  return 0;
}
