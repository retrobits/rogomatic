SUBDIRS = src tests fw-pkgin

memcheck:
	cd tests && $(MAKE) memcheck

memcheck-%:
	cd tests && $(MAKE) memcheck-$*

leakcheck:
	cd tests && $(MAKE) leakcheck

leakcheck-%:
	cd tests && $(MAKE) leakcheck-$*

MAINTAINERCLEANFILES = \
  Makefile.in          \
  ac-init.generated    \
  aclocal.m4           \
  build                \
  config.guess         \
  config.log           \
  config.status        \
  config.sub           \
  configure            \
  configure.ac         \
  create-package       \
  depcomp              \
  fw                   \
  fw-pkgout            \
  fw-pkgtmp            \
  install-sh           \
  libtool              \
  ltmain.sh            \
  missing              \
  pkgconfig-template.pc.in \
  *.tar.gz

include $(top_srcdir)/Makefile.am.local
