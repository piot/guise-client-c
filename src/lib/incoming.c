/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <datagram-transport/types.h>
#include <flood/in_stream.h>
#include <guise-client/client.h>
#include <guise-client/incoming.h>
#include <guise-serialize/client_in.h>
#include <imprint/allocator.h>
#include <inttypes.h>

static int onChallengeResponse(GuiseClient* self, FldInStream* inStream)
{
    GuiseSerializeClientNonce clientNonce;
    GuiseSerializeServerChallenge serverChallenge;
    guiseSerializeClientInChallenge(inStream, &clientNonce, &serverChallenge);

    self->passwordHashedWithChallenge = serverChallenge ^ self->secretPrivatePassword;

    CLOG_C_INFO(&self->log, "got challenge from server %" PRIx64, serverChallenge)

    self->serverChallenge = serverChallenge;
    self->state = GuiseClientStateLogin;

    return 0;
}

static int onLoginResponse(GuiseClient* self, FldInStream* inStream)
{
    GuiseSerializeClientNonce toClientNonce;
    GuiseSerializeUserSessionId userSessionId;
    GuiseSerializeUserName userName;

    guiseSerializeClientInLogin(inStream, &toClientNonce, &userName, &userSessionId);

    if (toClientNonce != self->nonce) {
        return 0;
    }

    CLOG_C_INFO(&self->log, "Logged in as '%s' session %" PRIx64, userName.utf8, userSessionId)

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
            // return -1;
    }
}

int guiseClientReceiveAllInUdpBuffer(GuiseClient* self)
{
    uint8_t receiveBuf[DATAGRAM_TRANSPORT_MAX_SIZE];
    size_t count = 0;
    while (1) {
        ssize_t octetCount = datagramTransportReceive(&self->transport, receiveBuf, DATAGRAM_TRANSPORT_MAX_SIZE);
        if (octetCount > 0) {
            guiseClientFeed(self, receiveBuf, (size_t) octetCount);
            count++;
        } else if (octetCount < 0) {
            CLOG_C_INFO(&self->log, "datagramTransportReceive error: %zd", octetCount)
            return (int) octetCount;
        } else {
            break;
        }
    }

    return (int) count;
}
