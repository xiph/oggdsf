mod_oggchef.la: mod_oggchef.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_oggchef.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_oggchef.la
