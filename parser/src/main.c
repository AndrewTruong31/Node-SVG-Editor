#include "SVGParser.h"


#ifdef LIBXML_TREE_ENABLED

int main(int argc, char **argv)
{
    if (argc != 2)
        return(1);
    
    SVGimage* image = createValidSVGimage(argv[1], "svg.xsd");
    // validateSVGimage(image, "svg.xsd");

    // Circle* compTemp = malloc(sizeof(Circle));
    // compTemp->cx = 42;
    // compTemp->cy = 42;
    // compTemp->r = 42;
    // strcpy(compTemp->units, "cm");
    // compTemp->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    // addComponent(image, 2, compTemp);

    Attribute* temp = malloc(sizeof(Attribute));
    temp->name = malloc(10);
    strncpy(temp->name, "r", 10);
    temp->value = malloc(10);
    strncpy(temp->value, "7", 10);

    setAttribute(image, PATH, 0, temp);
    
    // writeSVGimage(image, "test.svg");

    // SVGimage* image2 = createValidSVGimage("test.svg", "svg.xsd");
    // writeSVGimage(image2, "test2.svg");

    // validateSVGimage(image2, "svg.xsd");

    // char* svgString = SVGimageToString(image);
    // printf("\n%s\n", svgString);
    // free(svgString);

    char *svgString = SVGimageToString(image);
    printf("\n%s\n", svgString);
    free(svgString);


    // deleteSVGimage(image2);

    // validateSVGimage(image, "svg.xsd");

    // printf("SVGImage has %d attrs\n", getLength(image->otherAttributes));


    // List* rects = getRects(image);
    // char* rectDesc = toString(rects);
    // printf("GET RECTS: %s\n", rectDesc);
    // free(rectDesc);

    // List* circles = getCircles(image);
    // char* circleDesc = toString(circles);
    // printf("GET CIRCLES: %s\n", circleDesc);
    // free(circleDesc);

    // List* paths = getPaths(image);
    // char* pathDesc = toString(paths);
    // printf("GET PATHS: %s\n", pathDesc);
    // free(pathDesc);

    // List* groups = getGroups(image);
    // char* groupDesc = toString(groups);
    // printf("GET GROUPS: %s\n", groupDesc);
    // free(groupDesc);

    // printf("There are %d rectangles with area 20\n", numRectsWithArea(image, 20));
    // printf("There are %d circles with area 315\n", numCirclesWithArea(image, 315));
    // printf("There are %d paths with d ...\n", numPathsWithdata(image, "M200,300 L400,50 L600,300             L800,550 L1000,300"));
    // printf("There are %d groups with len 2\n", numGroupsWithLen(image, 2));
    // printf("There are %d other attributes in the SVG file\n", numAttr(image));

    // freeShallowList(rects->head);
    // free(rects);
    // freeShallowList(circles->head);
    // free(circles);
    // freeShallowList(paths->head);
    // free(paths);
    // freeShallowList(groups->head);
    // free(groups);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    
    
    deleteSVGimage(image);
    return 0;
}
#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif