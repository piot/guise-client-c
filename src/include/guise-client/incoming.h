/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/guise-client-c
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef GUISE_CLIENT_INCOMING_H
#define GUISE_CLIENT_INCOMING_H

struct GuiseClient;

int guiseClientReceiveAllInUdpBuffer(struct GuiseClient* self);

#endif
