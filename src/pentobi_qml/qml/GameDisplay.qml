//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/GameDisplay.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.1
import QtQuick.Window 2.1
import "GameDisplay.js" as Logic

Item
{
    id: root

    property var pickedPiece: null

    // Location of the piece selector (for centering items like the busy cursor
    // on it)
    readonly property real pieceAreaSize: pieceSelector.pieceAreaSize
    readonly property real pieceSelectorY: scorePiecePanel.y + pieceSelector.y
    readonly property real pieceSelectorHeight: pieceSelector.height

    property var _pieces0
    property var _pieces1
    property var _pieces2
    property var _pieces3
    property bool _isTrigon: boardModel.gameVariant.indexOf("trigon") >= 0

    signal play(var pieceModel, point gameCoord)

    function createPieces() { Logic.createPieces() }
    function showPieces(color) { pieceSelector.showColor(color) }
    function showMoveHint(move) { Logic.showMoveHint(move) }

    onWidthChanged: pickedPiece = null
    onHeightChanged: pickedPiece = null

    Background { anchors.fill: root }

    Board {
        id: board

        gameVariant: boardModel.gameVariant
        width: Math.min(root.width, 0.65 * root.height)
        height: width
        anchors.top: root.top
        anchors.horizontalCenter: root.horizontalCenter
    }

    Column {
        id: scorePiecePanel

        anchors.top: board.bottom
        anchors.horizontalCenter: root.horizontalCenter

        ScoreDisplay {
            id: scoreDisplay

            gameVariant: boardModel.gameVariant
            points0: boardModel.points0
            points1: boardModel.points1
            points2: boardModel.points2
            points3: boardModel.points3
            hasMoves0: boardModel.hasMoves0
            hasMoves1: boardModel.hasMoves1
            hasMoves2: boardModel.hasMoves2
            hasMoves3: boardModel.hasMoves3
            toPlay: boardModel.isGameOver ? -1 : boardModel.toPlay
            height: 0.06 * board.width
            pointSize: 0.03 * board.width
            anchors.horizontalCenter: scorePiecePanel.horizontalCenter
        }

        PieceSelector {
            id: pieceSelector

            pieces0: _pieces0
            pieces1: _pieces1
            pieces2: _pieces2
            pieces3: _pieces3
            toPlay: boardModel.toPlay
            nuColors: boardModel.nuColors
            width: board.width

            // Make piece size such that 7 pieces are visible below the board,
            // so that they are smaller than the pieces on the board but not too
            // small. Take into account that the effective visible width of the
            // piece list is only 90% of the piece selector width because of the
            // images indicating that the list is flickable. Ensure a minimum
            // piece size of 9 mm
            pieceAreaSize: Math.floor(Math.max(0.9 * board.width / 7,
                                      9 * Screen.pixelDensity))

            rows: {
                if (pieceAreaSize == 0)
                    return 1
                var rows = Math.floor((root.height - board.height - scoreDisplay.height) / pieceAreaSize)
                if (rows == 0)
                    return 1
                if (_isTrigon && rows >= 4)
                    return 4
                if (rows >= 3)
                    return 3
                return rows
            }

            height: rows * pieceAreaSize
            onPiecePicked: Logic.pickPiece(piece)
        }
    }

    PieceManipulator {
        id: pieceManipulator

        function isLegal() {
            if (pickedPiece === null)
                return false
            var boardCoord = parent.mapToItem(board,
                                              x + width / 2, y + height / 2)
            var coord = board.mapToGame(boardCoord.x, boardCoord.y)
            return boardModel.isLegalPos(pickedPiece.pieceModel, coord)
        }

        width: 0.6 * board.width
        z: 1
        visible: pickedPiece !== null
        pieceModel: pickedPiece !== null ? pickedPiece.pieceModel : null
        drag {
            minimumX: -width / 2; maximumX: root.width - width / 2
            minimumY: -height / 2; maximumY: root.height - height / 2
        }
        onPiecePlayed: {
            var pos = mapToItem(board, width / 2, height / 2)
            if (board.contains(Qt.point(pos.x, pos.y))) {
                if (isLegal()) {
                    var boardCoord =
                            pieceManipulator.mapToItem(board, pickedPiece.x, pickedPiece.y)
                    var coord = board.mapToGame(boardCoord.x, boardCoord.y)
                    play(pickedPiece.pieceModel, coord)
                    pickedPiece = null
                }
            }
            else
                pickedPiece = null
        }
        onXChanged: legal = isLegal()
        onYChanged: legal = isLegal()
    }
}
