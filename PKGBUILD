# Maintainer: "till busch" <buti@bux.at>
pkgname=pdfsearch-git
pkgver=12.fe7a35a
pkgrel=1
pkgdesc="CLI frontend to poppler-glib of PDF tools"
url="https://github.com/buxit/pdfsearch.git"
arch=('any')
license=('GPLv3')
depends=('poppler-glib' 'ghostscript')
makedepends=('git')
md5sums=('SKIP')

source=("https://github.com/buxit/pdfsearch")
_gitname="pdfsearch"

pkgver() {
	cd "$_gitname"
    echo "0.$(git rev-list --count HEAD).$(git describe --always )"
}

build() {
    cd "$_gitname"
	make
}

package() {
	cd "$_gitname"
	make DESTDIR="$pkgdir" install
} 
