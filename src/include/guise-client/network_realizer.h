/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef GUISE_CLIENT_NETWORK_REALIZE_H
#define GUISE_CLIENT_NETWORK_REALIZE_H

#include <guise-client/client.h>
#include <guise-client/network_realizer.h>
#include <guise-client/types.h>
#include <guise-serialize/types.h>
#include <stddef.h>

struct ImprintAllocator;

typedef enum GuiseClientRealizeState {
    GuiseClientRealizeStateInit,
    GuiseClientRealizeStateReInit,
    GuiseClientRealizeStateCleared,
    GuiseClientRealizeStateLogin,
} GuiseClientRealizeState;

typedef struct GuiseClientRealizeSettings {
    DatagramTransport transport;
    GuiseSerializeUserId userId;
    uint64_t secretPasswordHash;
    struct ImprintAllocator* memory;
    Clog log;
} GuiseClientRealizeSettings;

typedef struct GuiseClientRealize {
    GuiseClientRealizeState targetState;
    GuiseClientRealizeState state;
    GuiseClient client;
    GuiseClientRealizeSettings settings;
} GuiseClientRealize;

void guiseClientRealizeInit(GuiseClientRealize* self, const GuiseClientRealizeSettings* settings);
void guiseClientRealizeReInit(GuiseClientRealize* self, const GuiseClientRealizeSettings* settings);
void guiseClientRealizeDestroy(GuiseClientRealize* self);
void guiseClientRealizeReset(GuiseClientRealize* self);
void guiseClientRealizeUpdate(GuiseClientRealize* self, MonotonicTimeMs now);
void guiseClientRealizeUpdateOut(GuiseClientRealize* self, MonotonicTimeMs now);

#endif
