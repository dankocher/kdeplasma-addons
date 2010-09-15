/***************************************************************************
 *   Copyright (C) 2009 by Artur Duque de Souza <morpheuz@gmail.com>       *
 *                         Vardhman Jain <vardhman@gmail.com>              *
 *                         Gilles Caulier <caulier.gilles@gmail.com>       *
 *                         Michał Ziąbkowski <mziab@o2.pl>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "imageshack.h"

#include <QFile>
#include <QString>
#include <QXmlStreamReader>

#include <kmimetype.h>
#include <kurl.h>
#include <krandom.h>
#include <kapplication.h>
#include <KDebug>

ImageshackServer::ImageshackServer(const QString &server)
{
    if (server.isEmpty()) {
        m_server = IMAGESHACK_SERVER;
    } else {
        m_server = server;
    }

    m_boundary  = "----------";
    m_boundary += KRandom::randomString(42 + 13).toAscii();
}

ImageshackServer::~ImageshackServer()
{
}

void ImageshackServer::finished(KJob *job)
{
    QXmlStreamReader xml(_data);
    if (xml.hasError()) {
        emit postError();
        return;
    }

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.name() == "ad_link") {
            QString url = xml.readElementText();
            emit postFinished(url);
            return;
        }
    }
}

void ImageshackServer::readKIOData(KIO::Job *job, const QByteArray &data)
{
    Q_UNUSED(job);

    if (data.length() == 0) {
        return;
    }

    _data.append(data);
}

// taken from flickr KIPI Plugin
void ImageshackServer::finish()
{
    QByteArray str;
    str += "--";
    str += m_boundary;
    str += "--";

    m_buffer.append(str);
}

// taken from flickr KIPI Plugin
bool ImageshackServer::addFile(const QString& name, const QString& path)
{
    KMimeType::Ptr ptr = KMimeType::findByUrl(path);
    QString mime = ptr->name();
    if (mime.isEmpty()) {
        // if we ourselves can't determine the mime of the local file,
        // very unlikely the remote site will be able to identify it
        return false;
    }

    QFile imageFile(path);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray imageData = imageFile.readAll();
    imageFile.close();

    QByteArray str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.toAscii();
    str += "\"; ";
    str += "filename=\"";
    str += QFile::encodeName(KUrl(path).fileName()).replace(".tmp", ".jpg");
    str += "\"; ";
    str += "key=\"";
    str += IMAGESHACK_KEY;
    str += "\"";
    str += "\r\n";
    str += "Content-Type: ";
    str +=  mime.toAscii();
    str += "\r\n\r\n";

    m_buffer.append(str);

    int oldSize = m_buffer.size();
    m_buffer.resize(oldSize + imageData.size() + 2);
    memcpy(m_buffer.data() + oldSize, imageData.data(), imageData.size());
    m_buffer[m_buffer.size()-2] = '\r';
    m_buffer[m_buffer.size()-1] = '\n';

    return true;
}


void ImageshackServer::post(const QString& content)
{
    KUrl url(QString("%1/upload_api.php").arg(m_server));
    addFile("fileupload", content);

    finish();
    _data.clear();

    KIO::TransferJob *tf = KIO::http_post(url, m_buffer, KIO::HideProgressInfo);

    tf->addMetaData("content-type","Content-Type: multipart/form-data; boundary=" + m_boundary);

    connect(tf, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(readKIOData(KIO::Job*, const QByteArray&)));
    connect(tf, SIGNAL(result(KJob *)), this, SLOT(finished(KJob *)));
}