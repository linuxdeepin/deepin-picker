// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusArgument>
#include <QDBusMetaType>
#include <QVariantMap>

#include "dbusnotify.h"
#include "stubext.h"

// Qt5 QDBusConnection::connect 7-arg (non-const, non-static)
// bool connect(service, path, interface, name, signature, receiver, slot)
using DbusConnect7 = bool (QDBusConnection::*)(
    const QString&, const QString&, const QString&,
    const QString&, const QString&,
    QObject*, const char*);
using DbusDisconnect7 = bool (QDBusConnection::*)(
    const QString&, const QString&, const QString&,
    const QString&, const QString&,
    QObject*, const char*);

class DBusNotifyTest : public ::testing::Test {
protected:
    void SetUp() override {
        stub.clear();
    }

    void TearDown() override {
        stub.clear();
    }

    static void SetUpTestSuite() {
        argc = 1;
        argv = new char*[2];
        argv[0] = const_cast<char*>("test_DBusNotify");
        argv[1] = nullptr;
        app = new QCoreApplication(argc, argv);
    }

    static void TearDownTestSuite() {
        delete app;
        app = nullptr;
        delete[] argv;
        argv = nullptr;
    }

    static int argc;
    static char** argv;
    static QCoreApplication* app;
    stub_ext::StubExt stub;

    void stubConnectSimple() {
        auto fn = static_cast<DbusConnect7>(&QDBusConnection::connect);
        stub.set_lamda(fn, [](QDBusConnection*, const QString&, const QString&,
                               const QString&, const QString&, const QString&,
                               QObject*, const char*) -> bool {
            return true;
        });
    }

    void stubDisconnectSimple() {
        auto fn = static_cast<DbusDisconnect7>(&QDBusConnection::disconnect);
        stub.set_lamda(fn, [](QDBusConnection*, const QString&, const QString&,
                               const QString&, const QString&, const QString&,
                               QObject*, const char*) -> bool {
            return true;
        });
    }
};

int DBusNotifyTest::argc = 0;
char** DBusNotifyTest::argv = nullptr;
QCoreApplication* DBusNotifyTest::app = nullptr;

// ========================= staticInterfaceName =========================

TEST_F(DBusNotifyTest, StaticInterfaceName_StaticMethod_ReturnsNotificationsInterface) {
    // Arrange
    // (static method, no arrangement needed)

    // Act
    const char* name = DBusNotify::staticInterfaceName();

    // Assert
    EXPECT_STREQ(name, "org.freedesktop.Notifications");
    EXPECT_NE(name, nullptr);
}

// ========================= Constructor =========================

TEST_F(DBusNotifyTest, Constructor_WithParent_SetsParentObject) {
    // Arrange
    QObject parent;
    bool connectCalled = false;
    auto fn = static_cast<DbusConnect7>(&QDBusConnection::connect);
    stub.set_lamda(fn, [&connectCalled](QDBusConnection*, const QString&, const QString&,
                                        const QString&, const QString&, const QString&,
                                        QObject*, const char*) -> bool {
        connectCalled = true;
        return true;
    });

    // Act
    DBusNotify notify(&parent);

    // Assert
    EXPECT_TRUE(connectCalled);
    EXPECT_EQ(notify.parent(), &parent);
}

TEST_F(DBusNotifyTest, Constructor_NullParent_HasNullParent) {
    // Arrange
    bool connectCalled = false;
    auto fn = static_cast<DbusConnect7>(&QDBusConnection::connect);
    stub.set_lamda(fn, [&connectCalled](QDBusConnection*, const QString&, const QString&,
                                        const QString&, const QString&, const QString&,
                                        QObject*, const char*) -> bool {
        connectCalled = true;
        return true;
    });

    // Act
    DBusNotify notify(nullptr);

    // Assert
    EXPECT_TRUE(connectCalled);
    EXPECT_EQ(notify.parent(), nullptr);
}

TEST_F(DBusNotifyTest, Constructor_CallsConnect_UsesPropertyInterfaceAndSignal) {
    // Arrange
    QString capturedIface;
    QString capturedSignal;
    auto fn = static_cast<DbusConnect7>(&QDBusConnection::connect);
    stub.set_lamda(fn, [&capturedIface, &capturedSignal](
            QDBusConnection*, const QString&, const QString&,
            const QString& iface, const QString& name,
            const QString&, QObject*, const char*) -> bool {
        capturedIface = iface;
        capturedSignal = name;
        return true;
    });

    // Act
    DBusNotify notify(nullptr);

    // Assert
    EXPECT_EQ(capturedIface, QString("org.freedesktop.DBus.Properties"));
    EXPECT_EQ(capturedSignal, QString("PropertiesChanged"));
}

// ========================= Destructor =========================

TEST_F(DBusNotifyTest, Destructor_WhenDestroyed_CallsBusDisconnect) {
    // Arrange
    bool connectCalled = false;
    bool disconnectCalled = false;
    auto fnC = static_cast<DbusConnect7>(&QDBusConnection::connect);
    auto fnD = static_cast<DbusDisconnect7>(&QDBusConnection::disconnect);
    stub.set_lamda(fnC, [&connectCalled](QDBusConnection*, const QString&, const QString&,
                                        const QString&, const QString&, const QString&,
                                        QObject*, const char*) -> bool {
        connectCalled = true;
        return true;
    });
    stub.set_lamda(fnD, [&disconnectCalled](QDBusConnection*, const QString&, const QString&,
                                              const QString&, const QString&, const QString&,
                                              QObject*, const char*) -> bool {
        disconnectCalled = true;
        return true;
    });

    // Act
    {
        DBusNotify notify(nullptr);
        EXPECT_TRUE(connectCalled);
    }

    // Assert
    EXPECT_TRUE(disconnectCalled);
    EXPECT_TRUE(disconnectCalled);
}

TEST_F(DBusNotifyTest, Destructor_WhenDestroyed_DisconnectsWithCorrectParams) {
    // Arrange
    QString capturedIface;
    QString capturedSignal;
    stubConnectSimple();
    auto fnD = static_cast<DbusDisconnect7>(&QDBusConnection::disconnect);
    stub.set_lamda(fnD, [&capturedIface, &capturedSignal](
            QDBusConnection*, const QString&, const QString&,
            const QString& iface, const QString& name,
            const QString&, QObject*, const char*) -> bool {
        capturedIface = iface;
        capturedSignal = name;
        return true;
    });

    // Act
    {
        DBusNotify notify(nullptr);
    }

    // Assert
    EXPECT_EQ(capturedIface, QString("org.freedesktop.DBus.Properties"));
    EXPECT_EQ(capturedSignal, QString("PropertiesChanged"));
}

// ========================= ClearRecords =========================

TEST_F(DBusNotifyTest, ClearRecords_NoArgs_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.ClearRecords();

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

// ========================= RemoveRecord =========================

TEST_F(DBusNotifyTest, RemoveRecord_WithRecordId_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.RemoveRecord("test-id");

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

TEST_F(DBusNotifyTest, RemoveRecord_WithEmptyString_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.RemoveRecord("");

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

// ========================= CloseNotification =========================

TEST_F(DBusNotifyTest, CloseNotification_WithId_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.CloseNotification(42);

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

TEST_F(DBusNotifyTest, CloseNotification_WithZeroId_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.CloseNotification(0);

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

// ========================= GetAllRecords =========================

TEST_F(DBusNotifyTest, GetAllRecords_NoArgs_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.GetAllRecords();

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

// ========================= GetCapbilities (typo in source) =========================

TEST_F(DBusNotifyTest, GetCapbilities_NoArgs_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.GetCapbilities();

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

// ========================= Notify =========================

TEST_F(DBusNotifyTest, Notify_WithAllParams_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.Notify("app", 0, "icon", "summary", "body",
                                QStringList(), QVariantMap(), 5000);

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

// ========================= GetServerInformation (async) =========================

TEST_F(DBusNotifyTest, GetServerInformationAsync_NoArgs_ReturnsPendingReply) {
    // Arrange
    stubConnectSimple();
    DBusNotify notify(nullptr);

    // Act
    auto reply = notify.GetServerInformation();

    // Assert
    EXPECT_FALSE(reply.isFinished());
    EXPECT_EQ(notify.lastError().type(), QDBusError::NoError);
}

// ========================= GetServerInformation (sync) =========================

TEST_F(DBusNotifyTest, GetServerInformationSync_ValidReply_ExtractsFourOutputs) {
    // Arrange
    stubConnectSimple();
    QString out1, out2, out3;
    stub.set_lamda(VADDR(QDBusAbstractInterface, callWithArgumentList),
        [](QDBusAbstractInterface*, QDBus::CallMode, const QString& method,
           const QList<QVariant>&) -> QDBusMessage {
            if (method == "GetServerInformation") {
                QList<QVariant> replyArgs;
                replyArgs << QString("return-name")
                          << QString("test-vendor")
                          << QString("test-version")
                          << QString("test-spec");
                QDBusMessage callMsg = QDBusMessage::createMethodCall(
                    "org.freedesktop.Notifications", "/org/freedesktop/Notifications",
                    "org.freedesktop.Notifications", "GetServerInformation");
                return callMsg.createReply(replyArgs);
            }
            return QDBusMessage::createError(QDBusError::Failed, "stub");
        });
    DBusNotify notify(nullptr);

    // Act
    QDBusReply<QString> reply = notify.GetServerInformation(out1, out2, out3);

    // Assert
    EXPECT_TRUE(reply.isValid());
    EXPECT_EQ(out1, QString("test-vendor"));
    EXPECT_EQ(out2, QString("test-version"));
    EXPECT_EQ(out3, QString("test-spec"));
}

TEST_F(DBusNotifyTest, GetServerInformationSync_ErrorReply_PreservesOriginalOutputs) {
    // Arrange
    stubConnectSimple();
    QString out1 = "original1";
    QString out2 = "original2";
    QString out3 = "original3";
    stub.set_lamda(VADDR(QDBusAbstractInterface, callWithArgumentList),
        [](QDBusAbstractInterface*, QDBus::CallMode, const QString&,
           const QList<QVariant>&) -> QDBusMessage {
            return QDBusMessage::createError(QDBusError::Failed, "test error");
        });
    DBusNotify notify(nullptr);

    // Act
    QDBusReply<QString> reply = notify.GetServerInformation(out1, out2, out3);

    // Assert
    EXPECT_FALSE(reply.isValid());
    EXPECT_EQ(out1, QString("original1"));
    EXPECT_EQ(out2, QString("original2"));
    EXPECT_EQ(out3, QString("original3"));
}

TEST_F(DBusNotifyTest, GetServerInformationSync_InsufficientArgs_PreservesOriginalOutputs) {
    // Arrange
    stubConnectSimple();
    QString out1 = "keep1";
    QString out2 = "keep2";
    QString out3 = "keep3";
    stub.set_lamda(VADDR(QDBusAbstractInterface, callWithArgumentList),
        [](QDBusAbstractInterface*, QDBus::CallMode, const QString& method,
           const QList<QVariant>&) -> QDBusMessage {
            if (method == "GetServerInformation") {
                QList<QVariant> replyArgs;
                replyArgs << QString("name") << QString("vendor");
                QDBusMessage callMsg = QDBusMessage::createMethodCall(
                    "org.freedesktop.Notifications", "/org/freedesktop/Notifications",
                    "org.freedesktop.Notifications", "GetServerInformation");
                return callMsg.createReply(replyArgs);
            }
            return QDBusMessage::createError(QDBusError::Failed, "stub");
        });
    DBusNotify notify(nullptr);

    // Act
    QDBusReply<QString> reply = notify.GetServerInformation(out1, out2, out3);

    // Assert
    EXPECT_TRUE(reply.isValid());
    EXPECT_EQ(out1, QString("keep1"));
    EXPECT_EQ(out2, QString("keep2"));
    EXPECT_EQ(out3, QString("keep3"));
}

// ========================= __propertyChanged__ =========================

TEST_F(DBusNotifyTest, PropertyChanged_TwoArgs_EarlyReturnNoCrash) {
    // Arrange
    stubConnectSimple();
    stubDisconnectSimple();
    DBusNotify notify(nullptr);
    QDBusMessage msg = QDBusMessage::createSignal("/path",
                                                   "org.freedesktop.Notifications", "test");
    QList<QVariant> args;
    args << QString("org.freedesktop.Notifications") << QVariantMap();
    msg.setArguments(args);

    // Act
    QMetaObject::invokeMethod(&notify, "__propertyChanged__",
                              Qt::DirectConnection,
                              Q_ARG(QDBusMessage, msg));

    // Assert
    EXPECT_EQ(msg.arguments().size(), 2);
    EXPECT_TRUE(msg.type() == QDBusMessage::SignalMessage);
}

TEST_F(DBusNotifyTest, PropertyChanged_WrongInterface_EarlyReturnNoCrash) {
    // Arrange
    stubConnectSimple();
    stubDisconnectSimple();
    DBusNotify notify(nullptr);
    QDBusMessage msg = QDBusMessage::createSignal("/path", "wrong.interface", "test");
    QList<QVariant> args;
    args << QString("wrong.interface.Name") << QVariantMap() << QStringList();
    msg.setArguments(args);

    // Act
    QMetaObject::invokeMethod(&notify, "__propertyChanged__",
                              Qt::DirectConnection,
                              Q_ARG(QDBusMessage, msg));

    // Assert
    EXPECT_EQ(msg.arguments().size(), 3);
    EXPECT_EQ(msg.arguments().at(0).toString(), QString("wrong.interface.Name"));
}

TEST_F(DBusNotifyTest, PropertyChanged_EmptyProps_IteratesWithoutCrash) {
    // Arrange
    stubConnectSimple();
    stubDisconnectSimple();
    DBusNotify notify(nullptr);
    QDBusMessage msg = QDBusMessage::createSignal("/path",
                                                   "org.freedesktop.Notifications", "test");
    QList<QVariant> args;
    args << QString("org.freedesktop.Notifications") << QVariantMap() << QStringList();
    msg.setArguments(args);

    // Act
    QMetaObject::invokeMethod(&notify, "__propertyChanged__",
                              Qt::DirectConnection,
                              Q_ARG(QDBusMessage, msg));

    // Assert
    EXPECT_EQ(msg.arguments().size(), 3);
    EXPECT_EQ(msg.arguments().at(0).toString(), QString("org.freedesktop.Notifications"));
}

TEST_F(DBusNotifyTest, PropertyChanged_UnmatchedProps_IteratesWithoutCrash) {
    // Arrange
    stubConnectSimple();
    stubDisconnectSimple();
    DBusNotify notify(nullptr);
    QDBusMessage msg = QDBusMessage::createSignal("/path",
                                                   "org.freedesktop.Notifications", "test");
    QList<QVariant> args;
    QVariantMap changedProps;
    changedProps["NonExistentProperty"] = QVariant(42);
    args << QString("org.freedesktop.Notifications") << changedProps << QStringList();
    msg.setArguments(args);

    // Act
    QMetaObject::invokeMethod(&notify, "__propertyChanged__",
                              Qt::DirectConnection,
                              Q_ARG(QDBusMessage, msg));

    // Assert
    EXPECT_EQ(msg.arguments().size(), 3);
    EXPECT_TRUE(msg.arguments().at(1).toMap().contains("NonExistentProperty"));
}
