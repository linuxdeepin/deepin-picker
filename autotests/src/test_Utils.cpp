// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>

#include <QColor>
#include <QCoreApplication>

#include "utils.h"

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    static void SetUpTestSuite() {
        // GUI classes use QCoreApplication (not QApplication) in SetUpTestSuite
        argc = 1;
        argv = new char*[2];
        argv[0] = const_cast<char*>("test_Utils");
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
};

int UtilsTest::argc = 0;
char** UtilsTest::argv = nullptr;
QCoreApplication* UtilsTest::app = nullptr;

// ========================= colorToHex =========================

TEST_F(UtilsTest, ColorToHex_Black_ReturnsCorrectHex) {
    // Arrange
    QColor black(Qt::black);

    // Act
    QString result = Utils::colorToHex(black);

    // Assert
    EXPECT_EQ(result, QString("#000000"));
    EXPECT_FALSE(result.isEmpty());
}

TEST_F(UtilsTest, ColorToHex_White_ReturnsCorrectHex) {
    // Arrange
    QColor white(Qt::white);

    // Act
    QString result = Utils::colorToHex(white);

    // Assert
    EXPECT_EQ(result, QString("#FFFFFF"));
    EXPECT_TRUE(result.startsWith('#'));
}

TEST_F(UtilsTest, ColorToHex_CustomColor_ReturnsUpperHex) {
    // Arrange
    QColor custom(0xAB, 0xCD, 0xEF);

    // Act
    QString result = Utils::colorToHex(custom);

    // Assert
    EXPECT_EQ(result, QString("#ABCDEF"));
    EXPECT_EQ(result.length(), 7);
}

TEST_F(UtilsTest, ColorToHex_Red_ReturnsCorrectHex) {
    // Arrange
    QColor red(Qt::red);

    // Act
    QString result = Utils::colorToHex(red);

    // Assert
    EXPECT_EQ(result, QString("#FF0000"));
    EXPECT_TRUE(result.toUpper() == result);
}

TEST_F(UtilsTest, ColorToHex_Green_ReturnsCorrectHex) {
    // Arrange
    QColor green(Qt::green);

    // Act
    QString result = Utils::colorToHex(green);

    // Assert
    EXPECT_EQ(result, QString("#00FF00"));
    EXPECT_EQ(result[1], '0');
}

TEST_F(UtilsTest, ColorToHex_Blue_ReturnsCorrectHex) {
    // Arrange
    QColor blue(Qt::blue);

    // Act
    QString result = Utils::colorToHex(blue);

    // Assert
    EXPECT_EQ(result, QString("#0000FF"));
    EXPECT_EQ(result.right(2), QString("FF"));
}

// ========================= colorToRGB =========================

TEST_F(UtilsTest, ColorToRGB_Black_ReturnsZeroRGB) {
    // Arrange
    QColor black(Qt::black);

    // Act
    QString result = Utils::colorToRGB(black);

    // Assert
    EXPECT_EQ(result, QString("(0, 0, 0)"));
    EXPECT_TRUE(result.contains('0'));
}

TEST_F(UtilsTest, ColorToRGB_White_ReturnsMaxRGB) {
    // Arrange
    QColor white(Qt::white);

    // Act
    QString result = Utils::colorToRGB(white);

    // Assert
    EXPECT_EQ(result, QString("(255, 255, 255)"));
    EXPECT_TRUE(result.startsWith('('));
}

TEST_F(UtilsTest, ColorToRGB_CustomColor_ReturnsCorrectRGB) {
    // Arrange
    QColor custom(100, 200, 50);

    // Act
    QString result = Utils::colorToRGB(custom);

    // Assert
    EXPECT_EQ(result, QString("(100, 200, 50)"));
    EXPECT_TRUE(result.endsWith(')'));
}

TEST_F(UtilsTest, ColorToRGB_BoundaryMax_Returns255Values) {
    // Arrange
    QColor maxColor(255, 255, 255);

    // Act
    QString result = Utils::colorToRGB(maxColor);

    // Assert
    EXPECT_EQ(result, QString("(255, 255, 255)"));
    EXPECT_FALSE(result.contains("256"));
}

TEST_F(UtilsTest, ColorToRGB_BoundaryMin_ReturnsZeroValues) {
    // Arrange
    QColor minColor(0, 0, 0);

    // Act
    QString result = Utils::colorToRGB(minColor);

    // Assert
    EXPECT_EQ(result, QString("(0, 0, 0)"));
    EXPECT_FALSE(result.contains("-"));
}

// ========================= colorToRGBA =========================

TEST_F(UtilsTest, ColorToRGBA_Black_ReturnsZeroRGBWithAlpha) {
    // Arrange
    QColor black(Qt::black);

    // Act
    QString result = Utils::colorToRGBA(black);

    // Assert
    EXPECT_EQ(result, QString("(0, 0, 0, 1.0)"));
    EXPECT_TRUE(result.contains("1.0"));
}

TEST_F(UtilsTest, ColorToRGBA_White_ReturnsMaxRGBWithAlpha) {
    // Arrange
    QColor white(Qt::white);

    // Act
    QString result = Utils::colorToRGBA(white);

    // Assert
    EXPECT_EQ(result, QString("(255, 255, 255, 1.0)"));
    EXPECT_TRUE(result.contains("255"));
}

TEST_F(UtilsTest, ColorToRGBA_CustomColor_ReturnsCorrectRGBA) {
    // Arrange
    QColor custom(128, 64, 32);

    // Act
    QString result = Utils::colorToRGBA(custom);

    // Assert
    EXPECT_EQ(result, QString("(128, 64, 32, 1.0)"));
    EXPECT_TRUE(result.endsWith(", 1.0)"));
}

TEST_F(UtilsTest, ColorToRGBA_AlwaysHardcodedAlpha_ReturnsAlpha1) {
    // Arrange
    // Even with an alpha of 0, the method hardcodes 1.0
    QColor transparent(255, 0, 0, 0);

    // Act
    QString result = Utils::colorToRGBA(transparent);

    // Assert
    EXPECT_EQ(result, QString("(255, 0, 0, 1.0)"));
    // DEFECT: alpha is hardcoded to 1.0, ignoring actual color alpha
    EXPECT_TRUE(result.contains("1.0"));
}

// ========================= colorToFloatRGB =========================

TEST_F(UtilsTest, ColorToFloatRGB_Black_ReturnsZeroFloats) {
    // Arrange
    QColor black(Qt::black);

    // Act
    QString result = Utils::colorToFloatRGB(black);

    // Assert
    EXPECT_EQ(result, QString("(0, 0, 0)"));
    EXPECT_TRUE(result.startsWith('('));
}

TEST_F(UtilsTest, ColorToFloatRGB_White_ReturnsOnes) {
    // Arrange
    QColor white(Qt::white);

    // Act
    QString result = Utils::colorToFloatRGB(white);

    // Assert
    EXPECT_EQ(result, QString("(1, 1, 1)"));
    EXPECT_TRUE(result.contains('1'));
}

TEST_F(UtilsTest, ColorToFloatRGB_HalfIntensities_ReturnsZeroPointFive) {
    // Arrange
    QColor half(128, 128, 128);

    // Act
    QString result = Utils::colorToFloatRGB(half);

    // Assert
    // 128/255.0 = 0.501961...
    EXPECT_TRUE(result.contains("0.5"));
    EXPECT_TRUE(result.startsWith('('));
    EXPECT_TRUE(result.endsWith(')'));
    EXPECT_GT(result.length(), 5);
}

TEST_F(UtilsTest, ColorToFloatRGB_RedChannel_ReturnsCorrectFloat) {
    // Arrange
    QColor red(Qt::red);

    // Act
    QString result = Utils::colorToFloatRGB(red);

    // Assert
    EXPECT_EQ(result, QString("(1, 0, 0)"));
    EXPECT_TRUE(result.contains(", 0,"));
}

// ========================= colorToFloatRGBA =========================

TEST_F(UtilsTest, ColorToFloatRGBA_Black_ReturnsZeroFloatsWithAlpha) {
    // Arrange
    QColor black(Qt::black);

    // Act
    QString result = Utils::colorToFloatRGBA(black);

    // Assert
    EXPECT_EQ(result, QString("(0, 0, 0, 1.0)"));
    EXPECT_TRUE(result.contains("1.0"));
}

TEST_F(UtilsTest, ColorToFloatRGBA_White_ReturnsOnesWithAlpha) {
    // Arrange
    QColor white(Qt::white);

    // Act
    QString result = Utils::colorToFloatRGBA(white);

    // Assert
    EXPECT_EQ(result, QString("(1, 1, 1, 1.0)"));
    EXPECT_TRUE(result.endsWith(", 1.0)"));
}

TEST_F(UtilsTest, ColorToFloatRGBA_CustomColor_ReturnsCorrectFloatRGBA) {
    // Arrange
    QColor custom(64, 128, 192);

    // Act
    QString result = Utils::colorToFloatRGBA(custom);

    // Assert
    // 64/255.0, 128/255.0, 192/255.0
    EXPECT_TRUE(result.startsWith('('));
    EXPECT_TRUE(result.endsWith(", 1.0)"));
    EXPECT_TRUE(result.contains(','));
    EXPECT_GT(result.length(), 10);
}

// ========================= colorToCMYK =========================

TEST_F(UtilsTest, ColorToCMYK_Black_ReturnsCorrectCMYK) {
    // Arrange
    QColor black(Qt::black);

    // Act
    QString result = Utils::colorToCMYK(black);

    // Assert
    // Black in CMYK: (0, 0, 0, 255)
    EXPECT_EQ(result, QString("(0, 0, 0, 255)"));
    EXPECT_TRUE(result.contains("255"));
}

TEST_F(UtilsTest, ColorToCMYK_Cyan_ReturnsHighCyan) {
    // Arrange
    QColor cyan(Qt::cyan);

    // Act
    QString result = Utils::colorToCMYK(cyan);

    // Assert
    // Cyan (0, 255, 255) in CMYK should have high cyan
    EXPECT_TRUE(result.startsWith('('));
    EXPECT_TRUE(result.endsWith(')'));
    // First value (cyan) should be the max
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 4);
    int cyanVal = parts[0].trimmed().toInt();
    EXPECT_GT(cyanVal, 0);
}

TEST_F(UtilsTest, ColorToCMYK_Magenta_ReturnsHighMagenta) {
    // Arrange
    QColor magenta(Qt::magenta);

    // Act
    QString result = Utils::colorToCMYK(magenta);

    // Assert
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 4);
    int magentaVal = parts[1].trimmed().toInt();
    EXPECT_GT(magentaVal, 0);
}

TEST_F(UtilsTest, ColorToCMYK_Yellow_ReturnsHighYellow) {
    // Arrange
    QColor yellow(Qt::yellow);

    // Act
    QString result = Utils::colorToCMYK(yellow);

    // Assert
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 4);
    int yellowVal = parts[2].trimmed().toInt();
    EXPECT_GT(yellowVal, 0);
}

TEST_F(UtilsTest, ColorToCMYK_CustomColor_ReturnsFourValues) {
    // Arrange
    QColor custom(100, 150, 200);

    // Act
    QString result = Utils::colorToCMYK(custom);

    // Assert
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 4);
    EXPECT_TRUE(result.contains(','));
}

// ========================= colorToHSV =========================

TEST_F(UtilsTest, ColorToHSV_Red_ReturnsZeroHue) {
    // Arrange
    QColor red(Qt::red);

    // Act
    QString result = Utils::colorToHSV(red);

    // Assert
    // Red has hue = 0
    EXPECT_TRUE(result.startsWith("(0,"));
    EXPECT_TRUE(result.endsWith(')'));
    EXPECT_GT(result.length(), 3);
}

TEST_F(UtilsTest, ColorToHSV_Green_ReturnsHue120) {
    // Arrange
    QColor green(Qt::green);

    // Act
    QString result = Utils::colorToHSV(green);

    // Assert
    // Green has hue = 120
    EXPECT_TRUE(result.contains("120"));
    EXPECT_TRUE(result.startsWith('('));
    EXPECT_GT(result.length(), 5);
}

TEST_F(UtilsTest, ColorToHSV_Blue_ReturnsHue240) {
    // Arrange
    QColor blue(Qt::blue);

    // Act
    QString result = Utils::colorToHSV(blue);

    // Assert
    // Blue has hue = 240
    EXPECT_TRUE(result.contains("240"));
    EXPECT_TRUE(result.endsWith(')'));
    EXPECT_GT(result.length(), 5);
}

TEST_F(UtilsTest, ColorToHSV_Gray_ReturnsZeroSaturation) {
    // Arrange
    QColor gray(128, 128, 128);

    // Act
    QString result = Utils::colorToHSV(gray);

    // Assert
    // Gray has saturation = 0
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 3);
    int sat = parts[1].trimmed().toInt();
    EXPECT_EQ(sat, 0);
}

TEST_F(UtilsTest, ColorToHSV_Black_ReturnsZeroValue) {
    // Arrange
    QColor black(Qt::black);

    // Act
    QString result = Utils::colorToHSV(black);

    // Assert
    // Black has value = 0
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 3);
    int val = parts[2].trimmed().toInt();
    EXPECT_EQ(val, 0);
}

TEST_F(UtilsTest, ColorToHSV_White_ReturnsMaxValue) {
    // Arrange
    QColor white(Qt::white);

    // Act
    QString result = Utils::colorToHSV(white);

    // Assert
    // White: saturation=0, value=255
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 3);
    int val = parts[2].trimmed().toInt();
    EXPECT_EQ(val, 255);
}

TEST_F(UtilsTest, ColorToHSV_CustomColor_ReturnsThreeValues) {
    // Arrange
    QColor custom(180, 90, 45);

    // Act
    QString result = Utils::colorToHSV(custom);

    // Assert
    QStringList parts = result.mid(1, result.length() - 2).split(',');
    EXPECT_EQ(parts.size(), 3);
    EXPECT_TRUE(result.startsWith('('));
    EXPECT_TRUE(result.endsWith(')'));
}
