#ifndef UTILS_H_
#define UTILS_H_

//Structure holds a pointer and the number of
//elements that the pointer holds. This structure
//has the long unsigned int datatype
struct uint_list {
	__UINT_FAST32_TYPE__ *arr;
	__UINT_FAST32_TYPE__ nr_elm;
};

//Structure holds a pointer and the number of
//elements that the pointer holds. This structure
//has the long int datatype
struct int_list {
	__INT_FAST32_TYPE__ *arr;
	__INT_FAST32_TYPE__ nr_elm;
};

struct int_list *new_int_list(void);

struct uint_list *new_uint_list(void);

void free_int_list(struct int_list *lst);

void free_uint_list(struct uint_list *lst);

void enl_int_list(struct int_list **lst);

void enl_uint_list(struct uint_list **lst);

struct int_list *str_to_int_arr(char *str);

struct uint_list *str_to_uint_arr(char *str);

double clamp(double val, double low, double high);

char *compile_fname(char *string);

void swap(__UINT_FAST32_TYPE__ *a, __UINT_FAST32_TYPE__ *b);

unsigned int det_img_type(char *fname);

int check_if_number(char str[]);

unsigned int str_to_int(char str[]);

#endif
