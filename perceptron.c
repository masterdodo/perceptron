#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define WIDTH 100
#define HEIGHT 100

#define PPM_SCALER 10

typedef float Layer[HEIGHT][WIDTH];

static inline int clampi(int x, int low, int high) {
    if (x < low)  x = low;
    if (x > high) x = high;

    return x;
}

void layer_fill_rect(Layer layer, int x, int y, int w, int h, float val) {
    assert(w > 0);
    assert(h > 0);

    int x0 = clampi(x, 0, WIDTH - 1);
    int y0 = clampi(y, 0, HEIGHT - 1);
    int x1 = clampi(x0 + w - 1, 0, WIDTH - 1);
    int y1 = clampi(y0 + h - 1, 0, HEIGHT - 1);

    for (int xidx = x0; xidx <= x1; ++xidx) {
        for (int yidx = y0; yidx <= y1; ++yidx) {
            layer[yidx][xidx] = val;
        }
    }
}

void layer_fill_circle(Layer layer, int cx, int cy, int r, float val) {
    assert(r > 0);

    int x0 = clampi(cx - r, 0, WIDTH - 1);
    int y0 = clampi(cy - r, 0, HEIGHT - 1);
    int x1 = clampi(cx + r, 0, WIDTH - 1);
    int y1 = clampi(cy + r, 0, HEIGHT - 1);

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= r * r) {
                layer[y][x] = val;
            }
        }
    }
}

void layer_save_as_ppm(Layer layer, const char *file_path) {
    float min = layer[0][0];
    float max = layer[0][0];
    for (int y = 0; y < HEIGHT - 1; ++y) {
        for (int x = 0; x < WIDTH - 1; ++x) {
            if (layer[y][x] < min) {
                min = layer[y][x];
            }
            if (layer[y][x] > max) {
                max = layer[y][x];
            }
        }
    }

    FILE *f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: could not open file %s: %s\n", file_path, strerror(errno));
        exit(1);
    }

    fprintf(f, "P6\n%d %d 255\n", WIDTH * PPM_SCALER, HEIGHT * PPM_SCALER);

    for (int y = 0; y < HEIGHT * PPM_SCALER; ++y) {
        for (int x = 0; x < WIDTH * PPM_SCALER; ++x) {
            float scalar = (layer[y / PPM_SCALER][x / PPM_SCALER] - min) / (max - min);
            char pixel[3] = {
                (char)floorf(255 * (1.0f - scalar)),
                (char)floorf(255 * scalar),
                0};

            fwrite(pixel, sizeof(pixel), 1, f);
        }
    }

    fclose(f);
}

void layer_load_bin(Layer layer, const char *file_path) {
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: could not open file %s", file_path);
        exit(1);
    }

    fread(layer, sizeof(Layer), 1, f);
    fclose(f);
}

void layer_save_bin(Layer layer, const char *file_path) {
    FILE *f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: could not open file %s", file_path);
        exit(1);
    }

    fwrite(layer, sizeof(Layer), 1, f);
    fclose(f);
}

float apply_weights(Layer inputs, Layer weights) {
    float output = 0.0f;

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            output += inputs[y][x] * weights[y][x];
        }
    }

    return output;
}

void add_input_to_weights(Layer input, Layer weight) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            weight[y][x] += input[y][x];
        }
    }
}

void sub_input_to_weights(Layer input, Layer weight) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            weight[y][x] -= input[y][x];
        }
    }
}

void layer_random_rect(Layer layer) {
    layer_fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0.0f);

    int w = rand() % (WIDTH - 1) + 1;
    int h = rand() % (HEIGHT - 1) + 1;
    int x = rand() % (WIDTH - w);
    int y = rand() % (HEIGHT - h);

    layer_fill_rect(layer, x, y, w, h, 1.0f);
}

void layer_random_circle(Layer layer) {
    layer_fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0.0f);

    int r = rand() % ((WIDTH / 2) - 1) + 1;
    if (r >= (HEIGHT / 2)) {
        r = rand() % ((HEIGHT / 2) - 1) + 1;
    }
    int cx = rand() % ((WIDTH - r) - r) + r;
    int cy = rand() % ((HEIGHT - r) - r) + r;

    layer_fill_circle(layer, cx, cy, r, 1.0f);
}
