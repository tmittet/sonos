/************************************************************************/
/* Sonos UPnP, an UPnP based read/write remote control library, v1.1.   */
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
#include <MicroXPath_P.h>

#define SONOS_STATUS_POLL_DELAY_MS 10000

#define SERIAL_DATA_THRESHOLD_MS 500
#define SERIAL_ERROR_TIMEOUT "E: Serial"
#define ETHERNET_ERROR_DHCP "E: DHCP"
#define ETHERNET_ERROR_CONNECT "E: Connect"

EthernetClient g_ethClient;
SonosUPnP g_sonos = SonosUPnP(g_ethClient, ethConnectError);

byte g_mac[] = {0x54, 0x48, 0x4F, 0x4D, 0x41, 0x53};
IPAddress g_ethernetStaticIP(192, 168, 0, 123);

IPAddress g_sonosLivingrIP(192, 168, 0, 201);
const char g_sonosLivingrID[] = "000111111111";
IPAddress g_sonosBathroomIP(192, 168, 0, 202);
const char g_sonosBathroomID[] = "000222222222";
IPAddress g_sonosBedroomIP(192, 168, 0, 203);
const char g_sonosBedroomID[] = "000333333333";

unsigned long g_serialDataReceived_ms;

void setup()
{
  if (!Ethernet.begin(g_mac))
  {
    Serial.println(ETHERNET_ERROR_DHCP);
    Ethernet.begin(g_mac, g_ethernetStaticIP);
  }
  Serial.begin(9600);
}

void ethConnectError()
{
  Serial.println(ETHERNET_ERROR_CONNECT);
}

void loop()
{
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
      g_sonos.play(g_sonosLivingrIP);
    }
    // Pause
    else if (isCommand("pa", b1, b2))
    {
      g_sonos.pause(g_sonosLivingrIP);
    }
    // Stop
    else if (isCommand("st", b1, b2))
    {
      g_sonos.stop(g_sonosLivingrIP);
    }
    // Previous
    else if (isCommand("pr", b1, b2))
    {
      g_sonos.skip(g_sonosLivingrIP, SONOS_DIRECTION_BACKWARD);
    }
    // Next
    else if (isCommand("nx", b1, b2))
    {
      g_sonos.skip(g_sonosLivingrIP, SONOS_DIRECTION_FORWARD);
    }
    // Play File
    else if (isCommand("fi", b1, b2))
    {
      g_sonos.playFile(g_sonosLivingrIP, "//your_server_ip_address/music/test.mp3");
    }
    // Play HTTP
    else if (isCommand("ht", b1, b2))
    {
      // Playing file from music service WIMP (SID = 20)
      g_sonos.playHttp(g_sonosLivingrIP, "trackid_37554547.mp4?sid=20&amp;flags=32");
    }
    // Play Radio
    else if (isCommand("ra", b1, b2))
    {
      g_sonos.playRadio(g_sonosLivingrIP, "//lyd.nrk.no/nrk_radio_p3_mp3_h.m3u", "NRK P3");
    }
    // Play Line In
    else if (isCommand("li", b1, b2))
    {
      g_sonos.playLineIn(g_sonosLivingrIP, g_sonosLivingrID);
    }
    // Group
    else if (isCommand("gr", b1, b2))
    {
      g_sonos.playConnectToMaster(g_sonosBedroomIP, g_sonosLivingrID);
      g_sonos.playConnectToMaster(g_sonosBathroomIP, g_sonosLivingrID);
    }
    // UnGroup
    else if (isCommand("ug", b1, b2))
    {
      g_sonos.disconnectFromMaster(g_sonosBedroomIP);
      g_sonos.disconnectFromMaster(g_sonosBathroomIP);
    }
    // Repeat On
    else if (isCommand("re", b1, b2))
    {
      g_sonos.setPlayMode(g_sonosLivingrIP, SONOS_PLAY_MODE_REPEAT);
    }
    // Shuffle On
    else if (isCommand("sh", b1, b2))
    {
      g_sonos.setPlayMode(g_sonosLivingrIP, SONOS_PLAY_MODE_SHUFFLE);
    }
    // Repeat and Shuffle On
    else if (isCommand("rs", b1, b2))
    {
      g_sonos.setPlayMode(g_sonosLivingrIP, SONOS_PLAY_MODE_SHUFFLE_REPEAT);
    }
    // Repeat and Shuffle Off
    else if (isCommand("no", b1, b2))
    {
      g_sonos.setPlayMode(g_sonosLivingrIP, SONOS_PLAY_MODE_NORMAL);
    }
    // Loudness On
    else if (isCommand("lo", b1, b2))
    {
      g_sonos.setLoudness(g_sonosLivingrIP, true);
    }
    // Loudness Off
    else if (isCommand("l_", b1, b2))
    {
      g_sonos.setLoudness(g_sonosLivingrIP, false);
    }
    // Mute On
    else if (isCommand("mu", b1, b2))
    {
      g_sonos.setMute(g_sonosLivingrIP, true);
    }
    // Mute Off
    else if (isCommand("m_", b1, b2))
    {
      g_sonos.setMute(g_sonosLivingrIP, false);
    }
    // Volume/Bass/Treble
    else if (b2 >= '0' && b2 <= '9')
    {
      // Volume 0 to 99
      if (b1 >= '0' && b1 <= '9')
      {
        g_sonos.setVolume(g_sonosLivingrIP, ((b1 - '0') * 10) + (b2 - '0'));
      }
      // Bass 0 to -9
      else if (b1 == 'b')
      {
        g_sonos.setBass(g_sonosLivingrIP, (b2 - '0') * -1);
      }
      // Bass 0 to 9
      else if (b1 == 'B')
      {
        g_sonos.setBass(g_sonosLivingrIP, b2 - '0');
      }
      // Treble 0 to -9
      else if (b1 == 't')
      {
        g_sonos.setTreble(g_sonosLivingrIP, (b2 - '0') * -1);
      }
      // Treble 0 to 9
      else if (b1 == 'T')
      {
        g_sonos.setTreble(g_sonosLivingrIP, b2 - '0');
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