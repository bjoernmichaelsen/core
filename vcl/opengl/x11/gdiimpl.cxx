/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vcl/salbtype.hxx>

#include <svdata.hxx>

#include <unx/pixmap.hxx>
#include <unx/saldisp.hxx>
#include <unx/salframe.h>
#include <unx/salgdi.h>
#include <unx/salvd.h>
#include <unx/x11/xlimits.hxx>

#include <opengl/texture.hxx>
#include <opengl/x11/gdiimpl.hxx>
#include <opengl/x11/salvd.hxx>

#include <vcl/opengl/OpenGLContext.hxx>
#include <vcl/opengl/OpenGLHelper.hxx>

X11OpenGLSalGraphicsImpl::X11OpenGLSalGraphicsImpl( X11SalGraphics& rParent ):
    OpenGLSalGraphicsImpl(rParent,rParent.GetGeometryProvider()),
    mrParent(rParent)
{
}

X11OpenGLSalGraphicsImpl::~X11OpenGLSalGraphicsImpl()
{
}

void X11OpenGLSalGraphicsImpl::Init()
{
    // The m_pFrame and m_pVDev pointers are updated late in X11
    mpProvider = mrParent.GetGeometryProvider();
    OpenGLSalGraphicsImpl::Init();
}

OpenGLContext* X11OpenGLSalGraphicsImpl::CreateWinContext()
{
    X11WindowProvider *pProvider = dynamic_cast<X11WindowProvider*>(mrParent.m_pFrame);

    if( !pProvider )
        return NULL;

    Window aWin = pProvider->GetX11Window();
    OpenGLContext* pContext = new OpenGLContext();
    pContext->init( mrParent.GetXDisplay(), aWin,
                    mrParent.m_nXScreen.getXScreen() );
    return pContext;
}

bool X11OpenGLSalGraphicsImpl::UseContext( OpenGLContext* pContext )
{
    X11WindowProvider *pProvider = dynamic_cast<X11WindowProvider*>(mrParent.m_pFrame);

    if( !pContext->isInitialized() )
        return false;

    if( !pProvider )
        return ( pContext->getOpenGLWindow().win != None );
    else
        return ( pContext->getOpenGLWindow().win == pProvider->GetX11Window() );
}

void X11OpenGLSalGraphicsImpl::copyBits( const SalTwoRect& rPosAry, SalGraphics* pSrcGraphics )
{
    OpenGLSalGraphicsImpl *pImpl = pSrcGraphics ? static_cast< OpenGLSalGraphicsImpl* >(pSrcGraphics->GetImpl()) : static_cast< OpenGLSalGraphicsImpl *>(mrParent.GetImpl());
    OpenGLSalGraphicsImpl::DoCopyBits( rPosAry, *pImpl );
}

bool X11OpenGLSalGraphicsImpl::FillPixmapFromScreen( X11Pixmap* pPixmap, int nX, int nY )
{
    Display* pDisplay = mrParent.GetXDisplay();
    SalX11Screen nScreen = mrParent.GetScreenNumber();
    XVisualInfo aVisualInfo;
    XImage* pImage;
    char* pData;

    SAL_INFO( "vcl.opengl", "FillPixmapFromScreen" );

    if( !OpenGLHelper::GetVisualInfo( pDisplay, nScreen.getXScreen(), aVisualInfo ) )
        return false;

    // make sure everything is synced up before reading back
    mpContext->makeCurrent();
    glXWaitX();

    // TODO: lfrb: What if offscreen?
    pData = (char*) malloc( pPixmap->GetWidth() * pPixmap->GetHeight() * 4 );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glReadPixels( nX, GetHeight() - nY, pPixmap->GetWidth(), pPixmap->GetHeight(),
                  GL_RGBA, GL_UNSIGNED_BYTE, pData );

    pImage = XCreateImage( pDisplay, aVisualInfo.visual, 24, ZPixmap, 0, pData,
                           pPixmap->GetWidth(), pPixmap->GetHeight(), 8, 0 );
    XInitImage( pImage );
    GC aGC = XCreateGC( pDisplay, pPixmap->GetPixmap(), 0, NULL );
    XPutImage( pDisplay, pPixmap->GetDrawable(), aGC, pImage,
               0, 0, 0, 0, pPixmap->GetWidth(), pPixmap->GetHeight() );
    XFreeGC( pDisplay, aGC );
    XDestroyImage( pImage );

    CHECK_GL_ERROR();
    return true;
}

bool X11OpenGLSalGraphicsImpl::RenderPixmapToScreen( X11Pixmap* pPixmap, X11Pixmap* pMask, int nX, int nY )
{
    const int aAttribs[] = {
        GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
        GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGBA_EXT,
        None
    };
    Display* pDisplay = mrParent.GetXDisplay();
    bool bInverted;

    SAL_INFO( "vcl.opengl", "RenderPixmapToScreen (" << nX << " " << nY << ")" );

    const long nWidth = pPixmap->GetWidth();
    const long nHeight = pPixmap->GetHeight();
    SalTwoRect aPosAry(0, 0, nWidth, nHeight, nX, nY, nWidth, nHeight);

    PreDraw();
    //glClear( GL_COLOR_BUFFER_BIT );

    XSync( pDisplay, 0 );
    GLXFBConfig pFbConfig = OpenGLHelper::GetPixmapFBConfig( pDisplay, bInverted );
    GLXPixmap pGlxPixmap = glXCreatePixmap( pDisplay, pFbConfig, pPixmap->GetPixmap(), aAttribs);
    GLXPixmap pGlxMask;
    if( pMask != NULL )
        pGlxMask = glXCreatePixmap( pDisplay, pFbConfig, pMask->GetPixmap(), aAttribs);
    else
        pGlxMask = 0;
    XSync( pDisplay, 0 );

    if( !pGlxPixmap )
        SAL_WARN( "vcl.opengl", "Couldn't create GLXPixmap" );

    //TODO: lfrb: glXGetProc to get the functions

    OpenGLTexture aTexture( pPixmap->GetWidth(), pPixmap->GetHeight(), false );
    glActiveTexture( GL_TEXTURE0 );
    aTexture.Bind();
    glXBindTexImageEXT( pDisplay, pGlxPixmap, GLX_FRONT_LEFT_EXT, NULL );
    aTexture.Unbind();

    if( pMask != NULL && pGlxMask )
    {
        OpenGLTexture aMaskTexture( pMask->GetWidth(), pMask->GetHeight(), false );
        aMaskTexture.Bind();
        glXBindTexImageEXT( pDisplay, pGlxMask, GLX_FRONT_LEFT_EXT, NULL );
        aMaskTexture.Unbind();

        DrawTextureDiff( aTexture, aMaskTexture, aPosAry, bInverted );

        glXReleaseTexImageEXT( pDisplay, pGlxMask, GLX_FRONT_LEFT_EXT );
        glXDestroyPixmap( pDisplay, pGlxMask );
    }
    else
    {
        DrawTexture( aTexture, aPosAry, bInverted );
    }

    CHECK_GL_ERROR();

    glXReleaseTexImageEXT( pDisplay, pGlxPixmap, GLX_FRONT_LEFT_EXT );
    glXDestroyPixmap( pDisplay, pGlxPixmap );

    PostDraw();

    CHECK_GL_ERROR();
    return true;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
