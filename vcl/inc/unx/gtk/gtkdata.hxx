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

#ifndef INCLUDED_VCL_INC_UNX_GTK_GTKDATA_HXX
#define INCLUDED_VCL_INC_UNX_GTK_GTKDATA_HXX

#include <prex.h>
#define GLIB_DISABLE_DEPRECATION_WARNINGS
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <postx.h>

#include <generic/gendata.hxx>
#include <unx/saldisp.hxx>
#include <unx/saldata.hxx>
#include <unx/gtk/gtksys.hxx>
#include <vcl/ptrstyle.hxx>
#include <osl/conditn.h>
#include "saltimer.hxx"

#include <list>

class GtkSalDisplay;

inline GdkWindow * widget_get_window(GtkWidget *widget)
{
#if GTK_CHECK_VERSION(3,0,0)
    return gtk_widget_get_window(widget);
#else
    return widget->window;
#endif
}

inline ::Window widget_get_xid(GtkWidget *widget)
{
#if GTK_CHECK_VERSION(3,0,0)
    return GDK_WINDOW_XID(gtk_widget_get_window(widget));
#else
    return GDK_WINDOW_XWINDOW(widget->window);
#endif
}

inline void widget_set_can_focus(GtkWidget *widget, gboolean can_focus)
{
#if GTK_CHECK_VERSION(3,0,0)
    return gtk_widget_set_can_focus(widget, can_focus);
#else
    if (can_focus)
        GTK_WIDGET_SET_FLAGS( widget, GTK_CAN_FOCUS );
    else
        GTK_WIDGET_UNSET_FLAGS( widget, GTK_CAN_FOCUS );
#endif
}

inline void widget_set_can_default(GtkWidget *widget, gboolean can_default)
{
#if GTK_CHECK_VERSION(3,0,0)
    return gtk_widget_set_can_default(widget, can_default);
#else
    if (can_default)
        GTK_WIDGET_SET_FLAGS( widget, GTK_CAN_DEFAULT );
    else
        GTK_WIDGET_UNSET_FLAGS( widget, GTK_CAN_DEFAULT );
#endif
}

class GtkSalTimer : public SalTimer
{
    struct SalGtkTimeoutSource *m_pTimeout;
public:
    GtkSalTimer();
    virtual ~GtkSalTimer();
    virtual void Start( sal_uLong nMS ) SAL_OVERRIDE;
    virtual void Stop() SAL_OVERRIDE;
    bool         Expired();

    sal_uLong    m_nTimeoutMS;
};

class GtkData : public SalGenericData
{
    GSource *m_pUserEvent;
    oslMutex m_aDispatchMutex;
    oslCondition m_aDispatchCondition;
    bool blockIdleTimeout;

public:
    GtkData( SalInstance *pInstance );
    virtual ~GtkData();

    void Init();
    virtual void Dispose() SAL_OVERRIDE;

    void initNWF();
    void deInitNWF();

    static gboolean userEventFn( gpointer data );

    void PostUserEvent();
    void Yield( bool bWait, bool bHandleAllCurrentEvents );
    inline GdkDisplay *GetGdkDisplay();

    virtual void ErrorTrapPush() SAL_OVERRIDE;
    virtual bool ErrorTrapPop( bool bIgnoreError ) SAL_OVERRIDE;

    inline GtkSalDisplay *GetGtkDisplay() const;
    bool BlockIdleTimeout() const { return blockIdleTimeout; }
};

class GtkSalFrame;

#if GTK_CHECK_VERSION(3,0,0)
class GtkSalDisplay : public SalGenericDisplay
#else
class GtkSalDisplay : public SalDisplay
#endif
{
    GtkSalSystem*                   m_pSys;
    GdkDisplay*                     m_pGdkDisplay;
    GdkCursor                      *m_aCursors[ POINTER_COUNT ];
    bool                            m_bStartupCompleted;
    bool                            m_bX11Display;

    GdkCursor* getFromXBM( const unsigned char *pBitmap, const unsigned char *pMask,
                           int nWidth, int nHeight, int nXHot, int nYHot );
public:
             GtkSalDisplay( GdkDisplay* pDisplay );
    virtual ~GtkSalDisplay();

    GdkDisplay* GetGdkDisplay() const { return m_pGdkDisplay; }
    bool        IsX11Display() const { return m_bX11Display; }

    GtkSalSystem* getSystem() const { return m_pSys; }

    virtual void deregisterFrame( SalFrame* pFrame ) SAL_OVERRIDE;
    GdkCursor *getCursor( PointerStyle ePointerStyle );
#if GTK_CHECK_VERSION(3,0,0)
    virtual int CaptureMouse( SalFrame* pFrame );
#else
    virtual int CaptureMouse( SalFrame* pFrame ) SAL_OVERRIDE;
#endif

    int          GetDefaultScreen() { return m_pSys->GetDisplayBuiltInScreen(); }
    SalX11Screen GetDefaultXScreen() { return m_pSys->GetDisplayDefaultXScreen(); }
    Size         GetScreenSize( int nDisplayScreen );
    int          GetXScreenCount() { return m_pSys->GetDisplayXScreenCount(); }
#if GTK_CHECK_VERSION(3,0,0)
//    int          GetScreenCount() { return m_pSys->GetDisplayScreenCount(); }
#else
    virtual ScreenData *initScreen( SalX11Screen nXScreen ) const SAL_OVERRIDE;
#endif

    GdkFilterReturn filterGdkEvent( GdkXEvent* sys_event,
                                    GdkEvent* event );
    void startupNotificationCompleted() { m_bStartupCompleted = true; }

    void screenSizeChanged( GdkScreen* );
    void monitorsChanged( GdkScreen* );

    virtual void PostUserEvent() SAL_OVERRIDE;

#if GTK_CHECK_VERSION(3,0,0)
    guint32 GetLastUserEventTime( bool /* b */ ) { return GDK_CURRENT_TIME; } // horrible hack
#else
    virtual bool Dispatch( XEvent *pEvent ) SAL_OVERRIDE;
#endif
};

inline GtkData* GetGtkSalData()
{
    return static_cast<GtkData*>(ImplGetSVData()->mpSalData);
}
inline GdkDisplay *GtkData::GetGdkDisplay()
{
    return GetGtkDisplay()->GetGdkDisplay();
}

GtkSalDisplay *GtkData::GetGtkDisplay() const
{
    return static_cast<GtkSalDisplay *>(GetDisplay());
}

#endif // INCLUDED_VCL_INC_UNX_GTK_GTKDATA_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
