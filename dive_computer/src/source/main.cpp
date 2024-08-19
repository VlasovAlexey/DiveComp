#include "MicroBit.h"
#include "MicroBitUARTService.h"
#include "Tests.h"

// Create a global instance of the MicroBit model called uBit.
// Allows this singleton to be accessed consistently by tests/programs.

#ifdef MICROBIT_UBIT_AS_STATIC_OBJECT
// A statically allocated model can be simply created using the code below.
// This is the simplest, and ideal for C/C++ programs.
MicroBit uBit;

#else
// Alternatively, we can dynamically allocated the model on te heap.
// This is better for testing builds for environments that do this (such as MakeCode) 
MicroBit &uBit = *(new MicroBit());
#endif

MicroBitUARTService *uart;
int connected = 0;
int dPowerOff = 0;
int recordStarted = 0;
// The ADC channel to be used for the battery voltage measurement
static int BATTERY_ADC_CHANNEL = -1;

//get battary charge in percent
static int get_vdd_percent() {
    if (BATTERY_ADC_CHANNEL == -1) {
        // Not initilised
        return 0;
    }
    // Second arg ensures channel is activated before we do an analog read,
    // so that it configures CH[n].CONFIG before we apply our changes
    (void)uBit.adc.getChannel(uBit.io.P0, true);

    // Configure CH[n].CONFIG with 0.6V internal reference and 1/6 gain
    // so max Vin detection is 0-3.6V
    NRF_SAADC->CH[BATTERY_ADC_CHANNEL].CONFIG =
        // Positive channel resistor control set to bypass resistor ladder
        (SAADC_CH_CONFIG_RESP_Bypass << SAADC_CH_CONFIG_RESP_Pos) |
        // Negative channel resistor control set to bypass resistor ladder
        (SAADC_CH_CONFIG_RESN_Bypass << SAADC_CH_CONFIG_RESN_Pos) |
        // Gain control set to 1/6
        (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) |
        // Reference control set to internal 0.6V reference
        (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |
        // Acquisition time set to 3us
        (SAADC_CH_CONFIG_TACQ_3us << SAADC_CH_CONFIG_TACQ_Pos) |
        // Differential mode set to single ended
        (SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos) |
        // Burst mode set to disabled
        (SAADC_CH_CONFIG_BURST_Disabled << SAADC_CH_CONFIG_BURST_Pos);

    // And set the positive input to VDD
    NRF_SAADC->CH[BATTERY_ADC_CHANNEL].PSELP = SAADC_CH_PSELP_PSELP_VDD << SAADC_CH_PSELP_PSELP_Pos;

    int vin_millivolts = (1000 * 0.6 * 6 * uBit.io.P0.getAnalogValue()) / 1024;
    
    if (vin_millivolts > 3300){
        vin_millivolts = 3250;
    }
    if (vin_millivolts < 2100){
        vin_millivolts = 2100;
    }
    int vin_percent = (vin_millivolts - 2100)/((3300 - 2100) / 100);
    return vin_percent;
}

/**
 * Initialize the ADC to be able to measure Vdd voltage.
 *
 * To do this it initialises and finds the ADC channel for pin P0,
 * which is hijacked by get_vdd_millivolts() to measure Vdd instead.
 */
static void vdd_adc_init() {
    // Ensure CODAL has configured the P0 ADC channel
    (void)uBit.io.P0.getAnalogValue();
    (void)uBit.adc.getChannel(uBit.io.P0, true);
    (void)uBit.io.P0.getAnalogValue();

    // Now look in the MCU ADC channels for a channel configured for P0 (P0.2 AIN0)
    for (size_t i = 0; i < NRF52_ADC_CHANNELS; i++) {
        if (NRF_SAADC->CH[i].PSELP == (SAADC_CH_PSELP_PSELP_AnalogInput0 << SAADC_CH_PSELP_PSELP_Pos)) {
            BATTERY_ADC_CHANNEL = i;
            break;
        }
    }

    // Do one measurement and throw away the result, as the first is always a bit off
    (void)get_vdd_percent();
}

//create images for conect disconect and conected status
const char * const connect_emoji ="\
    255,255,000,255,255\n\
    555,255,000,255,255\n\
    000,000,000,000,000\n\
    255,000,000,000,255\n\
    000,255,255,255,000\n";

const char * const disconnect_emoji ="\
    255,000,000,000,255\n\
    000,255,000,255,000\n\
    000,000,255,000,000\n\
    000,255,000,255,000\n\
    255,000,000,000,255\n";

const char * const pwr1_emoji ="\
    255,255,255,255,255\n\
    255,255,255,255,255\n\
    255,255,255,255,255\n\
    255,255,255,255,255\n\
    255,255,255,255,255\n";

const char * const pwr2_emoji ="\
    128,128,128,128,128\n\
    128,128,128,128,128\n\
    128,128,128,128,128\n\
    128,128,128,128,128\n\
    128,128,128,128,128\n";

const char * const pwr3_emoji ="\
    64,64,64,64,64\n\
    64,64,64,64,64\n\
    64,64,64,64,64\n\
    64,64,64,64,64\n\
    64,64,64,64,64\n";

const char * const pwr4_emoji ="\
    32,32,32,32,32\n\
    32,32,32,32,32\n\
    32,32,32,32,32\n\
    32,32,32,32,32\n\
    32,32,32,32,32\n";

const char * const pwr5_emoji ="\
    16,16,16,16,16\n\
    16,16,16,16,16\n\
    16,16,16,16,16\n\
    16,16,16,16,16\n\
    16,16,16,16,16\n";

const char * const clear_emoji ="\
    0,0,0,0,0\n\
    0,0,0,0,0\n\
    0,0,0,0,0\n\
    0,0,0,0,0\n\
    0,0,0,0,0\n";

MicroBitImage power1(pwr1_emoji);
MicroBitImage power2(pwr2_emoji);
MicroBitImage power3(pwr3_emoji);
MicroBitImage power4(pwr4_emoji);
MicroBitImage power5(pwr5_emoji);
MicroBitImage clear_screen(clear_emoji);
MicroBitImage connect(connect_emoji);
MicroBitImage disconnect(disconnect_emoji);

// We use events and the 'connected' variable to keep track of the status of the Bluetooth connection
void onConnected(MicroBitEvent)
{
    uBit.display.print(connect);
    connected = 1;

    ManagedString eom(":");

    while(connected == 1) {
        ManagedString msg = uart->readUntil(eom);
        uart->send(msg);
    }
}

void onDisconnected(MicroBitEvent)
{
    if(dPowerOff == 0){
        uBit.display.print(disconnect);
        connected = 0;
    }
}
//device start info
void startScreen() {
    uBit.display.print(connect);
    uBit.sleep(2000);

    ManagedString battery_percent = " " + ManagedString(get_vdd_percent()) + "%";
    uBit.display.scroll(battery_percent);
    uBit.sleep(1000);
}

//recording process
void recording(){
    if(recordStarted == 1){
        uBit.display.print(power1);
        uBit.sleep(250);
        uBit.display.print(clear_screen);
        uBit.sleep(250);
    }
}

//device start and stop recording track
void onButtonA(MicroBitEvent)
{
    if(dPowerOff == 0 ){
        uBit.display.scroll("RECORD");
        if(recordStarted == 0){
            recording();
            uBit.sleep(500);
            recordStarted = 1;
        } else {    
            uBit.display.print(clear_screen);
            uBit.sleep(500);
            recordStarted = 0;
        }
    }
}
//device power off
void onButtonB(MicroBitEvent)
{
    if(dPowerOff == 0 ){
        if(recordStarted == 0){
            uBit.display.print(clear_screen);
        uBit.sleep(250);
        uBit.display.print(power5);
        uBit.sleep(250);
        uBit.display.print(power4);
        uBit.sleep(250);
        uBit.display.print(power3);
        uBit.sleep(250);
        uBit.display.print(power2);
        uBit.sleep(250);
        uBit.display.print(power1);
        uBit.sleep(250);
        uBit.display.print(power2);
        uBit.sleep(250);
        uBit.display.print(power3);
        uBit.sleep(250);
        uBit.display.print(power4);
        uBit.sleep(250);
        uBit.display.print(power5);
        uBit.sleep(250);
        uBit.display.print(clear_screen);
        dPowerOff = 1;
        //uBit.power.deepSleep(100000000000);
        }
    }
}
void onButtonAB(MicroBitEvent)
{   
    if(dPowerOff == 1 ){
        
        
        uBit.init();
        vdd_adc_init();
        
        startScreen();
        uBit.display.print(disconnect);
        dPowerOff = 0;
    }
}

int main() {
    uBit.init();
    vdd_adc_init();

    //start listenig all perephical devices
    new MicroBitMagnetometerService(*uBit.ble, uBit.compass);
    new MicroBitIOPinService(*uBit.ble, uBit.io);
    new MicroBitAccelerometerService(*uBit.ble, uBit.accelerometer);
    new MicroBitTemperatureService(*uBit.ble, uBit.thermometer);
    new MicroBitButtonService(*uBit.ble);
    new MicroBitLEDService(*uBit.ble, uBit.display);
    new MicroBitMagnetometerService(*uBit.ble, uBit.compass);
    uart = new MicroBitUARTService(*uBit.ble, 32, 32);

    //first start
    startScreen();
    uBit.display.print(disconnect);

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    
    uBit.messageBus.listen(DEVICE_ID_BUTTON_AB, DEVICE_BUTTON_EVT_CLICK, onButtonAB);
    
    //uBit.messageBus.listen(DEVICE_ID_BUTTON_A, DEVICE_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(DEVICE_ID_BUTTON_B, DEVICE_BUTTON_EVT_CLICK, onButtonB);
    
    uBit.io.buttonA.wakeOnActive(1);
    
    release_fiber();
    
    while (true) {
        recording();
        //uBit.sleep(10);
    }
    
}
