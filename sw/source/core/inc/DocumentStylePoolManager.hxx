/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_SW_SOURCE_CORE_INC_DOCUMENTSTYLEPOOLMANAGER_HXX
#define INCLUDED_SW_SOURCE_CORE_INC_DOCUMENTSTYLEPOOLMANAGER_HXX

#include <IDocumentStylePoolAccess.hxx>
#include <boost/utility.hpp>

class SwDoc;

namespace sw {

class DocumentStylePoolManager : public IDocumentStylePoolAccess,
                                 public ::boost::noncopyable
{

public:

    DocumentStylePoolManager( SwDoc& i_rSwdoc );

    virtual SwTxtFmtColl* GetTxtCollFromPool( sal_uInt16 nId, bool bRegardLanguage = true ) SAL_OVERRIDE;
    virtual SwFmt* GetFmtFromPool( sal_uInt16 nId ) SAL_OVERRIDE;
    virtual SwFrmFmt* GetFrmFmtFromPool( sal_uInt16 nId ) SAL_OVERRIDE;
    virtual SwCharFmt* GetCharFmtFromPool( sal_uInt16 nId ) SAL_OVERRIDE;
    virtual SwPageDesc* GetPageDescFromPool( sal_uInt16 nId, bool bRegardLanguage = true ) SAL_OVERRIDE;
    virtual SwNumRule* GetNumRuleFromPool( sal_uInt16 nId ) SAL_OVERRIDE;
    virtual bool IsPoolTxtCollUsed( sal_uInt16 nId ) const SAL_OVERRIDE;
    virtual bool IsPoolFmtUsed( sal_uInt16 nId ) const SAL_OVERRIDE;
    virtual bool IsPoolPageDescUsed( sal_uInt16 nId ) const SAL_OVERRIDE;

    virtual ~DocumentStylePoolManager();

private:

    SwDoc& m_rDoc;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

