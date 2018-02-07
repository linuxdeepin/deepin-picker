/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *               2011 ~ 2018 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "settings.h"

#include <QDebug>

Settings::Settings(QObject *parent) : QObject(parent)
{
    // Init.
    settings = new QSettings(configPath());
    groupName = "Configure";
}

Settings::~Settings()
{
    delete settings;
}

QString Settings::configPath()
{
    QString path = QString("%1/%2/config.conf").
                   arg(qApp->organizationName()).
                   arg(qApp->applicationName());
    return path;
}

QVariant Settings::getOption(const QString &key, const QVariant &defaultValue)
{
    QVariant result;
    settings->beginGroup(groupName);
    result = settings->value(key, defaultValue);
    settings->endGroup();
    return result;
}

void Settings::setOption(const QString &key, const QVariant &value)
{
    settings->beginGroup(groupName);
    settings->setValue(key, value);
    settings->endGroup();

    settings->sync();
}
