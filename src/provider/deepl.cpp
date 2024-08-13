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

#include "deepl.h"

#include <QtGlobal>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QDebug>

Deepl::Deepl(Plasma::AbstractRunner *runner, Plasma::RunnerContext &context, const QString &text,
             const QPair<QString, QString> &language, const QString &key)
        : m_runner(runner), m_context(context) {
    m_manager = new QNetworkAccessManager(this);

    QUrlQuery postData;
    postData.addQueryItem(QStringLiteral("text"), text);                           
    postData.addQueryItem(QStringLiteral("source_lang"), langMapper(language.first));
    postData.addQueryItem(QStringLiteral("target_lang"), langMapper(language.second));

    QNetworkRequest request;                                                       
    request.setUrl(QUrl("https://api-free.deepl.com/v2/translate"));
    request.setRawHeader(QByteArrayLiteral("Authorization"), "DeepL-Auth-Key " + key.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

    m_manager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());   
    connect(m_manager, &QNetworkAccessManager::finished, this, &Deepl::parseResult);
}

void Deepl::parseResult(QNetworkReply *reply) {
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
        emit finished();
        return;
    }

    const QString s = QString::fromUtf8(reply->readAll());
    const QJsonObject jsonObject = QJsonDocument::fromJson(s.toUtf8()).object();
    if (jsonObject.contains(QStringLiteral("message"))) {
        Plasma::QueryMatch match(m_runner);
        match.setType(Plasma::QueryMatch::HelperMatch);
        match.setIcon(QIcon::fromTheme(QStringLiteral("dialog-error")));
        match.setText(
                QString::fromUtf8("(Deepl) Error Message: %1").arg(jsonObject.value("message").toString()));
        match.setRelevance(1);
        m_context.addMatch(match);
    } else {
        QList<Plasma::QueryMatch> matches;
        const QJsonArray results = jsonObject.find("translations").value().toArray();
        float relevance = 1;
        for (const QJsonValue result: results) {
            Plasma::QueryMatch match(m_runner);
            match.setType(Plasma::QueryMatch::InformationalMatch);
            match.setIcon(QIcon::fromTheme("applications-education-language"));
            match.setText(result.toObject().value("text").toString());
            match.setMultiLine(true);
            match.setSubtext(QStringLiteral("Deepl Translate"));
            match.setRelevance(relevance);
            matches.append(match);
            relevance -= 0.01;
        }
        m_context.addMatches(matches);
    }
    emit finished();
}

QString Deepl::langMapper(QString lang) {
    return lang;
}

#include "moc_deepl.cpp"
