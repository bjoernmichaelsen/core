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
#ifndef INCLUDED_SVX_SOURCE_SIDEBAR_PARAGRAPH_PARALINESPACINGCONTROL_HXX
#define INCLUDED_SVX_SOURCE_SIDEBAR_PARAGRAPH_PARALINESPACINGCONTROL_HXX

#include <svtools/ctrlbox.hxx>
#include <sfx2/tbxctrl.hxx>

class SvxLineSpacingItem;

namespace svx {

class ParaLineSpacingControl : public SfxPopupWindow
{
public:
    ParaLineSpacingControl(sal_uInt16 nId);
    virtual ~ParaLineSpacingControl();

private:
    SfxMapUnit              meLNSpaceUnit;

    Edit*                   mpActLineDistFld;

    PushButton*             mpSpacing1Button;
    PushButton*             mpSpacing115Button;
    PushButton*             mpSpacing15Button;
    PushButton*             mpSpacing2Button;

    ListBox*                mpLineDist;

    FixedText*              mpLineDistLabel;
    MetricField*            mpLineDistAtPercentBox;
    MetricField*            mpLineDistAtMetricBox;

private:
    /// Setup the widgets with values from the document.
    void Initialize();

    /// Take the values from the widgets, and update the paragraph accordingly.
    void ExecuteLineSpace();

    /// Set one particular value.
    void SetLineSpace(SvxLineSpacingItem& rLineSpace, int eSpace, long lValue = 0);

    /// For the buttons - set the values, and close the popup.
    void ExecuteLineSpacing(sal_uInt16 aEntry);

    /// Set mpActlineDistFld and visibility of mpLineDist* fields according to what is just selected.
    void UpdateMetricFields();

    /// Set the entry and update the metric fields.
    void SelectEntryPos(sal_Int32 nPos);

    DECL_LINK(LineSPDistHdl_Impl, ListBox*);
    DECL_LINK(LineSPDistAtHdl_Impl, void*);
    DECL_LINK(PredefinedValuesHandler, void*);
};

} // namespace svx

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
