// Variables
unsigned long startTime;
int i=0;
int j=0;
unsigned char lightLvl;
char side = 2; // What side is currently allowing traffic
char changeTo = 0; // If ready, what side to change to
int ticks = 0;
bool waitTen = false; // If the circuit should wait ten seconds, then change
bool gotIt = false;
bool gotIt2 = false;
int startTicks;
int startTicks2;
unsigned short carCount1 = 0;
unsigned short carCount2 = 0;
bool carSen1;
bool carSen2;
bool prevCarSen1 = false;
bool prevCarSen2 = false;

// Pin definitions
#define S1 A0
#define S2 A1
#define R1 3
#define R2 5
#define Y1 6
#define Y2 9
#define G1 10
#define G2 11
#define NS A2 //nighttime sensor
#define CC1 A6 // car counter 1
#define CC2 A5 // car counter 2

// Properties

unsigned char dimLvl = 100;
unsigned char brtLvl = 255;
int darkThres = 100; // ATTENTION: This will likely need to be adjusted to the conditions in the room; Try to put it in the middle of bright and dark readings
int nightThres = 80; // ATTENTION: This will likely need to be adjusted to the conditions in the room; Try to put it in the middle of bright and dark readings
unsigned short countThres = 450; //threshold for car counter resistor drop

//function declarations
void changeSide();
void resetLights();
bool carCheck();
void carCounter();

void setup() {
  // Setup pin modes
  pinMode(S1, INPUT); //stopline 1 sensor
  pinMode(S2, INPUT); //stopline 2 sensor
  pinMode(R1, OUTPUT); //red 1
  pinMode(Y1, OUTPUT); //yellow 1
  pinMode(G1, OUTPUT); //green 1
  pinMode(R2, OUTPUT); //red 2
  pinMode(Y2, OUTPUT); //yellow 2
  pinMode(G2, OUTPUT); //green 2
  pinMode(NS, INPUT); //Night sensor
  pinMode(CC1, INPUT);
  pinMode(CC2, INPUT);

  // Start Serial log
  Serial.begin(9600);

}

void loop() {
  startTime = millis(); // Get time in ms at start

  // Determine whether to dim lights
  if (analogRead(NS) < nightThres) lightLvl = dimLvl;
  else lightLvl = brtLvl;

  // Read from stop line sensors (sides indexed 0 and 1)
  int stop1 = analogRead(S1); // These range from ~620 in light to ~900 in the dark (BRIGHT ROOM CONDITIONS)
  int stop2 = analogRead(S2);

  carCounter(); // count any cars passing

  // If NOT currently changing side, update lights
  if (changeTo == 0)
  {
    switch (side) {
    case 1:
      analogWrite(G1, lightLvl);
      analogWrite(R2, lightLvl);
      break;
    case 2:
      analogWrite(G2, lightLvl);
      analogWrite(R1, lightLvl);
      break;
    }

  }

  // Check if 30 seconds have elapsed
  if (ticks >= 600)
  {
    switch (side) {
      case 1:
        if (stop2 > darkThres) break; // Check if car at stopline
        if (ticks == 600) changeTo = 2; // If car waiting, change immediately
        else if (waitTen == false) waitTen = true; // If car shows up later, run the "wait 10 seconds" logic
        break;
      case 2:
        if (stop1 > darkThres) break;
        if (ticks == 600) changeTo = 1;
        else if (waitTen == false) waitTen = true;
        break;
    }

    if (waitTen && changeTo == 0)
    {
        if (!gotIt) {startTicks = ticks; gotIt = true;} // On first iteration, record current ticks
        if (ticks < startTicks + 200) goto break1; // Exit this logic if <10 seconds have elapsed
        switch (side) { // If 10 seconds have elapsed, change the green side
          case 1:
            changeTo = 2; break;
          case 2:
            changeTo = 1; break;
        }
        gotIt = false;
    }
    break1:

    if (changeTo != 0) changeSide();
  }

  while(millis() < startTime + 50); // wait until 50ms have elapsed before reiterating (has the effect of creating a "fixed update" function)

  // Increment ticks
  ticks++;

  // Debug
  if (j==19) {
    Serial.print("Stop 1: "); Serial.print(stop1); Serial.print(", Stop 2: "); Serial.println(stop2); 
    Serial.print(carCount1); Serial.println(carCount2);
    j=0;}
  else j++; 
}

void changeSide() { // Side changer logic
  switch (changeTo) {
      case 1:
        // Transition logic to side 1
        if (!gotIt) 
        {
          startTicks = ticks; 
          gotIt = true;
          analogWrite(Y2, lightLvl);
          analogWrite(G2, 0); 
        }
        waitTen = false;

        if (ticks >= startTicks + 60)
        {
          analogWrite(R2, lightLvl);
          analogWrite(Y2, 0);

          if (carCheck()) resetSide(); //TO DO: car counter logic, check if out = in
        }
        break;
      case 2:
        // Transition logic to side 2
        if (!gotIt) 
        {
          startTicks = ticks; 
          gotIt = true;
          analogWrite(Y1, lightLvl);
          analogWrite(G1, 0); 
        }
        waitTen = false;

        if (ticks >= startTicks + 60)
        {
          analogWrite(R1, lightLvl);
          analogWrite(Y1, 0);

          if (carCheck()) resetSide(); //TO DO: car counter logic, check if out = in
        }
        break;
      default:
        break;
    }
}

void resetSide() { // Runs logic for resetting the timers and changing the green side
  switch (changeTo) {
    case 1:
      if (!gotIt2) {startTicks2 = ticks; gotIt2 = true;}
      if (ticks < startTicks2 + 60) break;
      analogWrite(G1, lightLvl);
      analogWrite(R1, 0);
      ticks = 0;
      side = changeTo;
      changeTo = 0;
      gotIt = false;
      gotIt2 = false;
      carCount1 = 0;
      carCount2 = 0;
      break;
    case 2:
      if (!gotIt2) {startTicks2 = ticks; gotIt2 = true;}
      if (ticks < startTicks2 + 60) break;
      analogWrite(G2, lightLvl);
      analogWrite(R2, 0);
      ticks = 0;
      side = changeTo;
      changeTo = 0;
      gotIt2 = false;
      gotIt = false;
      carCount1 = 0;
      carCount2 = 0;
      break;
  }
}

bool carCheck() { // Checks if there are any cars currently crossing the bridge, True if bridge is clear
  // Finish later
  if (carCount1 == carCount2) return true;
  else return false;
}

void carCounter() {
  carSen1 = (bool) (analogRead(CC1) < countThres);
  carSen2 = (bool) (analogRead(CC2) < countThres);
  if (carSen1 && !prevCarSen1) carCount1++;
  if (carSen2 && !prevCarSen2) carCount2++;
  prevCarSen1 = carSen1;
  prevCarSen2 = carSen2;
}
