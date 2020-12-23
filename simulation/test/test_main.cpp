// from https://github.com/cinder/Cinder/blob/master/test/unit/src/TestMain.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#ifdef _MSC_VER

    #include <windows.h> // Provide typedefs for WinMain declaration

    INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        PSTR lpCmdLine, INT nCmdShow) {
        return Catch::Session().run();
    }

#endif