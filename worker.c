#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

//Local header files
#include "macros.h"
#include "effects.h"
#include "utils.h"
#include "img.h"
#include "worker.h"

//Receives a filename, an image and checks if the file exists,
//loads it using the propper load function (binary/ascii)
//and selects the all image
void load_cmd(char *fname, struct img *image)
{
	//checking access
	if (access(fname, R_OK)) {
		printf(FAILED_LOAD_FILE, fname);
		return;
	}
	//calling the right function
	if (det_img_type(fname) > 3)
		load_img_binary(fname, &(*image));
	else
		load_img_ascii(fname, &(*image));
	//selecting the all image and printing a nice message
	sel_all(&(*image));
	printf(LOADED_FILE, fname);
}

//Receives a filename, an image, a string containing the command
// and checks if the file exists, loads it using the propper load
//function (binary/ascii) /and selects the all image
void save_cmd(char *fname, struct img *image, char *command)
{
	//checks to see if we have a loaded image
	if ((*image).type == 0) {
		printf(NO_IMG);
		return;
	}
	//calling the right function
	if (strstr(command, " ascii")) {
		fname = compile_fname(command);
		save_ascii_cmd(fname, &(*image));
	} else {
		fname = compile_fname(command);
		save_bin_cmd(fname, &(*image));
	}
}

//Receives a filename, an image and checks to see if image pointer exists,
//calls the ascii print function and prints the required message
void save_ascii_cmd(char *fname, struct img *image)
{
	//calls the print function and outputs a nice message
	print_ascii(&(*image), fname);
	printf(SAVED_IMG, fname);
}

//Receives a filename, an image and checks to see if image pointer exists,
//calls the binary print function and prints the required message
void save_bin_cmd(char *fname, struct img *image)
{
	//calls the print function and outputs a nice message
	print_binary(&(*image), fname);
	printf(SAVED_IMG, fname);
}

//Receives the angle, an image and checks to see if the image can be
//rotated and thel decides which rotate function to call.
//It also prints the required message
void rotate_cmd(__INT_FAST32_TYPE__ angle, struct img *image)
{
	//checking to see if the angle is valid
	if (angle % 90 != 0) {
		printf(ROTATE_NOT_90);
		return;
	}
	//used to see if the all image is selected
	int b1 = (*image).sel.x2 == (*image).width,
		b2 = (*image).sel.y2 == (*image).height;
	//calling the rotate_all image if the all image is selected
	if (b1 && b2 && (*image).sel.x1 == 0 && (*image).sel.y1 == 0) {
		rotate_all(&(*image), angle);
		printf(ROTATE_IMG, angle);
		return;
	}
	//if the image is not square print the oof message
	if (((*image).sel.x2 - (*image).sel.x1) !=
		((*image).sel.y2 - (*image).sel.y1)) {
		printf(ROTATE_NOT_SQ);
		return;
	}
	//if all wents smooth we're rotaing it all and printing
	//a nice message
	rotate_sel(&(*image), angle);
	printf(ROTATE_IMG, angle);
}

//Receives an image and a string containing the command,
//checks weather the selection arguments are valid or not
//and selects the desired sub-image
void sel_prm_cmd(struct img *image, char *line)
{
	int ctr = 0;//used to see if we read all the 4 sizes
	char dummy[BUFFER_SIZE] = "";
	strcpy(dummy, line); //dummy is used for strtok bcs
	char *p = strtok(dummy, " ");//strtok destroys the string
	p = strtok(NULL, " ");
	while (p) { //checking to see if the data is valid
		if (atoi(p) != check_if_number(p)) {
			printf(INVAL_CMD);
			return;
		}
		p = strtok(NULL, " ");
		ctr++;
	}
	if (ctr < 4) {
		printf(INVAL_CMD);
		return;
	} //making integers out of the data
	struct int_list *lst = str_to_int_arr(line);
	if (sel(&(*image), lst)) { //selecting the image
		free_int_list(lst);    //returns 0 if it failed
		return;
	}
	free_int_list(lst);
	//flipping them if they're not the right way
	if ((*image).sel.x1 > (*image).sel.x2) {
		__UINT_FAST32_TYPE__ aux = (*image).sel.x1;
		(*image).sel.x1 = (*image).sel.x2;
		(*image).sel.x2 = aux;
	}
	if ((*image).sel.y1 > (*image).sel.y2) {
		__UINT_FAST32_TYPE__ aux = (*image).sel.y1;
		(*image).sel.y1 = (*image).sel.y2;
		(*image).sel.y2 = aux;
	}
	//printing a nice message
	printf(SEL_GOOD, (*image).sel.x1, (*image).sel.y1,
		   (*image).sel.x2, (*image).sel.y2);
}

//Receives an image and calls the select all function
//and prints the required message
void sel_all_cmd(struct img *image)
{
	//calls the select all function and prints a nice message
	sel_all(&(*image));
	printf(SEL_ALL);
}

//Receives an image and calls the crop function
//and prints the required message
void crop_cmd(struct img *image)
{
	//calls the crop function and prints a nice message
	crop(&(*image));
	printf(CROPPED);
}

//Receives an image and a string containing the command and
//decides which filter to apply
void apply_cmd(struct img *image, char *command)
{
	if ((*image).type == 0) { //checking to see if we have a loaded image
		printf(NO_IMG);
		return;
	}
	if (!strchr(command, ' ')) { //if we have no space command no good
		printf(INVAL_CMD);
		return;
	}
	//if image is not rgb command no good
	if ((*image).type != 3 && (*image).type != 6) {
		printf(EASY_CHARLIE);
		return;
	}
	//filtering through the filters :)) and calling the required function
	//also we print a nice message
	if (strstr(command, "EDGE")) {
		effect(&(*image), edge_matrix);
		printf(APPLY_PARAM, "EDGE");
		return;
	}
	if (strstr(command, "SHARPEN")) {
		effect(&(*image), sharp_matrix);
		printf(APPLY_PARAM, "SHARPEN");
		return;
	}
	if (strstr(command, "GAUSSIAN_BLUR")) {
		effect(&(*image), gblur_matrix);
		printf(APPLY_PARAM, "GAUSSIAN_BLUR");
		return;
	}
	if (strstr(command, "BLUR")) {
		effect(&(*image), bblur_matrix);
		printf(APPLY_PARAM, "BLUR");
		return;
	}
	printf(INVAL_PARAM);
}

//Receives an image and reads commands from the console until
//the EXIT command is "summoned". Returns 0 if all went smooth
unsigned int get_commands(struct img *image)
{
	char *command = (char *)malloc(BUFFER_SIZE * sizeof(char)),
		 *fname = NULL;

	while (1) {
		//reading the line
		fgets(command, BUFFER_SIZE, stdin);
		if (strstr(command, "EXIT")) { // exiting if the command is exit
			if ((*image).type == 0)
				printf(NO_IMG);
			else
				delete_img(&(*image));
			break;
		}
		command[strlen(command) - 1] = '\0';// adding '\0' cuz it reads a '\n'
		if (strstr(command, "LOAD")) { //loading
			fname = compile_fname(command);
			if ((*image).type != 0)
				delete_img(&(*image));
			load_cmd(fname, &(*image));
			continue;
		}
		if (strstr(command, "SAVE")) { // saving
			if ((*image).type == 0) {
				printf(NO_IMG);
				continue;
			}
			if (strstr(command, "ascii")) {
				fname = compile_fname(command);
				save_ascii_cmd(fname, &(*image));
			} else {
				fname = compile_fname(command);
				save_bin_cmd(fname, &(*image));
			}
			continue;
		}
		if (strstr(command, "ROTATE")) { // rotating
			if ((*image).type == 0) {
				printf(NO_IMG);
				continue;
			}
			char *str = strtok(command, " ");//getting the angle with strtok
			str = strtok(NULL, " ");//and atoi
			__INT_FAST32_TYPE__ angle = atoi(str);
			rotate_cmd(angle, &(*image));
			continue;
		}
		if (strstr(command, "SELECT ALL")) { // selecting all
			if ((*image).type == 0) {
				printf(NO_IMG);
				continue;
			}
			sel_all_cmd(&(*image));
			continue;
		}
		if (strstr(command, "SELECT")) { // selecting only a portion
			if ((*image).type == 0) {
				printf(NO_IMG);
				continue;
			}
			sel_prm_cmd(&(*image), command);
			continue;
		}
		if (strstr(command, "CROP")) { // cropping
			if ((*image).type == 0) {
				printf(NO_IMG);
				continue;
			}
			crop_cmd(&(*image));
			continue;
		}
		if (strstr(command, "APPLY")) { // filters thingy
			apply_cmd(&(*image), command);
			continue;
		}
		printf(INVAL_CMD);
	}
	free(command);
	return 0;
}
