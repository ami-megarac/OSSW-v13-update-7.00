-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: openssl
Binary: openssl, libssl3, libcrypto3-udeb, libssl3-udeb, libssl-dev, libssl-doc
Architecture: any all
Version: 3.0.12-2
Maintainer: Debian OpenSSL Team <pkg-openssl-devel@alioth-lists.debian.net>
Uploaders: Christoph Martin <christoph.martin@uni-mainz.de>, Kurt Roeckx <kurt@roeckx.be>, Sebastian Andrzej Siewior <sebastian@breakpoint.cc>
Homepage: https://www.openssl.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/debian/openssl
Vcs-Git: https://salsa.debian.org/debian/openssl.git
Testsuite: autopkgtest
Testsuite-Triggers: perl
Build-Depends: debhelper-compat (= 13), dpkg-dev (>= 1.15.7)
Package-List:
 libcrypto3-udeb udeb debian-installer optional arch=any profile=!noudeb
 libssl-dev deb libdevel optional arch=any
 libssl-doc deb doc optional arch=all
 libssl3 deb libs optional arch=any
 libssl3-udeb udeb debian-installer optional arch=any profile=!noudeb
 openssl deb utils optional arch=any
Checksums-Sha1:
 b48e20c07facfdf6da9ad43a6c5126d51897699b 15204575 openssl_3.0.12.orig.tar.gz
 471ebb7879a3372f84488f7bbcd272756eae178c 833 openssl_3.0.12.orig.tar.gz.asc
 e282e4606f6cbba6aa849c0e6dacc68d5b997228 71192 openssl_3.0.12-2.debian.tar.xz
Checksums-Sha256:
 f93c9e8edde5e9166119de31755fc87b4aa34863662f67ddfcba14d0b6b69b61 15204575 openssl_3.0.12.orig.tar.gz
 6caf710d17d6cde335b8df588ebf8af14c803653a871856c70ad17fec75c12f4 833 openssl_3.0.12.orig.tar.gz.asc
 3041a9794ea45d9c5e88233af8a1751b185511fe505495f487c5b854c2b06ba5 71192 openssl_3.0.12-2.debian.tar.xz
Files:
 e6a199cdf867873eef2c6491b674edbc 15204575 openssl_3.0.12.orig.tar.gz
 3288bf50e8096bfe6171d017bf098874 833 openssl_3.0.12.orig.tar.gz.asc
 438d431d1db1d810d53cc17c65e4d89f 71192 openssl_3.0.12-2.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQGzBAEBCgAdFiEEV4kucFIzBRM39v3RBWQfF1cS+lsFAmVL9YcACgkQBWQfF1cS
+ltbSQv+LH7fE4iQpFilws2lhlCJYWto5ogKMJamOoLp4PH7648wsgPJ+Z0Gv7bj
tDhbP5o1Sa+cmoLmiDkEShLS+GCjGyeq3XdeN7ctKDdaPUNCrIKaDMaETGZEb05T
+xE9ZX0ki7Za+q3fbqbiCuSYFjnmL+Bh9r/WNbcF0qJ8+hbdBObKclmOpg102kaz
houwp3R7X1oILBqFyKLCKzMLl7bojLSNa/CPeRC7WTTerzV47Cs5UaGxvn0JdmkY
GbVuasLRwtq5nFrdqoK1/6PbDmkCrPBvT00GviRPcua10hMOtvFBDhGA6otZ74f4
x0FPr6Xw1wLEaFQRzS1SVtIzWb2ty11Tfkx88hA8MaL0i3TpeouIK65H7FvFk2L2
u9CKEA+miSkXi+Qg7I9VehijMsMhxdP/fOHkhZP3XcG3/2AWdlTgCTvTJOgjMpBP
AOAPSj4aX5LIeZHSxXP6sbq2bFs2hwJuo26XJoLxoYaIELCTaFBPIMncS6il1NKn
88qhdjsg
=DZLG
-----END PGP SIGNATURE-----
