/***************
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
***************/
#include "SessionCreator.h"




SessionCreator::SessionCreator(QObject *parent) : QObject(parent)
{
    client = nullptr;
}

void SessionCreator::clear()
{
    if (client) {
        disconnect(client, 0, this, 0);
        client = nullptr;
    }
}

void SessionCreator::createSession(CortexClient* client,
                                   const QString &headsetId,
                                   const QString &license)
{
    clear();
    if (client == nullptr) return;
    if (!client->isConnected()) return;
    this->client = client;
    this->headsetId = headsetId;
    this->license = license;
    connect(client, &CortexClient::createSession, this, &SessionCreator::onCreateSession);
    bool activate = ! license.isEmpty();
    client->createSession(headsetId, activate);
}



void SessionCreator::onCreateSession(const QString &sessionId)
{
    qInfo() << "Session created, session id " << sessionId;
    emit sessionCreated(sessionId);
}
