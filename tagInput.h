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

 /** \file   tagInput.h
    \brief  Does all Session management related jobs.

*/

#pragma once




void                                              destroyTAGInput(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *pTagInput);
TAGInput *  createTAGInput(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *parentTAGDataBlock, bool isAddedToDatablockTAGInputLinkedList);
void                               printDataBlockDataItemInstance(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *inputInstance);
char*                      printDataBlockDataItemInstanceTypeText(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *inputInstance);
char*                  printDataBlockDataItemInstanceTypeTextArea(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *inputInstance);
char*                    printDataBlockDataItemInstanceTypeSelect(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *inputInstance);
char*                  printDataBlockDataItemInstanceTypeCheckBox(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *inputInstance, char *str);
char*               printDataBlockDataItemInstanceTypeRadioButton(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *inputInstance, char *str);
char*                     printDataBlockDataItemInstanceTypeImage(SodiumSession *currentMKSession, TAGInput *inputInstance);
void                       setCurrentTAGInputPropertyDatalistName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void                               setCurrentTAGInputPropertyName(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *input,const char *pPropValue);
void                     setCurrentTAGInputPropertyMasterItemName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void                       setCurrentTAGInputPropertySequenceName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void                 setCurrentTAGInputPropertySequenceSchemaName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
TAGInput                          *getCurrentTAGDataBlockTAGInput(SodiumSession *currentMKSession, HTSQLPage *page);
/** \brief Sets the nameWithBlockName property of TAGInput. This function should be called whenever the "name" property of the data block changed.
 *
 * \param currentSodiumSession MKSession*
 * \param input TAGInput*
 * \return void
 *
 */
void                                  setTAGDatablockFullItemName(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *input) ;

/** \brief HTML "input" and "select" tags are tracked by TAGInput types. For "select" element, call this method passing "select" value as second argument.
 *
 * \param currentSodiumSession MKSession*
 * \param pPropValue const char*
 * \return void
 *
 */
void                        setCurrentTAGInputPropertyType(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *current, const char *pPropValue);
void                                    setCurrentTAGInputPropertyInsertAllowed(SodiumSession *currentMKSession, HTSQLPage *page, char *pInsertAllowed);
void                                    setCurrentTAGInputPropertyUpdateAllowed(SodiumSession *currentMKSession, HTSQLPage *page, char *pUpdateAllowed);
/** \brief Set the initial state of the current TAGInput. That is, if pPropValue is not null, "value" property will be set to that data and "valueState" will be INPUT_TAG_VALUE_CLEAR.
 *
 * \param currentSodiumSession MKSession*
 * \param pPropValue const char*
 * \return void
 *
 * This function is especially designed to be called by HTML parser.
 * If HT/SQL programmer set <input value="XXXx"..></input> property in the *.frmx file, this function will implement it.
 * "valueState" proeprty of the TAGInput will be set to INPUT_TAG_VALUE_CLEAR.
 */
void                                setCurrentTAGInputPropertyValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void          setCurrentTAGInputPropertyDataType(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *current, const char *pPropValue);
void           setCurrentTAGInputPropertyMaxLength(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *input, const char *pPropValue);
void       setCurrentTAGInputPropertyColumnName(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *tagInput, const char *pPropValue);
void                           setCurrentTAGInputPropertyFormatMask(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void                         setCurrentTAGInputPropertyDefaultValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void                 setCurrentTAGInputPropertyAppendHTMLProperty(SodiumSession *currentMKSession, HTSQLPage *page, char *pPropertyIdAndData);
void                                 setCurrentTAGInputCheckedValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void                               setCurrentTAGInputUncheckedValue(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void                       setCurrentTAGInputPropertyLookupItemName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);
void		          setCurrentTAGInputPropertyLookupItemBlockName(SodiumSession *currentMKSession, HTSQLPage *page, const char *pPropValue);

/** \brief Search for a specific tag input in a data block. Returns null if data block or item is not exists.
 *
 * \param currentSodiumSession MKSession*
 * \param dataBlockName const char*
 * \param pPropName const char*
 * \return TAGInput*
 *
 */
TAGInput*              findTAGInputByName(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName);



/** \brief HTML specification says that Radio items must have same name in order to group them together.
 *         However, this is not acceptable for C object instance. Object instances must have unique name
 *         Just for radio buttons, unique name will be generated by concatenation of name + checkedValue.
 *
 * \param currentSodiumSession MKSession*
 * \param input TAGInput*
 * \return void
 *
 */
void setCurrentTAGInputPropertyNameWithNameAndValuePair(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *input);
void                             setTAGInputPropertySrc(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, const char *pPropValue);

void
SetTAGInputPropertyImageFileName(
    SodiumSession* session,
    HTSQLPage* page,
    TAGDataBlock* tagDataBlock,
    const char* pPropName,
    const char* pPropValue
);

void                   setTAGInputPropertyImageFileSize(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, int pPropValue);
void                              tagInputValidityCheck(SodiumSession *currentMKSession, HTSQLPage *page, TAGInput *object);

void setTAGInputPropertyVisibility(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, bool propertyValue);
void setTAGInputPropertyEnabled(SodiumSession *currentMKSession, HTSQLPage *page, TAGDataBlock *tagDataBlock, const char *pPropName, bool propertyValue);


