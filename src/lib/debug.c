/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <guise-client/client.h>
#include <guise-client/debug.h>

#if defined CONFIGURATION_DEBUG

static const char* stateToString(GuiseClientState state)
{
    switch (state) {
        case GuiseClientStateChallenge:
            return "challenge";
        case GuiseClientStateLogin:
            return "login";
        case GuiseClientStateIdle:
            return "idle";
        case GuiseClientStateLoggedIn:
            return "logged in";
        case GuiseClientStatePlaying:
            break;
    }

    return "unknown";
}

#endif

void guiseClientDebugOutput(const GuiseClient* self)
{
    (void) self;

    CLOG_C_INFO(&self->log, "state: %s", stateToString(self->state))
}
