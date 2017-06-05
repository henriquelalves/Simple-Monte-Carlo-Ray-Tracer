#include "geometries.h"
#include "scene.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
    // Default
    char title[128] = "POTATO";
    int drunk_mode = false;
    int width = 600;
    int height = 600;

    //-------------------------------------------------------
    // Reading from setup.txt
    FILE * file = fopen("setup/setup.txt", "r");

    if( file == NULL ){
        printf("No setup.txt file, using default values!\n");
    } else {
        printf("setup.txt file found!\n");
        while (true){
            // Read line
            char lineHeader[128];
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF){
                break;
            }

            // Parse line
            if ( strcmp(lineHeader, "TITLE:") == 0){
                fscanf(file, "%s\n", title);
            } else if (strcmp(lineHeader, "WIDTH:") == 0){
                fscanf(file, "%d\n", &width);
                printf("%d\n",width);
            } else if (strcmp(lineHeader, "HEIGHT:") == 0){
                fscanf(file, "%d\n", &height);
            } else if (strcmp(lineHeader, "DRUNK_MODE:") == 0) {
                fscanf(file, "%d\n", &drunk_mode);
                if(drunk_mode > 0)
                    drunk_mode = 1;
                else
                    drunk_mode = 0;
            } else {
                char stupidBuffer[1000];
                fgets(stupidBuffer, 1000, file);
            }
        }
    }

    //-------------------------------------------------------

    // create new image
    bitmap_image image(width, height);
    
    Scene s = Scene(width, height);

    // Add objects here

    // Render scene
    s.render_scene(&image, 0, drunk_mode);

    // save to file

    image.save_image(title);
	
    // application successfully returned
    return 0;
}
