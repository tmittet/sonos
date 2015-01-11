/************************************************************************/
/* Sonos UPnP, an UPnP based read/write remote control library, v1.0.   */
/*                                                                      */
/* This library is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* This library is distributed in the hope that it will be useful, but  */
/* WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     */
/* General Public License for more details.                             */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with this library. If not, see <http://www.gnu.org/licenses/>. */
/*                                                                      */
/* Written by Thomas Mittet (code@lookout.no) January 2015.             */
/************************************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <SonosUPnP.h>
#include <MicroXPath.h>

#define ETHERNET_MAC (byte[]) {0x54, 0x48, 0x4F, 0x4D, 0x41, 0x53}
#define ETHERNET_STATIC_IP  (byte[]) {192, 168, 0, 123}

#define SONOS_LIVINGR_IP (byte[]) {192, 168, 0, 201}
#define SONOS_LIVINGR_ID "000111111111"
#define SONOS_BATHROOM_IP (byte[]) {192, 168, 0, 202}
#define SONOS_BATHROOM_ID "000222222222"
#define SONOS_BEDROOM_IP (byte[]) {192, 168, 0, 203}
#define SONOS_BEDROOM_ID "000333333333"
#define SONOS_STATUS_POLL_DELAY_MS 10000

#define SERIAL_DATA_THRESHOLD_MS 500
#define SERIAL_ERROR_TIMEOUT "Error: Serial"
#define ETHERNET_ERROR_DHCP "Error: DHCP"
#define ETHERNET_ERROR_CONNECT "Error: Connect"

EthernetClient g_ethClient;
SonosUPnP g_sonos = SonosUPnP(g_ethClient, ethConnectError);

unsigned long g_sonosLastStateUpdate = 0;
unsigned long g_serialDataReceived_ms;

void setup()
{
  if (!Ethernet.begin(ETHERNET_MAC))
  {
    Serial.println(ETHERNET_ERROR_DHCP);
    Ethernet.begin(ETHERNET_MAC, ETHERNET_STATIC_IP);
  }
  Serial.begin(9600);
}

void ethConnectError()
{
  Serial.println(ETHERNET_ERROR_CONNECT);
}

void loop()
{
  // Sonos state polling
  if (g_sonosLastStateUpdate > millis() || millis() > g_sonosLastStateUpdate + SONOS_STATUS_POLL_DELAY_MS)
  {
    byte source = g_sonos.getSource(SONOS_LIVINGR_IP);
    switch (source)
    {
      case SONOS_SOURCE_FILE:
        Serial.print("File");
        break;
      case SONOS_SOURCE_HTTP:
        Serial.print("HTTP");
        break;
      case SONOS_SOURCE_RADIO:
        Serial.println("Radio");
        break;
      case SONOS_SOURCE_LINEIN:
        Serial.println("Line-In");
        break;
      case SONOS_SOURCE_MASTER:
        Serial.println("Other Speaker");
        break;
      default:
        Serial.println("Unknown");
        break; 
    }
    if (source == SONOS_SOURCE_FILE || source == SONOS_SOURCE_HTTP)
    {
      Serial.print(", track = ");
      Serial.println(g_sonos.getTrackNumber(SONOS_LIVINGR_IP), DEC);
      //Serial.print(", pos = ");
      //Serial.println(g_sonos.getTrackPositionInSeconds(SONOS_LIVINGR_IP), DEC);
    }
    g_sonosLastStateUpdate = millis();
  }
  // Serial command timeout error handling
  if (g_serialDataReceived_ms && (g_serialDataReceived_ms > millis() || millis() - g_serialDataReceived_ms > SERIAL_DATA_THRESHOLD_MS))
  {
    Serial.println(SERIAL_ERROR_TIMEOUT);
    while(Serial.read() != -1);
    g_serialDataReceived_ms = 0;
  }
}

void serialEvent()
{
  // Read 2 bytes from serial buffer
  if (Serial.available() >= 2)
  {
    byte b1 = Serial.read();
    byte b2 = Serial.read();
    // Play
    if (isCommand("pl", b1, b2))
    {
      g_sonos.play(SONOS_LIVINGR_IP);
    }
    // Pause
    else if (isCommand("pa", b1, b2))
    {
      g_sonos.pause(SONOS_LIVINGR_IP);
    }
    // Stop
    else if (isCommand("st", b1, b2))
    {
      g_sonos.stop(SONOS_LIVINGR_IP);
    }
    // Previous
    else if (isCommand("pr", b1, b2))
    {
      g_sonos.skip(SONOS_LIVINGR_IP, SONOS_DIRECTION_BACKWARD);
    }
    // Next
    else if (isCommand("nx", b1, b2))
    {
      g_sonos.skip(SONOS_LIVINGR_IP, SONOS_DIRECTION_FORWARD);
    }
    // Play File
    else if (isCommand("fi", b1, b2))
    {
      g_sonos.playFile(SONOS_LIVINGR_IP, "//your_server_ip_address/music/test.mp3");
    }
    // Play HTTP
    else if (isCommand("ht", b1, b2))
    {
      // Playing file from Norwegian music service WIMP (SID = 20)
      g_sonos.playHttp(SONOS_LIVINGR_IP, "trackid_37554547.mp4?sid=20&amp;flags=32");
    }
    // Play Radio
    else if (isCommand("ra", b1, b2))
    {
      g_sonos.playRadio(SONOS_LIVINGR_IP, "//lyd.nrk.no/nrk_radio_p3_mp3_h.m3u", "NRK P3");
    }
    // Play Line In
    else if (isCommand("li", b1, b2))
    {
      g_sonos.playLineIn(SONOS_LIVINGR_IP, SONOS_LIVINGR_ID);
    }
    // Group
    else if (isCommand("gr", b1, b2))
    {
      g_sonos.playConnectToMaster(SONOS_BEDROOM_IP, SONOS_LIVINGR_ID);
      g_sonos.playConnectToMaster(SONOS_BATHROOM_IP, SONOS_LIVINGR_ID);
    }
    // UnGroup
    else if (isCommand("ug", b1, b2))
    {
      g_sonos.disconnectFromMaster(SONOS_BEDROOM_IP);
      g_sonos.disconnectFromMaster(SONOS_BATHROOM_IP);
    }
    // Toggle Repeat
    else if (isCommand("re", b1, b2))
    {
      g_sonos.toggleRepeat(SONOS_LIVINGR_IP);
    }
    // Toggle Shuffle
    else if (isCommand("sh", b1, b2))
    {
      g_sonos.toggleShuffle(SONOS_LIVINGR_IP);
    }
    // Toggle Loudness
    else if (isCommand("lo", b1, b2))
    {
      g_sonos.toggleLoudness(SONOS_LIVINGR_IP);
    }
    // Toggle Mute
    else if (isCommand("mu", b1, b2))
    {
      g_sonos.toggleMute(SONOS_LIVINGR_IP);
    }
    // Volume/Bass/Treble
    else if (b2 >= '0' && b2 <= '9')
    {
      // Volume 0 to 99
      if (b1 >= '0' && b1 <= '9')
      {
        g_sonos.setVolume(SONOS_LIVINGR_IP, ((b1 - '0') * 10) + (b2 - '0'));
      }
      // Bass 0 to -9
      else if (b1 == 'b')
      {
        g_sonos.setBass(SONOS_LIVINGR_IP, (b2 - '0') * -1);
      }
      // Bass 0 to 9
      else if (b1 == 'B')
      {
        g_sonos.setBass(SONOS_LIVINGR_IP, b2 - '0');
      }
      // Treble 0 to -9
      else if (b1 == 't')
      {
        g_sonos.setTreble(SONOS_LIVINGR_IP, (b2 - '0') * -1);
      }
      // Treble 0 to 9
      else if (b1 == 'T')
      {
        g_sonos.setTreble(SONOS_LIVINGR_IP, b2 - '0');
      }
    }
    g_serialDataReceived_ms = 0;
  }
  // If partial message was received: store received time
  if (Serial.available() && Serial.available() < 2 && !g_serialDataReceived_ms)
  {
    g_serialDataReceived_ms = millis();
  }
}

bool isCommand(const char *command, byte b1, byte b2)
{
  return *command == b1 && *++command == b2;
}
