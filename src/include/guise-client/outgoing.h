/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef GUISE_CLIENT_OUTGOING_H
#define GUISE_CLIENT_OUTGOING_H

struct GuiseClient;
struct DatagramTransportOut;

#include <monotonic-time/monotonic_time.h>

int guiseClientOutgoing(struct GuiseClient* self, MonotonicTimeMs now, struct DatagramTransportOut* transportOut);

#endif
