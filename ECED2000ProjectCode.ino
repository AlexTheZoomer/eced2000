int startTime;
int i=0;
int j=0;

#define S1 A0
#define S2 A1
#define R1 A2
#define R2 A3
#define Y1 A4
#define Y2 A5
#define G1 A6
#define G2 A7

void setup() {
  // put your setup code here, to run once:

  // Setup pin modes
  pinMode(S1, INPUT); //stopline 1
  pinMode(S2, INPUT); //stopline 2
  pinMode(R1, OUTPUT); //red 1
  pinMode(Y1, OUTPUT); //yellow 1
  pinMode(G1, OUTPUT); //green 1
  pinMode(R2, OUTPUT); //red 2
  pinMode(Y2, OUTPUT); //yellow 2
  pinMode(G2, OUTPUT); //green 2

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  startTime = millis(); // Get time in ms at start

  // Read from stop line sensors (sides indexed 0 and 1)
  int stop1 = analogRead(S1); // These range from ~620 in light to ~900 in the dark
  int stop2 = analogRead(S2);

  if (j==19) {
    Serial.print("Stop 1: "); Serial.print(stop1); Serial.print(", Stop 2: "); Serial.println(stop2); // Debug printer
    j=0;}
  else j++;  

  while(millis()<startTime + 50); // wait until 50ms have elapsed before reiterating (has the effect of creating a "fixed update" function)
}
