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

#ifndef SonosUPnP_h
#define SonosUPnP_h

#include "Arduino.h"
#include "..\MicroXPath\MicroXPath.h"
#include "..\Ethernet\EthernetClient.h"

//#define SONOS_WRITE_ONLY_MODE

// HTTP headers:
#define HTTP_HEADER_POST "POST /%s HTTP/1.1"
#define HTTP_HEADER_HOST "Host: %d.%d.%d.%d:%d"
#define HTTP_HEADER_CONTENT_TYPE "Content-Type: text/xml; charset=\"utf-8\""
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length: %d"
#define HTTP_HEADER_CONNECTION "Connection: close"
#define HTTP_HEADER_SOAP_ACTION "SOAPAction: \"urn:%s%s#%s\""

// UPnP config:
#define UPNP_PORT 1400
#define UPNP_MULTICAST_IP (byte[]) {239,255,255,250}
#define UPNP_MULTICAST_PORT 1900
#define UPNP_MULTICAST_TIMEOUT_S 2
#define UPNP_RESPONSE_TIMEOUT_MS 500

// UPnP message data:
#define UPNP_URN_PREFIX "schemas-upnp-org:service:"
#define UPNP_AV_TRANSPORT 1
#define UPNP_AV_TRANSPORT_SERVICE "AVTransport:1"
#define UPNP_AV_TRANSPORT_ENDPOINT "MediaRenderer/AVTransport/Control"
#define UPNP_RENDERING_CONTROL 2
#define UPNP_RENDERING_CONTROL_SERVICE "RenderingControl:1"
#define UPNP_RENDERING_CONTROL_ENDPOINT "MediaRenderer/RenderingControl/Control"
#define UPNP_DEVICE_PROPERTIES 3
#define UPNP_DEVICE_PROPERTIES_SERVICE "DeviceProperties:1"
#define UPNP_DEVICE_PROPERTIES_ENDPOINT "DeviceProperties/Control"

// SOAP message data:
//#define SOAP_XML_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define SOAP_ENVELOPE_START "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
#define SOAP_ENVELOPE_END "</s:Envelope>"
#define SOAP_BODY_START "<s:Body>"
#define SOAP_BODY_END "</s:Body>"

// Sonos message data:
#define SONOS_ACTION_START_TAG_START "<u:"
#define SONOS_ACTION_START_TAG_NS " xmlns:u=\"urn:"
#define SONOS_ACTION_START_TAG_END "\">"
#define SONOS_ACTION_END_TAG_START "</u:"
#define SONOS_ACTION_END_TAG_END ">"
#define SONOS_ACTION_TAG_LEN 24
#define SONOS_COMMAND "<%s>%s</%s>"
#define SONOS_INSTANCE_ID_0 "<InstanceID>0</InstanceID>"
#define SONOS_SAVED_QUEUES "file:///jffs/settings/savedqueues.rsq#%d"

#define SONOS_URI_META_LIGHT "<CurrentURIMetaData></CurrentURIMetaData>"
#define SONOS_RADIO_META_FULL "<CurrentURIMetaData>&lt;DIDL-Lite xmlns:dc=&quot;http://purl.org/dc/elements/1.1/&quot; xmlns:upnp=&quot;urn:schemas-upnp-org:metadata-1-0/upnp/&quot; xmlns:r=&quot;urn:schemas-rinconnetworks-com:metadata-1-0/&quot; xmlns=&quot;urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/&quot;&gt;&lt;item id=&quot;R:0/0/46&quot; parentID=&quot;R:0/0&quot; restricted=&quot;true&quot;&gt;&lt;dc:title&gt;%s&lt;/dc:title&gt;&lt;upnp:class&gt;object.item.audioItem.audioBroadcast&lt;/upnp:class&gt;&lt;desc id=&quot;cdudn&quot; nameSpace=&quot;urn:schemas-rinconnetworks-com:metadata-1-0/&quot;&gt;SA_RINCON65031_&lt;/desc&gt;&lt;/item&gt;&lt;/DIDL-Lite&gt;</CurrentURIMetaData>"
#define SONOS_PLAYLIST_META_LIGHT "<EnqueuedURIMetaData></EnqueuedURIMetaData><DesiredFirstTrackNumberEnqueued>0</DesiredFirstTrackNumberEnqueued><EnqueueAsNext>1</EnqueueAsNext>"
#define SONOS_PLAYLIST_META_FULL "<EnqueuedURIMetaData>&lt;DIDL-Lite xmlns:dc=&quot;http://purl.org/dc/elements/1.1/&quot; xmlns:upnp=&quot;urn:schemas-upnp-org:metadata-1-0/upnp/&quot; xmlns:r=&quot;urn:schemas-rinconnetworks-com:metadata-1-0/&quot; xmlns=&quot;urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/&quot;&gt;&lt;item id=&quot;SQ:0&quot; parentID=&quot;SQ:&quot; restricted=&quot;true&quot;&gt;&lt;dc:title&gt;%s&lt;/dc:title&gt;&lt;upnp:class&gt;object.container.playlistContainer&lt;/upnp:class&gt;&lt;desc id=&quot;cdudn&quot; nameSpace=&quot;urn:schemas-rinconnetworks-com:metadata-1-0/&quot;&gt;RINCON_AssociatedZPUDN&lt;/desc&gt;&lt;/item&gt;&lt;/DIDL-Lite&gt;</EnqueuedURIMetaData><DesiredFirstTrackNumberEnqueued>0</DesiredFirstTrackNumberEnqueued><EnqueueAsNext>1</EnqueueAsNext>"

#define SONOS_CHANNEL_TEMPLATE "<Channel>%s</Channel>"
#define SONOS_CHANNEL_MASTER "Master"
#define SONOS_CHANNEL_LEFT "LF"
#define SONOS_CHANNEL_RIGHT "RF"

#define SONOS_DIRECTION_BACKWARD 0
#define SONOS_DIRECTION_FORWARD 1

#define SONOS_PLAY_MODE_NORMAL 1
#define SONOS_PLAY_MODE_REPEAT 2
#define SONOS_PLAY_MODE_SHUFFLE_REPEAT 3
#define SONOS_PLAY_MODE_SHUFFLE 4
#define SONOS_PLAY_MODE_NORMAL_VALUE "NORMAL"
#define SONOS_PLAY_MODE_REPEAT_VALUE "REPEAT_ALL"
#define SONOS_PLAY_MODE_SHUFFLE_REPEAT_VALUE "SHUFFLE"
#define SONOS_PLAY_MODE_SHUFFLE_VALUE "SHUFFLE_NOREPEAT"

#define SONOS_SEEK_MODE_TRACK_NR "TRACK_NR"
#define SONOS_SEEK_MODE_REL_TIME "REL_TIME"

#define SONOS_SOURCE_UNKNOWN 0
#define SONOS_SOURCE_FILE 1
#define SONOS_SOURCE_HTTP 2
#define SONOS_SOURCE_RADIO 3
#define SONOS_SOURCE_LINEIN 4
#define SONOS_SOURCE_MASTER 5
#define SONOS_SOURCE_FILE_PREFIX "x-file-cifs:"
#define SONOS_SOURCE_HTTP_PREFIX "x-sonos-http:"
#define SONOS_SOURCE_RADIO_PREFIX "x-rincon-mp3radio:"
#define SONOS_SOURCE_LINEIN_PREFIX "x-rincon-stream:"
#define SONOS_SOURCE_MASTER_PREFIX "x-rincon:"
#define SONOS_SOURCE_QUEUE_PREFIX "x-rincon-queue:"

#define SONOS_STATE_PLAYING 1
#define SONOS_STATE_PAUSED 2
#define SONOS_STATE_STOPPED 3
#define SONOS_STATE_PLAYING_VALUE "PLAYING"
#define SONOS_STATE_PAUSED_VALUE "PAUSED_PLAYBACK"
#define SONOS_STATE_STOPPED_VALUE "STOPPED"


class SonosUPnP {
  public:
    SonosUPnP(EthernetClient client, void (*ethernetErrCallback)(void));

    void setAVTransportURI(IPAddress speakerIP, const char *uri);
    void setAVTransportURI(IPAddress speakerIP, const char *uri, const char *uriMetaData);
    void seekTrack(IPAddress speakerIP, uint16_t index);
    void seekTime(IPAddress speakerIP, uint8_t hour, uint8_t minute, uint8_t second);
    void setPlayMode(IPAddress speakerIP, uint8_t playMode);
    void play(IPAddress speakerIP);
    void playFile(IPAddress speakerIP, const char *uri);
    void playHttp(IPAddress speakerIP, const char *uri);
    void playRadio(IPAddress speakerIP, const char *uri, const char *title);
    void playLineIn(IPAddress speakerIP, const char *speakerID);
    void playQueue(IPAddress speakerIP, const char *speakerID);
    void playConnectToMaster(IPAddress speakerIP, const char *masterSpeakerID);
    void disconnectFromMaster(IPAddress speakerIP);
    void stop(IPAddress speakerIP);
    void pause(IPAddress speakerIP);
    void skip(IPAddress speakerIP, uint8_t direction);
    void setMute(IPAddress speakerIP, bool state);
    void setVolume(IPAddress speakerIP, uint8_t volume);
    void setVolume(IPAddress speakerIP, uint8_t volume, const char *channel);
    void setBass(IPAddress speakerIP, int8_t bass);
    void setTreble(IPAddress speakerIP, int8_t treble);
    void setLoudness(IPAddress speakerIP, bool state);
    void setStatusLight(IPAddress speakerIP, bool state);
    void addPlaylistToQueue(IPAddress speakerIP, uint16_t playlistIndex);
    void removeAllTracksFromQueue(IPAddress speakerIP);
    
    #ifndef SONOS_WRITE_ONLY_MODE
    
    void setRepeat(IPAddress speakerIP, bool repeat);
    void setShuffle(IPAddress speakerIP, bool shuffle);
    void toggleRepeat(IPAddress speakerIP);
    void toggleShuffle(IPAddress speakerIP);
    void togglePause(IPAddress speakerIP);
    void toggleMute(IPAddress speakerIP);
    void toggleLoudness(IPAddress speakerIP);
    uint8_t getState(IPAddress speakerIP);
    uint8_t getPlayMode(IPAddress speakerIP);
    bool getRepeat(IPAddress speakerIP);
    bool getShuffle(IPAddress speakerIP);
    uint16_t getTrackNumber(IPAddress speakerIP);
    void getTrackURI(IPAddress speakerIP, char *resultBuffer, size_t resultBufferSize);
    uint8_t getSource(IPAddress speakerIP);
    uint8_t getSourceFromURI(const char *uri);
    uint32_t getTrackDurationInSeconds(IPAddress speakerIP);
    uint32_t getTrackPositionInSeconds(IPAddress speakerIP);
    uint16_t getTrackPositionPerMille(IPAddress speakerIP);
    bool getMute(IPAddress speakerIP);
    uint8_t getVolume(IPAddress speakerIP);
    uint8_t getVolume(IPAddress speakerIP, const char *channel);
    bool getOutputFixed(IPAddress speakerIP);
    int8_t getBass(IPAddress speakerIP);
    int8_t getTreble(IPAddress speakerIP);
    bool getLoudness(IPAddress speakerIP);
    
    #endif

  private:

    EthernetClient ethClient;
    void (*ethernetErrCallback)(void);
    void seek(IPAddress speakerIP, const char *mode, const char *data);
    void upnpSet(IPAddress ip, uint8_t upnpMessageType, const char *action);
    void upnpSet(IPAddress ip, uint8_t upnpMessageType, const char *action, const char *command, const char *value);
    void upnpSet(IPAddress ip, uint8_t upnpMessageType, const char *action, const char *command, const char *value, const char *extra);
    void upnpPost(IPAddress ip, uint8_t upnpMessageType, const char *action, const char *command, const char *value, const char *extra);
    const char *getUpnpService(uint8_t upnpMessageType);
    const char *getUpnpEndpoint(uint8_t upnpMessageType);
    void waitForResponse();
    uint8_t convertPlayMode(bool repeat, bool shuffle);

    #ifndef SONOS_WRITE_ONLY_MODE

    MicroXPath xPath;
    void upnpAvTransportGet(IPAddress speakerIP, const char *action, const char *path[], size_t pathSize, char *resultBuffer, size_t resultBufferSize);
    void upnpRenderingControlGet(IPAddress speakerIP, const char *action, const char *command, const char *value, const char *path[], size_t pathSize, char *resultBuffer, size_t resultBufferSize);
    uint32_t getTimeInSeconds(const char *time);
    uint8_t convertState(const char *input);
    uint8_t convertPlayMode(const char *input);
    bool convertRepeat(uint8_t playMode);
    bool convertShuffle(uint8_t playMode);

    #endif
};

#endif
