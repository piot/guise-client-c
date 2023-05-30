/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <flood/out_stream.h>
#include <guise-client/client.h>
#include <guise-client/debug.h>
#include <guise-client/outgoing.h>
#include <guise-serialize/serialize.h>

static int updateLogin(GuiseClient* self, FldOutStream* stream)
{
    CLOG_C_INFO(&self->log, "serialize login '%d'", self->userId)
    guiseSerializeClientOutLogin(stream, self->nonce, self->userId, self->passwordHashedWithChallenge);
    self->waitTime = 60;

    return 0;
}

static int updateChallenge(GuiseClient* self, FldOutStream* stream)
{
    CLOG_C_INFO(&self->log, "serialize challenge '%d'", self->userId)
    guiseSerializeClientOutChallenge(stream, self->userId, self->nonce);
    self->waitTime = 60;

    return 0;
}

static inline int handleStreamState(GuiseClient* self, FldOutStream* outStream)
{
    switch (self->state) {
        case GuiseClientStateChallenge:
            return updateChallenge(self, outStream);
            break;
        case GuiseClientStateLogin:
            return updateLogin(self, outStream);
            break;

        default:
            CLOG_C_ERROR(&self->log, "Unknown state %d", self->state)
    }
}

static inline int handleState(GuiseClient* self, MonotonicTimeMs now, DatagramTransportOut* transportOut)
{
#define UDP_MAX_SIZE (1200)
    static uint8_t buf[UDP_MAX_SIZE];

    switch (self->state) {
        case GuiseClientStateIdle:
        case GuiseClientStateLoggedIn:
        case GuiseClientStatePlaying:
            return 0;
            break;

        default: {
            FldOutStream outStream;
            fldOutStreamInit(&outStream, buf, UDP_MAX_SIZE);
            int result = handleStreamState(self, &outStream);
            if (result < 0) {
                CLOG_SOFT_ERROR("couldnt send it")
                return result;
            }
            CLOG_C_VERBOSE(&self->log, "sending packet %d octets", outStream.pos)
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
