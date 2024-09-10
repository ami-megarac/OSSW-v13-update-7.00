-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: redis
Binary: redis, redis-sentinel, redis-server, redis-tools
Architecture: any all
Version: 5:7.0.10-1~bpo11+1
Maintainer: Chris Lamb <lamby@debian.org>
Homepage: https://redis.io/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/lamby/pkg-redis
Vcs-Git: https://salsa.debian.org/lamby/pkg-redis.git -b debian/experimental
Testsuite: autopkgtest
Build-Depends: debhelper-compat (= 13), dpkg-dev (>= 1.17.14), libhiredis-dev (>= 0.14.0), libjemalloc-dev [linux-any], liblua5.1-dev, liblzf-dev, libssl-dev, libsystemd-dev, lua-bitop-dev, lua-cjson-dev, openssl <!nocheck>, pkg-config, procps <!nocheck>, tcl <!nocheck>, tcl-tls <!nocheck>
Package-List:
 redis deb database optional arch=all
 redis-sentinel deb database optional arch=any
 redis-server deb database optional arch=any
 redis-tools deb database optional arch=any
Checksums-Sha1:
 d5cd28c2907625532bef88828ba478a2f04d9bfa 3017600 redis_7.0.10.orig.tar.gz
 70cff96a37e1da9d7e5d85679f3b0a23b2173e0c 28752 redis_7.0.10-1~bpo11+1.debian.tar.xz
Checksums-Sha256:
 5be1f61c8ce4216e0ca80c835def3a16eb4a29fa80b2ecd04943eacac9d038ea 3017600 redis_7.0.10.orig.tar.gz
 667515946fcfd54e08d4a405c4247bd9d196eb7e7a53a8029331c0741a951ad9 28752 redis_7.0.10-1~bpo11+1.debian.tar.xz
Files:
 c2b06eb38e6094be789ad18aa5b178e2 3017600 redis_7.0.10.orig.tar.gz
 67a0b1a1fbf3b8c3188bc64c9d5cabc1 28752 redis_7.0.10-1~bpo11+1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCAAdFiEEwv5L0nHBObhsUz5GHpU+J9QxHlgFAmQn/ckACgkQHpU+J9Qx
HljLHA//bAGGrRmyu8IN6ALHG3EpE60XPdQcR5SJyG+3O6Z3EVUjIS4vjAD4FI4s
VgnGNNE4KunmugJSGOt2iZZS+fE1B12yL8TMWld42Y39NL9P+/o4SGak7vMt7VTq
92Bhor0P9osigca4J5aflriGo7rsirzaoBURwcEfcVhc7suvBKlRR7doXFZPcwc+
PsjKV9kNqxRFjStfN+fzt1ZsAum3pX4UEuURyMehoVdxIfVhxvyKWEAD85cPCqnf
IIk3DNUc/he6cD0csVu5moQPDE/TMBszlLcWRRkhbMLUM1VZlnhyyqKemK3R+2qE
GP3ZPDK3mWRdZu4x94qcJuoGNbd2tcm9S0R4eSc7ZvPD3X4pxNEnEACrSbti3T+M
pdglCLfgWonDos3wskCXUxsdA0qZiZgroCpPSeCQMrfkIw34KyeYO2UQz2YWQ9GH
VvcVR0gXwcEWI16NUlj/NgbF2wVnr89z8sRsXhNISPk0iivyLwYP8bRrn3Pffblx
GvtGmq8StyY0P9UNb1glP9/2qUv/62X/9blXNHXvbGFVx3FaQwYLTWOCNVp3as5f
DQTkwQ0Xvx4AG9RTWecDQaKEqi2XXBQqcRnvKHHAF7+cC75ag9QR1SaukwhRaIGw
rkX1H5li/U3hlltckPf7XWH2q8eQSs8evMsscSUjDs5lA5L5KuA=
=rYg9
-----END PGP SIGNATURE-----
