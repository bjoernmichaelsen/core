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

#ifndef INCLUDED_DBACCESS_SOURCE_UI_DLG_DSSELECT_HXX
#define INCLUDED_DBACCESS_SOURCE_UI_DLG_DSSELECT_HXX

#include "dsntypes.hxx"
#include "odbcconfig.hxx"
#include "commontypes.hxx"

#include <vcl/dialog.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/button.hxx>
#include <vcl/group.hxx>
#include <vcl/fixed.hxx>
#include <rtl/ustring.hxx>

#include <memory>

class SfxItemSet;
namespace dbaui
{

// ODatasourceSelector
class ODatasourceSelectDialog : public ModalDialog
{
protected:
    ListBox*        m_pDatasource;
    OKButton*       m_pOk;
    CancelButton*   m_pCancel;
    SfxItemSet*     m_pOutputSet;
#ifdef HAVE_ODBC_ADMINISTRATION
    PushButton*     m_pManageDatasources;
    ::std::unique_ptr< OOdbcManagement >
                    m_pODBCManagement;
#endif

public:
    ODatasourceSelectDialog( vcl::Window* _pParent, const StringBag& _rDatasources, SfxItemSet* _pOutputSet = NULL );
    virtual ~ODatasourceSelectDialog();

    OUString GetSelected() const { return m_pDatasource->GetSelectEntry();}
    void     Select( const OUString& _rEntry ) { m_pDatasource->SelectEntry(_rEntry); }

    virtual bool    Close() SAL_OVERRIDE;

protected:
    DECL_LINK( ListDblClickHdl, ListBox * );
#ifdef HAVE_ODBC_ADMINISTRATION
    DECL_LINK( ManageClickHdl, void* );
    DECL_LINK( ManageProcessFinished, void* );
#endif
    void fillListBox(const StringBag& _rDatasources);
};

}   // namespace dbaui

#endif // INCLUDED_DBACCESS_SOURCE_UI_DLG_DSSELECT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
