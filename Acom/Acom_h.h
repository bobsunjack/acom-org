

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Nov 20 10:34:41 2015
 */
/* Compiler settings for Acom.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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


#ifndef __Acom_h_h__
#define __Acom_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DAcom_FWD_DEFINED__
#define ___DAcom_FWD_DEFINED__
typedef interface _DAcom _DAcom;
#endif 	/* ___DAcom_FWD_DEFINED__ */


#ifndef ___DAcomEvents_FWD_DEFINED__
#define ___DAcomEvents_FWD_DEFINED__
typedef interface _DAcomEvents _DAcomEvents;
#endif 	/* ___DAcomEvents_FWD_DEFINED__ */


#ifndef __Acom_FWD_DEFINED__
#define __Acom_FWD_DEFINED__

#ifdef __cplusplus
typedef class Acom Acom;
#else
typedef struct Acom Acom;
#endif /* __cplusplus */

#endif 	/* __Acom_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __AcomLib_LIBRARY_DEFINED__
#define __AcomLib_LIBRARY_DEFINED__

/* library AcomLib */
/* [control][version][uuid] */ 


EXTERN_C const IID LIBID_AcomLib;

#ifndef ___DAcom_DISPINTERFACE_DEFINED__
#define ___DAcom_DISPINTERFACE_DEFINED__

/* dispinterface _DAcom */
/* [uuid] */ 


EXTERN_C const IID DIID__DAcom;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("44FB3261-2DF3-4596-9527-FE963F540F25")
    _DAcom : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAcomVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAcom * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAcom * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAcom * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAcom * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAcom * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAcom * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAcom * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAcomVtbl;

    interface _DAcom
    {
        CONST_VTBL struct _DAcomVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAcom_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAcom_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAcom_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAcom_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAcom_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAcom_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAcom_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAcom_DISPINTERFACE_DEFINED__ */


#ifndef ___DAcomEvents_DISPINTERFACE_DEFINED__
#define ___DAcomEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAcomEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__DAcomEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0731A0EF-DAA1-4DDD-B7C1-B7473A4164F8")
    _DAcomEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAcomEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAcomEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAcomEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAcomEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAcomEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAcomEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAcomEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAcomEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAcomEventsVtbl;

    interface _DAcomEvents
    {
        CONST_VTBL struct _DAcomEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAcomEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAcomEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAcomEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAcomEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAcomEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAcomEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAcomEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAcomEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Acom;

#ifdef __cplusplus

class DECLSPEC_UUID("6CB6C1B5-0E0D-4622-8510-DF1EB6350050")
Acom;
#endif
#endif /* __AcomLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


