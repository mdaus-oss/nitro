/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/TRE.h"
#include "nitf/PluginRegistry.h"


/* IF desc is null, look for it, if I can't load then fail */
NITFAPI(nitf_TRE *) nitf_TRE_createSkeleton(const char* tag,
                                            nitf_Error* error)
{
    nitf_TRE *tre = (nitf_TRE *) NITF_MALLOC(sizeof(nitf_TRE));

    if (!tre)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  Just in case the first malloc fails  */
    tre->handler = NULL;
    tre->priv = NULL;

    /* This happens with things like "DES" */
    size_t toCopy = NITF_MAX_TAG;
    if (strlen(tag) < NITF_MAX_TAG )
    {
        toCopy = strlen(tag);
    }
    memset(tre->tag, 0, NITF_MAX_TAG + 1);
    memcpy(tre->tag, tag, toCopy);

    return tre;
}


NITFAPI(nitf_TRE *) nitf_TRE_clone(nitf_TRE* source, nitf_Error* error)
{
    nitf_TRE *tre = NULL;

    if (source)
    {
        tre = (nitf_TRE *) NITF_MALLOC(sizeof(nitf_TRE));
        if (!tre)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }

        /* share the handler */
        tre->handler = source->handler;
        memcpy(tre->tag, source->tag, sizeof(tre->tag));

        /* call the handler clone method, if one is defined */
        if (tre->handler && tre->handler->clone)
        {
            if (!tre->handler->clone(source, tre, error))
            {
                nitf_TRE_destruct(&tre);
                return NULL;
            }
        }
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    return tre;
}



NITFAPI(void) nitf_TRE_destruct(nitf_TRE ** tre)
{
    if (*tre)
    {
        if ((*tre)->handler && (*tre)->handler->destruct)
        {
            /* let the handler destroy the private data */
            (*tre)->handler->destruct(*tre);
        }

        NITF_FREE(*tre);
        *tre = NULL;
    }
}
NITFAPI(nitf_TREHandler*) nitf_DefaultTRE_handler(nitf_Error * error);

static const nitf_TREPreloaded* findPreloadedTRE(const char* keyName)
{
    extern const nitf_TREPreloaded preloadedTREs[];
    for (size_t i = 0;; i++)
    {
        const char* pKeyName = preloadedTREs[i].name;
        if (pKeyName == NULL) // end of list
        {
            return NULL;
        }
        if (strcmp(keyName, pKeyName) == 0)
        {
            return &(preloadedTREs[i]);
        }
    }
}

/*
 *  Initialize a DSO.  The init hook is retrieved and called once
 *  when the DSO is loaded
 */
static const char** doInit(NITF_PLUGIN_INIT_FUNCTION init, const char* prefix, nitf_Error* error)
{
    /*  Else, call it  */
    const char**  ident = (*init)(error);
    if (!ident)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT, "The plugin [%s] is not retrievable", prefix);
        return NULL;
    }
    return ident;
}

static NRT_BOOL preloadTRE(const char* keyName, nitf_Error* error)
{
    const char** ident;
    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);

    /*  Construct the DLL object  */
    nitf_DLL* dll = nitf_DLL_construct(error);
    if (!dll)
    {
        return NITF_FAILURE;
    }
    dll->lib = NULL; // not a real DLL
    dll->dsoMain = NULL; // filled in after successful findPreloadedTRE()

    const nitf_TREPreloaded* plugin = findPreloadedTRE(keyName);
    if (plugin == NULL)
    {
        return NITF_FAILURE;
    }
    dll->dsoMain = (NRT_DLL_FUNCTION_PTR)plugin->handler;

    /* Now init the plugin!!!  */
    ident = doInit(plugin->init, keyName, error);
    return nitf_PluginRegistry_insertPlugin_("Successfully pre-loaded plugin: [%s] at [%p]\n", reg, ident, dll, error);
}

static nitf_TREHandler* retrievePreloadedTREHandler(nitf_PluginRegistry* reg, const char* treIdent,
    int* hadError, nitf_Error* error)
{
    if (!preloadTRE(treIdent, error))
    {
        *hadError = 1;
        return NULL;
    }

    // Successfully preloaded the TRE, it should now be in the hash table.
    return nitf_PluginRegistry_retrieveTREHandler(reg, treIdent, hadError, error);
}

static nitf_TREHandler* retrieveTREHandler(nitf_PluginRegistry* reg, const char* treIdent,
    int* hadError, nitf_Error* error)
{
    nitf_TREHandler* handler = nitf_PluginRegistry_retrieveTREHandler(reg, treIdent, hadError, error);

    if (*hadError)
    {
        *hadError = 0;
        return retrievePreloadedTREHandler(reg, treIdent, hadError, error);
    }

    // Normally, a NULL handler is **not** an error.
    if (handler == NULL)
    {
        int bad = 0;
        nitf_TREHandler* preloadedHandler = retrievePreloadedTREHandler(reg, treIdent, &bad, error);
        if (!bad)
            return preloadedHandler;
    }

    return handler;
}

NITFAPI(nitf_TRE *) nitf_TRE_construct(const char* tag,
                                       const char* id,
                                       nitf_Error * error)
{
    nitf_TRE* tre = nitf_TRE_createSkeleton(tag, error);
    if (!tre)
        return NULL;

    nitf_PluginRegistry *reg = nitf_PluginRegistry_getInstance(error);
    if (!reg)
        return NULL;

    tre->handler = NULL;
    /* if it's not a RAW id, try to load it from the registry */
    if (!id || strcmp(id, NITF_TRE_RAW) != 0)
    {
        int bad = 0;
        tre->handler = retrieveTREHandler(reg, tag, &bad, error);
        if (bad)
            return NULL;
    }
    if (!tre->handler)
    {
        tre->handler = nitf_DefaultTRE_handler(error);
        if (!tre->handler)
            return NULL;
    }

    if (tre->handler->init && !(tre->handler->init)(tre, id, error))
    {
        nitf_TRE_destruct(&tre);
        return NULL;
    }
    return tre;
}

NITFAPI(nitf_TREEnumerator*) nitf_TRE_begin(nitf_TRE* tre, nitf_Error* error)
{
    return tre->handler->begin(tre, error);
}

NITFAPI(NITF_BOOL) nitf_TRE_exists(nitf_TRE * tre, const char *tag)
{
    return (!tre) ? NITF_FAILURE :
        (nitf_TRE_getField(tre, tag) != NULL ? NITF_SUCCESS : NITF_FAILURE);
}


NITFAPI(nitf_List*) nitf_TRE_find(nitf_TRE* tre,
				  const char* pattern,
				  nitf_Error* error)
{
    return tre->handler->find(tre, pattern, error);
}


NITFAPI(NITF_BOOL) nitf_TRE_setField(nitf_TRE * tre,
                                     const char *tag,
                                     const NITF_DATA * data,
                                     size_t dataLength, nitf_Error * error)
{
    return tre->handler->setField(tre, tag, data, dataLength, error);
}

NITFAPI(nitf_Field*) nitf_TRE_getField(nitf_TRE* tre, const char* tag)
{
    return tre->handler->getField(tre, tag);
}

NITFAPI(int) nitf_TRE_getCurrentSize(nitf_TRE* tre, nitf_Error* error)
{
    return tre->handler->getCurrentSize(tre, error);
}

NITFAPI(const char*) nitf_TRE_getID(nitf_TRE* tre)
{
    return tre->handler->getID(tre);
}
