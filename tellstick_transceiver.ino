/*
 * Telldus transceiver
 * The setup is currently a 433 MHz transmitter and reveicer kit
 * from luxorparts and the code is running on an
 * Arduino Uno Rev3
 * 
 * Receiver functionality is based on original code from
 * Barnaby Gray 12/2008
 * Peter Mead 09/2008
 * "Homeeasy protocol receiver for the new protocol."
 */

// Receiver config =================================
int rxPin = 12; // Input of 433 MHz receiver
int ledPin = 13; // Onboard LED

int t1 = 0; // Latch 1 time only needed for debugging purposes
int t2 = 0; //latch 2 time only needed for debugging purposes.

// Here is the unique Transmitter code. Use the Serial monitor to identify your Transmitter code.

void setup() { 
  pinMode(rxPin, INPUT); // Input of 433 MHz receiver
  
  Serial.begin(9600);
  Serial.println ("Telldus Transceiver");
}


void loop() {
  receiverLoop();
}

void receiverLoop(void) {
  int i = 0;
  unsigned long t = 0;
  
  byte prevBit = 0;
  byte bit = 0;
  
  unsigned long sender = 0;
  bool group = false;
  bool onOff = false;
  unsigned int recipient = 0;
  
  // latch 1
  // Latch timing has been loosened to acommodate varieties in the Nexa transmitters.
  
  while ((t < 8000 || t > 13000)) { 
    t = pulseIn(rxPin, LOW, 1000000);
    t1 = t; // Save latch timing for debugging purposes
  }
  
  // Next line can be used to debug latch timing. Please note that this affects timing and that recieving the following data bits may fail.
  //Serial.println (t);
  
  // latch 2
  // Latch timing has been loosened to acommodate varieties in the Nexa transmitters.
  while (t < 2200 || t > 2900) { 
    t = pulseIn(rxPin, LOW, 1000000);
  }
  
  t2 = t; // Save latch timing for debugging purposes
  // Next line can be used to debug latch timing. Please note that this affects timing and that recieving the following data bits may fail.
  //Serial.println (t);
  
  
  
  // data collection from reciever circuit
  while (i < 64) {
    t = pulseIn(rxPin, LOW, 1000000);
  
    if (t > 200 && t < 400) { 
      bit = 0;
    }
    else if (t > 1100 && t < 1560) { 
      bit = 1;
    }
    else { 
      i = 0;
      break;
    }
  
    if (i % 2 == 1) {
      if ((prevBit ^ bit) == 0) { // must be either 01 or 10, cannot be 00 or 11
        i = 0;
        break;
      }
      if (i < 53) { // first 26 data bits
        sender <<= 1;
        sender |= prevBit;
      }
      else if (i == 53) { // 26th data bit
        // Group command not used by Tellstick Net v2
        group = prevBit;
      }
      else if (i == 55) { // 27th data bit
        onOff = prevBit;
      }
      else { // last 4 data bits
        recipient <<= 1;
        recipient |= prevBit;
      }
    }
  
  prevBit = bit;
  ++i;
  }
  
  // interpret message
  if (i > 0) { 
    // Print the result on Serial Monitor. Use this to identify your transmitter code.
    printResult(sender, group, onOff, recipient);
    // This is the check for the correct transimtter code. If code is incorrect then go back to look for new code.
    if (Sendercode == sender) {
      switch (onOff) {
        case 0:
          digitalWrite(ledPin, false);
          Serial.println("OFF!");
          break;
        case 1:
          digitalWrite(ledPin, true);
          Serial.println("ON!");
          break;
          
        break;
      }
    }
  }
}

void printResult(unsigned long sender, bool group, bool on, unsigned int recipient) {
  Serial.print("sender ");
  Serial.println(sender);
  if (group) { 
    Serial.println("group command");
  }
  else { 
    Serial.println("no group");
  }

  if (on) { 
    Serial.println("on");
  }
  else { 
    Serial.println("off");
  }

  Serial.print("recipient ");
  Serial.println(recipient);
  Serial.println(t1); // Timing for latch 1
  Serial.println(t2); // Timing for latch 2

  Serial.println();
}
