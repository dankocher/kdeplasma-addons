/*
 *   Copyright 2009 Artur Duque de Souza <asouza@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PASTEBIN_ENGINE_H
#define PASTEBIN_ENGINE_H

#include <Plasma/DataEngine>

class PastebinEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    PastebinEngine(QObject *parent, const QVariantList &args);
    void init();

    Plasma::Service *serviceForSource(const QString &source);
    friend class PastebinService;
};

#endif // PASTEBIN_ENGINE