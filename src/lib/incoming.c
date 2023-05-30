/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <guise-client/client.h>
#include <guise-client/incoming.h>
#include <guise-serialize/client_in.h>
#include <guise-serialize/debug.h>
#include <guise-serialize/serialize.h>
#include <flood/in_stream.h>
#include <imprint/allocator.h>


static int onChallengeResponse(GuiseClient* self, FldInStream* inStream)
{
    GuiseSerializeClientNonce clientNonce;
    GuiseSerializeServerChallenge serverChallenge;
    guiseSerializeClientInChallenge(inStream, &clientNonce, &serverChallenge);

    CLOG_C_INFO(&self->log, "got challenge from server %016lX", serverChallenge);

    self->serverChallenge = serverChallenge;
    self->state = GuiseClientStateLogin;

    return 0;
}

static int onLoginResponse(GuiseClient* self, FldInStream* inStream)
{
    GuiseSerializeClientNonce toClientNonce;
    GuiseSerializeUserSessionId userSessionId;

    guiseSerializeClientInLogin(inStream, &toClientNonce, &userSessionId);

    if (toClientNonce != self->nonce) {
        return 0;
    }

    CLOG_C_INFO(&self->log, "Logged in as session %d", userSessionId);

    self->mainUserSessionId = userSessionId;
    self->state = GuiseClientStateLoggedIn;

    return 0;
}


static int guiseClientFeed(GuiseClient* self, const uint8_t* data, size_t len)
{
    FldInStream inStream;
    fldInStreamInit(&inStream, data, len);

    uint8_t cmd;
    fldInStreamReadUInt8(&inStream, &cmd);
    // CLOG_C_VERBOSE(&self->log, "cmd: %s", guiseSerializeCmdToString(cmd));
    switch (data[0]) {
        case guiseSerializeCmdChallengeResponse:
            return onChallengeResponse(self, &inStream);
        case guiseSerializeCmdLoginResponse:
            return onLoginResponse(self, &inStream);
        default:
            CLOG_C_ERROR(&self->log, "unknown message %02X", cmd)
            return -1;
    }
    return 0;
}

int guiseClientReceiveAllInUdpBuffer(GuiseClient* self)
{
#define UDP_MAX_RECEIVE_BUF_SIZE (64000)
    uint8_t receiveBuf[UDP_MAX_RECEIVE_BUF_SIZE];
    size_t count = 0;
    while (1) {
        int octetCount = datagramTransportReceive(&self->transport, receiveBuf, UDP_MAX_RECEIVE_BUF_SIZE);
        if (octetCount > 0) {
            guiseClientFeed(self, receiveBuf, octetCount);
            count++;
        } else if (octetCount < 0) {
            printf("error: %d\n", octetCount);
            return octetCount;
        } else {
            break;
        }
    }

    return count;
}
