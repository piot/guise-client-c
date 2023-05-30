/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <guise-client/client.h>
#include <guise-client/incoming_api.h>
#include <guise-client/network_realizer.h>
#include <guise-client/outgoing.h>
#include <imprint/allocator.h>

void guiseClientRealizeInit(GuiseClientRealize* self, const GuiseClientRealizeSettings* settings)
{
    self->targetState = GuiseClientRealizeStateInit;
    self->state = GuiseClientRealizeStateInit;
    self->settings = *settings;
    guiseClientInit(&self->client, settings->memory, &self->settings.transport, settings->log);
}

void guiseClientRealizeReInit(GuiseClientRealize* self, const GuiseClientRealizeSettings* settings)
{
    self->targetState = GuiseClientRealizeStateReInit;
    self->state = GuiseClientRealizeStateReInit;
    self->settings = *settings;
    guiseClientReInit(&self->client, &self->settings.transport);
}

void guiseClientRealizeDestroy(GuiseClientRealize* self)
{
    guiseClientDestroy(&self->client);
}

void guiseClientRealizeReset(GuiseClientRealize* self)
{
    // guiseClientReset(&self->client);
    self->state = GuiseClientRealizeStateCleared;
}

static void tryConnectAndLogin(GuiseClientRealize* self)
{
    switch (self->client.state) {
        case GuiseClientStateConnected:
            guiseClientLogin(&self->client, self->settings.userId, &self->settings.password);
            break;
        case GuiseClientStateIdle:
            break;
        case GuiseClientStateConnecting:
            break;
        case GuiseClientStateLogin:
            break;
        case GuiseClientStateLoggedIn:
            break;
    }
}

void guiseClientRealizeUpdate(GuiseClientRealize* self, MonotonicTimeMs now)
{
    if (self->state != GuiseClientRealizeStateCleared && self->targetState != GuiseClientRealizeStateInit) {
        guiseClientUpdate(&self->client, now);
    }

    tryConnectAndLogin(self);

    switch (self->targetState) {
        case GuiseClientRealizeStateCleared:
            if (self->state != GuiseClientRealizeStateCleared) {
                self->state = self->targetState;
            }
            break;
        default:
            break;
    }
}
