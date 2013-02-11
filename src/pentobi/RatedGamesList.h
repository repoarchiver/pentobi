//-----------------------------------------------------------------------------
/** @file pentobi/RatedGamesList.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RATED_GAMES_LIST
#define PENTOBI_RATED_GAMES_LIST

#include <QTableView>
#include "RatingHistory.h"

class QStandardItemModel;

//-----------------------------------------------------------------------------

class RatedGamesList
    : public QTableView
{
    Q_OBJECT

public:
    RatedGamesList(QWidget* parent = nullptr);

    void updateContent(Variant variant, const RatingHistory& history);

signals:
    void openRatedGame(unsigned n);

protected:
    void focusInEvent(QFocusEvent* event);

    void focusOutEvent(QFocusEvent* event);

    void keyPressEvent(QKeyEvent* event);

private:
    QStandardItemModel* m_model;

private slots:
    void activateGame(const QModelIndex& index);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RATED_GAMES_LIST
