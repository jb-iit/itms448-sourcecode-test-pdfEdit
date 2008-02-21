/*
 * PDFedit - free program for PDF document manipulation.
 * Copyright (C) 2006, 2007, 2008  PDFedit team: Michal Hocko,
 *                                              Miroslav Jahoda,
 *                                              Jozef Misutka,
 *                                              Martin Petricek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in doc/LICENSE.GPL); if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 * MA  02111-1307  USA
 *
 * Project is hosted on http://sourceforge.net/projects/pdfedit
 */
#ifndef __QSSTREAM_H__
#define __QSSTREAM_H__

#include <kernel/cobject.h>
#include "qsiproperty.h"
class QString;

namespace gui {

using namespace pdfobjects;

/*= This type of object represents stream in document (in page) */
/** \brief QObject wrapper around CStream */
class QSStream : public QSIProperty {
 Q_OBJECT
public:
 static bool loadFile(const QString &fileName,QByteArray &qb);
 static QByteArray arrayFromBuffer(const CStream::Buffer &b);
 static const CStream::Buffer arrayToBuffer(const QByteArray &a);
 static const CStream::Buffer stringToBuffer(const QString &s);
 static QString stringFromBuffer(const CStream::Buffer &b);
 QSStream(boost::shared_ptr<CStream> _cs,BaseCore *_base);
 virtual ~QSStream();
public slots:
 /*- Sets buffer of this stream from given byte array */
 void setBuffer(const QByteArray &a);
 /*- Sets buffer of this stream from given string (overloaded method) */
 void setBuffer(const QString &s);
 /*- Sets raw buffer of this stream from given byte array */
 void setRawBuffer(const QByteArray &a);
 /*- Sets raw buffer of this stream from given string (overloaded method) */
 void setRawBuffer(const QString &s);
 /*- Loads buffer of this stream from given file. Return true on success, false on failure while loading */
 bool loadBuffer(const QString &fileName);
 /*- Loads raw buffer of this stream from given file. Return true on success, false on failure while loading */
 bool loadRawBuffer(const QString &fileName);
 /*- Gets buffer of this stream as string */
 QString getBufferString();
 /*- Gets buffer of this stream */
 QByteArray getBuffer();
 /*- Saves buffer of this stream to given file. Return true on success, false on failure while saving */
 bool saveBuffer(const QString &fileName);
 /*- Return decoded text representation of this property */
 QString getDecoded();
};

} // namespace gui

#endif
