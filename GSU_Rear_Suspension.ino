// Setup Time Based Variables
unsigned long currentTime = 0 ;
int rescanTime = 1000;

// Setup Transducer Variables
int transducerPin = A4;
int transducerReading = 0;
int lastCompressorSwitch = 0;
int history[5] = {0, 0, 0, 0, 0};
int historyTotal = 0;
int historyAvg = 0;
int currentPressure = 0;
int minPressure = 218;
int maxPressure = 250;

// Setup Compressor Variable
int compressorRelayPin = 3;
int compressorState = 0;
bool compressorWorking = true;

void setup() {
  Serial.begin(9600);
  pinMode(compressorRelayPin, OUTPUT);
}

void loop() {
  if (millis() > currentTime + rescanTime && compressorWorking) {
    currentTime = millis();
    currentPressure = analogRead(transducerPin);
    Serial.println("Reading is = ");
    Serial.println(currentPressure);
    
    // Stop loop if transducer is faulty.
    if (currentPressure <= 50) {
      return;
    }
    
    updateHistory(currentPressure);
    
    // Pause loop untill we have 3 pressure readings stored.
    if (history[2] == 0) {
      return;
    }
    
    // Check If Compressor Running & Above Target Pressure
    if (compressorState == 1 && currentPressure > maxPressure && history[1] >= maxPressure && history[2] >= maxPressure) {
      Serial.println("Compressor Running, Target Pressure Reached, Switching Off.");
      switchCompressor();
      return;
    }
    
    // Fault Condition if compressor isnt working switch off and stop the loop
    if (compressorState == 1 && lastCompressorSwitch >= currentPressure && history[4] != 0 && currentPressure <= 200) {
      Serial.println("Compressor/Transducer Fault Ending Loop");
      switchCompressor();
      compressorWorking = false;
      return;
    }
    
    // Underinflated turn on compressor
    if (compressorState == 0 && currentPressure <= minPressure && history[1] <= minPressure) {
      Serial.println("Switching compressor on to inflate");
      switchCompressor();
      return;
    }
    Serial.println("No Action Needed");
  }
}

void updateHistory(int newReading) {
  history[4] = history[3];
  history[3] = history[2];
  history[2] = history[1];
  history[1] = history[0];
  history[0] = newReading;
  historyTotal = history[0] + history[1] + history[2] + history[3] + history[4];
  historyAvg = historyTotal / 5; 
}

void switchCompressor() {
  switch(compressorState) {
    case 0:
      compressorState = 1;
      lastCompressorSwitch = analogRead(transducerPin);
      digitalWrite(compressorRelayPin, HIGH);
      break;
    case 1:
      compressorState = 0;
      lastCompressorSwitch = 0;
      digitalWrite(compressorRelayPin, LOW);
      break;
  }
}
