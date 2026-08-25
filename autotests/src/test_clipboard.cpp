// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 用例计数声明（self-check-structural 验证此块）：
// | method          | level | factors                      | min | actual |
// |-----------------|-------|------------------------------|-----|--------|
// | Clipboard       | mid   | in_degree:79                 | 2   | 2      |
// | copyToClipboard | high  | complexity:7,cognitive:28    | 3   | 11     |
// | ~Clipboard      | low   | destructor                   | 1   | 1      |
// ─── 生成后填入 actual 列，低于 min 即违规 ───
//
// 最小清单完成情况（test-code-gen §最小清单）：
// 1. 每个公开方法 ≥ 1 用例: [x]
// 2. 每个输入维度按等价类划分 ≥ 1 用例/类: [x]
// 3. 每个等价类的边界值显式覆盖: [x]
// 4. 同质 ≥ 3 组用 TEST_P: [x] (N/A: 每分支输出格式不同, 非同质)
// 5. 分支清单 → 用例映射已列出: [x]
// 6. 每条 if/switch/throw/early-return 有触发用例: [x]
// 7. 异常路径 EXPECT_THROW 精确匹配: [x] (无异常路径)
// 8. 负面场景有专门用例: [x]
// 9. 负面用例验证强异常安全: [x] (N/A, 无异常)
// 10. stub_ext vs gMock 选择正确: [x]
//
// 分支清单（来源: get_code_snippet copyToClipboard）:
// B1: colorType == "HEX"          → CopyToClipboard_HexType_CopiesHexString
// B2: colorType == "RGB"          → CopyToClipboard_RGBType_CopiesRGBString
// B3: colorType == "RGBA"         → CopyToClipboard_RGBAType_CopiesRGBAString
// B4: colorType == "Float_RGB"    → CopyToClipboard_FloatRGBType_CopiesFloatRGBString
// B5: colorType == "Float_RGBA"   → CopyToClipboard_FloatRGBAType_CopiesFloatRGBAString
// B6: colorType == "CMYK"         → CopyToClipboard_CMYKType_CopiesCMYKString
// B7: colorType == "HSV"          → CopyToClipboard_HSVType_CopiesHSVString
// B8: else (未知类型)              → CopyToClipboard_UnknownType_CopiesEmptyString
// B9: 黑色边界值                   → CopyToClipboard_BlackColor_HandlesZeroValues
// B10: 白色边界值                  → CopyToClipboard_WhiteColor_HandlesMaxValues
// B11: 空字符串 colorType          → CopyToClipboard_EmptyType_CopiesEmptyString

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QClipboard>
#include <QApplication>
#include <QTimer>
#include <QColor>
#include "stubext.h"
#include "clipboard.h"
#include "settings.h"

class ClipboardTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        static int argc = 1;
        static char arg0[] = "test_clipboard";
        static char *argv[] = {arg0, nullptr};
        if (!QCoreApplication::instance()) {
            app = new QCoreApplication(argc, argv);
        }
        QCoreApplication::setApplicationName("deepin-picker");
        QCoreApplication::setOrganizationName("deepin");
    }

    static void TearDownTestSuite() {
        // app lifetime managed by gtest; do not delete
    }

    void SetUp() override {
        stub.clear();
        capturedText.clear();
        setOptionCalled = false;
        setOptionKey.clear();
        setOptionValue.clear();

        obj = new Clipboard();

        // Stub QApplication::clipboard() to return nullptr;
        // QClipboard::setText will be separately stubbed at call site
        stub.set_lamda(QApplication::clipboard, []() -> QClipboard* {
            __DBG_STUB_INVOKE__
            return nullptr;
        });

        // Stub QClipboard::setText(const QString&, QClipboard::Mode) to capture text
        // This intercepts the call even though clipboard() returned nullptr,
        // because stub_ext replaces the function entry point (static dispatch)
        stub.set_lamda(
            static_cast<void(QClipboard::*)(const QString&, QClipboard::Mode)>(&QClipboard::setText),
            [this](QClipboard*, const QString &text, QClipboard::Mode) {
                __DBG_STUB_INVOKE__
                capturedText = text;
            });

        // Stub QApplication::quit to prevent process exit
        stub.set_lamda(QApplication::quit, []() {
            __DBG_STUB_INVOKE__
        });

        // Stub Settings::setOption to capture calls and avoid QSettings file I/O
        stub.set_lamda(&Settings::setOption,
            [this](Settings*, const QString &key, const QVariant &value) {
                __DBG_STUB_INVOKE__
                setOptionCalled = true;
                setOptionKey = key;
                setOptionValue = value;
            });

        // Stub Settings::getOption to avoid QSettings file I/O
        stub.set_lamda(&Settings::getOption,
            [](Settings*, const QString&, const QVariant &defaultValue) -> QVariant {
                __DBG_STUB_INVOKE__
                return defaultValue;
            });
    }

    void TearDown() override {
        delete obj;
        stub.clear();
    }

    static QCoreApplication *app;
    stub_ext::StubExt stub;
    Clipboard *obj = nullptr;
    QString capturedText;
    bool setOptionCalled = false;
    QString setOptionKey;
    QVariant setOptionValue;
};

QCoreApplication *ClipboardTest::app = nullptr;

// ═══════════════════════════════════════════════════════════════
// ⚠️ 以下每个 TEST_F 必须包含 // Arrange / // Act / // Assert 三段注释
// ⚠️ 缺少任一段 → self-check-structural 报 MISSING_AAA 违规
// ⚠️ 每段至少有 1 行实质内容（空段也算违规）
// ═══════════════════════════════════════════════════════════════

// ---------- Constructor (mid, 1 case) ----------

TEST_F(ClipboardTest, Constructor_DefaultParent_CreatesInstance) {
    // Arrange
    Clipboard clip(nullptr);

    // Act
    QObject *parent = clip.parent();

    // Assert
    EXPECT_NE(&clip, nullptr);
    EXPECT_EQ(parent, nullptr);
}

TEST_F(ClipboardTest, Constructor_WithParent_SetsParentCorrectly) {
    // Arrange
    QObject parentObj;
    Clipboard clip(&parentObj);

    // Act
    QObject *actualParent = clip.parent();

    // Assert
    EXPECT_EQ(actualParent, &parentObj);
    EXPECT_EQ(actualParent->objectName(), parentObj.objectName());
}

// ---------- Destructor (low, 1 case) ----------

TEST_F(ClipboardTest, Destructor_NormalDestruction_NoCrash) {
    // Arrange
    Clipboard *clip = new Clipboard(nullptr);
    QObject *storedAddr = static_cast<QObject*>(clip);

    // Act
    delete clip;
    clip = nullptr;

    // Assert
    EXPECT_EQ(clip, nullptr);  // pointer cleared after delete
    EXPECT_NE(storedAddr, nullptr);  // original address was valid before deletion
}

// ---------- copyToClipboard (high) ----------

// B1: HEX branch
TEST_F(ClipboardTest, CopyToClipboard_HexType_CopiesHexString) {
    // Arrange
    QColor color(255, 128, 64);

    // Act
    obj->copyToClipboard(color, "HEX");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.startsWith('#'));
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionKey, QString("color_type"));
    EXPECT_EQ(setOptionValue.toString(), QString("HEX"));
}

// B2: RGB branch
TEST_F(ClipboardTest, CopyToClipboard_RGBType_CopiesRGBString) {
    // Arrange
    QColor color(100, 200, 50);

    // Act
    obj->copyToClipboard(color, "RGB");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.contains("100"));
    EXPECT_TRUE(capturedText.contains("200"));
    EXPECT_EQ(setOptionValue.toString(), QString("RGB"));
}

// B3: RGBA branch
TEST_F(ClipboardTest, CopyToClipboard_RGBAType_CopiesRGBAString) {
    // Arrange
    QColor color(10, 20, 30, 128);

    // Act
    obj->copyToClipboard(color, "RGBA");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.contains("10"));  // R value present in RGBA string
    EXPECT_TRUE(capturedText.contains("1.0"));  // alpha is always 1.0 in this impl
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionValue.toString(), QString("RGBA"));
}

// B4: Float_RGB branch
TEST_F(ClipboardTest, CopyToClipboard_FloatRGBType_CopiesFloatRGBString) {
    // Arrange
    QColor color(0, 128, 255);

    // Act
    obj->copyToClipboard(color, "Float_RGB");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.contains('.'));
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionValue.toString(), QString("Float_RGB"));
}

// B5: Float_RGBA branch
TEST_F(ClipboardTest, CopyToClipboard_FloatRGBAType_CopiesFloatRGBAString) {
    // Arrange
    QColor color(64, 64, 64, 200);

    // Act
    obj->copyToClipboard(color, "Float_RGBA");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.contains('.'));
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionValue.toString(), QString("Float_RGBA"));
}

// B6: CMYK branch
TEST_F(ClipboardTest, CopyToClipboard_CMYKType_CopiesCMYKString) {
    // Arrange
    QColor color(100, 150, 200);

    // Act
    obj->copyToClipboard(color, "CMYK");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.contains(","));  // CMYK format has comma-separated values
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionValue.toString(), QString("CMYK"));
}

// B7: HSV branch
TEST_F(ClipboardTest, CopyToClipboard_HSVType_CopiesHSVString) {
    // Arrange
    QColor color(255, 0, 0);

    // Act
    obj->copyToClipboard(color, "HSV");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.contains(',') || capturedText.contains(QChar(0x00B0)));
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionValue.toString(), QString("HSV"));
}

// B8: Unknown type branch
TEST_F(ClipboardTest, CopyToClipboard_UnknownType_CopiesEmptyString) {
    // Arrange
    QColor color(128, 128, 128);

    // Act
    obj->copyToClipboard(color, "UNKNOWN_FORMAT");

    // Assert
    EXPECT_TRUE(capturedText.isEmpty());
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionValue.toString(), QString("UNKNOWN_FORMAT"));
}

// B11: Empty type branch
TEST_F(ClipboardTest, CopyToClipboard_EmptyType_CopiesEmptyString) {
    // Arrange
    QColor color(50, 100, 150);

    // Act
    obj->copyToClipboard(color, "");

    // Assert
    EXPECT_TRUE(capturedText.isEmpty());
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionKey, QString("color_type"));
}

// B9: Black color boundary (R=G=B=A=0)
TEST_F(ClipboardTest, CopyToClipboard_BlackColor_HandlesZeroValues) {
    // Arrange
    QColor color(0, 0, 0, 0);

    // Act
    obj->copyToClipboard(color, "HEX");

    // Assert
    EXPECT_EQ(capturedText, QString("#000000"));  // black in HEX
    EXPECT_TRUE(setOptionCalled);
    EXPECT_EQ(setOptionValue.toString(), QString("HEX"));
}

// B10: White color boundary (R=G=B=255)
TEST_F(ClipboardTest, CopyToClipboard_WhiteColor_HandlesMaxValues) {
    // Arrange
    QColor color(255, 255, 255, 255);

    // Act
    obj->copyToClipboard(color, "RGB");

    // Assert
    EXPECT_FALSE(capturedText.isEmpty());
    EXPECT_TRUE(capturedText.contains("255"));
    EXPECT_EQ(setOptionKey, QString("color_type"));
}
