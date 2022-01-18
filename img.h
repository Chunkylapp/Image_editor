#ifndef IMG_H_
#define IMG_H_

//This structure holds 3 values
//that are used as an rgb pixel
struct rgb {
	double r, g, b;
};

//This structure holds 4 values that
//represent the 2 points that delimit the selection
struct select {
	__UINT_FAST32_TYPE__ x1, x2, y1, y2;
};

//This structure holds 4 values that represent an image's
//properties ,a selection and 2 arrays,holding monochrome pixels
//or rgb pixels, which are allocated depending on the type of the image
struct img {
	__UINT_FAST32_TYPE__ type;
	__UINT_FAST32_TYPE__ width, height;
	__UINT_FAST32_TYPE__ max_val;
	double **mono_chr;
	struct select sel;
	struct rgb **rgb;
};

int sel(struct img *tmp, struct int_list *lst);

void sel_all(struct img *tmp);

void effect(struct img *tmp, const double effect_matrix[][3]);

void init_img(struct img *tmp);

void new_img(struct img *tmp, __UINT_FAST32_TYPE__ width,
			 __UINT_FAST32_TYPE__ height,
			 __UINT_FAST32_TYPE__ type,
			 __UINT_FAST32_TYPE__ max_val);

void delete_img(struct img *tmp);

void debug_print_header(struct img *tmp);

void debug_print(struct img *tmp);

void print_binary(struct img *tmp, char *fname);

void print_ascii(struct img *tmp, char *fname);

void rotate_all(struct img *tmp, int angle);

void rotate_sel(struct img *tmp, int angle);

void crop(struct img *tmp);

void load_img_binary(char *f_name, struct img *tmp);

void load_img_ascii(char *f_name, struct img *tmp);

#endif
