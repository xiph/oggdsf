mod_timeslice.la: mod_timeslice.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_timeslice.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_timeslice.la
