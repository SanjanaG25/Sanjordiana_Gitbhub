//global variables

#define CHOICE_OFF      0 //Used to control LEDs

#define CHOICE_NONE     0 //Used to check buttons

#define CHOICE_RED  (1 << 0)

#define CHOICE_GREEN    (1 << 1)

#define CHOICE_BLUE (1 << 2)

#define CHOICE_YELLOW   (1 << 3)


#define LED_RED     9 

#define LED_GREEN   5 

#define LED_BLUE    3

#define LED_YELLOW  8


// Button pin definitions

#define BUTTON_RED    7

#define BUTTON_GREEN  4

#define BUTTON_BLUE   2

#define BUTTON_YELLOW 6


// Buzzer pin definitions

#define BUZZER1  11

#define BUZZER2  12


// Define game parameters

#define ROUNDS_TO_WIN      10 //Number of rounds to succesfully remember before you win.

#define ENTRY_TIME_LIMIT   5000 //Amount of time to press a button before game times out. 5000ms = 5 sec

#define MODE_MEMORY  0

// Game state variables

byte gameMode = MODE_MEMORY; //playing the memory game

byte gameBoard[32]; //combination of buttons as we advance

byte gameRound = 0; //succesful rounds the player has made it through


void setup()

{

  //Enable pull ups on inputs

  pinMode(BUTTON_RED, INPUT_PULLUP);

  pinMode(BUTTON_GREEN, INPUT_PULLUP);

  pinMode(BUTTON_BLUE, INPUT_PULLUP);

  pinMode(BUTTON_YELLOW, INPUT_PULLUP);


  pinMode(LED_RED, OUTPUT);

  pinMode(LED_GREEN, OUTPUT);

  pinMode(LED_BLUE, OUTPUT);

  pinMode(LED_YELLOW, OUTPUT);


  pinMode(BUZZER1, OUTPUT);

  pinMode(BUZZER2, OUTPUT);


  gameMode = MODE_MEMORY; //initiating the game

  play_winner(); // After setup is complete

}


void loop()

{

  attractMode(); // Blink lights while waiting for user to press a button


  // Indicate the start of game play

  setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on

  delay(1000);

  setLEDs(CHOICE_OFF); // Turn off LEDs

  delay(250);


  if (gameMode == MODE_MEMORY)

  {

    // Play memory game and result

    if (play_memory() == true) 

      play_winner(); // Player won, play winner tones

    else 

      play_loser(); // Player lost, play loser tones

  }
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//gameplay stuff

// Play the regular memory game

// Returns 0 if player loses, or 1 if player wins

boolean play_memory(void)

{

  randomSeed(millis()); // Seed the random generator


  gameRound = 0; // Reset the game to the beginning


  while (gameRound < ROUNDS_TO_WIN) 

  {

    add_to_moves(); // Add a button to the current moves, then play them back


    playMoves(); // Play back the current game board


    // Then require the player to repeat the sequence.

    for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++)

    {

      byte choice = wait_for_button(); // See what button the user presses


      if (choice == 0) return false; // If wait timed out, player loses


      if (choice != gameBoard[currentMove]) return false; // If the choice is incorect, player loses

    }


    delay(1000); // Player was correct, delay before playing moves

  }


  return true; // Player made it through all the rounds to win!

}


// Plays the current contents of the game moves

void playMoves(void)

{

  for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) 

  {

    toner(gameBoard[currentMove], 150);


    // Wait some amount of time between button playback

    // Shorten this to make game harder

    delay(150); // 150 works well. 75 gets fast.

  }

}


// Adds a new random button to the game sequence, by sampling the timer

void add_to_moves(void)

{

  byte newButton = random(0, 4); //min (included), max (exluded)


  // We have to convert this number, 0 to 3, to CHOICEs

  if(newButton == 0) newButton = CHOICE_RED;

  else if(newButton == 1) newButton = CHOICE_GREEN;

  else if(newButton == 2) newButton = CHOICE_BLUE;

  else if(newButton == 3) newButton = CHOICE_YELLOW;


  gameBoard[gameRound++] = newButton; // Add this new button to the game array

}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Lights a given LEDs

// Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc

void setLEDs(byte leds)

{

  if ((leds & CHOICE_RED) != 0)

    digitalWrite(LED_RED, HIGH);

  else

    digitalWrite(LED_RED, LOW);


  if ((leds & CHOICE_GREEN) != 0)

    digitalWrite(LED_GREEN, HIGH);

  else

    digitalWrite(LED_GREEN, LOW);


  if ((leds & CHOICE_BLUE) != 0)

    digitalWrite(LED_BLUE, HIGH);

  else

    digitalWrite(LED_BLUE, LOW);


  if ((leds & CHOICE_YELLOW) != 0)

    digitalWrite(LED_YELLOW, HIGH);

  else

    digitalWrite(LED_YELLOW, LOW);

}


// Wait for a button to be pressed. 

// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out

byte wait_for_button(void)

{

  long startTime = millis();


  while ( (millis() - startTime) < ENTRY_TIME_LIMIT) // Loop until too much time has passed

  {

    byte button = checkButton();


    if (button != CHOICE_NONE)

    { 

      toner(button, 150); // Play the button the user just pressed


      while(checkButton() != CHOICE_NONE) ;  // Now let's wait for user to release button


      delay(10); // Error checking accidental double taps


      return button;

    }


  }


  return CHOICE_NONE; // timed out!

}


// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.

byte checkButton(void)

{

  if (digitalRead(BUTTON_RED) == 0) return(CHOICE_RED); 

  else if (digitalRead(BUTTON_GREEN) == 0) return(CHOICE_GREEN); 

  else if (digitalRead(BUTTON_BLUE) == 0) return(CHOICE_BLUE); 

  else if (digitalRead(BUTTON_YELLOW) == 0) return(CHOICE_YELLOW);


  return(CHOICE_NONE); // If no button is pressed, return none

}


// Light an LED and play tone

// Red, upper left:     440Hz - 2.272ms - 1.136ms pulse

// Green, upper right:  880Hz - 1.136ms - 0.568ms pulse

// Blue, lower left:    587.33Hz - 1.702ms - 0.851ms pulse

// Yellow, lower right: 784Hz - 1.276ms - 0.638ms pulse

void toner(byte which, int buzz_length_ms)

{

  setLEDs(which); //Turn on a given LED


  //Play the sound associated with the given LED

  switch(which) 

  {

  case CHOICE_RED:

    buzz_sound(buzz_length_ms, 1136); 

    break;

  case CHOICE_GREEN:

    buzz_sound(buzz_length_ms, 568); 

    break;

  case CHOICE_BLUE:

    buzz_sound(buzz_length_ms, 851); 

    break;

  case CHOICE_YELLOW:

    buzz_sound(buzz_length_ms, 638); 

    break;

  }


  setLEDs(CHOICE_OFF); // Turn off all LEDs

}


// Toggle buzzer every buzz_delay_us, for a duration of buzz_length_ms.

void buzz_sound(int buzz_length_ms, int buzz_delay_us)

{

  // Convert total play time from milliseconds to microseconds

  long buzz_length_us = buzz_length_ms * (long)1000;


  // Loop until the remaining play time is less than a single buzz_delay_us

  while (buzz_length_us > (buzz_delay_us * 2))

  {

    buzz_length_us -= buzz_delay_us * 2; //Decrease the remaining play time


    // Toggle the buzzer at various speeds

    digitalWrite(BUZZER1, LOW);

    digitalWrite(BUZZER2, HIGH);

    delayMicroseconds(buzz_delay_us);


    digitalWrite(BUZZER1, HIGH);

    digitalWrite(BUZZER2, LOW);

    delayMicroseconds(buzz_delay_us);

  }

}


// Play the winner sound and lights

void play_winner(void)

{

  setLEDs(CHOICE_GREEN | CHOICE_BLUE);

  winner_sound();

  setLEDs(CHOICE_RED | CHOICE_YELLOW);

  winner_sound();

  setLEDs(CHOICE_GREEN | CHOICE_BLUE);

  winner_sound();

  setLEDs(CHOICE_RED | CHOICE_YELLOW);

  winner_sound();

}


// Play the winner sound

// This is just a unique (annoying) sound we came up with, there is no magic to it

void winner_sound(void)

{

  // Toggle the buzzer at various speeds

  for (byte x = 250 ; x > 70 ; x--)

  {

    for (byte y = 0 ; y < 3 ; y++)

    {

      digitalWrite(BUZZER2, HIGH);

      digitalWrite(BUZZER1, LOW);

      delayMicroseconds(x);


      digitalWrite(BUZZER2, LOW);

      digitalWrite(BUZZER1, HIGH);

      delayMicroseconds(x);

    }

  }

}


// Play the loser sound/lights

void play_loser(void)

{

  setLEDs(CHOICE_RED | CHOICE_GREEN);

  buzz_sound(255, 1500);


  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);

  buzz_sound(255, 1500);


  setLEDs(CHOICE_RED | CHOICE_GREEN);

  buzz_sound(255, 1500);


  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);

  buzz_sound(255, 1500);

}


// Show a display while waiting for user to press button

void attractMode(void)

{

  while(1) 

  {

    setLEDs(CHOICE_RED);

    delay(100);

    if (checkButton() != CHOICE_NONE) return;


    setLEDs(CHOICE_BLUE);

    delay(100);

    if (checkButton() != CHOICE_NONE) return;


    setLEDs(CHOICE_GREEN);

    delay(100);

    if (checkButton() != CHOICE_NONE) return;


    setLEDs(CHOICE_YELLOW);

    delay(100);

    if (checkButton() != CHOICE_NONE) return;

  }

}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
