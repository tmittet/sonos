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

#include "SonosUPnP.h"

SonosUPnP::SonosUPnP(EthernetClient client, void (*ethernetErrCallback)(void))
{
  #ifndef SONOS_WRITE_ONLY_MODE
  this->xPath = MicroXPath();
  #endif
  this->ethClient = client;
  this->ethernetErrCallback = ethernetErrCallback;
}


void SonosUPnP::setAVTransportURI(IPAddress speakerIP, const char *uri)
{
  setAVTransportURI(speakerIP, uri, "<CurrentURIMetaData></CurrentURIMetaData>");
}

void SonosUPnP::setAVTransportURI(IPAddress speakerIP, const char *uri, const char *uriMetaData)
{
  upnpSet(
    speakerIP, UPNP_AV_TRANSPORT,
    // Info to show in player, in DIDL format, can be added as META data
    "SetAVTransportURI", "CurrentURI", uri, uriMetaData);
}

void SonosUPnP::seekTrack(IPAddress speakerIP, uint16_t index)
{
  char indexChar[6];
  itoa(index, indexChar, 10);
  seek(speakerIP, SONOS_SEEK_MODE_TRACK_NR, indexChar);
}

void SonosUPnP::seekTime(IPAddress speakerIP, uint8_t hour, uint8_t minute, uint8_t second)
{
  char time[11];
  sprintf(time, "%d:%02d:%02d", hour, minute, second);
  seek(speakerIP, SONOS_SEEK_MODE_REL_TIME, time);
}

void SonosUPnP::setPlayMode(IPAddress speakerIP, uint8_t playMode)
{
  const char *playModeValue;
  switch (playMode)
  {
    case SONOS_PLAY_MODE_REPEAT:
      playModeValue = SONOS_PLAY_MODE_REPEAT_VALUE;
      break;
    case SONOS_PLAY_MODE_SHUFFLE_REPEAT:
      playModeValue = SONOS_PLAY_MODE_SHUFFLE_REPEAT_VALUE;
      break;
    case SONOS_PLAY_MODE_SHUFFLE:
      playModeValue = SONOS_PLAY_MODE_SHUFFLE_VALUE;
      break;
    default:
      playModeValue = SONOS_PLAY_MODE_NORMAL_VALUE;
      break;
  }
  upnpSet(speakerIP, UPNP_AV_TRANSPORT, "SetPlayMode", "NewPlayMode", playModeValue);
}

void SonosUPnP::play(IPAddress speakerIP)
{
  upnpSet(speakerIP, UPNP_AV_TRANSPORT, "Play", "Speed", "1");
}

void SonosUPnP::playFile(IPAddress speakerIP, const char *uri)
{
  char ruri[301];
  sprintf(ruri, "%s%s", SONOS_SOURCE_FILE_PREFIX, uri);
  setAVTransportURI(speakerIP, ruri, SONOS_URI_META_LIGHT);
  play(speakerIP);
}

void SonosUPnP::playHttp(IPAddress speakerIP, const char *uri)
{
  char ruri[301];
  sprintf(ruri, "%s%s", SONOS_SOURCE_HTTP_PREFIX, uri);
  setAVTransportURI(speakerIP, ruri, SONOS_URI_META_LIGHT);
  play(speakerIP);
}

void SonosUPnP::playRadio(IPAddress speakerIP, const char *uri, const char *title)
{
  char ruri[201];
  sprintf(ruri, "%s%s", SONOS_SOURCE_RADIO_PREFIX, uri);
  char extra[701];
  sprintf(extra, SONOS_RADIO_META_FULL, title);
  setAVTransportURI(speakerIP, ruri, extra);
  play(speakerIP);
}

void SonosUPnP::playLineIn(IPAddress speakerIP, const char *speakerID)
{
  char uri[42];
  sprintf(uri, "%sRINCON_%s0%d", SONOS_SOURCE_LINEIN_PREFIX, speakerID, UPNP_PORT);
  setAVTransportURI(speakerIP, uri);
  play(speakerIP);
}

void SonosUPnP::playQueue(IPAddress speakerIP, const char *speakerID)
{
  char uri[42];
  sprintf(uri, "%sRINCON_%s0%d#0", SONOS_SOURCE_QUEUE_PREFIX, speakerID, UPNP_PORT);
  setAVTransportURI(speakerIP, uri);
  play(speakerIP);
}

void SonosUPnP::playConnectToMaster(IPAddress speakerIP, const char *masterSpeakerID)
{
  char uri[35];
  sprintf(uri, "%sRINCON_%s0%d", SONOS_SOURCE_MASTER_PREFIX, masterSpeakerID, UPNP_PORT);
  setAVTransportURI(speakerIP, uri);
}

void SonosUPnP::disconnectFromMaster(IPAddress speakerIP)
{
  upnpSet(speakerIP, UPNP_AV_TRANSPORT, "BecomeCoordinatorOfStandaloneGroup");
}

void SonosUPnP::stop(IPAddress speakerIP)
{
  upnpSet(speakerIP, UPNP_AV_TRANSPORT, "Stop");
}

void SonosUPnP::pause(IPAddress speakerIP)
{
  upnpSet(speakerIP, UPNP_AV_TRANSPORT, "Pause");
}

void SonosUPnP::skip(IPAddress speakerIP, uint8_t direction)
{
  upnpSet(
    speakerIP, UPNP_AV_TRANSPORT,
    direction == SONOS_DIRECTION_FORWARD ? "Next" : "Previous");
}

void SonosUPnP::setMute(IPAddress speakerIP, bool state)
{
  char channel[26];
  sprintf(channel, SONOS_CHANNEL_TEMPLATE, SONOS_CHANNEL_MASTER);
  upnpSet(
    speakerIP, UPNP_RENDERING_CONTROL,
    "SetMute", "DesiredMute", state ? "1" : "0", channel);
}

void SonosUPnP::setVolume(IPAddress speakerIP, uint8_t volume)
{
  setVolume(speakerIP, volume, SONOS_CHANNEL_MASTER);
}

void SonosUPnP::setVolume(IPAddress speakerIP, uint8_t volume, const char *channel)
{
  char channelBuffer[26];
  sprintf(channelBuffer, SONOS_CHANNEL_TEMPLATE, channel);
  if (volume > 100) volume = 100;
  char volumeChar[4];
  itoa(volume, volumeChar, 10);
  upnpSet(
    speakerIP, UPNP_RENDERING_CONTROL,
    "SetVolume", "DesiredVolume", volumeChar, channelBuffer);
}

void SonosUPnP::setBass(IPAddress speakerIP, int8_t bass)
{
  bass = constrain(bass, -10, 10);
  char bassChar[4];
  itoa(bass, bassChar, 10);
  upnpSet(speakerIP, UPNP_RENDERING_CONTROL, "SetBass", "DesiredBass", bassChar);
}

void SonosUPnP::setTreble(IPAddress speakerIP, int8_t treble)
{
  treble = constrain(treble, -10, 10);
  char trebleChar[4];
  itoa(treble, trebleChar, 10);
  upnpSet(speakerIP, UPNP_RENDERING_CONTROL, "SetTreble", "DesiredTreble", trebleChar);
}

void SonosUPnP::setLoudness(IPAddress speakerIP, bool state)
{
  char channel[26];
  sprintf(channel, SONOS_CHANNEL_TEMPLATE, SONOS_CHANNEL_MASTER);
  upnpSet(
    speakerIP, UPNP_RENDERING_CONTROL,
    "SetLoudness", "DesiredLoudness", state ? "1" : "0", channel);
}

void SonosUPnP::setStatusLight(IPAddress speakerIP, bool state)
{
  upnpSet(
    speakerIP, UPNP_DEVICE_PROPERTIES,
    "SetLEDState", "DesiredLEDState", state ? "On" : "Off");
}

void SonosUPnP::addPlaylistToQueue(IPAddress speakerIP, uint16_t playlistIndex)
{
  char uri[45];
  sprintf(uri, SONOS_SAVED_QUEUES, playlistIndex);
  upnpSet(
    speakerIP, UPNP_AV_TRANSPORT,
    "AddURIToQueue", "EnqueuedURI", uri, SONOS_PLAYLIST_META_LIGHT);
}

/*
void SonosUPnP::addPlaylistToQueue(IPAddress speakerIP, uint16_t playlistIndex, const char *playlistName)
{
  char uri[45];
  sprintf(uri, SONOS_SAVED_QUEUES, playlistIndex);
  char extra[801];
  sprintf(extra, SONOS_PLAYLIST_META_FULL, playlistName);
  upnpSet(
    speakerIP, UPNP_AV_TRANSPORT,
    "AddURIToQueue", "EnqueuedURI", uri, extra);
}
*/

void SonosUPnP::removeAllTracksFromQueue(IPAddress speakerIP)
{
  upnpSet(speakerIP, UPNP_AV_TRANSPORT, "RemoveAllTracksFromQueue");
}


#ifndef SONOS_WRITE_ONLY_MODE

void SonosUPnP::setRepeat(IPAddress speakerIP, bool repeat)
{
  bool shuffle = getShuffle(speakerIP);
  setPlayMode(speakerIP, convertPlayMode(repeat, shuffle));
}

void SonosUPnP::setShuffle(IPAddress speakerIP, bool shuffle)
{
  bool repeat = getRepeat(speakerIP);
  setPlayMode(speakerIP, convertPlayMode(repeat, shuffle));
}

void SonosUPnP::toggleRepeat(IPAddress speakerIP)
{
  uint8_t playMode = getPlayMode(speakerIP);
  bool repeat = convertRepeat(playMode);
  bool shuffle = convertShuffle(playMode);
  setPlayMode(speakerIP, convertPlayMode(!repeat, shuffle));
}

void SonosUPnP::toggleShuffle(IPAddress speakerIP)
{
  uint8_t playMode = getPlayMode(speakerIP);
  bool repeat = convertRepeat(playMode);
  bool shuffle = convertShuffle(playMode);
  setPlayMode(speakerIP, convertPlayMode(repeat, !shuffle));
}

void SonosUPnP::togglePause(IPAddress speakerIP)
{
  uint8_t state = getState(speakerIP);
  if (state == SONOS_STATE_PLAYING)
  {
    pause(speakerIP);
  }
  else if (state == SONOS_STATE_PAUSED)
  {
    play(speakerIP);
  }
}

void SonosUPnP::toggleMute(IPAddress speakerIP)
{
  setMute(speakerIP, !getMute(speakerIP));
}

void SonosUPnP::toggleLoudness(IPAddress speakerIP)
{
  setLoudness(speakerIP, !getLoudness(speakerIP));
}

uint8_t SonosUPnP::getState(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetTransportInfoResponse", "CurrentTransportState" };
  //                   { "s:Envelope", "s:Body", "u:GetTransportInfoResponse", "CurrentSpeed" };
  char result[sizeof(SONOS_STATE_PAUSED_VALUE) + 1] = "";
  upnpAvTransportGet(speakerIP, "GetTransportInfo", path, 4, result, sizeof(result));
  return convertState(result);
}

uint8_t SonosUPnP::getPlayMode(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetTransportSettingsResponse", "PlayMode" };
  char result[sizeof(SONOS_PLAY_MODE_SHUFFLE_VALUE) + 1] = "";
  upnpAvTransportGet(speakerIP, "GetTransportSettings", path, 4, result, sizeof(result));
  return convertPlayMode(result);
}

bool SonosUPnP::getRepeat(IPAddress speakerIP)
{
  return convertRepeat(getPlayMode(speakerIP));
}

bool SonosUPnP::getShuffle(IPAddress speakerIP)
{
  return convertShuffle(getPlayMode(speakerIP));
}

uint16_t SonosUPnP::getTrackNumber(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetPositionInfoResponse", "Track" };
  char result[6] = "0";
  upnpAvTransportGet(speakerIP, "GetPositionInfo", path, 4, result, sizeof(result));
  return atoi(result);
}

void SonosUPnP::getTrackURI(IPAddress speakerIP, char *resultBuffer, size_t resultBufferSize)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetPositionInfoResponse", "TrackURI" };
  upnpAvTransportGet(speakerIP, "GetPositionInfo", path, 4, resultBuffer, resultBufferSize);
}

uint8_t SonosUPnP::getSource(IPAddress speakerIP)
{
  char result[25] = "";
  getTrackURI(speakerIP, result, sizeof(result));
  return getSourceFromURI(result);
}

uint8_t SonosUPnP::getSourceFromURI(const char *uri)
{
  if (!strncmp(SONOS_SOURCE_FILE_PREFIX, uri, sizeof(SONOS_SOURCE_FILE_PREFIX) - 1))
  {
    return SONOS_SOURCE_FILE;
  }
  if (!strncmp(SONOS_SOURCE_HTTP_PREFIX, uri, sizeof(SONOS_SOURCE_HTTP_PREFIX) - 1))
  {
    return SONOS_SOURCE_HTTP;
  }
  if (!strncmp(SONOS_SOURCE_RADIO_PREFIX, uri, sizeof(SONOS_SOURCE_RADIO_PREFIX) - 1))
  {
    return SONOS_SOURCE_RADIO;
  }
  if (!strncmp(SONOS_SOURCE_MASTER_PREFIX, uri, sizeof(SONOS_SOURCE_MASTER_PREFIX) - 1))
  {
    return SONOS_SOURCE_MASTER;
  }
  if (!strncmp(SONOS_SOURCE_LINEIN_PREFIX, uri, sizeof(SONOS_SOURCE_LINEIN_PREFIX) - 1))
  {
    return SONOS_SOURCE_LINEIN;
  }
  return SONOS_SOURCE_UNKNOWN;
}

uint32_t SonosUPnP::getTrackDurationInSeconds(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetPositionInfoResponse", "TrackDuration" };
  char result[20] = "";
  upnpAvTransportGet(speakerIP, "GetPositionInfo", path, 4, result, sizeof(result));
  return getTimeInSeconds(result);
}

uint32_t SonosUPnP::getTrackPositionInSeconds(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetPositionInfoResponse", "RelTime" };
  char result[20] = "";
  upnpAvTransportGet(speakerIP, "GetPositionInfo", path, 4, result, sizeof(result));
  return getTimeInSeconds(result);
}

uint16_t SonosUPnP::getTrackPositionPerMille(IPAddress speakerIP)
{
  uint16_t perMille = 0;
  if (ethClient.connect(speakerIP, UPNP_PORT))
  {
    upnpPost(speakerIP, UPNP_AV_TRANSPORT, "GetPositionInfo", "!--", "", "");
    waitForResponse();
    
    const char *durationPath[] = { "s:Envelope", "s:Body", "u:GetPositionInfoResponse", "TrackDuration" };
    const char *currentPath[] = { "s:Envelope", "s:Body", "u:GetPositionInfoResponse", "RelTime" };
    char result[20];
    xPath.reset();
    xPath.setPath(durationPath, 4);
    while (ethClient.available() && !xPath.getValue(ethClient.read(), result, sizeof(result)));
    uint32_t durationSeconds = getTimeInSeconds(result);
    xPath.setPath(currentPath, 4);
    while (ethClient.available() && !xPath.getValue(ethClient.read(), result, sizeof(result)));
    uint32_t currentSeconds = getTimeInSeconds(result);
    while (ethClient.available()) ethClient.flush();
    if (durationSeconds && currentSeconds)
    {
      perMille = round(currentSeconds * 1000.0 / durationSeconds);
    }    
  }
  else
  {
    if (ethernetErrCallback) ethernetErrCallback();
  }
  ethClient.stop();
  return perMille;
}

bool SonosUPnP::getMute(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetMuteResponse", "CurrentMute" };
  char result[3] = "0";
  upnpRenderingControlGet(
    speakerIP, "GetMute", "Channel", SONOS_CHANNEL_MASTER, path, 4, result, sizeof(result));
  return strcmp(result, "1") == 0;
}

uint8_t SonosUPnP::getVolume(IPAddress speakerIP)
{
  getVolume(speakerIP, SONOS_CHANNEL_MASTER);
}

uint8_t SonosUPnP::getVolume(IPAddress speakerIP, const char *channel)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetVolumeResponse", "CurrentVolume" };
  char result[5] = "0";
  upnpRenderingControlGet(
    speakerIP, "GetVolume", "Channel", channel, path, 4, result, sizeof(result));
  return constrain(atoi(result), 0, 100);
}

bool SonosUPnP::getOutputFixed(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetOutputFixedResponse", "CurrentFixed" };
  char result[3] = "0";
  upnpRenderingControlGet(
    speakerIP, "GetOutputFixed", "!--", "", path, 4, result, sizeof(result));
  return strcmp(result, "1") == 0;
}

int8_t SonosUPnP::getBass(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetBassResponse", "CurrentBass" };
  char result[5] = "0";
  upnpRenderingControlGet(
    speakerIP, "GetBass", "Channel", SONOS_CHANNEL_MASTER, path, 4, result, sizeof(result));
  return constrain(atoi(result), -10, 10);
}

int8_t SonosUPnP::getTreble(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetTrebleResponse", "CurrentTreble" };
  char result[5] = "0";
  upnpRenderingControlGet(
    speakerIP, "GetTreble", "Channel", SONOS_CHANNEL_MASTER, path, 4, result, sizeof(result));
  return constrain(atoi(result), -10, 10);
}

bool SonosUPnP::getLoudness(IPAddress speakerIP)
{
  const char *path[] = { "s:Envelope", "s:Body", "u:GetLoudnessResponse", "CurrentLoudness" };
  char result[3] = "0";
  upnpRenderingControlGet(
    speakerIP, "GetLoudness", "Channel", SONOS_CHANNEL_MASTER, path, 4, result, sizeof(result));
  return strcmp(result, "1") == 0;
}

#endif


void SonosUPnP::seek(IPAddress speakerIP, const char *mode, const char *data)
{
  const char *action = "Seek";
  const char *command = "Target";
  char unit[25];
  sprintf(unit, "<Unit>%s</Unit>", mode);
  upnpSet(speakerIP, UPNP_AV_TRANSPORT, action, command, data, unit);
}

void SonosUPnP::upnpSet(IPAddress ip, uint8_t upnpMessageType, const char *action)
{
  upnpSet(ip, upnpMessageType, action, "!--", "", "");
}

void SonosUPnP::upnpSet(IPAddress ip, uint8_t upnpMessageType, const char *action, const char *command, const char *value)
{
  upnpSet(ip, upnpMessageType, action, command, value, "");
}

void SonosUPnP::upnpSet(IPAddress ip, uint8_t upnpMessageType, const char *action, const char *command, const char *value, const char *extra)
{
  if (ethClient.connect(ip, UPNP_PORT))
  {
    upnpPost(ip, upnpMessageType, action, command, value, extra);
    waitForResponse();
    while (ethClient.available())
    {
      ethClient.flush();
    }
  }
  else
  {
    if (ethernetErrCallback) ethernetErrCallback();
  }
  ethClient.stop();
}

void SonosUPnP::upnpPost(IPAddress ip, uint8_t upnpMessageType, const char *action, const char *command, const char *value, const char *extra)
{  
  // Buffer length: 100 for command + 200 for media URI
  char commandBuffer[300];
  sprintf(commandBuffer, SONOS_COMMAND, command, value, command);
  const char *upnpUrn = getUpnpService(upnpMessageType);
  uint16_t extraLen = strlen(extra);

  uint16_t contentLength =
    sizeof(SOAP_ENVELOPE_START) - 1 +
    sizeof(SOAP_BODY_START) - 1 +
    SONOS_ACTION_TAG_LEN +
    (strlen(action) * 2) +
    sizeof(UPNP_URN_PREFIX) - 1 +
    strlen(upnpUrn) +
    sizeof(SONOS_INSTANCE_ID_0) - 1 +
    strlen(commandBuffer) +
    extraLen +
    sizeof(SOAP_BODY_END) - 1 +
    sizeof(SOAP_ENVELOPE_END) - 1;

  char headerBuffer[100];
  sprintf(headerBuffer, HTTP_HEADER_POST, getUpnpEndpoint(upnpMessageType));
  ethClient.println(headerBuffer);
  sprintf(headerBuffer, HTTP_HEADER_HOST, ip[0], ip[1], ip[2], ip[3], UPNP_PORT);
  ethClient.println(headerBuffer);
  ethClient.println(HTTP_HEADER_CONTENT_TYPE);
  sprintf(headerBuffer, HTTP_HEADER_CONTENT_LENGTH, contentLength);
  ethClient.println(headerBuffer);
  sprintf(headerBuffer, HTTP_HEADER_SOAP_ACTION, UPNP_URN_PREFIX, upnpUrn, action);
  ethClient.println(headerBuffer);
  ethClient.println(HTTP_HEADER_CONNECTION);
  ethClient.println("");
  
  ethClient.print(SOAP_ENVELOPE_START);
  ethClient.print(SOAP_BODY_START);
  ethClient.print(SONOS_ACTION_START_TAG_START);
  ethClient.print(action);
  ethClient.print(SONOS_ACTION_START_TAG_NS);
  ethClient.print(UPNP_URN_PREFIX);
  ethClient.print(upnpUrn);
  ethClient.print(SONOS_ACTION_START_TAG_END);
  ethClient.print(SONOS_INSTANCE_ID_0);
  ethClient.print(commandBuffer);
  if (extraLen) ethClient.print(extra);
  ethClient.print(SONOS_ACTION_END_TAG_START);
  ethClient.print(action);
  ethClient.print(SONOS_ACTION_END_TAG_END);
  ethClient.print(SOAP_BODY_END);
  ethClient.print(SOAP_ENVELOPE_END);
}

const char *SonosUPnP::getUpnpService(uint8_t upnpMessageType)
{
  switch (upnpMessageType)
  {
    case UPNP_AV_TRANSPORT: return UPNP_AV_TRANSPORT_SERVICE;
    case UPNP_RENDERING_CONTROL: return UPNP_RENDERING_CONTROL_SERVICE;
    case UPNP_DEVICE_PROPERTIES: return UPNP_DEVICE_PROPERTIES_SERVICE;
  }
}

const char *SonosUPnP::getUpnpEndpoint(uint8_t upnpMessageType)
{
  switch (upnpMessageType)
  {
    case UPNP_AV_TRANSPORT: return UPNP_AV_TRANSPORT_ENDPOINT;
    case UPNP_RENDERING_CONTROL: return UPNP_RENDERING_CONTROL_ENDPOINT;
    case UPNP_DEVICE_PROPERTIES: return UPNP_DEVICE_PROPERTIES_ENDPOINT;
  }
}

void SonosUPnP::waitForResponse()
{
  uint32_t start = millis();
  while (!ethClient.available())
  {
    if (millis() > (start + UPNP_RESPONSE_TIMEOUT_MS))
    {
      if (ethernetErrCallback) ethernetErrCallback();
      break;
    }
  }
}

#ifndef SONOS_WRITE_ONLY_MODE

void SonosUPnP::upnpAvTransportGet(IPAddress speakerIP, const char *action, const char *path[], size_t pathSize, char *resultBuffer, size_t resultBufferSize)
{
  if (ethClient.connect(speakerIP, UPNP_PORT))
  {
    upnpPost(speakerIP, UPNP_AV_TRANSPORT, action, "!--", "", "");
    waitForResponse();
    xPath.reset();
    xPath.setPath(path, pathSize);
    while (ethClient.available() && !xPath.getValue(ethClient.read(), resultBuffer, resultBufferSize));
    while (ethClient.available()) ethClient.flush();
    ethClient.stop();
  }
  else
  {
    if (ethernetErrCallback) ethernetErrCallback();
    ethClient.stop();
  }
}

void SonosUPnP::upnpRenderingControlGet(IPAddress speakerIP, const char *action, const char *command, const char *value, const char *path[], size_t pathSize, char *resultBuffer, size_t resultBufferSize)
{
  if (ethClient.connect(speakerIP, UPNP_PORT))
  {
    upnpPost(speakerIP, UPNP_AV_TRANSPORT, action, command, value, "");
    waitForResponse();
    xPath.reset();
    xPath.setPath(path, pathSize);
    while (ethClient.available() && !xPath.getValue(ethClient.read(), resultBuffer, resultBufferSize));
    while (ethClient.available()) ethClient.flush();
    ethClient.stop();
  }
  else
  {
    if (ethernetErrCallback) ethernetErrCallback();
    ethClient.stop();
  }
}

uint32_t SonosUPnP::getTimeInSeconds(const char *time)
{
  uint8_t len = strlen(time);
  uint32_t seconds = 0;
  uint8_t dPower = 0;
  uint8_t tPower = 0;
  for (int8_t i = len; i > 0; i--)
  {
    char character = time[i - 1];
    if (character == ':')
    {
      dPower = 0;
      tPower++;
    }
    else if(character >= '0' && character <= '9')
    {
      seconds += round((character - '0') * pow(10, dPower) * pow(60, tPower));
      dPower++;
    }
  }  
  return seconds;
}

uint8_t SonosUPnP::convertState(const char *input)
{
  if (strcmp(input, SONOS_STATE_PLAYING_VALUE) == 0) return SONOS_STATE_PLAYING;
  if (strcmp(input, SONOS_STATE_PAUSED_VALUE) == 0)  return SONOS_STATE_PAUSED;
  return SONOS_STATE_STOPPED;
}

uint8_t SonosUPnP::convertPlayMode(const char *input)
{
  if (strcmp(input, SONOS_PLAY_MODE_NORMAL_VALUE) == 0)         return SONOS_PLAY_MODE_NORMAL;
  if (strcmp(input, SONOS_PLAY_MODE_REPEAT_VALUE) == 0)         return SONOS_PLAY_MODE_REPEAT;
  if (strcmp(input, SONOS_PLAY_MODE_SHUFFLE_REPEAT_VALUE) == 0) return SONOS_PLAY_MODE_SHUFFLE_REPEAT;
  if (strcmp(input, SONOS_PLAY_MODE_SHUFFLE_VALUE) == 0)        return SONOS_PLAY_MODE_SHUFFLE;
  return SONOS_PLAY_MODE_NORMAL;
}

uint8_t SonosUPnP::convertPlayMode(bool repeat, bool shuffle)
{
  if (repeat)
  {
    return shuffle ? SONOS_PLAY_MODE_SHUFFLE_REPEAT : SONOS_PLAY_MODE_REPEAT;
  }
  else
  {
    return shuffle ? SONOS_PLAY_MODE_SHUFFLE : SONOS_PLAY_MODE_NORMAL;
  }
}

bool SonosUPnP::convertRepeat(uint8_t playMode)
{
  return
    playMode == SONOS_PLAY_MODE_REPEAT ||
    playMode == SONOS_PLAY_MODE_SHUFFLE_REPEAT;
}

bool SonosUPnP::convertShuffle(uint8_t playMode)
{
  return
    playMode == SONOS_PLAY_MODE_SHUFFLE ||
    playMode == SONOS_PLAY_MODE_SHUFFLE_REPEAT;
}

#endif

