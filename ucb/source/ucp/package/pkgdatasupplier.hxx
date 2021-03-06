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

#ifndef INCLUDED_UCB_SOURCE_UCP_PACKAGE_PKGDATASUPPLIER_HXX
#define INCLUDED_UCB_SOURCE_UCP_PACKAGE_PKGDATASUPPLIER_HXX

#include <rtl/ref.hxx>
#include <ucbhelper/resultset.hxx>

namespace package_ucp {

struct DataSupplier_Impl;
class Content;

class DataSupplier : public ::ucbhelper::ResultSetDataSupplier
{
    DataSupplier_Impl* m_pImpl;

public:
    DataSupplier( const com::sun::star::uno::Reference<
                      com::sun::star::uno::XComponentContext >& rxContext,
                  const rtl::Reference< Content >& rContent,
                  sal_Int32 nOpenMode );
    virtual ~DataSupplier();

    virtual OUString queryContentIdentifierString( sal_uInt32 nIndex ) SAL_OVERRIDE;
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >
    queryContentIdentifier( sal_uInt32 nIndex ) SAL_OVERRIDE;
    virtual com::sun::star::uno::Reference< com::sun::star::ucb::XContent >
    queryContent( sal_uInt32 nIndex ) SAL_OVERRIDE;

    virtual bool getResult( sal_uInt32 nIndex ) SAL_OVERRIDE;

    virtual sal_uInt32 totalCount() SAL_OVERRIDE;
    virtual sal_uInt32 currentCount() SAL_OVERRIDE;
    virtual bool isCountFinal() SAL_OVERRIDE;

    virtual com::sun::star::uno::Reference< com::sun::star::sdbc::XRow >
    queryPropertyValues( sal_uInt32 nIndex  ) SAL_OVERRIDE;
    virtual void releasePropertyValues( sal_uInt32 nIndex ) SAL_OVERRIDE;

    virtual void close() SAL_OVERRIDE;

    virtual void validate()
        throw( com::sun::star::ucb::ResultSetException ) SAL_OVERRIDE;

    OUString assembleChildURL( const OUString& aName );
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
