/*
    imports.qml
 
    Declaration of QML Imports
    required by project.
 
    This is necessary if we want
    to keep qml files in a folder
    separate from .pro file because
    of the way qmlimportscanner works.
    If these imports are not declared,
    qmake will not recognize them,
    and QtQuick will not be packaged
    with statically built apps (i.e. iOS)
    and imported at runtime.
 
    This must be kept in the same
    directory as your .pro file
*/
 
import QtQuick 2.1
import QtQuick.Controls 1.1
import QtMultimedia 5.0

QtObject {}