//-----------------------------------------------------------------------------
/** @file pentobi/qml/DialogLoader.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Loader {
    property string url

    function get() {
        if (! item) source = url
        return item
    }
    function open() { get().open() }
}
