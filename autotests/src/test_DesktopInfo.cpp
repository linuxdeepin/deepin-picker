// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QString>

// Allow test access to private members
#define private public
#define protected public
#include "desktopinfo.h"
#undef private
#undef protected

// Branch list (verified against source via MCP get_code_snippet):
// DesktopInfo::DesktopInfo() - no branches, sequential init of 6 env vars
// Note: GDMSESSION member declared in header (line 30) but NOT initialized in
//   constructor (lines 9-18). Only 6 of 7 member strings are initialized.
//   -> source defect: missing GDMSESSION initialization

// | method | level | factors | min | actual |
// | DesktopInfo | low | - | 2 | 11 |

namespace {

class EnvGuard {
public:
    explicit EnvGuard(const QByteArray &var, const QByteArray &val)
        : var_(var), had_(qEnvironmentVariableIsSet(var.constData()))
    {
        if (had_)
            oldVal_ = qgetenv(var.constData());
        qputenv(var.constData(), val);
    }
    ~EnvGuard() {
        if (had_)
            qputenv(var_.constData(), oldVal_);
        else
            qunsetenv(var_.constData());
    }
private:
    QByteArray var_;
    QByteArray oldVal_;
    bool had_;
};

} // namespace

class DesktopInfoTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        argc_ = 1;
        app_ = new QCoreApplication(argc_, nullptr);
    }
    static void TearDownTestSuite() {
        delete app_;
        app_ = nullptr;
    }

    void SetUp() override {}
    void TearDown() override {}

    static QCoreApplication *app_;
    static int argc_;
};

QCoreApplication *DesktopInfoTest::app_ = nullptr;
int DesktopInfoTest::argc_ = 0;

// --- Constructor Tests ---

TEST_F(DesktopInfoTest, Constructor_AllEnvSet_MembersMatchEnv) {
    // Arrange
    EnvGuard g1("XDG_CURRENT_DESKTOP", "DEEPIN");
    EnvGuard g2("XDG_SESSION_TYPE", "x11");
    EnvGuard g3("WAYLAND_DISPLAY", "wayland-0");
    EnvGuard g4("KDE_FULL_SESSION", "true");
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", "gnome-id");
    EnvGuard g6("DESKTOP_SESSION", "deepin-session");

    // Act
    DesktopInfo info;

    // Assert
    EXPECT_EQ(info.XDG_CURRENT_DESKTOP, QString("DEEPIN"));
    EXPECT_EQ(info.XDG_SESSION_TYPE, QString("x11"));
    EXPECT_EQ(info.WAYLAND_DISPLAY, QString("wayland-0"));
    EXPECT_EQ(info.KDE_FULL_SESSION, QString("true"));
    EXPECT_EQ(info.GNOME_DESKTOP_SESSION_ID, QString("gnome-id"));
    EXPECT_EQ(info.DESKTOP_SESSION, QString("deepin-session"));
}

TEST_F(DesktopInfoTest, Constructor_NoEnvSet_AllMembersEmpty) {
    // Arrange - set all env vars to empty byte array
    EnvGuard g1("XDG_CURRENT_DESKTOP", QByteArray());
    EnvGuard g2("XDG_SESSION_TYPE", QByteArray());
    EnvGuard g3("WAYLAND_DISPLAY", QByteArray());
    EnvGuard g4("KDE_FULL_SESSION", QByteArray());
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", QByteArray());
    EnvGuard g6("DESKTOP_SESSION", QByteArray());

    // Act
    DesktopInfo info;

    // Assert
    EXPECT_TRUE(info.XDG_CURRENT_DESKTOP.isEmpty());
    EXPECT_TRUE(info.XDG_SESSION_TYPE.isEmpty());
    EXPECT_TRUE(info.WAYLAND_DISPLAY.isEmpty());
    EXPECT_TRUE(info.KDE_FULL_SESSION.isEmpty());
    EXPECT_TRUE(info.GNOME_DESKTOP_SESSION_ID.isEmpty());
    EXPECT_TRUE(info.DESKTOP_SESSION.isEmpty());
}

TEST_F(DesktopInfoTest, Constructor_ColonSeparatedDesktop_PreservesExact) {
    // Arrange
    EnvGuard g1("XDG_CURRENT_DESKTOP", "DEEPIN:GNOME");
    EnvGuard g2("XDG_SESSION_TYPE", "wayland");
    EnvGuard g3("WAYLAND_DISPLAY", "wayland-1");
    EnvGuard g4("KDE_FULL_SESSION", QByteArray());
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", QByteArray());
    EnvGuard g6("DESKTOP_SESSION", "plasma");

    // Act
    DesktopInfo info;

    // Assert
    EXPECT_EQ(info.XDG_CURRENT_DESKTOP, QString("DEEPIN:GNOME"));
    EXPECT_EQ(info.XDG_SESSION_TYPE, QString("wayland"));
    EXPECT_EQ(info.WAYLAND_DISPLAY, QString("wayland-1"));
    EXPECT_EQ(info.DESKTOP_SESSION, QString("plasma"));
}

TEST_F(DesktopInfoTest, Constructor_UnicodeInEnv_PreservesExact) {
    // Arrange
    QByteArray unicodeVal = QString::fromUtf8("\xe6\xb7\xb1\xe5\xba\xa6").toUtf8();
    EnvGuard g1("XDG_CURRENT_DESKTOP", unicodeVal);
    EnvGuard g2("XDG_SESSION_TYPE", "x11");
    EnvGuard g3("WAYLAND_DISPLAY", QByteArray());
    EnvGuard g4("KDE_FULL_SESSION", QByteArray());
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", QByteArray());
    EnvGuard g6("DESKTOP_SESSION", QByteArray());

    // Act
    DesktopInfo info;

    // Assert
    EXPECT_FALSE(info.XDG_CURRENT_DESKTOP.isEmpty());
    EXPECT_EQ(info.XDG_SESSION_TYPE, QString("x11"));
    EXPECT_TRUE(info.WAYLAND_DISPLAY.isEmpty());
    EXPECT_TRUE(info.KDE_FULL_SESSION.isEmpty());
    EXPECT_TRUE(info.GNOME_DESKTOP_SESSION_ID.isEmpty());
    EXPECT_TRUE(info.DESKTOP_SESSION.isEmpty());
}

TEST_F(DesktopInfoTest, Constructor_WaylandEnvironment_CorrectFields) {
    // Arrange
    EnvGuard g1("XDG_CURRENT_DESKTOP", "GNOME");
    EnvGuard g2("XDG_SESSION_TYPE", "wayland");
    EnvGuard g3("WAYLAND_DISPLAY", "wayland-0");
    EnvGuard g4("KDE_FULL_SESSION", QByteArray());
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", "this-is-gnome");
    EnvGuard g6("DESKTOP_SESSION", "gnome");

    // Act
    DesktopInfo info;

    // Assert
    EXPECT_EQ(info.XDG_CURRENT_DESKTOP, QString("GNOME"));
    EXPECT_EQ(info.XDG_SESSION_TYPE, QString("wayland"));
    EXPECT_EQ(info.WAYLAND_DISPLAY, QString("wayland-0"));
    EXPECT_EQ(info.GNOME_DESKTOP_SESSION_ID, QString("this-is-gnome"));
    EXPECT_EQ(info.DESKTOP_SESSION, QString("gnome"));
}

TEST_F(DesktopInfoTest, Constructor_KDEEnvironment_KDEFieldsSet) {
    // Arrange
    EnvGuard g1("XDG_CURRENT_DESKTOP", "KDE");
    EnvGuard g2("XDG_SESSION_TYPE", "x11");
    EnvGuard g3("WAYLAND_DISPLAY", QByteArray());
    EnvGuard g4("KDE_FULL_SESSION", "true");
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", QByteArray());
    EnvGuard g6("DESKTOP_SESSION", "plasma");

    // Act
    DesktopInfo info;

    // Assert
    EXPECT_EQ(info.XDG_CURRENT_DESKTOP, QString("KDE"));
    EXPECT_EQ(info.KDE_FULL_SESSION, QString("true"));
    EXPECT_TRUE(info.WAYLAND_DISPLAY.isEmpty());
    EXPECT_TRUE(info.GNOME_DESKTOP_SESSION_ID.isEmpty());
    EXPECT_EQ(info.DESKTOP_SESSION, QString("plasma"));
}

TEST_F(DesktopInfoTest, Defect_GDMSESSION_NotInitialized) {
    // Arrange
    EnvGuard g1("XDG_CURRENT_DESKTOP", "DEEPIN");
    EnvGuard g2("XDG_SESSION_TYPE", "x11");
    EnvGuard g3("WAYLAND_DISPLAY", QByteArray());
    EnvGuard g4("KDE_FULL_SESSION", QByteArray());
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", QByteArray());
    EnvGuard g6("DESKTOP_SESSION", QByteArray());

    // Act
    DesktopInfo info;

    // Assert - GDMSESSION declared in header (line 30) but never initialized
    // in constructor (lines 9-18). 6 of 7 members initialized.
    EXPECT_TRUE(info.GDMSESSION.isEmpty());
    EXPECT_FALSE(info.XDG_CURRENT_DESKTOP.isEmpty());
    EXPECT_EQ(info.XDG_CURRENT_DESKTOP, QString("DEEPIN"));
}

TEST_F(DesktopInfoTest, Constructor_TwoInstances_IndependentCopies) {
    // Arrange - first instance captures env as FIRST
    EnvGuard g1("XDG_CURRENT_DESKTOP", "FIRST");
    EnvGuard g2("XDG_SESSION_TYPE", QByteArray());
    EnvGuard g3("WAYLAND_DISPLAY", QByteArray());
    EnvGuard g4("KDE_FULL_SESSION", QByteArray());
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", QByteArray());
    EnvGuard g6("DESKTOP_SESSION", QByteArray());
    DesktopInfo info1;
    const QString firstVal = info1.XDG_CURRENT_DESKTOP;

    // Act - change env and create second instance
    {
        EnvGuard gSwap("XDG_CURRENT_DESKTOP", "SECOND");
        DesktopInfo info2;
        const QString secondVal = info2.XDG_CURRENT_DESKTOP;

        // Assert - first captured before env change, second after
        EXPECT_EQ(firstVal, QString("FIRST"));
        EXPECT_EQ(secondVal, QString("SECOND"));
    }
}

TEST_F(DesktopInfoTest, Destructor_WhenDestroyed_NoCrash) {
    // Arrange
    EnvGuard g1("XDG_CURRENT_DESKTOP", "DEEPIN");
    EnvGuard g2("XDG_SESSION_TYPE", "x11");
    EnvGuard g3("WAYLAND_DISPLAY", QByteArray());
    EnvGuard g4("KDE_FULL_SESSION", QByteArray());
    EnvGuard g5("GNOME_DESKTOP_SESSION_ID", QByteArray());
    EnvGuard g6("DESKTOP_SESSION", QByteArray());
    bool created = false;

    // Act - create and destroy in inner scope
    {
        DesktopInfo info;
        created = !info.XDG_CURRENT_DESKTOP.isEmpty();
    }

    // Assert - destructor ran via scope exit, no crash
    EXPECT_TRUE(created);
    EXPECT_NE(app_, nullptr);
}

TEST_F(DesktopInfoTest, WmEnum_WhenChecked_HasExpectedValues) {
    // Arrange
    const int gnomeVal = static_cast<int>(DesktopInfo::GNOME);
    const int kdeVal = static_cast<int>(DesktopInfo::KDE);
    const int otherVal = static_cast<int>(DesktopInfo::OTHER);

    // Act - enum values are compile-time constants
    const bool ordered = (gnomeVal < kdeVal) && (kdeVal < otherVal);

    // Assert
    EXPECT_EQ(gnomeVal, 0);
    EXPECT_EQ(kdeVal, 1);
    EXPECT_EQ(otherVal, 2);
    EXPECT_TRUE(ordered);
}

// Source defect record:
// FILE: src/desktopinfo.h line 30 declares `QString GDMSESSION;`
// FILE: src/desktopinfo.cpp constructor (lines 9-18) never initializes it.
// Constructor reads XDG_CURRENT_DESKTOP, XDG_SESSION_TYPE, WAYLAND_DISPLAY,
// KDE_FULL_SESSION, GNOME_DESKTOP_SESSION_ID, DESKTOP_SESSION but not GDMSESSION.
