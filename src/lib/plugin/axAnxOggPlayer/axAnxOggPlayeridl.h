

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Sun Oct 24 15:14:10 2004
 */
/* Compiler settings for .\axAnxOggPlayer.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __axAnxOggPlayeridl_h__
#define __axAnxOggPlayeridl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DaxAnxOggPlayer_FWD_DEFINED__
#define ___DaxAnxOggPlayer_FWD_DEFINED__
typedef interface _DaxAnxOggPlayer _DaxAnxOggPlayer;
#endif 	/* ___DaxAnxOggPlayer_FWD_DEFINED__ */


#ifndef ___DaxAnxOggPlayerEvents_FWD_DEFINED__
#define ___DaxAnxOggPlayerEvents_FWD_DEFINED__
typedef interface _DaxAnxOggPlayerEvents _DaxAnxOggPlayerEvents;
#endif 	/* ___DaxAnxOggPlayerEvents_FWD_DEFINED__ */


#ifndef __axAnxOggPlayer_FWD_DEFINED__
#define __axAnxOggPlayer_FWD_DEFINED__

#ifdef __cplusplus
typedef class axAnxOggPlayer axAnxOggPlayer;
#else
typedef struct axAnxOggPlayer axAnxOggPlayer;
#endif /* __cplusplus */

#endif 	/* __axAnxOggPlayer_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __axAnxOggPlayerLib_LIBRARY_DEFINED__
#define __axAnxOggPlayerLib_LIBRARY_DEFINED__

/* library axAnxOggPlayerLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_axAnxOggPlayerLib;

#ifndef ___DaxAnxOggPlayer_DISPINTERFACE_DEFINED__
#define ___DaxAnxOggPlayer_DISPINTERFACE_DEFINED__

/* dispinterface _DaxAnxOggPlayer */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DaxAnxOggPlayer;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B878657F-FA23-4915-8E80-6664F1738BEF")
    _DaxAnxOggPlayer : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DaxAnxOggPlayerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DaxAnxOggPlayer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DaxAnxOggPlayer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DaxAnxOggPlayer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DaxAnxOggPlayer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DaxAnxOggPlayer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DaxAnxOggPlayer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DaxAnxOggPlayer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DaxAnxOggPlayerVtbl;

    interface _DaxAnxOggPlayer
    {
        CONST_VTBL struct _DaxAnxOggPlayerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DaxAnxOggPlayer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DaxAnxOggPlayer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DaxAnxOggPlayer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DaxAnxOggPlayer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DaxAnxOggPlayer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DaxAnxOggPlayer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DaxAnxOggPlayer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DaxAnxOggPlayer_DISPINTERFACE_DEFINED__ */


#ifndef ___DaxAnxOggPlayerEvents_DISPINTERFACE_DEFINED__
#define ___DaxAnxOggPlayerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DaxAnxOggPlayerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DaxAnxOggPlayerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("380DE0F9-05B8-4D01-9221-C648BC432423")
    _DaxAnxOggPlayerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DaxAnxOggPlayerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DaxAnxOggPlayerEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DaxAnxOggPlayerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DaxAnxOggPlayerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DaxAnxOggPlayerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DaxAnxOggPlayerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DaxAnxOggPlayerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DaxAnxOggPlayerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DaxAnxOggPlayerEventsVtbl;

    interface _DaxAnxOggPlayerEvents
    {
        CONST_VTBL struct _DaxAnxOggPlayerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DaxAnxOggPlayerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DaxAnxOggPlayerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DaxAnxOggPlayerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DaxAnxOggPlayerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DaxAnxOggPlayerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DaxAnxOggPlayerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DaxAnxOggPlayerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DaxAnxOggPlayerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_axAnxOggPlayer;

#ifdef __cplusplus

class DECLSPEC_UUID("9B80DAA5-BCFA-44F8-B2AA-B2ECF80602A1")
axAnxOggPlayer;
#endif
#endif /* __axAnxOggPlayerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


