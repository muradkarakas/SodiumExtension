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

#include "tagInput.h"

#include "tagDataBlock.h"
#include "preObjectGeneric.h"
#include "htsqlVariable.h"


TAGInput *createTAGInput(SodiumSession *session, HTSQLPage *page, TAGDataBlock *parentTAGDataBlock, bool isAddedToDatablockTAGInputLinkedList) {

    TAGInput *tagInput = mkMalloc(session->heapHandle, sizeof(TAGInput), __FILE__, __LINE__);
    if (!tagInput)
        return NULL;

    // Setting default values
    tagInput->parentTAGDataBlock= parentTAGDataBlock; // do not destroy
    tagInput->name              = NULL;
    tagInput->nameWithBlockName = NULL;
    tagInput->imageFileName     = NULL;
    tagInput->imageFileSize     = 0;
    tagInput->src               = NULL;
    tagInput->masterItemName    = NULL;
	tagInput->sequenceSchemaName= NULL;
    tagInput->sequenceName      = NULL;
    tagInput->columnName        = NULL;
    tagInput->unCheckedValue    = NULL;
    tagInput->checkedValue      = NULL;
    tagInput->formatMask        = NULL;
    tagInput->htmlProperties    = NULL;
    tagInput->insertAllowed     = YES;
    tagInput->updateAllowed     = YES;
    tagInput->maxLength         = NULL;
    tagInput->defaultValue      = NULL;
    tagInput->value             = NULL;
    tagInput->dataListName      = NULL;
    tagInput->dataType          = INPUT_TAG_DATA_TYPE_CHAR;
    tagInput->inputType         = INPUT_TAG_TYPE_TEXT;
    tagInput->valueState        = INPUT_TAG_VALUE_NEW;
	tagInput->lookupItemBlockName = NULL;
    tagInput->lookupItemName	= NULL;
    tagInput->orjinalName       = NULL;
    tagInput->visibility        = true;
    tagInput->enabled           = true;

    tagInput->next              = NULL;

    if (isAddedToDatablockTAGInputLinkedList) {
        if (page->currentTAGDataBlock->rootTAGInput == NULL) {
            page->currentTAGDataBlock->rootTAGInput = tagInput;
        } else {
            page->currentTAGDataBlock->currentTAGInput->next = tagInput;
        }
        page->currentTAGDataBlock->currentTAGInput = tagInput;
    }
    return tagInput;
}

void setTAGInputPropertySrc(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, const char *pPropValue) {
    TAGInput *tagInput = findTAGInputByName(session, page, tagDataBlock, pPropName);
    if (tagInput) {
        if (tagInput->src) {
            mkFree(session->heapHandle, tagInput->src);
            tagInput->src = NULL;
        }
        if (pPropValue) {
            tagInput->src = mkStrdup(session->heapHandle, pPropValue, __FILE__, __LINE__);
        }
    }
}

void setTAGInputPropertyEnabled(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, bool propertyValue) {
    TAGInput *tagInput = findTAGInputByName(session, page, tagDataBlock, pPropName);
    if (tagInput) {
        tagInput->enabled = propertyValue;
    }
}

void setTAGInputPropertyVisibility(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, bool propertyValue) {
    TAGInput *tagInput = findTAGInputByName(session, page, tagDataBlock, pPropName);
    if (tagInput) {
        tagInput->visibility = propertyValue;
    }
}

void 
SetTAGInputPropertyImageFileName(
    SodiumSession *session, 
    HTSQLPage *page, 
    TAGDataBlock *tagDataBlock, 
    const char *pPropName, 
    const char *pPropValue
) 
{
    TAGInput *tagInput = findTAGInputByName(session, page, tagDataBlock, pPropName);
    if (tagInput) {
        if (tagInput->imageFileName) {
            mkFree(session->heapHandle, tagInput->imageFileName);
            tagInput->imageFileName = NULL;
        }
        if (pPropValue) {
            size_t len = strlen(pPropValue);
            size_t memSize = (len + 1) * sizeof(wchar_t);
            tagInput->imageFileName = mkMalloc(session->heapHandle, memSize, __FILE__, __LINE__);
            mbstowcs_s(NULL, tagInput->imageFileName, len, pPropValue, strlen(pPropValue));
            //tagInput->imageFileName = mkStrdup(session->heapHandle, pPropValue, __FILE__, __LINE__);
        }
    }
}


void setTAGInputPropertyImageFileSize(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, int pPropValue) {
    TAGInput *tagInput = findTAGInputByName(session, page, tagDataBlock, pPropName);
    if (tagInput) {
        if (pPropValue) {
            tagInput->imageFileSize = pPropValue;
        }
    }
}


void tagInputValidityCheck(SodiumSession *session, HTSQLPage *page, TAGInput *object) {

    if (object->sequenceName != NULL && (strlen(object->sequenceName) == 0) ) {
        // Sequence-name property cannot be empty
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "Sequence-name");
    }

	if (object->dataType == INPUT_TAG_TYPE_UNKNOWN) {
		// Something is wrong because input element's type is set to INPUT_TAG_TYPE_TEXT by default
	}
	
}

void setCurrentTAGInputPropertyValue(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);
	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current != NULL) {
            mkFree(session->heapHandle, current->value);
            current->value = NULL;
        }
        return;
    }
    if (current->value) {
        current->value = mkReAlloc(session->heapHandle, current->value, len + 1);
    } else {
        current->value = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    //strcpy( current->value, pPropValue);
	strcpy_s(current->value, len + 1, pPropValue);
    current->value = stripTAGPropertyValue(current->value);
    current->valueState = INPUT_TAG_VALUE_CLEAR;
}



TAGInput* findTAGInputByName(SodiumSession *session, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName) {
    if (pPropName == NULL || tagDataBlock == NULL) {
        return NULL;
    }
    TAGInput *input = tagDataBlock->rootTAGInput;
    while(input) {
        if (input->name && strcmp(input->name, pPropName) == 0) {
            return input;
        }
		input = input->next;
    }
    return NULL;
}




void destroyTAGInput(SodiumSession *session, HTSQLPage *page, TAGInput *input) {

    if(input->name) {
        mkFree(session->heapHandle, input->name);
        input->name = NULL;
    }
    if(input->nameWithBlockName) {
        mkFree(session->heapHandle, input->nameWithBlockName);
        input->nameWithBlockName = NULL;
    }
    if(input->imageFileName) {
        mkFree(session->heapHandle, input->imageFileName);
        input->imageFileName = NULL;
    }
    if(input->columnName) {
        mkFree(session->heapHandle, input->columnName);
        input->columnName = NULL;
    }
    if(input->formatMask) {
        mkFree(session->heapHandle, input->formatMask);
        input->formatMask = NULL;
    }
    if(input->defaultValue) {
        mkFree(session->heapHandle, input->defaultValue);
        input->defaultValue = NULL;
    }
    if(input->value) {
        mkFree(session->heapHandle, input->value);
        input->value = NULL;
    }
    if(input->dataListName) {
        mkFree(session->heapHandle, input->dataListName);
        input->dataListName = NULL;
    }
    if(input->htmlProperties) {
       mkFree(session->heapHandle, input->htmlProperties);
       input->htmlProperties = NULL;
    }
    if(input->lookupItemName) {
        mkFree(session->heapHandle, input->lookupItemName);
        input->lookupItemName = NULL;
    }
	if (input->lookupItemBlockName) {
		mkFree(session->heapHandle, input->lookupItemBlockName);
		input->lookupItemBlockName = NULL;
	}
    if(input->checkedValue) {
        mkFree(session->heapHandle, input->checkedValue);
        input->checkedValue = NULL;
    }
    if(input->masterItemName) {
        mkFree(session->heapHandle, input->masterItemName);
        input->masterItemName = NULL;
    }
    if(input->unCheckedValue) {
        mkFree(session->heapHandle, input->unCheckedValue);
        input->unCheckedValue = NULL;
    }
    if(input->sequenceName) {
        mkFree(session->heapHandle, input->sequenceName);
        input->sequenceName = NULL;
    }
    if(input->orjinalName) {
        mkFree(session->heapHandle, input->orjinalName);
        input->orjinalName = NULL;
    }
	if (input->maxLength) {
		mkFree(session->heapHandle, input->maxLength);
		input->maxLength = NULL;
	}
}

char *printDataBlockDataItemInstanceTypeSelect(SodiumSession *session, HTSQLPage *page, TAGInput *inputInstance) {
    char *tmpcat;
    char *str = mkStrcat(session->heapHandle, __FILE__, __LINE__,"<select name=\"", inputInstance->name, "\" ", classSelect, NULL);

    if (inputInstance->masterItemName != NULL && page->currentRequestMethodType == REQUEST_TYPE_POST) {
        // This is a detail block item (foreing key column) in the master-detail relationship.
        // If item has no value and the call is REQUEST_TYPE_POST, we will print master item value instead.
        TAGDataBlock *masterBlock = findTAGDataBlockByName(session, page, inputInstance->parentTAGDataBlock->masterBlockName);
        TAGInput *input = findTAGDataBlockCurrentRowTAGInputItem(session, masterBlock, inputInstance->masterItemName);
        if (input->value) {
            switch (input->dataType) {
                case INPUT_TAG_DATA_TYPE_CHAR:
                case INPUT_TAG_DATA_TYPE_NUMBER: {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", input->value, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                    break;
                }
                case INPUT_TAG_DATA_TYPE_DATE: {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", input->value, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                    break;
                }
                case INPUT_TAG_DATA_TYPE_UNKNOWN: {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"UNKNOWN TYPE\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                    break;
                }
            }
        }
    } else {
        // No master-detail relationship. So we are printing the value of this item instead of master item's value
        switch (inputInstance->dataType) {
            case INPUT_TAG_DATA_TYPE_CHAR:
            case INPUT_TAG_DATA_TYPE_NUMBER:
            case INPUT_TAG_DATA_TYPE_DATE: {
                if (inputInstance->dataListName) {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " datalist-name=\"", inputInstance->dataListName, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                }
                if (inputInstance->value) {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                }
                if (inputInstance->lookupItemName && strlen(inputInstance->lookupItemName) > 0) {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " lookup-input-name=\"", inputInstance->lookupItemName, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                }
				if (inputInstance->lookupItemBlockName && strlen(inputInstance->lookupItemBlockName) > 0) {
					tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " lookup-input-block-name=\"", inputInstance->lookupItemBlockName, "\"", NULL);
					mkFree(session->heapHandle, str);
					str = tmpcat;
				}
                if (inputInstance->columnName && strlen(inputInstance->columnName) > 0) {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " column-name=\"", inputInstance->columnName, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                }
                break;
            }
            case INPUT_TAG_DATA_TYPE_UNKNOWN: {
                break;
            }
        }
    }

    if (inputInstance->htmlProperties) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
        mkFree(session->heapHandle, str);
    str = tmpcat;
    }
    if (inputInstance->parentTAGDataBlock) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->masterItemName) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " master-item-name=\"", inputInstance->masterItemName, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, "></select>", NULL);
    mkFree(session->heapHandle, str);
    str = tmpcat;

    return str;
}

char *printDataBlockDataItemInstanceTypeText(SodiumSession *session, HTSQLPage *page, TAGInput *inputInstance) {

    char *type, *inputAutoWidth=NULL, *maxLengthStr=NULL;

    if (inputInstance->inputType == INPUT_TAG_TYPE_PASSWORD) {
        type = "password";
    } else if (inputInstance->inputType == INPUT_TAG_TYPE_TEXT) {

		if (inputInstance->maxLength) {
			maxLengthStr = mkStrcat(session->heapHandle, __FILE__, __LINE__, " maxlength=\"", inputInstance->maxLength, "\" ", NULL);

			char int2Str[10] = "450"; // maximum width for character filed is 450px. Must be "textarea" for bigger
			if (atoi(inputInstance->maxLength) < 150) {
				mkItoa(atoi(inputInstance->maxLength) * INPUT_TEXT_WIDTH_PER_CHARACTER, int2Str);
			}
			inputAutoWidth = mkStrcat(session->heapHandle, __FILE__, __LINE__,
				" style=\"width: ",
				int2Str,
				"px\" ",
				NULL);
		}
        type = "text";
    } else if (inputInstance->inputType == INPUT_TAG_TYPE_HIDDEN) {
        type = "hidden";
    }
	if (inputInstance->dataType == INPUT_TAG_DATA_TYPE_NUMBER) {
		inputAutoWidth = mkStrdup(session->heapHandle, " style=\"width: 50px\" ", __FILE__, __LINE__);
	}

	// input item tag and its mandatory properties are constructing
    char *str = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                                            "<input",
                                            " name=\"", inputInstance->name, "\"",
                                            " type=\"", type, "\"", 
											classText,
											(inputAutoWidth) ? inputAutoWidth : "",
											(maxLengthStr) ? maxLengthStr: "",
											NULL);
	// releasing auto with variable if exists
	if (inputAutoWidth) {
		mkFree(session->heapHandle, inputAutoWidth);
	}
	if (maxLengthStr) {
		mkFree(session->heapHandle, maxLengthStr);
	}
	// visibility attribute
    if (inputInstance->visibility == false) {
        char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                             str,
                             " style=\"visibility: hidden\"",
                             NULL);
        mkFree(session->heapHandle, str);
        str = tmp;
    }
    if (inputInstance->enabled == false) {
        char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                             str,
                             "  disabled ",
                             NULL);
        mkFree(session->heapHandle, str);
        str = tmp;
    }
    if (inputInstance->masterItemName != NULL && page->currentRequestMethodType == REQUEST_TYPE_POST) {
        // This is a detail block item (foreing key column) in the master-detail relationship.
        // If item has no value and the call is REQUEST_TYPE_POST, we will print master item value instead.
        TAGDataBlock *masterBlock = findTAGDataBlockByName(session, page,  inputInstance->parentTAGDataBlock->masterBlockName);

		if (masterBlock) {
			// It is a datablock
			TAGInput *input = findTAGDataBlockCurrentRowTAGInputItem(session, masterBlock, inputInstance->masterItemName);
			if (input) {
				if (input->value) {
					switch (input->dataType) {
						case INPUT_TAG_DATA_TYPE_CHAR:
						case INPUT_TAG_DATA_TYPE_NUMBER: {
							char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", input->value, "\"", NULL);
							mkFree(session->heapHandle, str);
							str = tmp;
							break;
						}
						case INPUT_TAG_DATA_TYPE_DATE: {
							char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", input->value, "\"", NULL);
							mkFree(session->heapHandle, str);
							str = tmp;
							break;
						}
						case INPUT_TAG_DATA_TYPE_UNKNOWN: {
							break;
						}
					}
				}
			}			
		}
		else {
			// It is not a data block. Maybe Session variable
			if (inputInstance && inputInstance->parentTAGDataBlock && inputInstance->parentTAGDataBlock->masterBlockName &&
				inputInstance && inputInstance->masterItemName) {
				char *variableName = mkStrcat(session->heapHandle, __FILE__, __LINE__,	":", inputInstance->parentTAGDataBlock->masterBlockName, ".", inputInstance->masterItemName, NULL);
				// Searching in session variables
				PreObjectGeneric *preObjectGeneric = findSessionGenericObject(session, variableName);
				if (preObjectGeneric && preObjectGeneric->value.preObjectVariable->value.text) {
					// found.
					char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", preObjectGeneric->value.preObjectVariable->value.text, "\"", NULL);
					mkFree(session->heapHandle, str);
					str = tmp;
				}
				mkFree(session->heapHandle, variableName);
			}			
		}

    } else {
        // No master-detail relationship. So we are printing the value of this item instead of master item's value
        switch (inputInstance->dataType) {
            case INPUT_TAG_DATA_TYPE_CHAR: {
                if (inputInstance->value) {
                    char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmp;
                }
                break;
            }
            case INPUT_TAG_DATA_TYPE_NUMBER: {
                if (inputInstance->value) {
                    char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmp;
                }
                break;
            }
            case INPUT_TAG_DATA_TYPE_DATE: {
                if (inputInstance->value) {
                    char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmp;
                }
                break;
            }
            case INPUT_TAG_DATA_TYPE_UNKNOWN: {
                break;
            }
        }

    }

    if (inputInstance->htmlProperties) {
        char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
        mkFree(session->heapHandle, str);
        str = tmp;
    }
    if (inputInstance->parentTAGDataBlock != NULL) {
        char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmp;
    }
    if (inputInstance->masterItemName != NULL) {
        char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " master-item-name=\"", inputInstance->masterItemName, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmp;
    }

    char *tmp = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, ">", NULL);
    mkFree(session->heapHandle, str);
    str = tmp;

    return str;
}

char *printDataBlockDataItemInstanceTypeTextArea(SodiumSession *session, HTSQLPage *page, TAGInput *inputInstance) {
    char *tmpcat;
    char *str = mkStrcat(session->heapHandle, __FILE__, __LINE__, "<textarea name=\"", inputInstance->name, "\" ", classTextArea, NULL);

    if (inputInstance->masterItemName != NULL && page->currentRequestMethodType == REQUEST_TYPE_POST) {
        //  Textarea element cannot be referenced as a foreign-key column
		CLOG(ERR_TEXTAREA_CANNOT_BE_FK, inputInstance->name);
    }
    if (inputInstance->htmlProperties != NULL) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->parentTAGDataBlock != NULL) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->value && strlen(inputInstance->value) > 0) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, ">", inputInstance->value, "</textarea>", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    } else {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, "></textarea>", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    return str;
}

char *printDataBlockDataItemInstanceTypeCheckBox(SodiumSession *session, HTSQLPage *page, TAGInput *inputInstance, char *str) {
    char *tmpcat;
    str = mkStrcat(session->heapHandle, __FILE__, __LINE__, "<input name=\"", inputInstance->name, "\" type=\"checkbox\" ", classCheckBox, NULL);

    if (inputInstance->unCheckedValue) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " unchecked-value=\"", inputInstance->unCheckedValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->checkedValue) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " checked-value=\"", inputInstance->checkedValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->defaultValue) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " default-value=\"", inputInstance->defaultValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }

    switch (inputInstance->dataType) {

        case INPUT_TAG_DATA_TYPE_CHAR: {
            if (inputInstance->value && inputInstance->checkedValue) {
                // If we get db value, compare it with the checkedValue
                if (strcmp(inputInstance->value, inputInstance->checkedValue) == 0) {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " checked ", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                }
            } else {
                //we get nothing from database, we are checking default value
                if (inputInstance->defaultValue == NULL || strlen(inputInstance->defaultValue)==0) {
                    // If we get nothing from database and no default value is provided, this means default value and db value is matched/equal.
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " checked ", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                } else {
                    // if we get nothing from db and default value is different than empty string, this means that input box appears unchecked
                }
            }
            break;
        }
        case INPUT_TAG_DATA_TYPE_NUMBER: {
            if (inputInstance->value) {
                tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                mkFree(session->heapHandle, str);
                str = tmpcat;
            }
            break;
        }
        case INPUT_TAG_DATA_TYPE_DATE: {
            if (inputInstance->value) {
                tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                mkFree(session->heapHandle, str);
                str = tmpcat;
            }
            break;
        }
        case INPUT_TAG_DATA_TYPE_UNKNOWN: {
            // invalid property
			CLOG(ERR_PROPERTY_INVALID, "data-type");
            break;
        }
    }

    if (inputInstance->htmlProperties != NULL) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->parentTAGDataBlock != NULL) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, "/>", NULL); // </input>
    mkFree(session->heapHandle, str);
    str = tmpcat;

    return str;
}

char *printDataBlockDataItemInstanceTypeRadioButton(SodiumSession *session, HTSQLPage *page, TAGInput *inputInstance, char *str) {
    char *tmpcat;

    /** For radio groups, we must generate a new name for each row. */
    str = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                    "<input "
                    " name=\"", inputInstance->orjinalName, "_", inputInstance->parentTAGDataBlock->keyColumnValue, "\"",
                    " orjinal-name=\"", inputInstance->orjinalName, "\" ",
					classButton,
                    NULL);

    /** We are generating a special property for finding data block current row item instance correctly when user modifies the datablock row */
    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " server-current-row-name=\"", inputInstance->name, "\"", NULL);
    mkFree(session->heapHandle, str);
    str = tmpcat;

    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " type=\"radio\"", NULL);
    mkFree(session->heapHandle, str);
    str = tmpcat;

    if (inputInstance->checkedValue) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " checked-value=\"", inputInstance->checkedValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->defaultValue) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " default-value=\"", inputInstance->defaultValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    switch (inputInstance->dataType) {

        case INPUT_TAG_DATA_TYPE_CHAR: {
            if (inputInstance->value && inputInstance->checkedValue) {
                // If we get db value, compare it with the checkedValue
                if (strcmp(inputInstance->value, inputInstance->checkedValue) == 0) {
                    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " checked ", NULL);
                    mkFree(session->heapHandle, str);
                    str = tmpcat;
                }
            } else {
                //we get nothing from database, we are checking default value
                if (inputInstance->defaultValue == NULL || strlen(inputInstance->defaultValue)==0) {
                    // If we get nothing from database and no default value is provided, this means default value and db value is matched/equal.
                    //str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " checked ", NULL);
                } else {
                    // if we get nothing from db and default value is different than empty string
                    if (strcmp(inputInstance->defaultValue, inputInstance->checkedValue) == 0) {
                        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " checked ", NULL);
                        mkFree(session->heapHandle, str);
                        str = tmpcat;
                    }
                }
            }
            break;
        }
        case INPUT_TAG_DATA_TYPE_NUMBER: {
            if (inputInstance->value) {
                tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                mkFree(session->heapHandle, str);
                str = tmpcat;
            }
            break;
        }
        case INPUT_TAG_DATA_TYPE_DATE: {
            if (inputInstance->value) {
                tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " value=\"", inputInstance->value, "\"", NULL);
                mkFree(session->heapHandle, str);
                str = tmpcat;
            }
            break;
        }
        case INPUT_TAG_DATA_TYPE_UNKNOWN: {
            // invalid property
			CLOG(ERR_PROPERTY_INVALID, "data-type");
            break;
        }
    }

    if (inputInstance->htmlProperties != NULL) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->parentTAGDataBlock != NULL) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, "/>", NULL); //</input>
    mkFree(session->heapHandle, str);
    str = tmpcat;

    return str;
}


void printDataBlockDataItemInstance(SodiumSession *session, HTSQLPage *page, TAGInput *inputInstance) {

    char *str = NULL;
    char *tmpcat = NULL;

    switch (inputInstance->inputType) {

        case INPUT_TAG_TYPE_CHECKBOX : {
            str = printDataBlockDataItemInstanceTypeCheckBox(session, page, inputInstance, str);
            break;
        }

        case INPUT_TAG_TYPE_FILE : {
            tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " name=\"", inputInstance->name, "\" ", " type=\"file\"", NULL);
            mkFree(session->heapHandle, str);
            str = tmpcat;
            break;
        }

        case INPUT_TAG_TYPE_IMAGE : {
            str = printDataBlockDataItemInstanceTypeImage(session, inputInstance);
            break;
        }

        case INPUT_TAG_TYPE_RADIO : {
            str = printDataBlockDataItemInstanceTypeRadioButton(session, page, inputInstance, str);
            break;
        }
        case INPUT_TAG_TYPE_RESET : {

            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, "<input", NULL);
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " name=\"", inputInstance->name, "\"", NULL);
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " type=\"reset\"", NULL);
            if (inputInstance->htmlProperties != NULL) {
                str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
            }
            if (inputInstance->parentTAGDataBlock != NULL) {
                str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
            }
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, ">", NULL);
            break;
        }

        case INPUT_TAG_TYPE_SUBMIT : {
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, "<input", NULL);
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " name=\"", inputInstance->name, "\"", NULL);
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " type=\"submit\"", NULL);
            if (inputInstance->htmlProperties != NULL) {
                str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
            }
            if (inputInstance->parentTAGDataBlock != NULL) {
                str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
            }
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, ">", NULL);
            break;
        }

        case INPUT_TAG_TYPE_BUTTON : {
            str = mkStrcat(session->heapHandle, __FILE__, __LINE__, "<input name=\"", inputInstance->name, "\" type=\"button\" value=\"", inputInstance->value, "\" ", classButton, NULL);
            if (inputInstance->htmlProperties) {
                tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
                mkFree(session->heapHandle, str);
                str = tmpcat;
            }
            if (inputInstance->parentTAGDataBlock != NULL) {
                tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
                mkFree(session->heapHandle, str);
                str = tmpcat;
            }
            tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, ">", NULL);
            mkFree(session->heapHandle, str);
            str = tmpcat;
            break;
        }

        case INPUT_TAG_TYPE_TEXTAREA: {
            str = printDataBlockDataItemInstanceTypeTextArea(session, page, inputInstance);
            break;
        }

        case INPUT_TAG_TYPE_HIDDEN :
        case INPUT_TAG_TYPE_PASSWORD :
        case INPUT_TAG_TYPE_TEXT : {
            str = printDataBlockDataItemInstanceTypeText(session, page, inputInstance);
            break;
        }

        case INPUT_TAG_TYPE_SELECT : {
            str = printDataBlockDataItemInstanceTypeSelect(session, page, inputInstance);
            break;
        }
        case INPUT_TAG_TYPE_UNKNOWN : {
			char *errMsg = mkStrcat(session->heapHandle, __FILE__, __LINE__, "Unknown input element type:", (inputInstance && inputInstance->name) ? inputInstance->name : "", NULL);
			CLOG(ERR_CORE_INVALID_VALUE, errMsg);
			if (str && inputInstance && inputInstance->name) {
				str = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " name=\"", inputInstance->name, "\"", NULL);
			}            
			mkFree(session->heapHandle, errMsg);
        }
    }

    mkPrint(session, str, NULL);
    mkFree(session->heapHandle, str);  
}


char *printDataBlockDataItemInstanceTypeImage(SodiumSession *session, TAGInput *inputInstance) {
    char *str, *tmpcat;

    const char *rowid = inputInstance->parentTAGDataBlock->keyColumnValue;

    str = mkStrcat(session->heapHandle, __FILE__, __LINE__, "<input",
                                     " name=\"", inputInstance->name, "\"",
                                     " type=\"image\"",
                                     " value=\"\" ",
									classImage,
                                     NULL);
    char *randomStr = mkStrRandom(session, 3, __FILE__, __LINE__);
    if (inputInstance->columnName) {
        /** It is not a formula column */
        if (inputInstance->parentTAGDataBlock) {

			if (strncmp(rowid, "NEWROW-", 7) == 0) {
				tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__,
					str,
					"src=\"htsql_noimage.png\" ",
					"datablockname=\"", inputInstance->parentTAGDataBlock->dataBlockName, "\" ",
					NULL);
			}
			else {
				char *urlEncoded = NULL;
				if (inputInstance->parentTAGDataBlock->keyColumnValue) {
					urlEncoded = url_encode(session, inputInstance->parentTAGDataBlock->keyColumnValue);
				}
				tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__,
					str,
					" src=\"?nativeproceduretorun=getimage&",
					"datablockname=", inputInstance->parentTAGDataBlock->dataBlockName,
					"&",
					"itemname=", inputInstance->name,
					"&",
					"rowid=", (urlEncoded) ? urlEncoded:"",
					"&",
					randomStr,  // this is for preventing browser to cache image file content. Some browser(s) ignores cache control directives
					"\"",
					NULL);
				if (urlEncoded) {
					mkFree(session->heapHandle, urlEncoded);
				}
			}
            mkFree(session->heapHandle, str);
            str = tmpcat;
        }
    } else {
        /** It is a formula column and image! Its value must be in "src" property as Base64 encoded. Lets check */
        if (inputInstance->src) {
            // Got it
            tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__,
                          str,
                          " src=\"", inputInstance->src, "\"",
                          inputInstance->parentTAGDataBlock->dataBlockName,
                          "&",
                          inputInstance->name,
                          "&",
                          inputInstance->parentTAGDataBlock->keyColumnValue,
                          "&",
                          randomStr,  // this is for preventing browser to cache image file content. Some browser(s) ignores cache control directives
                          "\"",
                          NULL);
            mkFree(session->heapHandle, str);
            str = tmpcat;
        }
    }
    mkFree(session->heapHandle, randomStr);

    if (inputInstance->htmlProperties) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " ", inputInstance->htmlProperties, NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    if (inputInstance->parentTAGDataBlock != NULL) {
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, " rowid=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"", NULL);
        mkFree(session->heapHandle, str);
        str = tmpcat;
    }
    tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, "/>", NULL); //</input>
    mkFree(session->heapHandle, str);
    str = tmpcat;

    if (inputInstance->columnName) {
        /** Creating link for image uploading if it is not a formula column */
        char *upload =  mkStrcat(session->heapHandle, __FILE__, __LINE__,
                        "<br/>"
                        "<form method=\"post\" enctype=\"multipart/form-data\" action=\"\"",
                        " onsubmit=\"uploadImageFile(this); return false;\" id=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\">"
                        " <input type=\"hidden\" name=\"nativeproceduretorun\" value=\"putimage\"/>"
                        " <input type=\"hidden\" name=\"datablockname\" value=\"", inputInstance->parentTAGDataBlock->dataBlockName, "\"/>"
                        " <input type=\"hidden\" name=\"input-name\" value=\"", inputInstance->name, "\"/>"
                        " <input type=\"hidden\" name=\"rowid\" value=\"", inputInstance->parentTAGDataBlock->keyColumnValue, "\"/>"
                        " <input type=\"file\"   name=\"filename\"/>"
                        " <button type=\"submit\">Submit</button>"
                        "</form>",
                        NULL);
        tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, str, upload, NULL);
        mkFree(session->heapHandle, str);
        mkFree(session->heapHandle, upload);
        str = tmpcat;
    }

    return str;
}

TAGInput *getCurrentTAGDataBlockTAGInput(SodiumSession *session, HTSQLPage *page) {
    return page->currentTAGDataBlock->currentTAGInput;
}

void setCurrentTAGInputPropertyNameWithNameAndValuePair(SodiumSession *session, HTSQLPage *page, TAGInput *input) {
	
	if (input) {
		if (input->orjinalName) {
			mkFree(session->heapHandle, input->orjinalName);
			input->orjinalName = NULL;
		}
		input->orjinalName = mkStrdup(session->heapHandle, input->name, __FILE__, __LINE__);

		char *newName = mkStrcat(session->heapHandle, __FILE__, __LINE__, input->name, "_", input->checkedValue, NULL);
		if (input->name) {
			mkFree(session->heapHandle, input->name);
			input->name = NULL;
		}
		input->name = newName;
		setTAGDatablockFullItemName(session, page, input);
	}
    
}

void setCurrentTAGInputPropertyMaxLength(SodiumSession *session, HTSQLPage *page, TAGInput *input, const char *pPropValue) {
	char *pPropValueTemp = mkStrdup(session->heapHandle, pPropValue, __FILE__, __LINE__);
	pPropValueTemp = stripTAGPropertyValue(pPropValueTemp);
	input->maxLength = pPropValueTemp;
}

void setCurrentTAGInputPropertyName(SodiumSession *session, HTSQLPage *page, TAGInput *input, const char *pPropValue) {

    if (strcmp(pPropValue, "\"\"") == 0) {
		CLOG(ERR_PROPERTY_CANNOT_BE_EMPTY, "name");
        return;
    }

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (input != NULL) {
            mkFree(session->heapHandle, input->name);
            input->name = NULL;
        }
        return;
    }
    if (input->name != NULL) {
        input->name = mkReAlloc(session->heapHandle, input->name, len + 1);
    } else {
        input->name = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }

    strcpy_s(input->name, len + 1, pPropValue);
    input->name = stripTAGPropertyValue(input->name);

    setTAGDatablockFullItemName(session, page, input);
}

void setTAGDatablockFullItemName(SodiumSession *session, HTSQLPage *page, TAGInput *input) {
    TAGDataBlock *tagCurrentDataBlock = getCurrentTAGDataBlock(session, page);

    if (input->nameWithBlockName) {
        mkFree(session->heapHandle, input->nameWithBlockName);
        input->nameWithBlockName = NULL;
    }
    input->nameWithBlockName = mkStrcat(session->heapHandle, __FILE__, __LINE__, ":", tagCurrentDataBlock->dataBlockName, ".", input->name, NULL);
}

void setCurrentTAGInputPropertySequenceName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
    TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
    size_t len = strlen(pPropValue);
    if (len == 0) {
        if (input != NULL) {
            mkFree(session->heapHandle, input->sequenceName);
            input->sequenceName = NULL;
        }
        return;
    }
    if (input->sequenceName != NULL) {
        input->sequenceName = mkReAlloc(session->heapHandle, input->sequenceName, len + 1);
    } else {
        input->sequenceName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(input->sequenceName, len+1,pPropValue);
    input->sequenceName = stripTAGPropertyValue(input->sequenceName);
}

void setCurrentTAGInputPropertySequenceSchemaName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
	TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);
	size_t len = strlen(pPropValue);
	if (len == 0) {
		if (input != NULL) {
			mkFree(session->heapHandle, input->sequenceSchemaName);
			input->sequenceSchemaName = NULL;
		}
		return;
	}
	if (input->sequenceSchemaName != NULL) {
		input->sequenceSchemaName = mkReAlloc(session->heapHandle, input->sequenceSchemaName, len + 1);
	}
	else {
		input->sequenceSchemaName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
	}
	strcpy_s(input->sequenceSchemaName, len + 1, pPropValue);
	input->sequenceSchemaName = stripTAGPropertyValue(input->sequenceSchemaName);
}

void setCurrentTAGInputPropertyMasterItemName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {

    TAGInput *input = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (input != NULL) {
            mkFree(session->heapHandle, input->masterItemName);
            input->masterItemName = NULL;
        }
        return;
    }
    if (input->masterItemName != NULL) {
        input->masterItemName = mkReAlloc(session->heapHandle, input->masterItemName, len + 1);
    } else {
        input->masterItemName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(input->masterItemName, len+1, pPropValue);
    input->masterItemName = stripTAGPropertyValue(input->masterItemName);
}

void setCurrentTAGInputPropertyType(SodiumSession *session, HTSQLPage *page, TAGInput *current, const char *pPropValue) {
    
	if (strcmp(pPropValue,"\"text\"") == 0) {

        if (current->inputType != INPUT_TAG_TYPE_UNKNOWN) {
            if (current->inputType == INPUT_TAG_TYPE_TEXTAREA) {
				CLOG(ERR_PROPERTY_INVALID, "'textarea' cannot have type property");
			}         
        } else {
            current->inputType = INPUT_TAG_TYPE_TEXT;
        }
    } else if (strcmp(pPropValue,"\"password\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_PASSWORD;
    } else if (strcmp(pPropValue,"\"checkbox\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_CHECKBOX;
    } else if (strcmp(pPropValue,"\"radio\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_RADIO;
    } else if (strcmp(pPropValue,"\"submit\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_SUBMIT;
    } else if (strcmp(pPropValue,"\"reset\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_RESET;
    } else if (strcmp(pPropValue,"\"file\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_FILE;
    } else if (strcmp(pPropValue,"\"hidden\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_HIDDEN;
    } else if (strcmp(pPropValue,"\"image\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_IMAGE;
        // Setting automatic properties imageFileName property
        if (current->imageFileName) {
            mkFree(session->heapHandle, current->imageFileName);
            current->imageFileName = NULL;
        }
        int memSize = sizeof(wchar_t) * MAX_PATH;
        current->imageFileName = mkMalloc(session->heapHandle, memSize, __FILE__, __LINE__);
        wcsncpy_s(current->imageFileName, MAX_PATH, getCurrentDirectory(session), MAX_PATH);  
        wchar_t wcTemp[100];
        mbstowcs_s(NULL, wcTemp, 100, current->parentTAGDataBlock->dataBlockName, strlen(current->parentTAGDataBlock->dataBlockName));
        wcscat_s(current->imageFileName, MAX_PATH, wcTemp);
        wcscat_s(current->imageFileName, MAX_PATH, L"_");
        mbstowcs_s(NULL, wcTemp, 100, current->name, strlen(current->name));
        wcscat_s(current->imageFileName, MAX_PATH, wcTemp);
        wcscat_s(current->imageFileName, MAX_PATH, L".jpg");

        /*current->imageFileName = mkStrcat(session->heapHandle, __FILE__, __LINE__, currentDirectory, "\\", 
									current->parentTAGDataBlock->dataBlockName, "_", current->name, ".jpg", NULL);
        if (currentDirectory) {
            mkFree(session->heapHandle, currentDirectory);
        }*/
    } else if (strcmp(pPropValue,"\"button\"") == 0) {
        current->inputType = INPUT_TAG_TYPE_BUTTON;
    } else if (strcmp(pPropValue,"select") == 0) {
        current->inputType = INPUT_TAG_TYPE_SELECT;
    } else if (strcmp(pPropValue,"textarea") == 0) {
        current->inputType = INPUT_TAG_TYPE_TEXTAREA;
    }
}


void setCurrentTAGInputPropertyDefaultValue(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {

    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current != NULL) {
            mkFree(session->heapHandle, current->defaultValue);
            current->defaultValue = NULL;
        }
        return;
    }
    if (current->defaultValue != NULL) {
        current->defaultValue = mkReAlloc(session->heapHandle, current->defaultValue, len + 1);
    } else {
        current->defaultValue = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( current->defaultValue, len+1, pPropValue);
    current->defaultValue = stripTAGPropertyValue(current->defaultValue);
}

void setCurrentTAGInputCheckedValue(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {

    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current != NULL) {
            mkFree(session->heapHandle, current->checkedValue);
            current->checkedValue = NULL;
        }
        return;
    }
    if (current->checkedValue != NULL) {
        current->checkedValue = mkReAlloc(session->heapHandle, current->checkedValue, len + 1);
    } else {
        current->checkedValue = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( current->checkedValue, len+1, pPropValue);
    current->checkedValue = stripTAGPropertyValue(current->checkedValue);
}

void setCurrentTAGInputUncheckedValue(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {

    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current != NULL) {
            mkFree(session->heapHandle, current->unCheckedValue);
            current->unCheckedValue = NULL;
        }
        return;
    }
    if (current->unCheckedValue != NULL) {
        current->unCheckedValue = mkReAlloc(session->heapHandle, current->unCheckedValue, len + 1);
    } else {
        current->unCheckedValue = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( current->unCheckedValue, len+1, pPropValue);
    current->unCheckedValue = stripTAGPropertyValue(current->unCheckedValue);
}


void setCurrentTAGInputPropertyColumnName(SodiumSession *session, HTSQLPage *page, TAGInput *tagInput, const char *pPropValue) {

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (tagInput != NULL) {
            mkFree(session->heapHandle, tagInput->columnName);
			tagInput->columnName = NULL;
        }
        return;
    }
    if (tagInput->columnName != NULL) {
		tagInput->columnName = mkReAlloc(session->heapHandle, tagInput->columnName, len + 1);
    } else {
		tagInput->columnName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(tagInput->columnName, len+1, pPropValue);
	tagInput->columnName = stripTAGPropertyValue(tagInput->columnName);
}



void setCurrentTAGInputPropertyFormatMask(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {

    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current != NULL) {
            mkFree(session->heapHandle, current->formatMask);
            current->formatMask = NULL;
        }
        return;
    }
    if (current->formatMask != NULL) {
        current->formatMask = mkReAlloc(session->heapHandle, current->formatMask, len + 1);
    } else {
        current->formatMask = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->formatMask, len+1, pPropValue);
    current->formatMask = stripTAGPropertyValue(current->formatMask);
}

void setCurrentTAGInputPropertyDatalistName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current != NULL) {
            mkFree(session->heapHandle, current->dataListName);
            current->dataListName = NULL;
        }
        return;
    }
    if (current->dataListName != NULL) {
        current->dataListName = mkReAlloc(session->heapHandle, current->dataListName, len + 1);
    } else {
        current->dataListName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s( current->dataListName, len+1, pPropValue);
    current->dataListName = stripTAGPropertyValue(current->dataListName);
}



void setCurrentTAGInputPropertyLookupItemName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
    if (len == 0) {
        if (current != NULL) {
            mkFree(session->heapHandle, current->lookupItemName);
            current->lookupItemName = NULL;
        }
        return;
    }
    if (current->lookupItemName != NULL) {
        current->lookupItemName = mkReAlloc(session->heapHandle, current->lookupItemName, len + 1);
    } else {
        current->lookupItemName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
    }
    strcpy_s(current->lookupItemName, len+1, pPropValue);
    current->lookupItemName = stripTAGPropertyValue(current->lookupItemName);
}

void setCurrentTAGInputPropertyLookupItemBlockName(SodiumSession *session, HTSQLPage *page, const char *pPropValue) {
	TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);

	size_t len = strlen(pPropValue);
	if (len == 0) {
		if (current != NULL) {
			mkFree(session->heapHandle, current->lookupItemBlockName);
			current->lookupItemBlockName = NULL;
		}
		return;
	}
	if (current->lookupItemBlockName != NULL) {
		current->lookupItemBlockName = mkReAlloc(session->heapHandle, current->lookupItemBlockName, len + 1);
	}
	else {
		current->lookupItemBlockName = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
	}
	strcpy_s(current->lookupItemBlockName, len + 1, pPropValue);
	current->lookupItemBlockName = stripTAGPropertyValue(current->lookupItemBlockName);
}

void setCurrentTAGInputPropertyDataType(SodiumSession *session, HTSQLPage *page, TAGInput *current, const  char *pPropValue) {
    if (strcmp(pPropValue, "char") == 0) {
        current->dataType = INPUT_TAG_DATA_TYPE_CHAR;
    } else if (strcmp(pPropValue, "number") == 0) {
        current->dataType = INPUT_TAG_DATA_TYPE_NUMBER;
    } else if (strcmp(pPropValue, "date") == 0) {
        current->dataType = INPUT_TAG_DATA_TYPE_DATE;
    } else {
		CLOG(ERR_PROPERTY_INVALID, "data-type");
    }
}


void setCurrentTAGInputPropertyInsertAllowed(SodiumSession *session, HTSQLPage *page, char *pPropValue) {
    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);
    if ( strcmp(pPropValue, "\"yes\"") == 0 ) {
         current->insertAllowed = YES;
    } else if ( strcmp(pPropValue, "\"no\"") == 0 ) {
        current->insertAllowed = NO;
    } else {
		CLOG(ERR_PROPERTY_INVALID, "[yes | no]");
    }
}



void setCurrentTAGInputPropertyUpdateAllowed(SodiumSession *session, HTSQLPage *page, char *pPropValue) {
    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);
    if ( strcmp(pPropValue, "\"yes\"") == 0 ) {
         current->updateAllowed = YES;
    } else if ( strcmp(pPropValue, "\"no\"") == 0 ) {
        current->updateAllowed = NO;
    } else {
		CLOG(ERR_PROPERTY_INVALID, "[yes | no]");
    }
}

void setCurrentTAGInputPropertyAppendHTMLProperty(SodiumSession *session, HTSQLPage *page, char *pPropertyIdAndData ) {

    TAGInput *current = getCurrentTAGDataBlockTAGInput(session, page);
	size_t len = strlen(pPropertyIdAndData);
    if (current->htmlProperties) {
        char *tmpcat = mkStrcat(session->heapHandle, __FILE__, __LINE__, current->htmlProperties, " ", pPropertyIdAndData, NULL);
        mkFree(session->heapHandle, current->htmlProperties);
        current->htmlProperties = tmpcat;
    } else {
        current->htmlProperties = mkMalloc(session->heapHandle, len + 1, __FILE__, __LINE__);
        strcpy_s(current->htmlProperties, len+1, pPropertyIdAndData);
    }
}



