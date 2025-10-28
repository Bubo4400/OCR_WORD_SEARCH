#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <err.h>

#define INPUT_NODES 784
#define HIDDEN_NODES 128
#define OUTPUT_NODES 26

#define NUM_TRAINING_IMAGES 124800
#define NUM_TEST_IMAGES 20800
#define NUM_EPOCHS 100
#define LEARNING_RATE 0.01

double (*training_images)[INPUT_NODES];
double (*training_labels)[OUTPUT_NODES];
double (*test_images)[INPUT_NODES];
double (*test_labels)[OUTPUT_NODES];

double (*weight1)[HIDDEN_NODES];
double (*weight2)[OUTPUT_NODES];
double *bias1;
double *bias2;

int correct_predictions;
int forward_prob_output;

void allocate_memory()
{
    /*
     * Allocates space to all needed variables
     */

    training_images = malloc(NUM_TRAINING_IMAGES * sizeof(*training_images));
    training_labels = malloc(NUM_TRAINING_IMAGES * sizeof(*training_labels));
    test_images = malloc(NUM_TEST_IMAGES * sizeof(*test_images));
    test_labels = malloc(NUM_TEST_IMAGES * sizeof(*test_labels));

    weight1 = malloc(INPUT_NODES * sizeof(*weight1));
    weight2 = malloc(HIDDEN_NODES * sizeof(*weight2));
    bias1 = malloc(HIDDEN_NODES * sizeof(*bias1));
    bias2 = malloc(OUTPUT_NODES * sizeof(*bias2));

    if (!training_images || !training_labels || !test_images || !test_labels ||
        !weight1 || !weight2 || !bias1 || !bias2)
        errx(EXIT_FAILURE, "Memory allocation failed\n");
}

void free_memory()
{
    /*
     * Free all of the allocated memory
     */

    free(training_images);
    free(training_labels);
    free(test_images);
    free(test_labels);
    free(weight1);
    free(weight2);
    free(bias1);
    free(bias2);
}

char label_to_letter(int label)
{
    // Gives the letter found
    return 'A' + label;
}

double *load_letter(char* filename)
{
    /*
     * Loads the letter that is going to be regonised.
     */

   FILE *image = fopen(filename, "rb");

   if (!image)
       errx(1, "Could not open the file.");

    unsigned char header[16];
    fread(header, 1, 16, image);

    double *pixels = malloc(INPUT_NODES * sizeof(double));
    if (!pixels)
        errx(1, "Failed to allocate memory.");

    for (int j = 0; j < INPUT_NODES; j++)
    {
        unsigned char pixel;
        fread(&pixel, sizeof(unsigned char), 1, image);
        pixels[j] = pixel / 255.0;
    }

    fclose(image);
    return pixels;
}

void shuffle_training_data(int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        // Swap images (each is an array of INPUT_NODES)
        for (int k = 0; k < INPUT_NODES; k++)
        {
            double tmp = training_images[i][k];
            training_images[i][k] = training_images[j][k];
            training_images[j][k] = tmp;
        }

        // Swap labels (one-hot arrays of OUTPUT_NODES)
        for (int k = 0; k < OUTPUT_NODES; k++)
        {
            double tmp = training_labels[i][k];
            training_labels[i][k] = training_labels[j][k];
            training_labels[j][k] = tmp;
        }
    }
}

void load_emnist_letters()
{
    /*
     * Loads all of the emnist files in the emnistSet folder to be able to test or train the neural network.
     * Then loads them into the desired variables.
     */

    FILE *train_images_file = fopen("emnistSet/emnist-train-images", "rb");
    FILE *train_labels_file = fopen("emnistSet/emnist-train-labels", "rb");
    FILE *test_images_file  = fopen("emnistSet/emnist-test-images", "rb");
    FILE *test_labels_file  = fopen("emnistSet/emnist-test-labels", "rb");

    if (!train_images_file || !train_labels_file || !test_images_file 
            || !test_labels_file)
        errx(1, "Error: Could not open EMNIST files.\n");

    // Skip headers
    unsigned char header[16];
    fread(header, 1, 16, train_images_file);
    fread(header, 1, 8,  train_labels_file);
    fread(header, 1, 16, test_images_file);
    fread(header, 1, 8,  test_labels_file);

    // Load training data
    for (int i = 0; i < NUM_TRAINING_IMAGES; i++)
    {
        for (int j = 0; j < INPUT_NODES; j++)
        {
            unsigned char pixel;
            fread(&pixel, sizeof(unsigned char), 1, train_images_file);
            training_images[i][j] = pixel / 255.0;
        }

        unsigned char label;
        fread(&label, sizeof(unsigned char), 1, train_labels_file);
        int index = label - 1;
        for (int j = 0; j < OUTPUT_NODES; j++)
            training_labels[i][j] = (j == index) ? 1.0 : 0.0;
    }

    // Load test data
    for (int i = 0; i < NUM_TEST_IMAGES; i++)
    {
        for (int j = 0; j < INPUT_NODES; j++)
        {
            unsigned char pixel;
            fread(&pixel, sizeof(unsigned char), 1, test_images_file);
            test_images[i][j] = pixel / 255.0;
        }

        unsigned char label;
        fread(&label, sizeof(unsigned char), 1, test_labels_file);
        int index = label - 1;
        for (int j = 0; j < OUTPUT_NODES; j++)
            test_labels[i][j] = (j == index) ? 1.0 : 0.0;
    }

    fclose(train_images_file);
    fclose(train_labels_file);
    fclose(test_images_file);
    fclose(test_labels_file);

    printf("Loaded EMNIST Letters dataset.\n");
}

double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative(double x)
{
    return x * (1.0 - x);
}

int max_index(double arr[], int size)
{
    int max_i = 0;
    for (int i = 1; i < size; i++)
    {
        if (arr[i] > arr[max_i])
            max_i = i;
    }
    return max_i;
}

void train(double input[INPUT_NODES], double target[OUTPUT_NODES], int correct)
{
    /*
     * Trains the network using the emnist dataset.
     */

    double hidden[HIDDEN_NODES];
    double output[OUTPUT_NODES];

    // Forward pass
    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        double sum = bias1[i];
        for (int j = 0; j < INPUT_NODES; j++)
            sum += input[j] * weight1[j][i];
        hidden[i] = sigmoid(sum);
    }

    for (int i = 0; i < OUTPUT_NODES; i++)
    {
        double sum = bias2[i];
        for (int j = 0; j < HIDDEN_NODES; j++)
            sum += hidden[j] * weight2[j][i];

        output[i] = sigmoid(sum);
    }

    // Track accuracy
    int predicted = max_index(output, OUTPUT_NODES);
    if (predicted == correct)
        forward_prob_output++;

    // Backpropagation
    double output_error[OUTPUT_NODES];
    double output_delta[OUTPUT_NODES];

    for (int i = 0; i < OUTPUT_NODES; i++)
    {
        output_error[i] = target[i] - output[i];
        output_delta[i] = output_error[i] * sigmoid_derivative(output[i]);
    }

    double hidden_error[HIDDEN_NODES];
    double hidden_delta[HIDDEN_NODES];

    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        hidden_error[i] = 0.0;
        for (int j = 0; j < OUTPUT_NODES; j++)
            hidden_error[i] += output_delta[j] * weight2[i][j];

        hidden_delta[i] = hidden_error[i] * sigmoid_derivative(hidden[i]);
    }

    // Update weights and biases
    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        for (int j = 0; j < OUTPUT_NODES; j++)
            weight2[i][j] += LEARNING_RATE * output_delta[j] * hidden[i];
    }

    for (int i = 0; i < INPUT_NODES; i++)
    {
        for (int j = 0; j < HIDDEN_NODES; j++)
            weight1[i][j] += LEARNING_RATE * hidden_delta[j] * input[i];
    }

    for (int i = 0; i < OUTPUT_NODES; i++)
        bias2[i] += LEARNING_RATE * output_delta[i];

    for (int i = 0; i < HIDDEN_NODES; i++)
        bias1[i] += LEARNING_RATE * hidden_delta[i];
}

void test(double input[INPUT_NODES], int correct_label)
{
    /*
     * Tests the neural network with the emnist dataset.
     */

    double hidden[HIDDEN_NODES];
    double output[OUTPUT_NODES];

    // Forward pass
    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        double sum = bias1[i];
        for (int j = 0; j < INPUT_NODES; j++)
            sum += input[j] * weight1[j][i];

        hidden[i] = sigmoid(sum);
    }

    for (int i = 0; i < OUTPUT_NODES; i++)
    {
        double sum = bias2[i];
        for (int j = 0; j < HIDDEN_NODES; j++)
            sum += hidden[j] * weight2[j][i];

        output[i] = sigmoid(sum);
    }

    int predicted = max_index(output, OUTPUT_NODES);

    if (predicted == correct_label)
        correct_predictions++;
}

int recognition(double input[INPUT_NODES])
{
    /*
     * Regonise the given letter.
     */

    double hidden[HIDDEN_NODES];
    double output[OUTPUT_NODES];

    // Forward pass
    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        double sum = bias1[i];
        for (int j = 0; j < INPUT_NODES; j++)
            sum += input[j] * weight1[j][i];

        hidden[i] = sigmoid(sum);
    }

    for (int i = 0; i < OUTPUT_NODES; i++)
    {
        double sum = bias2[i];
        for (int j = 0; j < HIDDEN_NODES; j++)
            sum += hidden[j] * weight2[j][i];

        output[i] = sigmoid(sum);
    }

    int predicted = max_index(output, OUTPUT_NODES);
    return predicted;
}

void initialize_weights()
{
    /*
     * Initialize weights to random values.
     * Executed even if there are already existing weights but they get overriden.
     */

    srand(time(NULL));
    for (int i = 0; i < INPUT_NODES; i++)
    {
        for (int j = 0; j < HIDDEN_NODES; j++)
            weight1[i][j] = ((double)rand() / RAND_MAX) - 0.5;
    }

    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        bias1[i] = ((double)rand() / RAND_MAX) - 0.5;
        for (int j = 0; j < OUTPUT_NODES; j++)
            weight2[i][j] = ((double)rand() / RAND_MAX) - 0.5;
    }

    for (int i = 0; i < OUTPUT_NODES; i++)
        bias2[i] = ((double)rand() / RAND_MAX) - 0.5;
}

void save_weights_biases(char* filename)
{
    /*
     * Saves weights to the file in input.
     * Used after every epoch of the training.
     */

    FILE* file = fopen(filename, "wb");
    if (!file)
    {
        printf("Error opening file for writing\n");
        return;
    }

    fwrite(weight1, sizeof(double), INPUT_NODES * HIDDEN_NODES, file);
    fwrite(weight2, sizeof(double), HIDDEN_NODES * OUTPUT_NODES, file);
    fwrite(bias1, sizeof(double), HIDDEN_NODES, file);
    fwrite(bias2, sizeof(double), OUTPUT_NODES, file);

    fclose(file);
    printf("Model saved to %s\n", filename);
}

void load_weights_biases(char* filename)
{
    /*
     * Loads the weights from the file given in the input.
     * If the file does not exist it exits this function.
     */

    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        printf("No existing model found. Starting fresh.\n");
        initialize_weights();
        return;
    }

    fread(weight1, sizeof(double), INPUT_NODES * HIDDEN_NODES, file);
    fread(weight2, sizeof(double), HIDDEN_NODES * OUTPUT_NODES, file);
    fread(bias1, sizeof(double), HIDDEN_NODES, file);
    fread(bias2, sizeof(double), OUTPUT_NODES, file);

    fclose(file);
    printf("Model loaded from %s\n", filename);
}

int main(int argc, char *argv[])
{
    /*
     * Excutes everything and check if the given argument is Train|Test|What.
     */

    if (argc < 2)
        errx(EXIT_FAILURE, "Usage: %s <Train|Test>", argv[0]);

    allocate_memory();

    // Excutes Training.
    if (!strcmp(argv[1], "Train"))
    {
        printf("Loading EMNIST letter dataset...\n");
        load_emnist_letters();

        printf("Initializing weights...\n");
        load_weights_biases("letter_model.bin");

        correct_predictions = 0;
        for (int i = 0; i < NUM_TEST_IMAGES; i++)
        {
            int correct_label = max_index(test_labels[i], OUTPUT_NODES);
            test(test_images[i], correct_label);
        }
        double start_accuracy = (double)correct_predictions / NUM_TEST_IMAGES;
        printf("\nCurrent start accuracy: %.4f%%\n", start_accuracy*100);

        printf("Training network for letter recognition...\n");
        double best_test_acc = start_accuracy;
        for (int epoch = 0; epoch < NUM_EPOCHS; epoch++)
        {
            forward_prob_output = 0;
            double epoch_mse = 0.0;

            // Shuffle training data each epoch
            shuffle_training_data(NUM_TRAINING_IMAGES);

            for (int i = 0; i < NUM_TRAINING_IMAGES; i++)
            {
                int correct_label = max_index(training_labels[i], OUTPUT_NODES);

                double hidden[HIDDEN_NODES];
                double output[OUTPUT_NODES];

                // Forward pass
                for (int h = 0; h < HIDDEN_NODES; h++)
                {
                    double sum = bias1[h];
                    for (int j = 0; j < INPUT_NODES; j++)
                        sum += training_images[i][j] * weight1[j][h];
                    hidden[h] = sigmoid(sum);
                }
                for (int o = 0; o < OUTPUT_NODES; o++)
                {
                    double sum = bias2[o];
                    for (int j = 0; j < HIDDEN_NODES; j++)
                        sum += hidden[j] * weight2[j][o];
                    output[o] = sigmoid(sum);
                }

                for (int o = 0; o < OUTPUT_NODES; o++)
                {
                    double diff = training_labels[i][o] - output[o];
                    epoch_mse += diff * diff;
                }

                train(training_images[i], training_labels[i], correct_label);
            }

            epoch_mse /= (NUM_TRAINING_IMAGES * OUTPUT_NODES);

            correct_predictions = 0;
            for (int i = 0; i < NUM_TEST_IMAGES; i++)
            {
                int correct_label = max_index(test_labels[i], OUTPUT_NODES);
                test(test_images[i], correct_label);
            }
            double test_accuracy = (double)correct_predictions / NUM_TEST_IMAGES;

            double train_accuracy = (double)forward_prob_output / NUM_TRAINING_IMAGES;
            printf("Epoch %d: Train Acc = %.4f%%, Test Acc = %.4f%%, MSE = %.6f\n",
                   epoch + 1, train_accuracy * 100.0, test_accuracy * 100.0, epoch_mse);

            // Save best model only
            if (test_accuracy > best_test_acc)
            {
                best_test_acc = test_accuracy;
                save_weights_biases("letter_model.bin");
                printf(" -> New best model saved (%.4f%%)\n", best_test_acc * 100.0);
            }

            printf("\n");
        }


        // Test the trained model.
        correct_predictions = 0;
        for (int i = 0; i < NUM_TEST_IMAGES; i++) 
        {
            int correct_label = max_index(test_labels[i], OUTPUT_NODES);
            test(test_images[i], correct_label);
        }

        double test_accuracy = (double)correct_predictions / NUM_TEST_IMAGES;
        printf("Final Test Accuracy: %.4f%%\n", test_accuracy*100);
        printf("Improvement of : %.4f%%\n", (test_accuracy-start_accuracy)*100);
    }

    // Excutes Testing.
    else if (!strcmp(argv[1], "Test"))
    {
        printf("Loading EMNIST letter dataset...\n");
        load_emnist_letters();

        printf("Loading model...\n");
        load_weights_biases("letter_model.bin");

        correct_predictions = 0;
        for (int i = 0; i < NUM_TEST_IMAGES; i++)
        {
            int correct_label = max_index(test_labels[i], OUTPUT_NODES);
            test(test_images[i], correct_label);
        }

        double test_accuracy = (double)correct_predictions / NUM_TEST_IMAGES;
        printf("Test Accuracy: %.4f%%\n", test_accuracy*100);
    }

    // Excutes recognition.
    else if (!strcmp(argv[1], "What"))
    {
        printf("Loading model...\n");
        load_weights_biases("letter_model.bin");

        printf("Loading given letter...\n");
        double *image = load_letter(argv[2]);

        printf("Getting prediction...\n");
        int predicted = recognition(image);
        printf("The letter is %c\n", predicted + 'A');
        free(image);
    }

    // If argument is not regonised.
    else
        printf("Unknown command. Use 'Train' or 'Test' or 'What'\n");

    free_memory();
    return 0;
}
