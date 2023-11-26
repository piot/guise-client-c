/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/guise-client-c
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef GUISE_CLIENT_OUTGOING_API_H
#define GUISE_CLIENT_OUTGOING_API_H

#include <guise-serialize/types.h>
#include <stddef.h>

struct GuiseClient;

int guiseClientLogin(struct GuiseClient* self, GuiseSerializeUserId userId, uint64_t secretPrivatePassword);
int guiseClientReJoin(struct GuiseClient* self);

#endif
