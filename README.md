**Console image editor coded in C:**  
  This is an image editor coded in C that can fulfill certain operations
on an image (crop, rotate, save, load) and apply certain filters(blur, sharpen,
gaussian blur, edge detection).  
  The program is implemented using dynamic allocation (which we all hate) and
only works on ppm and pgm formats.

**The code itself**  
  The code is split among multiple files which are used to separate functions
and structures by functionality, hence you're gonna find all the image related
functions into the image_editor.c & .h file, utility functions that can be used
in different programs/projects, the effects.c & .h files which hold the kernel
matrices, allowing for easy modifications. The worker.c & .h is the main
implementation of the program itself, taking care of reading, calling and
parsing commands. The img.c & .h holds all the image reltated functions and
structures.

**The img.c & .h files:**  
  The main img structure is made out of 4 key fields: type, width, height
and max value. This data is used to manouver around certain functions.  
  We also have a struct select field. That structure holds 4 values that 
represent 2 points that determine a selection within the image.  
  There are 2 pointers that hold the image's pixel data, mono_chr for
monochrome images and rgb for rgb images. When one is in use the other is
NULL.  
    ```c
    //The image structure looks like this:
    struct img {
        __UINT_FAST32_TYPE__ type;
        __UINT_FAST32_TYPE__ width, height;
        __UINT_FAST32_TYPE__ max_val;
        double **mono_chr;
        struct select sel;
        struct rgb **rgb;
    };
    //The select structure looks like this:
    struct select {
	__UINT_FAST32_TYPE__ x1, x2, y1, y2;
    };
    //The rgb structure looks like this:
    struct rgb {
	double r, g, b;
    };
    ```
    
  The majority of the functions are explained using comments inside the code,
so I am going to explain only the more complex.  
  The crop function is using an auxiliary matrix and crops the image based on
the data inside the select data inside the image structure. It copies the
pixels inside the selection into the auxiliary matrix, deletes the old image
puts the new matrix there.  
  The rotate functions ( rotate_all() and rotate_sel() ) work similarly, the
differences being subtle but big. They both use an auxiliary matrix as a
middle guy during the rotation. We move by collumns (starting from 0 to m) and
lines (starting from n-1 to 0) and we're placing them into the aux matrix.
After the pass is done (we're doing maximum 3 passes, decided by the angle) we
place the rotated matrix back into the image structure.  
  The effect() function takes image matrix and applies the kernel to it. By
taking the current pixel and it's neighbours, multiplying them wich each
kernel counterpart (same position in matrix) and adding them to a sum. That
sum is clamped and placed into an auxiliary matrix (in order to not screw
the callculations up).  


**The effects.c & .h files:**  
  They hold the kernels, that look like this:
        ```c
        //Kernel matrix for edge detection filter
        const double edge_matrix[3][3] = {
                {-1, -1, -1},
                {-1, 8, -1},
                {-1, -1, -1}};
        //Kernel matrix for sharpen filter
        const double sharp_matrix[3][3] = {
                {0, -1, 0},
                {-1, 5, -1},
                {0, -1, 0}};
        //Kernel matrix for blur filter
        const double bblur_matrix[3][3] = {
                {0.1111, 0.1111, 0.1111},
                {0.1111, 0.1111, 0.1111},
                {0.1111, 0.1111, 0.1111}};
        //Kernel matrix for Gaussian blur filter
        const double gblur_matrix[3][3] = {
                {0.0625, 0.125, 0.0625},
                {0.125, 0.25, 0.125},
                {0.0625, 0.125, 0.0625}};
	```

**The macros.h file:**  
  Holds macros that represent error codes, messages and values
Peek inside:
    ```c
    #define ZERO '0'
    #define UNDEF_ERR_CODE -1
    #define INVAL_COORD "Invalid set of coordinates\n"
    ```
**The utils.c & .h files:**  
  They work around 2 structures:

    ```c
    struct uint_list {
        __UINT_FAST32_TYPE__ *arr;
        __UINT_FAST32_TYPE__ nr_elm;
    };
    struct int_list {
        __INT_FAST32_TYPE__ *arr;
        __INT_FAST32_TYPE__ nr_elm;
    };
    ```

  Those structures are used as a support to reading data from
the command lines parsed through the console. They support multiple
operations that are explained inside the utils files. In those files
we also hold some other functions that do simpler stuff.

**The worker.c & .h files:**  
  Here we have the main implementation of the program, support for commands,
data handling and parsing (console to structures). It is all explained in there
given the fact that all of the code is pretty straightforward there.

**Conclusion:**  
  This program took 2 weeks out of my life and shattered them to pieces. It
was a legit NIGHTMARE due to the ammount  of checker bugs, minor bugs I was too
blind to see and the big ammount of code it took to make it work. Yesterday I
was contemplating jumping off the balcony, now I'm waiting for the next
challenge. I learned a lot, mainly that I can correctly write complex functions
and completley screw up the easy ones. I stay true to my thesis, that dynamic
allocation deserves to be hated. Stay classy!

**P.S. Accurate representation of me writing this homework:**
```
                                     )
                            )      ((     (
                           (        ))     )
                    )       )      //     (
               _   (        __    (     ~->>
        ,-----' |__,_~~___<'__`)-~__--__-~->> <
        | //  : | -__   ~__ o)____)),__ - '> >-  >
        | //  : |- \_ \ -\_\ -\ \ \ ~\_  \ ->> - ,  >>
        | //  : |_~_\ -\__\ \~'\ \ \, \__ . -<-  >>
        `-----._| `  -__`-- - ~~ -- ` --~> >
         _/___\_    //)_`//  | ||]
   _____[_______]_[~~-_ (.L_/  ||
  [____________________]' `\_,/'/
    ||| /          |||  ,___,'./
    ||| \          |||,'______|
    ||| /          /|| I==||
    ||| \       __/_||  __||__
-----||-/------`-._/||-o--o---o---
  ~~~~~'
```
