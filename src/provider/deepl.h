/******************************************************************************
 *  Copyright (C) 2024 by Sheng Huang <shenghuang147@gmail.com>               *
 *                                                                            *
 *  This library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as published  *
 *  by the Free Software Foundation; either version 2 of the License or (at   *
 *  your option) any later version.                                           *
 *                                                                            *
 *  This library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  Library General Public License for more details.                          *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with this library; see the file COPYING.LIB.                        *
 *  If not, see <http://www.gnu.org/licenses/>.                               *
 *****************************************************************************/

#ifndef DEEPL_H
#define DEEPL_H

#include <KRunner/AbstractRunner>
#include <QtNetwork/QNetworkReply>

/**
 * API Implementation for Deepl https://developers.deepl.com/docs/api-reference/translate/openapi-spec-for-text-translation)
 */

class Deepl : public QObject
{

    Q_OBJECT

public:
    Deepl(Plasma::AbstractRunner*, Plasma::RunnerContext&, const QString &, const QPair<QString, QString> &, const QString &);

private Q_SLOTS:
   void parseResult(QNetworkReply*);

Q_SIGNALS:
	void finished();
   
private:
   Plasma::AbstractRunner * m_runner;
   QNetworkAccessManager * m_manager;
   Plasma::RunnerContext m_context;
   QString langMapper(QString);
};


#endif