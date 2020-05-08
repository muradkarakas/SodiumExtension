#include "pch.h"

#include "mkConstants.h"


const char *mkSessionCookieName = "mkSession";

const char *stdHeadTAGContent =     "\n"									
									"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"

                                    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\n"
									"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"

                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "js/jquery-3.1.1.min.js\"></script>\n"
                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "pnotify/dist/iife/PNotify.js\"></script>\n"
                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "pnotify/dist/iife/PNotifyConfirm.js\"></script>\n"
                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "js/next-level-init.js?_=1\"></script>\n"
                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "js/next-level-response.js?_=1\"></script>\n"
                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "js/next-level-request.js\"></script>\n"
                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "js/next-level-selection.js\"></script>\n"
									"<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "js/next-level-jstree.js\"></script>\n"

                                    "<script type=\"text/javascript\" src=\"" RESOURCE_FOLDER "jsTree/jstree.min.js\"></script>\n"
                                    "<link rel=\"stylesheet\" type=\"text/css\" href=\"" RESOURCE_FOLDER "pnotify/dist/PNotifyBrightTheme.css\"/>\n"
                                    "<link rel=\"stylesheet\" type=\"text/css\" href=\"" RESOURCE_FOLDER "jsTree/themes/default/style.min.css\"/>\n"
                                    "<link id=\"bootstrap-css\" rel=\"stylesheet\" href=\"" RESOURCE_FOLDER "css/bootstrap.min.css\" crossorigin=\"anonymous\" />\n"
                                    
	"\n";

const char *treeOuterDivOpen    = "<div class=\"treeOuterSpan\">";
const char *treeOuterDivClose   = "</div>";

/** Page */
const char *trgName_PageAccess  = "page.access";
const char *trgName_PageLoad    = "page.load";

/** Datablock */
const char *trgName_DatablockRowSelected = ".row_selected";
const char *trgName_DatablockPostQuery = ".post_query";

const char *trgName_DatablockPostInsert = ".post_insert";
const char *trgName_DatablockPostUpdate = ".post_update";
const char *trgName_DatablockPostDelete = ".post_delete";

const char *trgName_DatablockPreInsert = ".pre_insert";
const char *trgName_DatablockPreUpdate = ".pre_update";
const char *trgName_DatablockPreDelete = ".pre_delete";

const char *trgName_DatablockAccess = ".access";

const char *trgName_TreeNodeExpanded = "tree_node_expanded";
const char *trgName_TreeNodeSelected = "tree_node_selected";


const char *classButton = "";
const char *classSelect = "";
const char *classImage = "";
const char *classText = "";
const char *classCheckBox = "";
const char *classTextArea = "";
char *classDatablock = "";

const wchar_t *cLogFileName = L"sodium.log";


