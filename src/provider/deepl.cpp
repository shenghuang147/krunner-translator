/******************************************************************************
 *  Copyright (C) 2025 by SHENG HUANG <shenghuang147@gmail.com>               *
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

#include <klocalizedstring.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QUrlQuery>

Deepl::Deepl(KRunner::AbstractRunner *runner, KRunner::RunnerContext &context, const QString &text,
               const QPair<QString, QString> &language, const QString &authKey)
        : m_runner(runner), m_context(context) {
    m_manager = new QNetworkAccessManager(this);

    QUrlQuery postData;
    postData.addQueryItem(QStringLiteral("text"), text);
    postData.addQueryItem(QStringLiteral("source_lang"), langMapper(language.first));
    postData.addQueryItem(QStringLiteral("target_lang"), langMapper(language.second));

    QString urlString;
    if (authKey.endsWith(QStringLiteral(":fx"))) {
        // deepl free
        urlString = QStringLiteral("https://api-free.deepl.com/v2/translate");
    } else {
        // deepl pro
        urlString = QStringLiteral("https://api.deepl.com/v2/translate");
    }

    QNetworkRequest request;
    request.setUrl(QUrl(urlString));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    request.setRawHeader(QByteArrayLiteral("Authorization"),
                        QByteArrayLiteral("DeepL-Auth-Key ") + authKey.toUtf8());

    m_manager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(m_manager, &QNetworkAccessManager::finished, this, &Deepl::parseResult);
}

void Deepl::parseResult(QNetworkReply *reply) {
    reply->deleteLater();
    
    const QByteArray data = reply->readAll();
    if (data.isEmpty()) {
        Q_EMIT finished();
        return;
    }
    
    QJsonParseError parseError;
    const QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        Q_EMIT finished();
        return;
    }
    const QJsonObject jsonObject = jsonDocument.object();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    KRunner::QueryMatch match(m_runner);
    match.setMultiLine(true);
    match.setRelevance(1);
    match.setSubtext(QStringLiteral("Deepl Translate"));

    switch (statusCode) {
        case 200: {
            /**
            *   {
            *       "translations":[
            *           {
            *               "detected_source_language":"EN",
            *               "text":"你好，这是一个测试"
            *           }
            *       ]
            *   }
            */
            const QJsonArray translations = jsonObject[QStringLiteral("translations")].toArray();
            if (translations.isEmpty()) {
                Q_EMIT finished();
                return;
            }

            const QString text = translations[0][QStringLiteral("text")].toString();
            if (text.isEmpty()) {
                Q_EMIT finished();
                return;
            }

            match.setIconName(QStringLiteral("applications-education-language"));
            match.setText(text);
            m_context.addMatch(match);
            Q_EMIT finished();
            return;
        }

        default: {
            QString errorMessage = jsonObject[QStringLiteral("message")].toString();
            if (errorMessage.isEmpty()) {
                errorMessage = QString(QStringLiteral("Unknown error (HTTP %1)")).arg(statusCode);
            }
            match.setIconName(QStringLiteral("dialog-error"));
            match.setText(QStringLiteral("(Deepl) Error: ") + errorMessage);
            m_context.addMatch(match);
            Q_EMIT finished();
            return;
        }
    }

}

QString Deepl::langMapper(QString lang) {
    return lang;
}

#include "moc_deepl.cpp"
