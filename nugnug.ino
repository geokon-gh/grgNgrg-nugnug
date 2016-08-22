#include <FastLED.h>
#include <SPI.h> // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h> // UDP library from: bjoern@cs.stanford.edu 12/30/2008
// R+G+B = 3
#define LED_CHANNELS 3


// USER DEFINED VARIABLES
// ##########################################################################################
// adjust these according to your setup
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// This is limited to the hardware to 1522.
//You can use a smaller value if you need the allocated buffers to be smaller
#define UDP_PACKET_MAX_SIZE 540
#define MAX_LEDS_ON_PORT UDP_PACKET_MAX_SIZE/LED_CHANNELS

// The first port the system will listen on. Subsequent ports are incremented from this value
#define FIRST_PORT 8888

// So if you have NUM_PORTS = 3, then you will use ports: 8888,8888,8888
#define NUM_PORTS 8

// This specifies how many LEDs you will be driven from each pin.
// If a pin has more than MAX_LEDS_ON_PORT lights on it it will use  multiple ports
const int16_t lights_per_pin[] = {180, 180, 180, 180, 180, 180, 180, 180};

// This should be the size of the lights_per_pin array
#define NUM_PINS 8

// This should match the largest value in the lights_per_pin array
#define MAX_LEDS_ON_PIN 180
// ##########################################################################################
// THE PINS THE OUTPUT WILL COME OUT ON (these values come from the FastLED library)
/*
  uint16_t available_pins[] = {
  25,
  26,
  27,
  28,
  14,
  15,
  29,
  11};
*/


uint16_t lights_per_port[NUM_PORTS];
uint16_t ports_per_pin[NUM_PINS];
CRGB led_buffer[NUM_PINS*MAX_LEDS_ON_PIN];
bool getting_packets;
char packet_buffer[UDP_PACKET_MAX_SIZE];
EthernetUDP Udp_connections[NUM_PORTS];

void initializePorts()
{
    Ethernet.begin(mac, ip);
    for (size_t port = 0; port < NUM_PORTS; ++port)
    {
        Udp_connections[port].begin(FIRST_PORT + port);
    }
}

void initializePins()
{
  LEDS.addLeds<WS2811_PORTD, 8>(led_buffer, MAX_LEDS_ON_PIN).setCorrection(TypicalLEDStrip);
  LEDS.setBrightness(255);
  FastLED.show();
}

void calculateLightsAndPortsOnEachPin()
{
    size_t port_index = 0;
    for (size_t pin_index = 0; pin_index < NUM_PINS; ++pin_index)
    {
        int16_t lights_left_on_pin = lights_per_pin[pin_index];
        uint16_t ports_on_pin = 0;
        while(lights_left_on_pin > 0 )
        {
            if(lights_left_on_pin > MAX_LEDS_ON_PORT)
            {
                lights_per_port[port_index] = MAX_LEDS_ON_PORT;
            }
            else
            {
                lights_per_port[port_index] = lights_left_on_pin;
            }
            lights_left_on_pin -= MAX_LEDS_ON_PORT;
            ++port_index;
            ++ports_on_pin;
        }
        ports_per_pin[pin_index] = ports_on_pin;
    }
}
void setup() {
/*
   Serial.begin(115200);
   Serial.println("Hello"); */
   initializePorts();
   initializePins();
   calculateLightsAndPortsOnEachPin();
/*
   Serial.println("Your variables are set up");

   Serial.print("There are: ");
   Serial.print(NUM_PORTS);
   Serial.println(" ports");


   Serial.print("There are: ");
   Serial.print(NUM_PINS);
   Serial.println(" pins");

   Serial.print("There are a maxiumum of: ");
   Serial.print(MAX_LEDS_ON_PORT);
   Serial.println(" LEDs on a port");

   Serial.println("The lights per pin are:");
   for(size_t pin_index = 0; pin_index < NUM_PINS; ++pin_index)
   {
    Serial.print(lights_per_pin[pin_index]);
    Serial.print("\t");
   }

   Serial.println("");
   Serial.println("The lights per port are:");
   for(size_t port_index = 0; port_index < NUM_PORTS; ++port_index)
   {
    Serial.print(lights_per_port[port_index]);
    Serial.print("\t");
   }
   Serial.println("");

   Serial.println("The ports per pin are:");
   for(size_t port_index = 0; port_index < NUM_PINS; ++port_index)
   {
    Serial.print(ports_per_pin[port_index]);
    Serial.print("\t");
   }
   Serial.println("");
*/
   getting_packets = false;
}
void loop()
{
  int16_t port_index = 0;
  for (size_t pin_index = 0; pin_index < NUM_PINS; ++pin_index)
  {
    for (size_t port = 0; port < ports_per_pin[pin_index]; ++port)
    {
      EthernetUDP Udp_connection = Udp_connections[port_index];
      if (Udp_connection.parsePacket())   // if the port has a packet
      {
        const uint16_t lights_on_port = lights_per_port[port_index];
        Udp_connection.read(packet_buffer, lights_on_port * LED_CHANNELS);
        for (uint16_t light_number = 0; light_number < lights_on_port; ++light_number)
        {
          led_buffer[pin_index*MAX_LEDS_ON_PIN + port*MAX_LEDS_ON_PORT + light_number].setRGB(
                  packet_buffer[(light_number * LED_CHANNELS)],
                  packet_buffer[(light_number * LED_CHANNELS) + 1],
                  packet_buffer[(light_number * LED_CHANNELS) + 2]);
        }
      }
      ++port_index;
    }
  }
  FastLED.show();
}
