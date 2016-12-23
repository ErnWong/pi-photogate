# A Photogate on a Raspberry Pi

Ahoy there! This is a simple, non-intrusive, motion-sensing device. Using LEDs as photodiodes, the program can detect when strong light is shining on the LED, and when this beam of light is blocked. Thus, this polling program can accurately and precisely timestamp when an object passes through this photogate. It can also find its velocity if you factor in the size of the object. Or, put this photogate over a spinning object, and it could find its RPM too.

## Disclaimer - a sanity notice

I am currently an amateur hobbyist, who is overwhelmed with joy when I finally get an LED lit up. In this project, I may have broken several hundred best practices regarding safety and reliability, as my only resources are the Internet and a few hobbyist books.

If feeling courageous, follow with caution.

## Ingredients

- Raspberry Pi
- A handful of resistors and some BJT NPN transistors (see the Circuit section below)
- A breadboard
- A handful of male-male jumper wires and female-male jumper wires. (Make sure their wire-gauge fits the breadboard snugly)
- Maybe some crocodile clips would come in handy, as well as a multimeter.

## Handling precautions

The BJT transistors and the raspberry pi could potentially be damaged (pun) by electrostatic discharge.

## Circuit

Each photogate uses the following circuitry:

![LED reverse biased, connected to a pull down resister, and fed into a schmitt trigger made out of BJT.](circuit.png?raw=true)

### Finding the right pull down resistance

Resister R1 may be different for you. To find an appropriate resistance, connect the 3.3V rail, LED, the resistor, and the ground rail as shown in the left side of the circuit (ignoring the rest). Using a voltmeter, increase the resistance until a lit LED causes around 2V reading over the resistor.

### How it works

```
Caution: another amateur nerd trying to sound like he knows his stuff.
```

When light shines onto the LED/photodiode, the photodiode generates a current through R1. This creates a voltage across R1, and this voltage is passed down to the schmitt trigger.

The NPN transistor Q1 starts to conduct. Initially, the voltage at the base of Q2 is determined by the voltage divider of R2 and R5. As Q1 starts to conduct, the voltage divider now consists of R2 at the top, and R4 and R5 in parallel at the bottom. Thus, the lower half of the voltage divider begins to drop in resistance, and the voltage at the base of Q2 also starts to drop.

The reduced base voltage of Q2 causes Q2 to start turning off, making the voltage at the emitter of Q1 to drop, and as a result, widens the p.d. between Q1's base and emitter. This means Q1 will further turn on.

As you can see, the entire cycle is a positive feedback loop, causing the transistors to "snap" on and off once the input voltage crosses the threshold. This is how the schmitt trigger works in a nutshell.

Another useful feature of the schmitt trigger is hysteresis. Once the trigger changes from LOW to HIGH, it requires a lower voltage to change it back from HIGH to LOW, and vice versa. This eliminates most of the noise from the photodiode.

## Program Dependencies

The program uses [WiringPi](wiringpi.com) so you'll need to [install](wiringpi.com/download-and-install/) on your raspberry pi before you embark your journey.

## Getting it to work

After you've set up the circuitry, the wiring, and you've adjusted the code in `main.c` to your likings, compile by running:

```sh
make
```

Then, run the program with:

```sh
sudo ./program
```

## Limitations

It's not perfect though. Noise from the LED as well as your own body's static may cause unwanted flickering, although that should be dealt with by the schmitt trigger and the software debouncer.

The software debouncer works by filtering out the pin changes that happen too quickly. Thus, when the software debouncer is activated, it imposes an upper limit on the frequency that the photogate can detect.

The detectable frequency range is also limited by the refresh rate of the program. The program estimates the possible error in `photogate.freqError`.
