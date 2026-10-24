#!/bin/bash
set -e

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

rm -rf AppDir
mkdir -p AppDir/usr/bin
mkdir -p AppDir/usr/share/applications
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps

cp build/cache-cleaner AppDir/usr/bin/

if [ -f "assets/cache-cleaner.svg" ]; then
    cp assets/cache-cleaner.svg AppDir/cache-cleaner.svg
    cp assets/cache-cleaner.svg AppDir/usr/share/icons/hicolor/256x256/apps/cache-cleaner.svg
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