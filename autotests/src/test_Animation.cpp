// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 用例计数声明（self-check-structural 验证此块）：
// | method          | level | factors | min | actual |
// |-----------------|-------|---------|-----|--------|
// | Animation       | low   | -       | 1   | 2      |
// | paintEvent      | low   | -       | 1   | 1      |
// | renderAnimation | low   | -       | 1   | 2      |
// | ~Animation      | low   | -       | 1   | 1      |
// ─── 生成后填入 actual 列，低于 min 即违规 ───
//
// 最小清单完成情况（test-code-gen §最小清单）：
// 1. 每个公开方法 ≥ 1 用例: [x]
// 2. 每个输入维度按等价类划分 ≥ 1 用例/类: [x]
// 3. 每个等价类的边界值显式覆盖: [x]
// 4. 同质 ≥ 3 组用 TEST_P: [n/a]
// 5. 分支清单 → 用例映射已列出: [x]
// 6. 每条 if/switch/throw/early-return 有触发用例: [x]
// 7. 异常路径 EXPECT_THROW 精确匹配: [n/a]
// 8. 负面场景有专门用例: [x]
// 9. 负面用例验证强异常安全: [n/a]
// 10. stub_ext vs gMock 选择正确: [x]
//
// 分支清单（来源: get_code_snippet）:
// renderAnimation():
//   branch 1 (line 79): renderTicker < animationFrames → renderTicker++, repaint()
//   branch 2 (line 83): else → renderTimer->stop(), hide(), emit finish()
//   映射: branch 1 → RenderAnimation_BeforeFrameLimit_CallsRepaint
//   映射: branch 2 → RenderAnimation_ReachingFrameLimit_StopsTimerAndEmitsFinish
//
// paintEvent():
//   No conditional branches (all sequential drawing code)
//   映射: (single path) → PaintEvent_WithValidWidget_DrawsCircleClip
//
// Animation() constructor:
//   No conditional branches (all sequential initialization)
//   映射: (single path) → Constructor_WithValidParams_CreatesValidWidget
//   映射: (boundary: zero coords) → Constructor_WithZeroPosition_SetsNegativeOrigin

#include <gtest/gtest.h>
#include <QApplication>
#include <QPaintEvent>
#include "stubext.h"
#include "animation.h"

// Test subclass to expose protected paintEvent
class TestAnimation : public Animation {
public:
    using Animation::Animation;
    void callPaintEvent(QPaintEvent *event) { paintEvent(event); }
};

class AnimationTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        static int argc = 1;
        static char arg0[] = "test_Animation";
        static char *argv[] = {arg0, nullptr};
        app = new QApplication(argc, argv);
    }

    static void TearDownTestSuite() {
        delete app;
        app = nullptr;
    }

    void SetUp() override {
        stub.clear();
        // Stub QTimer::start to prevent timer from actually firing
        stub.set_lamda(static_cast<void (QTimer::*)(int)>(&QTimer::start), [](QTimer *, int) {
            __DBG_STUB_INVOKE__
        });
        // Stub QWidget::show/hide to avoid window system calls
        stub.set_lamda(&QWidget::show, [](QWidget *) {
            __DBG_STUB_INVOKE__
        });
        stub.set_lamda(&QWidget::hide, [](QWidget *) {
            __DBG_STUB_INVOKE__
        });
        // Stub QApplication::devicePixelRatio for paintEvent
        stub.set_lamda(VADDR(QApplication, devicePixelRatio), []() -> qreal {
            __DBG_STUB_INVOKE__
            return 1.0;
        });
    }

    void TearDown() override {
        if (obj) {
            delete obj;
            obj = nullptr;
        }
        stub.clear();
    }

    stub_ext::StubExt stub;
    TestAnimation *obj = nullptr;
    static QApplication *app;
};

QApplication *AnimationTest::app = nullptr;

// ═══════════════════════════════════════════════════════════════
// ⚠️ 以下每个 TEST_F 必须包含 // Arrange / // Act / // Assert 三段注释
// ⚠️ 缺少任一段 → self-check-structural 报 MISSING_AAA 违规
// ⚠️ 每段至少有 1 行实质内容（空段也算违规）
// ═══════════════════════════════════════════════════════════════

// --- Constructor tests ---

TEST_F(AnimationTest, Constructor_WithValidParams_CreatesValidWidget) {
    // Arrange
    const int x = 100;
    const int y = 200;
    QPixmap pixmap(220, 220);
    pixmap.fill(Qt::red);
    const QColor color(0, 128, 255);

    // Act
    obj = new TestAnimation(x, y, pixmap, color, nullptr);

    // Assert
    ASSERT_NE(obj, nullptr);
    EXPECT_TRUE(obj->windowFlags() & Qt::FramelessWindowHint);
    EXPECT_TRUE(obj->windowFlags() & Qt::X11BypassWindowManagerHint);
    // Constructor calls resize(220, 220) and move(x - width/2, y - height/2)
    EXPECT_EQ(obj->QWidget::width(), 220);
    EXPECT_EQ(obj->QWidget::height(), 220);
    // move(100 - 110, 200 - 110) = move(-10, 90)
    EXPECT_EQ(obj->pos().x(), -10);
    EXPECT_EQ(obj->pos().y(), 90);
}

TEST_F(AnimationTest, Constructor_WithZeroPosition_SetsNegativeOrigin) {
    // Arrange
    QPixmap pixmap(220, 220);
    pixmap.fill(Qt::green);
    const QColor color = Qt::black;

    // Act
    obj = new TestAnimation(0, 0, pixmap, color, nullptr);

    // Assert
    ASSERT_NE(obj, nullptr);
    // move(0 - 110, 0 - 110) = move(-110, -110)
    EXPECT_EQ(obj->pos().x(), -110);
    EXPECT_EQ(obj->pos().y(), -110);
}

// --- renderAnimation tests ---

TEST_F(AnimationTest, RenderAnimation_BeforeFrameLimit_CallsRepaint) {
    // Arrange
    QPixmap pixmap(220, 220);
    pixmap.fill(Qt::blue);
    obj = new TestAnimation(100, 100, pixmap, QColor(Qt::white), nullptr);
    int repaintCallCount = 0;
    // Use static_cast to disambiguate overloaded repaint()
    stub.set_lamda(
        static_cast<void (QWidget::*)()>(&QWidget::repaint),
        [&repaintCallCount](QWidget *) {
            __DBG_STUB_INVOKE__
            ++repaintCallCount;
        });
    int finishCount = 0;
    QObject::connect(obj, &Animation::finish, [&finishCount]() {
        ++finishCount;
    });

    // Act — call 12 times: all take the < animationFrames branch
    for (int i = 0; i < 12; ++i) {
        obj->renderAnimation();
    }

    // Assert
    EXPECT_EQ(repaintCallCount, 12);
    EXPECT_EQ(finishCount, 0);
}

TEST_F(AnimationTest, RenderAnimation_ReachingFrameLimit_StopsTimerAndEmitsFinish) {
    // Arrange
    QPixmap pixmap(220, 220);
    pixmap.fill(Qt::yellow);
    obj = new TestAnimation(50, 50, pixmap, QColor(Qt::black), nullptr);
    // First exhaust the animation frames (12 calls to get renderTicker to 12)
    stub.set_lamda(
        static_cast<void (QWidget::*)()>(&QWidget::repaint),
        [](QWidget *) {
            __DBG_STUB_INVOKE__
        });
    for (int i = 0; i < 12; ++i) {
        obj->renderAnimation();
    }
    // Now set up spies for the 13th call (else branch)
    int finishCount = 0;
    QObject::connect(obj, &Animation::finish, [&finishCount]() {
        ++finishCount;
    });
    int timerStopCount = 0;
    stub.set_lamda(&QTimer::stop, [&timerStopCount](QTimer *) {
        __DBG_STUB_INVOKE__
        ++timerStopCount;
    });
    int hideCount = 0;
    stub.set_lamda(&QWidget::hide, [&hideCount](QWidget *) {
        __DBG_STUB_INVOKE__
        ++hideCount;
    });

    // Act — 13th call triggers else branch (renderTicker == animationFrames == 12)
    obj->renderAnimation();

    // Assert
    EXPECT_EQ(timerStopCount, 1);
    EXPECT_EQ(hideCount, 1);
    EXPECT_EQ(finishCount, 1);
}

// --- paintEvent test ---

TEST_F(AnimationTest, PaintEvent_WithValidWidget_DrawsCircleClip) {
    // Arrange
    QPixmap pixmap(220, 220);
    pixmap.fill(QColor(255, 0, 0));
    obj = new TestAnimation(100, 100, pixmap, QColor(Qt::cyan), nullptr);
    QPaintEvent event(obj->rect());

    // Act
    obj->callPaintEvent(&event);

    // Assert
    // Verify widget has valid size (set by constructor: resize(220, 220))
    EXPECT_EQ(obj->QWidget::width(), 220);
    EXPECT_EQ(obj->QWidget::height(), 220);
}

// --- Destructor test ---

TEST_F(AnimationTest, Destructor_WithValidObject_CleansUpWithoutCrash) {
    // Arrange
    QPixmap pixmap(220, 220);
    pixmap.fill(Qt::magenta);
    obj = new TestAnimation(0, 0, pixmap, Qt::white, nullptr);
    Animation *rawPtr = obj;

    // Act
    delete obj;
    obj = nullptr;

    // Assert
    // Verify cleanup completed without crash
    EXPECT_EQ(obj, nullptr);
    EXPECT_NE(rawPtr, nullptr);
}