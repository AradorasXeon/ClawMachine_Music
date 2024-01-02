/// @brief Class to contain music, can be used to start or stop given music. The class is designed with the requiments of the project, so the only thing this board will do is play music based on i2c messages

#include "/home/krisztian/arduino/ClawMachine_Nano/millisTimer.hpp"

class PlayableMusic
{
    public:
    PlayableMusic(int musicPin, const uint16_t* PROGMEM noteList, const uint8_t* PROGMEM noteDurationList, int lengthOfMusic, int tempo = 1000, float breakTime = 1.3f);

    void startMusic();
    void stopMusic();
    bool isStopped();

    private:
    const uint16_t* PROGMEM _noteList;
    const uint8_t* PROGMEM _noteDurationList;
    uint16_t _note;
    uint8_t _noteDuration;
    int _tempo;
    int _lengthOfList;
    float _breakTime;
    int _musicPin;
    bool _continuePlaying = false; //Program needs to be interrupted so the music can be stopped, luckily i2c incoming message makes a kind of interrupt.
    bool _isExited = false;
};

/// @brief This take in two array, one array for the list of music notes, and one are for the duration for each note
/// @param musicPin the pin where the tone function will controll the frequency
/// @param noteList pointer the melody / to the list of notes to be played
/// @param noteDurationList pointer to the array, where for each note there is a duration
/// @param tempo if bigger the music will be slower, if lower the music will be faster default is 1000
/// @param breakTime a little delay after each note, note duration will be multipled by this number and no sound will be played for this amount of ms
PlayableMusic::PlayableMusic(int musicPin, const uint16_t* PROGMEM noteList, const uint8_t* PROGMEM noteDurationList, int lengthOfMusic, int tempo = 1000, float breakTime = 1.3f)
{
    if(noteList == nullptr || noteDurationList == nullptr)
    {
        #ifdef DEBUG
            Serial.println("Empty PlayableMusic was created, adding zeros to it");
        #endif // DEBUG
        /*
        uint16_t zero[] = {0};
        uint8_t length[] = {4};
        _noteList = zero;
        _noteDurationList = length;
        _lengthOfList = 1;
        */
       //throw error;
       //assert(5 == 10);
    }
    else
    {
        _noteList = noteList;
        _noteDurationList = noteDurationList;
        _lengthOfList = lengthOfMusic;
    }
    _tempo = tempo;
    _breakTime = breakTime;
    _musicPin = musicPin;
    _isExited = false;

    pinMode(_musicPin, OUTPUT);
}

/// @brief start playing the music
void PlayableMusic::startMusic()
{
    #ifdef DEBUG
        Serial.println("PlayableMusic::startMusic() called");
    #endif // DEBUG
    _continuePlaying = true;
    _isExited = false;
    while(1)
    {
        for (int note = 0; note < _lengthOfList; note++) 
        {
            if(_continuePlaying)
            {
                //to calculate the note duration, take one second divided by the note type.
                //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
                _noteDuration = pgm_read_byte(_noteDurationList + note);
                int duration = _tempo / _noteDuration;
                _note = pgm_read_word(_noteList + note);
                #ifdef DEBUG
                    //Serial.println(duration);
                    Serial.print("Duration: ");
                    Serial.println(_noteDuration);
                    Serial.print("note: ");
                    Serial.println(_note);
                #endif // DEBUG
                tone(_musicPin, _note, duration);
                //to distinguish the notes, set a minimum time between them.
                int pauseBetweenNotes = duration * _breakTime;
                #ifdef DEBUG
                    //Serial.println(pauseBetweenNotes);
                    Serial.println("--------------------------------------");

                #endif // DEBUG
                MillisTimer::delayThisMuch(pauseBetweenNotes);
                //stop the tone playing:
                noTone(_musicPin);
            }
            else
            {
                goto EXIT_FROM_LOOP;
            }
        }
        #ifdef DEBUG
            Serial.println("Played song once.");
        #endif // DEBUG
    }
    EXIT_FROM_LOOP:
    #ifdef DEBUG
        Serial.println("Exiting from playing music loop.");
    #endif // DEBUG
    _isExited = true;
}

/// @brief stop playing the music
void PlayableMusic::stopMusic()
{
    _continuePlaying = false;
}

/// @brief should be used to check if the while loop was exited properly, may not be necesary at all
bool PlayableMusic::isStopped()
{
    return _isExited;
}