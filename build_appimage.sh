#!/bin/bash
set -e

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

rm -rf AppDir
mkdir -p AppDir/usr/bin
mkdir -p AppDir/usr/share/applications
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps

cp build/cache-cleaner AppDir/usr/bin/

if [ -f "assets/icon.png" ]; then
    cp assets/icon.png AppDir/cache-cleaner.png
    cp assets/icon.png AppDir/usr/share/icons/hicolor/256x256/apps/cache-cleaner.png
else
    ICON_PATH=$(find /usr/share/icons -name "drive-partition.svg" | head -n 1)
    if [ -n "$ICON_PATH" ]; then
        cp "$ICON_PATH" AppDir/cache-cleaner.png
    else
        exit 1
    fi
fi

cp assets/cache-cleaner.desktop AppDir/cache-cleaner.desktop

if [ ! -f "appimagetool-x86_64.AppImage" ]; then
    wget -c "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
    chmod +x appimagetool-x86_64.AppImage
fi

ln -sf usr/bin/cache-cleaner AppDir/AppRun
chmod +x AppDir/AppRun

./appimagetool-x86_64.AppImage --appimage-extract-and-run AppDir cache-cleaner-x86_64.AppImage

echo "AppImage created: cache-cleaner-x86_64.AppImage"