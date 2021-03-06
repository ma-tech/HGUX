#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XmExtFF_FSBUtils_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XmExtFF_FSBUtils.c
* \author	Richard Baldock
* \date		April 2009
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \ingroup      HGU_Xm
* \brief        Utility procedures to manage a Motif file selection
*		box for reading woolz images.
*/

#include <X11/keysym.h>
#include <Xm/XmAll.h>

#include <Wlz.h>
#include <WlzExtFF.h>
#include <HGU_XmUtils.h>

static void HGU_XmFSBSelectImageTypeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	dialog;
  WlzEffFormat	format=(WlzEffFormat) client_data;
  char		strBuf[16];
  const char	*fileExt, *name;
  XmString	xmstr;
  

  /* get the dialog */
  dialog = w;
  while( dialog && (XmIsFileSelectionBox(dialog)==False) ){
    dialog = XtParent(dialog);
  }
  if( dialog == NULL ){
    return;
  }

  /* get the extension string and re-filter */
  if( (name = WlzEffStringFromFormat(format,  &fileExt)) ){
    if( format == WLZEFF_FORMAT_JPEG ){
      sprintf(strBuf, "*.jp*g*");
    }
    else {
      sprintf(strBuf, "*.%s*", fileExt);
    }
    xmstr = XmStringCreateSimple(strBuf);
    XtVaSetValues(dialog, XmNpattern, xmstr, NULL);
    XmStringFree( xmstr );
    XmFileSelectionDoSearch( dialog, NULL );
  }

  /* set the user data */
  XtVaSetValues(dialog, XmNuserData, client_data, NULL);

  return;
}

WlzErrorNum HGU_XmWriteExtFFObject(
  WlzObject				*obj,
  Widget				dialog,
  XmFileSelectionBoxCallbackStruct	*cbs,
  WlzEffFormat				*dstFmt)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  FILE		*fp;
  WlzEffFormat	format;
  String	filename;

  /* get the dialog */
  while( dialog && (XmIsFileSelectionBox(dialog)==False) ){
    dialog = XtParent(dialog);
  }
  if( dialog == NULL ){
    return WLZ_ERR_PARAM_DATA;
  }

  /* get the image type */
  if( errNum == WLZ_ERR_NONE ){
    if( dialog ){
      XtVaGetValues(dialog, XmNuserData, &format, NULL);
    }
    else {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }

  /* now write the object */
  if( errNum == WLZ_ERR_NONE ){
    switch( format ){
    case WLZEFF_FORMAT_ICS:
    case WLZEFF_FORMAT_PNM:
    case WLZEFF_FORMAT_BMP:
    case WLZEFF_FORMAT_TIFF:
      if( (filename = HGU_XmGetFileStr(dialog, cbs->value, cbs->dir)) ){
	errNum = WlzEffWriteObj(NULL, filename, obj, format);
	AlcFree( filename );
      }
      else {
	errNum = WLZ_ERR_PARAM_DATA;
      }
      break;

    case WLZEFF_FORMAT_AM:
    case WLZEFF_FORMAT_IPL:
    case WLZEFF_FORMAT_VTK:
    case WLZEFF_FORMAT_VFF:
    case WLZEFF_FORMAT_SLC:
    case WLZEFF_FORMAT_PIC:
    case WLZEFF_FORMAT_DEN:
    case WLZEFF_FORMAT_WLZ:
    case WLZEFF_FORMAT_JPEG:
      /* check file pointer */
      if( (fp = HGU_XmGetFilePointer(dialog, cbs->value,
				     cbs->dir, "wb")) == NULL ){
	errNum = WLZ_ERR_FILE_OPEN;
      }
      else {
	errNum = WlzEffWriteObj(fp, NULL, obj, format);
	fclose(fp);
      }
      break;

    case WLZEFF_FORMAT_RAW:
      /* need more info for this */
      errNum = WLZ_ERR_FILE_FORMAT;
      break;

    default:
      obj = NULL;
      errNum = WLZ_ERR_FILE_FORMAT;
      break;
    }
  }

  /* assign format return */
  if( dstFmt ){
    *dstFmt = format;
  }
  return errNum;
}

WlzObject *HGU_XmReadExtFFObject(
  Widget				dialog,
  XmFileSelectionBoxCallbackStruct	*cbs,
  WlzEffFormat				*dstFmt,
  WlzErrorNum				*dstErr)
{
  WlzObject	*obj=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  FILE		*fp=NULL;
  WlzEffFormat	format;
  String	filename;

  /* get the image type */
  if( errNum == WLZ_ERR_NONE ){
    if( dialog ){
      XtVaGetValues(dialog, XmNuserData, &format, NULL);
    }
    else {
      errNum = WLZ_ERR_PARAM_DATA;
    }
  }

  /* now read the object */
  if( errNum == WLZ_ERR_NONE ){
    switch( format ){
    case WLZEFF_FORMAT_ICS:
    case WLZEFF_FORMAT_PNM:
    case WLZEFF_FORMAT_BMP:
    case WLZEFF_FORMAT_TIFF:
      if( (filename = HGU_XmGetFileStr(dialog, cbs->value, cbs->dir)) ){
	obj = WlzEffReadObj(NULL, filename, format, 0, 0, 0, &errNum);
	AlcFree( filename );
      }
      else {
	errNum = WLZ_ERR_PARAM_DATA;
      }
      break;

    case WLZEFF_FORMAT_AM:
    case WLZEFF_FORMAT_IPL:
    case WLZEFF_FORMAT_VTK:
    case WLZEFF_FORMAT_VFF:
    case WLZEFF_FORMAT_SLC:
    case WLZEFF_FORMAT_PIC:
    case WLZEFF_FORMAT_DEN:
    case WLZEFF_FORMAT_WLZ:
    case WLZEFF_FORMAT_JPEG:
      /* check file pointer */
      if( (fp = HGU_XmGetFilePointer(dialog, cbs->value,
				     cbs->dir, "rb")) == NULL ){
	errNum = WLZ_ERR_FILE_OPEN;
      }
      else {
	obj = WlzEffReadObj(fp, NULL, format, 0, 0, 0, &errNum);
	fclose(fp);
      }
      break;

    case WLZEFF_FORMAT_RAW:
      /* need more info for this */
      errNum = WLZ_ERR_FILE_FORMAT;
      break;

    default:
      obj = NULL;
      errNum = WLZ_ERR_FILE_FORMAT;
      break;
    }
  }

  /* assign return type and error */
  if( dstFmt ){
    *dstFmt = format;
  }
  if( dstErr ){
    *dstErr = errNum;
  }
  return obj;
}


Widget HGU_XmCreateExtFFObjectFSB(
  Widget	parent,
  String	name,
  XtCallbackProc	proc,
  XtPointer		client_data)
{
  Widget	dialog=NULL;
  Arg		arg[1];
  MenuItem	*menuItems;
  Widget	form, menu;
  int		i, numFormats;
  XmString	xmstr;

  /* create the file-selection-box, add standard callbacks */
  dialog = XmCreateFileSelectionDialog(parent, name, arg, 0);
  XtAddCallback(dialog, XmNokCallback, proc, client_data);
  XtAddCallback(dialog, XmNokCallback, PopdownCallback, NULL);
  XtAddCallback(dialog, XmNcancelCallback, PopdownCallback, NULL);
  XtAddCallback(dialog, XmNmapCallback, FSBPopupCallback, NULL);

  /* add form for image-format selection */
  form = XtVaCreateManagedWidget("formatFormRC", xmRowColumnWidgetClass,
				 dialog,
				 XmNborderWidth,	0,
				 XmNnumColumns,		1,
				 NULL);

  /* build the choice menu from the ExtFF library */
  numFormats = WlzEffNumberOfFormats();
  menuItems = (MenuItem *) AlcCalloc(numFormats+1, sizeof(MenuItem));
  for(i=1; i <= numFormats; i++){
    menuItems[i-1].name = (char *) WlzEffStringFromFormat(i, NULL);
    menuItems[i-1].wclass = &xmPushButtonGadgetClass;
    menuItems[i-1].callback = HGU_XmFSBSelectImageTypeCb;
    menuItems[i-1].callback_data = (XtPointer) i;
    menuItems[i-1].tear_off_model = XmTEAR_OFF_DISABLED;
    menuItems[i-1].radio_behavior = False;
    menuItems[i-1].always_one = False;
    menuItems[i-1].subitems = NULL;
  }
  menu = HGU_XmBuildMenu(form, XmMENU_OPTION, "formatMenu", 0,
			 XmTEAR_OFF_DISABLED, menuItems);
  xmstr = XmStringCreateSimple("Image Type:");
  XtVaSetValues(menu,
		XmNlabelString,		xmstr,
		NULL);
  XtManageChild( menu );
  XmStringFree(xmstr);

  /* set woolz as the default */
  HGU_XmExtFFObjectFSBSetType(dialog, WLZEFF_FORMAT_WLZ);

  return dialog;
}

WlzEffFormat HGU_XmExtFFObjectFSBGetType(
  Widget	w,
  WlzErrorNum	*dstErr)
{
  WlzEffFormat	format;
  Widget	dialog=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* get the dialog */
  dialog = w;
  while( dialog && (XmIsFileSelectionBox(dialog)==False) ){
    dialog = XtParent(dialog);
  }
  if( dialog == NULL ){
    errNum = WLZ_ERR_PARAM_DATA;;
  }

  /* get the user data/format */
  if( errNum == WLZ_ERR_NONE ){
    XtVaGetValues(dialog, XmNuserData, &format, NULL);
  }
  else {
    format = WLZEFF_FORMAT_NONE;
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return format;
}

WlzErrorNum HGU_XmExtFFObjectFSBSetType(
  Widget	dialog,
  WlzEffFormat	format)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	menu, button;
  char		strBuf[32];
  const char	*buttonName, *fileExt;
  XmString	xmstr;

  /* set the pattern string and re-filter */
  buttonName = WlzEffStringFromFormat(format, &fileExt);
  if( format == WLZEFF_FORMAT_JPEG ){
    sprintf(strBuf, "*.jp*g*");
  }
  else {
    sprintf(strBuf, "*.%s*", fileExt);
  }
  xmstr = XmStringCreateSimple(strBuf);
  XtVaSetValues(dialog, XmNpattern, xmstr, NULL);
  XmStringFree( xmstr );
  XmFileSelectionDoSearch( dialog, NULL );
  
  /* now set the menu entry */
  if( (menu = XtNameToWidget(dialog, "*formatMenu")) ){
    sprintf(strBuf, "*.%s", buttonName);
    if( (button = XtNameToWidget(menu, strBuf)) ){
      XtVaSetValues(menu, XmNmenuHistory, button, NULL);
    }
  }
  XtVaSetValues(dialog, XmNuserData, (XtPointer) format, NULL);
  
  return errNum;
}
