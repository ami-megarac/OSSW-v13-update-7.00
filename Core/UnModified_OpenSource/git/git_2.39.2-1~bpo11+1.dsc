-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: git
Binary: git, git-man, git-doc, git-cvs, git-svn, git-mediawiki, git-email, git-daemon-run, git-daemon-sysvinit, git-gui, gitk, gitweb, git-all
Architecture: any all
Version: 1:2.39.2-1~bpo11+1
Maintainer: Jonathan Nieder <jrnieder@gmail.com>
Uploaders: Anders Kaseorg <andersk@mit.edu>
Homepage: https://git-scm.com/
Standards-Version: 4.3.0.1
Vcs-Browser: https://repo.or.cz/w/git/debian.git/
Vcs-Git: https://repo.or.cz/r/git/debian.git/
Build-Depends: libz-dev, gettext, libpcre2-dev | libpcre3-dev, libcurl4-gnutls-dev, libexpat1-dev, subversion, libsvn-perl, libyaml-perl, tcl, python3, libhttp-date-perl | libtime-parsedate-perl, libcgi-pm-perl, liberror-perl, libmailtools-perl, cvs, cvsps, libdbd-sqlite3-perl, unzip, libio-pty-perl, debhelper-compat (= 10), dh-exec (>= 0.7), dh-apache2, dpkg-dev (>= 1.16.2~)
Build-Depends-Indep: asciidoc (>= 8.6.10), xmlto, docbook-xsl
Package-List:
 git deb vcs optional arch=any
 git-all deb vcs optional arch=all
 git-cvs deb vcs optional arch=all
 git-daemon-run deb vcs optional arch=all
 git-daemon-sysvinit deb vcs optional arch=all
 git-doc deb doc optional arch=all
 git-email deb vcs optional arch=all
 git-gui deb vcs optional arch=all
 git-man deb doc optional arch=all
 git-mediawiki deb vcs optional arch=all
 git-svn deb vcs optional arch=all
 gitk deb vcs optional arch=all
 gitweb deb vcs optional arch=all
Checksums-Sha1:
 3c415274f626589b37d5b0a9add11de34bcc2f5d 7163224 git_2.39.2.orig.tar.xz
 f9c2083e66ed5687aeef2c501c0e517a31b37a5e 740440 git_2.39.2-1~bpo11+1.debian.tar.xz
Checksums-Sha256:
 475f75f1373b2cd4e438706185175966d5c11f68c4db1e48c26257c43ddcf2d6 7163224 git_2.39.2.orig.tar.xz
 498894aa8bcaaf3e5227ec3cfdac9430e4504f7605b730f4a2709684ef140be9 740440 git_2.39.2-1~bpo11+1.debian.tar.xz
Files:
 32d34dc65ae0955cc68c7152b5ca8b13 7163224 git_2.39.2.orig.tar.xz
 00c92b9300a358e53491443055103516 740440 git_2.39.2-1~bpo11+1.debian.tar.xz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCAAdFiEEfAcX+forK514ixQbptwk2dokk9EFAmP7CQAACgkQptwk2dok
k9HFrRAAkSmFTDaQufJKRtooV+h8K6o9GDMI9amwHAPphs5vAKTgs89w/Slxjmbi
P4lQNdT6pdqO2IzyLVZ9zTi5G5owQp56s/NZHpTNHBCNd0M7oQGGEjADm+Dks52r
lIOKk5cyEIqoDKpMBflCxR0PgWKGg8feIAHnFm4bdNW+iBEW5J1WOkmjSVi1ETIM
pooAv/K3dIcZYRrlP1R7LKRAyi6gw86DfkTDwApeK8LOJ9tutaVzBJoDJejaumKG
Y7xYIqydm8pNABmT9nHHhDXNKfqrHfyRpmDQ5f7fKPIg+WlyGXGWXTFLeN4babvm
AUcgJ6vznWX284BkbKFEQWeynDsOWJ+N0LIQd1gUxOWF1Ajhs75/kYoeBjvrCN5F
DlLl3Hm3w1d4D7EELTJBJVaK44elJ5QL4gQI05jyir+Vnb3mJrKUy4CLjOkZoCOZ
R6gzs3wUK4+XvtobuSwDO3fyu+5OLDtmoYrcqSy80n/OXn0GcgTCQ1C6v0nmmbaj
yyIui5YnBPSeFcMTyHGATHyXK5nRl45pJ9BmGjmpWRTa8YbzptvfTyzEih1s6PtL
PV2RKhRyupY4dbaBRzX9dJDtDZKQjS3o5EO8zTvQRPwGVsmkrUy05zQptc7ccrl7
ge+3GOca5a8hI6gehz7JKe0DpAb1cxidLeB1jsFAujTPfuJfCRo=
=iCaP
-----END PGP SIGNATURE-----
