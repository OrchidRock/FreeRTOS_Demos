#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"

void InitConsole(void);

/*
 * PA0: UART0_RX; PA1: UART0_TX
 */
void InitConsole(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {} 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
    
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    
    //UARTConfigSetExpClk(UART0_BASE,16000000, 115200, UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8);
    //UARTEnable(UART0_BASE);
    UARTStdioConfig(0, 115200, 16000000);
}


int main()
{
    uint32_t pui32ADC0Value[2] = {0};
    uint32_t pui32Humidity = 0; 
    
    uint32_t pui32NTCResistance = 0;
    uint32_t pui32Temperature = 0;
    // 20M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    
    InitConsole();
     
    UARTprintf("ADC ->\r\n");
    UARTprintf("    Type: Single Ended\r\n");
    UARTprintf("    Samples = Two Channel\r\n");
    UARTprintf("    Update Rate: 250ms\r\n");
    UARTprintf("    Input Pin: AIN0/PE3 AIN1/PE2\r\n");
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    
    // PE3/PE2 : AM1101A temperature and humidity sensor.
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_2); 
    
    ADCSequenceDisable(ADC0_BASE, 2);
    // ADC0 Sequence 2 ; processor signal trigger. 
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0); 
    
    // Step
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);
    
    ADCSequenceEnable(ADC0_BASE, 2);
    ADCIntClear(ADC0_BASE, 2);

    for (;;) {
        ADCProcessorTrigger(ADC0_BASE, 2);

        while (!ADCIntStatus(ADC0_BASE, 2, false)) {
            
        }

        ADCIntClear(ADC0_BASE, 2);
        ADCSequenceDataGet(ADC0_BASE, 2, pui32ADC0Value);
        
        pui32Humidity = ((pui32ADC0Value[0] / (float)4096.0) * (float)3.3) * 100 / 3;
        
        // 10K serial resistance.
        pui32NTCResistance = (4096 * 10 * 1000) / pui32ADC0Value[1] - 10*1000;
        
        UARTprintf("AIN0 = %4d Humidity = %4d %%RH\n",pui32ADC0Value[0], pui32Humidity);
        UARTprintf("AIN1 = %4d NTC Resistance = %4d Ohm\n",pui32ADC0Value[1], pui32NTCResistance);

        SysCtlDelay(ROM_SysCtlClockGet() / 6);
    }
}

