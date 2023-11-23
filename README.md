# POPL_G24

## Instructions to run the CPP implementation
Store the carrier image in the tests directory.
Navigate into the folder CPP_Implementation and run the following commands to compile and run the code: 

```sh
❯ g++ -std=c++17 -g -c -o build/main.o src/main.cpp
❯ g++ -std=c++17 -g -c -o build/image.o src/image.cpp
❯ g++ -std=c++17 -o LSB_Steganography build/*.o
❯ ./LSB_Steganography tests/<insert_filename>
```

## Instructions to run the Rust implementation
Store the carrier image as "image.png" in the "rust" directory.
Navigate into the folder Rust_Implementation/rust and run the following command: 

```sh
❯ cargo run
```
The output is stored in the results directory.

NOTE: To decrypt an already encoded image, use the encoded image as the input image and leave the message.txt file blank.

