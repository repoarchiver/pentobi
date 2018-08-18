//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceGembloQ.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece for GembloQ. See PieceClassic for comments.
Item
{
    id: root

    property QtObject pieceModel
    property var color:
        switch (this.pieceModel.color) {
        case 0: return color0
        case 1: return color1
        case 2: return color2
        case 3: return color3
        }
    property Item parentUnplayed
    property real gridWidth: board.gridWidth
    property real gridHeight: board.gridHeight
    property string imageName:
        "image://pentobi/quarter-square/" + color[0] + "/" + color[1] + "/" +
        color[2]
    // Avoid fractional sizes for square piece elements
    property real scaleUnplayed:
        parentUnplayed ? Math.floor(0.08 * 2 * parentUnplayed.width)
                         / (2 * gridWidth) : 0
    property string imageNameBottom:
        "image://pentobi/quarter-square-bottom/" + color[0] + "/" + color[1] +
        "/" + color[2]
    property bool flippedX: Math.abs(flipX.angle - 180) < 90
    property bool flippedY: flipY.angle > 90
    property real pieceAngle: {
        if (! flippedY && ! flippedX) return rotation
        if (! flippedY && flippedX) return rotation + 90
        if (flippedX) return rotation + 180
        return rotation + 270
    }
    property real isSmall: scale < 0.5 ? 1 : 0
    property real imageOpacity0: imageOpacity(pieceAngle, 0) * (1 - isSmall)
    property real imageOpacity90: imageOpacity(pieceAngle, 90) * (1 - isSmall)
    property real imageOpacity180: imageOpacity(pieceAngle, 180) * (1 - isSmall)
    property real imageOpacity270: imageOpacity(pieceAngle, 270) * (1 - isSmall)
    property real imageOpacitySmall0: imageOpacity(pieceAngle, 0) * isSmall
    property real imageOpacitySmall90: imageOpacity(pieceAngle, 90) * isSmall
    property real imageOpacitySmall180: imageOpacity(pieceAngle, 180) * isSmall
    property real imageOpacitySmall270: imageOpacity(pieceAngle, 270) * isSmall

    function imageOpacity(pieceAngle, imgAngle) {
        var angle = (pieceAngle - imgAngle + 360) % 360
        if (angle <= 90) return 0
        if (angle <= 180) return -Math.cos(angle * Math.PI / 180)
        if (angle <= 270) return 1
        return -Math.sin(angle * Math.PI / 180)
    }

    transform: [
        Rotation {
            id: flipX

            axis { x: 1; y: 0; z: 0 }
        },
        Rotation {
            id: flipY

            axis { x: 0; y: 1; z: 0 }
        }
    ]

    Repeater {
        model: pieceModel.elements

        QuarterSquare {
            x: (modelData.x - pieceModel.center.x) * gridWidth
            y: (modelData.y - pieceModel.center.y) * gridHeight
            pointType: {
                var t = modelData.x
                if (modelData.y % 2 != 0) t += 2
                return (t % 4 + 4) % 4
            }
        }
    }
    Rectangle {
        opacity: moveMarking == "last_dot" && pieceModel.isLastMove ? 0.5 : 0
        color: gameModel.showVariations && ! gameModel.isMainVar ? "transparent" : border.color
        border { width: 0.2 * width; color: root.color[3] }
        width: 0.45 * gridHeight
        height: width
        radius: width / 2
        x: {
            var labelX = pieceModel.labelPos.x
            var labelY = pieceModel.labelPos.y
            var x = (labelX - pieceModel.center.x + 0.5) * gridWidth - width / 2
            if (labelX % 2 == 0) x -= gridWidth / 2
            else x += gridWidth / 2
            return x
        }
        y: {
            var labelX = pieceModel.labelPos.x
            var labelY = pieceModel.labelPos.y
            var y = (labelY - pieceModel.center.y + 0.5) * gridHeight - height / 2
            if (labelY % 2 == 0) y -= gridHeight / 2
            else y += gridHeight / 2
            return y
        }
        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    Text {
        text: moveMarking == "all_number"
              || (moveMarking == "last_number" && pieceModel.isLastMove) ?
                  pieceModel.moveLabel : ""
        opacity: text === "" ? 0 : 1
        color: root.color[3]
        font.pixelSize: 0.7 * gridHeight
        width: 0
        height: 0
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        x: {
            var labelX = pieceModel.labelPos.x
            var labelY = pieceModel.labelPos.y
            var x = (labelX - pieceModel.center.x + 0.5) * gridWidth
            if (labelX % 2 == 0) x -= gridWidth / 2
            else x += gridWidth / 2
            return x
        }
        y: {
            var labelX = pieceModel.labelPos.x
            var labelY = pieceModel.labelPos.y
            var y = (labelY - pieceModel.center.y + 0.5) * gridHeight
            if (labelY % 2 == 0) y -= gridHeight / 2
            else y += gridHeight / 2
            return y
        }
        transform: [
            Rotation {
                axis { x: 0; y: 1; z: 0 }
                angle: -flipY.angle
            },
            Rotation {
                axis { x: 1; y: 0; z: 0 }
                angle: -flipX.angle
            },
            Rotation {
                angle: -root.rotation
            }
        ]
        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    StateGroup {
        state: pieceModel.state

        states: [
            State {
                name: "rot90"
                PropertyChanges { target: root; rotation: 90 }
            },
            State {
                name: "rot180"
                PropertyChanges { target: root; rotation: 180 }
            },
            State {
                name: "rot270"
                PropertyChanges { target: root; rotation: 270 }
            },
            State {
                name: "flip"
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot90Flip"
                PropertyChanges { target: root; rotation: 90 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot180Flip"
                PropertyChanges { target: root; rotation: 180 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot270Flip"
                PropertyChanges { target: root; rotation: 270 }
                PropertyChanges { target: flipX; angle: 180 }
            }
        ]

        transitions: [
            Transition {
                from: ",rot180Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot90,rot270Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot180,flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot270,rot90Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                enabled: enableAnimations

                PieceRotationAnimation { }
                PieceFlipAnimation { target: flipX }
            }
        ]
    }

    states: [
        State {
            name: "picked"
            when: root === pickedPiece

            ParentChange {
                target: root
                parent: pieceManipulator
                x: pieceManipulator.width / 2
                y: pieceManipulator.height / 2
            }
        },
        State {
            name: "played"
            when: pieceModel.isPlayed

            ParentChange {
                target: root
                parent: board.grabImageTarget
                x: board.mapFromGameX(pieceModel.gameCoord.x) - board.grabImageTarget.x
                y: board.mapFromGameY(pieceModel.gameCoord.y) - board.grabImageTarget.y
            }
        },
        State {
            name: "unplayed"
            when: parentUnplayed != null

            PropertyChanges {
                target: root
                scale: scaleUnplayed
            }
            ParentChange {
                target: root
                parent: parentUnplayed
                x: parentUnplayed.width / 2
                y: parentUnplayed.height / 2
            }
        }
    ]
    transitions:
        Transition {
            from: "unplayed,picked,played"; to: from
            enabled: enableAnimations

            SequentialAnimation {
                PropertyAction {
                    target: parentUnplayed.parent
                    property: "z"; value: 1
                }
                ParentAnimation {
                    via: isDesktop ? null : gameDisplay

                    NumberAnimation {
                        properties: "x,y,scale"
                        duration: animationDurationMove
                        easing.type: Easing.InOutSine
                    }
                }
                PropertyAction {
                    target: parentUnplayed.parent
                    property: "z"; value: 0
                }
            }
    }
}