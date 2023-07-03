/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <datagram-transport/types.h>
#include <flood/out_stream.h>
#include <guise-client/client.h>
#include <guise-client/debug.h>
#include <guise-client/outgoing.h>
#include <guise-serialize/serialize.h>
#include <inttypes.h>

static int updateLogin(GuiseClient* self, FldOutStream* stream)
{
    CLOG_C_DEBUG(&self->log, "serialize login %" PRIx64, self->userId)
    guiseSerializeClientOutLogin(stream, self->nonce, self->userId, self->passwordHashedWithChallenge);
    self->waitTime = 60;

    return 0;
}

static int updateChallenge(GuiseClient* self, FldOutStream* stream)
{
    CLOG_C_DEBUG(&self->log, "serialize challenge '" PRIx64, self->userId)
    guiseSerializeClientOutChallenge(stream, self->userId, self->nonce);
    self->waitTime = 60;

    return 0;
}

static inline int handleStreamState(GuiseClient* self, FldOutStream* outStream)
{
    switch (self->state) {
        case GuiseClientStateChallenge:
            return updateChallenge(self, outStream);
        case GuiseClientStateLogin:
            return updateLogin(self, outStream);
        case GuiseClientStateIdle:
        case GuiseClientStateLoggedIn:
        case GuiseClientStatePlaying:
            return 0;
    }
}

static inline int handleState(GuiseClient* self, MonotonicTimeMs now, DatagramTransportOut* transportOut)
{
    (void) now;

    static uint8_t buf[DATAGRAM_TRANSPORT_MAX_SIZE];

    switch (self->state) {
        case GuiseClientStateIdle:
        case GuiseClientStateLoggedIn:
        case GuiseClientStatePlaying:
            return 0;

        case GuiseClientStateChallenge:
        case GuiseClientStateLogin: {
            FldOutStream outStream;
            fldOutStreamInit(&outStream, buf, DATAGRAM_TRANSPORT_MAX_SIZE);
            int result = handleStreamState(self, &outStream);
            if (result < 0) {
                CLOG_SOFT_ERROR("couldnt send it")
                return result;
            }
            CLOG_C_VERBOSE(&self->log, "sending packet %zu octets", outStream.pos)
            return transportOut->send(transportOut->self, outStream.octets, outStream.pos);
        }
    }
}

int guiseClientOutgoing(GuiseClient* self, MonotonicTimeMs now, DatagramTransportOut* transportOut)
{
    if (self->state != GuiseClientStatePlaying) {
        guiseClientDebugOutput(self);
    }

    int result = handleState(self, now, transportOut);
    if (result < 0) {
        return result;
    }

    self->frame++;

    return 0;
}
