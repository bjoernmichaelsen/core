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

#include <boost/optional.hpp>
#include <libxml/xmlwriter.h>
#include <svl/itempool.hxx>
#include <txatbase.hxx>
#include <fmtfld.hxx>
#include <docufld.hxx>

SwTxtAttr::SwTxtAttr( SfxPoolItem& rAttr, sal_Int32 nStart )
    : m_pAttr( &rAttr )
    , m_nStart( nStart )
    , m_bDontExpand( false )
    , m_bLockExpandFlag( false )
    , m_bDontMoveAttr( false )
    , m_bCharFmtAttr( false )
    , m_bOverlapAllowedAttr( false )
    , m_bPriorityAttr( false )
    , m_bDontExpandStart( false )
    , m_bNesting( false )
    , m_bHasDummyChar( false )
    , m_bFormatIgnoreStart(false)
    , m_bFormatIgnoreEnd(false)
    , m_bHasContent( false )
{
}

SwTxtAttr::~SwTxtAttr( )
{
}

sal_Int32* SwTxtAttr::GetEnd()
{
    return 0;
}

void SwTxtAttr::Destroy( SwTxtAttr * pToDestroy, SfxItemPool& rPool )
{
    if (!pToDestroy) return;
    SfxPoolItem * const pAttr = pToDestroy->m_pAttr;
    delete pToDestroy;
    rPool.Remove( *pAttr );
}

bool SwTxtAttr::operator==( const SwTxtAttr& rAttr ) const
{
    return GetAttr() == rAttr.GetAttr();
}

SwTxtAttrEnd::SwTxtAttrEnd( SfxPoolItem& rAttr,
        sal_Int32 nStart, sal_Int32 nEnd ) :
    SwTxtAttr( rAttr, nStart ), m_nEnd( nEnd )
{
}

sal_Int32* SwTxtAttrEnd::GetEnd()
{
    return & m_nEnd;
}

void SwTxtAttr::dumpAsXml(xmlTextWriterPtr pWriter) const
{
    xmlTextWriterStartElement(pWriter, BAD_CAST("swTxtAttr"));

    xmlTextWriterWriteAttribute(pWriter, BAD_CAST("start"), BAD_CAST(OString::number(m_nStart).getStr()));
    if (End())
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST("end"), BAD_CAST(OString::number(*End()).getStr()));
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST("whichId"), BAD_CAST(OString::number(Which()).getStr()));
    const char* pWhich = 0;
    boost::optional<OString> oValue;
    switch (Which())
    {
    case RES_TXTATR_AUTOFMT:
        pWhich = "autofmt";
        break;
    case RES_TXTATR_ANNOTATION:
        pWhich = "annotation";
        break;
    case RES_TXTATR_FLYCNT:
        pWhich = "fly content";
        break;
    case RES_TXTATR_CHARFMT:
        {
            pWhich = "character format";
            if (SwCharFmt* pCharFmt = GetCharFmt().GetCharFmt())
                oValue = "name: " + OUStringToOString(pCharFmt->GetName(), RTL_TEXTENCODING_UTF8);
            break;
        }
    default:
        break;
    }
    if (pWhich)
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST("whichId"), BAD_CAST(pWhich));
    if (oValue)
        xmlTextWriterWriteAttribute(pWriter, BAD_CAST("value"), BAD_CAST(oValue->getStr()));
    if (Which() == RES_TXTATR_AUTOFMT)
        GetAutoFmt().dumpAsXml(pWriter);

    xmlTextWriterEndElement(pWriter);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
