#include <stdio.h>
#include <stdbool.h>
#include <wiringPi.h>

int main()
{
  wiringPiSetup();
  pinMode(0, INPUT);
  unsigned int prevMicros = micros();
  bool prev = false;
  unsigned long long int frames = 0;

  bool hasChanged = false;
  double runningFrequency = 0;
  double runningError = 0;
  unsigned int nextUpdateMicros = 0;
  int flickers = 0;
  while (1)
  {
    unsigned int currentMicros = micros();
    bool current = digitalRead(0);
    if (!prev && current)
    {
      double timeDifference = currentMicros - prevMicros;
      timeDifference *= 1E-6;
      double frequency = 1 / timeDifference;
      double timePerFrame = timeDifference / frames;
      double maxFrequency = 1 / (timeDifference - timePerFrame);
      //printf("\r                                                                                 \r");
      //printf("1 || %f +/- %f Hz\n", frequency, maxFrequency - frequency);
      runningFrequency = frequency;
      runningError = maxFrequency - frequency;
      prevMicros = currentMicros;
      frames = 0;
      hasChanged = true;
      flickers++;
    }
    //printf("%d\n", current);
    if (currentMicros > nextUpdateMicros)
    {
      //if (!hasChanged) puts("--");
      //else printf("%f +/- %f Hz\n", runningFrequency, runningError);
      printf("%d\n", flickers);
      nextUpdateMicros = currentMicros + 5E5;
      hasChanged = false;
      flickers = 0;
    }
    prev = current;
    frames++;
  }
}
