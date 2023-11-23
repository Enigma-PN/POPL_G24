// #include "image.h"
// #include<string.h>

// int main(int argc, char** argv)
// {
//     Image test("tests/eldenring.jpeg");
//     Image test1("tests/eldenring.jpeg");
//     // test.write("new.png");
//     // Image copy = test;
//     // for(int i = 0; i < copy.w*copy.channels;++i)
//     // // for(int i = 0; i < 14935000;++i)
//     // {
//     //     copy.data[i] = 140;
//     // }
//     // copy.write("copy.png");
//     // Image blank(100, 100, 3);
//     // blank.write("blank.jpg");

//     test1.grayscale_lum();
//     test1.write("gray_avg.png");
//     // Image& gray_lum = test.grayscale_lum();
//     // gray_lum.write("gray_lum.png");
//     // Image& colormask = test.colorMask(1, 1, 1.05);
//     // colormask.write("blue.png");
//     // test.write("a.png");
//     // test.colorSwapRG();
//     // test.write("swap.png"); // SWap intensities of red and green
//     // test.remRed();
//     // test.write("output/remRed.png");

//     char buffer[1000] = {0}; // Use malloc for dynamic allocation
//     size_t len = 0;
//     char message[1000] = {0};
//     char temp[1000] ={0}; 
//     FILE* message_file = fopen("message.txt", "rw");
//     if(message_file == NULL) {
//       perror("Error opening file");
//       return(-1);
//     }
//     while((fgets (temp, 1000, message_file)!=NULL)) 
//     {
//         strcat(message, temp);
//     }
//     test.encodeMessage(message);
//     test.decodeMessage(buffer, &len);
//     test.write("stegano.png");
//     printf("Message: \n%s \nNumber of bytes : %zu\n", buffer, len);
//     fclose(message_file);
//     return 0;
// }

#include "image.h"
#include<string.h>

int main(int argc, char** argv)
{
    Image test(argv[1]);
    // Image test1("tests/eldenring.jpeg");
    // test.write("new.png");
    // Image copy = test;
    // for(int i = 0; i < copy.w*copy.channels;++i)
    // // for(int i = 0; i < 14935000;++i)
    // {
    //     copy.data[i] = 140;
    // }
    // copy.write("copy.png");
    // Image blank(100, 100, 3);
    // blank.write("blank.jpg");

    // test1.grayscale_lum();
    // test1.write("gray_avg.png");
    // Image& gray_lum = test.grayscale_lum();
    // gray_lum.write("gray_lum.png");
    // Image& colormask = test.colorMask(1, 1, 1.05);
    // colormask.write("blue.png");
    // test.write("a.png");
    // test.colorSwapRG();
    // test.write("swap.png"); // SWap intensities of red and green
    // test.remRed();
    // test.write("output/remRed.png");

    char buffer[1000] = {0}; // Use malloc for dynamic allocation
    size_t len = 0;
    char message[1000] = {0};
    char temp[1000] ={0}; 
    FILE* message_file = fopen("message.txt", "rw");
    if(message_file == NULL) {
      perror("Error opening file");
      return(-1);
    }
    while((fgets (temp, 1000, message_file)!=NULL)) 
    {
        strcat(message, temp);
    }
    test.encodeMessage(message);
    test.decodeMessage(buffer, &len);
    test.write("output.png");
    printf("Message: \n%s \nNumber of bytes : %zu\n", buffer, len);
    fclose(message_file);
    return 0;
}