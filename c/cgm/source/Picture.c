/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cgm/Picture.h"


NITFAPI(cgm_Picture*) cgm_Picture_construct(nitf_Error* error)
{
    cgm_Picture* picture = (cgm_Picture*)NITF_MALLOC(sizeof(cgm_Picture));
    if (!picture)
    {
	nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), 
			NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    picture->name = NULL;
    picture->colorSelectionMode = CGM_DIRECT;
    picture->edgeWidthSpec = CGM_ABSOLUTE;
    picture->lineWidthSpec = CGM_ABSOLUTE;
    picture->vdcExtent.corner1X = -1;
    picture->vdcExtent.corner2X = -1;
    picture->vdcExtent.corner1Y = -1;
    picture->vdcExtent.corner2Y = -1;

    picture->body = NULL;

    /*cgm_PictureBody_construct(error);
    if (picture->body == NULL)
    {
	cgm_PictureDestruct(& picture->body );
	}*/
    return picture;
}

NITFAPI(void) cgm_Picture_destruct(cgm_Picture** picture)
{
    if (*picture)
    {
	if ( (*picture)->body )
	    cgm_PictureBody_destruct((*picture)->body);

	if ( (*picture)->name )
	    NITF_FREE( (*picture)->name );

	NITF_FREE( *picture );
	*picture = NULL;
	
    }

}
