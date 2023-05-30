/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <guise-client/client.h>
#include <guise-client/incoming.h>
#include <guise-client/incoming_api.h>
#include <guise-client/outgoing.h>
#include <secure-random/secure_random.h>

void guiseClientReInit(GuiseClient* self, DatagramTransport* transport)
{
    self->transport = *transport;
    self->state = GuiseClientStateConnected;
}

int guiseClientInit(GuiseClient* self, struct ImprintAllocator* memory, DatagramTransport* transport, Clog log)
{
    self->log = log;
    self->userId = 0;
    self->password.payload[0] = 0;
    self->memory = memory;
    self->state = GuiseClientStateConnected;
    self->transport = *transport;
    self->nonce = secureRandomUInt64();

    return 0;
}

void guiseClientDestroy(GuiseClient* self)
{
}

void guiseClientDisconnect(GuiseClient* self)
{
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

    self->waitTime--;
    if (self->waitTime > 0) {
        return 0;
    }
    sendPackets(self, now);

    return errorCode;
}
