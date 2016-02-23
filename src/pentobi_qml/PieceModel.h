//-----------------------------------------------------------------------------
/** @file pentobi_qml/PieceModel.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_QML_PIECE_MODEL_H
#define PENTOBI_QML_PIECE_MODEL_H

#include <QObject>
#include <QPointF>
#include <QVariant>
#include "libpentobi_base/Board.h"

using libboardgame_base::Transform;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Piece;
using libpentobi_base::PiecePoints;

//-----------------------------------------------------------------------------

class PieceModel
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int color READ color CONSTANT)
    Q_PROPERTY(QVariantList elements READ elements CONSTANT)
    Q_PROPERTY(QVariantList junctions READ junctions CONSTANT)
    Q_PROPERTY(QVariantList junctionType READ junctionType CONSTANT)
    Q_PROPERTY(QPointF center READ center CONSTANT)
    Q_PROPERTY(QPointF labelPos READ labelPos CONSTANT)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool isPlayed READ isPlayed NOTIFY isPlayedChanged)
    Q_PROPERTY(bool isLastMove READ isLastMove NOTIFY isLastMoveChanged)
    Q_PROPERTY(QPointF gameCoord READ gameCoord NOTIFY gameCoordChanged)

public:
    static QPointF findCenter(const Board& bd, const PiecePoints& points,
                              bool isOriginDownward);

    PieceModel(QObject* parent, const Board& bd, libpentobi_base::Piece piece,
               Color c);

    int color();

    /** List of QPointF instances with coordinates of piece elements. */
    QVariantList elements();

    /** List of QPointF instances with coordinates of piece junctions.
        Only used in Nexos. */
    QVariantList junctions();

    /** List of integers determining the type of junctions.
        In Nexos, this is the type of junction in junction(). In Callisto, it
        is the information if the squares in elements() have a right and/or
        down neighbor. See implementation for the meaning of the numbers. */
    QVariantList junctionType();

    QPointF center() const;

    QPointF labelPos() const;

    QString state() const;

    bool isPlayed() const;

    bool isLastMove() const;

    QPointF gameCoord() const;

    Piece getPiece() const { return m_piece; }

    const Transform* getTransform(const QString& state) const;

    const Transform* getTransform() const { return getTransform(m_state); }

    void setState(const QString& state);

    void setTransform(const Transform* transform);

    void setIsPlayed(bool isPlayed);

    void setIsLastMove(bool isLastMove);

    void setGameCoord(QPointF gameCoord);

    Q_INVOKABLE void rotateLeft();

    Q_INVOKABLE void rotateRight();

    Q_INVOKABLE void flipAcrossX();

    Q_INVOKABLE void flipAcrossY();

signals:
    void stateChanged(QString);

    void isPlayedChanged(bool);

    void isLastMoveChanged(bool);

    void gameCoordChanged(QPointF);

private:
    const Board& m_bd;

    Color m_color;

    Piece m_piece;

    bool m_isPlayed = false;

    bool m_isLastMove = false;

    QPointF m_gameCoord;

    QPointF m_center;

    QPointF m_labelPos;

    QVariantList m_elements;

    QVariantList m_junctions;

    QVariantList m_junctionType;

    QString m_state;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_QML_PIECE_MODEL_H
