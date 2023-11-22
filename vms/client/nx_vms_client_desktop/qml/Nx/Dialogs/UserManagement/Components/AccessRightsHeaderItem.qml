// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

import QtQuick

import Nx
import Nx.Core
import Nx.Controls

Item
{
    id: item

    property alias icon: icon.source
    property alias text: caption.text
    property alias color: icon.color

    property int textMargin: 4

    property alias enabled: mouseArea.enabled
    property bool interactive: false

    readonly property alias hovered: mouseArea.hovered

    signal clicked()

    MouseArea
    {
        id: mouseArea

        anchors.fill: item

        hoverEnabled: item.interactive
        acceptedButtons: Qt.LeftButton

        property bool hovered: false

        onClicked:
        {
            if (!hovered)
                return

            item.clicked()
            hovered = false
        }

        // Clicks temporarily disable hover.
        // This binding will restore it when the mouse re-enters the area.
        Binding on hovered
        {
            value: item.interactive && mouseArea.containsMouse
        }

        Rectangle
        {
            id: background

            x: -1
            y: -1
            width: mouseArea.width + 2
            height: mouseArea.height + 2

            color:
            {
                if (mouseArea.hovered)
                    return ColorTheme.colors.dark12

                return item.interactive && item.enabled
                    ? ColorTheme.colors.dark11
                    : ColorTheme.colors.dark7
            }

            border.color: ColorTheme.colors.dark7
        }

        IconImage
        {
            id: icon

            y: 8
            sourceSize: Qt.size(24, 20)
            anchors.horizontalCenter: mouseArea.horizontalCenter
        }

        Text
        {
            id: caption

            anchors.horizontalCenter: mouseArea.horizontalCenter
            anchors.verticalCenter: mouseArea.top
            anchors.verticalCenterOffset: 44
            width: item.width - indent * 2

            font.pixelSize: 10
            font.weight: Font.Medium
            lineHeight: 0.9
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            textFormat: Text.PlainText
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            maximumLineCount: 2

            color: item.color

            FontMetrics
            {
                id: metrics
                font: caption.font
            }

            readonly property int indent:
            {
                if (!caption.text.includes(" "))
                    return item.textMargin

                // Ensure that short captions with space are forcefully wrapped.

                const textWidth = metrics.boundingRect(caption.text).width

                return Math.max(Math.round((mouseArea.width - textWidth) / 2 + /*tolerance*/ 1),
                    item.textMargin)
            }
        }
    }
}
