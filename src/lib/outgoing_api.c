/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <guise-client/client.h>

int guiseClientLogin(GuiseClient* self, GuiseSerializeUserId userId, const GuiseSerializePassword* password)
{
    self->userId = userId;
    self->password = *password;
    self->state = GuiseClientStateChallenge;
    self->waitTime = 0;

    return 0;
}
