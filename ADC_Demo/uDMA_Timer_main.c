#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "uartstdio.h"

#define ADC_SAMPLE_BUF_SIZE 64

uint8_t g_ui8ControlTable[1024] __attribute__ ((aligned(1024))) ;

static uint16_t g_ui16ADCBuffer1[ADC_SAMPLE_BUF_SIZE];
static uint16_t g_ui16ADCBuffer2[ADC_SAMPLE_BUF_SIZE];

enum BUFFER_STATUS {
    EMPTY,
    FILLING,
    FULL
};

static enum BUFFER_STATUS g_ui32BufferStatus[2];
static uint32_t g_ui32DMAErrCount = 0u;

void InitConsole(void);
void ADCSeq0_Handler(void);
void uDMAError_Handler(void);

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

void uDMAError_Handler(void) {
    uint32_t ui32Status;
    ui32Status = uDMAErrorStatusGet();

    if (ui32Status) {
        uDMAErrorStatusClear();
        g_ui32DMAErrCount++;
    }
}

void ADCSeq0_Handler(void) {
    ADCIntClear(ADC0_BASE, 0);
    if ((uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT) == UDMA_MODE_STOP) && (g_ui32BufferStatus[0] == FILLING)) {
        g_ui32BufferStatus[0] = FULL;
        g_ui32BufferStatus[1] = FILLING;
    }else if ((uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT) == UDMA_MODE_STOP) && (g_ui32BufferStatus[1] == FILLING)) {
        g_ui32BufferStatus[0] = FILLING;
        g_ui32BufferStatus[1] = FULL;
    }
}

int main()
{
    uint32_t ui32Count;
    uint32_t ui32AverageResult1 = 0;
   uint32_t  ui32AverageResult2 = 0;
    uint32_t ui32SamplesTaken = 0;
    
    g_ui32BufferStatus[0] = FILLING;
    g_ui32BufferStatus[1] = EMPTY;

    // 80M HZ
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_INT);
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    
    InitConsole();
     
    UARTprintf("Timer->ADC->uDMA Demo\r\n");
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    

    // PE3/Channel 0 : AM1101A temperature and humidity sensor.
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    
    // uDMA
    uDMAEnable();
    uDMAControlBaseSet(g_ui8ControlTable);
    uDMAChannelAttributeDisable(UDMA_CHANNEL_ADC0, UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
    
    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT, UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT, UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
    
    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
                    UDMA_MODE_PINGPONG,
                    (void*)(ADC0_BASE + ADC_O_SSFIFO0),
                    &g_ui16ADCBuffer1, ADC_SAMPLE_BUF_SIZE);
    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
                    UDMA_MODE_PINGPONG,
                    (void*)(ADC0_BASE + ADC_O_SSFIFO0),
                    &g_ui16ADCBuffer2, ADC_SAMPLE_BUF_SIZE);
    
    uDMAChannelAttributeEnable(UDMA_CHANNEL_ADC0, UDMA_ATTR_USEBURST);
    uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    uDMAIntRegister(INT_UDMAERR, uDMAError_Handler);
    IntEnable(INT_UDMAERR);

    // ADC0
    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF, 1);
    SysCtlDelay(10);
    
    // Sequence 0: FIFO depth is 8. 
    IntDisable(INT_ADC0SS0); // ADC0 Sequence 0
    ADCIntDisable(ADC0_BASE, 0);
    ADCSequenceDisable(ADC0_BASE, 0);
    
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_END | ADC_CTL_IE);
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);
    ADCSequenceDMAEnable(ADC0_BASE, 0);
    ADCIntEnable(ADC0_BASE, 0);
    ADCIntRegister(ADC0_BASE, 0, ADCSeq0_Handler);
    IntEnable(INT_ADC0SS0);

    // Timer 0A
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet()/16000)-1); // 16 kHZ
    // Enable ADC trigger
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
    
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);

    for (;;) {
        if (g_ui32BufferStatus[0] == FULL) {
            ui32AverageResult1 = 0;
            for (ui32Count = 0;  ui32Count < ADC_SAMPLE_BUF_SIZE; ui32Count++) {
                ui32AverageResult1 += g_ui16ADCBuffer1[ui32Count];
                g_ui16ADCBuffer1[ui32Count] = 0;
            }

            g_ui32BufferStatus[0] = EMPTY;

            uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT, 
                            UDMA_MODE_PINGPONG,
                            (void*)(ADC0_BASE + ADC_O_SSFIFO0),
                            &g_ui16ADCBuffer1, ADC_SAMPLE_BUF_SIZE);
            uDMAChannelEnable(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
            ui32SamplesTaken += ADC_SAMPLE_BUF_SIZE;
            ui32AverageResult1 = ((ui32AverageResult1 + (ADC_SAMPLE_BUF_SIZE / 2)) / ADC_SAMPLE_BUF_SIZE);
        }

        if (g_ui32BufferStatus[1] == FULL) {
            ui32AverageResult2 = 0;
            for (ui32Count = 0;  ui32Count < ADC_SAMPLE_BUF_SIZE; ui32Count++) {
                ui32AverageResult2 += g_ui16ADCBuffer2[ui32Count];
                g_ui16ADCBuffer2[ui32Count] = 0;
            }

            g_ui32BufferStatus[1] = EMPTY;

            uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT, 
                            UDMA_MODE_PINGPONG,
                            (void*)(ADC0_BASE + ADC_O_SSFIFO0),
                            &g_ui16ADCBuffer2, ADC_SAMPLE_BUF_SIZE);
            uDMAChannelEnable(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);
            ui32SamplesTaken += ADC_SAMPLE_BUF_SIZE;
            ui32AverageResult2 = ((ui32AverageResult2 + (ADC_SAMPLE_BUF_SIZE / 2)) / ADC_SAMPLE_BUF_SIZE);
        

            UARTprintf("\t%4d\t\t\t%4d\t\t%d\n", ui32AverageResult1, ui32AverageResult2, ui32SamplesTaken); 
        }
    }
}

