/**
 * @name Javaughn Burke
 * StudentID#: 1025920
 * Course: CIS 2750
 * @file SVGParcer.c
 * @date January 2020
 * @brief File containing the function C code of an SVG Parcer
 */
#define _DEFAULT_SOURCE
#include "SVGParser.h"
#include "ExtraHeaders.h"
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#define free(variable) if(variable != NULL) free(variable);


SVGimage* createSVGimage(char* filename) {
    SVGimage* returnSVG = NULL;
    xmlDoc* doc = NULL;
    xmlNode* temp = NULL;
    char* tempString = NULL;
    xmlNs* nameSpace = NULL;
    xmlNode* rootNode = NULL;
    xmlAttr* attribute = NULL;
    Attribute* at = NULL;
    xmlAttr* currentAttr = NULL;

    if(filename == NULL) return NULL;
    if(access(filename, F_OK) != -1) {
        if(access(filename, R_OK) == -1) {
            return NULL;
        }
    }else {
        return NULL;
    }

    LIBXML_TEST_VERSION

    doc = xmlReadFile(filename, NULL, 0);
    if(doc == NULL) return NULL;

    rootNode = xmlDocGetRootElement(doc);
    if(rootNode ==  NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser(); 
        return NULL;
    }
    nameSpace = rootNode->ns;
    if(nameSpace == NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        return NULL;
    }
    if(nameSpace->href == NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        return NULL;
    }
    if(strcmp((char*) nameSpace->href, "") == 0) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        return NULL;
    }
    returnSVG = initializeSVGImage();
    tempString = checkFieldLength(256, (char*) nameSpace->href);
    if(tempString == NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        deleteSVGimage(returnSVG);
        returnSVG = NULL;
        return NULL;
    } else{
        strcpy(returnSVG->namespace, tempString);
        free(tempString);
        tempString = NULL;
    }
    
   temp = findRootChild("title", rootNode->children);
   
    if(temp == NULL) {
        strcpy(returnSVG->title, "");
    }
    else{
        tempString = checkFieldLength(256, (char*) temp->children->content);
        if(tempString == NULL) {
            xmlFreeDoc(doc);
            doc = NULL;
            xmlCleanupParser();
            deleteSVGimage(returnSVG);
            returnSVG = NULL;
            return NULL;
        } else{
            strcpy(returnSVG->title, tempString);
            free(tempString);
            tempString = NULL;
        }
    }
    
    temp = findRootChild("desc", rootNode->children);
    if(temp == NULL) {
        strcpy(returnSVG->description, "");
    } else{
        tempString = checkFieldLength(256, (char*) temp->children->content);
        if(tempString == NULL) {
            xmlFreeDoc(doc);
            doc = NULL;
            xmlCleanupParser();
            deleteSVGimage(returnSVG);
            returnSVG = NULL;
            return NULL;
        } else{
             for(int a = 0; a < strlen(tempString); a++) {
                if(tempString[a] == '\"') {
                    tempString[a] = '\'';
                }
            }
            strcpy(returnSVG->description, tempString);
            free(tempString);
            tempString = NULL;
        }
    }
    if(rootNode->children != NULL) {
    findRectangles(rootNode->children, returnSVG->rectangles);
    findCircles(rootNode->children, returnSVG->circles);
    findPaths(rootNode->children, returnSVG->paths);
    findGroups(rootNode->children, returnSVG->groups);
    }

    attribute = rootNode->properties;
    if(attribute != NULL) {
        for(currentAttr = attribute; currentAttr != NULL; currentAttr = currentAttr->next) {
            if(strcmp((char*) currentAttr->name, "") != 0 && strcmp((char*) currentAttr->name, "xmlns") != 0) {
                at = createAttribute(currentAttr);
                if(at != NULL) insertBack(returnSVG->otherAttributes, at);
            }
        }
    }

    xmlFreeDoc(doc);
    doc = NULL;
    xmlCleanupParser(); 
    return returnSVG;
}


char* SVGimageToString(SVGimage* img) {
    char* returnString = "";
    SVGimage* temp = (SVGimage*) img;
    char* tempString = NULL;
    
    if(temp == NULL) return NULL; 

    returnString = (char*) calloc(100, sizeof(char));
    strcat(returnString, "namespace: ");
    
    if(temp->namespace != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(temp->namespace) + 20);
        strcat(returnString, temp->namespace);
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 8);
    strcat(returnString, "title: ");
    if(temp->title != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(temp->title) + 20);
        strcat(returnString, temp->title);
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 15);
    strcat(returnString, "description: ");
    if(temp->description != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(temp->description) + 20);
        strcat(returnString, temp->description);
    }
    strcat(returnString, "\n");
    if(temp->rectangles->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 15);
        strcat(returnString, "rectangles: \n");
        if(temp->rectangles) tempString = toString(temp->rectangles);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 20);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(temp->circles->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 12);
        strcat(returnString, "circles: \n");
        if(temp->circles) tempString = toString(temp->circles);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 20);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(temp->paths->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 50);
        strcat(returnString, "paths: \n");
        if(temp->paths != NULL) tempString = toString(temp->paths);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 20);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(temp->groups->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 10);
        strcat(returnString, "groups: \n");
        if(temp->groups != NULL) tempString = toString(temp->groups);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 20);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(temp->otherAttributes->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 20);
        strcat(returnString, "otherAttributes: \n");
        if(temp->otherAttributes != NULL) tempString = toString(temp->otherAttributes);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 20);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL; 
        }
    }   

    return returnString;
}



void deleteSVGimage(SVGimage* img) {
    
    if(img == NULL) return;

    strcpy(img->namespace, "");
    strcpy(img->title, "");
    strcpy(img->description, "");
    freeList(img->rectangles);
    img->rectangles = NULL;
    freeList(img->circles);
    img->circles = NULL;
    freeList(img->paths);
    img->paths = NULL;
    freeList(img->groups);
    img->groups = NULL;
    freeList(img->otherAttributes);
    img->otherAttributes = NULL;
    free(img);
    img = NULL;

return;
}


void deleteAttribute(void* data) {
    Attribute* attribute = (Attribute*) data;

    if(attribute ==  NULL) return;

    free(attribute->name);
    attribute->name = NULL;
    free(attribute->value);
    attribute->value = NULL;
    free(attribute);
    attribute = NULL;

    return;
}


char* attributeToString(void* data) {
    char* returnString = NULL;
    Attribute* attr = (Attribute*) data;

    if(attr == NULL) return NULL;

    returnString = (char*) calloc(100, sizeof(char));
    strcat(returnString, "name: ");
    
    if(attr->name != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(attr->name) + 20);
        strcat(returnString, attr->name);
    }
    strcat(returnString, "\n");
    strcat(returnString, "value: ");
    if(attr->value != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(attr->value) + 20);
        strcat(returnString, attr->value);
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 140);
    strcat(returnString, "====================================================================\n");
    strcat(returnString, "====================================================================\n");

    return returnString;
}


int compareAttributes(const void *first, const void *second) {
    int returnValue = -1;
    Attribute* a1 = (Attribute*) first;
    Attribute* a2 = (Attribute*) second;
    char* string1 = NULL;
    char* string2 = NULL;

    if(a1 == NULL || a2 == NULL) return returnValue;

    string1 = attributeToString(a1);
    string2 = attributeToString(a2);

    if((string1 != NULL) && (string2 != NULL)) {
        if(strcmp(string1, string2) != 0) {
            free(string1);
            string1 = NULL;
            free(string2);
            string2 = NULL;
            return returnValue;
        }
    }
    
    returnValue = 0;
    free(string1);
    string1 = NULL;
    free(string2);
    string2 = NULL;
    return returnValue;
}





void deleteGroup(void* data) {
    Group* group = (Group*) data;

    if(group == NULL) return;

    if(group->rectangles) {
        freeList(group->rectangles);
        group->rectangles = NULL;
    }
    if(group->circles) {
        freeList(group->circles);
        group->circles = NULL;
    }
    if(group->paths) {
        freeList(group->paths);
        group->paths = NULL;
    }
    if(group->groups) {
        freeList(group->groups);
        group->groups = NULL;
    }
    if(group->otherAttributes) {
        freeList(group->otherAttributes);
        group->otherAttributes = NULL;
    }
    free(group);
    group = NULL;

    return;
 }


char* groupToString( void* data) {
    char* returnString = NULL;
    Group* group = (Group*) data;
    char* tempString = NULL;

    if(group == NULL) return NULL;
    
    returnString = (char*) calloc(100, sizeof(char));
    if(group->rectangles->head != NULL) {
        strcat(returnString, "rectangles: \n");
        if(group->rectangles) tempString = toString(group->rectangles);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 2);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(group->circles->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 12);
        strcat(returnString, "circles: \n");
        if(group->circles) tempString = toString(group->circles);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 2);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(group->paths->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 10);
        strcat(returnString, "paths: \n");
        if(group->paths) tempString = toString(group->paths);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 2);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(group->groups->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 10);
        strcat(returnString, "groups: \n");
        if(group->groups != NULL) tempString = toString(group->groups);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 2);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL;
        }
        strcat(returnString, "\n");
    }
    if(group->otherAttributes->head != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + 20);
        strcat(returnString, "otherAttributes: \n");
        if(group->otherAttributes != NULL) tempString = toString(group->otherAttributes);
        if(tempString != NULL) {
            returnString = (char*) realloc(returnString, strlen(returnString) + strlen(tempString) + 2);
            strcat(returnString, tempString);
            free(tempString);
            tempString = NULL; 
        }
    }

    return returnString;
}


int compareGroups(const void *first, const void *second) {
    int returnValue = -1;
    Group* g1 = (Group*) first;
    Group* g2 = (Group*) second;
    char* string1 = NULL;
    char* string2 = NULL;

    if(g1 == NULL || g2 == NULL) return returnValue;

    string1 = groupToString(g1);
    string2 = groupToString(g2);

    if(string1 != NULL && string2 != NULL) {
        if(strcmp(string1, string2) != 0) {
            free(string1);
            string1 = NULL;
            free(string2);
            string2 = NULL;
            return returnValue;
        }
    }else if(string1 == NULL) {
        free(string2);
        string2 = NULL;
        return returnValue;
    }else if(string2 == NULL) {
        free(string1);
        string1 = NULL;
        return returnValue;
    }

    returnValue = 0;
    free(string1);
    string1 = NULL;
    free(string2);
    string2 = NULL;

    return returnValue;
}




void deleteRectangle(void* data) {
    Rectangle* rectangle = (Rectangle*) data;

    if(rectangle == NULL) return;
    if(rectangle->otherAttributes) {
        freeList(rectangle->otherAttributes);
        rectangle->otherAttributes = NULL;
    }
    free(rectangle);
    rectangle = NULL;
    
    return;
}


char* rectangleToString(void* data) {
    char* returnString = NULL;
    Rectangle* rect = (Rectangle*) data;
    char* temp = NULL;
    char buff[20];

    if(rect == NULL) return NULL;

    returnString = (char*) calloc(100, sizeof(char));
    strcat(returnString, "x: ");
    gcvt(rect->x, 6, buff);
    strcat(returnString, buff);
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 12);
    strcat(returnString, "y: ");
    gcvt(rect->y, 6, buff);
    strcat(returnString, buff);
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 15);
    strcat(returnString, "width: ");
    gcvt(rect->width, 6, buff);
    strcat(returnString, buff);
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 23);
    strcat(returnString, "height: ");
    gcvt(rect->height, 6, buff);
    strcat(returnString, buff);
    strcat(returnString, "\n");
    strcat(returnString, "units: ");
    if(rect->units != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(rect->units) + 2);
        strcat(returnString, rect->units);
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 20);
    if(rect->otherAttributes->head == NULL) {
        strcat(returnString, "\n");
        returnString = (char*) realloc(returnString, strlen(returnString) + 140);
        strcat(returnString, "====================================================================\n");
        strcat(returnString, "====================================================================\n");
        return returnString;
    }
    strcat(returnString, "otherAttributes: \n");
    if(rect->otherAttributes != NULL) temp = toString(rect->otherAttributes);
    if(temp != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(temp) + 20);
        strcat(returnString, temp);
        free(temp);
        temp = NULL; 
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 140);
    strcat(returnString, "====================================================================\n");
    strcat(returnString, "====================================================================\n");

    return returnString;
}


int compareRectangles(const void *first, const void *second) {
    int returnValue = -1;
    Rectangle* r1 = (Rectangle*) first;
    Rectangle* r2 = (Rectangle*) second;
    char* string1 = NULL;
    char* string2 = NULL;

    if(r1 == NULL || r2 == NULL) return returnValue;

    string1 = rectangleToString(r1);
    string2 = rectangleToString(r2);
    
    if(string1 != NULL && string2 != NULL) {
        if(strcmp(string1, string2) != 0) {
            free(string1);
            string1 = NULL;
            free(string2);
            string2 = NULL;
            return returnValue;
        }
    }else if(string1 == NULL){
        free(string2);
        string2 = NULL;
        return returnValue;
    }else if(string2 == NULL) {
        free(string1);
        string1 = NULL;
        return returnValue;
    }

    returnValue = 0;
    free(string1);
    string1 = NULL;
    free(string2);
    string2 = NULL;

    return returnValue;
}




void deleteCircle(void* data) {
    Circle* circle = (Circle*) data;

    if(circle == NULL) return;
    
    if(circle->otherAttributes) {
        freeList(circle->otherAttributes);
        circle->otherAttributes = NULL;
    }
    free(circle);
    circle = NULL;
    
    return;
}


char* circleToString(void* data) {
    char* returnString = NULL;
    Circle* circle = (Circle*) data;
    char* temp = NULL;
    char buff[20];
    
    if(circle == NULL) return NULL;

    returnString = (char*) calloc(100, sizeof(char));
    strcat(returnString, "cx: ");
    gcvt(circle->cx, 6, buff);
    strcat(returnString, buff);
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 12);
    strcat(returnString, "cy: ");
    gcvt(circle->cy, 6, buff);
    strcat(returnString, buff);
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 22);
    strcat(returnString, "r: ");
    gcvt(circle->r, 6, buff);
    strcat(returnString, buff);
    strcat(returnString, "\n");
    strcat(returnString, "units: ");
    if(circle->units != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(circle->units) + 20);
        strcat(returnString, circle->units);
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 20);
    if(circle->otherAttributes->head == NULL) {
        strcat(returnString, "\n");
        returnString = (char*) realloc(returnString, strlen(returnString) + 140);
        strcat(returnString, "====================================================================\n");
        strcat(returnString, "====================================================================\n");
        return returnString;
    }
    strcat(returnString, "otherAttributes: \n");
    if(circle->otherAttributes != NULL) temp = toString(circle->otherAttributes);
    if(temp != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(temp) + 20);
        strcat(returnString, temp);
        free(temp);
        temp = NULL; 
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 140);
    strcat(returnString, "====================================================================\n");
    strcat(returnString, "====================================================================\n");

    return returnString;
}


int compareCircles(const void *first, const void *second) {
    int returnValue = -1;
    Circle* c1 = (Circle*) first;
    Circle* c2 =  (Circle*) second;
    char* string1 = NULL;
    char* string2 = NULL;

    if(c1 == NULL || c2 == NULL) return returnValue;

    string1 = circleToString(c1);
    string2 = circleToString(c2);

    if(string1 != NULL && string2 != NULL){
        if(strcmp(string1, string2) != 0) {
            free(string1);
            string1 = NULL;
            free(string2);
            string2 = NULL;
            return returnValue;
        }
    } else if(string1 == NULL) {
        free(string2);
        string2 = NULL;
        return returnValue;
    }else if(string2 == NULL) {
        free(string1);
        string1 = NULL;
        return returnValue;
    }

    returnValue = 0;
    free(string1);
    string1 = NULL;
    free(string2);
    string2 = NULL;
    return returnValue;
}




void deletePath(void* data) {
    if(data ==  NULL) return;
    
    Path* path = (Path*) data;
    if(path->data != NULL) {
        free(path->data);
        path->data = NULL;
    }
    
    if(path->otherAttributes != NULL) {
        freeList(path->otherAttributes);
        path->otherAttributes = NULL;
    }
    
    free(path);
    path = NULL;
}


char* pathToString(void* data) {
    char* returnString = NULL;
    Path* path = (Path*) data;
    char* temp = NULL;

    if(path == NULL) return NULL;

    returnString = (char*) calloc(100, sizeof(char));
    strcat(returnString, "data: ");
    if(path->data != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(path->data) + 20);
        strcat(returnString, path->data);
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 20);
    if(path->otherAttributes->head == NULL) {
        strcat(returnString, "\n");
        returnString = (char*) realloc(returnString, strlen(returnString) + 140);
        strcat(returnString, "====================================================================\n");
        strcat(returnString, "====================================================================\n");
        return returnString;
    }
    strcat(returnString, "otherAttributes: \n");
    if(path->otherAttributes != NULL) temp = toString(path->otherAttributes);
    if(temp != NULL) {
        returnString = (char*) realloc(returnString, strlen(returnString) + strlen(temp) + 20);
        strcat(returnString, temp);
        free(temp);
        temp = NULL; 
    }
    strcat(returnString, "\n");
    returnString = (char*) realloc(returnString, strlen(returnString) + 140);
    strcat(returnString, "====================================================================\n");
    strcat(returnString, "====================================================================\n");

    return returnString; 
}


int comparePaths(const void *first, const void *second) {
    int returnValue = -1;
    Path* p1 = (Path*) first;
    Path* p2 = (Path*) second;
    char* string1 = NULL;
    char* string2 = NULL;

    if(p1 == NULL || p2 == NULL) return returnValue;

    string1 = pathToString(p1);
    string2 = pathToString(p2);

    if((string1 != NULL) && (string2 != NULL)) {
        if(strcmp(string1, string2) != 0) {
            free(string1);
            string1 = NULL;
            free(string2);
            string2 = NULL;
            return returnValue;
        }
    }else if(string1 == NULL) {
        free(string2);
        string2 = NULL;
        return returnValue;
    }else if(string2 == NULL) {
        free(string1);
        string1 = NULL;
        return returnValue;
    }

    returnValue = 0;
    free(string1);
    string1 = NULL;
    free(string2);
    string2 = NULL;

    return returnValue;
}




List* getRects(SVGimage* img) {
    List* rectList = NULL;
    ListIterator* tempIterator = NULL;
    SVGimage* tempImage = img;
    Rectangle* tempRectangle = NULL;
    ListIterator temp;

    rectList = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    if(tempImage == NULL) return rectList;

    if(tempImage->rectangles != NULL) {
        if(tempImage->rectangles->head != NULL) {
            temp = createIterator(tempImage->rectangles);
            tempIterator = &temp;
            tempRectangle = (Rectangle*) nextElement(tempIterator);
            for(;tempRectangle != NULL; tempRectangle = (Rectangle*) nextElement(tempIterator)) {
                insertBack(rectList, tempRectangle);
            }
        }
    }
    if(tempImage->groups != NULL) {
        if(tempImage->groups->head) {
            addRectToList(tempImage->groups, rectList);
        }
    }

    return rectList;
}

void printRectList(List* rectangleList) {
    char* string = NULL;
    ListIterator temp;
    ListIterator* rectIterator = NULL;
    Rectangle* tempRect;

    if(rectangleList == NULL) return;
    if(rectangleList->head == NULL) return;

    temp = createIterator(rectangleList);
    rectIterator = &temp;
    tempRect = (Rectangle*) nextElement(rectIterator);
    for(;tempRect != NULL; tempRect = (Rectangle*) nextElement(rectIterator)) {
        string = rectangleToString(tempRect);
        printf("%s\n", string);
    }
}


List* getCircles(SVGimage* img) {
    List* circleList = NULL;
    ListIterator* tempIterator = NULL;
    SVGimage* tempImage = img;
    Circle* tempCircle = NULL;
    ListIterator temp;

    circleList = initializeList(circleToString, deleteCircle, compareCircles);
    if(tempImage == NULL) return circleList;

    if(tempImage->circles != NULL) {
        if(tempImage->circles->head != NULL) {
            temp = createIterator(tempImage->circles);
            tempIterator = &temp;
            tempCircle = (Circle*) nextElement(tempIterator);
            for(;tempCircle != NULL; tempCircle = (Circle*) nextElement(tempIterator)) {
                insertBack(circleList, tempCircle);
            }
        }
    }
    if(tempImage->groups != NULL) {
        if(tempImage->groups->head != NULL) {
            addCircleToList(tempImage->groups, circleList);
        }
    }

    return circleList;
}


void printCircleList(List* circleList) {
    char* string = NULL;
    ListIterator temp;
    ListIterator* circleIterator = NULL;
    Circle* tempCircle;

    if(circleList == NULL) return;
    if(circleList->head == NULL) return;

    temp = createIterator(circleList);
    circleIterator = &temp;
    tempCircle = (Circle*) nextElement(circleIterator);
    for(;tempCircle != NULL; tempCircle = (Circle*) nextElement(circleIterator)) {
        string = circleToString(tempCircle);
        printf("%s\n", string);
    }
}

void printGroupList(List* groupList) {
    char* string = NULL;
    ListIterator temp;
    ListIterator* groupIterator = NULL;
    Group* tempGroup;

    if(groupList == NULL) return;
    if(groupList->head == NULL) return;

    temp = createIterator(groupList);
    groupIterator = &temp;
    tempGroup = (Group*) nextElement(groupIterator);
    for(; tempGroup != NULL; tempGroup = (Group*) nextElement(groupIterator)) {
        string = groupToString(tempGroup);
        printf("%s\n", string);
    }
}

List* getGroups(SVGimage* img) {
    List* groupList = NULL;
    SVGimage* tempImage = img;

    groupList = initializeList(groupToString, deleteGroup, compareGroups);
     if(tempImage == NULL) return groupList;

    if(tempImage->groups != NULL) {
        if(tempImage->groups->head != NULL) {
            addGroupToList(tempImage->groups, groupList);
        } 
    }

    return groupList;
}


List* getPaths(SVGimage* img) {
    List* pathList = NULL;
    ListIterator* tempIterator = NULL;
    SVGimage* tempImage = img;
    Path* tempPath = NULL;
    ListIterator temp;

    pathList = initializeList(pathToString, deletePath, comparePaths);
     if(tempImage == NULL) return pathList;

    if(tempImage->paths != NULL) {
        if(tempImage->paths->head != NULL) {
            temp = createIterator(tempImage->paths);
            tempIterator = &temp;
            tempPath = (Path*) nextElement(tempIterator);
            for(;tempPath != NULL; tempPath = (Path*) nextElement(tempIterator)) {
                insertBack(pathList, tempPath);
            }
        }
    }

    if(tempImage->groups != NULL) {
        if(tempImage->groups->head != NULL) {
            addPathToList(tempImage->groups, pathList);
        }
    }

    return pathList;
}

void addRectToList(List* groupList, List* rectList) {
    ListIterator* groupIterator = NULL;
    ListIterator temp;
    ListIterator* rectIterator = NULL;
    Rectangle* tempRectangle = NULL;
    Group* tempGroup = NULL;

    if(groupList == NULL || rectList == NULL) return;
    if(groupList->head == NULL) return;

    temp = createIterator(groupList);
    groupIterator = &temp;
    tempGroup = nextElement(groupIterator);

    for(;tempGroup != NULL; tempGroup = nextElement(groupIterator)) {
        ListIterator t;
        if(tempGroup->rectangles != NULL) {
            if(tempGroup->rectangles->head != NULL) {
                t = createIterator(tempGroup->rectangles);
                rectIterator = &t;
                tempRectangle = nextElement(rectIterator);
                for(; tempRectangle !=  NULL; tempRectangle = nextElement(rectIterator)) {
                    insertBack(rectList, tempRectangle);
                }
            }
        }

        if(tempGroup->groups != NULL) {
            if(tempGroup->groups->head != NULL) {
                addRectToList(tempGroup->groups, rectList);
            }
        }
    }
}

void addCircleToList(List* groupList, List* circleList) {
    ListIterator* groupIterator = NULL;
    ListIterator* circleIterator = NULL;
    ListIterator temp;
    Circle* tempCircle = NULL;
    Group* tempGroup = NULL;
    if(groupList == NULL || circleList == NULL) return;
    if(groupList->head == NULL) return;

    temp = createIterator(groupList);
    groupIterator = &temp;
    tempGroup = nextElement(groupIterator);

    for(;tempGroup != NULL; tempGroup = nextElement(groupIterator)) {
        ListIterator t;
        if(tempGroup->circles != NULL) {
            if(tempGroup->circles->head != NULL) {
                t = createIterator(tempGroup->circles);
                circleIterator = &t;
                tempCircle = nextElement(circleIterator);
                for(; tempCircle !=  NULL; tempCircle = nextElement(circleIterator)) {
                    insertBack(circleList, tempCircle);
                }
            }
        }

        if(tempGroup->groups != NULL) {
            if(tempGroup->groups->head != NULL) {
                addCircleToList(tempGroup->groups, circleList);
            }
        }
    }
}


void addPathToList(List* groupList, List* pathList) {
    ListIterator* groupIterator = NULL;
    ListIterator* pathIterator = NULL;
    ListIterator temp;
    Path* tempPath = NULL;
    Group* tempGroup = NULL;
    if(groupList == NULL || pathList == NULL) return;
    if(groupList->head == NULL) return;

    temp = createIterator(groupList);
    groupIterator = &temp;
    tempGroup = nextElement(groupIterator);

    for(;tempGroup != NULL; tempGroup = nextElement(groupIterator)) {
        ListIterator t;
        if(tempGroup->paths != NULL) {
            if(tempGroup->paths->head != NULL) {
                t = createIterator(tempGroup->paths);
                pathIterator = &t;
                tempPath = nextElement(pathIterator);
                for(; tempPath !=  NULL; tempPath = nextElement(pathIterator)) {
                    insertBack(pathList, tempPath);
                }
            }
        }

        if(tempGroup->groups != NULL) {
            if(tempGroup->groups->head != NULL) {
                addPathToList(tempGroup->groups, pathList);
            }
        }
    }
}

void addGroupToList(List* groupList, List* gList) {
    ListIterator* groupIterator = NULL;
    ListIterator iterator;
    Group* tempGroup = NULL;


    if(groupList == NULL || gList == NULL) return;
    if(groupList->head == NULL) return;

    iterator = createIterator(groupList);
    groupIterator = &iterator;
    tempGroup = (Group*) nextElement(groupIterator);

    for(; tempGroup != NULL; tempGroup = (Group*) nextElement(groupIterator)) {
        if(tempGroup->groups != NULL) {
            if(tempGroup->groups->head != NULL) {
                addGroupToList(tempGroup->groups, gList);
            }
        }
        insertBack(gList, tempGroup);
    }
}



int numRectsWithArea(SVGimage* img, float area) {
    int returnValue = 0;
    List* rectangleList = NULL;
    ListIterator rectIterator;
    ListIterator* list = NULL;
    Rectangle* tempRectangle = NULL;
    int resultArea = 0;
    
    if(img == NULL) return 0;
    if(area < 0) return 0;

    rectangleList = getRects(img);
    rectIterator = createIterator(rectangleList);
    list = &rectIterator;
    tempRectangle = (Rectangle*) nextElement(list);

    for(;tempRectangle != NULL; tempRectangle = (Rectangle*) nextElement(list)) {
        resultArea = (int) ceil(tempRectangle->width * tempRectangle->height);
        if(resultArea == (int) ceil(area)) {
            returnValue++;
        }
    }

    if(rectangleList != NULL) {
        freeListOnly(rectangleList);
        rectangleList = NULL;
    }

    return returnValue;
}


int numCirclesWithArea(SVGimage* img, float area) {
    int returnValue = 0;
    List* circleList = NULL;
    ListIterator circleIterator;
    ListIterator* list = NULL;
    Circle* tempCircle = NULL;
    int resultArea = 0;
    
    if(img == NULL) return 0;
    if(area < 0) return 0;

    circleList = getCircles(img);
    circleIterator = createIterator(circleList);
    list = &circleIterator;
    tempCircle = (Circle*) nextElement(list);

    for(;tempCircle != NULL; tempCircle = (Circle*) nextElement(list)) {
        resultArea = (int) ceil(3.141592653589793238 * (tempCircle->r * tempCircle->r));
        if(resultArea == (int)ceil(area)) {
            returnValue++;
        }
    }

    if(circleList != NULL) {
        freeListOnly(circleList);
        circleList = NULL;
    }
    return returnValue;
}


int numPathsWithdata(SVGimage* img, char* data) {
    int returnValue = 0;
    List* pathList = NULL;
    ListIterator pathIterator;
    ListIterator* list = NULL;
    Path* tempPath = NULL;

    if(img == NULL || data == NULL) return 0;
    if(strcmp(data, "") == 0) return 0;

    pathList = getPaths(img);
    pathIterator = createIterator(pathList);
    list = &pathIterator;
    tempPath = (Path*) nextElement(list);

    for(;tempPath != NULL; tempPath = (Path*) nextElement(list)) {
        if(strcmp(tempPath->data, data) == 0) {
            returnValue++;
        }
    }

    if(pathList != NULL) {
        freeListOnly(pathList);
        pathList = NULL;
    }

    return returnValue;
}


int numGroupsWithLen(SVGimage* img, int len) {
    int returnValue = 0;
    int result = 0;
    List* groupList = NULL; 
    ListIterator groupIterator;
    ListIterator* list;
    Group* tempGroup = NULL;

    if(img == NULL || len < 0) return 0;

    groupList = getGroups(img);
    groupIterator = createIterator(groupList);
    list = &groupIterator;
    tempGroup = (Group*) nextElement(list);

    for(; tempGroup != NULL; tempGroup = (Group*) nextElement(list)) {
        if(tempGroup->rectangles != NULL) {
            result += tempGroup->rectangles->length;
        }
        if(tempGroup->circles != NULL) {
            result += tempGroup->circles->length;
        }
        if(tempGroup->paths != NULL) {
            result += tempGroup->paths->length;
        }
        if(tempGroup->groups != NULL) {
            result += tempGroup->groups->length;
        }

        if(result == len){
            returnValue++;
        }
        result = 0;
    }

    if(groupList != NULL) {
        freeListOnly(groupList);
        groupList = NULL;
    }

    return returnValue;
}




int numAttr(SVGimage* img) {
    int returnValue = 0;
    List* rectangles = NULL;
    List* circles = NULL;
    List* paths = NULL;
    List* groups = NULL;
    Rectangle* tempRectangles = NULL;
    Circle* tempCircle = NULL;
    Path* tempPath = NULL;
    Group* tempGroup = NULL;
    ListIterator rectangleIterator;
    ListIterator* rIterator = NULL;
    ListIterator circleIterator;
    ListIterator* cIterator = NULL;
    ListIterator pathIterator;
    ListIterator* pIterator = NULL;
    ListIterator groupIterator;
    ListIterator* grIterator = NULL;
    List* otherAttributes = NULL;
    Attribute* tempAttribute = NULL;
    ListIterator attrIterator;
    ListIterator* aIterator = NULL;


    if(img == NULL) return 0;

    rectangles = getRects(img);
    if(rectangles != NULL) {
        if(rectangles->head != NULL) {
            rectangleIterator = createIterator(rectangles);
            rIterator = &rectangleIterator;
            tempRectangles = (Rectangle*) nextElement(rIterator);
            for(; tempRectangles != NULL; tempRectangles = (Rectangle*) nextElement(rIterator)) {
                if(tempRectangles->otherAttributes != NULL) {
                    returnValue += tempRectangles->otherAttributes->length;
                }
            }
        }
    }

    circles = getCircles(img);
    if(circles != NULL) {
        if(circles->head != NULL) {
            circleIterator = createIterator(circles);
            cIterator = &circleIterator;
            tempCircle = (Circle*) nextElement(cIterator);
            for(; tempCircle != NULL; tempCircle = (Circle*) nextElement(cIterator)) {
                if(tempCircle->otherAttributes != NULL) {
                    returnValue += tempCircle->otherAttributes->length;
                }
            }
        }
    }

    paths = getPaths(img);
    if(paths !=  NULL) {
        if(paths->head != NULL) {
            pathIterator = createIterator(paths);
            pIterator = &pathIterator;
            tempPath = (Path*) nextElement(pIterator);
            for(; tempPath != NULL; tempPath = (Path*) nextElement(pIterator)) {
                if(tempPath->otherAttributes != NULL) {
                    returnValue += tempPath->otherAttributes->length;
                }
            }
        }
    }

    groups = getGroups(img);
    if(groups != NULL) {
        if(groups->head != NULL) {
            groupIterator = createIterator(groups);
            grIterator = &groupIterator;
            tempGroup = (Group*) nextElement(grIterator);
            for(; tempGroup != NULL; tempGroup = (Group*) nextElement(grIterator)) {
                if(tempGroup->otherAttributes != NULL) {
                    returnValue += tempGroup->otherAttributes->length;
                }
            }
        }
    }

    if(img->otherAttributes != NULL) {
        otherAttributes = img->otherAttributes;
        attrIterator = createIterator(otherAttributes);
        aIterator = &attrIterator;
        tempAttribute = (Attribute*) nextElement(aIterator);
        for(; tempAttribute != NULL; tempAttribute = (Attribute*) nextElement(aIterator)) {
            returnValue++;
        }
    }

    if(rectangles != NULL) {
        freeListOnly(rectangles);
        rectangles = NULL;
    }
    if(circles != NULL) {
        freeListOnly(circles);
        circles = NULL;
    }
    if(paths != NULL) {
        freeListOnly(paths);
        paths = NULL;
    }
    if(groups != NULL) {
        freeListOnly(groups);
        groups = NULL;
    }
    
    return returnValue;
}


char* checkFieldLength(int maxValue, char* string) {
    char* returnString = NULL;

    if(string == NULL) {
        return NULL;
    }

    returnString = (char*) calloc(maxValue, sizeof(char) + 2);
    if(strlen(string) > maxValue) {
        strncpy(returnString, string, maxValue);
    } else {
        strcpy(returnString, string);
    }
    return returnString;
}



xmlNode* findRootChild(char* name, xmlNode* rootNode) {
    xmlNode*  current =  NULL;
    bool nameFound = false;
    
    if(name == NULL || rootNode == NULL) {
        return NULL;
    } else if(strcmp(name, "") == 0) return NULL;

    for(current = rootNode; current != NULL; current = current->next) {
        if(current->name != NULL) {
            if(strcmp((char*) current->name, name) == 0) {
                nameFound = true;
                break;
            }
        }
    }

    if(nameFound != true) {
        return NULL;
    }
    return current;
}

void findCircles(xmlNode* rootNode, List* circleList) {
    xmlNode* current = NULL;
    Circle* tempCircle = NULL;

    if(rootNode == NULL || circleList == NULL) return;

    for(current = rootNode; current != NULL; current = current->next) {
        if(strcmp((char*) current->name, "circle") == 0) {
            tempCircle = createCircle(current);
            if(tempCircle != NULL) insertBack(circleList, tempCircle);
        }
    }

    return;
}

void findRectangles(xmlNode* rootNode, List* rectangleList) {
    xmlNode* current = NULL;
    Rectangle* tempRectangle = NULL;

    if(rootNode == NULL || rectangleList == NULL) return;
    
    for(current = rootNode; current != NULL; current = current->next) {
        if(strcmp((char*) current->name, "rect") == 0) {
            tempRectangle = createRectangle(current);
            if(tempRectangle != NULL) insertBack(rectangleList, tempRectangle);
        }
    }    
    return;
}

void findPaths(xmlNode* rootNode, List* pathList) {
    xmlNode* current = NULL;
    Path* tempPath = NULL;
    
    if(rootNode == NULL || pathList == NULL) return;
    
    for(current = rootNode; current != NULL; current = current->next) {
        if(strcmp((char*) current->name, "path") == 0) {
            tempPath = createPath(current);
            if(tempPath != NULL) insertBack(pathList, tempPath);
        }
    }
    
    return;
}

void findGroups(xmlNode* rootNode, List* groupList) {
    xmlNode* current = NULL;
    Group* tempGroup = NULL;

    if(rootNode == NULL || groupList == NULL) return;

    for(current = rootNode; current != NULL; current = current->next) {
        if(strcmp((char*) current->name, "g") == 0) {
            tempGroup = createGroup(current);
            if(tempGroup != NULL) insertBack(groupList, tempGroup);
        }
    }
    
    return;
}

Group* createGroup(xmlNode* groupNode) {
    Group* returnGroup = NULL;
    xmlAttr* attribute = NULL;
    Attribute* at = NULL;
    xmlAttr* currentAttr = NULL;

    if(groupNode == NULL) return NULL;
    if(groupNode->name == NULL) return NULL;
    if(strcmp((char*) groupNode->name, "g") != 0) return NULL;

    returnGroup = initializeGroup();
    if(groupNode->children != NULL) {
        findRectangles(groupNode->children, returnGroup->rectangles);
        findCircles(groupNode->children, returnGroup->circles);
        findPaths(groupNode->children, returnGroup->paths);
        findGroups(groupNode->children, returnGroup->groups);
    }
    attribute = groupNode->properties;
    if(attribute != NULL) {
        for(currentAttr = attribute; currentAttr != NULL; currentAttr = currentAttr->next) {
            if(strcmp((char*) currentAttr->name, "") != 0) {
                at = createAttribute(currentAttr);
                if(at != NULL) insertBack(returnGroup->otherAttributes, at);
            }
        }
    }
    return returnGroup;
}

Rectangle* createRectangle(xmlNode* rectangleNode) {
    Rectangle* returnRectangle = NULL;
    xmlAttr* attribute = NULL;
    xmlAttr* currentAttr = NULL;
    char tempString[20];
    int i = 0;
    int j = 0;
    Attribute* at = NULL;
    char* attrContent = NULL;

    if(rectangleNode == NULL) {
        return NULL;
    } else if(rectangleNode->name == NULL) {
        return NULL;
    } else if(strcmp((char*) rectangleNode->name, "rect") != 0) {
        return NULL;
    }
    if(rectangleNode->properties == NULL) return NULL;

    returnRectangle = initializeRectangle();
    attribute = findAttribute(rectangleNode->properties, "x");
    attrContent = getAttributeContent(attribute);

    if(attrContent == NULL) {
        returnRectangle->x = 0.0;
    }else {
        strcpy(tempString, "");
        i = 0;
        while(attrContent[i] != '\0') {
            if(isdigit(attrContent[i]) == 0 && attrContent[i] != '.') {
                break;
            }
            tempString[i] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1]) == 0) {
                tempString[i + 1] = '\0';
            }
            i++;
        }
        returnRectangle->x = strtof(tempString, NULL);
        strcpy(tempString, "");
        j = 0;
        while(attrContent[i] != '\0' && isdigit(attrContent[i]) == 0) {
            tempString[j] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1] != 0)) {
                tempString[j + 1] = '\0';
            }
            i++;
            j++;
        }
        strcpy(returnRectangle->units, tempString);
    }
    free(attrContent);
    attrContent = NULL;

    attribute = findAttribute(rectangleNode->properties, "y");
    attrContent = getAttributeContent(attribute);
    if(attrContent == NULL) {
        returnRectangle->y = 0.0;
    } else {
        strcpy(tempString, "");
        i = 0;
        while(attrContent[i] != '\0') {
            if(isdigit(attrContent[i]) == 0 && attrContent[i] != '.') {
                break;
            }
            tempString[i] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1]) == 0) {
                tempString[i + 1] = '\0';
            }
            i++;
        }
        returnRectangle->y = strtof(tempString, NULL);
        strcpy(tempString, "");
        j = 0;
        while(attrContent[i] != '\0' && isdigit(attrContent[i]) == 0) {
            tempString[j] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1] != 0)) {
                tempString[j + 1] = '\0';
            }
            i++;
            j++;
        }
        strcpy(returnRectangle->units, tempString);
    }
    free(attrContent);
    attrContent = NULL;

    attribute = findAttribute(rectangleNode->properties, "width");
    attrContent = getAttributeContent(attribute);
    if(attrContent == NULL) {
        deleteRectangle(returnRectangle);
        returnRectangle = NULL;
        return NULL;
    } else {
        strcpy(tempString, "");
        i = 0;
        while(attrContent[i] != '\0') {
            if(isdigit(attrContent[i]) == 0 && attrContent[i] != '.') {
                break;
            }
            tempString[i] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1]) == 0) {
                tempString[i + 1] = '\0';
            }
            i++;
        }
        returnRectangle->width = strtof(tempString, NULL);
        strcpy(tempString, "");
        j = 0;
        while(attrContent[i] != '\0' && isdigit(attrContent[i]) == 0) {
            tempString[j] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1] != 0)) {
                tempString[j + 1] = '\0';
            }
            i++;
            j++;
        }
        strcpy(returnRectangle->units, tempString);
        if(returnRectangle->width < 0.0){
        deleteRectangle(returnRectangle);
        returnRectangle = NULL;
        free(attrContent);
        attrContent = NULL;
        return NULL;
        }
    }
    free(attrContent);
    attrContent = NULL;

    attribute = findAttribute(rectangleNode->properties, "height");
    attrContent = getAttributeContent(attribute);
    if(attrContent == NULL) {
        deleteRectangle(returnRectangle);
        returnRectangle = NULL;
        return NULL;
    } else {
        strcpy(tempString, "");
        i = 0;
        while(attrContent[i] != '\0') {
            if(isdigit(attrContent[i]) == 0 && attrContent[i] != '.') {
                break;
            }
            tempString[i] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1]) == 0) {
                tempString[i + 1] = '\0';
            }
            i++;
        }
        returnRectangle->height = strtof(tempString, NULL);
        strcpy(tempString, "");
        j = 0;
        while(attrContent[i] != '\0' && isdigit(attrContent[i]) == 0) {
            tempString[j] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1] != 0)) {
                tempString[j + 1] = '\0';
            }
            i++;
            j++;
        }
        strcpy(returnRectangle->units, tempString);
        if(returnRectangle->height < 0.0) {
        deleteRectangle(returnRectangle);
        returnRectangle = NULL;
        free(attrContent);
        attrContent = NULL;
        return NULL;
        }
    }
    free(attrContent);
    attrContent = NULL;

    attribute = rectangleNode->properties;
    for(currentAttr = attribute; currentAttr != NULL; currentAttr = currentAttr->next) {
        if(strcmp((char*) currentAttr->name, "x") != 0 && strcmp((char*) currentAttr->name, "y") != 0 && strcmp((char*) currentAttr->name, "width") != 0 && strcmp((char*) currentAttr->name, "height") != 0) {
            at = createAttribute(currentAttr);
            if(at != NULL) insertBack(returnRectangle->otherAttributes, at);   
        }
    }
    return returnRectangle;
}

Circle* createCircle(xmlNode* circleNode) {
    Circle* returnCircle = NULL;
    xmlAttr* attribute = NULL;
    xmlAttr* currentAttr = NULL;
    char tempString[20];
    int i = 0;
    int j = 0;
    Attribute* at = NULL;
    char* attrContent = NULL;

    if(circleNode == NULL) return NULL;
    if(circleNode->name == NULL) return NULL;
    if(strcmp((char*) circleNode->name, "circle") != 0) return NULL;
    if(circleNode->properties == NULL) return NULL;

    returnCircle = initializeCircle();
    attribute = findAttribute(circleNode->properties, "cx");
    attrContent = getAttributeContent(attribute);
    if(attrContent == NULL) {
        returnCircle->cx = 0.0;
    }else {
        strcpy(tempString, "");
        i = 0;
        while(attrContent[i] != '\0') {
            if(isdigit(attrContent[i]) == 0 && attrContent[i] != '.') {
                break;
            }
            tempString[i] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1]) == 0) {
                tempString[i + 1] = '\0';
            }
            i++;
        }
        returnCircle->cx = strtof(tempString, NULL);
        strcpy(tempString, "");
        j = 0;
        while(attrContent[i] != '\0' && isdigit(attrContent[i] == 0)) {
            tempString[j] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1] != 0)) {
                tempString[j + 1] = '\0';
            }
            i++;
            j++;
        }
        strcpy(returnCircle->units, tempString);
    }
    free(attrContent);
    attrContent = NULL;

    attribute = findAttribute(circleNode->properties, "cy");
    attrContent = getAttributeContent(attribute);
    if(attrContent == NULL) {
        returnCircle->cy = 0.0;
    }else {
        strcpy(tempString, "");
        i = 0;
        while(attrContent[i] != '\0') {
            if(isdigit(attrContent[i]) == 0 && attrContent[i] != '.') {
                break;
            }
            tempString[i] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1]) == 0) {
                tempString[i + 1] = '\0';
            }
            i++;
        }
        returnCircle->cy = strtof(tempString, NULL);
        strcpy(tempString, "");
        j = 0;
        while(attrContent[i] != '\0' && isdigit(attrContent[i]) == 0) {
            tempString[j] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1] != 0)) {
                tempString[j + 1] = '\0';
            }
            i++;
            j++;
        }
        strcpy(returnCircle->units, tempString);
    }
    free(attrContent);
    attrContent = NULL;

    attribute = findAttribute(circleNode->properties, "r");
    attrContent = getAttributeContent(attribute);
    if(attrContent == NULL) {
        deleteCircle(returnCircle);
        returnCircle = NULL;
        return NULL;
    } else {
        strcpy(tempString, "");
        i = 0;
        while(attrContent[i] != '\0') {
            if(isdigit(attrContent[i]) == 0 && attrContent[i] != '.') {
                break;
            }
            tempString[i] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1]) == 0) {
                tempString[i + 1] = '\0';
            }
            i++;
        }
        returnCircle->r = strtof(tempString, NULL);
        strcpy(tempString, "");
        j = 0;
        while(attrContent[i] != '\0' && isdigit(attrContent[i]) == 0) {
            tempString[j] = attrContent[i];
            if(attrContent[i + 1] == '\0' || isdigit(attrContent[i + 1] != 0)) {
                tempString[j + 1] = '\0';
            }
            i++;
            j++;
        }
        strcpy(returnCircle->units, tempString);
        if(returnCircle->r < 0.0) {
            deleteCircle(returnCircle);
            returnCircle = NULL;
            free(attrContent);
            attrContent = NULL;
            return NULL;
        }
    }
    free(attrContent);
    attrContent = NULL;

    attribute = circleNode->properties;
    for(currentAttr = attribute; currentAttr != NULL; currentAttr = currentAttr->next) {
        if(strcmp((char*) currentAttr->name, "cx") != 0 && strcmp((char*) currentAttr->name, "cy") != 0 && strcmp((char*) currentAttr->name, "r") != 0) {
            at = createAttribute(currentAttr);
            if(at != NULL) insertBack(returnCircle->otherAttributes, at);   
        }
    }
    return returnCircle;
}

Path* createPath(xmlNode* pathNode) {
    Path* returnPath = NULL;
    xmlAttr* attribute = NULL;
    xmlAttr* currentAttr = NULL;
    Attribute* at = NULL;
    char* attrContent = NULL;

    if(pathNode == NULL) return NULL;
    if(pathNode->name == NULL) return NULL;
    if(strcmp((char*) pathNode->name, "path") != 0) return NULL;
    if(pathNode->properties == NULL) return NULL;
    
    returnPath = initializePath();
    attribute = findAttribute(pathNode->properties, "d"); 
    attrContent = getAttributeContent(attribute);
    
    if(attrContent == NULL) {
        deletePath(returnPath);
        returnPath = NULL;
    }else {
        if(strlen(attrContent) > 300) {
            returnPath->data = (char*) realloc(returnPath->data, strlen(attrContent) + 10);
            strcpy(returnPath->data, "");
        }
        strcpy(returnPath->data, attrContent);
    }
    free(attrContent);
    attrContent = NULL;
    
    attribute = pathNode->properties;
    for(currentAttr = attribute; currentAttr != NULL;) {
        if(strcmp((char*) currentAttr->name, "d") != 0) {
            at = createAttribute(currentAttr);
            if(at != NULL) insertBack(returnPath->otherAttributes, at);   
        }
        currentAttr = currentAttr->next;
    }
    
    return returnPath;
}

Attribute* createAttribute(xmlAttr* attribute) {
    Attribute* returnAttr= NULL;

    if(attribute == NULL) return NULL;

    returnAttr = initializeAttribute();
    if(attribute->name != NULL) {
        if(strlen((char*) attribute->name) > 300) {
            returnAttr->name = (char*) realloc(returnAttr->name, strlen((char*) attribute->name) + 10);
            strcpy(returnAttr->name, "");
        }
        strcpy(returnAttr->name, (char*) attribute->name);
    }
    
    if(attribute->children->content != NULL) {
        if(strlen((char*) attribute->children->content) > 300) {
            returnAttr->value = (char*) realloc(returnAttr->value, strlen((char*) attribute->children->content) + 10);
            strcpy(returnAttr->value, "");
        }
        strcpy(returnAttr->value, (char*) attribute->children->content);
    }

    return returnAttr;
}

Rectangle* initializeRectangle() {
    Rectangle* returnRectangle = NULL;

    returnRectangle = (Rectangle*) calloc(1, sizeof(Rectangle));
    returnRectangle->x = 0.0;
    returnRectangle->y = 0.0;
    returnRectangle->width = 0.0;
    returnRectangle->height = 0.0;
    strcpy(returnRectangle->units, "");
//  ADD List Inittialization HERE!!!!!!
    returnRectangle->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    
    return returnRectangle;
}

Circle* initializeCircle() {
    Circle* returnCircle = NULL;

    returnCircle = (Circle*) calloc(1, sizeof(Circle));
    returnCircle->cx = 0.0;
    returnCircle->cy = 0.0;
    returnCircle->r = 0.0;
    strcpy(returnCircle->units, "");
//  ADD List Inittialization HERE!!!!!!
    returnCircle->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    
    return returnCircle; 
}

Path* initializePath() {
    Path* returnPath = NULL;

    returnPath = (Path*) calloc(1, sizeof(Path));
    returnPath->data = (char*) calloc(1000, sizeof(char));
//  ADD List Inittialization HERE!!!!!!
    returnPath->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    return returnPath;
}

Attribute* initializeAttribute() {
    Attribute* returnAttribute = NULL;

    returnAttribute = (Attribute*) calloc(1, sizeof(Attribute));
    returnAttribute->name = (char*) calloc(700, sizeof(char));
    strcpy(returnAttribute->name, "");
    returnAttribute->value = (char*) calloc(700, sizeof(char));
    strcpy(returnAttribute->value, "");

    return returnAttribute;
}

Group* initializeGroup() {
    Group* returnGroup = NULL;

    returnGroup = (Group*) calloc(1, sizeof(Group));
    returnGroup->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    returnGroup->circles = initializeList(circleToString, deleteCircle, compareCircles);
    returnGroup->paths = initializeList(pathToString, deletePath, comparePaths);
    returnGroup->groups = initializeList(groupToString, deleteGroup, compareGroups);
    returnGroup->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    return returnGroup;
}

SVGimage* initializeSVGImage() {
    SVGimage* returnSVG = NULL;

    returnSVG = (SVGimage*) calloc(1, sizeof(SVGimage));
    returnSVG->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    returnSVG->circles = initializeList(circleToString, deleteCircle, compareCircles);
    returnSVG->paths = initializeList(pathToString, deletePath, comparePaths);
    returnSVG->groups = initializeList(groupToString, deleteGroup, compareGroups);
    returnSVG->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    return returnSVG;
}

xmlAttr* findAttribute(xmlAttr* node, char* attribute) {
    xmlAttr* returnAttribute = NULL;
    xmlAttr* temp = node;

    if(temp == NULL) {
        return NULL;
    }else if(attribute == NULL) {
        return NULL;
    }else if(strcmp(attribute, "") == 0) {
        return NULL;
    }
    
    for(temp = node; temp != NULL; temp = temp->next) {
        if(strcmp((char*) temp->name, attribute) == 0) {
            returnAttribute = temp;
            break;
        }
    }
    
    return returnAttribute;
}


char* getAttributeContent(xmlAttr* node) {
    char* returnString = NULL;
    xmlNode* tempNode = NULL;
    xmlAttr* temp = node;
    int i = 0;

   
    if(temp == NULL) return NULL;
    if(temp->children == NULL) return NULL;
    
    tempNode = temp->children;
    if(tempNode->content == NULL) return NULL;
    i = strlen((char*) tempNode->content);
    returnString = (char*) calloc(i + 2, sizeof(char));
    strcpy(returnString, (char*) tempNode->content);
    return returnString;
}


void freeListOnly(List* list) {
    
    if(list == NULL) return;
	
	if(list->head == NULL && list->tail == NULL) {
        free(list);
        return;
    }
	
	Node* tmp;
	
	while (list->head != NULL){
		tmp = list->head;
		list->head = list->head->next;
		free(tmp);
        tmp = NULL;

	}
	
	list->head = NULL;
	list->tail = NULL;
	list->length = 0;

    free(list);
    list = NULL;
}


/************************************* A2 STUFF ************************************/
/************************************* A2 STUFF ************************************/



SVGimage* createValidSVGimage(char* fileName, char* schemaFile) {
    SVGimage* returnSVG = NULL;
    xmlDoc* doc = NULL;
    xmlSchemaParserCtxtPtr ctext = NULL;
    xmlSchemaPtr schemaPtr = NULL;
    xmlNode* temp = NULL;
    char* tempString = NULL;
    xmlNs* nameSpace = NULL;
    xmlNode* rootNode = NULL;
    xmlAttr* attribute = NULL;
    Attribute* at = NULL;
    xmlAttr* currentAttr = NULL;

    if(fileName == NULL || schemaFile == NULL) return NULL;
    if(strcmp(fileName, "") == 0 || strcmp(schemaFile, "") == 0) return NULL;

    if(access(fileName, F_OK) != -1) {
        if(access(fileName, R_OK) == -1) {
            return NULL;
        }
    }else {
        return NULL;
    }

    if(access(schemaFile, F_OK) != -1) {
        if(access(schemaFile, R_OK) == -1) {
            return NULL;
        }
    }else {
        return NULL;
    }

    ctext = xmlSchemaNewParserCtxt(schemaFile);
    xmlSchemaSetParserErrors(ctext, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schemaPtr = xmlSchemaParse(ctext);
    xmlSchemaFreeParserCtxt(ctext);
    ctext = NULL;

    doc = xmlReadFile(fileName, NULL, 0);

    if(doc == NULL) {
        if(schemaPtr != NULL) xmlSchemaFree(schemaPtr);
        schemaPtr = NULL;
        return NULL;
    } else {
        xmlSchemaValidCtxt* validCtext;
        int returnValue = 0;

        validCtext = xmlSchemaNewValidCtxt(schemaPtr);
        xmlSchemaSetValidErrors(validCtext, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        returnValue = xmlSchemaValidateDoc(validCtext, doc);
        xmlSchemaFreeValidCtxt(validCtext);
        validCtext = NULL;
        if(schemaPtr != NULL) xmlSchemaFree(schemaPtr);
        schemaPtr = NULL;
        xmlSchemaCleanupTypes();

        if(returnValue == 0) {

        } else if(returnValue > 0) {
            xmlFreeDoc(doc);
            doc = NULL;
            xmlCleanupParser();
            xmlMemoryDump();
            return NULL;
        } else {
            xmlFreeDoc(doc);
            doc = NULL;
            xmlCleanupParser();
            xmlMemoryDump();
            return NULL;
        }
    }


    rootNode = xmlDocGetRootElement(doc);
    if(rootNode ==  NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }
    nameSpace = rootNode->ns;
    if(nameSpace == NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }
    if(nameSpace->href == NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }
    if(strcmp((char*) nameSpace->href, "") == 0) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }
    returnSVG = initializeSVGImage();
    tempString = checkFieldLength(256, (char*) nameSpace->href);
    if(tempString == NULL) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
        deleteSVGimage(returnSVG);
        returnSVG = NULL;
        return NULL;
    } else{
        strcpy(returnSVG->namespace, tempString);
        free(tempString);
        tempString = NULL;
    }
    
   temp = findRootChild("title", rootNode->children);
   
    if(temp == NULL) {
        strcpy(returnSVG->title, "");
    }
    else{
        tempString = checkFieldLength(256, (char*) temp->children->content);
        if(tempString == NULL) {
            xmlFreeDoc(doc);
            doc = NULL;
            xmlCleanupParser();
            xmlMemoryDump();
            deleteSVGimage(returnSVG);
            returnSVG = NULL;
            return NULL;
        } else{
            strcpy(returnSVG->title, tempString);
            free(tempString);
            tempString = NULL;
        }
    }
    
    temp = findRootChild("desc", rootNode->children);
    if(temp == NULL) {
        strcpy(returnSVG->description, "");
    } else{
        tempString = checkFieldLength(256, (char*) temp->children->content);
        if(tempString == NULL) {
            xmlFreeDoc(doc);
            doc = NULL;
            xmlCleanupParser();
            xmlMemoryDump();
            deleteSVGimage(returnSVG);
            returnSVG = NULL;
            return NULL;
        } else{
            for(int a = 0; a < strlen(tempString); a++) {
                if(tempString[a] == '\"') {
                    tempString[a] = '\'';
                }
            }
            strcpy(returnSVG->description, tempString);
            free(tempString);
            tempString = NULL;
        }
    }
    if(rootNode->children != NULL) {
    findRectangles(rootNode->children, returnSVG->rectangles);
    findCircles(rootNode->children, returnSVG->circles);
    findPaths(rootNode->children, returnSVG->paths);
    findGroups(rootNode->children, returnSVG->groups);
    }

    attribute = rootNode->properties;
    if(attribute != NULL) {
        for(currentAttr = attribute; currentAttr != NULL; currentAttr = currentAttr->next) {
            if(strcmp((char*) currentAttr->name, "") != 0 && strcmp((char*) currentAttr->name, "xmlns") != 0) {
                at = createAttribute(currentAttr);
                if(at != NULL) insertBack(returnSVG->otherAttributes, at);
            }
        }
    }

    if(validateSVGimage(returnSVG, schemaFile) == false) {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
        deleteSVGimage(returnSVG);
        returnSVG = NULL;
        return NULL;
    }

    xmlFreeDoc(doc);
    doc = NULL;

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    return returnSVG;
}


bool validateSVGimage(SVGimage* image, char* schemaFile) {
    bool returnValue = false;
    xmlDoc* imageDoc = NULL;
    xmlSchemaParserCtxtPtr ctext = NULL;
    xmlSchemaPtr schemaPtr = NULL;

    if(image == NULL || schemaFile == NULL) return returnValue;
    if(strcmp(schemaFile, "") == 0) return returnValue;
    if(image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->otherAttributes == NULL) return returnValue;

    if(access(schemaFile, F_OK) != -1) {
        if(access(schemaFile, R_OK) == -1) {
            return returnValue;
        }
    }else {
        return returnValue;
    }

    imageDoc = SVGimageToXml(image);
    if(imageDoc == NULL) return returnValue;

    ctext = xmlSchemaNewParserCtxt(schemaFile);
    xmlSchemaSetParserErrors(ctext, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schemaPtr = xmlSchemaParse(ctext);
    xmlSchemaFreeParserCtxt(ctext);
    ctext = NULL;

    xmlSchemaValidCtxtPtr validCtext;
    int intValue = 0;

    validCtext = xmlSchemaNewValidCtxt(schemaPtr);
    xmlSchemaSetValidErrors(validCtext, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    intValue = xmlSchemaValidateDoc(validCtext, imageDoc);

    if(intValue == 0) {

    } else if(intValue > 0) {
        printf("fails to validate image\n");
        xmlSchemaFreeValidCtxt(validCtext);
        validCtext = NULL;
        if(schemaPtr != NULL) xmlSchemaFree(schemaPtr);
        schemaPtr = NULL;
        xmlFreeDoc(imageDoc);
        imageDoc = NULL;
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
        return returnValue;
    } else {
        printf("svalidation generated an internal error\n");
        xmlSchemaFreeValidCtxt(validCtext);
        validCtext = NULL;
        if(schemaPtr != NULL) xmlSchemaFree(schemaPtr);
        schemaPtr = NULL;
        xmlFreeDoc(imageDoc);
        imageDoc = NULL;
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
        return returnValue;
    }
    xmlSchemaFreeValidCtxt(validCtext);
    validCtext = NULL;
    

    if(schemaPtr != NULL) {
        xmlSchemaFree(schemaPtr);
        schemaPtr = NULL;
    }

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    returnValue = true;
    xmlFreeDoc(imageDoc);
    imageDoc = NULL;


    return returnValue;
}






bool writeSVGimage(SVGimage* image, char* fileName) {
    bool returnValue = false;
    xmlDoc* imageDoc = NULL;
    int fileWritten = 0;

    if(image == NULL || fileName == NULL) return returnValue;
    if(strcmp(fileName, "") == 0) return returnValue;
    imageDoc = SVGimageToXml(image);
    if(imageDoc == NULL) {
        return returnValue;
    }

    fileWritten = xmlSaveFormatFileEnc(fileName, imageDoc, NULL, 1);

    if(fileWritten < 0) {
        xmlFreeDoc(imageDoc);
        imageDoc = NULL;
        xmlCleanupParser();
        xmlMemoryDump();
        return returnValue;
    }

    xmlFreeDoc(imageDoc);
    imageDoc = NULL;
    xmlCleanupParser();
    xmlMemoryDump();

    returnValue = true;

    return returnValue;
}




void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute) {
    if(image == NULL || newAttribute == NULL) return;
    if(elemType != SVG_IMAGE && elemType != CIRC && elemType != RECT && elemType != PATH && elemType != GROUP) return;
    if(elemType == CIRC || elemType == RECT || elemType == PATH || elemType == GROUP) {
        if(elemIndex < 0) return;
    }
    if(newAttribute->name == NULL || newAttribute->value == NULL) return;
    if(strcmp(newAttribute->name, "") == 0) return;
    if(image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL) return;
    

    if(elemType == 0) {
       addOtherAttr(image->otherAttributes, newAttribute);
    }else if(elemType == 2) {
        ListIterator rect;
        ListIterator* rectList = NULL;
        Rectangle* tempRect = NULL;
        int count = 0;

        rect = createIterator(image->rectangles);
        rectList = &rect;
        tempRect = (Rectangle*) nextElement(rectList);

        for(; tempRect != NULL; tempRect = (Rectangle*) nextElement(rectList)) {
            if(count == elemIndex) {
                if(strcmp(newAttribute->name, "x") == 0) {
                    tempRect->x = attrStringToFloat(newAttribute->value);
                    deleteAttribute(newAttribute);
                    newAttribute = NULL;
                }else if(strcmp(newAttribute->name, "y") == 0) {
                    tempRect->y = attrStringToFloat(newAttribute->value);
                    deleteAttribute(newAttribute);
                    newAttribute = NULL;
                }else if(strcmp(newAttribute->name, "width") == 0) {
                    tempRect->width = attrStringToFloat(newAttribute->value);
                    deleteAttribute(newAttribute);
                    newAttribute = NULL;
                }else if(strcmp(newAttribute->name, "height")== 0) {
                    tempRect->height = attrStringToFloat(newAttribute->value);
                    deleteAttribute(newAttribute);
                    newAttribute = NULL;
                }else {
                    addOtherAttr(tempRect->otherAttributes, newAttribute);
                }
            }
            count++;
        }
    }else if(elemType == 1) {
        ListIterator circle;
        ListIterator* circleList = NULL;
        Circle* tempCircle = NULL;
        int count = 0;

        circle = createIterator(image->circles);
        circleList = &circle;
        tempCircle = (Circle*) nextElement(circleList);

        for(; tempCircle != NULL; tempCircle = (Circle*) nextElement(circleList)) {
            if(count == elemIndex) {
                if(strcmp(newAttribute->name, "cx") == 0) {
                    tempCircle->cx = attrStringToFloat(newAttribute->value);
                    deleteAttribute(newAttribute);
                    newAttribute = NULL;
                }else if(strcmp(newAttribute->name, "cy") == 0) {
                    tempCircle->cy = attrStringToFloat(newAttribute->value);
                    deleteAttribute(newAttribute);
                    newAttribute = NULL;
                }else if(strcmp(newAttribute->name, "r") == 0) {
                    tempCircle->r = attrStringToFloat(newAttribute->value);
                    deleteAttribute(newAttribute);
                    newAttribute = NULL;
                }else {
                    addOtherAttr(tempCircle->otherAttributes, newAttribute);
                }
            }
            count++;
        }
    } else if(elemType == 3) {
        ListIterator path;
        ListIterator* pathList = NULL;
        Path* tempPath = NULL;
        int count = 0;

        path = createIterator(image->paths);
        pathList = &path;
        tempPath = (Path*) nextElement(pathList);

        for(; tempPath != NULL; tempPath = (Path*) nextElement(pathList)) {
            if(count == elemIndex) {
                if(strcmp(newAttribute->name, "d") == 0) {
                    if(strlen(newAttribute->value) > strlen(tempPath->data)) {
                        tempPath->data = (char*) realloc(tempPath->data, strlen(newAttribute->value) + 10);
                        strcpy(tempPath->data, newAttribute->value);
                        deleteAttribute(newAttribute);
                        newAttribute = NULL;
                    } else {
                        strcpy(tempPath->data, newAttribute->value);
                        deleteAttribute(newAttribute);
                        newAttribute = NULL;
                    }
                } else {
                    addOtherAttr(tempPath->otherAttributes, newAttribute);
                }
            }
            count++;
        }
    }else if(elemType == 4) {
        ListIterator group;
        ListIterator* groupList = NULL;
        Group* tempGroup = NULL;
        int count = 0;

        group = createIterator(image->groups);
        groupList = &group;
        tempGroup = (Group*) nextElement(groupList);

        for(; tempGroup != NULL; tempGroup = (Group*) nextElement(groupList)) {
            if(count == elemIndex) {
                addOtherAttr(tempGroup->otherAttributes, newAttribute);
            }
            count++;
        }
    }

    return;
}




void addComponent(SVGimage* image, elementType type, void* newElement) {

    if(image == NULL || newElement == NULL) return;
    if(type != SVG_IMAGE && type != RECT && type != CIRC && type != PATH && type != GROUP) return;
    if(image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL) return;

    if(type == RECT) {
        Rectangle* tempRect = NULL;

        tempRect = (Rectangle*) newElement;
        if(tempRect == NULL) return;
        if(tempRect->otherAttributes == NULL) return;
        insertBack(image->rectangles, tempRect);
    }else if(type == CIRC) {
        Circle* tempCircle = NULL;

        tempCircle = (Circle*) newElement;
        if(tempCircle == NULL) return;
        if(tempCircle->otherAttributes == NULL) return
        insertBack(image->circles, tempCircle);
    }else if(type == PATH) {
        Path* tempPath = NULL;

        tempPath = (Path*) newElement;
        if(tempPath == NULL) return;
        if(tempPath->otherAttributes == NULL) return;
        insertBack(image->paths, tempPath);
    }

    return;
}

xmlDoc* SVGimageToXml(SVGimage* image) {
    xmlDoc* returnDoc = NULL;
    xmlNodePtr root = NULL;
    xmlNs* nameSpace = NULL;
    xmlNode* temp = NULL;
    ListIterator attrList;
    ListIterator* list = NULL;
    Attribute* attr = NULL;

    if(image == NULL) return NULL;
    if(image->otherAttributes == NULL) return NULL;
    if(strcmp(image->namespace, "") == 0) return NULL;
    if(image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL) return NULL;
    returnDoc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST "svg");
    nameSpace = xmlNewNs(root, BAD_CAST image->namespace, NULL);
    xmlSetNs(root, nameSpace);
    xmlDocSetRootElement(returnDoc, root);

    attrList = createIterator(image->otherAttributes);
    list = &attrList;
    attr = (Attribute*) nextElement(list);

    for(; attr != NULL; attr = (Attribute*) nextElement(list)) {
        if(attr->name == NULL || attr->value == NULL) {
            xmlFreeDoc(returnDoc);
            returnDoc = NULL;
            root = NULL;
            return NULL;
        }
        if(strcmp(attr->name, "") == 0) {
            xmlFreeDoc(returnDoc);
            returnDoc = NULL;
            root = NULL;
            return NULL;
        }
        xmlNewProp(root, BAD_CAST attr->name, BAD_CAST attr->value);
    }

    if(strcmp(image->title, "") != 0) {
        xmlNewChild(root, NULL, BAD_CAST "title", BAD_CAST image->title);
    }

    if(strcmp(image->description, "") != 0) {
        xmlNewChild(root, NULL, BAD_CAST "desc", BAD_CAST image->description);
    }

    if(image->rectangles != NULL) {
        ListIterator rectList;
        ListIterator* rectangle = NULL;
        Rectangle* tempRect = NULL;
        char units[50] = "";
        char buffer[60] = "";

        rectList = createIterator(image->rectangles);
        rectangle = &rectList;
        tempRect = (Rectangle*) nextElement(rectangle);

        for(; tempRect != NULL; tempRect = (Rectangle*) nextElement(rectangle)) {
            if(tempRect->otherAttributes == NULL) {
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
            }
           temp = xmlNewChild(root, NULL, BAD_CAST "rect", NULL);
           strcpy(units, tempRect->units);
           if(strcmp(units, "cm") != 0 && strcmp(units, "mm") != 0 && strcmp(units, "ex") != 0 && strcmp(units, "em") != 0 && strcmp(units, "px") != 0 && strcmp(units, "pt") != 0 && strcmp(units, "pc") != 0 && strcmp(units, "in") != 0 && strcmp(units, "") != 0) {
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
            }
           gcvt(tempRect->x, 8, buffer);
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "x", BAD_CAST buffer);
           strcpy(buffer, "");
           gcvt(tempRect->y, 8, buffer);
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "y", BAD_CAST buffer);
           strcpy(buffer, "");
           gcvt(tempRect->width, 8, buffer);
           if(tempRect->width < 0) {
               xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
           }
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "width", BAD_CAST buffer);
           strcpy(buffer, "");
           gcvt(tempRect->height, 8, buffer);
           if(tempRect->height < 0) {    
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
           }
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "height", BAD_CAST buffer);

           if(tempRect->otherAttributes->head != NULL) {
               int value = 0;
               value = addAttrToTree(temp, tempRect->otherAttributes);
               if(value == 1) {
                    xmlFreeDoc(returnDoc);
                    returnDoc = NULL;
                    root = NULL;
                    return NULL;
               }
           }
        }
    }else {
        xmlFreeDoc(returnDoc);
        returnDoc = NULL;
        root = NULL;
        return NULL;
    }


    if(image->circles !=  NULL) {
        ListIterator circleList;
        ListIterator* circle = NULL;
        Circle* tempCircle = NULL;
        char units[50] = "";
        char buffer[60] = "";

        circleList = createIterator(image->circles);
        circle = &circleList;
        tempCircle = (Circle*) nextElement(circle);

        for(; tempCircle != NULL; tempCircle = (Circle*) nextElement(circle)) {
            if(tempCircle->otherAttributes == NULL) {
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
            }
            temp = xmlNewChild(root, NULL, BAD_CAST "circle", NULL);
            strcpy(units, tempCircle->units);
            if(strcmp(units, "cm") != 0 && strcmp(units, "mm") != 0 && strcmp(units, "ex") != 0 && strcmp(units, "em") != 0 && strcmp(units, "px") != 0 && strcmp(units, "pt") != 0 && strcmp(units, "pc") != 0 && strcmp(units, "in") != 0 && strcmp(units, "") != 0) {
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
            }
            gcvt(tempCircle->cx, 8, buffer);
            xmlNewProp(temp, BAD_CAST "cx", BAD_CAST buffer);
            strcpy(buffer, "");
            gcvt(tempCircle->cy, 8, buffer);
            xmlNewProp(temp, BAD_CAST "cy", BAD_CAST buffer);
            strcpy(buffer, "");
            gcvt(tempCircle->r, 8, buffer);
            if(tempCircle->r < 0) {    
               xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
           }
            xmlNewProp(temp, BAD_CAST "r", BAD_CAST buffer);

            if(tempCircle->otherAttributes->head != NULL) {
                int value = 0;
                value = addAttrToTree(temp, tempCircle->otherAttributes);
                if(value == 1) {
                    xmlFreeDoc(returnDoc);
                    returnDoc = NULL;
                    root = NULL;
                    return NULL;
                }
            }
        }
    }else {
        xmlFreeDoc(returnDoc);
        returnDoc = NULL;
        root = NULL;
        return NULL;
    }

    if(image->paths !=  NULL) {
        ListIterator pathList;
        ListIterator* paths = NULL;
        Path* tempPath = NULL;

        pathList = createIterator(image->paths);
        paths = &pathList;
        tempPath = (Path*) nextElement(paths);

        for(; tempPath != NULL; tempPath = (Path*) nextElement(paths)) {
            if(tempPath->data == NULL || tempPath->otherAttributes == NULL) {
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
            }
            temp = xmlNewChild(root, NULL, BAD_CAST "path", NULL);
            if(tempPath->data != NULL) {
                xmlNewProp(temp, BAD_CAST "d", BAD_CAST tempPath->data);
            }

            if(tempPath->otherAttributes->head != NULL) {
                int value = 0;
                value = addAttrToTree(temp, tempPath->otherAttributes);
                if(value == 1) {
                    xmlFreeDoc(returnDoc);
                    returnDoc = NULL;
                    root = NULL;
                    return NULL;
                }
            }
        }
    }else {
        xmlFreeDoc(returnDoc);
        returnDoc = NULL;
        root = NULL;
        return NULL;
    }

    if(image->groups != NULL) {
        ListIterator groupList;
        ListIterator* groups = NULL;
        Group* tempGroup = NULL;

        groupList = createIterator(image->groups);
        groups = &groupList;
        tempGroup = (Group*) nextElement(groups);

        for(; tempGroup != NULL; tempGroup = (Group*) nextElement(groups)) {
            if(tempGroup->rectangles == NULL || tempGroup->circles == NULL || tempGroup->paths == NULL || tempGroup->groups == NULL || tempGroup->otherAttributes == NULL) {
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
            }
            temp = xmlNewChild(root, NULL, BAD_CAST "g", NULL);

            int l = 0;
            l = addGroupToTree(temp, tempGroup);
            if(l == 1) {
                xmlFreeDoc(returnDoc);
                returnDoc = NULL;
                root = NULL;
                return NULL;
            }
            if(tempGroup->otherAttributes->head != NULL) {
                int value = 0;
                value = addAttrToTree(temp, tempGroup->otherAttributes);
                if(value == 1) {
                    xmlFreeDoc(returnDoc);
                    returnDoc = NULL;
                    root = NULL;
                    return NULL;
                }
            }
        }
    }else {
        xmlFreeDoc(returnDoc);
        returnDoc = NULL;
        root = NULL;
        return NULL;
    }

    return returnDoc;
}


int addAttrToTree(xmlNode* node, List* attributes) {
    ListIterator list;
    ListIterator* tempList = NULL;
    Attribute* tempAttr = NULL;
    int returnInt = 0;


    if(node == NULL || attributes == NULL) return returnInt;

    list = createIterator(attributes);
    tempList = &list;
    tempAttr = (Attribute*) nextElement(tempList);
    for(; tempAttr != NULL; tempAttr = (Attribute*) nextElement(tempList)) {
        if(tempAttr->name == NULL || tempAttr->value == NULL) {
            returnInt = 1;
            return returnInt;
        }
        xmlNewProp(node, BAD_CAST tempAttr->name, BAD_CAST tempAttr->value);
    }

    return returnInt;
}

int addGroupToTree(xmlNode* node, Group* group) {
    xmlNode * temp = NULL;
    int returnInt = 0;

    if(node  == NULL || group == NULL) return returnInt;

    if(group->rectangles != NULL) {
        ListIterator rectList;
        ListIterator* rectangle = NULL;
        Rectangle* tempRect = NULL;
        char units[50] = "";
        char buffer[60] = "";

        rectList = createIterator(group->rectangles);
        rectangle = &rectList;
        tempRect = (Rectangle*) nextElement(rectangle);

        for(; tempRect != NULL; tempRect = (Rectangle*) nextElement(rectangle)) {
            if(tempRect->otherAttributes == NULL) {
                returnInt = 1;
                return returnInt;
            }
           temp = xmlNewChild(node, NULL, BAD_CAST "rect", NULL);
           strcpy(units, tempRect->units);
           if(strcmp(units, "cm") != 0 && strcmp(units, "mm") != 0 && strcmp(units, "ex") != 0 && strcmp(units, "em") != 0 && strcmp(units, "px") != 0 && strcmp(units, "pt") != 0 && strcmp(units, "pc") != 0 && strcmp(units, "in") != 0 && strcmp(units, "") != 0) {
               returnInt = 1;
               return returnInt;
            }
           gcvt(tempRect->x, 8, buffer);
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "x", BAD_CAST buffer);
           strcpy(buffer, "");
           gcvt(tempRect->y, 8, buffer);
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "y", BAD_CAST buffer);
           strcpy(buffer, "");
           gcvt(tempRect->width, 8, buffer);
           if(tempRect->width < 0) {
               returnInt = 1;
               return returnInt;
           }
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "width", BAD_CAST buffer);
           strcpy(buffer, "");
           gcvt(tempRect->height, 8, buffer);
           if(tempRect->height < 0) {
               returnInt = 1;
               return returnInt;
           }
           strcat(buffer, units);
           xmlNewProp(temp, BAD_CAST "height", BAD_CAST buffer);

           if(tempRect->otherAttributes->head != NULL) {
               int value = 0;
               value = addAttrToTree(temp, tempRect->otherAttributes);
               if(value == 1) {
                   returnInt = value;
                   return returnInt;
               }
           }
        }
    } else {
       return returnInt; 
    }

    if(group->circles != NULL) {
        ListIterator circleList;
        ListIterator* circle = NULL;
        Circle* tempCircle = NULL;
        char units[50] = "";
        char buffer[60] = "";

        circleList = createIterator(group->circles);
        circle = &circleList;
        tempCircle = (Circle*) nextElement(circle);

        for(; tempCircle != NULL; tempCircle = (Circle*) nextElement(circle)) {
            if(tempCircle->otherAttributes == NULL) {
                returnInt = 1;
                return returnInt;
            }
            temp = xmlNewChild(node, NULL, BAD_CAST "circle", NULL);
            strcpy(units, tempCircle->units);
            if(strcmp(units, "cm") != 0 && strcmp(units, "mm") != 0 && strcmp(units, "ex") != 0 && strcmp(units, "em") != 0 && strcmp(units, "px") != 0 && strcmp(units, "pt") != 0 && strcmp(units, "pc") != 0 && strcmp(units, "in") != 0 && strcmp(units, "") != 0) {
               returnInt = 1;
               return returnInt;
            }
            gcvt(tempCircle->cx, 8, buffer);
            xmlNewProp(temp, BAD_CAST "cx", BAD_CAST buffer);
            strcpy(buffer, "");
            gcvt(tempCircle->cy, 8, buffer);
            xmlNewProp(temp, BAD_CAST "cy", BAD_CAST buffer);
            strcpy(buffer, "");
            gcvt(tempCircle->r, 8, buffer);
            if(tempCircle->r < 0) {
               returnInt = 1;
               return returnInt;
           }
            xmlNewProp(temp, BAD_CAST "r", BAD_CAST buffer);

            if(tempCircle->otherAttributes->head != NULL) {
                int value = 0;
                value = addAttrToTree(temp, tempCircle->otherAttributes);
                if(value == 1) {
                    returnInt = value;
                    return returnInt;
                }
            }
        }

    }else {
        return returnInt;
    }

    if(group->paths != NULL) {
        ListIterator pathList;
        ListIterator* paths = NULL;
        Path* tempPath = NULL;

        pathList = createIterator(group->paths);
        paths = &pathList;
        tempPath = (Path*) nextElement(paths);

        for(; tempPath != NULL; tempPath = (Path*) nextElement(paths)) {
            if(tempPath->otherAttributes == NULL) {
                returnInt = 1;
                return returnInt;
            }
            temp = xmlNewChild(node, NULL, BAD_CAST "path", NULL);
            if(tempPath->data != NULL) {
                xmlNewProp(temp, BAD_CAST "d", BAD_CAST tempPath->data);
            }else {
                returnInt = 1;
                return returnInt;
            }

            if(tempPath->otherAttributes->head != NULL) {
                int value = 0;
                value = addAttrToTree(temp, tempPath->otherAttributes);
                if(value == 1) {
                    returnInt = value;
                    return returnInt;
                }
            }
        }
    }else {
        return returnInt;
    }

    if(group->groups != NULL) {
        ListIterator groupList;
        ListIterator* groups = NULL;
        Group* tempGroup = NULL;

        groupList = createIterator(group->groups);
        groups = &groupList;
        tempGroup = (Group*) nextElement(groups);

        for(; tempGroup != NULL; tempGroup = (Group*) nextElement(groups)) {
            if(tempGroup->otherAttributes == NULL) {
                returnInt = 1;
                return returnInt;
            }
            temp = xmlNewChild(node, NULL, BAD_CAST "g", NULL);
            
            int l = 0;
            l = addGroupToTree(temp, tempGroup);
            if(l == 1) {
                returnInt = l;
                return returnInt;
            }
            if(tempGroup->otherAttributes->head != NULL) {
                int value = 0;
                value = addAttrToTree(temp, tempGroup->otherAttributes);
                if(value == 1) {
                    returnInt = value;
                    return returnInt;
                }
            }
        }
    }else {
        return returnInt;
    }
    return 0;
}


void addOtherAttr(List* attributeList, Attribute* attribute) {
    ListIterator attr;
    ListIterator* attrList = NULL;
    Attribute* tempAttr = NULL;
    int temp = 0;

    if(attributeList == NULL || attribute == NULL) return;

    attr = createIterator(attributeList);
    attrList = &attr;
    tempAttr = (Attribute*) nextElement(attrList);

    for(; tempAttr != NULL; tempAttr = (Attribute*) nextElement(attrList)) {
        if(strcmp(tempAttr->name, attribute->name) == 0) {
            free(tempAttr->value);
            tempAttr->value = NULL;
            tempAttr->value = (char*) calloc(strlen(attribute->value) + 50, sizeof(char));
            strcpy(tempAttr->value, "");
            strcpy(tempAttr->value, attribute->value);
            temp = 1;
            break;
        }
    }

    if(temp == 0) {
        insertBack(attributeList, attribute);
        return;
    }
}

float attrStringToFloat(char* string) {
    float returnFloat = 0;
    int i = 0;
    char tempString[20] = "";

    if(string == NULL) return returnFloat;

    strcpy(tempString, "");
    i = 0;
    while(string[i] != '\0') {
        if(isdigit(string[i]) == 0 && string[i] != '.') {
            break;
        }
        tempString[i] = string[i];
        if(string[i + 1] == '\0' || isdigit(string[i + 1]) == 0) {
            tempString[i + 1] = '\0';
        }
        i++;
    }
    returnFloat = strtof(tempString, NULL);

    return returnFloat;
}




char* attrToJSON(const Attribute* a) {
    char* attributeString = NULL;

    if(a == NULL){
        attributeString = emptyJSON();
        return attributeString;
    }
    if(a->name == NULL || a->value == NULL) {
        attributeString = emptyJSON();
        return attributeString;
    }
    if(strcmp(a->name, "") == 0) {
        attributeString = emptyJSON();
        return attributeString;
    }

    attributeString = (char*) calloc(12, sizeof(char));
    strcpy(attributeString, "{\"name\":\"");
    attributeString = realloc(attributeString, 12 + strlen(a->name) + 10);
    strcat(attributeString, a->name);
    attributeString = (char*) realloc(attributeString, strlen(attributeString) + 20);
    strcat(attributeString, "\",\"value\":\"");
    attributeString = (char*) realloc(attributeString, strlen(attributeString) + strlen(a->value) + 10);
    strcat(attributeString, a->value);
    strcat(attributeString, "\"}");

    return attributeString;
}



char* circleToJSON(const Circle* c) {
    char* circleString = NULL;

    if(c == NULL){
        circleString = emptyJSON();
        return circleString;
    }
    if(c->otherAttributes == NULL) {
        circleString = emptyJSON();
        return circleString;
    }

    circleString = (char*) calloc(300, sizeof(char));
    if(circleString == NULL) {
        circleString = emptyJSON();
        return circleString;
    }
    strcpy(circleString, "");
    sprintf(circleString, "{\"cx\":%.2f,\"cy\":%2.f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, c->otherAttributes->length, c->units);
    circleString[strlen(circleString)] = '\0';

    return circleString;
}




char* rectToJSON(const Rectangle* r) {
    char* rectangleString = NULL;

    if(r == NULL){
        rectangleString = emptyJSON();
        return rectangleString;
    }
    if(r->otherAttributes == NULL) {
        rectangleString = emptyJSON();
        return rectangleString;
    }

    rectangleString = (char*) calloc(400, sizeof(char));
    if(rectangleString == NULL) {
        rectangleString = emptyJSON();
        return rectangleString;
    }
    strcpy(rectangleString, "");
    sprintf(rectangleString, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, r->otherAttributes->length, r->units);
    rectangleString[strlen(rectangleString)] = '\0';

    return rectangleString;
}




char* pathToJSON(const Path* p) {
    char* pathString = NULL;

    if(p == NULL){
        pathString = emptyJSON();
        return pathString;
    }
    if(p->data == NULL) {
        pathString = emptyJSON();
        return pathString;
    }
    if(strcmp(p->data, "") == 0) {
        pathString = emptyJSON();
        return pathString;
    }
    if(p->otherAttributes == NULL) {
        pathString = emptyJSON();
        return pathString;
    }

    pathString = (char*) calloc(50 + strlen(p->data) + 50, sizeof(char));
    if(pathString == NULL) {
        pathString = emptyJSON();
        return pathString;
    }
    strcpy(pathString, "");
    if(strlen(p->data) > 64) {
        char* tempString = (char*) calloc(70, sizeof(char));
        strcpy(tempString, "");
        strncpy(tempString, p->data, 64);
        tempString[64] = '\0';
        sprintf(pathString, "{\"d\":\"%s\",\"numAttr\":%d}", tempString, p->otherAttributes->length);
        pathString[strlen(pathString)] = '\0';
        return pathString;
    }
    sprintf(pathString, "{\"d\":\"%s\",\"numAttr\":%d}", p->data, p->otherAttributes->length);
    pathString[strlen(pathString)] = '\0';

    return pathString;
}




char* groupToJSON(const Group* g) {
    char* groupString = NULL;
    int listTotal = 0;

    if(g == NULL) {
        groupString = emptyJSON();
        return groupString;
    }
    if(g->rectangles == NULL) {
        groupString = emptyJSON();
        return groupString;
    }
    if(g->circles == NULL) {
        groupString = emptyJSON();
        return groupString;
    }
    if(g->paths == NULL) {
        groupString = emptyJSON();
        return groupString;
    }
    if(g->groups == NULL) {
        groupString = emptyJSON();
        return groupString;
    }
    if(g->otherAttributes == NULL) {
        groupString = emptyJSON();
        return groupString;
    }

    listTotal = g->rectangles->length + g->circles->length + g->paths->length + g->groups->length;
    
    groupString = (char*) calloc(100, sizeof(char));
    if(groupString == NULL) {
        groupString = emptyJSON();
        return groupString;
    }
    strcpy(groupString, "");
    sprintf(groupString, "{\"children\":%d,\"numAttr\":%d}", listTotal, g->otherAttributes->length);
    groupString[strlen(groupString)] = '\0';

    return groupString;
}



char* attrListToJSON(const List* list) {
    char* attrListString = NULL;
    ListIterator attr;
    ListIterator* attrList = NULL;
    Attribute* tempAttr = NULL;
    int count  = 0;
    char* tempString = NULL;
    List* tempList = NULL;
    
    if(list == NULL) {
        attrListString = emptyJSONForList();
        return attrListString;
    }
    if(list->head == NULL) {
        attrListString = emptyJSONForList();
        return attrListString;
    }

    tempList = (List*) list;
    attr = createIterator(tempList);
    attrList = &attr;
    tempAttr = (Attribute*) nextElement(attrList);

    attrListString = (char*) calloc(50, sizeof(char));
    strcpy(attrListString, "");


    for(; tempAttr != NULL; tempAttr = (Attribute*) nextElement(attrList)) {
        if(count == 0) {
            strcpy(attrListString, "[");
        }

        tempString = attrToJSON((const Attribute*) tempAttr);
        attrListString = (char*) realloc(attrListString, strlen(attrListString) + strlen(tempString) + 50);
        strcat(attrListString, tempString);
        free(tempString);
        tempString = NULL;

        if(count == list->length - 1) {
            strcat(attrListString, "]");
            break;
        }
        strcat(attrListString, ",");
        count++;
    }
    tempList = NULL;

    return attrListString;
}




char* circleListToJSON(const List* list) {
    char* circleListString = NULL;
    ListIterator circle;
    ListIterator* circleList = NULL;
    Circle* tempCircle = NULL;
    int count  = 0;
    char* tempString = NULL;
    List* tempList = NULL;

    if(list == NULL) {
        circleListString = emptyJSONForList();
        return circleListString;
    }
    if(list->head == NULL) {
        circleListString = emptyJSONForList();
        return circleListString;
    }

    tempList = (List*) list;
    circle = createIterator(tempList);
    circleList = &circle;
    tempCircle = (Circle*) nextElement(circleList);

    circleListString = (char*) calloc(50, sizeof(char));
    strcpy(circleListString, "");

    for(; tempCircle != NULL; tempCircle = (Circle*) nextElement(circleList)) {
        if(count == 0) {
            strcpy(circleListString, "[");
        }

        tempString = circleToJSON((const Circle*) tempCircle);
        circleListString = (char*) realloc(circleListString, strlen(circleListString) + strlen(tempString) + 50);
        strcat(circleListString, tempString);
        free(tempString);
        tempString = NULL;

        if(count == list->length - 1) {
            strcat(circleListString, "]");
            break;
        }
        strcat(circleListString, ",");
        count++;
    }
    tempList = NULL;

    return circleListString;
}




char* rectListToJSON(const List* list) {
    char* rectListString = NULL;
    ListIterator rectangle;
    ListIterator* rectList = NULL;
    Rectangle* tempRect = NULL;
    int count  = 0;
    char* tempString = NULL;
    List* tempList = NULL;

    if(list == NULL) {
        rectListString = emptyJSONForList();
        return rectListString;
    }
    if(list->head == NULL) {
        rectListString = emptyJSONForList();
        return rectListString;
    }
    tempList = (List*) list;
    rectangle = createIterator(tempList);
    rectList = &rectangle;
    tempRect = (Rectangle*) nextElement(rectList);

    rectListString = (char*) calloc(50, sizeof(char));
    strcpy(rectListString, "");

    for(; tempRect != NULL; tempRect = (Rectangle*) nextElement(rectList)) {
        if(count == 0) {
            strcpy(rectListString, "[");
        }

        tempString = rectToJSON((const Rectangle*) tempRect);
        rectListString = (char*) realloc(rectListString, strlen(rectListString) + strlen(tempString) + 50);
        strcat(rectListString, tempString);
        free(tempString);
        tempString = NULL;

        if(count == list->length - 1) {
            strcat(rectListString, "]");
            break;
        }
        strcat(rectListString, ",");
        count++;
    }
    tempList = NULL;

    return rectListString;
}



char* pathListToJSON(const List* list) {
    char* pathListString = NULL;
    ListIterator path;
    ListIterator* pathList = NULL;
    Path* tempPath = NULL;
    int count  = 0;
    char* tempString = NULL;
    List* tempList = NULL;

    if(list == NULL) {
        pathListString = emptyJSONForList();
        return pathListString;
    }
    if(list->head == NULL) {
        pathListString = emptyJSONForList();
        return pathListString;
    }

    tempList = (List*) list;
    path = createIterator(tempList);
    pathList = &path;
    tempPath = (Path*) nextElement(pathList);

    pathListString = (char*) calloc(50, sizeof(char));
    strcpy(pathListString, "");

    for(; tempPath != NULL; tempPath = (Path*) nextElement(pathList)) {
        if(count == 0) {
            strcpy(pathListString, "[");
        }

        tempString = pathToJSON((const Path*) tempPath);
        pathListString = (char*) realloc(pathListString, strlen(pathListString) + strlen(tempString) + 50);
        strcat(pathListString, tempString);
        free(tempString);
        tempString = NULL;

        if(count == list->length - 1) {
            strcat(pathListString, "]");
            break;
        }
        strcat(pathListString, ",");
        count++;
    }
    tempList = NULL;

    return pathListString;
}


char* groupListToJSON(const List* list) {
    char* groupListString = NULL;
    ListIterator group;
    ListIterator* groupList = NULL;
    Group* tempGroup = NULL;
    int count  = 0;
    char* tempString = NULL;
    List* tempList = NULL;

    if(list == NULL) {
        groupListString = emptyJSONForList();
        return groupListString;
    }
    if(list->head == NULL) {
        groupListString = emptyJSONForList();
        return groupListString;
    }

    tempList = (List*) list;
    group = createIterator(tempList);
    groupList = &group;
    tempGroup = (Group*) nextElement(groupList);

    groupListString = (char*) calloc(50, sizeof(char));
    strcpy(groupListString, "");

    for(; tempGroup != NULL; tempGroup = (Group*) nextElement(groupList)) {
        if(count == 0) {
            strcpy(groupListString, "[");
        }

        tempString = groupToJSON((const Group*) tempGroup);
        groupListString = (char*) realloc(groupListString, strlen(groupListString) + strlen(tempString) + 50);
        strcat(groupListString, tempString);
        free(tempString);
        tempString = NULL;

        if(count == list->length - 1) {
            strcat(groupListString, "]");
            break;
        }
        strcat(groupListString, ",");
        count++;
    }
    tempList = NULL;

    return groupListString;
}



char* SVGtoJSON(const SVGimage* image) {
    char* imageString = NULL;
    List* rectList = NULL;
    List* circleList = NULL;
    List* pathList = NULL;
    List* groupList = NULL;
    SVGimage* tempImage = NULL; 

    if(image == NULL) {
        imageString = emptyJSON();
        return imageString;
    }
    if(image->rectangles == NULL) {
        imageString = emptyJSON();
        return imageString;
    }
    if(image->circles == NULL) {
        imageString = emptyJSON();
        return imageString;
    }
    if(image->paths == NULL) {
        imageString = emptyJSON();
        return imageString;
    }
    if(image->groups == NULL) {
        imageString = emptyJSON();
        return imageString;
    }
    if(image->otherAttributes == NULL) {
        imageString = emptyJSON();
        return imageString;
    }

    imageString = (char*) calloc(100, sizeof(char));
    if(imageString == NULL) {
        imageString = emptyJSON();
        return imageString;
    }
    strcpy(imageString, "");

    tempImage = (SVGimage*) image;
    rectList = getRects(tempImage);
    circleList = getCircles(tempImage);
    pathList = getPaths(tempImage);
    groupList = getGroups(tempImage);
    tempImage = NULL;

    sprintf(imageString, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", rectList->length, circleList->length, pathList->length, groupList->length);
    imageString[strlen(imageString)] = '\0';

    freeListOnly(rectList);
    rectList = NULL;
    freeListOnly(circleList);
    circleList = NULL;
    freeListOnly(pathList);
    pathList = NULL;
    freeListOnly(groupList);
    groupList = NULL;

    return imageString;
}



/***************************************** A2 Bonus Functions *************************************/


SVGimage* JSONtoSVG(const char* svgString) {
    SVGimage* returnSVG = NULL;
    int i = 0;
    char* tempString = NULL;
    char* token = NULL;
    printf("hey ho\n");
    if(svgString == NULL) return NULL;
    if(strcmp(svgString, "") == 0) return NULL;
    returnSVG = initializeSVGImage();
    strcpy(returnSVG->namespace, "http://www.w3.org/2000/svg");
    
    printf("yow yow\n");
    tempString = (char*) calloc(800, sizeof(char));
    if(tempString == NULL) {
        deleteSVGimage(returnSVG);
        return NULL;
    }

    strcpy(tempString, "");
    for(i = 1; i < (strlen(svgString) - 1); i++ ) {
        tempString[i - 1] = svgString[i];
    }
    
    token = strtok(tempString, ",");
    for(; token != NULL; token = strtok(NULL, ",")) {
        if(token[0] == '"' && token[strlen(token - 1)] == '"') {
            char temp[50] = "";

            for(i = 0; i < 5; i++) {
                temp[i] = token[i + 1];
            }
            temp[5] = '\0';
            printf("temp = %s\n", temp);
            printf("%s\n", temp);
            if(strcmp(temp, "title") == 0) {
                char temp2[256] = "";

                for(i = 0; i < 256; i++) {
                    if(token[i + 9] == '"') {
                        break;
                    }
                    temp2[i] = token[i + 9];
                }
                temp2[i] = '\0';
                strcpy(returnSVG->title, temp2);
            } else if(strcmp(temp, "descr") == 0) {
                char temp2[256] = "";

                for(i = 0; i < 256; i++) {
                    if(token[i + 9] == '"') {
                        break;
                    }
                    temp2[i] = token[i + 9];
                }
                temp2[i] = '\0';
                strcpy(returnSVG->description, temp2);
            }else {
                strcpy(tempString, "");
                free(tempString);
                tempString = NULL;
                return returnSVG;
            }

        }else {
            strcpy(tempString, "");
            free(tempString);
            tempString = NULL;
            return returnSVG;
        }
    }
    printf("namespace = %s\n", returnSVG->namespace);
    printf("title = %s\n", returnSVG->title);
    printf("desrcr = %s\n", returnSVG->description);

    strcpy(tempString, "");
    free(tempString);
    tempString = NULL;

    return returnSVG;
}




Rectangle* JSONtoRect(const char* svgString) {
    Rectangle* returnRect = NULL;
    char* token = NULL;
    char* tempString = NULL;
    int i = 0;

    if(svgString == NULL) return NULL;
    if(strcmp(svgString, "") == 0) return NULL;
    if(svgString[0] != '{' && svgString[strlen(svgString - 1)] != '}') return NULL;
    returnRect = initializeRectangle();

    tempString = (char*) calloc(600, sizeof(char));
    if(tempString == NULL) {
        deleteRectangle(returnRect);
        return NULL;
    }
    for(i = 0; i < (strlen(svgString) - 1); i++) {
        tempString[i] = svgString[i + 1];
    }

    token = strtok(tempString, ",");
    for(; token != NULL; token = strtok(NULL, ",")) {
        char temp[50] = "";
        i = 0;
        while(token[i + 1] != '"' || token[i + 1] != '\0') {
            temp[i] = token[i + 1];
            i++;
        }
        temp[i] = '\0';

        if(strcmp(temp, "x") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 4] != '\0') {
                temp2[i] = token[i + 4];
                i++;
            }
            temp2[i] = '\0';
            returnRect->x = strtof(temp2, NULL);

        }else if(strcmp(temp, "y") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 4] != '\0') {
                temp2[i] = token[i + 4];
                i++;
            }
            temp2[i] = '\0';
            returnRect->y = strtof(temp2, NULL);

        }else if(strcmp(temp, "w") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 4] != '\0') {
                temp2[i] = token[i + 4];
                i++;
            }
            temp2[i] = '\0';
            returnRect->width = strtof(temp2, NULL);
        }else if(strcmp(temp, "h") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 4] != '\0') {
                temp2[i] = token[i + 4];
                i++;
            }
            temp2[i] = '\0';
            returnRect->height = strtof(temp2, NULL);
        }else if(strcmp(temp, "units") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 9] != '"') {
                temp2[i] = token[i + 9];
                i++;
            }
            temp2[i] = '\0';
            strcpy(returnRect->units, temp2);
        } else {
            strcpy(tempString, "");
            free(tempString);
            tempString = NULL;
            return returnRect;
        }
    }

    strcpy(tempString, "");
    free(tempString);
    tempString = NULL;

    return returnRect;
}


Circle* JSONtoCircle(const char* svgString) {
    Circle* returnCircle = NULL;
    char* token = NULL;
    char* tempString = NULL;
    int i = 0;

    
    if(svgString == NULL) return NULL;
    if(strcmp(svgString, "") == 0) return NULL;
    if(svgString[0] != '{' && svgString[strlen(svgString - 1)] != '}') return NULL;

    returnCircle = initializeCircle();
    tempString = (char*) calloc(600, sizeof(char));
    if(tempString == NULL) { 
        deleteCircle(returnCircle);
        return NULL;
    }
    for(i = 0; i < (strlen(svgString) - 1); i++) {
        tempString[i] = svgString[i + 1];
    }

     token = strtok(tempString, ",");
    for(; token != NULL; token = strtok(NULL, ",")) {
        char temp[50] = "";
        i = 0;
        while(token[i + 1] != '"' || token[i + 1] != '\0') {
            temp[i] = token[i + 1];
            i++;
        }
        temp[i] = '\0';

        if(strcmp(temp, "cx") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 5] != '\0') {
                temp2[i] = token[i + 5];
                i++;
            }
            temp2[i] = '\0';
            returnCircle->cx = strtof(temp2, NULL);

        }else if(strcmp(temp, "cy") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 5] != '\0') {
                temp2[i] = token[i + 5];
                i++;
            }
            temp2[i] = '\0';
            returnCircle->cy = strtof(temp2, NULL);

        }else if(strcmp(temp, "r") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 4] != '\0') {
                temp2[i] = token[i + 4];
                i++;
            }
            temp2[i] = '\0';
            returnCircle->r = strtof(temp2, NULL);
        }else if(strcmp(temp, "units") == 0) {
            char temp2[100] = "";
            i = 0;
            while(token[i + 9] != '"') {
                temp2[i] = token[i + 9];
                i++;
            }
            temp2[i] = '\0';
            strcpy(returnCircle->units, temp2);
        } else {
            strcpy(tempString, "");
            free(tempString);
            tempString = NULL;
            return returnCircle;
        }
    }

    strcpy(tempString, "");
    free(tempString);
    tempString = NULL;


    return returnCircle;
}



char* emptyJSON() {
    char* string = NULL;

    string = (char*) calloc(3, sizeof(char));
    strcpy(string, "{}");

    return string;
}

char* emptyJSONForList() {
    char* string = NULL;

    string = (char*) calloc(3, sizeof(char));
    strcpy(string, "[]");

    return string;
}




void print_element_names(xmlNode* a_node) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        // Uncomment the code below if you want to see the content of every node.

        if (cur_node->content != NULL ){
            printf("  content: %s\n", cur_node->content);
        }

        // Iterate through every attribute of the current node
        xmlAttr *attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }

        print_element_names(cur_node->children);
    }
}


/***************************************** A3 Functions *************************************/

char* JsSVG(char* fileName) {
    char* returnString = calloc(1, sizeof(char) * 1000);
    
    if(fileName == NULL) {
        strcpy(returnString, "{}");
        return returnString;
    }
    if(strcmp(fileName, "") == 0) {
        strcpy(returnString, "{}");
        return returnString;
    }
    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);
    SVGimage* svg = createValidSVGimage(directory, "svg.xsd");
    free(directory);
    directory = NULL;
    if(svg == NULL) {
        strcpy(returnString, "{\"svg is null\"}");
        return returnString;
    }
    free(returnString);
    returnString = SVGtoJSON((const SVGimage*) svg);
    if(returnString == NULL) {
        deleteSVGimage(svg);
        svg = NULL;
        returnString = calloc(1, sizeof(char) * 1000);
        strcpy(returnString, "{\"json to svg is null\"}");
        return returnString;
    }

    deleteSVGimage(svg);
    svg = NULL;

    return returnString;
}

bool vaildateJavascript(char* fileName) {
    SVGimage* image = NULL;
    if(fileName == NULL) return false;
    if(strcmp(fileName, "") == 0) return false;
    
    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);

    image = createSVGimage(directory);
    free(directory);
    directory = NULL;

    if(image == NULL){ 
        return NULL;
    }
    bool value = validateSVGimage(image, "svg.xsd");
    if(value == false) {
        deleteSVGimage(image);
        image = NULL;
        return false;
    }

    deleteSVGimage(image);
    image = NULL;

    return true;
}


char* createSVG(char* fileName) {
    if(fileName == NULL) return NULL;
    if(strcmp(fileName, "") == 0) return NULL;

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);
    SVGimage* svg = createValidSVGimage(directory, "svg.xsd");
    free(directory);
    directory = NULL;
    if(svg == NULL) return NULL;

    char* returnString = calloc(1, sizeof(char) * 6000);
    sprintf(returnString, "{\"title\":\"%s\",\"desc\":\"%s\",\"rects\":%s,\"circles\":%s,\"paths\":%s,\"groups\":%s}", svg->title, svg->description, rectListToJSON((const List*) svg->rectangles), circleListToJSON((const List*) svg->circles), pathListToJSON((const List*) svg->paths), groupListToJSON((const List*) svg->groups));
    
    deleteSVGimage(svg);
    svg = NULL;

    return returnString;
} 



char* showOtherAttributes(char* fileName, char* shape, int index) {
    SVGimage* image = NULL;
    char* returnString = NULL;
    if(fileName == NULL || shape == NULL) return NULL;
    if(index < 0) return NULL;
    if(strcmp(fileName, "") == 0 || strcmp(shape, "") == 0) return NULL;

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);
    image = createValidSVGimage(directory, "svg.xsd");
    free(directory);
    directory = NULL;
    if(image == NULL) return NULL;


    if(strcmp(shape, "rect") == 0) {
        ListIterator* rectIterator = NULL;
        ListIterator rIterator;
        Rectangle* tempRect = NULL;
        List* tempList = (List*) image->rectangles;

        rIterator = createIterator(tempList);
        rectIterator = &rIterator;
        tempRect = (Rectangle*) nextElement(rectIterator);

        for(int i = 0; tempRect != NULL; tempRect = (Rectangle*) nextElement(rectIterator)) {
            if(i == index) {
                returnString = attrListToJSON((const List*) tempRect->otherAttributes);
                deleteSVGimage(image);
                image = NULL;
                break;
            }
            i++;
        }
    }else if(strcmp(shape, "circle") == 0) {
        ListIterator* circleIterator = NULL;
        ListIterator cIterator;
        Circle* tempCircle = NULL;
        List* tempList = (List*) image->circles;

        cIterator = createIterator(tempList);
        circleIterator = &cIterator;
        tempCircle = (Circle*) nextElement(circleIterator);

        for(int i = 0; tempCircle != NULL; tempCircle = (Circle*) nextElement(circleIterator)) {
            if(i == index) {
                returnString = attrListToJSON((const List*) tempCircle->otherAttributes);
                deleteSVGimage(image);
                image = NULL;
                break;
            }
            i++;
        }

    }else if(strcmp(shape, "path") == 0) {
        ListIterator* pathIterator = NULL;
        ListIterator pIterator;
        Path* tempPath = NULL;
        List* tempList = (List*) image->circles;

        pIterator = createIterator(tempList);
        pathIterator = &pIterator;
        tempPath = (Path*) nextElement(pathIterator);

        for(int i = 0; tempPath != NULL; tempPath = (Path*) nextElement(pathIterator)) {
            if(i == index) {
                returnString = attrListToJSON((const List*) tempPath->otherAttributes);
                deleteSVGimage(image);
                image = NULL;
                break;
            }
            i++;
        }

    }else if(strcmp(shape, "group") == 0) {
        ListIterator* gorupIterator = NULL;
        ListIterator gIterator;
        Group* tempGroup = NULL;
        List* tempList = (List*) image->circles;

        gIterator = createIterator(tempList);
        gorupIterator = &gIterator;
        tempGroup = (Group*) nextElement(gorupIterator);

        for(int i = 0; tempGroup != NULL; tempGroup = (Group*) nextElement(gorupIterator)) {
            if(i == index) {
                returnString = attrListToJSON((const List*) tempGroup->otherAttributes);
                deleteSVGimage(image);
                image = NULL;
                break;
            }
            i++;
        }
    }

    return returnString;
}




void editAttribute(char* fileName, char* shape, int index, char* name, char* value) {
    Attribute* toAdd = NULL;
    SVGimage* image = NULL;
    if(fileName == NULL || shape == NULL || name == NULL || value == NULL) return;
    if(strcmp(fileName, "") == 0 || strcmp(shape, "") == 0 || strcmp(name, "") == 0 || strcmp(value, "") == 0 || index < 0) return;

    toAdd = initializeAttribute();

    strcpy(toAdd->name, name);
    strcpy(toAdd->value, value);

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);
    image = createValidSVGimage(directory, "svg.xsd");
    if(image == NULL) {
        deleteAttribute(toAdd);
        toAdd = NULL;
        free(directory);
        directory = NULL;
        return;
    }

    if(strcmp(shape, "Rectangle") == 0) {
        elementType element = 2;
        setAttribute(image, element, index, toAdd);
    }else if(strcmp(shape, "Circle") == 0) {
        elementType element = 1;
        setAttribute(image, element, index, toAdd);
    }else if(strcmp(shape, "Path") == 0) {
        elementType element = 3;
        setAttribute(image, element, index, toAdd);
    }else if(strcmp(shape, "Group") == 0) {
        elementType element = 4;
        setAttribute(image, element, index, toAdd);
    }

    validateSVGimage(image, "svg.xsd");
    writeSVGimage(image, directory);

    deleteSVGimage(image);
    image = NULL;
    free(directory);
    directory = NULL;

    return;
}


bool editTitle(char* fileName, char* string, char* value) {
    bool solution = false;
    SVGimage* image = NULL;
    if(fileName == NULL || string == NULL || value == NULL) return solution;
    if(strcmp(fileName, "") == 0 || strcmp(string, "") == 0 || strcmp(value, "") == 0) return solution;

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);
    image = createValidSVGimage(directory, "svg.xsd");
    if(image == NULL) {
        free(directory);
        directory = NULL;
        return solution;
    }

    if(strcmp(string, "title") == 0) {
        strcpy(image->title, value);
    }else if(strcmp(string, "desc") == 0) {
        strcpy(image->description, value);
    }
    bool result = false;
    result = validateSVGimage(image, "svg.xsd");

    if(result == false) {
        free(directory);
        directory = NULL;
        deleteSVGimage(image);
        image = NULL;
        return solution;
    }

    result = writeSVGimage(image, directory);

    if(result == false) {
        free(directory);
        directory = NULL;
        deleteSVGimage(image);
        image = NULL;
        return solution;
    }

    solution = true;

    return solution;
}


int upload(char* fileName, char* newFile) {
    SVGimage* image = NULL;
    if(fileName == NULL || newFile == NULL) return 2;
    if(strcmp(fileName, "") == 0 || strcmp(newFile, "") == 0) return 2;

    image = createValidSVGimage(fileName, "svg.xsd");
    if(image == NULL) return 2;

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, newFile);

    if( access( directory, F_OK ) != -1 ) {
        free(directory);
        directory = NULL;
        deleteSVGimage(image);
        image = NULL;
        return 1;
    }

    bool result = writeSVGimage(image, directory);

    free(directory);
    directory = NULL;
    deleteSVGimage(image);
    image = NULL;

    if(result == false){
        return 2;
    }

    return 0;
}

bool scale(char* fileName, char* shape, int scale) {
    SVGimage* image = NULL;
    if(fileName == NULL || shape == NULL) return false;
    if(strcmp(fileName, "") == 0 || strcmp(shape, "") == 0) return false;

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);

    image = createValidSVGimage(directory, "svg.xsd");
    if(image == NULL) {
        free(directory);
        directory = NULL;
        return false;
    }

    if(strcmp(shape, "rect") == 0) {
        ListIterator* rectIterator = NULL;
        ListIterator rIterator;
        List* tempList = NULL;
        Rectangle* tempRect = NULL;

        tempList = getRects(image);
        rIterator = createIterator(tempList);
        rectIterator = &rIterator;
        tempRect = (Rectangle*) nextElement(rectIterator);

        for(; tempRect != NULL; tempRect = (Rectangle*) nextElement(rectIterator)) {
            tempRect->x = tempRect->x * ((float) scale);
            tempRect->y = tempRect->y * ((float) scale);
            tempRect->width = tempRect->width * ((float) scale);
            tempRect->height = tempRect->height * ((float) scale);
        }
        freeListOnly(tempList);
        tempList = NULL;

    }else if(strcmp(shape, "circle") == 0) {
        ListIterator* circleIterator = NULL;
        ListIterator cIterator;
        List* tempList = NULL;
        Circle* tempCircle = NULL;

        tempList = getCircles(image);
        cIterator = createIterator(tempList);
        circleIterator = &cIterator;
        tempCircle = (Circle*) nextElement(circleIterator);

        for(; tempCircle != NULL; tempCircle = (Circle*) nextElement(circleIterator)) {
            tempCircle->cx = tempCircle->cx * ((float) scale);
            tempCircle->cy = tempCircle->cy * ((float) scale);
            tempCircle->r = tempCircle->r * ((float) scale);
        }
        freeListOnly(tempList);
        tempList = NULL;
    }
    
    bool result = validateSVGimage(image, "svg.xsd");

    if(result == false) {
        free(directory);
        directory = NULL;
        deleteSVGimage(image);
        image = NULL;
        return false;
    }

    result = writeSVGimage(image, directory);

    free(directory);
    directory = NULL;
    deleteSVGimage(image);
    image = NULL;

    if(result == false) {
        return false;
    }

    return true;
}


bool addShape(char* fileName, char* shape, char* jsonString) {
    SVGimage* image = NULL;
    if(fileName == NULL || shape == NULL || jsonString == NULL) return false;
    if(strcmp(fileName, "") == 0 || strcmp(shape, "") == 0 || strcmp(jsonString, "") == 0) return false;

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);

    image = createValidSVGimage(directory, "svg.xsd");
    if(image == NULL) {
        free(directory);
        directory = NULL;
        return false;
    }

    if(strcmp(shape, "rect") == 0) {
        Rectangle* rect = (Rectangle*) JSONtoRect((const char*) jsonString);
        elementType element = 2;
        addComponent(image, element, rect);
        rect = NULL;

    }else if(strcmp(shape, "circle") == 0) {
        Circle* circle = (Circle*) JSONtoCircle((const char*) jsonString);
        elementType element = 1;
        addComponent(image, element, circle);
         circle = NULL;
    }

    bool result = validateSVGimage(image, "svg.xsd");
    if(result == false) {
        free(directory);
        directory = NULL;
        deleteSVGimage(image);
        image = NULL;
        return false;
    }

    result = writeSVGimage(image, directory);

    free(directory);
    directory = NULL;
    deleteSVGimage(image);
    image = NULL;

    if(result == false) {
        return false;
    }

    return true;
}


bool create(char* fileName, char* jsonString) {
    SVGimage* image = NULL;
    if(fileName == NULL || jsonString == NULL) return false;
    if(strcmp(fileName, "") == 0 || strcmp(jsonString, "") == 0) return false;

    image = JSONtoSVG((const char*) jsonString);

    bool result = validateSVGimage(image, "svg.xsd");

    if(result == false) {
        deleteSVGimage(image);
        image = NULL;
        return false;
    }

    char* directory = calloc(1, sizeof(char) * 1000);
    strcpy(directory, "uploads/");
    strcat(directory, fileName);

    FILE* fp = fopen(directory, "w+");
    if(fp == NULL) {
        free(directory);
        directory = NULL;
        deleteSVGimage(image);
        image = NULL;
        return false;
    }
    fclose(fp);
    result = writeSVGimage(image, directory);

    free(directory);
    directory = NULL;
    deleteSVGimage(image);
    image = NULL;

    if(result == false) {
        return false;
    }

    return true;
}