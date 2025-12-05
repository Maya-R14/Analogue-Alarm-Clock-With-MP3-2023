#include <SoftwareSerial.h>

#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

int Play;     

int16_t Track;
int16_t Max_Track;

int volumeflag;
int volume;

int CardStatus;

int Shuffle;
int16_t ShuffleTrack;
int16_t ShuffledSongs[500];

int AlarmStat;

//Fix play status when previous/next

void setup() {
  // put your setup code here, to run once:
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  pinMode(A2, INPUT);       // ADC Volume    
  
  pinMode(4, INPUT_PULLUP); // Shuffle Switch
  
  pinMode(6, INPUT_PULLUP); // set arduino pin to input pull-up mode Previous track
  pinMode(7, INPUT_PULLUP); // set arduino pin to input pull-up mode Pause/Play
  pinMode(8, INPUT_PULLUP); // set arduino pin to input pull-up mode Next Track
   
  pinMode(2, INPUT);        // Interrupt Pin for Alarm
  attachInterrupt(digitalPinToInterrupt(2), Alarm, FALLING);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  int volume=analogRead(A2);
  volume = volume/42.9;
  myDFPlayer.volume(volume);  
  int volumeflag=volume;
  
  Max_Track=myDFPlayer.readFileCounts();
  
  Track = 1;
  myDFPlayer.playMp3Folder(Track);
  delay(200);
  myDFPlayer.pause();
  
  Play=0;
  Shuffle= 0;
  CardStatus=1;
  AlarmStat=0;
}

void loop() {

  
  if (myDFPlayer.readType() == 2 && CardStatus != 1){ //if card is inserted. Reinitialize the MP3 player and checks the total number of songs
    delay(200);
    Max_Track=myDFPlayer.readFileCounts();
    
    Track = 1;
    myDFPlayer.playMp3Folder(Track); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)();  //Play the first mp3
    delay(200);
    myDFPlayer.pause();
    
    Play=0;
    Shuffle= 0;
    CardStatus=1;
    Serial.println(Max_Track);
  }
  
  if (myDFPlayer.readType() == 3 && CardStatus != 0){ //if card is ejected CardStatus goes low
    CardStatus=0;
  }

  PausePlayVolume(); //Handles the Pause Play and Volume Function

  ShuffleCheck(); //Shuffles tracks

  PreviousNext(); //Auto Plays next song

  AutoPlay(); //Plays next song when current one finishes

  if(AlarmStat==1){
    if (Shuffle == 0){ 
      Track = 1;
      myDFPlayer.playMp3Folder(Track);
      Play = 1;
      Serial.println("Alarm (linnear)"); 
    }
  
    if (Shuffle == 1){
      Track = random(1, Max_Track); 
      myDFPlayer.playMp3Folder(Track);
      Play = 1;
      Shuffle = 0;
      Serial.println("Alarm (Shuffled)"); 
    }
    AlarmStat=0;
  }

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
}

void PausePlayVolume(void){
  //Pause Play//
  if(digitalRead(7) == LOW){
    Play=!Play;
    Serial.println("The button is pressed");
    
    if (Play==1){
      myDFPlayer.start();  //start the mp3 from the pause
      Serial.println("Music is playing");
    }
    if (Play==0){
      myDFPlayer.pause();  //pause the mp3
      Serial.println("Music is Paused");
    }
  
    delay(500);
  }
  //Volume
  volume=analogRead(A2); //Speaker can't handle a volume above 25. It starts crackling and the voltage drawn is high enough that it affects the voltage reference
  
  volume = volume/42.9; //Volume range is from 0 to 23.
  if(volumeflag != volume){
    Serial.println(volume);
    myDFPlayer.volume(volume);  
    volumeflag=volume;
  }
}

void ShuffleCheck (void){
  //Shuffle
  if(digitalRead(4)== HIGH && Shuffle != 1){ //If shuffle switch is HIGH and the songs haven't been shuffled before. 
    int16_t sum = 0;
    ShuffledSongs[0] = Track; //store in the 0 position of the array the current song being played. 
    for (int16_t i = 1; i < Max_Track ; i++) { //Generate an array with the rest of the number of song (eg. if Track 2 current being played, array = 2,1,3,4,5,...)
      sum++;
      if (sum == Track){
        sum++;
        ShuffledSongs[i] = sum;
      }
      else{
        ShuffledSongs[i] = sum;
      }
    }
    
    for (int16_t i=1; i < Max_Track; i++) { //Take previous array and shuffled by switching positions of the array at random N number of times)(N=max amount of songs in the array)
      int16_t n = random(1, Max_Track); //range between 1 and max songs because we don't want to affect the position array position 0. This is the song currently being played. 
      int16_t temp = ShuffledSongs[n];
      ShuffledSongs[n] =  ShuffledSongs[i];
      ShuffledSongs[i] = temp;
    }  
  Shuffle = 1; //Shuffle status is set to HIGH
  ShuffleTrack = 0; //Sets the current songs as the first song in the array. 
  Serial.println("Music Shuffled!");
  }
  //Unshuffle
  if(digitalRead(4)== LOW && Shuffle != 0){ //If the shuffle switch is LOW and the songs HAVE been shuffled.
    Track=ShuffledSongs[ShuffleTrack]; //Simply store the song currently being played as the Current Track
    Shuffle = 0; //Set shuffle status to LOW
    Serial.println("Linnear Play");
  }
}

void PreviousNext(void){
  //Previous track
    if(digitalRead(6) == LOW){ //if Previous button pressed
    if(Shuffle == 0){ //If unshuffled 
      if(Track == 1){Track = Max_Track;} //Check if we're at the first song and if so roll over. 
      else Track--; //If not play previous song
      myDFPlayer.playMp3Folder(Track); 
      Serial.println("Previous Track (Linear)");
      delay(500);   
    }
    if(Shuffle == 1){ //If shuffled
      if(ShuffleTrack == 0){ShuffleTrack = Max_Track-1;} //Check if we're at the first shuffled track and if so roll over. 
      else ShuffleTrack--; //if not play previous shuffled track
      myDFPlayer.playMp3Folder(ShuffledSongs[ShuffleTrack]);
      Serial.println("Previous Track (Shuffle)");
      delay(500); 
    }
    Play=1;
  }
  //Next track
  if(digitalRead(8) == LOW){ //same as Previous function but for the forwards instead of backwards. 
    if (Shuffle == 0){ 
      if(Track == Max_Track){Track=1;}
      else Track++;
      myDFPlayer.playMp3Folder(Track); 
      Serial.println("Next Track (Linnear)");
      delay(500);      
    }
    if (Shuffle == 1){ //Next track (Shuffled)
      if(ShuffleTrack == Max_Track-1){ShuffleTrack=0;}
      else ShuffleTrack++;
      myDFPlayer.playMp3Folder(ShuffledSongs[ShuffleTrack]);
      Serial.println("Next Track (Shuffle)");
      delay(500);  
    }
    Play=1;
  }
}

void AutoPlay(void){ //A copy of the Next Song function but with a different trigger. 
  if (myDFPlayer.readType() == 5){ 
    if (Shuffle == 0){ //Next track (Unshuffled)
      if(Track == Max_Track){Track=1;}
      else Track++;
      myDFPlayer.playMp3Folder(Track);
      Serial.println("Next Track (Linnear)");
    }
    if (Shuffle == 1){ //Next track (Shuffled)
      if(ShuffleTrack == Max_Track-1){ShuffleTrack=0;}
      else ShuffleTrack++;
      myDFPlayer.playMp3Folder(ShuffledSongs[ShuffleTrack]);
      Serial.println("Next Track (Shuffle)"); 
    }
  }
}

void Alarm(void){ //If alarm goes off, set alarm status to high 
  Serial.println("Alarm! Interrupt!"); 
  AlarmStat=1; 
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
