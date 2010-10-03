found_devenv := $(shell which devenv.com >/dev/null 2>&1 && echo yes)
.nodevenv.once:
	@echo "  * devenv.com not found in path."
	@echo "  * "
	@echo "  * You will have to build all configurations manually using the"
	@echo "  * Visual Studio IDE. To allow make to build them automatically,"
	@echo "  * add the Common7/IDE directory of your Visual Studio"
	@echo "  * installation to your path, eg:"
	@echo "  *   C:\Program Files\Microsoft Visual Studio 8\Common7\IDE"
	@echo "  * "
	@touch $@
CLEAN-OBJS += $(if $(found_devenv),,.nodevenv.once)

BUILD_TARGETS += $(if $(NO_LAUNCH_DEVENV),,Debug_Win32)
clean::
	rm -rf "Win32"/"Debug"
.PHONY: Debug_Win32
ifneq ($(found_devenv),)
  ifeq ($(CONFIG_VS_VERSION),7)
Debug_Win32: vpx.sln
	devenv.com vpx.sln -build "Debug"

  else
Debug_Win32: vpx.sln
	devenv.com vpx.sln -build "Debug|Win32"

  endif
else
Debug_Win32: vpx.sln .nodevenv.once
	@echo "  * Skipping build of Debug|Win32 (devenv.com not in path)."
	@echo "  * "
endif

BUILD_TARGETS += $(if $(NO_LAUNCH_DEVENV),,Release_Win32)
clean::
	rm -rf "Win32"/"Release"
.PHONY: Release_Win32
ifneq ($(found_devenv),)
  ifeq ($(CONFIG_VS_VERSION),7)
Release_Win32: vpx.sln
	devenv.com vpx.sln -build "Release"

  else
Release_Win32: vpx.sln
	devenv.com vpx.sln -build "Release|Win32"

  endif
else
Release_Win32: vpx.sln .nodevenv.once
	@echo "  * Skipping build of Release|Win32 (devenv.com not in path)."
	@echo "  * "
endif

