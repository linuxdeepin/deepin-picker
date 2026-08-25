// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <QApplication>
#include <QColor>
#include <QMenu>
#include <QAction>
#include <QGraphicsDropShadowEffect>
#include <QPaintEvent>
#include <QTimer>

#include <stubext.h>

#define private public
#define protected public
#include "colormenu.h"
#include "settings.h"
#undef private
#undef protected

// Branch list (verified against source via MCP get_code_snippet):
// ColorMenu::ColorMenu(x,y,size,color,parent):
//   B1: colorType=="HEX"          -> hexAction->setChecked(true)
//   B2: colorType=="RGB"          -> rgbAction->setChecked(true)
//   B3: colorType=="RGBA"         -> rgbaAction->setChecked(true)
//   B4: colorType=="Float_RGB"    -> rgbFloatAction->setChecked(true)
//   B5: colorType=="Float_RGBA"   -> rgbaFloatAction->setChecked(true)
//   B6: colorType=="CMYK"         -> cmykAction->setChecked(true)
//   B7: colorType=="HSV"          -> hsvAction->setChecked(true)
//   B8: else (no match)            -> no action checked
// ColorMenu::paintEvent(QPaintEvent*):
//   No branches - sequential drawing calls
// copyXxxColor() methods: no branches, set clickMenuItem=true, emit copyColor

// | method | level | factors | min | actual |
// | ColorMenu | high | complexity:8 | 8 | 35 |

// Free function stubs (non-capturing, compatible with Stub::set)
static void stubSetWindowFlags(Qt::WindowFlags) {}
static void stubSetMouseTracking(bool) {}
static void stubInstallEventFilter(QObject *) {}
static void stubSetGraphicsEffect(QGraphicsEffect *) {}
static void stubMoveInt(int, int) {}
static void stubResizeInt(int, int) {}
static void stubShow() {}
static void stubHide() {}
static int stubWidth() { return 100; }
static int stubHeight() { return 100; }

class ColorMenuTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        argc_ = 1;
        app_ = new QApplication(argc_, nullptr);
    }
    static void TearDownTestSuite() {
        delete app_;
        app_ = nullptr;
    }

    void SetUp() override {
        stub.clear();
    }

    void TearDown() override {
        stub.clear();
    }

    // Stub all QWidget methods that interact with windowing system
    void stubQWidgetMethods() {
        stub.set(ADDR(QWidget, setWindowFlags), stubSetWindowFlags);
        stub.set(ADDR(QWidget, setMouseTracking), stubSetMouseTracking);
        stub.set(ADDR(QObject, installEventFilter), stubInstallEventFilter);
        stub.set(ADDR(QWidget, setGraphicsEffect), stubSetGraphicsEffect);
        stub.set((void(QWidget::*)(int, int))&QWidget::move, stubMoveInt);
        stub.set((void(QWidget::*)(int, int))&QWidget::resize, stubResizeInt);
        stub.set(ADDR(QWidget, show), stubShow);
        stub.set(ADDR(QWidget, hide), stubHide);
        stub.set(ADDR(QWidget, width), stubWidth);
        stub.set(ADDR(QWidget, height), stubHeight);
    }

    // Stub Settings::getOption to return a specific color type
    // VADDR produces member function ptr, so lambda receives (this, args...)
    void stubSettingsgetOption(const QString &returnType) {
        stub.set_lamda(VADDR(Settings, getOption),
                      [returnType](Settings *, const QString &, const QVariant &) -> QVariant {
                          return returnType;
                      });
    }

    // Create a test ColorMenu with all necessary stubs
    std::unique_ptr<ColorMenu> createMenu(
            int x = 100, int y = 200, int size = 50,
            const QColor &color = QColor(255, 0, 0),
            const QString &colorType = "HEX")
    {
        stubQWidgetMethods();
        stubSettingsgetOption(colorType);
        return std::make_unique<ColorMenu>(x, y, size, color, nullptr);
    }

    stub_ext::StubExt stub;
    static QApplication *app_;
    static int argc_;
};

QApplication *ColorMenuTest::app_ = nullptr;
int ColorMenuTest::argc_ = 0;

// ============================================================================
// Constructor Tests (HIGH - complexity:8, cognitive:29, lines:102)
// ============================================================================

TEST_F(ColorMenuTest, Constructor_WhenCreated_InitializesState) {
    // Arrange
    const int x = 100, y = 200, size = 50;
    const QColor color(255, 0, 0);

    // Act
    auto menu = createMenu(x, y, size, color, "HEX");

    // Assert
    EXPECT_EQ(menu->windowColor, color);
    EXPECT_EQ(menu->windowSize, size);
    EXPECT_EQ(menu->windowX, x);
    EXPECT_EQ(menu->windowY, y);
}

TEST_F(ColorMenuTest, Constructor_WhenCreated_OffsetsInitialized) {
    // Arrange
    const int testSize = 30;
    const QColor testColor(0, 0, 0);

    // Act
    auto menu = createMenu(0, 0, testSize, testColor, "HEX");

    // Assert
    EXPECT_EQ(menu->menuOffsetX, 10);
    EXPECT_EQ(menu->menuOffsetY, 40);
    EXPECT_EQ(menu->shadowBottomMargin, 20);
    EXPECT_EQ(menu->shadowXMargin, 20);
}

TEST_F(ColorMenuTest, Constructor_WhenCreated_ClickMenuItemFalse) {
    // Arrange
    const QColor testColor(0, 0, 0);

    // Act
    auto menu = createMenu(0, 0, 30, testColor, "HEX");

    // Assert
    EXPECT_FALSE(menu->clickMenuItem);
    EXPECT_EQ(menu->windowSize, 30);
}

TEST_F(ColorMenuTest, Constructor_WhenCreated_AllActionsNotNull) {
    // Arrange
    const QColor testColor(0, 0, 0);
    const int testSize = 30;

    // Act
    auto menu = createMenu(0, 0, testSize, testColor, "HEX");

    // Assert
    EXPECT_NE(menu->hexAction, nullptr);
    EXPECT_NE(menu->rgbAction, nullptr);
    EXPECT_NE(menu->rgbFloatAction, nullptr);
    EXPECT_NE(menu->rgbaAction, nullptr);
    EXPECT_NE(menu->rgbaFloatAction, nullptr);
    EXPECT_NE(menu->cmykAction, nullptr);
    EXPECT_NE(menu->hsvAction, nullptr);
}

TEST_F(ColorMenuTest, Constructor_WhenCreated_AllActionsCheckable) {
    // Arrange
    const QColor testColor(0, 0, 0);

    // Act
    auto menu = createMenu(0, 0, 30, testColor, "HEX");

    // Assert
    EXPECT_TRUE(menu->hexAction->isCheckable());
    EXPECT_TRUE(menu->rgbAction->isCheckable());
    EXPECT_TRUE(menu->rgbFloatAction->isCheckable());
    EXPECT_TRUE(menu->rgbaAction->isCheckable());
    EXPECT_TRUE(menu->rgbaFloatAction->isCheckable());
    EXPECT_TRUE(menu->cmykAction->isCheckable());
    EXPECT_TRUE(menu->hsvAction->isCheckable());
}

// --- Branch coverage for color type selection ---

TEST_F(ColorMenuTest, Constructor_ColorTypeHEX_HexActionChecked) {
    // Arrange
    const QString colorType("HEX");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B1: HEX branch
    EXPECT_TRUE(menu->hexAction->isChecked());
    EXPECT_FALSE(menu->rgbAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeRGB_RgbActionChecked) {
    // Arrange
    const QString colorType("RGB");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B2: RGB branch
    EXPECT_TRUE(menu->rgbAction->isChecked());
    EXPECT_FALSE(menu->hexAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeRGBA_RgbaActionChecked) {
    // Arrange
    const QString colorType("RGBA");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B3: RGBA branch
    EXPECT_TRUE(menu->rgbaAction->isChecked());
    EXPECT_FALSE(menu->hexAction->isChecked());
    EXPECT_FALSE(menu->rgbAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeFloatRGB_RgbFloatActionChecked) {
    // Arrange
    const QString colorType("Float_RGB");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B4: Float_RGB branch
    EXPECT_TRUE(menu->rgbFloatAction->isChecked());
    EXPECT_FALSE(menu->hexAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeFloatRGBA_RgbaFloatActionChecked) {
    // Arrange
    const QString colorType("Float_RGBA");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B5: Float_RGBA branch
    EXPECT_TRUE(menu->rgbaFloatAction->isChecked());
    EXPECT_FALSE(menu->hexAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeCMYK_CmykActionChecked) {
    // Arrange
    const QString colorType("CMYK");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B6: CMYK branch
    EXPECT_TRUE(menu->cmykAction->isChecked());
    EXPECT_FALSE(menu->hexAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeHSV_HsvActionChecked) {
    // Arrange
    const QString colorType("HSV");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B7: HSV branch
    EXPECT_TRUE(menu->hsvAction->isChecked());
    EXPECT_FALSE(menu->hexAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeUnknown_NoActionChecked) {
    // Arrange
    const QString colorType("UNKNOWN_TYPE");

    // Act
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), colorType);

    // Assert - B8: else branch (no match)
    EXPECT_FALSE(menu->hexAction->isChecked());
    EXPECT_FALSE(menu->rgbAction->isChecked());
    EXPECT_FALSE(menu->rgbaAction->isChecked());
    EXPECT_FALSE(menu->rgbFloatAction->isChecked());
    EXPECT_FALSE(menu->rgbaFloatAction->isChecked());
    EXPECT_FALSE(menu->cmykAction->isChecked());
    EXPECT_FALSE(menu->hsvAction->isChecked());
}

TEST_F(ColorMenuTest, Constructor_ColorTypeEmpty_NoActionChecked) {
    // Arrange
    const QString colorType("");
    const int testSize = 30;

    // Act
    auto menu = createMenu(0, 0, testSize, QColor(0, 0, 0), colorType);

    // Assert - B8: empty string falls through to else
    EXPECT_FALSE(menu->hexAction->isChecked());
    EXPECT_FALSE(menu->rgbAction->isChecked());
    EXPECT_FALSE(menu->hsvAction->isChecked());
    EXPECT_EQ(menu->windowSize, testSize);
}

// --- Boundary/edge case constructor tests ---

TEST_F(ColorMenuTest, Constructor_NegativeCoords_StoresAsIs) {
    // Arrange
    const int x = -100, y = -200;

    // Act
    auto menu = createMenu(x, y, 30, QColor(0, 0, 0), "HEX");

    // Assert
    EXPECT_EQ(menu->windowX, -100);
    EXPECT_EQ(menu->windowY, -200);
}

TEST_F(ColorMenuTest, Constructor_ZeroSize_StoresZero) {
    // Arrange
    const int zeroSize = 0;

    // Act
    auto menu = createMenu(0, 0, zeroSize, QColor(0, 0, 0), "HEX");

    // Assert
    EXPECT_EQ(menu->windowSize, 0);
    EXPECT_NE(menu, nullptr);
}

TEST_F(ColorMenuTest, Constructor_LargeSize_HandlesCorrectly) {
    // Arrange
    const int largeSize = 10000;
    const QColor testColor(128, 128, 128);

    // Act
    auto menu = createMenu(0, 0, largeSize, testColor, "HEX");

    // Assert
    EXPECT_EQ(menu->windowSize, largeSize);
    EXPECT_EQ(menu->windowColor, testColor);
}

TEST_F(ColorMenuTest, Constructor_TransparentColor_StoresAlpha) {
    // Arrange
    const QColor transparent(255, 0, 0, 128);

    // Act
    auto menu = createMenu(0, 0, 30, transparent, "HEX");

    // Assert
    EXPECT_EQ(menu->windowColor, transparent);
    EXPECT_EQ(menu->windowColor.alpha(), 128);
}

TEST_F(ColorMenuTest, Constructor_BlackColor_StoresBlack) {
    // Arrange
    const QColor blackColor(0, 0, 0);

    // Act
    auto menu = createMenu(0, 0, 30, blackColor, "HEX");

    // Assert
    EXPECT_EQ(menu->windowColor, blackColor);
    EXPECT_TRUE(menu->windowColor.isValid());
}

TEST_F(ColorMenuTest, Constructor_WhiteColor_StoresWhite) {
    // Arrange
    const QColor whiteColor(255, 255, 255);

    // Act
    auto menu = createMenu(0, 0, 30, whiteColor, "HEX");

    // Assert
    EXPECT_EQ(menu->windowColor, whiteColor);
    EXPECT_TRUE(menu->windowColor.isValid());
}

// ============================================================================
// Copy Method Tests (all follow pattern: clickMenuItem=true, emit copyColor)
// ============================================================================

TEST_F(ColorMenuTest, CopyRGBColor_WhenCalled_EmitsSignalWithRGBType) {
    // Arrange
    const QColor testColor(10, 20, 30);
    auto menu = createMenu(0, 0, 30, testColor, "HEX");
    QColor receivedColor;
    QString receivedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         receivedColor = c;
                         receivedType = t;
                     });

    // Act
    menu->copyRGBColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(receivedColor, testColor);
    EXPECT_EQ(receivedType, QString("RGB"));
}

TEST_F(ColorMenuTest, CopyRGBAColor_WhenCalled_EmitsSignalWithRGBAType) {
    // Arrange
    const QColor testColor(100, 150, 200);
    auto menu = createMenu(0, 0, 30, testColor, "HEX");
    QColor receivedColor;
    QString receivedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         receivedColor = c;
                         receivedType = t;
                     });

    // Act
    menu->copyRGBAColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(receivedColor, testColor);
    EXPECT_EQ(receivedType, QString("RGBA"));
}

TEST_F(ColorMenuTest, CopyHexColor_WhenCalled_EmitsSignalWithHEXType) {
    // Arrange
    const QColor testColor(255, 128, 0);
    auto menu = createMenu(0, 0, 30, testColor, "HEX");
    QColor receivedColor;
    QString receivedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         receivedColor = c;
                         receivedType = t;
                     });

    // Act
    menu->copyHexColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(receivedColor, testColor);
    EXPECT_EQ(receivedType, QString("HEX"));
}

TEST_F(ColorMenuTest, CopyFloatRGBColor_WhenCalled_EmitsFloatRGB) {
    // Arrange
    const QColor testColor(0, 255, 128);
    auto menu = createMenu(0, 0, 30, testColor, "RGB");
    QColor receivedColor;
    QString receivedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         receivedColor = c;
                         receivedType = t;
                     });

    // Act
    menu->copyFloatRGBColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(receivedColor, testColor);
    EXPECT_EQ(receivedType, QString("Float_RGB"));
}

TEST_F(ColorMenuTest, CopyFloatRGBAColor_WhenCalled_EmitsFloatRGBA) {
    // Arrange
    const QColor testColor(64, 64, 64);
    auto menu = createMenu(0, 0, 30, testColor, "RGBA");
    QColor receivedColor;
    QString receivedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         receivedColor = c;
                         receivedType = t;
                     });

    // Act
    menu->copyFloatRGBAColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(receivedColor, testColor);
    EXPECT_EQ(receivedType, QString("Float_RGBA"));
}

TEST_F(ColorMenuTest, CopyCmykColor_WhenCalled_EmitsCMYKType) {
    // Arrange
    const QColor testColor(200, 100, 50);
    auto menu = createMenu(0, 0, 30, testColor, "CMYK");
    QColor receivedColor;
    QString receivedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         receivedColor = c;
                         receivedType = t;
                     });

    // Act
    menu->copyCmykColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(receivedColor, testColor);
    EXPECT_EQ(receivedType, QString("CMYK"));
}

TEST_F(ColorMenuTest, CopyHsvColor_WhenCalled_EmitsHSVType) {
    // Arrange
    const QColor testColor(180, 60, 240);
    auto menu = createMenu(0, 0, 30, testColor, "HSV");
    QColor receivedColor;
    QString receivedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         receivedColor = c;
                         receivedType = t;
                     });

    // Act
    menu->copyHsvColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(receivedColor, testColor);
    EXPECT_EQ(receivedType, QString("HSV"));
}

// --- Copy method edge cases ---

TEST_F(ColorMenuTest, CopyMethod_CalledMultipleTimes_ClickStaysTrue) {
    // Arrange
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), "HEX");
    int signalCount = 0;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor, QString) { signalCount++; });

    // Act
    menu->copyRGBColor();
    menu->copyHexColor();
    menu->copyRGBAColor();

    // Assert
    EXPECT_TRUE(menu->clickMenuItem);
    EXPECT_EQ(signalCount, 3);
}

TEST_F(ColorMenuTest, CopyRGBA_TransparentColor_EmitsWithAlpha) {
    // Arrange
    const QColor semiTransparent(100, 200, 50, 80);
    auto menu = createMenu(0, 0, 30, semiTransparent, "RGBA");
    QColor receivedColor;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString) { receivedColor = c; });

    // Act
    menu->copyRGBAColor();

    // Assert
    EXPECT_EQ(receivedColor.alpha(), 80);
    EXPECT_EQ(receivedColor, semiTransparent);
}

// ============================================================================
// paintEvent Tests
// ============================================================================

TEST_F(ColorMenuTest, PaintEvent_WhenCalled_NoCrash) {
    // Arrange
    const QColor testColor(255, 0, 0);
    auto menu = createMenu(0, 0, 30, testColor, "HEX");
    QPaintEvent event(QRect(0, 0, 30, 30));

    // Act
    menu->paintEvent(&event);

    // Assert - no crash, paint completed
    EXPECT_NE(menu, nullptr);
    EXPECT_EQ(menu->windowSize, 30);
}

TEST_F(ColorMenuTest, PaintEvent_DifferentSize_NoCrash) {
    // Arrange
    const QColor testColor(0, 255, 0);
    auto menu = createMenu(0, 0, 200, testColor, "RGB");
    QPaintEvent event(QRect(0, 0, 200, 200));

    // Act
    menu->paintEvent(&event);

    // Assert
    EXPECT_EQ(menu->windowSize, 200);
    EXPECT_EQ(menu->windowColor, testColor);
}

TEST_F(ColorMenuTest, PaintEvent_BlackColor_NoCrash) {
    // Arrange
    const QColor blackColor(0, 0, 0);
    auto menu = createMenu(0, 0, 10, blackColor, "HEX");
    QPaintEvent event(QRect(0, 0, 10, 10));

    // Act
    menu->paintEvent(&event);

    // Assert
    EXPECT_EQ(menu->windowColor, blackColor);
    EXPECT_NE(menu, nullptr);
}

// ============================================================================
// Destructor Test
// ============================================================================

TEST_F(ColorMenuTest, Destructor_WhenCalled_NoCrash) {
    // Arrange
    const QColor testColor(128, 128, 128);
    auto menu = createMenu(100, 200, 50, testColor, "HEX");
    const bool isValid = (menu != nullptr);

    // Act
    menu.reset();

    // Assert - destructor called via reset, no crash
    EXPECT_TRUE(isValid);
    EXPECT_TRUE(true);
}

// ============================================================================
// Signal Tests
// ============================================================================

TEST_F(ColorMenuTest, ExitSignal_WhenEmitted_ReceivesCallback) {
    // Arrange
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), "HEX");
    bool exitEmitted = false;
    QObject::connect(menu.get(), &ColorMenu::exit, [&]() {
        exitEmitted = true;
    });

    // Act
    emit menu->exit();

    // Assert
    EXPECT_TRUE(exitEmitted);
    EXPECT_NE(menu, nullptr);
}

TEST_F(ColorMenuTest, CopyColorSignal_WhenEmitted_CarriesCorrectData) {
    // Arrange
    const QColor sentColor(42, 84, 168);
    auto menu = createMenu(0, 0, 30, QColor(0, 0, 0), "HEX");
    QColor capturedColor;
    QString capturedType;
    QObject::connect(menu.get(), &ColorMenu::copyColor,
                     [&](QColor c, QString t) {
                         capturedColor = c;
                         capturedType = t;
                     });

    // Act
    emit menu->copyColor(sentColor, "TEST_TYPE");

    // Assert
    EXPECT_EQ(capturedColor, sentColor);
    EXPECT_EQ(capturedType, QString("TEST_TYPE"));
}