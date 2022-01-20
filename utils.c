#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "macros.h"
#include "utils.h"

//Creates a new long int list and returns it
struct int_list *new_int_list(void)
{
	//allocating space for the new array
	struct int_list *lst = (struct int_list *)
							calloc(1, sizeof(struct int_list));
	if (!lst) {
		fprintf(stderr, UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	(*lst).arr = NULL;
	(*lst).nr_elm = 0;

	return lst;
}

//Creates a new long unsigned int list and returns it
struct uint_list *new_uint_list(void)
{
	//allocating space for the new array
	struct uint_list *lst = (struct uint_list *)
							calloc(1, sizeof(struct uint_list));
	if (!lst) {
		fprintf(stderr, UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	(*lst).arr = NULL;
	(*lst).nr_elm = 0;

	return lst;
}

//Deletes a long int list
void free_int_list(struct int_list *lst)
{
	//burning the array
	free((*lst).arr);
	free(lst);
}

//Deletes a long unsigned int list
void free_uint_list(struct uint_list *lst)
{
	//burning the array
	free((*lst).arr);
	free(lst);
}

//Enlarges a long int list with 1 position
void enl_int_list(struct int_list **lst)
{
	//making the array larger by 1 space
	(**lst).arr = (__INT_FAST32_TYPE__ *)
					realloc((**lst).arr,
							((**lst).nr_elm + 1) *
							sizeof(__INT_FAST32_TYPE__));
	if (!lst) {
		fprintf(stderr, UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	(**lst).arr[(**lst).nr_elm] = 0;
}

//Enlarges a long unsigned  int list with 1 position
void enl_uint_list(struct uint_list **lst)
{
	//making the array larger by 1 space
	(**lst).arr = (__UINT_FAST32_TYPE__ *)
					realloc((**lst).arr,
							((**lst).nr_elm + 1) *
							sizeof(__UINT_FAST32_TYPE__));
	if (!lst) {
		fprintf(stderr, UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	(**lst).arr[(**lst).nr_elm] = 0;
}

//Converts a string to a long int list
struct int_list *str_to_int_arr(char *str)
{
	//converting string to integer using strtok
	//to split it into substrings divided by spaces
	struct int_list *lst = new_int_list();
	char *ptr = strtok(str, " ");
	while (ptr) {
		enl_int_list(&lst);
		(*lst).arr[(*lst).nr_elm++] = atoi(ptr);
		ptr = strtok(NULL, " ");
	}
	return lst;
}

//Converts a string to a long unsigned int list
struct uint_list *str_to_uint_arr(char *str)
{
	//converting string to integer using strtok
	//to split it into substrings divided by spaces
	struct uint_list *lst = new_uint_list();
	char *ptr = strtok(str, " ");
	while (ptr) {
		enl_uint_list(&lst);
		(*lst).arr[(*lst).nr_elm++] = atoi(ptr);
		ptr = strtok(NULL, " ");
	}
	return lst;
}

//Clamps the val value between 2 points, low and high
double clamp(double val, double low, double high)
{
	if (val < low)
		return low;
	if (val > high)
		return high;
	return val;
}

//Returns the filename string from a command string
char *compile_fname(char *string)
{
	//if the command is correct (which we check before calling
	//this function) the filename is the 2nd substring divided by spaces
	char *fname;
	fname = strtok(string, " ");
	fname = strtok(NULL, " ");
	return fname;
}

//Swaps 2 long unsigned int values
void swap(__UINT_FAST32_TYPE__ *a, __UINT_FAST32_TYPE__ *b)
{
	__UINT_FAST32_TYPE__ c;
	c = *a;
	*a = *b;
	*b = c;
}

//Determines the image type of a given file
//and returns 0 if the file is not a supported format
unsigned int det_img_type(char *fname)
{
	//opening the file, getting the 1st line and returning
	//the image's type. The file is not a supported format we return 0
	FILE *input = fopen(fname, "r");
	char *line = (char *)malloc(BUFFER_SIZE * sizeof(char));
	if (!line) {
		fprintf(stderr, UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	fgets(line, BUFFER_SIZE, input);
	while (line[0] == '#')
		fgets(line, BUFFER_SIZE, input);
	fclose(input);
	if (line[1] == '1') {
		free(line);
		return 1;
	}
	if (line[1] == '2') {
		free(line);
		return 2;
	}
	if (line[1] == '3') {
		free(line);
		return 3;
	}
	if (line[1] == '4') {
		free(line);
		return 4;
	}
	if (line[1] == '5') {
		free(line);
		return 5;
	}
	if (line[1] == '6') {
		free(line);
		return 6;
	}
	return 0;
}

//Equivalent of atoi()
int check_if_number(char str[])
{
	int i = 0, sum = 0;
	if (str[i] == '-')
		i++;
	while (str[i] != '\0') {
		sum = sum * 10 + str[i] - '0';
		i++;
	}
	if (str[0] == '-')
		sum = sum * -1;
	return sum;
}

//Equivalent of atoi()
unsigned int str_to_int(char str[])
{
	char *p = strtok(str, " ");
	p = strtok(NULL, " ");
	unsigned int i = 0, nr = 0;
	while (p[i] != '\n') {
		nr = nr * 10 + p[i] - '0';
		i++;
	}
	return nr;
}
