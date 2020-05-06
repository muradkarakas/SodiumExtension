#pragma once

#include <Windows.h>
#include "tagDataBlockDataStructures.h"

typedef enum INPUT_TAG_VALUE_STATUS {
	INPUT_TAG_VALUE_NEW,
	INPUT_TAG_VALUE_CLEAR,
	INPUT_TAG_VALUE_DIRTY
} INPUT_TAG_VALUE_STATE;

typedef enum INPUT_TAG_DATA_TYPES {
	INPUT_TAG_DATA_TYPE_CHAR,
	INPUT_TAG_DATA_TYPE_NUMBER,
	INPUT_TAG_DATA_TYPE_DATE,
	INPUT_TAG_DATA_TYPE_UNKNOWN
} INPUT_TAG_DATA_TYPE;


/** \brief  HTML "input" and "select" tags are tracked by TAGInput data structure.
*          For "select" element, call this method passing "select" value as second argument.
*/
typedef enum INPUT_TAG_TYPES {
	INPUT_TAG_TYPE_TEXT,
	INPUT_TAG_TYPE_PASSWORD,
	INPUT_TAG_TYPE_CHECKBOX,
	INPUT_TAG_TYPE_RADIO,
	INPUT_TAG_TYPE_SUBMIT,
	INPUT_TAG_TYPE_RESET,
	INPUT_TAG_TYPE_FILE,
	INPUT_TAG_TYPE_HIDDEN,
	INPUT_TAG_TYPE_IMAGE,
	INPUT_TAG_TYPE_BUTTON,
	INPUT_TAG_TYPE_SELECT,
	INPUT_TAG_TYPE_TEXTAREA,
	INPUT_TAG_TYPE_UNKNOWN
} INPUT_TAG_TYPE;


/** \brief Memory structure for <input></input> tag and its properties.
*/
typedef struct structInput {
	/** These variables will be set by setter of "name" function. Do not set it manually !! */
	char               *nameWithBlockName;

	/** This variables are for type of image */
	wchar_t                *imageFileName;
	int                     imageFileSize;
	void                             *src; /** May contain url or image binary content encoded as Base64 */

										   /** DB column name */
	char                      *columnName;
	char                  *masterItemName;
	char              *sequenceSchemaName;
	char                    *sequenceName;
	char                      *formatMask;
	char                    *defaultValue;
	char                    *dataListName;
	char             *lookupItemBlockName;
	char                  *lookupItemName;

	/** Html "value" property value */
	char                    *name;
	char                    *value; /** For images, value property holds the size of image */
	char					*maxLength;
	char                    insertAllowed;
	char                    updateAllowed;
	char                    *htmlProperties;
	INPUT_TAG_TYPE          inputType;
	INPUT_TAG_DATA_TYPE     dataType;

	BOOL                    enabled;
	BOOL                    visibility;

	/** For radio and/or check boxes */
	char                    *unCheckedValue;
	char                    *checkedValue;
	char                    *orjinalName;   /** It is used only for radio items. This variable hold the html radio item's orjinal name defined in FRMX file.
											*  "name" property holds different string then the real name property in order to differentiate c object instance name.
											For more information, please see the description of the "setCurrentTAGInputPropertyNameWithNameAndValuePair" function.
											*/

											/** INTERNAL VARIABLES */

											/** This variable is defined to track the value of db cell/tag input value.
											\n
											If the row/cell is in initial state value will be INPUT_TAG_VALUE_NEW. That is, its value will be NULL. This happens if value of the current TAGInput has not been read yet.
											\n
											If the current TAGInput value has been read from the DB, the value will be set to the "name" property and valueState will be INPUT_TAG_VALUE_CLEAR.
											\n
											If the current TAGInput value has been modified, but not updated in db, valueState will be INPUT_TAG_VALUE_DIRTY.
											"Modified" means going from INPUT_TAG_VALUE_CLEAR to INPUT_TAG_VALUE_DIRTY.
											\n
											Have a look at description of setTAGInputPropertyValue() and modifyTAGInputValue().
											*/
	INPUT_TAG_VALUE_STATE   valueState;

	struct structDataBlock *parentTAGDataBlock;
	struct structInput  *next;
} TAGInput;


