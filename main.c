#include "hw.h"
#include "low_power.h"
#include "lora.h"
#include "bsp.h"
#include "timeServer.h"
#include "vcom.h"
#include "version.h"
#include <stdlib.h>

#define LORAWAN_ADR_ON                              1
#define LORAWAN_CONFIRMED_MSG                       DISABLE
#define LORAWAN_APP_PORT                            2
#define JOINREQ_NBTRIALS                            3
#define APP_TX_DUTYCYCLE                            30000

static void LoraTxData(lora_AppData_t *AppData, FunctionalState* IsTxConfirmed);
static void LoraRxData(lora_AppData_t *AppData);

static LoRaMainCallback_t LoRaMainCallbacks = {
    HW_GetBatteryLevel,
    HW_GetUniqueId,
    HW_GetRandomSeed,
    LoraTxData,
    LoraRxData
};

static uint8_t AppLedStateOn = RESET;

static LoRaParam_t LoRaParamInit = {
    TX_ON_TIMER,
    APP_TX_DUTYCYCLE,
    CLASS_A,
    LORAWAN_ADR_ON,
    DR_0,
    LORAWAN_PUBLIC_NETWORK,
    JOINREQ_NBTRIALS
};

// Variables pour la position GPS simulée
static int32_t latitude = (int32_t)(42.67511528354634 * 10000);
static int32_t longitude = (int32_t)(2.8479040528748936 * 10000);
static uint8_t uplinkCount = 0; // Nouvelle variable pour tracer les liaisons montantes

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    DBG_Init();
    HW_Init();

    lora_Init(&LoRaMainCallbacks, &LoRaParamInit);

    PRINTF("VERSION: %X\n\r", VERSION);

    while (1)
    {
        lora_fsm();

        DISABLE_IRQ();
        if (lora_getDeviceState() == DEVICE_STATE_SLEEP)
        {
#ifndef LOW_POWER_DISABLE
            LowPower_Handler();
#endif
        }
        ENABLE_IRQ();
    }
}

static void LoraTxData(lora_AppData_t *AppData, FunctionalState* IsTxConfirmed)
{
    static uint8_t uplinkCount = 0;

    // Déplacement constant vers l’est
    int32_t deltaLon = 12700; // ≈ 100 km vers est

    // Oscillazione nord/sud
    int32_t deltaLat;
    if (uplinkCount % 2 == 0) {
        deltaLat = 90;   // ≈ +1 km vers nord
    } else {
        deltaLat = -180;  // ≈ -2 km vers sud
    }

    latitude += deltaLat;
    longitude += deltaLon;
    uplinkCount++;

    uint16_t altitudeGps = 50;
    int16_t temperature = 2200;
    uint16_t pressure = 10130;
    uint16_t humidity = 500;
    uint8_t batteryLevel = HW_GetBatteryLevel();

    uint32_t i = 0;
    AppData->Port = LORAWAN_APP_PORT;
    *IsTxConfirmed = LORAWAN_CONFIRMED_MSG;

    AppData->Buff[i++] = AppLedStateOn;
    AppData->Buff[i++] = (pressure >> 8) & 0xFF;
    AppData->Buff[i++] = pressure & 0xFF;
    AppData->Buff[i++] = (temperature >> 8) & 0xFF;
    AppData->Buff[i++] = temperature & 0xFF;
    AppData->Buff[i++] = (humidity >> 8) & 0xFF;
    AppData->Buff[i++] = humidity & 0xFF;
    AppData->Buff[i++] = batteryLevel;
    AppData->Buff[i++] = (latitude >> 16) & 0xFF;
    AppData->Buff[i++] = (latitude >> 8) & 0xFF;
    AppData->Buff[i++] = latitude & 0xFF;
    AppData->Buff[i++] = (longitude >> 16) & 0xFF;
    AppData->Buff[i++] = (longitude >> 8) & 0xFF;
    AppData->Buff[i++] = longitude & 0xFF;
    AppData->Buff[i++] = (altitudeGps >> 8) & 0xFF;
    AppData->Buff[i++] = altitudeGps & 0xFF;

    AppData->BuffSize = i;
}


static void LoraRxData(lora_AppData_t *AppData)
{
    switch (AppData->Port)
    {
    case LORAWAN_APP_PORT:
        if (AppData->BuffSize == 1)
        {
            AppLedStateOn = AppData->Buff[0] & 0x01;
            if (AppLedStateOn == RESET)
            {
                PRINTF("LED OFF\n\r");
                LED_Off(LED_BLUE);
            }
            else
            {
                PRINTF("LED ON\n\r");
                LED_On(LED_BLUE);
            }
        }
        break;
    default:
        break;
    }
}

#ifdef USE_B_L072Z_LRWAN1
static TimerEvent_t TxLedTimer;
static void OnTimerLedEvent(void)
{
    LED_Off(LED_RED1);
}
#endif
