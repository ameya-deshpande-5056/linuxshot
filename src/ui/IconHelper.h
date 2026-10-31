#pragma once

#include <QIcon>
#include <QColor>
#include "annotation/AnnotationTool.h"

class IconHelper {
public:
    enum class Action {
        Save,
        Copy,
        Upload,
        Cancel,
        Undo
    };

    static QIcon actionIcon(Action action, const QColor &color = QColor("#E2E8F0"));
    static QIcon toolIcon(ToolType tool, const QColor &color = QColor("#E2E8F0"));
};
