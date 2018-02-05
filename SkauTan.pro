#-------------------------------------------------
#
# Project created by QtCreator 2017-12-25T05:47:41
#
#-------------------------------------------------

QT       += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SkauTan
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


# Turn on warnings-as-errors and extra warnings
msvc {
	QMAKE_CXXFLAGS += /WX
}
gcc | clang {
	CONFIG += warn_on
	QMAKE_CXXFLAGS += -Werror
	QMAKE_CXXFLAGS += -Wextra
	QMAKE_CXXFLAGS += -Wunknown-pragmas -Wundef
	QMAKE_CXXFLAGS += -Wold-style-cast
	QMAKE_CXXFLAGS += -Wdisabled-optimization -Wstrict-overflow=4
	QMAKE_CXXFLAGS += -Winit-self -Wpointer-arith
	QMAKE_CXXFLAGS += -Wlogical-op

	# Override the C and C++ targets to selectively replace -I with -isystem for include paths
	QMAKE_RUN_CC      = $(CC) -o $obj -c $(CFLAGS) $(subst -I/usr/include,-isystem /usr/include,$(INCPATH)) $src
	QMAKE_RUN_CC_IMP  = $(CC) -o $@ -c $(CFLAGS) $(subst -I/usr/include,-isystem /usr/include,$(INCPATH)) $<
	QMAKE_RUN_CXX     = $(CXX) -o $obj -c $(CXXFLAGS) $(subst -I/usr/include,-isystem /usr/include,$(INCPATH)) $src
	QMAKE_RUN_CXX_IMP = $(CXX) -o $@ -c $(CXXFLAGS) $(subst -I/usr/include,-isystem /usr/include,$(INCPATH)) $<
}


SOURCES += \
	main.cpp \
	PlayerWindow.cpp \
	Playlist.cpp \
	PlaylistItemSong.cpp \
	Song.cpp \
	SongDatabase.cpp \
	DlgSongs.cpp \
	SongModel.cpp \
	MetadataScanner.cpp \
	HashCalculator.cpp \
	PlaylistItemModel.cpp

HEADERS += \
	PlayerWindow.h \
	IPlaylistItem.h \
	Playlist.h \
	PlaylistItemSong.h \
	Song.h \
	SongDatabase.h \
	DlgSongs.h \
	SongModel.h \
	MetadataScanner.h \
	HashCalculator.h \
	PlaylistItemModel.h

FORMS += \
	PlayerWindow.ui \
	DlgSongs.ui