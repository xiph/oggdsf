#pragma once

#ifndef __IOGGMUXSETTINGS__
#define __IOGGMUXSETTINGS__

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(IOggMuxSettings, IUnknown) {

	virtual STDMETHODIMP_(unsigned long) maxPacketsPerPage() PURE;
	virtual STDMETHODIMP_(bool) setMaxPacketsPerPage(unsigned long inMaxPacketsPerPage) PURE;
};

#ifdef __cplusplus
}
#endif

#endif