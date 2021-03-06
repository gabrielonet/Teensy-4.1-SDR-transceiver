// Libraries
#include <Wire.h>                          // I2C comms library
#include <si5351.h>                       // Si5351Jason library
#include <LiquidCrystal_I2C.h>             // LCD library
#include <Audio.h>                         // Teensy audio library

// Number of Filter Coefficients
#define NO_HILBERT_COEFFS 200               // Used to define the Hilbert transform filter arrays. More typical than 'const int'.

// Define Constants and Vaviables
static const long bandStart = 1000000;     // start of HF band
static const long bandEnd =   30000000;    // end of HF band
static const long bandInit =  7000000;     // where to initially set the frequency
volatile long oldfreq = 0;
volatile long freq = bandInit ;
volatile long radix = 1;                // how much to change the frequency by clicking the rotary encoder will change this.
volatile int updatedisplay = 0;
int freq_length_0 = 7;

// Rotary Encoder
static const int pushPin = 39;
static const int rotBPin = 36;
static const int rotAPin = 35;
volatile int rotState = 0;
volatile int rotAval = 1;
volatile int rotBval = 1;
volatile int rotAcc = 0;


// Iowa Hills Hilbert transform filter coefficients
const short Hilbert_Plus_45_Coeffs[NO_HILBERT_COEFFS] = {
  (short)(32768 * -0.000287988910943357),
  (short)(32768 * -0.000383511439791303),
  (short)(32768 * -0.000468041804899774),
  (short)(32768 * -0.000529324432676899),
  (short)(32768 * -0.000569479602046985),
  (short)(32768 * -0.000616670267768531),
  (short)(32768 * -0.000731530748681977),
  (short)(32768 * -0.001002372095321225),
  (short)(32768 * -0.001525299390682192),
  (short)(32768 * -0.002370114347025230),
  (short)(32768 * -0.003539247773172147),
  (short)(32768 * -0.004932965382552984),
  (short)(32768 * -0.006337182914262393),
  (short)(32768 * -0.007448193692118567),
  (short)(32768 * -0.007940501940620482),
  (short)(32768 * -0.007570802072162988),
  (short)(32768 * -0.006296120449841751),
  (short)(32768 * -0.004371955618154949),
  (short)(32768 * -0.002391875073164555),
  (short)(32768 * -0.001236984700413469),
  (short)(32768 * -0.001922560128827416),
  (short)(32768 * -0.005356720327533458),
  (short)(32768 * -0.012055656297010635),
  (short)(32768 * -0.021882952959947619),
  (short)(32768 * -0.033888748300090733),
  (short)(32768 * -0.046312736456333638),
  (short)(32768 * -0.056783367797647665),
  (short)(32768 * -0.062699937453677912),
  (short)(32768 * -0.061735375084135742),
  (short)(32768 * -0.052358513976237808),
  (short)(32768 * -0.034257179158167443),
  (short)(32768 * -0.008554500746482946),
  (short)(32768 * 0.022249911747384360),
  (short)(32768 * 0.054622962942346594),
  (short)(32768 * 0.084568844473140448),
  (short)(32768 * 0.108316122839950818),
  (short)(32768 * 0.122979341462627859),
  (short)(32768 * 0.127056096658453188),
  (short)(32768 * 0.120656295327679283),
  (short)(32768 * 0.105420364259485699),
  (short)(32768 * 0.084152608145489444),
  (short)(32768 * 0.060257510644444748),
  (short)(32768 * 0.037105711921879434),
  (short)(32768 * 0.017464092086704748),
  (short)(32768 * 0.003100559033325746),
  (short)(32768 * -0.005373489802481697),
  (short)(32768 * -0.008418211280310166),
  (short)(32768 * -0.007286730644726664),
  (short)(32768 * -0.003638388931163832),
  (short)(32768 * 0.000858330713630433),
  (short)(32768 * 0.004847436504682235),
  (short)(32768 * 0.007476399317750315),
  (short)(32768 * 0.008440227567663121),
  (short)(32768 * 0.007898970420636600),
  (short)(32768 * 0.006314366257036837),
  (short)(32768 * 0.004261033495040515),
  (short)(32768 * 0.002261843500794377),
  (short)(32768 * 0.000680212977485724),
  (short)(32768 * -0.000319493110301691),
  (short)(32768 * -0.000751893569425181),
  (short)(32768 * -0.000752248417868501),
  (short)(32768 * -0.000505487955986662),
  (short)(32768 * -0.000184645628631330),
  (short)(32768 * 0.000087913008490067),
  (short)(32768 * 0.000253106348867209),
  (short)(32768 * 0.000306473486382603),
  (short)(32768 * 0.000277637042003169),
  (short)(32768 * 0.000207782317481292),
  (short)(32768 * 0.000132446796990356),
  (short)(32768 * 0.000072894261560354)
};

// Iowa Hills Hilbert transform filter coefficients
const short Hilbert_Minus_45_Coeffs[NO_HILBERT_COEFFS] = {
  (short)(32768 * -0.000072894261560345),
  (short)(32768 * -0.000132446796990344),
  (short)(32768 * -0.000207782317481281),
  (short)(32768 * -0.000277637042003168),
  (short)(32768 * -0.000306473486382623),
  (short)(32768 * -0.000253106348867259),
  (short)(32768 * -0.000087913008490148),
  (short)(32768 * 0.000184645628631233),
  (short)(32768 * 0.000505487955986583),
  (short)(32768 * 0.000752248417868491),
  (short)(32768 * 0.000751893569425298),
  (short)(32768 * 0.000319493110301983),
  (short)(32768 * -0.000680212977485245),
  (short)(32768 * -0.002261843500793748),
  (short)(32768 * -0.004261033495039842),
  (short)(32768 * -0.006314366257036280),
  (short)(32768 * -0.007898970420636345),
  (short)(32768 * -0.008440227567663343),
  (short)(32768 * -0.007476399317751102),
  (short)(32768 * -0.004847436504683540),
  (short)(32768 * -0.000858330713632029),
  (short)(32768 * 0.003638388931162351),
  (short)(32768 * 0.007286730644725833),
  (short)(32768 * 0.008418211280310565),
  (short)(32768 * 0.005373489802483816),
  (short)(32768 * -0.003100559033321630),
  (short)(32768 * -0.017464092086698697),
  (short)(32768 * -0.037105711921871905),
  (short)(32768 * -0.060257510644436532),
  (short)(32768 * -0.084152608145481672),
  (short)(32768 * -0.105420364259479538),
  (short)(32768 * -0.120656295327675800),
  (short)(32768 * -0.127056096658453216),
  (short)(32768 * -0.122979341462631633),
  (short)(32768 * -0.108316122839958146),
  (short)(32768 * -0.084568844473150454),
  (short)(32768 * -0.054622962942358168),
  (short)(32768 * -0.022249911747396132),
  (short)(32768 * 0.008554500746472333),
  (short)(32768 * 0.034257179158159054),
  (short)(32768 * 0.052358513976232306),
  (short)(32768 * 0.061735375084133286),
  (short)(32768 * 0.062699937453678217),
  (short)(32768 * 0.056783367797650072),
  (short)(32768 * 0.046312736456337288),
  (short)(32768 * 0.033888748300094730),
  (short)(32768 * 0.021882952959951244),
  (short)(32768 * 0.012055656297013388),
  (short)(32768 * 0.005356720327535105),
  (short)(32768 * 0.001922560128828006),
  (short)(32768 * 0.001236984700413229),
  (short)(32768 * 0.002391875073163812),
  (short)(32768 * 0.004371955618154038),
  (short)(32768 * 0.006296120449840938),
  (short)(32768 * 0.007570802072162439),
  (short)(32768 * 0.007940501940620253),
  (short)(32768 * 0.007448193692118624),
  (short)(32768 * 0.006337182914262643),
  (short)(32768 * 0.004932965382553323),
  (short)(32768 * 0.003539247773172483),
  (short)(32768 * 0.002370114347025498),
  (short)(32768 * 0.001525299390682370),
  (short)(32768 * 0.001002372095321316),
  (short)(32768 * 0.000731530748682004),
  (short)(32768 * 0.000616670267768521),
  (short)(32768 * 0.000569479602046963),
  (short)(32768 * 0.000529324432676881),
  (short)(32768 * 0.000468041804899765),
  (short)(32768 * 0.000383511439791304),
  (short)(32768 * 0.000287988910943362)
};


// Instantiate the Objects
LiquidCrystal_I2C lcd(0x27, 16, 2);       // Name for the LCD. Set the LCD address to either 0x27 or 0x3F for a 16 chars and 2 line display
Si5351 si5351;                            // Name for the Si5351 DDS
AudioControlSGTL5000    audioShield;      // Name for the Teensy audio CODEC on the audio shield

// Audio shield
AudioInputI2S           audioInput;                                           // Name for the input to the audio shield
AudioOutputI2S          audioOutput;                                          // Name for the output of the audio shield
// Receiver
AudioFilterFIR          RX_Hilbert_Plus_45;                                   // Name for the RX +45 Hilbert transform
AudioFilterFIR          RX_Hilbert_Minus_45;                                  // Name for the RX +45 Hilbert transform
AudioMixer4             RX_Summer;                                            // Name for the RX summer

// Audio connections
AudioConnection         patchCord5(audioInput, 0, RX_Hilbert_Plus_45, 0);     // Left channel in Hilbert transform +45
AudioConnection         patchCord10(audioInput, 1, RX_Hilbert_Minus_45, 0);   // Right channel in Hilbert transform -45
AudioConnection         patchCord15(RX_Hilbert_Plus_45, 0, RX_Summer, 0);     // Hilbert transform +45 to receiver summer
AudioConnection         patchCord20(RX_Hilbert_Minus_45, 0, RX_Summer, 1);    // Hilbert transform -45 to receiver summer
AudioConnection         patchCord25(RX_Summer, 0, audioOutput, 0);            // Receiver summer to receiver LPF


void setup()
{
  // Setup input switches
  pinMode(rotAPin, INPUT);
  pinMode(rotBPin, INPUT);
  pinMode(pushPin, INPUT);
  digitalWrite(rotAPin, HIGH);
  digitalWrite(rotBPin, HIGH);
  digitalWrite(pushPin, HIGH);
  Serial.begin(9600);

  // Setup interrupt pins
  attachInterrupt(digitalPinToInterrupt(rotAPin), ISRrotAChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotBPin), ISRrotBChange, CHANGE);

  // Setup the lcd
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("YO8RXP SDR");

  // Setup the DDS
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0,0);
  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.set_freq((freq * 100ULL), SI5351_CLK0);

  // Setup the audio shield
  AudioNoInterrupts();
  AudioMemory(16);
  audioShield.enable();
  audioShield.volume(0.7);            // Constant. Use external volume control on the audio amp
  AudioInterrupts();

  // Setup transceiver mode
  Turn_On_Receiver();
  UpdateDisplay();
}

void loop()
{
  if (freq != oldfreq)                // Check to see if the frequency has changed. If so, update everything.
  {
    UpdateDisplay();
    SendFrequency();
    oldfreq = freq;
  }

  if (digitalRead(pushPin) == LOW)    // Update cursor, but also stop it from flickering
  {
    while (digitalRead(pushPin) == LOW)
    {
      if (updatedisplay == 1)
      {
        UpdateDisplay();
        updatedisplay = 0;
      }
    }
  }
}


void Turn_On_Receiver()
{
  AudioNoInterrupts();
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);
  audioShield.lineInLevel(5);                                               // Default is 5
  audioShield.unmuteHeadphone();
  RX_Hilbert_Plus_45.begin(Hilbert_Plus_45_Coeffs, NO_HILBERT_COEFFS);
  RX_Hilbert_Minus_45.begin(Hilbert_Minus_45_Coeffs, NO_HILBERT_COEFFS);

  if (freq <= 9999999)          // LSB
  {
    RX_Summer.gain(0, 1);
    RX_Summer.gain(1, -1);
  }
  if (freq > 9999999)           // USB
  {
    RX_Summer.gain(0, 1);
    RX_Summer.gain(1, 1);
  }

  AudioInterrupts();
}


// Interrupt routines
void ISRrotAChange()
{
  
  if (digitalRead(rotAPin))
  {
    rotAval = 1;
    UpdateRot();
  }
  else
  {
    rotAval = 0;
    UpdateRot();
  }
}


void ISRrotBChange()
{
  if (digitalRead(rotBPin))
  {
    rotBval = 1;
    UpdateRot();
  }
  else
  {
    rotBval = 0;
    UpdateRot();
  }
}


void UpdateRot()

{


  switch (rotState)
  {

    case 0:                                         // Idle state, look for direction
      if (!rotBval)
        rotState = 1;                               // CW 1
      if (!rotAval)
        rotState = 11;                              // CCW 1
      break;

    case 1:                                         // CW, wait for A low while B is low
      if (!rotBval)
      {
        if (!rotAval)
        {
          // either increment radixindex or freq
          if (digitalRead(pushPin) == LOW)
          {
            updatedisplay = 1;
            if (radix == 1000000)
              radix = 100000;
            else if (radix == 100000)
              radix = 10000;
            else if (radix == 10000)
              radix = 1000;
            else if (radix == 1000)
              radix = 100;
            else if (radix == 100)
              radix = 10;
            else if (radix == 10)
              radix = 1;
            else
              radix = 1000000;
          }
          else
          {
            freq = (freq + radix);
            if (freq > bandEnd)
              freq = bandEnd;
          }
          rotState = 2;                             // CW 2
        }
      }
      else if (rotAval)
        rotState = 0;                               // It was just a glitch on B, go back to start
      break;

    case 2:                                         // CW, wait for B high
      if (rotBval)
        rotState = 3;                               // CW 3
      break;

    case 3:                                         // CW, wait for A high
      if (rotAval)
        rotState = 0;                               // back to idle (detent) state
      break;

    case 11:                                        // CCW, wait for B low while A is low
      if (!rotAval)
      {
        if (!rotBval)
        {
          // either decrement radixindex or freq
          if (digitalRead(pushPin) == LOW)
          {
            updatedisplay = 1;
            if (radix == 1)
              radix = 10;
            else if (radix == 10)
              radix = 100;
            else if (radix == 100)
              radix = 1000;
            else if (radix == 1000)
              radix = 10000;
            else if (radix == 10000)
              radix = 100000;
            else if (radix == 100000)
              radix = 1000000;
            else
              radix = 1;
          }
          else
          {
            freq = (freq - radix);
            if (freq < bandStart)
              freq = bandStart;
          }
          rotState = 12;                            // CCW 2
        }
      }
      else if (rotBval)
        rotState = 0;                               // It was just a glitch on A, go back to start
      break;

    case 12:                                        // CCW, wait for A high
      if (rotAval)
        rotState = 13;                              // CCW 3
      break;

    case 13:                                        // CCW, wait for B high
      if (rotBval)
        rotState = 0;                               // back to idle (detent) state
      break;
  }

}


void UpdateDisplay()
{
  int freq_length = String(freq).length();
   if (freq_length_0 != freq_length )
  {
      lcd.setCursor(0, 0);
      lcd.print("                ");   // Clear display 
      freq_length_0 = freq_length ;
  }

  String mhz = String(freq).substring(0,1);
  String khz = String(freq).substring(1,4);
  String hz = String(freq).substring(4,7);

  lcd.setCursor(0, 0);
  lcd.print(mhz + "," + khz + "."+ hz + " Mhz");
  
}


void SendFrequency()
{
  //si5351.set_freq((freq * 4) * 100ULL , SI5351_CLK0);

}
