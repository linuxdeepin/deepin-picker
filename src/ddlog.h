// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DDLOG_H
#define DDLOG_H

#include <QDebug>
#include <QLoggingCategory>

// 全项目统一的日志分类，便于 DConfig 调试模式（deepin-debug-config）按应用统一过滤。
// deepin-picker 为单进程应用，仅一个分类；如需子分类，按 org.deepin.picker.<子模块> 命名。
inline Q_LOGGING_CATEGORY(appLog, "org.deepin.picker")

#endif // DDLOG_H
