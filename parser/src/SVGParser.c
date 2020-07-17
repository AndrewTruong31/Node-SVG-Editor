#include "SVGParser.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#define PI 3.14159265358979323846

/*
* Name: Andrew Truong
* Student #: 1049488
* Username: Atruon03

* Used Example code from: http://www.xmlsoft.org/examples/tree1.c
*/

/* PROTOTYPES since everything has to be in 1 .c file */
void parseChar(char* in, char out[]);
float parseInt(char* in);
char* parseData(char* in);
char* getStr(char* in);

void freeAttr(void* in);
void initLists(SVGimage* img);
void setTitle(xmlNode* a_attr, SVGimage* img);
void setDesc(xmlNode* a_attr, SVGimage* img);
void setNameSpace( xmlNode* root_element, SVGimage* img);
void parseElements(xmlNode * a_node, SVGimage* img);
void print_attrs(xmlAttr *a_attr);
Group* setGroupAttr(xmlNode * a_node, SVGimage* img);
void initGroupLists(Group* group);
Path* setPathAttr(xmlAttr* a_attr, SVGimage* img);
Circle* setCirAttr(xmlAttr* a_attr, SVGimage* img);
Rectangle* setRectAttr(xmlAttr* a_attr, SVGimage* img);

void getCircleGroup(Node* a_node, List* circles);
void getPathGroup(Node* a_node, List* paths);
void getRectGroup(Node* cur_node, List* rects);
void getGroupGroup(Node* a_node, List* groups);
void freeShallowList(Node* a_node);



/* Creates the SVG image with the given fileName */
SVGimage* createSVGimage(char* fileName){
    
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if(fileName == NULL || strlen(fileName) == 0) return NULL; //returns NULL if the fileName is NULL or is an empty string
    
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) { //returns NULL if the file could not be read/does not exist
        printf("error: could not parse file %s\n", fileName);
        return NULL;
    }

    SVGimage* image = calloc( 1, sizeof(SVGimage)); //allocates memory with the size of SVGimage

    root_element = xmlDocGetRootElement(doc);

    initLists(image); //Allocates memory for each of the lists in the struct of SVGimage

    setNameSpace(root_element, image); //Gets the Namespace of the SVG file

    parseElements(root_element->children, image); //Parses the elements from the XML tree and places them into their respective structs in the SVGimage

    xmlFreeDoc(doc); //Frees anything related to the XML tree
    xmlCleanupParser(); //Frees up any memory used by the XML parser
    return image; //Returns the fully parsed SVGimage
}

/* TOSTRING FUNCTIONS */

/* Takes the information from the SVGimage struct and returns a human readable string */
char* SVGimageToString(SVGimage* img){ 
    char* output;

    if(img == NULL) return NULL;

    output = calloc(strlen(img->title) + 20, sizeof(char));
    strcat(output, "Title: ");
    strcat(output, img->title);
    strcat(output, "\n");

    output = realloc(output, strlen(output) + strlen(img->description) + 20);

    strcat(output, "Description: ");
    strcat(output, img->description);
    strcat(output, "\n");

    output = realloc(output, strlen(output) + strlen(img->namespace) + 20);
    strcat(output, "Namespace: ");
    strcat(output, img->namespace);
    strcat(output, "\n");

    char* rect = toString(img->rectangles);
    output = realloc(output, strlen(output) + strlen(rect) + 20);
    strcat(output, rect);
    free(rect);

    char* cir = toString(img->circles);
    output = realloc(output, strlen(output) + strlen(cir) + 20);
    strcat(output, cir);
    free(cir);

    char* path = toString(img->paths);
    output = realloc(output, strlen(output) + strlen(path) + 20);
    strcat(output, path);
    free(path);

    char* group = toString(img->groups);
    output = realloc(output, strlen(output) + strlen(group) + 20);
    strcat(output, group);
    free(group);

    char* attr = toString(img->otherAttributes);
    output = realloc(output, strlen(output) + strlen(attr) + 20);
    strcat(output, attr);
    free(attr);

    return output;

}

/* goes through the rectangle struct and returns a human readable string */
char* rectangleToString(void* in){
    Rectangle* rect = in;

    if(in == NULL) return NULL;

    char* temp = calloc(10,1);
    char conv[512];
    Node* cur_node = NULL;

    strcat(temp, "Rect: ");
    sprintf(conv, "%f", rect->x);
    temp = realloc(temp, strlen(temp) + strlen(conv) + strlen(rect->units) + 20);
    strcat(temp, "x = ");
    strcat(temp, conv);
    strcat(temp, rect->units);

    sprintf(conv, "%f", rect->y);
    temp = realloc(temp, strlen(temp) + strlen(conv) + strlen(rect->units) + 20);
    strcat(temp, " y = ");
    strcat(temp, conv);
    strcat(temp, rect->units);

    sprintf(conv, "%f", rect->width);
    temp = realloc(temp, strlen(temp) + strlen(conv) + strlen(rect->units) + 20);
    strcat(temp, " width = ");
    strcat(temp, conv);
    strcat(temp, rect->units);

    sprintf(conv, "%f", rect->height);
    temp = realloc(temp, strlen(temp) + strlen(conv) + strlen(rect->units) + 20);
    strcat(temp, " height = ");
    strcat(temp, conv);
    strcat(temp, rect->units);
    

    for (cur_node = rect->otherAttributes->head; cur_node; cur_node = cur_node->next) {
        temp = realloc(temp, strlen(temp) + strlen(((Attribute*)cur_node->data)->name) + strlen(((Attribute*)cur_node->data)->value) + 10);
        strcat(temp, " ");
        strcat(temp, ((Attribute*)cur_node->data)->name);
        strcat(temp, " = ");
        strcat(temp, ((Attribute*)cur_node->data)->value);
        strcat(temp, " ");

    }

    return temp;

}

/* goes through the circle struct and returns a human readable string */
char* circleToString(void* in){
    Circle* cir = in;

    if(in == NULL) return NULL;
    
    char* temp = calloc(10,1);
    char conv[512];
    Node* cur_node = NULL;

    strcat(temp, "Cir: ");
    sprintf(conv, "%f", cir->cx);
    temp = realloc(temp, strlen(temp) + strlen(conv) + strlen(cir->units) + 20);
    strcat(temp, "cx = ");
    strcat(temp, conv);
    strcat(temp, cir->units);

    sprintf(conv, "%f", cir->cy);
    temp = realloc(temp, strlen(temp) + strlen(conv) + strlen(cir->units) + 20);
    strcat(temp, " cy = ");
    strcat(temp, conv);
    strcat(temp, cir->units);

    sprintf(conv, "%f", cir->r);
    temp = realloc(temp, strlen(temp) + strlen(conv) + strlen(cir->units) + 20);
    strcat(temp, " r = ");
    strcat(temp, conv);
    strcat(temp, cir->units);

    for (cur_node = cir->otherAttributes->head; cur_node; cur_node = cur_node->next) {
        
        temp = realloc(temp, strlen(temp) + strlen(((Attribute*)cur_node->data)->name) + strlen(((Attribute*)cur_node->data)->value) + 10);
        strcat(temp, " ");
        strcat(temp, ((Attribute*)cur_node->data)->name);
        strcat(temp, " = ");
        strcat(temp, ((Attribute*)cur_node->data)->value);
        strcat(temp, " ");

    }

    return temp;

}

/* goes through the path struct and returns a human readable string */
char* pathToString(void* in){
    Path* path = in;

    if(in == NULL) return NULL;

    char* temp = calloc(10,1);
    // char conv[50000];
    Node* cur_node = NULL;

    strcat(temp, "Path: ");
    // sprintf(conv, "%s", path->data);
    temp = realloc(temp, strlen(temp) + strlen(path->data) + 10);
    strcat(temp, "d = ");
    strcat(temp, path->data);

    for (cur_node = path->otherAttributes->head; cur_node; cur_node = cur_node->next) {

        temp = realloc(temp, strlen(temp) + strlen(((Attribute*)cur_node->data)->name) + strlen(((Attribute*)cur_node->data)->value) + 10);
        strcat(temp, " ");
        strcat(temp, ((Attribute*)cur_node->data)->name);
        strcat(temp, " = ");
        strcat(temp, ((Attribute*)cur_node->data)->value);
        strcat(temp, " ");

    }
    return temp;

}

/* goes through the attribute struct and returns a human readable string */
char* attributeToString(void* in){
    Attribute* attr = in;

    if(in == NULL) return NULL;

    char* temp = calloc(10,1);
    
    temp = realloc(temp, strlen(attr->name) + strlen(attr->value) + 50);
    strcat(temp, "Other Attributes: ");
    strcat(temp, "Name = ");
    strcat(temp,attr->name);
    strcat(temp, " Value = ");
    strcat(temp,attr->value);
    strcat(temp, " ");

    return temp;

}

/* goes through the group struct and returns a human readable string */
char* groupToString(void* in){
    Group* group = in;

    if(in == NULL) return NULL;

    char* temp = calloc(15,1);
    char conv[512];
    Node* cur_node = NULL;
    char* temp1;
    strcat(temp, "Group: \t\t");
    if(getLength(group->otherAttributes) > 0){
        for (cur_node = group->otherAttributes->head; cur_node; cur_node = cur_node->next) {
            temp = realloc(temp, strlen(temp) + strlen(((Attribute*)cur_node->data)->name) + strlen(((Attribute*)cur_node->data)->value) + 10);
            strcat(temp, ((Attribute*)cur_node->data)->name);
            strcat(temp, " = ");
            strcat(temp, ((Attribute*)cur_node->data)->value);
            strcat(temp, " ");

        }
    }
    strcat(temp, "\n\t\t");

    if(getLength(group->rectangles) > 0){
        for (cur_node = group->rectangles->head; cur_node; cur_node = cur_node->next) {
            temp1 = rectangleToString(cur_node->data);
            sprintf(conv,"%s",temp1);
            temp = realloc(temp, strlen(temp) + strlen(conv) + 20);
            free(temp1);
            strcat(temp,conv);
            strcat(temp, "\n\t\t");
        }
    }

    cur_node = NULL;

    if(getLength(group->circles) > 0){
        for (cur_node = group->circles->head; cur_node; cur_node = cur_node->next) {
            temp1 = circleToString(cur_node->data);
            sprintf(conv,"%s",temp1);
            temp = realloc(temp, strlen(temp) + strlen(conv) + 20);
            free(temp1);
            strcat(temp,conv);
            strcat(temp, "\n\t\t");
        }
    }

    cur_node = NULL;

        if(getLength(group->paths) > 0){
        for (cur_node = group->paths->head; cur_node; cur_node = cur_node->next) {
            temp1 = pathToString(cur_node->data);
            temp = realloc(temp, strlen(temp) + strlen(temp1) + 20);
            strcat(temp,temp1);
            strcat(temp, "\n\t\t");
            free(temp1);
        }
    }

    cur_node = NULL;

    if(getLength(group->groups) > 0){
        for (cur_node = group->groups->head; cur_node; cur_node = cur_node->next) {
            temp1 = groupToString(cur_node->data);
            temp = realloc(temp, strlen(temp) + strlen(temp1) + 20);
            strcat(temp,temp1);
            strcat(temp, "\n\t\t");
            free(temp1);
        }
    }
    
    cur_node = NULL;

    

    return temp;
}


/* FREE/DELETE FUNCTIONS */

/* calls all of the other free functions to free memory used by the SVGimage struct */
void deleteSVGimage(SVGimage* img) {
    if(img == NULL) return;
    freeList(img->rectangles);
    freeList(img->circles);
    freeList(img->paths);
    freeList(img->groups);
    freeList(img->otherAttributes);
    free(img);
}

/* deletes the attributes of the given struct */
void deleteAttribute(void* in){
    if(in == NULL) return;
    Attribute* attr = in;
    free(attr->name);
    free(attr->value);

    free(attr);

}

/* frees any list within the group struct */
void deleteGroup(void* in){
    if(in == NULL) return;
    Group* group = in;
    freeList(group->rectangles);
    freeList(group->circles);
    freeList(group->paths);
    freeList(group->groups);
    freeAttr(group->otherAttributes->head);
    free(group->otherAttributes);

    free(group);
}

/* frees all of the attributes of the rectangle struct */
void deleteRectangle(void* in){
    if(in == NULL) return;
    Rectangle* rect = in;
    freeAttr(rect->otherAttributes->head);
    free(rect->otherAttributes);

    free(rect);
}

/* frees all of the attributes of the circle struct */
void deleteCircle(void* in){
    if(in == NULL) return;
    Circle* cir = in;

    freeAttr(cir->otherAttributes->head);
    free(cir->otherAttributes);

    free(cir);
}

/* frees all of the attributes of the path struct */
void deletePath(void* in){
    if(in == NULL) return;
    Path* path = in;
    free(path->data);
    freeAttr(path->otherAttributes->head);
    free(path->otherAttributes);
    free(path);
}


/* COMPARE FUNCTIONS */

/* compares the 2 input values */
int compareAttributes(const void* a,const void* b){
    int in1 = *(const int*)a;
    int in2 = *(const int*)b;

    return in2-in1;
}

/* compares the 2 input values */
int compareGroups(const void* a,const void* b){
    int in1 = *(const int*)a;
    int in2 = *(const int*)b;

    return in2-in1;
}

/* compares the 2 input values */
int compareRectangles(const void* a,const void* b){
    int in1 = *(const int*)a;
    int in2 = *(const int*)b;

    return in2-in1;
}

/* compares the 2 input values */
int compareCircles(const void* a,const void* b){
    int in1 = *(const int*)a;
    int in2 = *(const int*)b;

    return in2-in1;
}

/* compares the 2 input values */
int comparePaths(const void* a,const void* b){
    int in1 = *(const int*)a;
    int in2 = *(const int*)b;

    return in2-in1;
}

/* GET FUNCTIONS */

/* returns a list of all rectanlges in the SVGimage */
List* getRects(SVGimage* img){
    if(img == NULL) return NULL;
    List* rects = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    Node* cur_node = NULL;

    //iterates through the list of rectangles in the SVGimage
    for(cur_node = img->rectangles->head; cur_node; cur_node = cur_node->next){
        insertBack(rects, cur_node->data);
    }
    
    //checks and adds any rectangles in the group list
    getRectGroup(img->groups->head, rects);

    return rects;
}

/* returns a list of all circles in the SVGimage */
List* getCircles(SVGimage* img){
    if(img == NULL) return NULL;
    List* circles = initializeList(circleToString, deleteCircle, compareCircles);
    Node* cur_node = NULL;

    //iterates through the list of circles in the SVGimage
    for(cur_node = img->circles->head; cur_node; cur_node = cur_node->next){
        insertBack(circles, cur_node->data);
    }
    
    //checks and adds any circles in the group list
    getCircleGroup(img->groups->head, circles);

    return circles;
}

/* returns a list of all paths in the SVGimage */
List* getPaths(SVGimage* img){
    if(img == NULL) return NULL;
    List* paths = initializeList(pathToString, deletePath, comparePaths);
    Node* cur_node = NULL;

    //iterates through the list of paths in the SVGimage
    for(cur_node = img->paths->head; cur_node; cur_node = cur_node->next){
        insertBack(paths, cur_node->data);
    }
    
    //checks and adds any paths in the group list
    getPathGroup(img->groups->head, paths);

    return paths;
}

/* returns a list of all groups in the SVGimage */
List* getGroups(SVGimage* img){
    if(img == NULL) return NULL;
    List* groups = initializeList(groupToString, deleteGroup, compareGroups);
    Node* cur_node = NULL;

    //iterates through the list of groups in the SVGimage
    for(cur_node = img->groups->head; cur_node; cur_node = cur_node->next){
        insertBack(groups, cur_node->data);
    }
    
    //iterates through the list of groups in the SVGimage
    getGroupGroup(img->groups->head, groups);

    return groups;
}

/* NUM AREAS */

/* returns the number of rectangles with the given area */
int numRectsWithArea(SVGimage* img, float area){
    if(img == NULL) return 0;
    area = ceil(area);
    List* rects = getRects(img);
    Node* cur_node = NULL;
    int count = 0;

    //iterates through the list of rectangles and checks their areas
    for(cur_node = rects->head; cur_node; cur_node = cur_node->next){
        if(ceil(((Rectangle*)(cur_node->data))->width * ((Rectangle*)(cur_node->data))->height) == area ){
            count ++;
        }
    }

    freeShallowList(rects->head);
    free(rects);
    return count;
}

/* returns the number of circles with the given area */
int numCirclesWithArea(SVGimage* img, float area){
    if(img == NULL) return 0;
    area = ceil(area);
    List* circles = getCircles(img);
    Node* cur_node = NULL;
    int count = 0;

    //iterates through the list of circles and checks their areas
    for(cur_node = circles->head; cur_node; cur_node = cur_node->next){
        if(ceil((((Circle*)(cur_node->data))->r) * PI * ((Circle*)(cur_node->data))->r) == area ){
            count ++;
        }
    }

    freeShallowList(circles->head);
    free(circles);
    return count;
}

/* returns the number of paths with the given data */
int numPathsWithdata(SVGimage* img, char* data){
    if(img == NULL) return 0;
    List* paths = getPaths(img);
    Node* cur_node = NULL;
    int count = 0;

    //iterates through the list of path and checks their data
    for(cur_node = paths->head; cur_node; cur_node = cur_node->next){
        if(strcasecmp(((Path*)(cur_node->data))->data, data) == 0){
            count ++;
        }
    }
    freeShallowList(paths->head);
    free(paths);
    return count;
}

/* returns the number of groups with the given length */
int numGroupsWithLen(SVGimage* img, int len){
    if(img == NULL) return 0;
    int count = 0;
    int temp = 0;
    Node* cur_node = NULL;

    List* groups = getGroups(img);

    //iterates through the list of groups and checks their lengths
    for(cur_node = groups->head; cur_node; cur_node = cur_node->next){
        temp += getLength(((Group*)(cur_node->data))->rectangles);
        temp += getLength(((Group*)(cur_node->data))->paths);
        temp += getLength(((Group*)(cur_node->data))->circles);
        temp += getLength(((Group*)(cur_node->data))->groups);
        if(temp == len) count ++;
        temp = 0;
    }

    return count;
}

/* returns the number of attributes in the SVGimage */
int numAttr(SVGimage* img){
    if(img == NULL) return 0;
    int count = 0;
    Node* cur_node = NULL;

    List* rects = getRects(img);
    List* circles = getCircles(img);
    List* paths = getPaths(img);
    List* groups = getGroups(img);

    //itterates through every node and gets their number of attributes
    for(cur_node = rects->head; cur_node; cur_node = cur_node->next){
        count += getLength(((Rectangle*)(cur_node->data))->otherAttributes);
    }
    for(cur_node = circles->head; cur_node; cur_node = cur_node->next){
        count += getLength(((Circle*)(cur_node->data))->otherAttributes);
    }
    for(cur_node = paths->head; cur_node; cur_node = cur_node->next){
        count += getLength(((Path*)(cur_node->data))->otherAttributes);
    }
    for(cur_node = groups->head; cur_node; cur_node = cur_node->next){
        count += getLength(((Group*)(cur_node->data))->otherAttributes);
    }
    count += getLength(img->otherAttributes);

    freeShallowList(rects->head);
    free(rects);
    freeShallowList(circles->head);
    free(circles);
    freeShallowList(paths->head);
    free(paths);
    freeShallowList(groups->head);
    free(groups);
    
    return count;
}


/* parses a float out of a string of characters */
float parseInt(char* in){

    float val;
    val = atof(in);

    return val;
}

//allocates memory for the data 
char* parseData(char* in){
    int len = strlen(in);

    char* temp = calloc(len + 1, sizeof(char));

    strncpy(temp, in, len);

    return temp;
}

//parses out the char within a string
void parseChar(char* in, char out[]){
    char temp[50];
    int len = strlen(in);

    if(len == 0){
        out = strcpy(out,"");
    }

    int i,j,k,l;
    for(i = 0; i < len; i ++){
        if((in[i] < '0' || in[i] > '9') && (in[i] != '.')){
            break;
        }
    }

    for(j = i; j < len; j ++){
        if((in[j] >= '0' && in[j] <= '9') || in[j] == '\0'){
            break;
        } 
    }

    for(k = i, l = 0; k < j; k ++, l ++){
        temp[l] = in[k];
    }

    strncpy(out, temp,(j-i));
}

//allocates memory for a string
char* getStr(char* in){
    int len = strlen(in);

    char* temp = calloc(len + 1, sizeof(char));
    strncpy(temp, in, len);
    return temp;
}

/* iterates through the rectangle tree to find and parse relevent data */
Rectangle* setRectAttr(xmlAttr* a_attr, SVGimage* img){
    xmlAttr *cur_attr = NULL;
    Rectangle* rect = calloc(1, sizeof(Rectangle));
    rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    //iterates through the xmlAttributes, if the name matches, parses the information
    for (cur_attr = a_attr; cur_attr; cur_attr = cur_attr->next) {
        if (cur_attr->type == XML_ATTRIBUTE_NODE) {
            if (strcasecmp((char*)cur_attr->name, "x") == 0) {
                rect->x = parseInt((char*)cur_attr->children->content);
                if(strlen(rect->units) == 0){
                    parseChar((char*)cur_attr->children->content, rect->units);
                }
            } else if (strcasecmp((char*)cur_attr->name, "y") == 0) {
                rect->y = parseInt((char*)cur_attr->children->content);
                if(strlen(rect->units) == 0){
                    parseChar((char*)cur_attr->children->content, rect->units);
                }
            } else if (strcasecmp((char*)cur_attr->name, "width") == 0) {
                rect->width = parseInt((char*)cur_attr->children->content);
                if(rect->width < 0) rect->width = 0;
                
                if(strlen(rect->units) == 0){
                    parseChar((char*)cur_attr->children->content, rect->units);
                }
            }else if (strcasecmp((char*)cur_attr->name, "height") == 0) {
                rect->height = parseInt((char*)cur_attr->children->content);
                if(rect->height < 0) rect->height = 0;

                if(strlen(rect->units) == 0){
                    parseChar((char*)cur_attr->children->content, rect->units);
                }
            }else{
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr((char*)cur_attr->name);
                tempAttr->value = getStr((char*)cur_attr->children->content);
                insertBack(rect->otherAttributes, tempAttr);
                
            }
            
        }

    }

    return rect;
}

/* iterates though the circle tree to find and parse relevent data */
Circle* setCirAttr(xmlAttr* a_attr, SVGimage* img){
    xmlAttr *cur_attr = NULL;
    Circle* cir = calloc(1, sizeof(Circle));
    cir->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    //iterates through the xmlAttributes, if the name matches, parses the information
    for (cur_attr = a_attr; cur_attr; cur_attr = cur_attr->next) {
        if (cur_attr->type == XML_ATTRIBUTE_NODE) {

            if (strcasecmp((char*)cur_attr->name, "cx") == 0) {
                cir->cx = parseInt((char*)cur_attr->children->content);
                
                if(strlen(cir->units) == 0){
                    parseChar((char*)cur_attr->children->content, cir->units);
                }
            } else if (strcasecmp((char*)cur_attr->name, "cy") == 0) {
                cir->cy = parseInt((char*)cur_attr->children->content);
                if(strlen(cir->units) == 0){
                    parseChar((char*)cur_attr->children->content, cir->units);
                }
            } else if (strcasecmp((char*)cur_attr->name, "r") == 0) {
                cir->r = parseInt((char*)cur_attr->children->content);
                if(cir->r < 0) cir->r = 0;

                if(strlen(cir->units) == 0){
                    parseChar((char*)cur_attr->children->content, cir->units);
                }
            } else {
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr((char*)cur_attr->name);
                tempAttr->value = getStr((char*)cur_attr->children->content);
                insertBack(cir->otherAttributes, tempAttr);
            }
            
        }
    }

    return cir;
}

/* iterates though the path tree to find and parse relevent data */
Path* setPathAttr(xmlAttr* a_attr, SVGimage* img){
    xmlAttr *cur_attr = NULL;
    Path* path = calloc(1, sizeof(Path));
    path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    //iterates through the xmlAttributes, if the name matches, parses the information
    for(cur_attr = a_attr; cur_attr; cur_attr = cur_attr->next){
        if(cur_attr->type == XML_ATTRIBUTE_NODE) {
            if (strcasecmp((char*)cur_attr->name, "d") == 0) {
                path->data = parseData((char*)cur_attr->children->content);
                
            } else {
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr((char*)cur_attr->name);
                tempAttr->value = getStr((char*)cur_attr->children->content);
                insertBack(path->otherAttributes, tempAttr);
            }
        }
    }

    return path;

}

/* initializes all the lists in the group struct */
void initGroupLists(Group* group){
    group->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    group->circles = initializeList(circleToString, deleteCircle, compareCircles);
    group->paths = initializeList(pathToString, deletePath, comparePaths);
    group->groups = initializeList(groupToString, deleteGroup, compareGroups);
    group->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
}

/* iterates though the group tree to find and parse relevent data */
Group* setGroupAttr(xmlNode * a_node, SVGimage* img) {
    xmlNode *cur_node = NULL;
    Group* group = calloc(1,sizeof(Group));
    initGroupLists(group);

    //iterates through the xmlAttributes, if the name matches, parses the information
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {//Iterates through the branches
        if (cur_node->type == XML_ELEMENT_NODE) {
            // printf("node type: Element, name: %s \n", cur_node->name);
            if(strcasecmp((char*)cur_node->name, "rect") == 0){
                Rectangle* rect = setRectAttr(cur_node->properties, img);
                insertBack(group->rectangles, rect);

            } else if(strcasecmp((char*)cur_node->name, "path") == 0){

                Path* path = setPathAttr(cur_node->properties, img);
                insertBack(group->paths, path);

            }else if(strcasecmp((char*)cur_node->name, "circle") == 0){

                Circle* cir = setCirAttr(cur_node->properties, img);
                insertBack(group->circles, cir);

            }else if(strcasecmp((char*)cur_node->name, "g") == 0){
                //recurses through the groups in case they are nested
                Group* tempGroup = setGroupAttr(cur_node->children, img);
                insertBack(group->groups, tempGroup);
            } else {
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr((char*)cur_node->name);
                tempAttr->value = getStr((char*)cur_node->children->content);
                insertBack(group->otherAttributes, tempAttr);
            }
        }

    }

    return group;
}

/* iterates though the XML tree to find and parse relevent data in the SVG */
void parseElements(xmlNode* a_node, SVGimage* img) {
    xmlNode *cur_node = NULL;
    xmlAttr *cur_attr = NULL;

    //gets the SVG attributes if they exist
    for (cur_attr = a_node->parent->properties; cur_attr; cur_attr = cur_attr->next) {
        if (cur_attr->type == XML_ATTRIBUTE_NODE) {
            Attribute* tempAttr = calloc(1, sizeof(Attribute));
            tempAttr->name = getStr((char*)cur_attr->name);
            tempAttr->value = getStr((char*)cur_attr->children->content);
            insertBack(img->otherAttributes, tempAttr);
        }
    }
    
    //iterates through the xmlAttributes, if the name matches, parses the information
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if(strcasecmp((char*)cur_node->name, "rect") == 0){

                Rectangle* rect = setRectAttr(cur_node->properties, img);
                insertBack(img->rectangles, rect);

            } else if(strcasecmp((char*)cur_node->name, "path") == 0){
                Path* path = setPathAttr(cur_node->properties, img);
                insertBack(img->paths, path);

            }else if(strcasecmp((char*)cur_node->name, "circle") == 0){
                Circle* cir = setCirAttr(cur_node->properties, img);
                insertBack(img->circles, cir);
            }else if(strcasecmp((char*)cur_node->name, "g") == 0){
                Group* group = setGroupAttr(cur_node->children, img);
                if(cur_node->properties){
                    Attribute* tempAttr = calloc(1, sizeof(Attribute));
                    tempAttr->name = getStr((char*)cur_node->properties->name);
                    tempAttr->value = getStr((char*)cur_node->properties->children->content);
                    insertBack(group->otherAttributes, tempAttr);
                }
                insertBack(img->groups, group);
            }else if(strcasecmp((char*)cur_node->name, "title") == 0){
                setTitle(cur_node->children, img);
            }else if(strcasecmp((char*)cur_node->name, "desc") == 0){
                setDesc(cur_node->children, img);
            }else{
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr((char*)cur_node->name);
                tempAttr->value = getStr((char*)cur_node->children->content);
                insertBack(img->otherAttributes, tempAttr);
            }
        }

        
    }
}

/* sets the namespace in the XML tree */
void setNameSpace( xmlNode* root_element, SVGimage* img) {
    //TODO error handling
    if (strlen((char*)root_element->ns->href) < 256){
        strcpy( img->namespace, (char*)root_element->ns->href);
    } 
}

/* sets the description in the SVG image */
void setDesc(xmlNode* a_attr, SVGimage* img){
    if(a_attr != NULL)
        strncpy(img->description, (char*)a_attr->content, 255);
    else
        strncpy(img->description, "", 255);
}

/* sets the title in the SVG image */
void setTitle(xmlNode* a_attr, SVGimage* img) {
    if(a_attr != NULL)
        strncpy(img->title, (char*)a_attr->content, 255);
    else
        strncpy(img->title, "",255);
}

/* initializes all of the lists in the SVGimage */
void initLists(SVGimage* img) {
    img->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    img->circles = initializeList(circleToString, deleteCircle, compareCircles);
    img->paths = initializeList(pathToString, deletePath, comparePaths);
    img->groups = initializeList(groupToString, deleteGroup, compareGroups);
    img->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
}

/* frees all of the attributes in the given node */
void freeAttr(void* in){
    Node* cur_node = in;
    if(cur_node == NULL) return;
    
    free(((Attribute*)cur_node->data)->name);
    free(((Attribute*)cur_node->data)->value);
    free(((Attribute*)cur_node->data));

    freeAttr(cur_node->next);

    free(cur_node);

}

/* adds all the rectangles in the group list */
void getRectGroup(Node* a_node, List* rects){
    if(a_node == NULL) return;
    Node* cur_node = NULL;
    if(((Group*)(a_node->data))->rectangles->head){
        for(cur_node = ((Group*)(a_node->data))->rectangles->head; cur_node; cur_node = cur_node->next){
            insertBack(rects, ((Rectangle*)(cur_node->data)));           
        }
        
    }
    getRectGroup(((Group*)(a_node->data))->groups->head, rects);
    getRectGroup(a_node->next, rects);
}

/* adds all the circles in the group list */
void getCircleGroup(Node* a_node, List* circles){
    if(a_node == NULL) return;
    Node* cur_node = NULL;
    if(((Group*)(a_node->data))->circles->head){
        for(cur_node = ((Group*)(a_node->data))->circles->head; cur_node; cur_node = cur_node->next){
            insertBack(circles, ((Circle*)(cur_node->data)));
        }
        
    }
    getCircleGroup(((Group*)(a_node->data))->groups->head, circles);
    getCircleGroup(a_node->next, circles);
}

/* adds all the paths in the group list */
void getPathGroup(Node* a_node, List* paths){
    if(a_node == NULL) return;
    Node* cur_node = NULL;
    if(((Group*)(a_node->data))->paths->head){
        for(cur_node = ((Group*)(a_node->data))->paths->head; cur_node; cur_node = cur_node->next){
            insertBack(paths, ((Path*)(cur_node->data)));
        }
        
    }
    getPathGroup(((Group*)(a_node->data))->groups->head, paths);
    getPathGroup(a_node->next, paths);
}

/* adds all the groups in the group list */
void getGroupGroup(Node* a_node, List* groups){
    if(a_node == NULL) return;
    Node* cur_node = NULL;
    if(((Group*)(a_node->data))->groups->head){
        for(cur_node = ((Group*)(a_node->data))->groups->head; cur_node; cur_node = cur_node->next){
            insertBack(groups, ((Group*)(cur_node->data)));
        }
        
    }
    getGroupGroup(a_node->next, groups);
}

/* frees the created lists without freeing the data within them */
void freeShallowList(Node* a_node){
    if(a_node == NULL) return;
    
    freeShallowList(a_node->next);
    free(a_node);
}




/* A2 FUNCTIONS */
void setRootProp(SVGimage* image, xmlNodePtr root_node);
void setRectProp(List* rectangles, xmlNodePtr root_node);
void setCirProp(List* circles, xmlNodePtr root_node);
void setPathProp(List* paths, xmlNodePtr root_node);
void setGroupProp(List* groups, xmlNodePtr root_node);
bool checkDoc(xmlDoc* doc, char* schemaFile);
bool writeSVGimage(SVGimage* image, char* fileName);
void createTree(SVGimage* image, xmlNodePtr doc);

bool validateRect(SVGimage* image);
bool validateCir(SVGimage* image);
bool validatePath(SVGimage* image);
bool validateGroup(SVGimage* image);
bool validateAttr(List* otherAttributes);


SVGimage* createValidSVGimage(char* fileName, char* schemaFile){
    xmlDoc *doc = NULL;

    if(fileName == NULL || strlen(fileName) == 0) return NULL; //returns NULL if the fileName is NULL or is an empty string
    
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) { //returns NULL if the file could not be read/does not exist
        printf("error: could not parse file %s\n", fileName);
        return NULL;
    }

    if(!checkDoc(doc, schemaFile)){
        printf("The SVG file does not validate\n");
        return NULL;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return createSVGimage(fileName);
}

bool writeSVGimage(SVGimage* doc, char* fileName){
    xmlDocPtr doc1 = NULL;
    xmlNodePtr root_node = NULL;
    xmlNsPtr ns;
    int check;

    if(doc == NULL || fileName == NULL) return 0;

    doc1 = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc1, root_node);

    ns = xmlNewNs(root_node, BAD_CAST doc->namespace, NULL);
    xmlSetNs(root_node, ns);

    createTree(doc, root_node);

    check = xmlSaveFileEnc(fileName,  doc1, "UTF-8");
    xmlFreeDoc(doc1);
    xmlCleanupParser();

    return check;
}

void createTree(SVGimage* image, xmlNodePtr root_node){
    setRootProp(image, root_node);
    setRectProp(image->rectangles, root_node);
    setCirProp(image->circles, root_node);
    setPathProp(image->paths, root_node);
    setGroupProp(image->groups, root_node);
}

bool validateSVGimage(SVGimage* image, char* schemaFile){
    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNsPtr ns;
    int check;
    
    if(image == NULL || schemaFile == NULL) return 0;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);
    ns = xmlNewNs(root_node, BAD_CAST image->namespace, NULL);
    xmlSetNs(root_node, ns);

    if(!validateRect(image) || !validateCir(image)
        || !validatePath(image) || !validateGroup(image)){
            return 0;
    }

    createTree(image, root_node);

    check = checkDoc(doc, schemaFile);
    xmlFreeDoc(doc);

    if(check){
        return check;
    }else return 0;

    
}

bool validateRect(SVGimage* image){
    Node* cur_node;
    if(image->rectangles == NULL) return 0;
    for(cur_node = image->rectangles->head; cur_node; cur_node = cur_node->next){
        if(((Rectangle*)(cur_node->data))->width < 0 || ((Rectangle*)(cur_node->data))->height < 0){
            return 0;
        }
        if(!validateAttr(((Rectangle*)(cur_node->data))->otherAttributes)){
            return 0;
        }
    }
    return 1;
}

bool validateCir(SVGimage* image){
    Node* cur_node;
    if(image->circles == NULL) return 0;
    for(cur_node = image->circles->head; cur_node; cur_node = cur_node->next){
        if(((Circle*)(cur_node->data))->r < 0){
            return 0;
        }
        if(!validateAttr(((Circle*)(cur_node->data))->otherAttributes)){
            return 0;
        }
    }
    return 1;
}

bool validatePath(SVGimage* image){
    Node* cur_node;
    if(image->paths == NULL) return 0;
    for(cur_node = image->paths->head; cur_node; cur_node = cur_node->next){
        if(((Path*)(cur_node->data))->data == NULL){
            return 0;
        }
        if(!validateAttr(((Path*)(cur_node->data))->otherAttributes)){
            return 0;
        }
    }
    return 1;
}

bool validateGroup(SVGimage* image){
    Node* cur_node;
    if(image->groups == NULL) return 0;
    for(cur_node = image->groups->head; cur_node; cur_node = cur_node->next){
        if(((Group*)(cur_node->data))->rectangles == NULL || ((Group*)(cur_node->data))->circles == NULL 
            || ((Group*)(cur_node->data))->paths == NULL || ((Group*)(cur_node->data))->groups == NULL ){
                if(((Group*)(cur_node->data))->rectangles == NULL) printf("RECT\n");
                if(((Group*)(cur_node->data))->circles == NULL) printf("CIR\n");
                if(((Group*)(cur_node->data))->paths == NULL) printf("PATH\n");
                if(((Group*)(cur_node->data))->groups == NULL) printf("GROUPS\n");
            return 0;
        }
        if(!validateAttr(((Group*)(cur_node->data))->otherAttributes)){
            return 0;
        }
    }
    return 1;
}

bool validateAttr(List* otherAttributes){
    Node* cur_node;
    if(otherAttributes != NULL){
        for(cur_node = otherAttributes->head; cur_node; cur_node = cur_node->next){
            if(((Attribute*)(cur_node->data))->value == NULL || ((Attribute*)(cur_node->data))->name == NULL){
                return 0;
            }
        }
    }else return 0;
        
    return 1;
}



bool checkDoc(xmlDoc* doc, char* schemaFile){
    /* Validation */
    int ret;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);
    ctxt = xmlSchemaNewParserCtxt(schemaFile);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    if(doc == NULL){
        fprintf(stderr, "Could not parse\n");
    }else{
        xmlSchemaValidCtxtPtr ctxt;

		ctxt = xmlSchemaNewValidCtxt(schema);
		xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
		ret = xmlSchemaValidateDoc(ctxt, doc);
		
		xmlSchemaFreeValidCtxt(ctxt);
    }

    if(schema != NULL)
    	xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    if (ret == 0){
        printf("Validates\n");
		return 1;
	}else if (ret > 0){
        printf("Doesn't validate\n");
        return 0;
	}else{
        printf("Doesn't validate\n");
        return 0;
	}
}

void setRootProp(SVGimage* image, xmlNodePtr root_node){
    Node* cur_node = NULL;

    if(strlen(image->title) != 0)
        xmlNewChild(root_node, NULL, BAD_CAST "title", BAD_CAST image->title);
    if(strlen(image->description) != 0)
        xmlNewChild(root_node, NULL, BAD_CAST "desc", BAD_CAST image->description);

    for(cur_node = image->otherAttributes->head; cur_node; cur_node = cur_node->next){
        xmlNewProp(root_node, BAD_CAST (((Attribute*)(cur_node->data))->name), BAD_CAST (((Attribute*)(cur_node->data))->value));
    }

}

void setRectProp(List* rectangles, xmlNodePtr root_node){
    Node* cur_node = NULL;
    Node* attr_node = NULL;
    xmlNodePtr temp_node;
    char temp[255];

    //goes through all of the rectangles in the SVGimage
    for(cur_node = rectangles->head; cur_node; cur_node = cur_node->next){
        temp_node = xmlNewChild(root_node, NULL, BAD_CAST "rect", NULL);

        snprintf(temp, sizeof(temp), "%f", ((Rectangle*)(cur_node->data))->x);
        xmlNewProp(temp_node, BAD_CAST "x", BAD_CAST temp);

        snprintf(temp, sizeof(temp), "%f", ((Rectangle*)(cur_node->data))->y);
        xmlNewProp(temp_node, BAD_CAST "y", BAD_CAST temp);

        snprintf(temp, sizeof(temp), "%f%s", ((Rectangle*)(cur_node->data))->width, ((Rectangle*)(cur_node->data))->units);
        xmlNewProp(temp_node, BAD_CAST "width", BAD_CAST temp);

        snprintf(temp, sizeof(temp), "%f%s", ((Rectangle*)(cur_node->data))->height, ((Rectangle*)(cur_node->data))->units);
        xmlNewProp(temp_node, BAD_CAST "height", BAD_CAST temp);

        for(attr_node = ((Rectangle*)(cur_node->data))->otherAttributes->head; attr_node; attr_node = attr_node->next){
            xmlNewProp(temp_node, BAD_CAST (((Attribute*)(attr_node->data))->name), BAD_CAST (((Attribute*)(attr_node->data))->value));
        }

        // xmlAddChild(root_node, temp_node);
    }
}

void setCirProp(List* circles, xmlNodePtr root_node){
    Node* cur_node = NULL;
    Node* attr_node = NULL;
    xmlNodePtr temp_node;
    char temp[255];
    for(cur_node = circles->head; cur_node; cur_node = cur_node->next){
        temp_node = xmlNewChild(root_node, NULL, BAD_CAST "circle", NULL);

        snprintf(temp, sizeof(temp), "%f", ((Circle*)(cur_node->data))->cx);
        xmlNewProp(temp_node, BAD_CAST "cx", BAD_CAST temp);

        snprintf(temp, sizeof(temp), "%f", ((Circle*)(cur_node->data))->cy);
        xmlNewProp(temp_node, BAD_CAST "cy", BAD_CAST temp);

        snprintf(temp, sizeof(temp), "%f%s", ((Circle*)(cur_node->data))->r, ((Circle*)(cur_node->data))->units);
        xmlNewProp(temp_node, BAD_CAST "r", BAD_CAST temp);

        if(((Circle*)(cur_node->data))->otherAttributes){
            for(attr_node = ((Circle*)(cur_node->data))->otherAttributes->head; attr_node; attr_node = attr_node->next){
                xmlNewProp(temp_node, BAD_CAST (((Attribute*)(attr_node->data))->name), BAD_CAST (((Attribute*)(attr_node->data))->value));
            }
        }
            
        

        // xmlAddChild(root_node, temp_node);
    }
}

void setPathProp(List* paths, xmlNodePtr root_node){
    Node* cur_node = NULL;
    Node* attr_node = NULL;
    xmlNodePtr temp_node;

    for(cur_node = paths->head; cur_node; cur_node = cur_node->next){
        temp_node = xmlNewChild(root_node, NULL, BAD_CAST "path", NULL);

        xmlNewProp(temp_node, BAD_CAST "d", BAD_CAST (((Path*)(cur_node->data))->data));

        for(attr_node = ((Path*)(cur_node->data))->otherAttributes->head; attr_node; attr_node = attr_node->next){
            xmlNewProp(temp_node, BAD_CAST (((Attribute*)(attr_node->data))->name), BAD_CAST (((Attribute*)(attr_node->data))->value));
        }

        // xmlAddChild(root_node, temp_node);
    }
}

void setGroupProp(List* groups, xmlNodePtr root_node){
    Node* cur_node = NULL;
    Node* attr_node = NULL;
    xmlNodePtr temp_node;

    for(cur_node = groups->head; cur_node; cur_node = cur_node->next){
        temp_node = xmlNewChild(root_node, NULL, BAD_CAST "g", NULL);

        setRectProp(((Group*)(cur_node->data))->rectangles, temp_node);
        setCirProp(((Group*)(cur_node->data))->circles, temp_node);
        setPathProp(((Group*)(cur_node->data))->paths, temp_node);
        setGroupProp(((Group*)(cur_node->data))->groups, temp_node);

        for(attr_node = ((Group*)(cur_node->data))->otherAttributes->head; attr_node; attr_node = attr_node->next){
            xmlNewProp(temp_node, BAD_CAST (((Attribute*)(attr_node->data))->name), BAD_CAST (((Attribute*)(attr_node->data))->value));
        }

        // xmlAddChild(root_node, temp_node);
    }
}

/* Module 2 */
void changeSVG(SVGimage* image, int elemIndex, Attribute* newAttribute);
void changeRect(SVGimage* image, int elemIndex, Attribute* newAttribute);
void changeCir(SVGimage* image, int elemIndex, Attribute* newAttribute);
void changePath(SVGimage* image, int elemIndex, Attribute* newAttribute);
void changeGroup(SVGimage* image, int elemIndex, Attribute* newAttribute);
bool checkAttrSVG(SVGimage* image, Attribute* newAttribute);
bool checkRect(Rectangle* rect, Attribute* newAttribute);
bool checkCir(Circle* cir, Attribute* newAttribute);
bool checkPath(Path* path, Attribute* newAttribute);
bool checkGroup(Group* group, Attribute* newAttribute);
bool validRect(Rectangle* rect);
bool validCir(Circle* cir);
bool validPath(Path* path);

void addComponent(SVGimage* image, elementType type, void* newElement){
    if(image == NULL || newElement == NULL) return;
    
    switch(type){
        case RECT: 
            if(validRect((Rectangle*)newElement)) insertBack(image->rectangles, newElement);
            break;
        case CIRC: 
            if(validCir((Circle*)newElement)) insertBack(image->circles, newElement);
            break;
        case PATH: 
            if(validPath((Path*)newElement)) insertBack(image->paths, newElement);
            break;
        default:
            printf("element type does not exist\n");
            return;
    }
}

void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute){
    if(image == NULL) return;

    if(newAttribute->name == NULL || newAttribute->value == NULL) return;
    
    switch(elemType){
        case SVG_IMAGE: changeSVG(image, elemIndex, newAttribute);
            break;
        case RECT: changeRect(image, elemIndex, newAttribute);
            break;
        case CIRC: changeCir(image, elemIndex, newAttribute);
            break;
        case PATH: changePath(image, elemIndex, newAttribute);
            break;
        case GROUP: changeGroup(image, elemIndex, newAttribute);
            break;
        default:
            printf("element type does not exist\n");
            return;
    }
}

bool validRect(Rectangle* rect){
    if(rect->width < 0 || rect->height < 0) return 0;
    if(rect->otherAttributes == NULL) return 0;
    return 1;
}
bool validCir(Circle* cir){
    if(cir->r < 0) return 0;
    if(cir->otherAttributes == NULL) return 0;
    return 1;
}
bool validPath(Path* path){
    if(path->data == NULL) return 0;
    if(path->otherAttributes == NULL) return 0;
    return 1;
}

void changeSVG(SVGimage* image, int elemIndex, Attribute* newAttribute){
    
    if(image == NULL){
        if(newAttribute == NULL) return;
        free(newAttribute->name);
        free(newAttribute->value);
        free(newAttribute);
        return;
    }

    if(strcmp(newAttribute->name, "title") == 0 ){
        strncpy(image->title, newAttribute->value, 256);
    }else if(strcmp(newAttribute->name, "desc") == 0 ){
        strncpy(image->description, newAttribute->value, 256);
    }else if(!checkAttrSVG(image, newAttribute)){
        Attribute* tempAttr = calloc(1, sizeof(Attribute));
        tempAttr->name = parseData(newAttribute->name);
        tempAttr->value = parseData(newAttribute->value);
        insertBack(image->otherAttributes, tempAttr);
    }

    free(newAttribute->name);
    free(newAttribute->value);
    free(newAttribute);

}

bool checkAttrSVG(SVGimage* image, Attribute* newAttribute){
    Node* cur_node;
    char* name, *value;
    int check = 0;
    for(cur_node = image->otherAttributes->head; cur_node; cur_node = cur_node->next){
        if(strcmp(((Attribute*)(cur_node->data))->name, newAttribute->name) == 0){
            free(((Attribute*)(cur_node->data))->name);
            free(((Attribute*)(cur_node->data))->value);
            name = parseData(newAttribute->name);
            value = parseData(newAttribute->value);
            (((Attribute*)(cur_node->data))->name) = name;
            (((Attribute*)(cur_node->data))->value) = value;
            check = 1;
            break;
        }
    }

    return check;
}

void changeRect(SVGimage* image, int elemIndex, Attribute* newAttribute){
    Node* cur_node;
    int i;
    float temp;

    if(getLength(image->rectangles) == 0){
        printf("There are no Rectangles!\n");
        return;
    } 

    if(getLength(image->rectangles) <= elemIndex){
        printf("INDEX OUT OF BOUNDS");
        return;
    }

    for(cur_node = image->rectangles->head, i = 0; i < elemIndex ; cur_node = cur_node->next, i ++);
    
    if(strcmp(newAttribute->name, "x") == 0){
        temp = parseInt(newAttribute->value);
        ((Rectangle*)(cur_node->data))->x = temp;
    }else if(strcmp(newAttribute->name, "y") == 0){
        temp = parseInt(newAttribute->value);
        ((Rectangle*)(cur_node->data))->y = temp;
    }else if(strcmp(newAttribute->name, "width") == 0){
        temp = parseInt(newAttribute->value);
        if(temp < 0) temp = 0;
        ((Rectangle*)(cur_node->data))->width = temp;
    }else if(strcmp(newAttribute->name, "height") == 0){
        temp = parseInt(newAttribute->value);
        if(temp < 0) temp = 0;
        ((Rectangle*)(cur_node->data))->height = temp;
    }else if(!checkRect((Rectangle*)(cur_node)->data, newAttribute)){
        Attribute* tempAttr = calloc(1, sizeof(Attribute));
        tempAttr->name = parseData(newAttribute->name);
        tempAttr->value = parseData(newAttribute->value);
        insertBack(((Rectangle*)(cur_node->data))->otherAttributes, tempAttr);
    }
    // free(newAttribute->name);
    // free(newAttribute->value);
    // free(newAttribute);
}

bool checkRect(Rectangle* rect, Attribute* newAttribute){
    
    Node* cur_node;
    char* name, *value;
    int check = 0;
    for(cur_node = rect->otherAttributes->head; cur_node; cur_node = cur_node->next){
        if(strcmp(((Attribute*)(cur_node->data))->name, newAttribute->name) == 0){
            free(((Attribute*)(cur_node->data))->name);
            free(((Attribute*)(cur_node->data))->value);
            name = parseData(newAttribute->name);
            value = parseData(newAttribute->value);
            (((Attribute*)(cur_node->data))->name) = name;
            (((Attribute*)(cur_node->data))->value) = value;
            check = 1;
            break;
        }
    }

    return check;
}

void changeCir(SVGimage* image, int elemIndex, Attribute* newAttribute){
    Node* cur_node;
    int i;
    float temp;


    printf("elemIndex = %d\n", elemIndex);
    printf("Name = %s\n", newAttribute->name);
    printf("Val = %s\n", newAttribute->value);

    if(getLength(image->circles) == 0){
        printf("There are no Circles!\n");
        return;
    } 

    if(getLength(image->circles) <= elemIndex){
        printf("INDEX OUT OF BOUNDS");
        return;
    }

    for(cur_node = image->circles->head, i = 0; i < elemIndex ; cur_node = cur_node->next, i ++);
    
    if(strcmp(newAttribute->name, "cx") == 0){
        temp = parseInt(newAttribute->value);
        ((Circle*)(cur_node->data))->cx = temp;
    }else if(strcmp(newAttribute->name, "cy") == 0){
        temp = parseInt(newAttribute->value);
        ((Circle*)(cur_node->data))->cy = temp;
    }else if(strcmp(newAttribute->name, "r") == 0){
        temp = parseInt(newAttribute->value);
        if(temp < 0) temp = 0;
        ((Circle*)(cur_node->data))->r = temp;
        // char* svgString = SVGimageToString(image);
        // printf("\n%s\n", svgString);
        // free(svgString);
        // printf("[%s] = [%s]\n", newAttribute->name ,newAttribute->value);
        // printf("[%s] = [%f]\n", "r" ,((Circle*)(cur_node->data))->r);

    }else if(!checkCir((Circle*)(cur_node)->data, newAttribute)){
        Attribute* tempAttr = calloc(1, sizeof(Attribute));
        tempAttr->name = parseData(newAttribute->name);
        tempAttr->value = parseData(newAttribute->value);
        insertBack(((Circle*)(cur_node->data))->otherAttributes, tempAttr);
    }

    // free(newAttribute->name);
    // free(newAttribute->value);
    // free(newAttribute);

    

}

bool checkCir(Circle* cir, Attribute* newAttribute){
    Node* cur_node;
    char* name, *value;
    int check = 0;
    for(cur_node = cir->otherAttributes->head; cur_node; cur_node = cur_node->next){
        if(strcmp(((Attribute*)(cur_node->data))->name, newAttribute->name) == 0){
            free(((Attribute*)(cur_node->data))->name);
            free(((Attribute*)(cur_node->data))->value);
            name = parseData(newAttribute->name);
            value = parseData(newAttribute->value);
            (((Attribute*)(cur_node->data))->name) = name;
            (((Attribute*)(cur_node->data))->value) = value;
            check = 1;
            break;
        }
    }

    return check;
}

void changePath(SVGimage* image, int elemIndex, Attribute* newAttribute){
    Node* cur_node;
    char* value;
    int i;

    if(getLength(image->paths) == 0){
        printf("There are no Paths!\n");
        return;
    } 

    if(getLength(image->paths) <= elemIndex){
        printf("INDEX OUT OF BOUNDS");
        return;
    }

    for(cur_node = image->paths->head, i = 0; i < elemIndex ; cur_node = cur_node->next, i ++);
    
    if(strcmp(newAttribute->name, "d") == 0){
        free(((Path*)(cur_node->data))->data);
        value = parseData(newAttribute->value);
        ((Path*)(cur_node->data))->data = value;
        printf("[%s] = [%s]\n", newAttribute->name ,newAttribute->value);
        // int n = strlen(((Path*)(cur_node->data))->data);
        // strncpy(((Path*)(cur_node->data))->data, newAttribute->value, n);
    }else if(!checkPath((Path*)(cur_node)->data, newAttribute)){
        Attribute* tempAttr = calloc(1, sizeof(Attribute));
        tempAttr->name = parseData(newAttribute->name);
        tempAttr->value = parseData(newAttribute->value);
        insertBack(((Path*)(cur_node->data))->otherAttributes, tempAttr);
    }
    // free(newAttribute->name);
    // free(newAttribute->value);
    // free(newAttribute);
}

bool checkPath(Path* path, Attribute* newAttribute){
    Node* cur_node;
    char* name, *value;
    int check = 0;
    for(cur_node = path->otherAttributes->head; cur_node; cur_node = cur_node->next){
        if(strcmp(((Attribute*)(cur_node->data))->name, newAttribute->name) == 0){
            free(((Attribute*)(cur_node->data))->name);
            free(((Attribute*)(cur_node->data))->value);
            name = parseData(newAttribute->name);
            value = parseData(newAttribute->value);
            (((Attribute*)(cur_node->data))->name) = name;
            (((Attribute*)(cur_node->data))->value) = value;
            check = 1;
            break;
        }
    }

    return check;
}

void changeGroup(SVGimage* image, int elemIndex, Attribute* newAttribute){
    Node* cur_node;
    int i;

    if(getLength(image->groups) == 0){
        printf("There are no Groups!\n");
        return;
    } 

    if(getLength(image->groups) <= elemIndex){
        printf("INDEX OUT OF BOUNDS");
        return;
    }

    for(cur_node = image->groups->head, i = 0; i < elemIndex ; cur_node = cur_node->next, i ++);
    
    if(!checkGroup((Group*)(cur_node)->data, newAttribute)){
        Attribute* tempAttr = calloc(1, sizeof(Attribute));
        tempAttr->name = parseData(newAttribute->name);
        tempAttr->value = parseData(newAttribute->value);
        insertBack(((Group*)(cur_node->data))->otherAttributes, tempAttr);
    }
    // free(newAttribute->name);
    // free(newAttribute->value);
    // free(newAttribute);
}

bool checkGroup(Group* group, Attribute* newAttribute){
    Node* cur_node;
    char* name, *value;
    int check = 0;
    for(cur_node = group->otherAttributes->head; cur_node; cur_node = cur_node->next){
        if(strcmp(((Attribute*)(cur_node->data))->name, newAttribute->name) == 0){
            free(((Attribute*)(cur_node->data))->name);
            free(((Attribute*)(cur_node->data))->value);
            name = parseData(newAttribute->name);
            value = parseData(newAttribute->value);
            (((Attribute*)(cur_node->data))->name) = name;
            (((Attribute*)(cur_node->data))->value) = value;
            check = 1;
            break;
        }
    }

    return check;
}


/* Module 3 */

int getNumIntegers(int in);
int getGroupChildren(Group *g);

int getNumIntegers(int in){
    int i;
    for(i = 0; in != 0 ;i ++){
        in /= 10;
    }
    
    return i;
}

char* attrToJSON(const Attribute *a){
    if(a == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "{}\0");
        return temp;
    } 

    char* name = parseData(a->name);
    char* val = parseData(a->value);
    int buffer = 30 + strlen(name) + strlen(val);

    char* temp = calloc(buffer, sizeof(char));
    snprintf(temp, buffer, "{\"name\":\"%s\",\"value\":\"%s\"}",name,val);

    free(name);
    free(val);

    return temp;
}

char* circleToJSON(const Circle *c){
    if(c == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "{}\0");
        return temp;
    }

    Circle* cir = (Circle*)c;
    char* units = cir->units;

    int buffer = 50 + strlen(c->units) + getNumIntegers((int)c->cx) + 3 + getNumIntegers((int)c->cy) + 3 + getNumIntegers((int)c->r) + 3 + getLength(c->otherAttributes);
    char* temp = calloc(buffer, sizeof(char));

    snprintf(temp, buffer, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}",c->cx, c->cy, c->r, getLength(c->otherAttributes), units);

    return temp;
}

char* rectToJSON(const Rectangle *r){
    if(r == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "{}\0");
        return temp;
    }

    int buffer = 50 + strlen(r->units) + (sizeof(float)*4) + sizeof(int);
    char* temp = calloc(buffer, sizeof(char));
    snprintf(temp, buffer, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}",r->x, r->y, r->width, r->height, getLength(r->otherAttributes), r->units);

    return temp;
}

char* pathToJSON(const Path *p){
    if(p == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "{}\0");
        return temp;
    }

    int data = strlen(p->data);
    if(data > 64) data = 64;
    int buffer = 50 + strlen(p->data);

    char* trunc = malloc(64+1);
    trunc = strncpy(trunc, p->data, 64);
    trunc[64] = '\0';

    char* temp = calloc(buffer, sizeof(char));
    snprintf(temp, buffer, "{\"d\":\"%s\",\"numAttr\":%d}", trunc, getLength(p->otherAttributes));

    free(trunc);
    return temp;
}

int getGroupChildren(Group *g){
     int temp = 0;

    temp += getLength(g->rectangles);
    temp += getLength(g->circles);
    temp += getLength(g->paths);
    temp += getLength(g->groups);

    return temp;
}

char* groupToJSON(const Group *g){
    if(g == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "{}\0");
        return temp;
    }

    Group* tempG = (Group*)g;

    int size = getGroupChildren(tempG);

    int buffer = 50 + getNumIntegers(size);

    char* temp = calloc(buffer, sizeof(char));
    snprintf(temp, buffer, "{\"children\":%d,\"numAttr\":%d}", size, getLength(g->otherAttributes));

    return temp;
}

char* attrListToJSON(const List *list){
    if(list == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "[]\0");
        return temp;
    }

    Node* cur_node;
    char* out, *temp;
    int buffer = 50;
    out = calloc(buffer, sizeof(char));
    strcat(out,"[");
    for(cur_node = list->head; cur_node; cur_node = cur_node->next){
        temp = attrToJSON((Attribute*)(cur_node->data));
        buffer += strlen(temp);
        out = realloc(out, buffer);
        strcat(out,temp);
        if(cur_node->next != NULL) strcat(out,",");
        free(temp);
    }
    strcat(out,"]");

    return out;
}

char* circListToJSON(const List *list){
    if(list == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "[]\0");
        return temp;
    }

    Node* cur_node;
    char* out, *temp;
    int buffer = 50;
    out = calloc(buffer, sizeof(char));
    strcat(out,"[");

    for(cur_node = list->head; cur_node; cur_node = cur_node->next){
        temp = circleToJSON((Circle*)(cur_node->data));
        buffer += strlen(temp);
        out = realloc(out, buffer);
        strcat(out,temp);
        if(cur_node->next != NULL) strcat(out,",");
        free(temp);
    }
    strcat(out,"]");

    return out;

}

char* rectListToJSON(const List *list){
    if(list == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "[]\0");
        return temp;
    }

    Node* cur_node;
    char* out, *temp;
    int buffer = 50;
    out = calloc(buffer, sizeof(char));
    strcat(out,"[");

    for(cur_node = list->head; cur_node; cur_node = cur_node->next){
        temp = rectToJSON((Rectangle*)(cur_node->data));
        buffer += strlen(temp);
        out = realloc(out, buffer);
        strcat(out,temp);
        if(cur_node->next != NULL) strcat(out,",");
        free(temp);
    }
    strcat(out,"]");

    return out;

}

char* pathListToJSON(const List *list){
    if(list == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "[]\0");
        return temp;
    }

    Node* cur_node;
    char* out, *temp;
    int buffer = 50;
    out = calloc(buffer, sizeof(char));
    strcat(out,"[");

    for(cur_node = list->head; cur_node; cur_node = cur_node->next){
        temp = pathToJSON((Path*)(cur_node->data));
        buffer += strlen(temp);
        out = realloc(out, buffer);
        strcat(out,temp);
        if(cur_node->next != NULL) strcat(out,",");
        free(temp);
    }
    strcat(out,"]");

    return out;
}

char* groupListToJSON(const List *list){
    if(list == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "[]\0");
        return temp;
    }

    Node* cur_node;
    char* out, *temp;
    int buffer = 50;
    out = calloc(buffer, sizeof(char));
    strcat(out,"[");

    for(cur_node = list->head; cur_node; cur_node = cur_node->next){
        temp = groupToJSON((Group*)(cur_node->data));
        buffer += strlen(temp);
        out = realloc(out, buffer);
        strcat(out,temp);
        if(cur_node->next != NULL) strcat(out,",");
        free(temp);
    }
    strcat(out,"]");

    return out;
}

char* SVGtoJSON(const SVGimage* imge){
    if (imge == NULL){
        char *temp = malloc(3);
        temp = strcpy(temp, "{}\0");
        return temp;
    }

    List* rects = getRects((SVGimage*)imge);
    List* circs = getCircles((SVGimage*)imge);
    List* paths = getPaths((SVGimage*)imge);
    List* groups = getGroups((SVGimage*)imge);

    int numRects = getLength(rects);
    int numCircs = getLength(circs);
    int numPaths = getLength(paths);
    int numGroups = getLength(groups);

    int buffer = 50 + getNumIntegers(numRects) + 3 + getNumIntegers(numCircs) + 3 + getNumIntegers(numPaths) + 3 + getNumIntegers(numGroups) + 3;

    char* temp = calloc(buffer, sizeof(char));
    snprintf(temp, buffer, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}",numRects, numCircs, numPaths, numGroups);

    freeShallowList(rects->head);
    free(rects);
    freeShallowList(circs->head);
    free(circs);
    freeShallowList(paths->head);
    free(paths);
    freeShallowList(groups->head);
    free(groups);

    printf("%s\n",temp);
    return temp;
}

/* MODULE 3 BONUS */

char* getStrInQuote(int index, char* svgString);
char* getIntAfterColon(int index, char* svgString);

char* getStrInQuote(int index, char* svgString){
    int j, t, i;
    index += 2;
    for(j = index; svgString[j] != '\"' && j < strlen(svgString); j++);

    char* temp = malloc(j-index+1);
    for(t = 0, i = index; i < j; i ++, t++){
        temp[t] = svgString[i];
    }
    temp[t] = '\0';

    return temp;
}

char* getIntAfterColon(int index, char* svgString){
    int j, t, i;
    index += 1;
    for(j = index; svgString[j] != ',' && j < strlen(svgString); j++);

    char* temp = malloc(j-index+1);
    for(t = 0, i = index; i < j; i ++, t++){
        temp[t] = svgString[i];
    }
    temp[t] = '\0';

    return temp;
}

SVGimage* JSONtoSVG(const char* svgString){
    if(svgString == NULL) return NULL;

    SVGimage* image = malloc(sizeof(SVGimage));
    initLists(image);
    int i;
    for(i = 0; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* title = getStrInQuote(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* desc = getStrInQuote(i, (char*)svgString);
    printf("%s\n", title);
    printf("%s\n", desc);

    strcpy(image->title, title);
    strcpy(image->description, desc); 
    strcpy(image->namespace, "http://www.w3.org/2000/svg");

    free(title);
    free(desc);

    return image;
}

Rectangle* JSONtoRect(const char* svgString){
    if(svgString == NULL) return NULL;

    Rectangle* rect = malloc(sizeof(Rectangle));
    rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    int i;
    for(i = 0; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* strx = getIntAfterColon(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* stry = getIntAfterColon(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* strwidth = getIntAfterColon(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* strheight = getIntAfterColon(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* units = getStrInQuote(i, (char*)svgString);
    
    float x = parseInt(strx);
    float y = parseInt(stry);
    float width = parseInt(strwidth);
    float height = parseInt(strheight);

    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    strcpy(rect->units, units);

    free(strx);
    free(stry);
    free(strwidth);
    free(strheight);
    free(units);

    return rect;
}

Circle* JSONtoCircle(const char* svgString){
    if(svgString == NULL) return NULL;

    Circle* cir = malloc(sizeof(Circle));
    cir->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    int i;
    for(i = 0; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* strcx = getIntAfterColon(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* strcy = getIntAfterColon(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* strr = getIntAfterColon(i, (char*)svgString);
    for(i = i+1; svgString[i] != ':' && i < strlen(svgString); i ++);
    char* units = getStrInQuote(i, (char*)svgString);
    
    float cx = parseInt(strcx);
    float cy = parseInt(strcy);
    float r = parseInt(strr);

    cir->cx = cx;
    cir->cy = cy;
    cir->r = r;
    strcpy(cir->units, units);

    free(strcx);
    free(strcy);
    free(strr);
    free(units);

    return cir;
}

char *interpretFileCall(char *fileName, int functionCall);
int getFileSize(char* fileName);
char *getTitle(char *fileName);
char *getDesc(char *fileName);
char *getAttr(char *fileName, char *type, int index);
int changeAttr(char *fileName, char *type, int index, char *name, char *value);

/* A3 FUNCTIONS */
char *interpretFileCall(char *fileName, int functionCall){
    SVGimage *img = createSVGimage(fileName);
    char* temp;
    switch(functionCall){
        case 1: //SVGtoJson
            temp = SVGtoJSON(img);
            deleteSVGimage(img);
            return temp;
            break;
        case 2: //circListToJSON
            temp = circListToJSON(img->circles);
            deleteSVGimage(img);
            return temp;
            break;
        case 3: //rectListToJSON
            temp = rectListToJSON(img->rectangles);
            deleteSVGimage(img);
            return temp;
            break;
        case 4: //pathListToJSON
            temp = pathListToJSON(img->paths);
            deleteSVGimage(img);
            return temp;
            break;
        case 5: //groupListToJSON
            temp = groupListToJSON(img->groups);
            deleteSVGimage(img);
            return temp;
            break;

        default:
            return NULL;
    }

}

int getFileSize(char* fileName){
    FILE *fp = fopen(fileName, "r");
    fseek(fp,0,SEEK_END);
    float size = ftell(fp);
    fclose(fp);

    size /= 1024;
    size = round(size);    

    return size;
}

char *getTitle(char *fileName){
    SVGimage *img = createSVGimage(fileName);
    char *temp = malloc(strlen(img->title) + 1);
    strcpy(temp,img->title);
    deleteSVGimage(img);
    return temp;
}

char *getDesc(char *fileName){
    SVGimage *img = createSVGimage(fileName);
    char *temp = malloc(strlen(img->description) + 1);
    strcpy(temp,img->description);
    deleteSVGimage(img);
    return temp;
}

char *getAttr(char *fileName, char *type, int index){
    SVGimage *img = createSVGimage(fileName);
    char* temp;
    int i;
    Node* cur_node;
    if(strcmp(type,"Rectangle") == 0){
        for(cur_node = img->rectangles->head, i = 0; i < index; cur_node = cur_node->next ,i++ );    
        temp = attrListToJSON(((Rectangle*)(cur_node->data))->otherAttributes);
        deleteSVGimage(img);
        return temp;
    }else if(strcmp(type,"Circle") == 0){
        for(cur_node = img->circles->head, i = 0; i < index; cur_node = cur_node->next ,i++ );    
        temp = attrListToJSON(((Circle*)(cur_node->data))->otherAttributes);
        deleteSVGimage(img);
        return temp;
    }else if(strcmp(type,"Path") == 0){
        for(cur_node = img->paths->head, i = 0; i < index; cur_node = cur_node->next ,i++ );    
        temp = attrListToJSON(((Path*)(cur_node->data))->otherAttributes);
        deleteSVGimage(img);
        return temp;
    }else if(strcmp(type,"Group") == 0){
        for(cur_node = img->groups->head, i = 0; i < index; cur_node = cur_node->next ,i++ );    
        temp = attrListToJSON(((Group*)(cur_node->data))->otherAttributes);
        deleteSVGimage(img);
        return temp;
    }else return NULL;
}

int changeAttr(char *fileName, char *type, int index, char *name, char *value){
    SVGimage *img = createSVGimage(fileName);
    Attribute* attr = malloc(sizeof(Attribute));
    attr->name = name;
    attr->value = value;

    if(strcmp(type,"Rectangle") == 0){
        setAttribute(img, RECT, index, attr);
    }else if(strcmp(type,"Circle") == 0){
        setAttribute(img, CIRC, index, attr);
    }else if(strcmp(type,"Path") == 0){
        setAttribute(img, PATH, index, attr);
    }else if(strcmp(type,"Group") == 0){  
        setAttribute(img, GROUP, index, attr);
    }

    int ret = writeSVGimage(img, fileName);
    deleteSVGimage(img);
    return ret;
}

void setDescJs(char* filename, char* desc){
    SVGimage* img = createSVGimage(filename);

    strncpy(img->description, desc, 255);

    writeSVGimage(img, filename);
}

void setTitleJs(char* filename, char* desc){
    SVGimage* img = createSVGimage(filename);

    strncpy(img->title, desc, 255);

    writeSVGimage(img, filename);
}

int addCompJs(char* fileName, char* type, char* name, char* value, int len){
    printf("%s\n", fileName);
    SVGimage* img = createSVGimage(fileName);

    printf("%s\n", SVGimageToString(img));


    char **nameList = malloc(len * sizeof(char*));
    char **valList = malloc(len * sizeof(char*));
    char* token;
    char* temp;
    int iter = 0;
    token = strtok(name, " ");
    while(token != NULL){
        temp = malloc(strlen(token) + 1);
        strcpy(temp, token);
        nameList[iter] = temp;

        token = strtok(NULL, " ");
        iter ++;
    }    

    iter = 0;
    token = strtok(value, " ");
    while(token != NULL){
        temp = malloc(strlen(token) + 1);
        strcpy(temp, token);
        valList[iter] = temp;

        token = strtok(NULL, " ");
        iter ++;
    }    
    int i;

    for(i = 0; i < len; i ++){
        printf("[%s] [%s]\n", nameList[i], valList[i]);
    }

    

    if(strcmp("Rectangle", type) == 0){
        Rectangle* rect = malloc(sizeof(Rectangle));
        rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
        for(i = 0; i < len; i ++){
            if (strcasecmp(nameList[i], "x") == 0) {
                rect->x = parseInt(valList[i]);
            }else if (strcasecmp(nameList[i], "y") == 0) {
                rect->y = parseInt(valList[i]);
            }else if (strcasecmp(nameList[i], "width") == 0) {
                rect->width = parseInt(valList[i]);
            }else if (strcasecmp(nameList[i], "height") == 0) {
                rect->height = parseInt(valList[i]);
            }else{
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr(nameList[i]);
                tempAttr->value = getStr(valList[i]);
                insertBack(rect->otherAttributes, tempAttr);
            }
        }
        addComponent(img, RECT, rect);
    }else if(strcmp("Circle", type) == 0){
        Circle* cir = malloc(sizeof(Circle));
        cir->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
        for(i = 0; i < len; i ++){
            if (strcasecmp(nameList[i], "cx") == 0) {
                cir->cx = parseInt(valList[i]);
            }else if (strcasecmp(nameList[i], "cy") == 0) {
                cir->cy = parseInt(valList[i]);
            }else if (strcasecmp(nameList[i], "r") == 0) {
                cir->r = parseInt(valList[i]);
            }else{
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr(nameList[i]);
                tempAttr->value = getStr(valList[i]);
                insertBack(cir->otherAttributes, tempAttr);
            }
        }
        addComponent(img, CIRC, cir);
    }else if(strcmp("Path", type) == 0){
        Path* path = malloc(sizeof(Path));
        path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
        for(i = 0; i < len; i ++){
            if (strcasecmp(nameList[i], "d") == 0) {
                path->data = valList[i];
            }else{
                Attribute* tempAttr = calloc(1, sizeof(Attribute));
                tempAttr->name = getStr(nameList[i]);
                tempAttr->value = getStr(valList[i]);
                insertBack(path->otherAttributes, tempAttr);
            }
        }
        addComponent(img, PATH, path);
    }

    int ret;
    ret = validateSVGimage(img,"svg.xsd");
    if(ret) writeSVGimage(img, fileName);
    return ret;
}