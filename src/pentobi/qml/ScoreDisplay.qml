//-----------------------------------------------------------------------------
/** @file pentobi/qml/ScoreDisplay.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0

Item {
    id: root

    property int altPlayer: gameModel.altPlayer
    property real points0: gameModel.points0
    property real points1: gameModel.points1
    property real points2: gameModel.points2
    property real points3: gameModel.points3
    property real bonus0: gameModel.bonus0
    property real bonus1: gameModel.bonus1
    property real bonus2: gameModel.bonus2
    property real bonus3: gameModel.bonus3
    property bool hasMoves0: gameModel.hasMoves0
    property bool hasMoves1: gameModel.hasMoves1
    property bool hasMoves2: gameModel.hasMoves2
    property bool hasMoves3: gameModel.hasMoves3

    RowLayout {
        id: rowLayout

        width: root.width
        height: gameModel.nuPlayers === 2 && gameModel.nuColors > 2 ?
                    Math.min(root.height, 0.04 * root.width)
                  : Math.min(root.height, 0.05 * root.width)
        anchors.centerIn: parent

        Item { Layout.fillWidth: true }
        ScoreElement2 {
            id: playerScore0

            visible: gameModel.nuColors === 4 && gameModel.nuPlayers === 2
            value: points0 + points2
            isFinal: ! hasMoves0 && ! hasMoves2
            fontSize: rowLayout.height
            color1: gameDisplay.color0[0]
            color2: gameDisplay.color2[0]
        }
        Item { visible: playerScore0.visible; Layout.fillWidth: true }
        ScoreElement2 {
            id: playerScore1

            visible: gameModel.nuColors === 4 && gameModel.nuPlayers === 2
            value: points1 + points3
            isFinal: ! hasMoves1 && ! hasMoves3
            fontSize: rowLayout.height
            color1: gameDisplay.color1[0]
            color2: gameDisplay.color3[0]
        }
        Item { visible: playerScore1.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore0

            value: points0
            bonus: bonus0
            isFinal: ! hasMoves0
            fontSize: rowLayout.height
            color: color0[0]
        }
        Item { visible: colorScore0.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore1

            value: points1
            bonus: bonus1
            isFinal: ! hasMoves1
            fontSize: rowLayout.height
            color: color1[0]
        }
        Item { visible: colorScore1.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore2

            visible: gameModel.nuColors > 2
            value: points2
            bonus: bonus2
            isFinal: ! hasMoves2
            fontSize: rowLayout.height
            color: color2[0]
        }
        Item { visible: colorScore2.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore3

            visible: gameModel.nuColors > 3
                     && gameModel.gameVariant !== "classic_3"
            value: points3
            bonus: bonus3
            isFinal: ! hasMoves3
            fontSize: rowLayout.height
            color: color3[0]
        }
        Item { visible: colorScore3.visible; Layout.fillWidth: true }
        ScoreElement2 {
            id: altColorIndicator

            visible: gameModel.gameVariant === "classic_3" && hasMoves3
            value: points3
            isAltColor: true
            isFinal: ! hasMoves3
            fontSize: rowLayout.height
            color1: theme.colorGreen[0]
            color2:
                switch (altPlayer) {
                case 0: return gameDisplay.color0[0]
                case 1: return gameDisplay.color1[0]
                case 2: return gameDisplay.color2[0]
                }
        }
        Item { visible: altColorIndicator.visible; Layout.fillWidth: true }
    }
}
