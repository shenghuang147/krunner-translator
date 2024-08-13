/******************************************************************************
 *  Copyright (C) 2013 – 2018 by David Baum <david.baum@naraesk.eu>           *
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

#ifndef TRANSLATORCONFIG_H
#define TRANSLATORCONFIG_H

#include "ui_translator_config.h"
#include <KCModule>
#include <src/LanguageRepository.h>

static const char CONFIG_PRIMARY[] = "primaryLanguage";
static const char CONFIG_SECONDARY[] = "secondaryLanguage";

// baidu
static const char CONFIG_BAIDU_APPID[] = "baiduAPPID";
static const char CONFIG_BAIDU_APIKEY[] = "baiduAPIKey";
static const char CONFIG_BAIDU_ENABLE[] = "baiduEnable";

// youdao
static const char CONFIG_YOUDAO_APPID[] = "youdaoAPPID";
static const char CONFIG_YOUDAO_APPSEC[] = "youdaoAPPSec";
static const char CONFIG_YOUDAO_ENABLE[] = "youdaoEnable";

// deepl
static const char CONFIG_DEEPL_APIKEY[] = "deeplAPIKey";
static const char CONFIG_DEEPL_ENABLE[] = "deeplEnable";

// google
static const char CONFIG_GOOGLE_ENABLE[] = "googleEnable";

// bing
static const char CONFIG_BING_ENABLE[] = "bingEnable";


class TranslatorConfigForm : public QWidget, public Ui::TranslatorConfigUi {
Q_OBJECT

public:
    explicit TranslatorConfigForm(QWidget *parent);
};

class TranslatorConfig : public KCModule {
Q_OBJECT

public:
    explicit TranslatorConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());

public Q_SLOTS:

    void save() override;

    void load() override;

    void warningHandler();

private:
    TranslatorConfigForm *m_ui;
    LanguageRepository languages;
};

#endif
