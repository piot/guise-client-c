/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <guise-client/client.h>
#include <guise-client/incoming.h>
#include <guise-client/outgoing.h>
#include <secure-random/secure_random.h>

int guiseClientInit(GuiseClient* self, struct ImprintAllocator* memory, Clog log)
{
    self->log = log;
    self->userId = 0;
    self->secretPrivatePassword = 0;
    self->memory = memory;
    self->state = GuiseClientStateIdle;
    self->nonce = secureRandomUInt64();
    self->waitTime = 0;

    return 0;
}

int guiseClientReInit(GuiseClient* self, struct DatagramTransport* transport, GuiseSerializeUserId userId,
                      uint64_t secretPrivatePassword)
{
    self->userId = userId;
    self->secretPrivatePassword = secretPrivatePassword;
    self->state = GuiseClientStateChallenge;
    self->waitTime = 0;
    self->transport = *transport;
    self->nonce = secureRandomUInt64();

    return 0;
}

void guiseClientDestroy(GuiseClient* self)
{
    (void) self;
}

void guiseClientDisconnect(GuiseClient* self)
{
    (void) self;
}

static int sendPackets(GuiseClient* self, MonotonicTimeMs now)
{
    DatagramTransportOut transportOut;
    transportOut.self = self->transport.self;
    transportOut.send = self->transport.send;

    int errorCode = guiseClientOutgoing(self, now, &transportOut);
    if (errorCode < 0) {
        return errorCode;
    }

    return 0;
}

int guiseClientUpdate(GuiseClient* self, MonotonicTimeMs now)
{
    int errorCode;

    errorCode = guiseClientReceiveAllInUdpBuffer(self);
    if (errorCode < 0) {
        return errorCode;
    }

    return guiseClientUpdateOut(self, now);
}

int guiseClientUpdateOut(GuiseClient* self, MonotonicTimeMs now)
{
    self->waitTime--;
    if (self->waitTime > 0) {
        return 0;
    }
    return sendPackets(self, now);
}
