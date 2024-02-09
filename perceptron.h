#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <stdbool.h>

#define WIDTH 100
#define HEIGHT 100

typedef float Layer[HEIGHT][WIDTH];

// Function declarations
void layer_fill_rect(Layer layer, int x, int y, int w, int h, float val);
void layer_fill_circle(Layer layer, int cx, int cy, int r, float val);
void layer_save_as_ppm(Layer layer, const char *file_path);
void layer_load_bin(Layer layer, const char *file_path);
void layer_save_bin(Layer layer, const char *file_path);
float apply_weights(Layer inputs, Layer weights);
void add_input_to_weights(Layer input, Layer weight);
void sub_input_to_weights(Layer input, Layer weight);
void layer_random_rect(Layer layer);
void layer_random_circle(Layer layer);
int train_iteration(Layer input, Layer weights_layer);
int test_iteration(Layer input, Layer weights_layer);

#endif // PERCEPTRON_H
