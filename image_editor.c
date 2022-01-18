#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "utils.h"
#include "img.h"
#include "worker.h"

int main(void)
{
	struct img image;//the image we're gonna use
	init_img(&image);//initiating it
	//letting the get_commands() function do the job hehe
	return get_commands(&image);
}
