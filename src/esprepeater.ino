/**
 * @file esprepeater.ino
 * @brief Simplex (parrot) radio repeater
 * @author Philippe MOUGIN
 * @copyright GPLv3
 *  Dependencies: 
 * AudioTools library by Phil Schatzmann  https://github.com/pschatzmann/arduino-audio-tools
 * AudioDriver library by Phil Schatzmann https://github.com/pschatzmann/arduino-audio-driver.git
 */
#include <Arduino.h>

// audio tools library import https://github.com/pschatzmann/arduino-audio-tools
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/AudioLibs/MemoryManager.h"
#include "AudioTools/CoreAudio/BaseStream.h"
// #include "Morse.h"


// Seconds before recording
#define WAIT_BEFORE_REC 1
// Seconds before playing the recorded sound
#define WAIT_BEFORE_PLAY 1
// Beep duration in seconds
#define DURATION_BEEP1 1   // secondes
// Beep1 frequency in Hz (E5)
#define BEEP1_FREQ  659.0
// record lenght in ms (to be adjusted according to the memory available on your board)    
#define RECORD_MS  5000   
// input jack (see board reference)
#define INPUT_LINE ADC_INPUT_LINE2

// Audiokit ESP32 board type
// See https://github.com/pschatzmann/arduino-audio-driver/blob/main/src/AudioBoard.h for available boards and how to define your own
#define AUDIO_BOARD_TYPE AudioKitAC101


const uint32_t SAMPLE_RATE = 8000; 
const uint8_t CHANNELS = 1;        
const uint8_t BITS_PER_SAMPLE = 16;
const uint16_t RECORD_THRESHOLD = 80;

const size_t BUFFER_SIZE = (SAMPLE_RATE * CHANNELS * sizeof(int16_t) * RECORD_MS) / 1000;
// threshold to stop recording when silence is detected
const int SILENCE_THRESHOLD = 200; 


AudioBoardStream kit(AUDIO_BOARD_TYPE); // kit driver for AudioKit with AC101 codec : change to appropriate board if you use another one (see
DynamicMemoryStream recording(true); // buffer to record into RAM
StreamCopy recorder;                 // copies from mic → memory or memory → codec

MultiOutput multiout;
VolumeMeter volumemeter; // to measure the volume of the recorded signal
StreamCopy multirecorder(multiout, kit);

/////////////////////// beep
const int TOTAL_SAMPLES_BEEP1 = SAMPLE_RATE * DURATION_BEEP1;
// PCM buffer in memory. Samples are 16-bit signed integers
int16_t beep1_buffer[TOTAL_SAMPLES_BEEP1]; 
// Stream objects
MemoryStream memStream;

// Output stream to AudioKit
const AudioInfo info(SAMPLE_RATE, CHANNELS, BITS_PER_SAMPLE);
bool soundDetected = false;

// Buffer to hold a chunk of mic data
static const uint32_t MIC_CHUNK = 256;
static int16_t micbuf[MIC_CHUNK];


static bool soundAboveThreshold(int16_t *buf, size_t len)
{
  long sum = 0;
  for (size_t i = 0; i < len; i++)
  {
    int v = buf[i];
    sum += (v < 0) ? -v : v;
  }
  long avg = sum / len;
  // Serial.println(avg);
  return (avg > RECORD_THRESHOLD);
}

void prepareBeep1()
{
  for (int i = 0; i < TOTAL_SAMPLES_BEEP1; i++)
  {
    float t = (float)i / SAMPLE_RATE;
    float s = sin(2.0 * PI * BEEP1_FREQ * t);

    beep1_buffer[i] = (int16_t)(s * 30000);
  }
}

void playBeep1()
{
  Serial.println("Beep1...");
  MemoryStream memStream((uint8_t *)beep1_buffer, TOTAL_SAMPLES_BEEP1);
  StreamCopy player(kit, memStream);
  bool playing = true;

  while (memStream.available())
  {
    player.copy();
  }
  Serial.println("Playbeep1 finished");
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // init AudioKit for input & output
  auto cfg = kit.defaultConfig(RXTX_MODE); // use both RX (mic) and TX (speaker)
  cfg.sample_rate = SAMPLE_RATE;
  cfg.channels = CHANNELS;
  cfg.bits_per_sample = BITS_PER_SAMPLE;
  cfg.sd_active = false;
  cfg.input_device = INPUT_LINE; 

  kit.begin(cfg);
  kit.setVolume(1.0);

  prepareBeep1(); // fill the PCM buffer with the beep sound
  playBeep1();    // play the beep to indicate that we are ready to listen

  multiout.add(recording);
  multiout.add(volumemeter);
  multiout.begin(cfg);
  Serial.println("Ready and listening...");
}

void loop()
{
  sleep(WAIT_BEFORE_REC);

  // 1) WAIT until we see loud sound


  // read raw mic into buffer
  soundDetected = false;
  Serial.println("Waiting for sound");

  while (!soundDetected)
  {
    kit.readBytes((uint8_t *)micbuf, MIC_CHUNK);
    if (soundAboveThreshold(micbuf, MIC_CHUNK))
    {
      Serial.println("Sound detected → start recording");
      soundDetected = true;
    }
  }

  // 2) RECORD into recording buffer
  recording.begin();
  volumemeter.begin(info);

  uint32_t startMs = millis();
  uint32_t recordedMs = 0;
  int silenceCount = 0;
  while (millis() - startMs < RECORD_MS && silenceCount < 3)
  {
    multirecorder.copy();
    if (volumemeter.volume() < SILENCE_THRESHOLD)
    {
      silenceCount++;
      Serial.println("Silence detected");
    }
    else
    {
      silenceCount = 0;
    }
  }
  // stop recording
  recorder.end();
  volumemeter.end();
  Serial.println("Recording done");

  recordedMs = millis() - startMs;
  Serial.println(recorder.bufferSize());

  // important pause
  sleep(WAIT_BEFORE_PLAY); 

  
  int recordingSize = recording.size();
  
  // copy from memory → kit
  recorder.begin(kit, recording); 
  Serial.println("Playing back...");

  int copied = 0;
  while (copied < recordingSize)
  {
    copied += recorder.copy();
  }

  recorder.end();
  playBeep1();

  Serial.println("Playback completed, back to listening");
}