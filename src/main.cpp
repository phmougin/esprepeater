// Simplex (parrot) radio repeater
// Philippe MOUGIN
// V1.0 2026/02/27
// Dependency: AudioTools library by Phil Schatzmann  https://github.com/pschatzmann/arduino-audio-tools

#include <Arduino.h>

// audio tools library import https://github.com/pschatzmann/arduino-audio-tools
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/AudioLibs/MemoryManager.h"
#include "AudioTools/CoreAudio/BaseStream.h"
//#include "Morse.h"



const uint32_t SAMPLE_RATE = 8000;      // sampling rate 
const uint8_t CHANNELS    = 1;           // mic is mono
const uint16_t THRESHOLD  = 80;        // threshold to start recording const int BITS_PER_SAMPLE = 8;
const uint32_t RECORD_MS  = 5000;        // record ~5 sec
const size_t BUFFER_SIZE  = (SAMPLE_RATE * CHANNELS * sizeof(int16_t) * RECORD_MS) / 1000;

AudioBoardStream kit(AudioKitAC101);     // kit driver for AudioKit with AC101 codec : change to appropriate board if you use another one (see

DynamicMemoryStream recording(true);      // buffer to record into RAM
StreamCopy recorder;                      // copies from mic → memory or memory → codec


/////////////////////// beep
const int DURATION_BEEP = 1;
const float TONE_FREQ = 440.0;
const int TOTAL_SAMPLES_BEEP = SAMPLE_RATE * DURATION_BEEP;
// PCM buffer in memory
int16_t pcm_buffer[TOTAL_SAMPLES_BEEP];
// Stream objects
MemoryStream memStream;
// Output stream to AudioKit
/////////////

const AudioInfo info(8000, 2, 16);

//TalkiePCM voice(kit, CHANNELS);

bool soundDetected=false;


static bool soundAboveThreshold(int16_t *buf, size_t len) {
  long sum = 0;
  for (size_t i = 0; i < len; i++) {
    int v = buf[i];
    sum += (v < 0) ? -v : v;
  }
  long avg = sum / len;
  //Serial.println(avg);
  return (avg > THRESHOLD);
}



void generateBeep()
{
    for (int i = 0; i < TOTAL_SAMPLES_BEEP; i++) {
        float t = (float)i / SAMPLE_RATE;
        float s = sin(2.0 * PI * TONE_FREQ * t);

        pcm_buffer[i] = (int16_t)(s * 30000);
    }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // init AudioKit for input & output
  auto cfg = kit.defaultConfig(RXTX_MODE);   // use both RX (mic) and TX (speaker)
  cfg.sample_rate = SAMPLE_RATE;
  cfg.channels    = CHANNELS;
  cfg.bits_per_sample = 16;
  cfg.sd_active = false;
  cfg.input_device = ADC_INPUT_LINE2; //LINE1: mic  LINE2: input jack
  


  generateBeep();

 
  kit.begin(cfg);
  kit.setVolume(1.0);
  


  Serial.println("Ready and listening...");
}
void playBeep() {
 
  Serial.println("Beep...");
 
 
  MemoryStream memStream((uint8_t*)pcm_buffer, TOTAL_SAMPLES_BEEP);
  StreamCopy player(kit, memStream);
  bool playing=true;
  while(memStream.available()){
    player.copy();

  }
  Serial.println("Playbeep finished");
    
}

void loop() {
  //playBeep();
  sleep(1);
 

  // 1) WAIT until we see loud sound

  // buffer to hold a chunk of mic data
  static const uint32_t CHUNK = 256;
  static int16_t buf[CHUNK];

  // read raw mic into buffer
  //size_t got = kit.read((uint8_t*)buf, CHUNK * sizeof(int16_t));
  size_t got;

  soundDetected=false;

  Serial.println("Waiting for sound");
 
  while(!soundDetected) {
    got=kit.readBytes((uint8_t*)buf,CHUNK);
    //if (got >= CHUNK * sizeof(int16_t)) {
        if (soundAboveThreshold(buf, CHUNK)) {
          Serial.println("Sound detected → start recording");
          soundDetected=true;
        }
    //}
  }
 


  // 2) RECORD into recording buffer
  recording.begin();             // clear
  recorder.begin(recording, kit); // copy from mic → memory

  unsigned long startMs = millis();
  while (millis() - startMs < RECORD_MS) {
    if (!recorder.copy()) {
      // continue copying
    }
  }
  recorder.end();               // stop recording
  Serial.println("Recording done");
  
  //sleep(1);
  //voice.say(spa_TONE1);
  //playBeep();
  sleep(1);  /// important pause !  
  //unsigned long recms=
  // 3) PLAYBACK the recording
  recorder.begin(kit, recording); // copy from memory → kit
  Serial.println("Playing back...");
  
  startMs = millis();
  while (millis() - startMs < RECORD_MS) {
    recorder.copy();
    // let the data play!
    
  }
  
  recorder.end();
  playBeep();
  sleep(1);
  Serial.println("Playback completed, back to listening");
}




