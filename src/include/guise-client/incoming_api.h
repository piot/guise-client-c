/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/guise-client-c
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef GUISE_CLIENT_INCOMING_API_H
#define GUISE_CLIENT_INCOMING_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct GuiseClient;

int guiseClientInReadPacket(struct GuiseClient* self, int* connectionId, uint8_t* octets, size_t octetCount);

#endif
