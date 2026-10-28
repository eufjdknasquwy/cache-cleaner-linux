#!/bin/bash
set -e

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

rm -rf AppDir
mkdir -p AppDir/usr/bin
mkdir -p AppDir/usr/share/applications
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps

cp build/cache-cleaner AppDir/usr/bin/
cp assets/cache-cleaner.desktop AppDir/cache-cleaner.desktop
cp assets/cache-cleaner.svg AppDir/cache-cleaner.svg
cp assets/cache-cleaner.svg AppDir/usr/share/icons/hicolor/256x256/apps/

if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    wget -c "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod +x linuxdeploy-x86_64.AppImage
fi

./linuxdeploy-x86_64.AppImage --appimage-extract-and-run --appdir AppDir --output appimage

mv Cache_Cleaner*.AppImage cache-cleaner-x86_64.AppImage || true

echo "AppImage created: cache-cleaner-x86_64.AppImage"