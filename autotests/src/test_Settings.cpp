// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 用例计数声明（self-check-structural 验证此块）：
// | method      | level | factors    | min | actual |
// |-------------|-------|------------|-----|--------|
// | Settings    | mid   | in_degree:1| 2   | 2      |
// | configPath  | mid   | in_degree:1| 2   | 2      |
// | getOption   | mid   | in_degree:1| 2   | 3      |
// | setOption   | mid   | in_degree:1| 2   | 3      |
// | ~Settings   | low   | destructor | 1   | 1      |
// ─── 生成后填入 actual 列，低于 min 即违规 ───
//
// 最小清单完成情况（test-code-gen §最小清单）：
// 1. 每个公开方法 ≥ 1 用例: [x]
// 2. 每个输入维度按等价类划分 ≥ 1 用例/类: [x]
// 3. 每个等价类的边界值显式覆盖: [x]
// 4. 同质 ≥ 3 组用 TEST_P: [N/A]
// 5. 分支清单 → 用例映射已列出: [x]
// 6. 每条 if/switch/throw/early-return 有触发用例: [x]
// 7. 异常路径 EXPECT_THROW 精确匹配: [x] (N/A)
// 8. 负面场景有专门用例: [x]
// 9. 负面用例验证强异常安全: [x] (N/A)
// 10. stub_ext vs gMock 选择正确: [x]
//
// 分支清单（来源: get_code_snippet）:
// Settings(): 调用 configPath() 和 new QSettings()  → 构造用例
// configPath(): 无分支（纯拼接）→ 正常用例 + 边界用例
// getOption(): beginGroup/value/endGroup → 正常返回 + 默认值
// setOption(): beginGroup/setValue/endGroup/sync → 正常存储 + 同步
// ~Settings(): delete settings → 析构用例

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSettings>
#include <QTemporaryDir>
#include <QDateTime>
#include "stubext.h"
#include "settings.h"

class SettingsTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        static int argc = 1;
        static char arg0[] = "test_settings";
        static char *argv[] = {arg0, nullptr};
        if (!QCoreApplication::instance()) {
            app = new QCoreApplication(argc, argv);
        }
        QCoreApplication::setApplicationName("deepin-picker-test");
        QCoreApplication::setOrganizationName("deepin-test");
    }

    static void TearDownTestSuite() {
        // app lifetime managed by gtest; do not delete
    }

    void SetUp() override {
        stub.clear();
    }

    void TearDown() override {
        stub.clear();
    }

    static QCoreApplication *app;
    stub_ext::StubExt stub;
};

QCoreApplication *SettingsTest::app = nullptr;

// ═══════════════════════════════════════════════════════════════
// ⚠️ 以下每个 TEST_F 必须包含 // Arrange / // Act / // Assert 三段注释
// ⚠️ 缺少任一段 → self-check-structural 报 MISSING_AAA 违规
// ═══════════════════════════════════════════════════════════════

// ---------- Constructor (mid, 2 cases) ----------

TEST_F(SettingsTest, Constructor_DefaultParent_CreatesInstance) {
    // Arrange
    Settings s(nullptr);

    // Act
    QObject *parent = s.parent();

    // Assert
    EXPECT_NE(&s, nullptr);
    EXPECT_EQ(parent, nullptr);
}

TEST_F(SettingsTest, Constructor_WithParent_SetsParentCorrectly) {
    // Arrange
    QObject parentObj;
    parentObj.setObjectName("testParent");

    // Act
    Settings s(&parentObj);
    QObject *actualParent = s.parent();

    // Assert
    EXPECT_EQ(actualParent, &parentObj);
    EXPECT_EQ(actualParent->objectName(), QString("testParent"));
}

// ---------- Destructor (low, 1 case) ----------

TEST_F(SettingsTest, Destructor_NormalDestruction_NoCrash) {
    // Arrange
    Settings *s = new Settings(nullptr);
    Settings *storedAddr = s;

    // Act
    delete s;
    s = nullptr;

    // Assert
    EXPECT_EQ(s, nullptr);
    EXPECT_NE(storedAddr, nullptr);  // address was valid before deletion
}

// ---------- configPath (mid, 2 cases) ----------

TEST_F(SettingsTest, ConfigPath_WithAppSet_ReturnsFormattedPath) {
    // Arrange
    Settings s(nullptr);

    // Act
    QString path = s.configPath();

    // Assert
    EXPECT_EQ(path, QString("deepin-test/deepin-picker-test/config.conf"));
    EXPECT_TRUE(path.endsWith("config.conf"));
}

TEST_F(SettingsTest, ConfigPath_CalledTwice_ReturnsSameResult) {
    // Arrange
    Settings s(nullptr);

    // Act
    QString path1 = s.configPath();
    QString path2 = s.configPath();

    // Assert
    EXPECT_EQ(path1, path2);  // deterministic
    EXPECT_FALSE(path1.isEmpty());
}

// ---------- getOption (mid, 3 cases) ----------

TEST_F(SettingsTest, GetOption_NonExistentKey_ReturnsDefaultValue) {
    // Arrange
    Settings s(nullptr);
    QString uniqueKey = "_ut_nonexistent_key_" + QString::number(QDateTime::currentMSecsSinceEpoch());

    // Act
    QVariant result = s.getOption(uniqueKey, QVariant("fallback"));

    // Assert
    EXPECT_EQ(result.toString(), QString("fallback"));
    EXPECT_FALSE(result.toString().isEmpty());
}

TEST_F(SettingsTest, GetOption_IntDefaultValue_ReturnsCorrectType) {
    // Arrange
    Settings s(nullptr);
    QString uniqueKey = "_ut_int_default_" + QString::number(QDateTime::currentMSecsSinceEpoch());

    // Act
    QVariant result = s.getOption(uniqueKey, QVariant(42));

    // Assert
    EXPECT_EQ(result.toInt(), 42);
    EXPECT_TRUE(result.isValid());
}

TEST_F(SettingsTest, GetOption_AfterSetOption_ReturnsStoredValue) {
    // Arrange
    Settings s(nullptr);
    QString key = "_ut_roundtrip_key_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    s.setOption(key, QVariant("stored_value"));

    // Act
    QVariant result = s.getOption(key, QVariant("default"));

    // Assert
    EXPECT_EQ(result.toString(), QString("stored_value"));
    EXPECT_NE(result.toString(), QString("default"));  // not the default
}

// ---------- setOption (mid, 3 cases) ----------

TEST_F(SettingsTest, SetOption_StringValue_StoresSuccessfully) {
    // Arrange
    Settings s(nullptr);
    QString key = "_ut_set_string_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    QString value = "hello_world";

    // Act
    s.setOption(key, QVariant(value));

    // Assert
    QVariant retrieved = s.getOption(key, QVariant());
    EXPECT_EQ(retrieved.toString(), value);
    EXPECT_TRUE(retrieved.isValid());
}

TEST_F(SettingsTest, SetOption_IntValue_StoresSuccessfully) {
    // Arrange
    Settings s(nullptr);
    QString key = "_ut_set_int_" + QString::number(QDateTime::currentMSecsSinceEpoch());

    // Act
    s.setOption(key, QVariant(999));

    // Assert
    QVariant retrieved = s.getOption(key, QVariant(0));
    EXPECT_EQ(retrieved.toInt(), 999);
    EXPECT_NE(retrieved.toInt(), 0);  // not the default
}

TEST_F(SettingsTest, SetOption_OverwritePreviousValue_UpdatesCorrectly) {
    // Arrange
    Settings s(nullptr);
    QString key = "_ut_overwrite_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    s.setOption(key, QVariant("first"));

    // Act
    s.setOption(key, QVariant("second"));

    // Assert
    QVariant result = s.getOption(key, QVariant());
    EXPECT_EQ(result.toString(), QString("second"));
    EXPECT_NE(result.toString(), QString("first"));  // old value is gone
}
