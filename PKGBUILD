# Maintainer: eufjdknasquwy <e0723044@gmail.com>
pkgname=cache-cleaner
pkgver=0.4.0
pkgrel=1
pkgdesc="A utility for Linux to free disk space by cleaning caches of package managers and other caches"
arch=('x86_64')
url="https://github.com/eufjdknasquwy/cache-cleaner-linux"
license=('MIT')
depends=('gtkmm3' 'nlohmann-json')
makedepends=('cmake' 'ninja')
source=("$pkgname-$pkgver.tar.gz::$url/archive/v$pkgver.tar.gz")
sha256sums=('935286707f0fb9798f18f2d814ba5093039dff8ae370dd04761ce2a0aedccdce')

build() {
  cmake -S "cache-cleaner-linux-$pkgver" -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_SKIP_RPATH=ON
  cmake --build build
}

package() {
  DESTDIR="$pkgdir" cmake --install build
}