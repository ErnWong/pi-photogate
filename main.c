#include <stdio.h>
#include <stdbool.h>
#include <wiringPi.h>

// Threshold values:
//  - Direct: ?
//  - Direct + Schmitt Trigger: ?
//  - Pulldown: 5000
//  - Pulldown + Schmitt Trigger: 0 
#define FLICKER_THRESHOLD 0

#define DATALOG 1
#define FLICKERLOG 0

#define datalog(...) \
  do { \
    if (DATALOG) \
      printf(__VA_ARGS__); \
  } while (0);

#define flickerlog(...) \
  do { \
    if (FLICKERLOG) \
      printf(__VA_ARGS__); \
  } while (0);

int main()
{
  wiringPiSetup();
  pinMode(0, INPUT);
  unsigned int prevMicros = micros();
  bool prev = false;
  unsigned long long int frames = 0;

  unsigned int prevChangeMicros = 0;
  bool debouncedValue = false;

  printf("Just to let you know that this program is actually running ^_^\n");

  while (1)
  {
    unsigned int currentMicros = micros();
    bool current = digitalRead(0);
    if (!prev && current)
    {
      unsigned int duration = currentMicros - prevChangeMicros;
      flickerlog("LOW  for %d milliseconds.\n", duration);
      if (duration > FLICKER_THRESHOLD && !debouncedValue)
      {
        debouncedValue = true;

        double timeDifference = currentMicros - prevMicros;
        timeDifference *= 1E-6;
        double frequency = 1 / timeDifference;
        double timePerFrame = timeDifference / frames;
        double maxFrequency = 1 / (timeDifference - timePerFrame);
        double frequencyError = maxFrequency - frequency;
        prevMicros = currentMicros;
        frames = 0;
        datalog("Frequency: %f +/- %f\n", frequency, frequencyError);
      }
      prevChangeMicros = currentMicros;
    }
    else if (prev && !current)
    {
      unsigned int duration = currentMicros - prevChangeMicros;
      flickerlog("HIGH for %d milliseconds.\n", duration);
      if (duration > FLICKER_THRESHOLD && debouncedValue)
      {
        debouncedValue = false;
      }
      prevChangeMicros = currentMicros;
    }
    prev = current;
    frames++;
  }
}
