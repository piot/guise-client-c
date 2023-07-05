/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <guise-client/client.h>
#include <guise-client/network_realizer.h>
#include <imprint/allocator.h>

void guiseClientRealizeInit(GuiseClientRealize* self, const GuiseClientRealizeSettings* settings)
{
    self->targetState = GuiseClientRealizeStateLogin;
    self->state = GuiseClientRealizeStateLogin;
    self->settings = *settings;
    guiseClientInit(&self->client, settings->memory, &self->settings.transport, settings->log);
    guiseClientLogin(&self->client, self->settings.userId, self->settings.secretPasswordHash);
}

void guiseClientRealizeReInit(GuiseClientRealize* self, const GuiseClientRealizeSettings* settings)
{
    self->targetState = GuiseClientRealizeStateLogin;
    self->state = GuiseClientRealizeStateLogin;
    self->settings = *settings;
    guiseClientReInit(&self->client, &self->settings.transport);
    guiseClientLogin(&self->client, self->settings.userId, self->settings.secretPasswordHash);
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

void guiseClientRealizeUpdate(GuiseClientRealize* self, MonotonicTimeMs now)
{
    guiseClientRealizeUpdateOut(self, now);
}

void guiseClientRealizeUpdateOut(GuiseClientRealize* self, MonotonicTimeMs now)
{
    if (self->state != GuiseClientRealizeStateCleared && self->targetState != GuiseClientRealizeStateInit) {
        guiseClientUpdateOut(&self->client, now);
    }

    switch (self->targetState) {
        case GuiseClientRealizeStateCleared:
            if (self->state != GuiseClientRealizeStateCleared) {
                self->state = self->targetState;
            }
            break;
        case GuiseClientRealizeStateLogin:
        case GuiseClientRealizeStateInit:
        case GuiseClientRealizeStateReInit:
            break;
    }
}
