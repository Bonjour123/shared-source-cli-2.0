// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// EEConfigFactory.h -
//
// Parses XML files and adding runtime entries to the EEConfig list
//
//

#ifndef EECONFIGFACTORY_H
#define EECONFIGFACTORY_H

#include <xmlparser.h>
#include <objbase.h>
#include "unknwn.h"
#include "../xmlparser/_reference.h"
#include "../xmlparser/_unknown.h"
#include "eehash.h"
#include "eeconfig.h"

#define CONFIG_KEY_SIZE 128

class EEConfigFactory : public _unknown<IXMLNodeFactory, &IID_IXMLNodeFactory>
{

public:
    EEConfigFactory(
        EEUnicodeStringHashTable* pTable, 
        LPCWSTR, 
        ParseCtl parseCtl = parseAll);
    ~EEConfigFactory();
    HRESULT STDMETHODCALLTYPE NotifyEvent( 
            /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
            /* [in] */ XML_NODEFACTORY_EVENT iEvt);

    HRESULT STDMETHODCALLTYPE BeginChildren( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ XML_NODE_INFO* __RPC_FAR pNodeInfo);

    HRESULT STDMETHODCALLTYPE EndChildren( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ BOOL fEmptyNode,
        /* [in] */ XML_NODE_INFO* __RPC_FAR pNodeInfo);
    
    HRESULT STDMETHODCALLTYPE Error( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ HRESULT hrErrorCode,
        /* [in] */ USHORT cNumRecs,
        /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo)
    {
        LEAF_CONTRACT;
      /* 
         UNUSED(pSource);
         UNUSED(hrErrorCode);
         UNUSED(cNumRecs);
         UNUSED(apNodeInfo);
      */
        return hrErrorCode;
    }
    
    HRESULT STDMETHODCALLTYPE CreateNode( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ PVOID pNodeParent,
        /* [in] */ USHORT cNumRecs,
        /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo);

private:

    HRESULT GrowKey(DWORD dwSize)
    {
        CONTRACTL
        {
      	    NOTHROW;	    
      	    GC_NOTRIGGER;
      	    INJECT_FAULT(return E_OUTOFMEMORY);
        }
        CONTRACTL_END;
        
        if(dwSize > m_dwSize) {
            DeleteKey();
            m_pCurrentRuntimeElement = new(nothrow) WCHAR[dwSize];
            if(m_pCurrentRuntimeElement == NULL) return E_OUTOFMEMORY;
            m_dwSize = dwSize;
        }
        return S_OK;
    }

    void ClearKey()
    {
        LEAF_CONTRACT;

        *m_pCurrentRuntimeElement = 0;
        m_dwCurrentRuntimeElement = 0;
    }

    void DeleteKey()
    {
        LEAF_CONTRACT;
        if(m_pCurrentRuntimeElement != NULL && m_pCurrentRuntimeElement != m_pBuffer)
            delete [] m_pCurrentRuntimeElement;
        m_dwSize = 0;
        m_dwCurrentRuntimeElement = 0;
    }

    HRESULT CopyToKey(__in_z LPCWSTR pString, DWORD dwString)
    {
        LEAF_CONTRACT;
        dwString++; // add in the null
        HRESULT hr = GrowKey(dwString);
        if(FAILED(hr)) return hr;
        wcsncpy_s(m_pCurrentRuntimeElement, m_dwSize, pString, dwString-1);

        m_dwCurrentRuntimeElement = dwString;
        return S_OK;
    }
        
    HRESULT CopyVersion(LPCWSTR version, DWORD dwVersion);

    EEUnicodeStringHashTable* m_pTable;
    BOOL    m_fUnderRuntimeElement;
    BOOL    m_fOnEnabledAttribute;
    BOOL    m_fVersionedRuntime;
    BOOL    m_fDeveloperSettings;

    LPCWSTR m_pVersion;
    LPWSTR  m_pCurrentRuntimeElement;
    DWORD   m_dwCurrentRuntimeElement;

    WCHAR   m_pBuffer[CONFIG_KEY_SIZE];
    DWORD   m_dwSize;

    DWORD   m_dwDepth;

    bool    m_bSafeMode; // If true, will ignore any settings that may compromise security
    ParseCtl m_parseCtl; // usually parseAll, sometimes stopAfterRuntimeSection
};

#endif
