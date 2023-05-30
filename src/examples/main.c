/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/console.h>
#include <flood/in_stream.h>
#include <flood/out_stream.h>
#include <flood/text_in_stream.h>
#include <guise-client/client.h>
#include <guise-client/network_realizer.h>
#include <guise-serialize/debug.h>
#include <guise-serialize/parse_text.h>
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

typedef struct Secret {
    GuiseSerializeUserId userId;
    uint64_t passwordHash;
} Secret;

static int readSecret(Secret* secret)
{
    CLOG_DEBUG("reading secret file")
    FILE* fp = fopen("secret.txt", "r");
    if (fp == 0) {
        CLOG_ERROR("could not find secret.txt")
        return -4;
    }

    size_t usersRead = 0;
    char line[1024];
    char* ptr = fgets(line, 1024, fp);
    if (ptr == 0) {
        return -39;
    }
    fclose(fp);

    FldTextInStream textInStream;
    FldInStream inStream;

    fldInStreamInit(&inStream, line, tc_strlen(line));
    fldTextInStreamInit(&textInStream, &inStream);

    int err = guiseTextStreamReadUserId(&textInStream, &secret->userId);
    if (err < 0) {
        return err;
    }
    char name[33];

    err = guiseTextStreamReadUserName(&textInStream, name, 33);
    if (err >= 0) {
        CLOG_VERBOSE("found username '%s'", name)
    }

    err = guiseTextStreamReadPasswordHash(&textInStream, &secret->passwordHash);
    if (err < 0) {
        return err;
    }

    return 0;
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
    udpClientInit(&udpClientSocket, hostToConnectTo, 27004);

    transportInOut.self = &udpClientSocket;
    transportInOut.receive = clientReceive;
    transportInOut.send = clientSend;

    Secret secret;
    readSecret(&secret);

    settings.memory = &memory.tagAllocator.info;
    settings.transport = transportInOut;
    settings.userId = secret.userId;
    settings.secretPasswordHash = secret.passwordHash;
    Clog guiseClientLog;
    guiseClientLog.config = &g_clog;
    guiseClientLog.constantPrefix = "GuiseClient";
    settings.log = guiseClientLog;

    guiseClientRealizeInit(&clientRealize, &settings);
    guiseClientRealizeReInit(&clientRealize, &settings);

    clientRealize.state = GuiseClientStateChallenge;
    clientRealize.targetState = GuiseClientStateLoggedIn;

    GuiseClientRealizeState reportedState;
    reportedState = GuiseClientRealizeStateInit;

    while (true) {
        usleep(16 * 1000);

        MonotonicTimeMs now = monotonicTimeMsNow();
        guiseClientRealizeUpdate(&clientRealize, now);

        if (reportedState != clientRealize.state) {
            reportedState = clientRealize.state;
        }
    }

    imprintDefaultSetupDestroy(&memory);
}
