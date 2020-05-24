/**
 * This file is part of Sodium Language project
 *
 * Copyright © 2020 Murad Karakaþ <muradkarakas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v3.0
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 *	https://choosealicense.com/licenses/gpl-3.0/
 */

#include "pch.h"

#include "tagGeneric.h"


#define DYNAMIC_TAG_CONDITIONALS	(page->bInDataBlock > 0 && page->bInTable > 0 && page->bInTableBody > 0 /*&& page->bInTableHead == 0 && page->bInTableFoot == 0*/ && page->bInTableTr > 0)


#include "lemon.html.h"
#include "tagInput.h"


TAGGeneric *createOrMergeTAGGeneric(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTagDataBlock, int pTagTOKEN, char *tagSemanticValue) {
	
	TAGGeneric *tagGeneric = NULL;
	bool isDynamic = DYNAMIC_TAG_CONDITIONALS;

	if (pTagDataBlock->tailTAGGeneric && pTagDataBlock->tailTAGGeneric->tagInput == NULL) {
		/* this is not the first tag. so lets check if it is a candidate for merging. 
		   There are 2 posibilities for merging;  */

		// First; merging sequential static contents into just one TAGGeneric object.
		if (pTagDataBlock->tailTAGGeneric->isDynamicContent == false && isDynamic == false && pTagDataBlock->tailTAGGeneric->tagSemanticValue) {
			// merge
			char *mergedSematicValue = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
				pTagDataBlock->tailTAGGeneric->tagSemanticValue,
				tagSemanticValue,
				NULL);
			mkFree(currentMKSession->heapHandle, pTagDataBlock->tailTAGGeneric->tagSemanticValue);
			pTagDataBlock->tailTAGGeneric->tagSemanticValue = mergedSematicValue;
			tagGeneric = pTagDataBlock->tailTAGGeneric;

		// Second; merging sequential dynamic contents into just one TAGGeneric object. (Dynamic content must not be INPUT, SELECT or TEXTAREA
		} else if (pTagDataBlock->tailTAGGeneric->isDynamicContent == true && isDynamic == true && pTagDataBlock->tailTAGGeneric->tagSemanticValue && page->bInInput == 0) {
			
			// merge
			char *mergedSematicValue = mkStrcat(currentMKSession->heapHandle, __FILE__, __LINE__,
				pTagDataBlock->tailTAGGeneric->tagSemanticValue,
				tagSemanticValue,
				NULL);
			mkFree(currentMKSession->heapHandle, pTagDataBlock->tailTAGGeneric->tagSemanticValue);
			pTagDataBlock->tailTAGGeneric->tagSemanticValue = mergedSematicValue;
			tagGeneric = pTagDataBlock->tailTAGGeneric;
		}
		else {
			// if it is not a candidate for merging, then must be created.
			tagGeneric = createTAGGeneric(currentMKSession, page, pTagDataBlock, pTagTOKEN, tagSemanticValue);
		}

	}
	else {
		// this is the first tag. so it must be created (not merged). 
		tagGeneric = createTAGGeneric(currentMKSession, page, pTagDataBlock, pTagTOKEN, tagSemanticValue);
	}
	return tagGeneric;
}

TAGGeneric *createTAGGeneric(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *pTagDataBlock, int pTagTOKEN, char *tagSemanticValue) {

    TAGGeneric *tagGeneric              = mkMalloc(currentMKSession->heapHandle, sizeof(TAGGeneric), __FILE__, __LINE__ );
    tagGeneric->next                    = NULL;
    tagGeneric->tagTOKEN                = pTagTOKEN;

    if (tagSemanticValue != NULL && strlen(tagSemanticValue) > 0) {
        tagGeneric->tagSemanticValue    = (char *) mkStrdup(currentMKSession->heapHandle, tagSemanticValue, __FILE__, __LINE__);
    } else {
        tagGeneric->tagSemanticValue    = NULL;
    }
    tagGeneric->isDynamicContent        = DYNAMIC_TAG_CONDITIONALS;

    tagGeneric->tagInput            = NULL;

    if (tagGeneric->tagTOKEN == TAG_INPUT_OPEN || tagGeneric->tagTOKEN == TAG_SELECT_OPEN || tagGeneric->tagTOKEN == TAG_TEXTAREA_OPEN) {
        tagGeneric->tagInput            = getCurrentTAGDataBlockTAGInput(currentMKSession, page);
    }

    if (pTagDataBlock->rootTAGGeneric == NULL) {
        pTagDataBlock->rootTAGGeneric = tagGeneric;
    } else {
        pTagDataBlock->tailTAGGeneric->next = tagGeneric;
    }

    pTagDataBlock->tailTAGGeneric = tagGeneric;

    return tagGeneric;
}


void destroyTAGGeneric(SodiumSession *currentMKSession, HTSQLPage *page, TAGGeneric *pTagGeneric) {

    if (pTagGeneric->tagSemanticValue != NULL) {
        mkFree(currentMKSession->heapHandle, pTagGeneric->tagSemanticValue);
        pTagGeneric->tagSemanticValue = NULL;
    }
    if (pTagGeneric->tagInput != NULL) {
        destroyTAGInput(currentMKSession, page, pTagGeneric->tagInput);
        mkFree(currentMKSession->heapHandle, pTagGeneric->tagInput);
        pTagGeneric->tagInput = NULL;
    }

}
