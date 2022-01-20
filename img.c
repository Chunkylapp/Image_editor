#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//Local header files
#include "macros.h"
#include "utils.h"
#include "img.h"

//Receives an image and an integer array and
//selects the image using the values in the array
int sel(struct img *tmp, struct int_list *lst)
{
	//this block checks to see if the coordinates are valid
	//the coordinates are held into the integer list *lst
	if ((*lst).arr[1] < 0 || (*lst).arr[1] > (int)(*tmp).width) {
		printf(INVAL_COORD);
		return 1;
	}
	if ((*lst).arr[2] < 0 || (*lst).arr[2] > (int)(*tmp).height) {
		printf(INVAL_COORD);
		return 1;
	}
	if ((*lst).arr[3] < 0 || (*lst).arr[3] > (int)(*tmp).width) {
		printf(INVAL_COORD);
		return 1;
	}
	if ((*lst).arr[4] < 0 || (*lst).arr[4] > (int)(*tmp).height) {
		printf(INVAL_COORD);
		return 1;
	}
	__UINT_FAST32_TYPE__ aux;
	if ((*lst).arr[1] == (*lst).arr[3] || (*lst).arr[2] == (*lst).arr[4]) {
		printf(INVAL_COORD);
		return 1;
	}
	//putting the coordinates in the image's selection structure
	(*tmp).sel.x1 = (__UINT_FAST32_TYPE__)(*lst).arr[1];
	(*tmp).sel.y1 = (__UINT_FAST32_TYPE__)(*lst).arr[2];
	(*tmp).sel.x2 = (__UINT_FAST32_TYPE__)(*lst).arr[3];
	(*tmp).sel.y2 = (__UINT_FAST32_TYPE__)(*lst).arr[4];
	//checking to see if they're in the right variable
	if ((*tmp).sel.x1 > (*tmp).sel.x2) {
		aux = (*tmp).sel.x2;
		(*tmp).sel.x2 = (*tmp).sel.x1;
		(*tmp).sel.x1 = aux;
	}
	if ((*tmp).sel.y1 > (*tmp).sel.y2) {
		aux = (*tmp).sel.y2;
		(*tmp).sel.y2 = (*tmp).sel.y1;
		(*tmp).sel.y1 = aux;
	}
	return 0;
}

//Receives an image and selects it all
void sel_all(struct img *tmp)
{
	(*tmp).sel.x1 = 0;
	(*tmp).sel.y1 = 0;
	(*tmp).sel.x2 = (*tmp).width;
	(*tmp).sel.y2 = (*tmp).height;
}

//Receives an image and one of the 4 kernels defined in
//ffects.h and effects.c and applies it to the image
void effect(struct img *tmp, const double effect_matrix[][3])
{
	//creating an auxiliary matrix to place the modified pixels
	struct rgb **aux = NULL;
	aux = (struct rgb **)calloc(((*tmp).height + 1), sizeof(struct rgb *));
	if (!aux) {
		fprintf(stderr, UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	for (unsigned int i = 0; i < (*tmp).height + 1; i++) {
		aux[i] = (struct rgb *)calloc(((*tmp).width + 1), sizeof(struct rgb));
		if (!aux[i]) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
	}

	//increasing the selection in order to create a border around the selection
	//if the selection covers the all image and marking it by changing the value
	//of the modf_ value
	unsigned int modf_x1 = 0, modf_x2 = 0, modf_y1 = 0, modf_y2 = 0;
	if ((*tmp).sel.x1 == 0) {
		(*tmp).sel.x1++;
		modf_x1 = 1;
	}
	if ((*tmp).sel.y1 == 0) {
		(*tmp).sel.y1++;
		modf_y1 = 1;
	}
	if ((*tmp).sel.x2 == (*tmp).width) {
		(*tmp).sel.x2--;
		modf_x2 = 1;
	}
	if ((*tmp).sel.y2 == (*tmp).height) {
		(*tmp).sel.y2--;
		modf_y2 = 1;
	}
	//cicling through each pixel
	for (unsigned int i = (*tmp).sel.y1; i < (*tmp).sel.y2; i++)
		for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++) {
			//cycling through the kernel matrix and the neighbouring
			//pixels of the current pixel given by the 2 fors above
			double sum_r = 0.0, sum_g = 0.0, sum_b = 0.0;
			for (int k = -1; k < 2; k++)
				for (int p = -1; p < 2; p++) {
					sum_r += (*tmp).rgb[i + k][j + p].r
							* effect_matrix[k + 1][p + 1];
					sum_g += (*tmp).rgb[i + k][j + p].g
							* effect_matrix[k + 1][p + 1];
					sum_b += (*tmp).rgb[i + k][j + p].b
							* effect_matrix[k + 1][p + 1];
				}
			//clamping the value between the pixel value range
			aux[i][j].r = clamp(sum_r, 0.0, (*tmp).max_val * 1.0);
			aux[i][j].g = clamp(sum_g, 0.0, (*tmp).max_val * 1.0);
			aux[i][j].b = clamp(sum_b, 0.0, (*tmp).max_val * 1.0);
		}
	//placing it back in the image matrix
	for (unsigned int i = (*tmp).sel.y1; i < (*tmp).sel.y2; i++)
		for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++) {
			(*tmp).rgb[i][j].r = aux[i][j].r;
			(*tmp).rgb[i][j].g = aux[i][j].g;
			(*tmp).rgb[i][j].b = aux[i][j].b;
		}
	//burning the auxiliary matrix
	for (unsigned int i = 0; i < (*tmp).height + 1; i++)
		free(aux[i]);
	free(aux);
	//if we bordered the image we're moving the selection
	//to it's previous value (selecting the all image)
	if (modf_x1)
		(*tmp).sel.x1--;
	if (modf_x2)
		(*tmp).sel.x2++;
	if (modf_y1)
		(*tmp).sel.y1--;
	if (modf_y2)
		(*tmp).sel.y2++;
}

//Receives and image and initializes it with NULL and 0 values
void init_img(struct img *tmp)
{
	(*tmp).type = 0;
	(*tmp).width = 0;
	(*tmp).height = 0;
	(*tmp).max_val = 0;
}

//Receives an image and it's properties, assigns them to the
//image and allocates memory
void new_img(struct img *tmp,
			 __UINT_FAST32_TYPE__ width,
			 __UINT_FAST32_TYPE__ height,
			 __UINT_FAST32_TYPE__ type,
			 __UINT_FAST32_TYPE__ max_val)
{	//placing the properties in the image's given fields
	(*tmp).width = width;
	(*tmp).height = height;
	(*tmp).type = type;
	(*tmp).max_val = max_val;
	(*tmp).sel.x1 = 0;
	(*tmp).sel.y1 = 0;
	(*tmp).sel.x2 = width;
	(*tmp).sel.y2 = height;
	//allocating space for the image if it's monochrome
	if ((*tmp).type != 3 && (*tmp).type != 6) {
		(*tmp).mono_chr = (double **)
						  calloc(height, sizeof(double *));
		if (!(*tmp).mono_chr) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		for (unsigned int i = 0; i < height; i++) {
			(*tmp).mono_chr[i] = (double *)
								 calloc(width, sizeof(double));
			if (!(*tmp).mono_chr[i]) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR_CODE);
			}
		}
	} else { //allocating space for the image if it's rgb
		(*tmp).rgb = (struct rgb **)calloc(height, sizeof(struct rgb *));
		if (!(*tmp).rgb) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		for (unsigned int i = 0; i < height; i++) {
			(*tmp).rgb[i] = (struct rgb *)calloc(width, sizeof(struct rgb));
			if (!(*tmp).rgb[i]) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR_CODE);
			}
		}
	}
}

//Receives an image and deletes it
void delete_img(struct img *tmp)
{
	//frees the mono_chr matrix if image is monochrome
	if ((*tmp).type != 3 && (*tmp).type != 6) {
		for (unsigned int i = 0; i < (*tmp).height; i++)
			free((*tmp).mono_chr[i]);
		free((*tmp).mono_chr);
	} else {//frees the rgb matrix if image is rgb
		for (unsigned int i = 0; i < (*tmp).height; i++)
			free((*tmp).rgb[i]);
		free((*tmp).rgb);
	}
	//marks the image as not loaded
	(*tmp).type = 0;
}

//Receives an image and a string containing a filename
//and prints the data as binary
void print_binary(struct img *tmp, char *fname)
{
	//opening the file and checking if it opened correctly
	FILE *output = fopen(fname, "wb");

	if (!output) {
		delete_img(&(*tmp));
		printf(UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	//if the image is in ascii format we print +3 at the type
	if ((*tmp).type < 4)
		fprintf(output, "P%lu\n", (*tmp).type + 3);
	else
		fprintf(output, "P%lu\n", (*tmp).type);
	fprintf(output, "%lu %lu\n", (*tmp).width, (*tmp).height);
	if ((*tmp).type != 1 && (*tmp).type != 4)
		fprintf(output, "%lu\n", (*tmp).max_val);
	//in order to print the double value we use an auxiliary
	//variable and cast the pixel value to that variable after
	//rounding the pixel value
	if ((*tmp).type != 3 && (*tmp).type != 6)
		for (unsigned int i = 0; i < (*tmp).height; i++)
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				uint_fast8_t aux = (uint_fast8_t)
										  round((*tmp).mono_chr[i][j]);
				fwrite(&aux, sizeof(uint_fast8_t), 1, output);
			}
	else
		for (unsigned int i = 0; i < (*tmp).height; i++)
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				uint_fast8_t aux = (uint_fast8_t)
										   round((*tmp).rgb[i][j].r);
				fwrite(&aux, sizeof(uint_fast8_t), 1, output);
				aux = (uint_fast8_t)round((*tmp).rgb[i][j].g);
				fwrite(&aux, sizeof(uint_fast8_t), 1, output);
				aux = (uint_fast8_t)round((*tmp).rgb[i][j].b);
				fwrite(&aux, sizeof(uint_fast8_t), 1, output);
			}
	fclose(output);
}

//Receives an image and a string containing a filename
//and prints the data as ascii
void print_ascii(struct img *tmp, char *fname)
{
	//opening the file and checking if it opened correctly
	FILE *output = fopen(fname, "w");

	if (!output) {
		delete_img(&(*tmp));
		printf(UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	}
	//if the image is in binary format we print -3 at the type
	if ((*tmp).type > 3)
		fprintf(output, "P%lu\n", (*tmp).type - 3);
	else
		fprintf(output, "P%lu\n", (*tmp).type);
	fprintf(output, "%lu %lu\n", (*tmp).width, (*tmp).height);

	if ((*tmp).type != 1 && (*tmp).type != 4)
		fprintf(output, "%lu\n", (*tmp).max_val);

	//in order to print the double value we use an auxiliary
	//variable and cast the pixel value to that variable after
	//rounding the pixel value
	if ((*tmp).type != 3 && (*tmp).type != 6)
		for (unsigned int i = 0; i < (*tmp).height; i++) {
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				uint_fast8_t aux = (uint_fast8_t)
										  round((*tmp).mono_chr[i][j]);
				fprintf(output, "%hhu ", aux);
			}
			fprintf(output, "\n");
		}
	else
		for (unsigned int i = 0; i < (*tmp).height; i++) {
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				uint_fast8_t aux = (uint_fast8_t)
										   round((*tmp).rgb[i][j].r);
				fprintf(output, "%hhu ", aux);
				aux = (uint_fast8_t)round((*tmp).rgb[i][j].g);
				fprintf(output, "%hhu ", aux);
				aux = (uint_fast8_t)round((*tmp).rgb[i][j].b);
				fprintf(output, "%hhu   ", aux);
			}
			fprintf(output, "\n");
		}
	fclose(output);
}

//Function Receives an image and the angle to rotate it
//this function works only if the all image is selected
void rotate_all(struct img *tmp, int angle)
{
	//if the angle is 360 or -360 we're not doing anything lol
	if (angle == 360 || angle == -360)
		return;
	if (angle < 0)
		angle = 360 + angle;
	//we're using an auxiliary matrix
	double **aux_mono_chr;
	struct rgb **aux_rgb;
	for (; angle > 0; angle = angle - 90) {
		__UINT_FAST32_TYPE__ new_i = 0, new_j = 0;
		if ((*tmp).type != 3 && (*tmp).type != 6) {
			aux_mono_chr = (double **)
						  calloc((*tmp).width, sizeof(double *));
			if (!aux_mono_chr) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR);
			}

			for (unsigned int i = 0; i < (*tmp).width; i++) {
				aux_mono_chr[i] = (double *)
								  calloc((*tmp).height,
										 sizeof(double));
				if (!aux_mono_chr[i]) {
					fprintf(stderr, UNDEF_ERR);
					exit(UNDEF_ERR);
				} //after allocating space we're moving by columns (from 0
			} //and lines (from the height value) and copying them into
			//the aux matrix, hence rotating the values by 90 degrees to the
			//right new_i and new_j hold the sizes of the auxiliary matrix
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				for (int i = (*tmp).height - 1; i >= 0; i--)
					aux_mono_chr[new_i][new_j++] = (*tmp).mono_chr[i][j];
				new_j = 0;
				new_i++;
			} //freeing the image matrix and putting the aux in there hehe :P
			for (unsigned int i = 0; i < (*tmp).height; i++)
				free((*tmp).mono_chr[i]);
			free((*tmp).mono_chr);
			(*tmp).mono_chr = aux_mono_chr;
		} else {
			aux_rgb = (struct rgb **)
					  calloc((*tmp).width, sizeof(struct rgb *));
			if (!aux_rgb) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR);
			}
			for (unsigned int i = 0; i < (*tmp).width; i++) {
				aux_rgb[i] = (struct rgb *)
							 calloc((*tmp).height, sizeof(struct rgb));
				if (!aux_rgb[i]) {
					fprintf(stderr, UNDEF_ERR);
					exit(UNDEF_ERR);
				} //after allocating space we're moving by columns (from 0)
			} //and lines (from the height value) and copying them into
			//the aux matrix, hence rotating the values by 90 degrees to the
			//right new_i and new_j hold the sizes of the auxiliary matrix
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				for (int i = (*tmp).height - 1; i >= 0; i--) {
					aux_rgb[new_i][new_j].r = (*tmp).rgb[i][j].r;
					aux_rgb[new_i][new_j].g = (*tmp).rgb[i][j].g;
					aux_rgb[new_i][new_j++].b = (*tmp).rgb[i][j].b;
				}
				new_j = 0;
				new_i++;
			} //freeing the image matrix and putting the aux in there hehe :P
			for (unsigned int i = 0; i < (*tmp).height; i++)
				free((*tmp).rgb[i]);
			free((*tmp).rgb);
			(*tmp).rgb = aux_rgb;
		} //switching the sizes and selecting the all image in order to not
		//screw things up even more xD
		__UINT_FAST32_TYPE__ aux = (*tmp).height;
		(*tmp).height = (*tmp).width;
		(*tmp).width = aux;
		sel_all(&(*tmp));
	}
}

//Function Receives an image and the angle to rotate it
//this function works only if a sub-image is selected
void rotate_sel(struct img *tmp, int angle)
{
	if (angle == 360 || angle == -360) //if angle is (-)360 plain return
		return;
	if (angle < 0)
		angle = 360 + angle;
	double **aux_mono_chr = NULL; //we're using an auxiliary matrix
	struct rgb **aux_rgb = NULL;
	__UINT_FAST32_TYPE__ size = (*tmp).sel.x2 - (*tmp).sel.x1;
	if ((*tmp).type != 3 && (*tmp).type != 6) {
		aux_mono_chr = (double **)
						calloc(size, sizeof(double *));
		if (!aux_mono_chr) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		for (unsigned int i = 0; i < size; i++) {
			aux_mono_chr[i] = (double *)
							  calloc(size, sizeof(double));
			if (!aux_mono_chr[i]) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR_CODE);
			}
		}
	} else {
		aux_rgb = (struct rgb **)calloc(size, sizeof(struct rgb *));
		if (!aux_rgb) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		for (unsigned int i = 0; i < size; i++) {
			aux_rgb[i] = (struct rgb *)calloc(size, sizeof(struct rgb));
			if (!aux_rgb[i]) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR_CODE);
			}
		}
	}
	for (; angle > 0; angle = angle - 90) { //rotating the matrix
		__UINT_FAST32_TYPE__ new_i = 0, new_j = 0;
		if ((*tmp).type != 3 && (*tmp).type != 6) {
			for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++) {
				for (int i = (*tmp).sel.y2 - 1; i >= (int)(*tmp).sel.y1; i--)
					aux_mono_chr[new_i][new_j++] = (*tmp).mono_chr[i][j];
				new_j = 0;
				new_i++;
			} //putting them back into the image matrix
			for (unsigned int i = (*tmp).sel.y1; i < (*tmp).sel.y2; i++)
				for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++)
					(*tmp).mono_chr[i][j] =
							aux_mono_chr[i - (*tmp).sel.y1][j - (*tmp).sel.x1];
		} else { //rotating the values using the auxiliary matrix
			for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++) {
				for (int i = (*tmp).sel.y2 - 1; i >= (int)(*tmp).sel.y1; i--) {
					aux_rgb[new_i][new_j].r = (*tmp).rgb[i][j].r;
					aux_rgb[new_i][new_j].g = (*tmp).rgb[i][j].g;
					aux_rgb[new_i][new_j++].b = (*tmp).rgb[i][j].b;
				}
				new_j = 0;
				new_i++;
			} //putting them back into the image matrix
			for (unsigned int i = (*tmp).sel.y1; i < (*tmp).sel.y2; i++)
				for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++) {
					(*tmp).rgb[i][j].r =
							aux_rgb[i - (*tmp).sel.y1][j - (*tmp).sel.x1].r;
					(*tmp).rgb[i][j].g =
							aux_rgb[i - (*tmp).sel.y1][j - (*tmp).sel.x1].g;
					(*tmp).rgb[i][j].b =
							aux_rgb[i - (*tmp).sel.y1][j - (*tmp).sel.x1].b;
				}
		}
	} //burning the auxiliary matrices
	if ((*tmp).type != 3 && (*tmp).type != 6) {
		for (unsigned int i = 0; i < size; i++)
			free(aux_mono_chr[i]);
		free(aux_mono_chr);
	} else {
		for (unsigned int i = 0; i < size; i++)
			free(aux_rgb[i]);
		free(aux_rgb);
	}
}

//Function Receives an image and it crops it based
//on the selection values inside the image structure
void crop(struct img *tmp)
{
	//auxiliary matrices (surprise tools we're gonna use later)
	double **aux_mono_chr;
	struct rgb **aux_rgb;
	//getting the sizes (size1 is equal to size 2)
	unsigned int size1 = (*tmp).sel.y2 - (*tmp).sel.y1,
				 size2 = (*tmp).sel.x2 - (*tmp).sel.x1;
	if ((*tmp).type != 3 && (*tmp).type != 6) {
		//allocating space
		aux_mono_chr = (double **)
						calloc(size1, sizeof(double *));
		if (!aux_mono_chr) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		for (unsigned int i = 0; i < size1; i++) {
			aux_mono_chr[i] = (double *)
								calloc(size2, sizeof(double));
			if (!aux_mono_chr[i]) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR_CODE);
			}
		}
		//copying the selection into the auxiliary
		for (unsigned int i = (*tmp).sel.y1; i < (*tmp).sel.y2; i++)
			for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++)
				aux_mono_chr[i - (*tmp).sel.y1][j - (*tmp).sel.x1] =
					(*tmp).mono_chr[i][j];
		//burning the old matrix
		for (unsigned int i = 0; i < (*tmp).height; i++)
			free((*tmp).mono_chr[i]);
		free((*tmp).mono_chr);
		//moving the new matrix and it's data to the image
		(*tmp).mono_chr = aux_mono_chr;
		(*tmp).height = (*tmp).sel.y2 - (*tmp).sel.y1;
		(*tmp).width = (*tmp).sel.x2 - (*tmp).sel.x1;
		(*tmp).sel.x1 = (*tmp).sel.y1 = 0;
		(*tmp).sel.x2 = (*tmp).width;
		(*tmp).sel.y2 = (*tmp).height;
	} else {
		//allocating space
		aux_rgb = (struct rgb **)calloc(size1, sizeof(struct rgb *));
		if (!aux_rgb) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		for (unsigned int i = 0; i < size1; i++) {
			aux_rgb[i] = (struct rgb *)calloc(size2, sizeof(struct rgb));
			if (!aux_rgb[i]) {
				fprintf(stderr, UNDEF_ERR);
				exit(UNDEF_ERR_CODE);
			}
		}
		//copying the selection into the auxiliary
		for (unsigned int i = (*tmp).sel.y1; i < (*tmp).sel.y2; i++)
			for (unsigned int j = (*tmp).sel.x1; j < (*tmp).sel.x2; j++) {
				aux_rgb[i - (*tmp).sel.y1][j - (*tmp).sel.x1].r =
						(*tmp).rgb[i][j].r;
				aux_rgb[i - (*tmp).sel.y1][j - (*tmp).sel.x1].g =
						(*tmp).rgb[i][j].g;
				aux_rgb[i - (*tmp).sel.y1][j - (*tmp).sel.x1].b =
						(*tmp).rgb[i][j].b;
			}
		//burning the old matrix
		for (unsigned int i = 0; i < (*tmp).height; i++)
			free((*tmp).rgb[i]);
		free((*tmp).rgb);
		(*tmp).rgb = aux_rgb;
		//moving the new matrix and it's data to the image
		(*tmp).height = (*tmp).sel.y2 - (*tmp).sel.y1;
		(*tmp).width = (*tmp).sel.x2 - (*tmp).sel.x1;
		(*tmp).sel.x1 = (*tmp).sel.y1 = 0;
		(*tmp).sel.x2 = (*tmp).width;
		(*tmp).sel.y2 = (*tmp).height;
	}
}

//Function Receives a filename and an image and reads
//data from the file into the image. It only works on
//binary files
void load_img_binary(char *f_name, struct img *tmp)
{
	FILE *input = fopen(f_name, "rb");//opening the image
	if (!input) { //checking if it's opened correctly
		printf(UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	} //long unsigned int array used to fetch numbers from the lines
	struct uint_list *lst;
	__UINT_FAST32_TYPE__ type = 0, width = 0, height = 0, max_val = 0;
	char *line = NULL, c = 0;
	while (!feof(input)) {
		line = (char *)calloc(BUFFER_SIZE, sizeof(char));
		if (!line) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		fgets(line, BUFFER_SIZE, input);
		if (line[0] == COMM) { //if line is a comment we're skipping it
			free(line);
			continue;
		} //if the line's 1st element is 'P' we're getting the 2nd value
		if (line[0] == TYPE) { //and putting it in the type variable
			type = line[1] - ZERO;
			free(line);
			continue;
		}
		//if the width is 0 and the line passed the if above
		//it must hold the sizes so we're fetching the numbers
		//using the lst and placing them in the width/height variables
		if (width == 0) {
			lst = str_to_uint_arr(line);
			width = (*lst).arr[0];
			height = (*lst).arr[1];
			free_uint_list(lst);
			free(line);
			if (type == 1 || type == 4)
				break;
			continue;
		}
		//if image is not type 1 or 4 and the max_val is 0
		//it means that we must be encountering the max value
		//line and we're fetching it using the lst
		if (type != 1 && type != 4 && max_val == 0) {
			lst = str_to_uint_arr(line);
			max_val = (*lst).arr[0];
			free_uint_list(lst);
			free(line);
			break;
		}
	}
	//skipping through comments by reading only the 1st element of the line
	//if it's equal to '#' we;re putting it back and breaking out of the loop
	while (!feof(input)) {
		c = fgetc(input);
		if (c != '#') {
			ungetc(c, input);
			break;
		}
		line = (char *)calloc(BUFFER_SIZE, sizeof(char));
		fgets(line, BUFFER_SIZE, input);
		free(line);
	}
	uint_fast8_t aux;//aux variable used to read data
	new_img(&(*tmp), width, height, type, max_val);//making a new image hehe
	if ((*tmp).type != 3 && (*tmp).type != 6) //reading stuff
		for (unsigned int i = 0; i < (*tmp).height; i++)
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				fread(&aux, sizeof(uint_fast8_t), 1, input);
				(*tmp).mono_chr[i][j] = (double)aux;
			}
	else
		for (unsigned int i = 0; i < (*tmp).height; i++)
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				fread(&aux, sizeof(uint_fast8_t), 1, input);
				(*tmp).rgb[i][j].r = (double)aux;
				fread(&aux, sizeof(uint_fast8_t), 1, input);
				(*tmp).rgb[i][j].g = (double)aux;
				fread(&aux, sizeof(uint_fast8_t), 1, input);
				(*tmp).rgb[i][j].b = (double)aux;
			}
	fclose(input);
}

//Function Receives a filename and an image and reads
//data from the file into the image. It only works on
//ascii files
void load_img_ascii(char *f_name, struct img *tmp)
{
	FILE *input = fopen(f_name, "r");//opening the image
	if (!input) { //checking if it's opened correctly
		printf(UNDEF_ERR);
		exit(UNDEF_ERR_CODE);
	} //long unsigned int array used to fetch numbers from the line
	struct uint_list *lst;
	__UINT_FAST32_TYPE__ type = 0, width = 0, height = 0, max_val = 0;
	char *line = NULL, c = 0;
	while (!feof(input)) {
		line = (char *)calloc(BUFFER_SIZE, sizeof(char));
		if (!line) {
			fprintf(stderr, UNDEF_ERR);
			exit(UNDEF_ERR_CODE);
		}
		fgets(line, BUFFER_SIZE, input);
		if (line[0] == COMM) {//if line is a comment we're skipping it
			free(line);
			continue;
		} //if the line's 1st element is 'P' we're getting the 2nd value
		if (line[0] == TYPE) {//and putting it in the type variable
			type = line[1] - ZERO;
			free(line);
			continue;
		}
		//if the width is 0 and the line passed the if above
		//it must hold the sizes so we're fetching the numbers
		//using the lst and placing them in the width/height variables
		if (width == 0) {
			lst = str_to_uint_arr(line);
			width = (*lst).arr[0];
			height = (*lst).arr[1];
			free_uint_list(lst);
			free(line);
			if (type == 1 || type == 4)
				break;
			continue;
		}
		//if image is not type 1 or 4 and the max_val is 0
		//it means that we must be encountering the max value
		//line and we're fetching it using the lst
		if (type != 1 && type != 4 && max_val == 0) {
			lst = str_to_uint_arr(line);
			max_val = (*lst).arr[0];
			free_uint_list(lst);
			free(line);
			break;
		}
	}
	//skipping through comments by reading only the 1st element of the line
	//if it's equal to '#' we;re putting it back and breaking out of the loop
	while (!feof(input)) {
		c = fgetc(input);
		if (c != '#') {
			ungetc(c, input);
			break;
		}
		line = (char *)calloc(BUFFER_SIZE, sizeof(char));
		fgets(line, BUFFER_SIZE, input);
		free(line);
	}
	uint_fast8_t aux;//aux variable used to read data
	new_img(&(*tmp), width, height, type, max_val);//making a new image hehe
	if ((*tmp).type != 3 && (*tmp).type != 6)//reading stuff
		for (unsigned int i = 0; i < (*tmp).height; i++)
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				fscanf(input, "%hhu", &aux);
				(*tmp).mono_chr[i][j] = (double)aux;
			}
	else
		for (unsigned int i = 0; i < (*tmp).height; i++)
			for (unsigned int j = 0; j < (*tmp).width; j++) {
				fscanf(input, "%hhu", &aux);
				(*tmp).rgb[i][j].r = (double)aux;
				fscanf(input, "%hhu", &aux);
				(*tmp).rgb[i][j].g = (double)aux;
				fscanf(input, "%hhu", &aux);
				(*tmp).rgb[i][j].b = (double)aux;
			}
	fclose(input);
}
