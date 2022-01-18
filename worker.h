#ifndef WORKER_H_
#define WORKER_H_

void load_cmd(char *fname, struct img *image);

void save_cmd(char *fname, struct img *image, char *command);

void save_ascii_cmd(char *fname, struct img *image);

void save_bin_cmd(char *fname, struct img *image);

void rotate_cmd(__INT_FAST32_TYPE__ angle, struct img *image);

void sel_prm_cmd(struct img *image, char *line);

void sel_all_cmd(struct img *image);

void crop_cmd(struct img *image);

void apply_cmd(struct img *image, char *command);

unsigned int get_commands(struct img *image);
#endif
