/**
 * @name Javaughn Burke
 * StudentID#: 1025920
 * Course: CIS 2750
 * @file ExtraHeaders.h
 * @date January 2020
 * @brief File containing the function definitions of my created functions
 */

 #ifndef EXTRA_HEADERS_H
 #define EXTRA_HEADERS_H

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"

xmlNode* findRootChild(char* name, xmlNode* rootNode);
char* checkFieldLength(int maxValue, char* string);
void findCircles(xmlNode* rootNode, List* circleList);
void findRectangles(xmlNode* rootNode, List* rectangleList);
void findPaths(xmlNode* rootNode, List* pathList);
void findGroups(xmlNode* rootNode, List* groupList);
Rectangle* createRectangle(xmlNode* rectangleNode);
Circle* createCircle(xmlNode* circleNode);
Path* createPath(xmlNode* pathNode);
Group* createGroup(xmlNode* groupNode);
Rectangle* initializeRectangle();
Circle* initializeCircle();
Path* initializePath();
xmlAttr* findAttribute(xmlAttr* node, char* attribute);
char* getAttributeContent(xmlAttr* node);
Attribute* initializeAttribute();
Group* initializeGroup();
void deleteGroup(void* data);
char* groupToString( void* data);
int compareGroups(const void *first, const void *second);
SVGimage* initializeSVGImage();
Attribute* createAttribute(xmlAttr* attribute);
void addRectToList(List* groupList, List* rectList);
void addCircleToList(List* groupList, List* circleList);
void addPathToList(List* groupList, List* pathList);
void printRectList(List* rectangleList);
void printCircleList(List* circleList);
void addGroupToList(List* groupList, List* gList);
void printGroupList(List* groupList);
void freeListOnly(List* list);

/************************************* A2 Functions ****************************/
int addAttrToTree(xmlNode* node, List* attributes);
int addGroupToTree(xmlNode* node, Group* group);
xmlDoc* SVGimageToXml(SVGimage* image);
void addOtherAttr(List* attributeList, Attribute* attribute);
float attrStringToFloat(char* string);
char* emptyJSON();
char* emptyJSONForList();
void print_element_names(xmlNode* a_node);


/************************************* A3 Functions ****************************/
char* JsSVG(char* fileName);
bool vaildateJavascript(char* fileName);
char* createSVG(char* fileName);
char* showOtherAttributes(char* fileName, char* shape, int index);
void editAttribute(char* fileName, char* shape, int index, char* name, char* value);
bool editTitle(char* fileName, char* string, char* value);
int upload(char* fileName, char* newFile);
bool scale(char* fileName, char* shape, int scale);
bool addShape(char* fileName, char* shape, char* jsonString);
bool create(char* fileName, char* jsonString);

 #endif