
#用 pkg-config 机制增加第三方依赖库到 LIBS 和 QMAKE_CXXFLAGS  
PKG_CONFIG = $$myPkgConfigExecutable()
message("PKG_CONFIG:$$PKG_CONFIG")

pkgvar = MYPKGCONFIG
libvar = LIBS
for(PKGCONFIG_LIB, $$list($$unique($$pkgvar))) {

    # don't proceed if the .pro asks for a package we don't have!
    !myPackagesExist($$PKGCONFIG_LIB): error("$$PKGCONFIG_LIB development package not found")

    PKGCONFIG_CFLAGS = $$system($$PKG_CONFIG --cflags $$PKGCONFIG_LIB)
    #message("PKGCONFIG_LIB:$$PKGCONFIG_LIB PKGCONFIG_CFLAGS:$$PKGCONFIG_CFLAGS")
    PKGCONFIG_INCLUDEPATH = $$find(PKGCONFIG_CFLAGS, ^-I.*)
    PKGCONFIG_INCLUDEPATH ~= s/^-I(.*)/\\1/g

    PKGCONFIG_DEFINES = $$find(PKGCONFIG_CFLAGS, ^-D.*)
    PKGCONFIG_DEFINES ~= s/^-D(.*)/\\1/g

    PKGCONFIG_CFLAGS ~= s/^-[ID].*//g

    INCLUDEPATH *= $$PKGCONFIG_INCLUDEPATH
    DEFINES *= $$PKGCONFIG_DEFINES

    QMAKE_CXXFLAGS *= $$PKGCONFIG_CFLAGS
    QMAKE_CFLAGS *= $$PKGCONFIG_CFLAGS
    $$libvar += $$system($$PKG_CONFIG --libs $$PKGCONFIG_LIB)
}

LIBS += $$OPENCV_LIBRARY

equals(QXMPP_USE_VPX, 1) : myPackagesExist(vpx){
    android {
        LIBS *= -lcpu-features
    }
}

!msvc{
    message("MYPKGCONFIG:$$MYPKGCONFIG")
    message("DEFINES:$$DEFINES")
    message("INCLUDEPATH:$$INCLUDEPATH")
}
message("LIBS:$$LIBS")
