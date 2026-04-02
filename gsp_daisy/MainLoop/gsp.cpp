// Title: Guitar Sound Processing - GSP
// Description: Sound effects for guitar
// Hardware: Daisy Seed
// Author: Valdemir Carrara
// main gsp

// Guitar Sound Processing
// ****************************************************************************
#include "daisy_seed.h"
#include "guitar_dsp.h"

using namespace daisy;

#define   BUFFER_SIZE   262144  
#define   REV_BUFSIZE   8192

// ****************************************************************************
static    DaisySeed   hw;

//HardwareSerial  SerialCom(14UL, 13UL); // USART1  Pins 14 (Rx) and 13 (Tx)
//>>>HardwareSerial  SerialPot(11UL, 12UL); // UART4  Pins 11 (Rx) and 12 (Tx)

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

// Built-in LED
bool        ledstate = false;

uint8_t     inp_source = 0;

// USB-Serial
char        usb_buff[256];
static uint32_t   usb_buf_ready = 0;
char        st[64];

// UART     Communicate to ESP32
UartHandler         uart;
UartHandler::Config uart_config;
char                uart_buff[256];
uint8_t             uart_ready = 0, uart_len = 0;
uint8_t             uart_cs, uart_comm = 0;
uint8_t             *u_st;
uint8_t             uart_com[4] = {123, 10, 13, 0}; // {'{', '\r', '\n', '\0'}} 

// Potentiometers
char                nb_pot = 0;
uint8_t             send_pot_data;
uint8_t             ipot = 0, pot_aux, pot_data[20];
uint32_t            pot_effect, pot_fr = 0;
uint8_t             pot_start[]     = "}S\n";
uint8_t             pot_clear[]     = "}C\n";

//uint8_t             u_presult[64], icon;
//char                presult[64];

union     TwoBytes 
{
  uint16_t full;
  struct 
  {
    uint8_t low, high;
  };
} pot;

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

// ****************************************************************************
// Prototypes

void    loop() ;
void    _Setcommand(char ct[], uint8_t source);
char    *CommandName(char ct[], char cmd[]);
int32_t CommandDecoder(char ct[], int32_t *chn_chg, 
        int32_t *chn_code, float fl[], int32_t *fl_nb);
int8_t  PotDecoder(GSP_SignalChain *chain_, char ct[],  
        int32_t* effect_number, int32_t* pot_number);
int32_t CommandDecoder(char ct[], char cmd[], int32_t *chn_code, float fl[], int32_t *fl_nb);
void    ChangeEffectParams(float fl[], float fn[], int32_t nb);
void    SendPotStruct(GSP_Pots *pots_);

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
    uart.Init(uart_config);
    uart.DmaReceiveFifo();      // 2026
    
    //System::Delay(5000);

    // ============================================================================
    // GSP
    send_pot_data   = false;
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

    u_st    = reinterpret_cast<uint8_t*>(const_cast<char*>(st));
 
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
            sprintf(st, "%f\r\n", duty);
            if (inp_source == 0) hw.PrintLine(st);
            if (inp_source == 1) 
            {
                //uart.PollTx(uart_com, 1);
                //uart.PollTx(u_st, strlen(st));
                uart.BlockingTransmit(uart_com, 1);
                uart.BlockingTransmit(u_st, strlen(st));
            }
            // Print the maximum and minimum ADC sampled value
            //hw.PrintLine("Max %ld,  Min %ld", smp_max, smp_min);

            // Print the last potentiometer data
            //hw.Print("Pot data: ");
            //hw.PrintLine(" eff: %ld  value: %d ", pot_effect, pot.full);
    
            smp_max     = 0;
            smp_min     = 0;
        }
        tend        = 0;

        // Blink led to signal processor activity
        hw.SetLed(ledstate);
        ledstate = !ledstate;

        //hw.PrintLine("Pot frequency: %ld", pot_fr);
        pot_fr      = 0;
        // Send request for potentimeter data
        if (send_pot_data)
        {
            uart.BlockingTransmit(pot_start, 3);
            System::Delay(1);
            ipot = 0;
            send_pot_data   = false;
        }

    }

    // ----------------------------------------------------------------------
    //          Data from ESP32 UART

    if (uart.ReadableFifo())
    {
        // Character to choose among Effect Command (123 {) and Potentiometer (125 })
        // Get data coming from ESP32 
        //uart_cs = uart.PopRx();
		uart_cs = uart.PopFifo();
        if (uart_ready == 0)
        {
            if (uart_cs == 123)
            {
                uart_comm   = 1; // eff command
                uart_ready  = 1;
            } 
            if (uart_cs == 125) 
            {
                uart_comm   = 2; // pot command
                uart_ready  = 1;
            }
        }
        else
        {
            //hw.PrintLine("-> Received 1: %c %d", (char)uart_cs, uart_cs);
            if (uart_comm == 1)
            {
                if (uart_cs == 10 || uart_cs == 13)
                {
                    uart_buff[uart_len]     = 0;
                    uart_ready  = 0;
                    uart_len    = 0;
                    uart_comm   = 0;
                    inp_source  = 1;
                    _Setcommand(uart_buff, 1);
                }
                else
                {
                    uart_buff[uart_len]     = uart_cs;
                    uart_len++;
                    if (uart_len > 128) uart_comm = 0;
                }
            }
            if (uart_comm == 2)
            {
                // Potentiometer data - P
                if (uart_cs == 'P')
                {
                    uart_comm   = 3;
                }
                else uart_ready = 0;
            }
            else 
            {
                if (uart_comm == 3)
                {
                    // Potentiometer data - number_pot
                    if (uart_cs == nb_pot)
                    {
                        uart_comm   = 4;
                    }
                    else uart_ready = 0;
                }
                else
                {
                    if (uart_comm == 4)
                    {
                        pot_data[ipot]  = uart_cs;
                        ipot++;
                        if (ipot >= 2*nb_pot && uart_cs == 13)
                        {
                            //hw.Print("Pot data: ");
                            for (ipot = 0; ipot < nb_pot; ipot++)
                            {
                                pot_fr++;
                                pot_aux   = 2*ipot;
                                pot.low   = pot_data[pot_aux];
                                pot.high  = pot_data[pot_aux + 1];
                                pot_effect      = expot.effect_id[ipot];
                                if (pot_effect == GSP_PHR)  phr.lfo.SetGain((uint32_t)pot.full);
                                if (pot_effect == GSP_WAH)  wah.lfo.SetGain((uint32_t)pot.full);
                                if (pot_effect == GSP_CHS)  chs.lfo.SetGain((uint32_t)pot.full);
                                if (pot_effect == GSP_VBT)  vbt.lfo.SetGain((uint32_t)pot.full);
                                if (pot_effect == GSP_TML)  tml.lfo.SetGain((uint32_t)pot.full);
                                if (pot_effect == GSP_VOL)  vol.lfo.SetGain((uint32_t)pot.full);
                                //hw.Print(" eff: %ld  value: %d ", pot_effect, pot.full);
                            }
                            ipot  = 0;
                            uart_ready      = 0;
                            //hw.PrintLine(" ");
                            //uart.PollTx("S", 2);
                        }
                    }
                }
            }
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

/*
    if(!uart.RxActive())
    {
        hw.PrintLine("UART reset");
        uart.FlushRx();
        uart.StartRx();
    }
*/

    return;
}

// ****************************************************************************

void _Setcommand(char ct[], uint8_t source)
{
    // ct       Effect setup string
    // source   Command source: 0=Daisy Seed USB, 1=ESP32 UART
    uint32_t    i;
    int8_t      chainf, decoded, gpot;
    uint8_t     pot_com[24];
    int32_t     ceff, pos;
    int32_t     effect_n;
    int32_t     pot_id;
    char        cmd[8], pout[250], pname[32], phal, *stc;
    float       fn[8] = {0, 0, 0, 0, 0, 0, 0, 0}, fl[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int32_t     cdec, fl_nb;
    uint8_t     *u_ct;
    uint8_t     *u_pout;

    decoded     = 0;
    chainf      = 0;
    gpot        = 0;
    
    if (source == 1)
    {
        //https://stackoverflow.com/questions/51640225/how-to-cast-const-uint8-t-to-char
        u_ct    = reinterpret_cast<uint8_t*>(const_cast<char*>(ct));
        u_pout  = reinterpret_cast<uint8_t*>(const_cast<char*>(pout));
    }

    if (verbose_flag)
    {
        if (source == 0)
        {
            hw.Print("<-");
            hw.PrintLine(ct);
        }
        if (source == 1)
        {
            pot_com[0]  = '<';
            pot_com[1]  = '-';
            uart.BlockingTransmit(uart_com, 1);
            uart.BlockingTransmit(pot_com, 2);
            uart.BlockingTransmit(u_ct, strlen(ct));
            uart.BlockingTransmit(uart_com+1, 1);
            System::Delay(1);
        }
        hw.PrintLine("---> %s", ct);
    }

    stc     = CommandName(ct, cmd);

    cdec    = -1;
    ceff    = 0;

    if (stc != NULL)
    {
        if (strcmp(cmd, "pot") != 0)
        {
            cdec     = CommandDecoder(stc, &ceff, &pos, fl, &fl_nb);
            if (ceff != 0) chainf = 1;
        }
        else cdec   = 0;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
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
			decoded   = 1;
		}

		// ************************************* External potentiometer
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
                    
                        chain.Name(effect_n, pname);
                        sprintf(pout, "->POT: Effect: %s | Potentiometer ID: %ld\r\n",
                                pname, pot_id);

                        if (source == 0) hw.PrintLine(pout);
                        if (source == 1) 
                        {
                            uart.BlockingTransmit(uart_com, 1);
                            uart.BlockingTransmit(u_pout, strlen(pout));
                        }
                        decoded = 2; // no error
                        gpot    = 0; // no pot printing
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
                    // gpot = -1, something goes wrong, command not decoded
                    gpot        = 0; // no pot printing
                    decoded     = 0;
                }
			}

            if (gpot > 0)
            {
                if (expot.number_pots == 0)
                {
                    sprintf(pout, "->POT <none>");
                    if (source == 0) hw.PrintLine(pout);
                    if (source == 1) 
                    {
                        uart.BlockingTransmit(uart_com, 1);
                        uart.BlockingTransmit(u_pout, strlen(pout));
                    }
                }
                else
                {
                    for (i = 0; i < expot.number_pots; i++)
                    {
                        chain.Name(expot.effect_id[i], pname);
                        sprintf(pout, "->POT: Effect: %s | Potentiometer ID: %ld\r\n",
                                pname, expot.pot_id[i]);
                        if (source == 0) hw.PrintLine(pout);
                        if (source == 1) 
                        {
                            uart.BlockingTransmit(uart_com, 1);
                            uart.BlockingTransmit(u_pout, strlen(pout));
                        }
                    }
                }
                decoded = 2;
            }
		}

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
            if (fl[0] < 0 || fl[0] > 1) fl[0]   = 0;
            out_list    = fl[0];
            sprintf(pout, "->FMT: Complete(0)|Data only(1) %d\n", out_list);
            decoded     = 1;
		}
        // #******************************************* Chain Printout
		if (chainf == 1)
		{
			chain.Printout(pout);
            decoded = 1;
		}
 		// #*********************************************** Output
		if (decoded == 1)
		{
            if (strlen(pout) > 120)
            {
                phal = pout[120];
                pout[120] = 0;
                if (source == 0) hw.Print(pout);
                if (source == 1) 
                {
                    uart.BlockingTransmit(uart_com, 1);
                    uart.BlockingTransmit(u_pout, strlen(pout));
                }
                pout[120] = phal;
                if (source == 0) hw.PrintLine(pout+120);
                if (source == 1) uart.BlockingTransmit(u_pout+120, strlen(pout)-120);
            }
            else
            {
                if (source == 0) hw.PrintLine(pout);
                if (source == 1) 
                {
                    //hw.PrintLine("send echo: %s", pout);
                    uart.BlockingTransmit(uart_com, 1);
                    uart.BlockingTransmit(u_pout, strlen(pout));
                    //hw.PrintLine("send echo 0: %s", pout);
                }
            }
		}
		//********************************************** Unknown command
		if (decoded == 0)
		{
            if (source == 0) hw.PrintLine("-> ?");
            if (source == 1) 
            {
                sprintf(pout, "-> ?\n");
                uart.BlockingTransmit(uart_com, 1);
                uart.BlockingTransmit(u_pout, strlen(pout));
            }
//>>>			Serial1.println("-> ?");
		}
	}
	else
    {
        hw.PrintLine("-> ?");
//>>>   Serial1.println("-> ?");
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
        while (cmd[k] != 0 && (size_t)k < strlen(st))   // change token to lowcase
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

int8_t PotDecoder(GSP_SignalChain *chain_, char ct[],  
        int32_t* effect_number, int32_t* pot_number)
{
    /*
    To define a link between effect and volume pedal
    chain_:
        Pointer to the signal chain effects
    ct:
        String with the pot command in ASCII to be decoded, in the format 'eff' potnb
    effect_number
        Returning pointer to the selected effect 'eff'
    pot_number
        Returning pointer to the potentiometer number potnb
    Returns 0 if successfully decoded the pot command, or 1 if the command is empty,
    or -1 if failed.
    */
    
    char      *st, *clast;
    
	//st  = strtok(ct, " ,;");  // effect token
	st  = strtok(NULL, " ,;");  // effect token
    
	if (st != NULL)
	{
        //hw.PrintLine(st);
		*effect_number   = chain_->Number(st);
        //hw.PrintLine("effect_number = %ld", *effect_number);

		if (*effect_number < 0) return -1;

		st    = strtok(NULL, " ,;"); // potentiometer identifier
		if (st != NULL)
		{
			*pot_number  = (int32_t)strtod(st, &clast);
		}
		else return -1;
	}
	else return 1;

	return 0;
}

// ****************************************************************************

void    SendPotStruct(GSP_Pots *pots_)
{
    // To send the A command through UART to ESP32
    uint8_t     pot_com[24];
    uint32_t    i;

    // sending A command to ESP32 ...

    uart.PollTx(pot_clear, 3);
    System::Delay(1);
   
    pot_com[0]  = '}';
    pot_com[1]  = 'A';
	//SerialPot.write('A');
    
    //hw.PrintLine("number_pots: %ld", chain_->number_pots);  // to be removed
	
    for (i = 0; i < pots_->number_pots; i++)
    {
        pot_com[i+2]    = (uint8_t)pots_->pot_id[i] + 48;
		//SerialPot.write(chain.seq_to_pot_id[i]+48);
	}
    pot_com[i+2] = '\n';
    uart.BlockingTransmit(pot_com, i+3);
    System::Delay(1);
    //hw.PrintLine("Sent pot A command to ESP");
    //hw.PrintLine("Command A to ESP: %s", pot_com);  // to be removed
	//SerialPot.write('\n');

    return;
}
