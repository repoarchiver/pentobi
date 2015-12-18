//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardConst.h"

#include <algorithm>
#include "PieceTransformsClassic.h"
#include "PieceTransformsTrigon.h"
#include "SymmetricPoints.h"
#include "libboardgame_base/Transform.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {

using libboardgame_base::PointTransfRot180;
using libboardgame_base::Transform;
using libboardgame_util::split;
using libboardgame_util::to_lower;
using libboardgame_util::trim;

//-----------------------------------------------------------------------------

namespace {

const bool log_move_creation = false;

// Sort points using the ordering used in blksgf files (switches the direction
// of the y axis!)
inline void sort_piece_points(PiecePoints& points)
{
    auto check = [&](unsigned short a, unsigned short b)
    {
        if ((points[a].y == points[b].y && points[a].x > points[b].x)
                || points[a].y < points[b].y)
            swap(points[a], points[b]);
    };
    // Minimal number of necessary comparisons with sorting networks
    auto size = points.size();
    switch (size)
    {
    case 7:
        check(1, 2);
        check(3, 4);
        check(5, 6);
        check(0, 2);
        check(3, 5);
        check(4, 6);
        check(0, 1);
        check(4, 5);
        check(2, 6);
        check(0, 4);
        check(1, 5);
        check(0, 3);
        check(2, 5);
        check(1, 3);
        check(2, 4);
        check(2, 3);
        break;
    case 6:
        check(1, 2);
        check(4, 5);
        check(0, 2);
        check(3, 5);
        check(0, 1);
        check(3, 4);
        check(2, 5);
        check(0, 3);
        check(1, 4);
        check(2, 4);
        check(1, 3);
        check(2, 3);
        break;
    case 5:
        check(0, 1);
        check(3, 4);
        check(2, 4);
        check(2, 3);
        check(1, 4);
        check(0, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 4:
        check(0, 1);
        check(2, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 3:
        check(1, 2);
        check(0, 2);
        check(0, 1);
        break;
    case 2:
        check(0, 1);
        break;
    default:
        LIBBOARDGAME_ASSERT(size == 1);
    }
}

vector<PieceInfo> create_pieces_classic(const Geometry& geo,
                                        const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    // Define the 21 standard pieces. The piece names are the standard names as
    // in http://blokusstrategy.com/?p=48. The default orientation is chosen
    // such that it resembles the letter.
    pieces.reserve(21);
    pieces.emplace_back("V5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2), CoordPoint(1, 0),
                                     CoordPoint(2, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Z5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("N",
                        PiecePoints{ CoordPoint(-1, 1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2)},
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("T5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("U",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, CoordPoint(0, 0));
    return pieces;
}

vector<PieceInfo> create_pieces_junior(const Geometry& geo,
                                       const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    pieces.reserve(12);
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, CoordPoint(0, 0));
    return pieces;
}

vector<PieceInfo> create_pieces_trigon(const Geometry& geo,
                                       const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    // Define the 22 standard Trigon pieces. The piece names are similar to one
    // of the possible notations from the thread "Trigon book: how to play, how
    // to win" from August 2010 in the Blokus forums
    // http://forum.blokus.refreshed.be/viewtopic.php?f=2&t=2539#p9867
    // apart from that the smallest pieces are named '2' and '1' like in
    // Classic to avoid to many pieces with letter 'I' and that numbers are
    // only used if there is more than one piece with the same letter.
    pieces.reserve(22);
    pieces.emplace_back("I6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("V",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("S",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("P6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(2, 1), CoordPoint(1, 2) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(2, 0), CoordPoint(3, 0) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("A6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(0, 1), CoordPoint(2, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("G",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(-1, 1),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(-1, 1),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("C5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(2, 1) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("P5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("C4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("A4",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, CoordPoint(0, 0));
    return pieces;
}

vector<PieceInfo> create_pieces_nexos(const Geometry& geo,
                                      const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    pieces.reserve(24);
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, -3), CoordPoint(0, -2),
                                     CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, -3), CoordPoint(0, -2),
                                     CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3)},
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("N",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3)},
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("V4",
                        PiecePoints{ CoordPoint(-3, 0), CoordPoint(-2, 0),
                                     CoordPoint(-1, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2), CoordPoint(0, -3) },
                        geo, transforms, CoordPoint(-1, 0));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2)},
                        geo, transforms, CoordPoint(-1, 0));
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, -2), CoordPoint(0, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 2) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("E",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(-1, 2)},
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("U4",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(2, -1) },
                        geo, transforms, CoordPoint(-1, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1)},
                        geo, transforms, CoordPoint(0, -1));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(1, -2), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(0, 1)},
                        geo, transforms, CoordPoint(0, -1));
    pieces.emplace_back("H",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(2, 1)},
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("J",
                        PiecePoints{ CoordPoint(0, -3), CoordPoint(0, -2),
                                     CoordPoint(0, -1), CoordPoint(-1, 0),
                                     CoordPoint(-2, -1) },
                        geo, transforms, CoordPoint(-1, 0));
    pieces.emplace_back("G",
                        PiecePoints{ CoordPoint(2, -1), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2)},
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(1, 0), CoordPoint(2, 1),
                                     CoordPoint(0, 1), CoordPoint(1, 2)},
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("L3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("T3",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("Z3",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 2) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("U3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(2, -1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("V2",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, -1) },
                        geo, transforms, CoordPoint(-1, 0));
    pieces.emplace_back("I2",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(1, 0));
    return pieces;
}

} // namespace

//-----------------------------------------------------------------------------

Marker BoardConst::s_marker;

Grid<array<ArrayList<Point, PrecompMoves::adj_status_nu_adj>,
                 PrecompMoves::nu_adj_status>>
    BoardConst::s_adj_status;

BoardConst::BoardConst(BoardType board_type, PieceSet piece_set)
    : m_board_type(board_type),
      m_piece_set(piece_set),
      m_geo(libpentobi_base::get_geometry(board_type))
{
    switch (board_type)
    {
    case BoardType::classic:
        LIBBOARDGAME_ASSERT(piece_set == PieceSet::classic);
        m_transforms.reset(new PieceTransformsClassic);
        m_pieces = create_pieces_classic(m_geo, *m_transforms);
        m_nu_moves = Move::onboard_moves_classic + 1;
        break;
    case BoardType::trigon:
        LIBBOARDGAME_ASSERT(piece_set == PieceSet::trigon);
        m_transforms.reset(new PieceTransformsTrigon);
        m_pieces = create_pieces_trigon(m_geo, *m_transforms);
        m_nu_moves = Move::onboard_moves_trigon + 1;
        break;
    case BoardType::trigon_3:
        LIBBOARDGAME_ASSERT(piece_set == PieceSet::trigon);
        m_transforms.reset(new PieceTransformsTrigon);
        m_pieces = create_pieces_trigon(m_geo, *m_transforms);
        m_nu_moves = Move::onboard_moves_trigon_3 + 1;
        break;
    case BoardType::duo:
        m_transforms.reset(new PieceTransformsClassic);
        if (piece_set == PieceSet::classic)
        {
            m_pieces = create_pieces_classic(m_geo, *m_transforms);
            m_nu_moves = Move::onboard_moves_duo + 1;
        }
        else
        {
            LIBBOARDGAME_ASSERT(piece_set == PieceSet::junior);
            m_pieces = create_pieces_junior(m_geo, *m_transforms);
            m_nu_moves = Move::onboard_moves_junior + 1;
        }
        break;
    case BoardType::nexos:
        LIBBOARDGAME_ASSERT(piece_set == PieceSet::nexos);
        m_transforms.reset(new PieceTransformsClassic);
        m_pieces = create_pieces_nexos(m_geo, *m_transforms);
        m_nu_moves = Move::onboard_moves_nexos + 1;
        break;
    }
    m_move_info.reset(new MoveInfo[m_nu_moves]);
    m_move_info_ext.reset(new MoveInfoExt[m_nu_moves]);
    m_move_info_ext_2.reset(new MoveInfoExt2[m_nu_moves]);
    m_nu_pieces = static_cast<Piece::IntType>(m_pieces.size());
    init_adj_status();
    auto width = m_geo.get_width();
    auto height = m_geo.get_height();
    for (Point p : m_geo)
        m_compare_val[p] =
                (height - m_geo.get_y(p) - 1) * width + m_geo.get_x(p);
    create_moves();
    m_max_piece_size = 0;
    for (const PieceInfo& piece : m_pieces)
        m_max_piece_size = max(m_max_piece_size, piece.get_size());
    switch (piece_set)
    {
    case PieceSet::classic:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        break;
    case PieceSet::junior:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 12);
        break;
    case PieceSet::trigon:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 22);
        break;
    case PieceSet::nexos:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 24);
        break;
    }
    if (board_type == BoardType::duo || board_type == BoardType::trigon)
        init_symmetry_info();
}

void BoardConst::create_move(unsigned& moves_created, Piece piece,
                             const MovePoints& points, Point label_pos)
{
    Move mv(static_cast<Move::IntType>(moves_created));
    LIBBOARDGAME_ASSERT(moves_created <= m_nu_moves);
    auto& info = m_move_info[moves_created];
    auto& info_ext = m_move_info_ext[moves_created];
    auto& info_ext_2 = m_move_info_ext_2[moves_created];
    ++moves_created;
    info = MoveInfo(piece, points);
    auto begin = info.begin();
    auto end = info.end();
    s_marker.clear();
    for (auto i = begin; i != end; ++i)
        s_marker.set(*i);
    for (auto i = begin; i != end; ++i)
        for (unsigned j = 0; j < PrecompMoves::nu_adj_status; ++j)
        {
            bool is_compatible_with_adj_status = true;
            for (Point p_adj : s_adj_status[*i][j])
                if (s_marker[p_adj])
                {
                    is_compatible_with_adj_status = false;
                    break;
                }
            if (is_compatible_with_adj_status)
                (*m_full_move_table)[j][piece][*i].push_back(mv);
        }
    Point* p = info_ext.points;
    for (auto i = begin; i != end; ++i)
        for (Point j : m_geo.get_adj(*i))
            if (! s_marker[j])
            {
                s_marker.set(j);
                *(p++) = j;
            }
    info_ext.size_adj_points = static_cast<uint_least8_t>(p - info_ext.points);
    for (auto i = begin; i != end; ++i)
        for (Point j : m_geo.get_diag(*i))
            if (! s_marker[j])
            {
                s_marker.set(j);
                *(p++) = j;
            }
    info_ext.size_attach_points =
            static_cast<uint_least8_t>(p - info_ext.end_adj());
    info_ext_2.label_pos = label_pos;
    info_ext_2.breaks_symmetry = false;
    info_ext_2.symmetric_move = Move::null();
    m_nu_attach_points[piece] =
        max(m_nu_attach_points[piece],
            static_cast<unsigned>(info_ext.size_attach_points));
    if (log_move_creation)
    {
        Grid<char> grid;
        grid.fill('.', m_geo);
        for (Point p : info)
            grid[p] = 'O';
        for (auto i = info_ext.begin_adj(); i != info_ext.end_adj(); ++i)
            grid[*i] = '+';
        for (auto i = info_ext.begin_attach(); i != info_ext.end_attach(); ++i)
            grid[*i] = '*';
        LIBBOARDGAME_LOG("Move ", mv.to_int(), ":\n", grid.to_string(m_geo));
    }
}

void BoardConst::create_moves()
{
    // Unused move infos for Move::null()
    LIBBOARDGAME_ASSERT(Move::null().to_int() == 0);
    unsigned moves_created = 1;
    m_full_move_table.reset(new FullMoveTable);
    for (Piece::IntType i = 0; i < m_nu_pieces; ++i)
        create_moves(moves_created, Piece(i));
    LIBBOARDGAME_ASSERT(moves_created == m_nu_moves);
    unsigned n = 0;
    for (Point p : m_geo)
        for (unsigned i = 0; i < PrecompMoves::nu_adj_status; ++i)
            for (Piece::IntType j = 0; j < m_nu_pieces; ++j)
            {
                Piece piece(j);
                auto& list = (*m_full_move_table)[i][piece][p];
                auto begin = n;
                for (auto mv : list)
                    m_precomp_moves.set_move(n++, mv);
                m_precomp_moves.set_list_range(p, i, piece, begin, n - begin);
            }
    if (log_move_creation)
        LIBBOARDGAME_LOG("Created moves: ", moves_created, ", precomp: ", n);
    m_full_move_table.reset(nullptr); // Free space, no longer needed
}

void BoardConst::create_moves(unsigned& moves_created, Piece piece)
{
    auto& piece_info = m_pieces[piece.to_int()];
    if (log_move_creation)
        LIBBOARDGAME_LOG("Creating moves for piece ", piece_info.get_name());
    auto& transforms = piece_info.get_transforms();
    auto nu_transforms = transforms.size();
    vector<PiecePoints> transformed_points(nu_transforms);
    vector<CoordPoint> transformed_label_pos(nu_transforms);
    for (size_t i = 0; i < nu_transforms; ++i)
    {
        auto transform = transforms[i];
        transformed_points[i] = piece_info.get_points();
        transform->transform(transformed_points[i].begin(),
                             transformed_points[i].end());
        sort_piece_points(transformed_points[i]);
        transformed_label_pos[i] =
                transform->get_transformed(piece_info.get_label_pos());
    }
    auto piece_size = static_cast<MovePoints::IntType>(piece_info.get_size());
    MovePoints points;
    for (MovePoints::IntType i = 0; i < MovePoints::max_size; ++i)
        points.get_unchecked(i) = Point::null();
    points.resize(piece_size);
    // Make outer loop iterator over geometry for better memory locality
    for (Point p : m_geo)
    {
        if (log_move_creation)
            LIBBOARDGAME_LOG("Creating moves at ", m_geo.to_string(p));
        auto x = m_geo.get_x(p);
        auto y = m_geo.get_y(p);
        auto point_type = m_geo.get_point_type(x, y);
        for (size_t i = 0; i < nu_transforms; ++i)
        {
            if (log_move_creation)
            {
#if ! LIBBOARDGAME_DISABLE_LOG
                auto& transform = *transforms[i];
                LIBBOARDGAME_LOG("Transformation ", typeid(transform).name());
#endif
            }
            if (transforms[i]->get_new_point_type() != point_type)
                continue;
            bool is_onboard = true;
            for (MovePoints::IntType j = 0; j < piece_size; ++j)
            {
                auto& pp = transformed_points[i][j];
                int xx = pp.x + x;
                int yy = pp.y + y;
                if (! m_geo.is_onboard(CoordPoint(xx, yy)))
                {
                    is_onboard = false;
                    break;
                }
                points[j] = m_geo.get_point(xx, yy);
            }
            if (! is_onboard)
                continue;
            CoordPoint label_pos = transformed_label_pos[i];
            label_pos.x += x;
            label_pos.y += y;
            create_move(moves_created, piece, points,
                        m_geo.get_point(label_pos.x, label_pos.y));
        }
    }
}

Move BoardConst::from_string(const string& s) const
{
    string trimmed = to_lower(trim(s));
    if (trimmed == "null")
        return Move::null();
    vector<string> v = split(trimmed, ',');
    if (v.size() > PieceInfo::max_size)
        throw runtime_error("illegal move (too many points)");
    MovePoints points;
    for (const auto& s : v)
    {
        Point p;
        if (! m_geo.from_string(s, p))
            throw runtime_error("illegal move (invalid point)");
        points.push_back(p);
    }
    Move mv;
    if (! find_move(points, mv))
        throw runtime_error("illegal move");
    return mv;
}

const BoardConst& BoardConst::get(Variant variant)
{
    static map<BoardType, map<PieceSet, unique_ptr<BoardConst>>> board_const;
    auto board_type = libpentobi_base::get_board_type(variant);
    auto piece_set = libpentobi_base::get_piece_set(variant);
    auto& bc = board_const[board_type][piece_set];
    if (! bc)
        bc.reset(new BoardConst(board_type, piece_set));
    return *bc;
}

bool BoardConst::get_piece_by_name(const string& name, Piece& piece) const
{
    for (Piece::IntType i = 0; i < m_nu_pieces; ++i)
        if (get_piece_info(Piece(i)).get_name() == name)
        {
            piece = Piece(i);
            return true;
        }
    return false;
}

bool BoardConst::find_move(const MovePoints& points, Move& move) const
{
    MovePoints sorted_points = points;
    sort(sorted_points);
    for (Piece::IntType i = 0; i < m_pieces.size(); ++i)
    {
        Piece piece(i);
        if (get_piece_info(piece).get_size() != points.size())
            continue;
        for (auto mv : get_moves(piece, points[0]))
            if (equal(sorted_points.begin(), sorted_points.end(),
                      m_move_info[mv.to_int()].begin()))
            {
                move = mv;
                return true;
            }
    }
    return false;
}

bool BoardConst::find_move(const MovePoints& points, Piece piece,
                           Move& move) const
{
    MovePoints sorted_points = points;
    sort(sorted_points);
    for (auto mv : get_moves(piece, points[0]))
        if (equal(sorted_points.begin(), sorted_points.end(),
                  m_move_info[mv.to_int()].begin()))
        {
            move = mv;
            return true;
        }
    return false;
}

void BoardConst::init_adj_status()
{
    for (Point p : m_geo)
    {
        auto& l = m_adj_status_list[p];
        for (Point pp : m_geo.get_adj(p))
        {
            if (l.size() == PrecompMoves::adj_status_nu_adj)
                break;
            l.push_back(pp);
        }
        for (Point pp : m_geo.get_diag(p))
        {
            if (l.size() == PrecompMoves::adj_status_nu_adj)
                break;
            l.push_back(pp);
        }
        for (auto i = l.end(); i < l.begin() + PrecompMoves::adj_status_nu_adj;
             ++i)
            *i = Point::null();
    }
    array<bool, PrecompMoves::adj_status_nu_adj> forbidden;
    for (Point p : m_geo)
        init_adj_status(p, forbidden, 0);
}

void BoardConst::init_adj_status(
                       Point p,
                       array<bool, PrecompMoves::adj_status_nu_adj>& forbidden,
                       unsigned i)
{
    auto& adj_status_list = m_adj_status_list[p];
    if (i == adj_status_list.size())
    {
        unsigned index = 0;
        for (unsigned j = 0; j < i; ++j)
            if (forbidden[j])
                index |= (1 << j);
        s_adj_status[p][index].clear();
        unsigned n = 0;
        for (Point j : adj_status_list)
        {
            if (n >= i)
                return;
            if (forbidden[n])
                s_adj_status[p][index].push_back(j);
            ++n;
        }
        return;
    }
    forbidden[i] = false;
    init_adj_status(p, forbidden, i + 1);
    forbidden[i] = true;
    init_adj_status(p, forbidden, i + 1);
}

void BoardConst::init_symmetry_info()
{
    SymmetricPoints symmetric_points;
    PointTransfRot180<Point> transform;
    symmetric_points.init(m_geo, transform);
    for (unsigned i = 1; i < m_nu_moves; ++i)
    {
        const auto& info = m_move_info[i];
        auto& info_ext_2 = m_move_info_ext_2[i];
        MovePoints sym_points;
        info_ext_2.breaks_symmetry = false;
        for (Point p : info)
        {
            if (info.contains(symmetric_points[p]))
                info_ext_2.breaks_symmetry = true;
            sym_points.push_back(symmetric_points[p]);
        }
        find_move(sym_points, info.get_piece(), info_ext_2.symmetric_move);
    }
}

inline void BoardConst::sort(MovePoints& points) const
{
    auto check = [&](unsigned short a, unsigned short b)
    {
        if (m_compare_val[points[a]] > m_compare_val[points[b]])
            swap(points[a], points[b]);
    };
    // Minimal number of necessary comparisons with sorting networks
    auto size = points.size();
    switch (size)
    {
    case 7:
        check(1, 2);
        check(3, 4);
        check(5, 6);
        check(0, 2);
        check(3, 5);
        check(4, 6);
        check(0, 1);
        check(4, 5);
        check(2, 6);
        check(0, 4);
        check(1, 5);
        check(0, 3);
        check(2, 5);
        check(1, 3);
        check(2, 4);
        check(2, 3);
        break;
    case 6:
        check(1, 2);
        check(4, 5);
        check(0, 2);
        check(3, 5);
        check(0, 1);
        check(3, 4);
        check(2, 5);
        check(0, 3);
        check(1, 4);
        check(2, 4);
        check(1, 3);
        check(2, 3);
        break;
    case 5:
        check(0, 1);
        check(3, 4);
        check(2, 4);
        check(2, 3);
        check(1, 4);
        check(0, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 4:
        check(0, 1);
        check(2, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 3:
        check(1, 2);
        check(0, 2);
        check(0, 1);
        break;
    case 2:
        check(0, 1);
        break;
    default:
        LIBBOARDGAME_ASSERT(size == 1);
    }
}

string BoardConst::to_string(Move mv, bool with_piece_name) const
{
    if (mv.is_null())
        return "null";
    auto& info = get_move_info(mv);
    ostringstream s;
    if (with_piece_name)
        s << '[' << get_piece_info(info.get_piece()).get_name() << "]";
    bool is_first = true;
    for (Point p : info)
    {
        if (! is_first)
            s << ',';
        else
            is_first = false;
        s << m_geo.to_string(p);
    }
    return s.str();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
