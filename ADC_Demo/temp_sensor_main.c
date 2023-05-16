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
    uint32_t pui32ADC0Value[1];
    
    uint32_t ui32TempValueC; // Celsius
    uint32_t ui32TempValueF; // Fahrenheit

    // 20M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    
    InitConsole();
     
    UARTprintf("ADC ->\r\n");
    UARTprintf("    Type: Internal Temperature Sensor\r\n");
    UARTprintf("    Samples = One\r\n");
    UARTprintf("    Update Rate: 250ms\r\n");
    UARTprintf("    Input Pin: Internal temperature sensor\r\n");
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    // ADC0 Sequence 3 ; processor signal trigger. 
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); 
    
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);

    for (;;) {
        ADCProcessorTrigger(ADC0_BASE, 3);

        while (!ADCIntStatus(ADC0_BASE, 3, false)) {
            
        }

        ADCIntClear(ADC0_BASE, 3);
        ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

        ui32TempValueC = (float)147.5  - ((75 * (float)3.3 * pui32ADC0Value[0]) / 4096);

        ui32TempValueF = ((ui32TempValueC * 9) / 5) + 32;

        UARTprintf("Temperature = %3d*C or %3d*F\n", ui32TempValueC, ui32TempValueF);

        SysCtlDelay(ROM_SysCtlClockGet() / 6);
    }
}

