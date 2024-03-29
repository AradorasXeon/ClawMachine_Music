// https://projecthub.arduino.cc/tmekinyan/playing-popular-songs-with-arduino-and-a-buzzer-546f4a

#define DEBUG
#include "pitches.h"
#include "musics.h"
#include "melodies.h"
#include "/home/krisztian/arduino/clawnanov2/communicationMusic.h"
#include <Wire.h>


#define BUZZER_PIN 3


Music* Music::instance = nullptr; //have to be 'assigned'
Music msgMusic(false);
PlayableMusic pirates(BUZZER_PIN, PIRATES_melody, PIRATES_durations, sizeof(PIRATES_melody)/sizeof(PIRATES_melody[0]), 1000, 1.5);
PlayableMusic gameOfThrones(BUZZER_PIN, GAME_OF_THRONES_melody, GAME_OF_THRONES_durations, sizeof(GAME_OF_THRONES_melody)/sizeof(GAME_OF_THRONES_melody[0]), 1000, 1.5);
PlayableMusic pinkPanther(BUZZER_PIN, PINK_PANTHER_melody, PINK_PANTHER_durations, sizeof(PINK_PANTHER_melody)/sizeof(PINK_PANTHER_melody[0]), 1000, 1.5);
PlayableMusic homeAlone(BUZZER_PIN, HOME_ALONE_melody, HOME_ALONE_durations, sizeof(HOME_ALONE_melody)/sizeof(HOME_ALONE_melody[0]), 1000, 1.5);

CurrentMusic currentlyPlayedMusic;

struct MusicPair
{
  PlayableMusic musicClass;
  CurrentMusic pairedMusicType;
};

MusicPair musicPairs[4] =
{
  {gameOfThrones,   CurrentMusic::CALIBRATION},
  {pirates,         CurrentMusic::GAME_PLAY},
  {homeAlone,       CurrentMusic::CLAW_ACTION},
  {pinkPanther,     CurrentMusic::PRIZE_DROP}
};

void setup()
{
  msgMusic.instance = &msgMusic;
  Wire.onReceive(inComingMsg);

  msgMusic.setGamePlayMusic();
  //Debug:
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("SETUP RAN.");
  #endif // DEBUG
}

static void inComingMsg(int byteCount)
{
  #ifdef DEBUG
  Serial.println(byteCount);
  #endif // DEBUG
  Music::onReceiveCallBack(byteCount);                    //reading i2c msg
  if (currentlyPlayedMusic != msgMusic.getMusicToPlay())  //check if the currently played music is different from the requested one
  {
    //if different we have to stop the currentlyPlayedMusic
    stopActualMusic(currentlyPlayedMusic);
  }
}

void stopActualMusic(CurrentMusic curPlayed)
{
  if(curPlayed == CurrentMusic::NOT_SET) return;

  int numMusicPairs = sizeof(musicPairs) / sizeof(musicPairs[0]);
  for(int i = 0; i < numMusicPairs; i++)
  {
    if(curPlayed == musicPairs[i].pairedMusicType)
    {
      musicPairs[i].musicClass.stopMusic();
      /*
      while(!musicPairs[i].musicClass.isStopped())
      {
        //just not letting it to end while it is note stopped for sure
      }
      */
    }
  }

  /*
  #ifdef DEBUG
  Serial.println("Cannot stop actual music, because curPlayed is not in the list.");
  #endif // DEBUG
  */
}

void startWantedMusic(CurrentMusic wantedMusic)
{
  int numMusicPairs = sizeof(musicPairs) / sizeof(musicPairs[0]);
  for(int i = 0; i < numMusicPairs; i++)
  {
    if(wantedMusic == musicPairs[i].pairedMusicType)
    {
      currentlyPlayedMusic = wantedMusic;
      musicPairs[i].musicClass.startMusic();
    }
  }

  #ifdef DEBUG
  Serial.println("Cannot start wanted music, because wantedMusic is not in the list.");
  #endif // DEBUG
}

void loop()
{
  if(msgMusic.getMusicToPlay() == CurrentMusic::NOT_SET)
  {
    #ifdef DEBUG
    Serial.println("MUSIC IS NOT SET!");
    #endif // DEBUG
  }
  else
  {
    #ifdef DEBUG
    Serial.println("MUSIC IS SET!");
    #endif // DEBUG
    startWantedMusic(msgMusic.getMusicToPlay());
  }
}