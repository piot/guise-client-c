/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/console.h>
#include <flood/out_stream.h>
#include <guise-client/client.h>
#include <guise-client/network_realizer.h>
#include <guise-serialize/debug.h>
#include <imprint/default_setup.h>
#include <stdio.h>
#include <udp-client/udp_client.h>
#include <unistd.h>

clog_config g_clog;

static ssize_t clientReceive(void* _self, uint8_t* data, size_t size)
{
    UdpClientSocket* self = _self;

    return udpClientReceive(self, data, size);
}

static int clientSend(void* _self, const uint8_t* data, size_t size)
{
    UdpClientSocket* self = _self;

    return udpClientSend(self, data, size);
}

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    g_clog.log = clog_console;
    g_clog.level = CLOG_TYPE_VERBOSE;

    CLOG_VERBOSE("example start")
    CLOG_VERBOSE("initialized")

    FldOutStream outStream;

    uint8_t buf[1024];
    fldOutStreamInit(&outStream, buf, 1024);

    GuiseClientRealize clientRealize;
    GuiseClientRealizeSettings settings;

    ImprintDefaultSetup memory;

    DatagramTransport transportInOut;

    imprintDefaultSetupInit(&memory, 16 * 1024 * 1024);

    int startupErr = udpClientStartup();
    if (startupErr < 0) {
        return startupErr;
    }

    const char* hostToConnectTo = "127.0.0.1";

    if (argc > 1) {
        hostToConnectTo = argv[1];
    }

    UdpClientSocket udpClientSocket;
    udpClientInit(&udpClientSocket, hostToConnectTo, 27003);

    transportInOut.self = &udpClientSocket;
    transportInOut.receive = clientReceive;
    transportInOut.send = clientSend;

    settings.memory = &memory.tagAllocator.info;
    settings.transport = transportInOut;
    settings.userId = 1334;
    Clog guiseClientLog;
    guiseClientLog.config = &g_clog;
    guiseClientLog.constantPrefix = "GuiseClient";
    settings.log = guiseClientLog;

    guiseClientRealizeInit(&clientRealize, &settings);
    guiseClientRealizeReInit(&clientRealize, &settings);

    GuiseClientRealizeState reportedState;
    reportedState = GuiseClientRealizeStateInit;
    uint16_t packetCount = 0;
    while (true) {
        usleep(16 * 1000);

        MonotonicTimeMs now = monotonicTimeMsNow();
        guiseClientRealizeUpdate(&clientRealize, now);

        if (reportedState != clientRealize.state) {
            reportedState = clientRealize.state;
            switch (clientRealize.state) {
                case GuiseClientRealizeStateCreateRoom:
                    CLOG_NOTICE("CREATED ROOM!")
                    break;
                case GuiseClientRealizeStateJoinRoom:
                    CLOG_NOTICE("Joined ROOM!")
                    break;
            }
        }
    }

    imprintDefaultSetupDestroy(&memory);
}
