#include <stdio.h>
#include <stdlib.h>
#include "perceptron.h"

#define PERCEPTRON_MODE 0 // 0 - TEST local weights; 1 - TRAIN & TEST

#define TRAIN_SEED 123
#define TEST_SEED 1234

#define SAMPLE_SIZE 500
#define BIAS 6.0

int train_iteration(Layer input, Layer weights_layer) {
    int adjustments = 0;

    for (int i = 0; i < SAMPLE_SIZE; ++i) {
        layer_random_rect(input);
        // got excited if 1 on rectangle
        if (apply_weights(input, weights_layer) > BIAS) {
            sub_input_to_weights(input, weights_layer);
            adjustments++;
        }

        layer_random_circle(input);
        // got supressed if 0 on circle
        if (apply_weights(input, weights_layer) < BIAS) {
            add_input_to_weights(input, weights_layer);
            adjustments++;
        }
    }

    return adjustments;
}

int test_iteration(Layer input, Layer weights_layer) {
    int adjustments = 0;

    for (int i = 0; i < SAMPLE_SIZE; ++i) {
        layer_random_rect(input);
        if (apply_weights(input, weights_layer) > BIAS) {
            adjustments++;
        }

        layer_random_circle(input);
        if (apply_weights(input, weights_layer) < BIAS) {
            adjustments++;
        }
    }

    return adjustments;
}

static Layer input;
static Layer weights;

void load_weights(const char *file_path) {
    layer_load_bin(weights, file_path);
}

void perceptron_test() {
    load_weights("weights.bin");

    srand(TEST_SEED);
    int failed_predictions = test_iteration(input, weights);
    int correct_predictions = (SAMPLE_SIZE * 2) - failed_predictions;
    int all_predictions = (SAMPLE_SIZE * 2);
    float accuracy = correct_predictions / (all_predictions * 1.0);
    printf("Accuracy - Loaded weights (bin): %d/%d - %.2f %%\n", 
        correct_predictions, 
        all_predictions, 
        accuracy * 100.0);
}

void perceptron_train(bool save_weights) {
    srand(TEST_SEED);
    int failed_predictions = test_iteration(input, weights);
    int correct_predictions = (SAMPLE_SIZE * 2) - failed_predictions;
    int all_predictions = (SAMPLE_SIZE * 2);
    float accuracy = correct_predictions / (all_predictions * 1.0);
    printf("Accuracy - Untrained model (Law of Large Numbers - Proof): %d/%d - %.2f %%\n", 
        correct_predictions, 
        all_predictions, 
        accuracy * 100.0);

    do {
        srand(TRAIN_SEED);
    } 
    while (train_iteration(input, weights) > 0);

    if (save_weights) {
        layer_save_as_ppm(weights, "weights.ppm");
        layer_save_bin(weights, "weights.bin");
    }

    srand(TEST_SEED);
    failed_predictions = test_iteration(input, weights);
    correct_predictions = (SAMPLE_SIZE * 2) - failed_predictions;
    accuracy = correct_predictions / (all_predictions * 1.0);
    printf("Accuracy - Trained model: %d/%d - %.2f %%\n", correct_predictions, all_predictions, accuracy * 100.0);
}

int main() {
    if (PERCEPTRON_MODE == 0) {
        perceptron_test();
    }
    else {
        bool save_trained_weights = true;
        perceptron_train(save_trained_weights);
    }

    return 0;
}
