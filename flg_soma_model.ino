// vim:set ts=4 sw=4 ai et:

/*
 * Arduino code for lighting FLG's model of Soma.  Written during the week leading
 * up to the party on May 31st, 2013.
 *
 *        -- Michael Toren <mct@toren.net>
 *           Fri May 31 15:55:48 PDT 2013
 */

/*
 * There are three conceptually different parts of the model: The two somas
 * (spheres) at either end, and the axon (bridge) that connects them.  See
 * <http://en.wikipedia.org/wiki/Neuron> for a picture of a typical neuron,
 * but some ASCII art follows:
 *
 *                                      __
 *                                   -      `_
 *                                 _/   _  _   \        ____
 *                               /    /      \    \   .X+.   .
 *                            /    /            \  x.Xx+-.     .
 *                          /   / ^                \XXx++-..
 *                        /   /                     XXxx++--..
 *                 ____  /  /                       `XXXxx+++--'
 *               .X+.   ,  /                          `XXXxxx'
 *             .Xx+-.    `,                             |""|
 *             XXx++-..                                 |''|
 *             XXxx++--..                               |""|
 *             `XXXxx+++--'                             |''|
 *               `XXXxxx'                               |''|
 *                 |""|                                 |""|
 *           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We'll treat each component as a separate light effect, and drive each with
 * its own state machine.  Occasionally a component will influence the state of
 * a neighboring component (e.g., when triggering an impulse across the axon),
 * but the majority of the time each state machine is doing its own thing.
 *
 * The state machines are named "sender" (for the soma on the left that
 * generates pulses), "axon" (for the axon in the middle), and "receiver" for
 * the soma on the right.  Each state machine has its own "heartbeat" function,
 * called every N milliseconds.
 */

#include <Adafruit_NeoPixel.h>

// There are 12 rings on the Axon.  Each ring has 3 LEDS.
// Each Soma at either end of the Axon has 11 LEDS.
#define NUM_RINGS       12
#define NUM_SOMA_LEDS   11
#define NUM_LEDS        (NUM_SOMA_LEDS + NUM_RINGS*3 + NUM_SOMA_LEDS)
#define DATA_PIN        6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// Current time, in milliseconds. Updated before calling each "heartbeat" function.
unsigned long now;

// Returns ture N percent of the time
int probability(int percent)
{
    if (random(100)+1 <= percent)
        return 1;
    else
        return 0;
}

void receiver_fire();

/* Axon */

int axon_state = 100;
int axon_repeat_count;

void axon_fire()
{
    axon_state = 0;
    axon_repeat_count = 0;
}

// Light portions of the given ring.
//
// @pa: Probability the 1st LED in this ring will be lit
// @pb: Probability the 2nd LED in this ring will be lit
// @pc: Probability the 34d LED in this ring will be lit
// @n:  Ring number, in the range [0 .. NUM_RINGS-1]
// @r:  Red value
// @g:  Green value
// @b:  Blue value
void axon_ring(int pa, int pb, int pc, int n, int r, int g, int b)
{
    if (n < 0) return;
    if (n >= NUM_RINGS) return;
    int offset = random(2);
    if (probability(pa)) strip.setPixelColor(NUM_SOMA_LEDS + n*3 + (0+offset)%3, r, g, b);
    if (probability(pb)) strip.setPixelColor(NUM_SOMA_LEDS + n*3 + (1+offset)%3, r, g, b);
    if (probability(pc)) strip.setPixelColor(NUM_SOMA_LEDS + n*3 + (2+offset)%3, r, g, b);
}

void axon_heartbeat()
{
    int i;

    for (i = 0; i < NUM_RINGS; i++)
        axon_ring(100, 100, 100, i, 0, 0, 0);

    if (axon_state < 13) {
        axon_ring( 25,  25,  25, axon_state-2,   64,  64,  64);
        axon_ring(100, 100, 100, axon_state-1,  128, 128, 128);
        axon_ring(100, 100, 100, axon_state,    255, 255, 255);
    }

    if (axon_state == 15)
        receiver_fire();

    if (axon_state < 200)
        axon_state++;
}


/* Sender */

unsigned long next_axon_fire_time;
int sender_state = 1;
int sender_red, sender_green, sender_blue;
int sender_sleep;
int sender_step = 3;

void sender_heartbeat() {
    if (now > next_axon_fire_time) {
       // Fire the axon between every 8 and 12 seconds
       next_axon_fire_time = now + 8000 + random(4000);
       sender_state = 3;
    }

    switch (sender_state) {
        case 1:
            if (sender_blue < 255)
                sender_blue += sender_step, sender_green -= sender_step;
            else
                sender_state++;
            break;

        case 2:
            if (sender_green < 255)
                sender_green += sender_step,  sender_blue -= sender_step;
            else
                sender_state--;
            break;

        case 3:
            sender_red = 255;
            sender_green = 255;
            sender_blue = 255;
            sender_sleep = 3;
            sender_state++;
            axon_fire();
            break;

        case 4:
            if (sender_sleep-- == 0)
                sender_state++;
            break;

        case 5:
            if (sender_red)
                sender_red -= 5, sender_blue -= 5;
            else
                sender_state = 1;
            break;
    }

    if (sender_red   > 255) sender_red   = 255;
    if (sender_green > 255) sender_green = 255;
    if (sender_blue  > 255) sender_blue  = 255;
    if (sender_red   <   0) sender_red   = 0;
    if (sender_green <   0) sender_green = 0;
    if (sender_blue  <   0) sender_blue  = 0;

    for (int i = 0; i < NUM_SOMA_LEDS; i++)
        strip.setPixelColor(i, sender_red, sender_green, sender_blue);
}



/* Receiver */

int receiver_state = 1;
int receiver_red, receiver_green, receiver_blue;
int receiver_sleep;
int receiver_step = 3;

void receiver_heartbeat() {
    switch (receiver_state) {
        case 1:
            if (receiver_green < 255)
                receiver_green += receiver_step, receiver_blue -= receiver_step;
            else
                receiver_state++;
            break;

        case 2:
            if (receiver_blue < 255)
                receiver_blue += receiver_step, receiver_green -= receiver_step;
            else
                receiver_state--;
            break;

        case 3:
            receiver_red = 255;
            receiver_green = 255;
            receiver_blue = 255;
            receiver_sleep = 50;
            receiver_state++;
            break;

        case 4:
            if (receiver_sleep-- == 0)
                receiver_state++;
            break;

        case 5:
            if (receiver_red > 128)
                receiver_red -= 1, receiver_blue -= random(1,3), receiver_green -= random(1,3);
            else
                receiver_state = 2;
            break;
    }

    if (receiver_red   > 255) receiver_red   = 255;
    if (receiver_green > 255) receiver_green = 255;
    if (receiver_blue  > 255) receiver_blue  = 255;
    if (receiver_red   <   0) receiver_red   = 0;
    if (receiver_green <   0) receiver_green = 0;
    if (receiver_blue  <   0) receiver_blue  = 0;

    for (int i = 0; i < NUM_SOMA_LEDS; i++)
        strip.setPixelColor(NUM_SOMA_LEDS + NUM_RINGS*3 + i, receiver_red, receiver_green, receiver_blue);

}

void receiver_fire() {
    receiver_state = 3;
}

void setup() {
    strip.begin();
    strip.show();
    randomSeed(analogRead(2));
    Serial.begin(9600);
}

void loop() {
    // For debugging
    if (Serial.available()) {
        int c = Serial.read();
        Serial.print("Received:" );
        Serial.println(c);
        next_axon_fire_time = 0;
    }

    now = millis();

    sender_heartbeat();
    axon_heartbeat();
    receiver_heartbeat();

    strip.show();
    delay(10);
}
