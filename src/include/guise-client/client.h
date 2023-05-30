/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef GUISE_CLIENT_CLIENT_H
#define GUISE_CLIENT_CLIENT_H

#include <clog/clog.h>
#include <datagram-transport/multi.h>
#include <datagram-transport/transport.h>
#include <guise-client/outgoing_api.h>
#include <guise-serialize/client_out.h>
#include <monotonic-time/monotonic_time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct ImprintAllocator;

struct FldOutStream;

typedef enum GuiseClientState {
    GuiseClientStateIdle,
    GuiseClientStateConnecting,
    GuiseClientStateConnected,
    GuiseClientStateChallenge,
    GuiseClientStateLogin,
    GuiseClientStateLoggedIn,
    GuiseClientStatePlaying,
} GuiseClientState;

#define GUISE_CLIENT_MAX_LOCAL_USERS_COUNT (8)

struct ImprintAllocator;

typedef struct GuiseClient {
    GuiseSerializeUserId userId;
    GuiseSerializePassword password;

    int waitTime;

    uint8_t localPlayerIndex;
    GuiseClientState state;
    GuiseSerializeUserSessionId mainUserSessionId;
    GuiseSerializeClientNonce nonce;
    DatagramTransport transport;
    GuiseSerializeServerChallenge serverChallenge;

    size_t frame;

    struct ImprintAllocator* memory;

    Clog log;
} GuiseClient;

int guiseClientInit(GuiseClient* self, struct ImprintAllocator* memory, DatagramTransport* transport, Clog log);
void guiseClientReset(GuiseClient* self);
void guiseClientReInit(GuiseClient* self, DatagramTransport* transport);
void guiseClientDestroy(GuiseClient* self);
void guiseClientDisconnect(GuiseClient* self);
int guiseClientUpdate(GuiseClient* self, MonotonicTimeMs now);
int guiseClientReJoin(GuiseClient* self);

#endif