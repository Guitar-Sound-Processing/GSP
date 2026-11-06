// Title: Guitar Sound Processing - GSP
// Description: Sound effects for guitar
// Hardware: Daisy Seed
// Author: Valdemir Carrara
// main gsp

//  Daisy Examples: ?
//  libdaisy:       8.1.0

// Guitar Sound Processing
// ****************************************************************************
#include "daisy_seed.h"
#include "guitar_dsp.h"

//using namespace daisy;

#define   BUFFER_SIZE   262144  
#define   REV_BUFSIZE   8192
#define   UART_BUFFER_SIZE  512

// ****************************************************************************
static    DaisySeed   hw;

//HardwareSerial  SerialCom(14UL, 13UL); // USART1  Pins 14 (Rx) and 13 (Tx)
//>>>HardwareSerial  SerialPot(11UL, 12UL); // UART4  Pins 11 (Rx) and 12 (Tx)

uint32_t    icon;

// Delay and Echo memory
int16_t     DSY_SDRAM_BSS adc_buffer[BUFFER_SIZE];   // chorus, delay
int16_t     DSY_SDRAM_BSS rvb_buffer[REV_BUFSIZE];   // reverber
uint32_t    buffer_pointer;

//  Time control
float       delt_ms = 0.f, delt_time = 1000.f, time_ms = 0;
uint32_t    tick, tick_0;
float       duty;
uint32_t    t0, tend = 0;

int32_t     smp_max = 0, smp_min = 0;

//  Audio
size_t      num_channels;
uint32_t    samplerate;

// Output flags
volatile uint8_t  poutFlag = 0, verbose_flag = 0;
uint8_t     out_list = 0;
bool        muted = false;

// Built-in LED
bool        ledstate = false;

uint8_t     inp_source = 0;

// USB-Serial
char        usb_buff[256];
static uint32_t   usb_buf_ready = 0;
char        st[64];

char        uart_cs;
uint8_t     uart_com[4] = {123, 10, 13, 0}; // {'{', '\r', '\n', '\0'}} 

// USART_1:
UartHandler             uart1;
UartHandler::Config     uart_config;

uint8_t             DMA_BUFFER_MEM_SECTION uart1_buffer[UART_BUFFER_SIZE];
char                uart1_str[UART_BUFFER_SIZE];
uint8_t             uart1_last;
size_t              uart1_length, uart1_start;
bool                uart1_ready;

// UART_4:

// Potentiometers
char                nb_pot = 0;
uint8_t             ipot = 0, pot_aux, pot_data[20];
uint32_t            pot_effect, pot_fr = 0;
char                disambig_com[]  = "{";
char                disambig_pot[]  = "}";
char                pot_start[]     = "S\n";
char                pot_clear[]     = "C\n";
uint16_t            pot;
uint8_t             pot_2bytes[2];
bool                show_expression = false;

// Debug
bool                is_potdata;

// Effects:
GSP_Compressor    cps;
GSP_Overdrive     ovd;
GSP_Phaser        phr;
GSP_WahWah        wah;
GSP_Detune        dtn;
GSP_PitchShifter  sft;
GSP_Octave        oct;
GSP_Equalizer     eqz;
GSP_Reverber      rvb;
GSP_DelayFB       dfb(DELAY_FB), efb(ECHO_FB);
GSP_DelayFF       dff(DELAY_FF), eff(ECHO_FF);
GSP_Chorus        chs(CHORUS);
GSP_Chorus        vbt(VIBRATO);
GSP_Tremolo       tml, vol;
GSP_Limiter       lmt;
GSP_NoiseGate     ngt;

GSP_SignalChain   chain;
GSP_Pots          expot;
LowFreqOsc        lffg;

// ****************************************************************************
// Prototypes

void    loop() ;
void    _Setcommand(char ct[], uint8_t source);
int8_t  _SetPotGain(char ct[]);
char    *CommandName(char ct[], char cmd[]);
int32_t CommandDecoder(char ct[], int32_t *chn_chg, 
        int32_t *chn_code, float fl[], int32_t *fl_nb);
int8_t  PotDecoder(GSP_SignalChain *chain_, char ct[],  
        int32_t* effect_number, int32_t* pot_number);
void    ChangeEffectParams(float fl[], float fn[], int32_t nb);
void    SendPotStruct(GSP_Pots *pots_);
uint16_t convert_from_2bytes(uint8_t lsb_msb[]);
void    printAll(char st[], uint8_t input_source);
void    uartPrintAnswer(char str[]);
void    uartPrintPot(char str[]);
void    uartSendString(const char str[]);
void    uartDebug(char str[]);
void    uartDebug(const char str[]);
void    debugPotStruct(GSP_Pots *pots_);


// ****************************************************************************
// Code

static void GuitardspCB(AudioHandle::InterleavingInputBuffer  in,
        AudioHandle::InterleavingOutputBuffer out,
        size_t size)
{
    static int32_t  sampl;
//    static int32_t  level;
    static uint32_t i;
    static float    outspl;
    
    t0        = System::GetTick();
    sampl     = ADC_HALFRES*in[0];
//    level     = in[1];

    if (sampl > smp_max) smp_max = sampl;
    if (sampl < smp_min) smp_min = sampl;

    adc_buffer[buffer_pointer]  = sampl;

    i   = LevelDetectorProcess(sampl);  // level detector for LFO
    
    for (i = 0; i < chain.number_effects; i++)
    {
        switch (chain.sgn_chain[i])
        {
            case GSP_CMP:
                if (cps.state == GSP_ON) sampl  = cps.Process(sampl);
                break;
            case GSP_OVD:
                if (ovd.state == GSP_ON) sampl  = ovd.Process(sampl);
                break;
            case GSP_PHR:
                if (phr.state == GSP_ON) sampl  = phr.Process(sampl);
                break;
            case GSP_OCT:
                if (oct.state == GSP_ON) sampl  = oct.Process(sampl, buffer_pointer);
                break;
            case GSP_SFT:
                if (sft.state == GSP_ON) sampl  = sft.Process(sampl, buffer_pointer);
                break;
            case GSP_DTN:
                if (dtn.state == GSP_ON) sampl  = dtn.Process(sampl, buffer_pointer);
                break;
            case GSP_WAH:
                if (wah.state == GSP_ON) sampl  = wah.Process(sampl);
                break;
            case GSP_EQZ:
                if (eqz.state == GSP_ON) sampl  = eqz.Process(sampl);
                break;
            case GSP_CHS:
                if (chs.state == GSP_ON) sampl  = chs.Process(sampl, buffer_pointer);
                break;
            case GSP_VBT:
                if (vbt.state == GSP_ON) sampl  = vbt.Process(sampl, buffer_pointer);
                break;
            case GSP_RVB:
                if (rvb.state == GSP_ON) sampl  = rvb.Process(sampl);
                break;
            case GSP_DFB:
                if (dfb.state == GSP_ON) 
                {
                    sampl  = dfb.Process(sampl, buffer_pointer);
                    adc_buffer[buffer_pointer] = sampl;
                }
                break;
            case GSP_EFB:
                if (efb.state == GSP_ON) 
                {
                    sampl  = efb.Process(sampl, buffer_pointer);
                    adc_buffer[buffer_pointer] = sampl;
                }
                break;
            case GSP_DFF:
                if (dff.state == GSP_ON) sampl  = dff.Process(sampl, buffer_pointer);
                break;
            case GSP_EFF:
                if (eff.state == GSP_ON) sampl  = eff.Process(sampl, buffer_pointer);
                break;
            case GSP_TML:
                if (tml.state == GSP_ON) sampl  = tml.Process(sampl);
                break;
            case GSP_LIM:
                if (lmt.state == GSP_ON) sampl  = lmt.Process(sampl);
                break;
            case GSP_VOL:
                if (vol.state == GSP_ON) sampl  = vol.Process(sampl);
                break;
            case GSP_NGT:
                if (ngt.state == GSP_ON) sampl  = ngt.Process(sampl);
                break;
            default:
                break;
        }
    }
    
    outspl    = sampl*ADC_INVHRESF;
    out[0]    = outspl;
    out[1]    = outspl;

    buffer_pointer++;
    if (buffer_pointer == BUFFER_SIZE) buffer_pointer = 0;
    
    tend  += System::GetTick() - t0;

    return;
}

// ****************************************************************************

void UsbCallback(uint8_t* buf, uint32_t* len)
{
    static size_t  i;
    i   = 0;

    while (i < *len && buf[i] >= 13)
    {
        usb_buff[i] = buf[i];
        i++;
    }
    usb_buf_ready   = i;
    usb_buff[i]  = 0;
    return;
}

// ****************************************************************************

void uart1Callback(uint8_t* data, size_t size, void* context, UartHandler::Result res)
{
    // some messages come truncated due to buffer size limitation. Therefore a terminator 
    // chacacter code (<32) had to be set to identify message ending.

    if (size > 0)
    {
        uart1_last  = data[size - 1];
        std::copy(&data[0], &data[size], &uart1_str[uart1_start]);
        if (uart1_last == 13 || uart1_last == 10)
        {
            uart1_length    = size + uart1_start;
            uart1_start     = 0;
            uart1_ready     = true;
            uart1_str[uart1_length - 1]   = 0;
        }
        else
        {
            uart1_length    = size;
            uart1_start     = size;
        }
    }
    //hw.PrintLine("received");
    return;
}

// ****************************************************************************

int main(void)
{

    hw.Init(true);  // 480 Mhz
    // hw.Init(false);  // 400 Mhz
 
    hw.SetAudioBlockSize(1);

    samplerate      = (uint32_t)(hw.AudioSampleRate() + 0.1f);
    
    //hw.SetAudioSampleRate(enum - verificar);

    buffer_pointer  = 0;

    // Enable Logging, and set up the USB connection.
    hw.StartLog(false);

    // ============================================================================
    // USB Serial
    hw.usb_handle.SetReceiveCallback(UsbCallback, UsbHandle::FS_INTERNAL);

    // ============================================================================
    // UART Serial
    // baudrates: 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
    uart_config.baudrate        = 115200;
    uart_config.periph          = UartHandler::Config::Peripheral::USART_1;
    uart_config.pin_config.rx   = {DSY_GPIOB, 7};  // (USART_1 RX) Daisy pin 15
    uart_config.pin_config.tx   = {DSY_GPIOB, 6};  // (USART_1 TX) Daisy pin 14
    uart_config.stopbits        = UartHandler::Config::StopBits::BITS_1;
    uart_config.parity          = UartHandler::Config::Parity::NONE;
    uart_config.mode            = UartHandler::Config::Mode::TX_RX;
    uart_config.wordlength      = UartHandler::Config::WordLength::BITS_8;
    uart1.Init(uart_config);
    uart1.DmaListenStart(uart1_buffer, UART_BUFFER_SIZE, uart1Callback, nullptr);
    
    uart1_ready     = false;
    uart1_start     = 0;

    //System::Delay(5000);

    // ============================================================================
    // GSP
    chain.New();
    
    LevelDetectorSetSamples(samplerate, 48, 48000);
    cps.Init(samplerate);
    ovd.Init(samplerate);
    phr.Init(samplerate);
    wah.Init(samplerate);
    dtn.Init(samplerate, adc_buffer, BUFFER_SIZE);
    sft.Init(samplerate, adc_buffer, BUFFER_SIZE);
    oct.Init(samplerate, adc_buffer, BUFFER_SIZE);
    chs.Init(samplerate, adc_buffer, BUFFER_SIZE);
    vbt.Init(samplerate, adc_buffer, BUFFER_SIZE);
    dfb.Init(samplerate, adc_buffer, BUFFER_SIZE);
    efb.Init(samplerate, adc_buffer, BUFFER_SIZE);
    dff.Init(samplerate, adc_buffer, BUFFER_SIZE);
    eff.Init(samplerate, adc_buffer, BUFFER_SIZE);
    eqz.Init(samplerate);
    rvb.Init(samplerate, rvb_buffer, REV_BUFSIZE);
    tml.Init(samplerate);
    lmt.Init(samplerate);
    vol.Init(samplerate);
    ngt.Init(samplerate);
    
    //dsy_audio_set_blocksize(DSY_AUDIO_INTERNAL, 1);   // Just one sample at each callback
    //DAISY.SetAudioBlockSize(1); // Just one sample per audio callback
            
    //DAISY.begin(GuitardspCB);
    hw.StartAudio(GuitardspCB);

    while(1) 
    {
        loop();
    }
}

// ****************************************************************************

void loop() 
{

    // ----------------------------------------------------------------------
    //          Print duty time
    tick        = System::GetTick();
    delt_ms     = (float)(tick - tick_0)/200000.f;
    if (delt_ms > delt_time)
    {
        time_ms     += delt_ms;
        tick_0      = tick;

        // Print duty time
        duty        = (float)tend/2.0e6;    // in percent of total time
        if (poutFlag)
        {
            sprintf(st, "Duty: %f\n", duty);
            printAll(st, inp_source);
            
            // Print the maximum and minimum ADC sampled value
            //hw.PrintLine("Max %ld,  Min %ld", smp_max, smp_min);

            // Print the last potentiometer data
            // hw.Print("Pot data: ");
            // hw.PrintLine("value: %d ", pot);

            smp_max     = 0;
            smp_min     = 0;
        }

        if (show_expression)
        {
            sprintf(st, "EP: %d \n", pot);
            printAll(st, inp_source);
        }
    
        tend        = 0;

        // Blink led to signal processor activity
        hw.SetLed(ledstate);
        ledstate = !ledstate;

        //hw.PrintLine("Pot frequency: %ld", pot_fr);
        pot_fr      = 0;

        // Debug
        if (is_potdata)
        {
            //uartDebug(uart1_str);
        }
    }
    // ----------------------------------------------------------------------
    //          Data from ESP32 UART

    if (uart1_ready)
    {
        // Character to choose among Effect Command (123 {) and Potentiometer (125 })
        // Get data coming from ESP32 

        uart1_ready     = false;

        uart_cs = uart1_str[0];
        uart1_length--;
        for (icon = 0; icon < uart1_length; icon++) 
        {
            uart1_str[icon] = uart1_str[icon+1];
            if (uart1_str[icon] < 30) uart1_str[icon] = 0;
        }

        if (uart_cs == 123)
        {
            _Setcommand(uart1_str, 1);
            inp_source  = 1;
            is_potdata  = false;
        } 
        if (uart_cs == 125) 
        {
            _SetPotGain(uart1_str);
            is_potdata  = true;

        }
    }

    // ----------------------------------------------------------------------
    //          Command from Daisy Seed USB
    if (usb_buf_ready > 0)
    {
        inp_source  = 0;
        _Setcommand(usb_buff, 0);
        usb_buf_ready = 0;
    }

    return;
}

// ****************************************************************************

void _Setcommand(char ct[], uint8_t source)
{
    // ct       Effect setup string
    // source   Command source: 0=Daisy Seed USB, 1=ESP32 UART
    uint32_t    i;
    int8_t      chainf, decoded;
    int32_t     ceff, pos;
    char        cmd[8], pout[250], phal, *stc;
    float       fn[8] = {0, 0, 0, 0, 0, 0, 0, 0}, fl[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int32_t     cdec, fl_nb;

    decoded     = 0;
    chainf      = 0;
        
    if (verbose_flag)
    {
        sprintf(pout, "<-%s \r\n", ct);
        printAll(pout, source);
        cdec = strlen(ct); // this line is required, otherwise ct is corrupted. WHY??????????????????
    }

    stc     = CommandName(ct, cmd);

    cdec    = -1;
    ceff    = 0;

    if (stc != NULL)
    {
        cdec     = CommandDecoder(stc, &ceff, &pos, fl, &fl_nb);
    }
    //hw.PrintLine("ceff = %ld, pos = %ld", ceff, pos);

	if (cdec >= 0)
	{    
		//************************************* Level Detector
		if (strcmp(cmd, "lvd") == 0)
		{
			LevelDetectorParameters(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			LevelDetectorSetTimes(samplerate, fn[0], fn[1]);
			LevelDetectorPrintout(out_list, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Compressor
		if (strcmp(cmd, "cmp") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_CMP, pos);
            if (ceff < 0) chain.Remove(GSP_CMP);
            pos = chain.Locate(GSP_CMP);
            cps.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			cps.SetParams(fn);
            cps.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Overdrive
		if (strcmp(cmd, "ovd") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_OVD, pos);
            if (ceff < 0) chain.Remove(GSP_OVD);
            pos = chain.Locate(GSP_OVD);
			ovd.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			ovd.SetParams(fn);
			ovd.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Phaser
		if (strcmp(cmd, "phr") == 0)
		{        
            if (ceff > 0) chain.Swap(GSP_PHR, pos);
            if (ceff < 0) chain.Remove(GSP_PHR);
            pos = chain.Locate(GSP_PHR);
			phr.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			phr.SetParams(fn);
			phr.Printout(out_list, pos, pout);
            if (phr.lfo.profile == LFO_EXTERNAL)
            {
                expot.AssignPot(GSP_PHR, phr.lfo.channel);
            }
            else
            {
                expot.RemovePot(GSP_PHR);
            }
            if (expot.number_pots > 0) 
            {
                SendPotStruct(&expot);
                uartPrintPot(pot_start);
            }
            else
            {
                uartPrintPot(pot_clear);
            }
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Octave
		if (strcmp(cmd, "oct") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_OCT, pos);
            if (ceff < 0) chain.Remove(GSP_OCT);
            pos = chain.Locate(GSP_OCT);
			oct.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			oct.SetParams(fn);
			oct.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* PShifter
		if (strcmp(cmd, "sft") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_SFT, pos);
            if (ceff < 0) chain.Remove(GSP_SFT);
            pos = chain.Locate(GSP_SFT);
			sft.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			sft.SetParams(fn);
			sft.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Detune
		if (strcmp(cmd, "dtn") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_DTN, pos);
            if (ceff < 0) chain.Remove(GSP_DTN);
            pos = chain.Locate(GSP_DTN);
			dtn.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			dtn.SetParams(fn);
			dtn.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* WahWah
		if (strcmp(cmd, "wah") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_WAH, pos);
            if (ceff < 0) chain.Remove(GSP_WAH);
            pos = chain.Locate(GSP_WAH);
			wah.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			wah.SetParams(fn);
			wah.Printout(out_list, pos, pout);
            if (wah.lfo.profile == LFO_EXTERNAL)
            {
                expot.AssignPot(GSP_WAH, wah.lfo.channel);
            }
            else
            {
                expot.RemovePot(GSP_WAH);
            }
            if (expot.number_pots > 0) 
            {
                SendPotStruct(&expot);
                uartPrintPot(pot_start);
            }
            else
            {
                uartPrintPot(pot_clear);
            }
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Equalizer
		if (strcmp(cmd, "eqz") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_EQZ, pos);
            if (ceff < 0) chain.Remove(GSP_EQZ);
            pos = chain.Locate(GSP_EQZ);
			eqz.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			eqz.SetParams(fn);
			eqz.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Chorus
		if (strcmp(cmd, "chs") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_CHS, pos);
            if (ceff < 0) chain.Remove(GSP_CHS);
            pos = chain.Locate(GSP_CHS);
			chs.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			chs.SetParams(fn);
			chs.Printout(out_list, pos, pout);
            if (chs.lfo.profile == LFO_EXTERNAL)
            {
                expot.AssignPot(GSP_CHS, chs.lfo.channel);
            }
            else
            {
                expot.RemovePot(GSP_CHS);
            }
            if (expot.number_pots > 0) 
            {
                SendPotStruct(&expot);
                uartPrintPot(pot_start);
            }
            else
            {
                uartPrintPot(pot_clear);
            }
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		//************************************* Vibrato
		if (strcmp(cmd, "vbt") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_VBT, pos);
            if (ceff < 0) chain.Remove(GSP_VBT);
            pos = chain.Locate(GSP_VBT);
			vbt.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			vbt.SetParams(fn);
			vbt.Printout(out_list, pos, pout);
            if (vbt.lfo.profile == LFO_EXTERNAL)
            {
                expot.AssignPot(GSP_VBT, vbt.lfo.channel);
            }
            else
            {
                expot.RemovePot(GSP_VBT);
            }
            if (expot.number_pots > 0) 
            {
                SendPotStruct(&expot);
                uartPrintPot(pot_start);
            }
            else
            {
                uartPrintPot(pot_clear);
            }
            //debugPotStruct(&expot);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Reverber
		if (strcmp(cmd, "rvb") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_RVB, pos);
            if (ceff < 0) chain.Remove(GSP_RVB);
            pos = chain.Locate(GSP_RVB);
			rvb.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			rvb.SetParams(fn);
			rvb.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Delay_FB
		if (strcmp(cmd, "dfb") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_DFB, pos);
            if (ceff < 0) chain.Remove(GSP_DFB);
            pos = chain.Locate(GSP_DFB);
			dfb.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			dfb.SetParams(fn);
			dfb.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Echo_FB
		if (strcmp(cmd, "efb") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_EFB, pos);
            if (ceff < 0) chain.Remove(GSP_EFB);
            pos = chain.Locate(GSP_EFB);
			efb.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			efb.SetParams(fn);
			efb.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Delay_FF
		if (strcmp(cmd, "dff") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_DFF, pos);
            if (ceff < 0) chain.Remove(GSP_DFF);
            pos = chain.Locate(GSP_DFF);
			dff.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			dff.SetParams(fn);
			dff.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Echo_FF
		if (strcmp(cmd, "eff") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_EFF, pos);
            if (ceff < 0) chain.Remove(GSP_EFF);
            pos = chain.Locate(GSP_EFF);
			eff.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			eff.SetParams(fn);
			eff.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Tremolo
		if (strcmp(cmd, "tml") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_TML, pos);
            if (ceff < 0) chain.Remove(GSP_TML);
            pos = chain.Locate(GSP_TML);
			tml.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			tml.SetParams(fn);
			tml.Printout(out_list, pos, pout);
            if (tml.lfo.profile == LFO_EXTERNAL)
            {
                expot.AssignPot(GSP_TML, tml.lfo.channel);
            }
            else
            {
                expot.RemovePot(GSP_TML);
            }
            if (expot.number_pots > 0) 
            {
                SendPotStruct(&expot);
                uartPrintPot(pot_start);
            }
            else
            {
                uartPrintPot(pot_clear);
            }
            //debugPotStruct(&expot);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Limiter
		if (strcmp(cmd, "lim") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_LIM, pos);
            if (ceff < 0) chain.Remove(GSP_LIM);
            pos = chain.Locate(GSP_LIM);
			lmt.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			lmt.SetParams(fn);
			lmt.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Volume
		if (strcmp(cmd, "vol") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_VOL, pos);
            if (ceff < 0) chain.Remove(GSP_VOL);
            pos = chain.Locate(GSP_VOL);
			vol.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			vol.SetParams(fn);
			vol.Printout(out_list, pos, pout);
            pout[2]   = 'V';
            pout[3]   = 'O';
            pout[4]   = 'L';
            if (vol.lfo.profile == LFO_EXTERNAL)
            {
                expot.AssignPot(GSP_VOL, vol.lfo.channel);
            }
            else
            {
                expot.RemovePot(GSP_VOL);
            }
            if (expot.number_pots > 0) 
            {
                SendPotStruct(&expot);
                uartPrintPot(pot_start);
            }
            else
            {
                uartPrintPot(pot_clear);
            }
            //debugPotStruct(&expot);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}
		//************************************* Noise Gate
		if (strcmp(cmd, "ngt") == 0)
		{
            if (ceff > 0) chain.Swap(GSP_NGT, pos);
            if (ceff < 0) chain.Remove(GSP_NGT);
            pos = chain.Locate(GSP_NGT);
			ngt.GetParams(fn);
            ChangeEffectParams(fl, fn, fl_nb);
			ngt.SetParams(fn);
			ngt.Printout(out_list, pos, pout);
            if (muted) decoded      = 2;
            else decoded    = 1;
		}

		// ************************************* External potentiometer
        /*
        if (strcmp(cmd, "pot") == 0)
		{
			gpot  = PotDecoder(&chain, stc, &effect_n, &pot_id);

            if (gpot == 0)
			{
                
                // decoded succeeded with a valid id
                if (pot_id >= 0)
                {

                    // trying to add pot to effect
				    if (expot.AssignPot(effect_n, pot_id) == 0)
				    {
                        // valid effect and pot_id
                        // sending A command to ESP32 ...
                        SendPotStruct(&expot);
                        nb_pot      = expot.number_pots;
                        send_pot_data   = true;
                        decoded     = 2;
                        gpot        = 1;    // print all assingned pots
                    }
                    else
 		    		{
                        // invalid effect or pot_id
                        decoded     = 0; // error
                        gpot        = 0; // no pot printing
    				}
                }
                if (pot_id < 0)
                {
                    // remove the pot from effect
                    if (expot.RemovePot(effect_n) == 0)
                    {
                        // pot removed successfully
                        SendPotStruct(&expot);
                        nb_pot      = expot.number_pots;
                        decoded     = 2; // no error
                        gpot        = 1; // pot printing
                    }
                    else
                    {
                        // something goes wrong
                        gpot        = 0; // no pot printing
                        decoded     = 0; // error
                    }
                }
            }
			else
			{
                if (gpot > 0)
                {
                    // empty pot command, so print existing pot
                    decoded     = 2;
                    gpot        = 1; // pot printing
                }
                else
                {
                    // gpot = -1, something went wrong, command not decoded
                    gpot        = 0; // no pot printing
                    decoded     = 0;
                }
			}

            if (gpot > 0 && !muted)
            {
                if (out_list == 0)
                {
                    for (i = 0; i < expot.number_lfo; i++)
                    {
                        chain.Name(expot.effect_lfo[i], pname);
                        sprintf(pout, "->POT Effect: %s | Potentiometer ID: %ld\r\n",
                            pname, expot.link_id[i]);
                        if (source == 0) hw.PrintLine(pout);
                        if (source == 1)  uartPrintAnswer(pout);
                    }
                }
                if (out_list == 1)
                {
                    sprintf(pout, "->POT ");
                    for (i = 0; i < expot.number_lfo; i++)
                    {
                        chain.Name(expot.effect_lfo[i], pname);
                        strcat(pout, pname);
                        sprintf(pname, " %ld ", expot.link_id[i]);
                        strcat(pout, pname);
                    }
                    sprintf(pname, "\n");
                    strcat(pout, pname);

                    if (source == 0) hw.PrintLine(pout);
                    if (source == 1) uartPrintAnswer(pout);
                }
                decoded = 2;
            }
		}
        */

		//*********************************************** All - Chain
		if (strcmp(cmd, "all") == 0)
		{
			chainf    = 1;
		}
		//*********************************************** Clear - Chain
		if (strcmp(cmd, "clr") == 0)
		{
			chain.Clear();
			chainf    = 1;
		}
		//*********************************************** New - Chain
		if (strcmp(cmd, "new") == 0)
		{
            chain.New();
			chainf    = 1;
		}
		//*********************************************** Command ID - Chain
		if (strcmp(cmd, "cid") == 0)
		{
            sprintf(pout, "-> %lu \n", chain.max_effect_number);
            printAll(pout, source);

            chain.Effect_Name(-1, pout);
            if (source == 0) hw.Print(pout);
            if (source == 1) 
            {
                uartPrintAnswer(pout);
            }

            for (i = 0; i < chain.max_effect_number; i++)
            {
                chain.Effect_Name(i, pout);
                if (source == 0) hw.Print(pout);
                if (source == 1) uartPrintAnswer(pout);
            }
            decoded     = 2;
		}
		//*********************************************** LFFG profiles
		if (strcmp(cmd, "pfl") == 0)
		{
            sprintf(pout, "-> %d \n", lffg.profiles_number);
            printAll(pout, source);

            for (i = 0; i < lffg.profiles_number; i++)
            {
                lffg.Printout(i, pout);
                if (source == 0) hw.Print(pout);
                if (source == 1) uartPrintAnswer(pout);
            }
            decoded     = 2;
		}
		//*********************************************** Time print
		if (strcmp(cmd, "out") == 0)
		{
			poutFlag = ~poutFlag;
            if (poutFlag) sprintf(pout, "->Duty Time on\n");
            else sprintf(pout, "->Duty Time off\n");
			decoded   = 1;
		}
		//*********************************************** Verbose
		if (strcmp(cmd, "vbs") == 0)
		{
			verbose_flag = ~verbose_flag;
            if (verbose_flag) sprintf(pout, "->Verbose on\n");
            else sprintf(pout, "->Verbose off\n");
            decoded     = 1;
            //if (verbose_flag) hw.PrintLine("->Verbose on");
            //else hw.PrintLine("->Verbose off");
			//decoded   = 2;
		}
		//*********************************************** Output type
		if (strcmp(cmd, "fmt") == 0)
		{
            i       = 0;
            if (fl[0] < -1.5 || fl[0] > 1.5) i  = 0.;
            if (fl[0] > -0.5 && fl[0] < 0.5) i  = 0;
            if (fl[0] > 0.5 && fl[0] < 1.5) i   = 1;
            out_list    = i;
            if (fl[0] < -0.5)
            {
                i    = -1;
                muted = true;
            }
            else    muted = false;

            sprintf(pout, "->FMT: Mute (-1) | Complete (0) | Data only (1) %ld\n", i);
            decoded     = 1;
		}
		//*********************************************** Verbose
		if (strcmp(cmd, "sep") == 0)
		{
			show_expression = !show_expression;
            decoded     = 2;
		}
        // #******************************************* Chain Printout
		if (chainf == 1)
		{
			chain.Printout(out_list, pout);
            decoded = 1;
		}
 		// #*********************************************** Output
		if (decoded == 1)
		{
            if (source == 0)
            {
                // Maximum hw.Print string length is 126 characters, no matter pout's size
                // but first line is shorter: 92 characters.
                if (strlen(pout) > 90)
                {
                    phal = pout[90];
                    pout[90] = 0;
                    hw.Print(pout);
                    if (strlen(pout) > 180)
                    {
                        pout[90] = phal;
                        phal = pout[180];
                        pout[180] = 0;
                        hw.Print(pout+90);
                        pout[180] = phal;
                        hw.Print(pout+180);
                    }
                    else
                    {
                        pout[90] = phal;
                        hw.Print(pout+90);
                    }
                }
                else
                {
                    hw.Print(pout);
                }
            }
            if (source == 1)
            {
                // question: does uart has also buffer size restriction?
                uartPrintAnswer(pout);
            }

            /*
            // Maximum hw.Print string length is 126 characters, no matter pout's size
            if (strlen(pout) > 120)
            {
                phal = pout[120];
                pout[120] = 0;
                if (source == 0) hw.Print(pout);
                if (source == 1) 
                {
                    uart1.BlockingTransmit(uart_com, 1);
                    uart1.BlockingTransmit(u_pout, strlen(pout));
                }
                pout[120] = phal;
                if (source == 0) hw.PrintLine(pout+120);
                if (source == 1) uart1.BlockingTransmit(u_pout+120, strlen(pout)-120);
            }
            else
            {
                if (source == 0) hw.PrintLine(pout);
                if (source == 1) 
                {
                    //hw.PrintLine("send echo: %s", pout);
                    uart1.BlockingTransmit(uart_com, 1);
                    uart1.BlockingTransmit(u_pout, strlen(pout));
                    //hw.PrintLine("send echo 0: %s", pout);
                }
            }
            */
		}
		//********************************************** Unknown command
		if (decoded == 0)
		{
            sprintf(pout, "-> ?\n");
            printAll(pout, source);
		}
	}
	else
    {
        sprintf(pout, "->Unknown\n");
        printAll(pout, source);
    }

    //hw.PrintLine("send echo 1: %s", pout);

    return;
}

// ****************************************************************************

char *CommandName(char ct[], char cmd[])
{
    /*
    Command name. Returns 0 if succeeded or -1 if it fails.
    Inputs:
      ct
        Input string with the command line to be decoded, in format "eff (s) p1 p2 p3 ...\0"
    Outputs
      cmd[]
        Effect name - eff (command). For instance: "ovd", "tml", "phr", "wah", etc.
        Any capital letter is changed to lower case: OvD -> ovd
      *st
        Pointer to the remainder of the ct string, after extraction of the first token (cmd)
    */
    
    int32_t k;
    char *st;
    
    st  = strtok(ct, " ,;");  // command token
    if (st != NULL)
    {
        strcpy(cmd, st);
        k     = 0;
        while (cmd[k] != 0 && (size_t)k < strlen(st))   // change token to lowercase
        {
            if (cmd[k] < 91 && cmd[k] > 64)   cmd[k] += 32;
            k++;
        }
    }
    
    return st; 
}

// ****************************************************************************

int32_t CommandDecoder(char ct[], int32_t *chn_change, int32_t *chn_code, 
                      float fl[], int32_t *fl_nb)
{
    /*
    Command decoder. Returns 0 if succeeded or -1 if it fails.
    Inputs:
      ct
        Input string with the command line to be decoded, in format "(s) p1 p2 p3 ...\0"
    Outputs
      chn_change
        Pointer to indicates if there is a change in chain, like to add or change the effect 
        position in chain (1), remove it (-1), or no change in position (0).
      chn_code
        Pointer to effect chain position s under parenthesis. It can be the required effect 
        position in chain, if greather than or equal to zero, or -1 if there is no changing 
        in effect position.
      fl[]
        Array of the effect parameters present in command
      fl_nb
        Pointer to the number of readen parameters in command.
      Example: "ovd (4) 1 0.95 0.72"
        cmd -> "ovd"
        chn_change -> 1
        chn_code -> 4 (position in chain)
        fl[0] -> 1  (switch)
        fl[1] -> 0.95 (parameter 1)
        fl[2] -> 0.72 (parameter 2)
        fl_nb -> 3
    */
    char *st, *sax;
    int32_t result;
    
    //st    = strtok(ct, " ,;"); // next token
    st    = strtok(NULL, " ,;"); // next token

    if (st != NULL) 
    {                           
        //hw.PrintLine("db3: %s\n", st);
        sax   = strchr(st, '(');  // check to see if there is a position token
        *chn_change   = 0;

        if (sax != NULL)
        {                         // yes, there is a position parameter
            *chn_code  = (int32_t)strtod(st+1, &sax);
            st   = strtok(NULL, " ,;");
            if (*chn_code < 0)
            {                       // remove this effect in chain
                *chn_change   = -1;
                *chn_code     = -*chn_code;
            }
            else
            {                       // add or change the effect position in chain
                *chn_change   = 1;
            }
        }
        else
        {                         // There is no position parameter
            *chn_code = -1; 
        }
        *fl_nb  = 0;
        while (st != NULL)
        {                         // Read all the numeric command data
            fl[*fl_nb]   = strtod(st, &sax);
            st   = strtok(NULL, " ,;");
            (*fl_nb)++;
        }
        result  = 0;
    }
    else
    {
        *chn_code     = -1;
        *chn_change   = 0;
        *fl_nb        = 0;
        result        = 0;
    }
    
    return result;
}

// ****************************************************************************
void ChangeEffectParams(float fl[], float fn[], int32_t nb)
{
    int32_t     i;
  
    for (i = 0; i < nb; i++) fn[i] = fl[i];
  
    return;
}

// ****************************************************************************

void    SendPotStruct(GSP_Pots *pots_)
{
    // To send the A command through UART to ESP32
    char        pot_com[24];
    uint32_t    i;

    // sending A command to ESP32 ...

    uartPrintPot(pot_clear);

    pot_com[0]  = 'A';
    
    for (i = 0; i < pots_->number_pots; i++)
    {
        pot_com[i+1]    = (char)pots_->pot_id[i] + 48;
	}

    pot_com[i+1] = '\n';
    pot_com[i+2] = 0;

    uartPrintPot(pot_com);

    return;
}
void debugPotStruct(GSP_Pots *pots_)
{
    char    dbg[60];
    uint32_t i;
    
    sprintf(dbg, " %ld \r\n", pots_->number_pots);
    uartDebug(dbg);

    for (i = 0; i < pots_->number_pots; i++)
    {
        sprintf(dbg, " %ld  %ld \r\n", pots_->effect_id[i], pots_->pot_id[i]);
        uartDebug(dbg);
    }

    return;
}

// ****************************************************************************

int8_t _SetPotGain(char ct[])
{
    int8_t      resl;
    uint16_t    ct_length;

    // Potentiometer data - P
    nb_pot  = expot.number_pots;

    if (ct[0] == 'P')
    {   
        if (ct[1] == nb_pot + 48)
        {
            ct_length   = strlen(ct);
            if (ct_length == 2*nb_pot + 2)
            {
                for (ipot = 0; ipot < nb_pot; ipot++)
                {
                    pot_2bytes[0]   = ct[2*ipot+2];
                    pot_2bytes[1]   = ct[2*ipot+3];
                    pot             = convert_from_2bytes(pot_2bytes);
                    pot             = pot*4;        // convert 14 bits to 16 bits
                    pot_effect      = expot.effect_id[ipot];

                    switch (pot_effect)
                    {
                        case GSP_VOL: 
                            vol.lfo.SetGain((uint32_t)pot);
                            break;
                        case GSP_TML:
                            tml.lfo.SetGain((uint32_t)pot);
                            break;
                        case GSP_WAH:
                            wah.lfo.SetGain((uint32_t)pot);
                            break;
                        case GSP_VBT:
                            vbt.lfo.SetGain((uint32_t)pot);
                            break;
                        case GSP_CHS:
                            chs.lfo.SetGain((uint32_t)pot);
                            break;
                        case GSP_PHR:
                            phr.lfo.SetGain((uint32_t)pot);
                            break;
                        default:
                            break;
                    }
                }
                resl    = 0;
            }
            else
            {
                resl    = 3;
            }
        }
        else
        {
            resl    = 2;
        }
    }
    else 
    {
        resl    = 1;
    }
    return resl;
}

// ****************************************************************************

uint16_t convert_from_2bytes(uint8_t lsb_msb[])
{
  /*
    Function to convert two unsigned bytes with 7 significant 
    bits each stored in lsb_msb[2] (less significant byte and 
    most significant byte) to a 14 bit unsigned integer number. 
    The left most bit (8th) of both bytes in lsb_msb will be
    disregarded.
  */
  return ((127 & lsb_msb[1]) << 7) | (127 & lsb_msb[0]);
}

// ****************************************************************************

void printAll(char st[], uint8_t input_source)
{

    if (input_source == 0) hw.Print(st);
    if (input_source == 1) 
    {
        uartPrintAnswer(st);
    }

    return;
}

// ****************************************************************************

void uartPrintAnswer(char str[])
{
    // This function must be used to send messages to UART. It is required because
    // after sending the message a fine tuned delay must be done in order to avoid
    // loosing messages.

    uart1.BlockingTransmit((uint8_t*)disambig_com, 1);
    uart1.BlockingTransmit((uint8_t*)str, strlen(str));
    // Delay is necessary to avoid missing messages. The problem can
    // be in Daisy Seed that can't send a second message before the last
    // one has been sent, or in the ESP32 that miss the message that arrives
    // before the last one has been processed
    System::Delay(100);

}

// ****************************************************************************

void uartSendString(const char str[])
{
    // This function can be used to send messages to UART for debuging purposes. 

    uart1.BlockingTransmit((uint8_t*)disambig_com, 1);
    uart1.BlockingTransmit((uint8_t*)str, strlen(str));
    System::Delay(100);

}

// ****************************************************************************

void uartPrintPot(char str[])
{
    // This function must be used to send messages to UART. It is required because
    // after sending the message a fine tuned delay must be done in order to avoid
    // loosing messages.

    uart1.BlockingTransmit((uint8_t*)disambig_pot, 1);
    uart1.BlockingTransmit((uint8_t*)str, strlen(str));
    // Delay is necessary to avoid missing messages. The problem can
    // be in Daisy Seed that can't send a second message before the last
    // one has been sent, or in the ESP32 that miss the message that arrives
    // before the last one has been processed
    System::Delay(100);

}

// ****************************************************************************

void uartDebug(char str[])
{
    // This function must be used to send debug messages to UART. It is required because
    // after sending the message a fine tuned delay must be done in order to avoid
    // loosing messages.

    uint8_t    debug = '#';
    uart1.BlockingTransmit(&debug, 1);
    uart1.BlockingTransmit((uint8_t*)str, strlen(str));
    System::Delay(100);

}

// ****************************************************************************

void uartDebug(const char str[])
{
    // This function must be used to send debug messages to UART. It is required because
    // after sending the message a fine tuned delay must be done in order to avoid
    // loosing messages.

    uint8_t    debug = '#';
    uart1.BlockingTransmit(&debug, 1);
    uart1.BlockingTransmit((uint8_t*)str, strlen(str));
    System::Delay(100);

}
