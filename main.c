#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wiringPi.h>

// Microseconds to wait before outputting data
#define OUTPUT_PERIOD 1E5

// What information do you want to log?
//  - 1 = enable
//  - 0 = disable
#define DATALOG 1
#define FLICKERLOG 0
#define PINLOG 0

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

#define pinlog(...) \
  do { \
    if (PINLOG) \
      printf(__VA_ARGS__); \
  } while (0);

typedef struct
Photogate
{
  int pin;
  int raw;
  int value;
  int flickerThreshold;
  unsigned int prevEdgeMicros;
  unsigned int prevHighMicros;
  int frames;
  double timeDiff;
  double freq;
  double framePeriod;
  double maxFreq;
  double freqError;
  double smoothing;
  double freqSmoothed;
}
Photogate;

// Threshold values:
//  - Direct: ?
//  - Direct + Schmitt Trigger: ?
//  - Pulldown: 5000
//  - Pulldown + Schmitt Trigger: 0

Photogate *
photogateInit(int pin, int flickerThreshold, double smoothing)
{
  Photogate * p = malloc(sizeof(Photogate));

  pinMode(pin, INPUT);

  p->pin = pin;
  p->raw = digitalRead(pin);
  p->value = digitalRead(pin);
  p->flickerThreshold = flickerThreshold;
  p->prevEdgeMicros = micros();
  p->prevHighMicros = micros();

  p->frames = 0;
  p->timeDiff = 0;
  p->freq = 0;
  p->framePeriod = 0;
  p->maxFreq = 0;
  p->freqError = 0;

  p->smoothing = smoothing;
  p->freqSmoothed = 0;

  return p;
}

void
photogateUpdate(Photogate * p, unsigned int currMicros)
{
  int prev = p->raw;
  int curr = digitalRead(p->pin);

  // Flicker Edge
  if (prev != curr)
  {
    int duration = currMicros - p->prevEdgeMicros;
    flickerlog("Pin %d was %d for %d ms", p->pin, prev, duration);

    // Criteria for a flicker that isn't noise
    if (duration > p->flickerThreshold)
    {
      // Rising Edge
      if (!p->value && curr)
      {
        p->value = 1;

        p->timeDiff = currMicros - p->prevHighMicros;
        p->timeDiff *= 1E-6;
        p->freq = 1 / p->timeDiff;

        p->framePeriod = p->timeDiff / p->frames;
        p->maxFreq = 1 / (p->timeDiff - p->framePeriod);
        p->freqError = p->maxFreq - p->freq;

        double dt = p->timeDiff >= 1 ? 1 : 0;
        p->freqSmoothed += (p->freq - p->freqSmoothed) * p->smoothing * dt;

        p->frames = 0;
        p->prevHighMicros = currMicros;
      }

      // Falling Edge
      else if (p->value && !curr)
      {
        p->value = 0;
      }
    }

    p->prevEdgeMicros = currMicros;
  }
  p->raw = curr;
  p->frames++;
}

int
main()
{
  wiringPiSetup();

  Photogate * greenGate = photogateInit(0, 0, 1);
  Photogate * yellowGate = photogateInit(1, 0, 1);

  unsigned int nextOutputMicros = micros();

  while (1)
  {
    unsigned int currMicros  = micros();
    photogateUpdate(greenGate, currMicros);
    photogateUpdate(yellowGate, currMicros);
    pinlog("%d\t%d\n", greenGate->raw, yellowGate->raw);
    if (nextOutputMicros < currMicros)
    {
      nextOutputMicros = currMicros + OUTPUT_PERIOD;
      datalog("%f\t%f\n", greenGate->freqSmoothed, yellowGate->freqSmoothed);
    }
  }
}
