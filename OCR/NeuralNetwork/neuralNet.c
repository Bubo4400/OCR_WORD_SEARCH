#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <err.h>

// ---------------------- Constants ----------------------

#define INPUT_NODES 2
#define HIDDEN_NODES 2
#define OUTPUT_NODES 1

#define LEARNING_RATE 0.25
#define EPOCHS 2000

// ----------------------- Global -----------------------

double hiddenWeights[INPUT_NODES * HIDDEN_NODES];
double outputWeights[HIDDEN_NODES * OUTPUT_NODES];
double hiddenLayer[HIDDEN_NODES];
double hiddenBias[HIDDEN_NODES];
double outputBias[OUTPUT_NODES];

// -------------------- Neural Network --------------------

double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

// Derivative of the sigmoid
double sigDerivation(double x)
{
    return x * (1.0 - x);
}

// Initialisation of weights and bias 
// Only called if no file exists
void initialisation(double *w, int size)
{
    for (int i = 0; i < size; i++)
        w[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
}

// Save Weights and Bias into file given in argument
void saveInformation(char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
        errx(1, "Could not save Information to \"%s\".\n", filename);

    fwrite(hiddenWeights, sizeof(double), INPUT_NODES * HIDDEN_NODES, file);
    fwrite(outputWeights, sizeof(double), HIDDEN_NODES * OUTPUT_NODES, file);
    fwrite(hiddenBias, sizeof(double), HIDDEN_NODES, file);
    fwrite(outputBias, sizeof(double), OUTPUT_NODES, file);

    fclose(file);
    printf("Succesfully saved information to \"%s\".\n", filename);
}

// Loads weigths and bias from file given in argument
// If the file does not exist, it initialises them 
void loadInformation(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        printf("The file \"%s\" does not exist.\n", filename);
        printf("Initialising random weights.\n");
        initialisation(hiddenWeights, INPUT_NODES * HIDDEN_NODES);
        initialisation(outputWeights, HIDDEN_NODES * OUTPUT_NODES);
        initialisation(hiddenBias, HIDDEN_NODES);
        initialisation(outputBias, OUTPUT_NODES);
        return;
    }

    fread(hiddenWeights, sizeof(double), INPUT_NODES * HIDDEN_NODES, file);
    fread(outputWeights, sizeof(double), HIDDEN_NODES * OUTPUT_NODES, file);
    fread(hiddenBias, sizeof(double), HIDDEN_NODES, file);
    fread(outputBias, sizeof(double), OUTPUT_NODES, file);

    fclose(file);
    printf("Succesfully loaded information from \"%s\".\n", filename);
}

// Gives prediction of what it is
void Forward(double input[INPUT_NODES], double *out)
{
    // Calculate value od each node
    for (int i = 0; i < HIDDEN_NODES; i++)
    {
        hiddenLayer[i] = 0;
        for (int j = 0; j < INPUT_NODES; j++)
            hiddenLayer[i] += input[j] * hiddenWeights[j * HIDDEN_NODES + i];
        hiddenLayer[i] += hiddenBias[i];
        hiddenLayer[i] = sigmoid(hiddenLayer[i]);
    }

    *out = 0;
    for (int i = 0; i < HIDDEN_NODES; i++)
        *out += hiddenLayer[i] * outputWeights[i];
    *out += outputBias[0];
    *out = sigmoid(*out);
}

// Train for one Epoch by calculating and ajusting weigths and bias accordingly
double train(int s, double in[s][INPUT_NODES], double expect[s])
{
    double correct = 0;

    for (int i = 0; i < s; i++)
    {
        double out;
        Forward(in[i], &out);

        int predicted = out > 0.5 ? 1 : 0;
        int actual = expect[i] > 0.5 ? 1 : 0;
        if (predicted == actual) correct++;

        double error = expect[i] - out;
        double delta = error * sigDerivation(out);

        // Update output weights and bias
        for (int j = 0; j < HIDDEN_NODES; j++)
            outputWeights[j] += LEARNING_RATE * delta * hiddenLayer[j];
        outputBias[0] += LEARNING_RATE * delta;

        // Update hidden weights and biases
        for (int k = 0; k < HIDDEN_NODES; k++)
        {
            double hiddenError = delta * outputWeights[k];
            double hiddenDelta = hiddenError * sigDerivation(hiddenLayer[k]);

            for (int l = 0; l < INPUT_NODES; l++)
            {
                int pos = l * HIDDEN_NODES + k;
                hiddenWeights[pos] += LEARNING_RATE * hiddenDelta * in[i][l];
            }
            hiddenBias[k] += LEARNING_RATE * hiddenDelta;
        }
    }

    return correct / s;
}

// Tests the Neural Network
void test(int s, double in[s][INPUT_NODES], double expect[s])
{
    printf("\tA  B  | Expected | Output | Out\n");
    printf("\t------|----------|--------|--------\n");

    for (int i = 0; i < 4; i++)
    {
        double out;
        Forward(in[i], &out);

        int ans = 0;
        if (out > 0.5) ans = 1;

        printf("\t%.0f  %.0f  | %.0f        | %i      | %.4f\n", 
               in[i][0], in[i][1], expect[i], ans, out);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        errx(1, "Usage: ./neuralNet <Train|Test|Identify A B>.");

    double inputs[4][2] = {{0,0}, {0,1}, {1,0}, {1,1}};
    double expected[4]  = {1, 0, 0, 1};

    if (!strcmp(argv[1], "Train"))
    {
        printf("Loading the weights....\n");
        loadInformation("trainingData.bin");
        
        printf("Training Neural Network...\n\n");
        int epoch;
        double trainAccu = 0;
        for (epoch = 1; epoch < EPOCHS; epoch++)
        {
            trainAccu = train(4, inputs, expected);

            if (epoch % 500 == 0)
            {
                printf("Epoch(%d)\n"
                        "{\n"
                        "\tTraining accuracy: %.1f%%\n"
                        "\tTest:\n", epoch, trainAccu * 100);
                test(4, inputs, expected);
                printf("}\n\n");
            }
        }
        printf("Epoch(%d)\n"
                "{\n"
                "\tTraining accuracy: %.1f%%\n"
                "\tTest:\n", epoch, trainAccu * 100);
        test(4, inputs, expected);
        printf("}\n\n");

        printf("Training completed!\n");
        saveInformation("trainingData.bin");
    }
    else if (!strcmp(argv[1], "Test"))
    {
        printf("Loading the weights....\n");
        loadInformation("trainingData.bin");
        printf("\n");
        
        printf("Test()\n{\n");
        test(4, inputs, expected);
        printf("}\n\n");
    }
    else if (!strcmp(argv[1], "Identify"))
    {
        if (argc != 4)
            errx(1, "Usage: ./neuralNet <Train|Test|Identify A B>.");
        printf("Initialising the weights....\n");
        loadInformation("trainingData.bin");
        printf("Thinking...\n\n");

        double out;
        int A = atoi(argv[2]);
        int B = atoi(argv[3]);
        double input[] = {A, B};
        Forward(input, &out);
        int ans = 0;
        if (out > 0.5) ans = 1;
        
        printf("Answer()\n{\n");
        printf("\tA  B  | Output | Out\n");
        printf("\t------|--------|--------\n");
        printf("\t%i  %i  | %i      | %.4f\n",
               A, B, ans, out);
        printf("}\n");
    }
    else 
        errx(1, "Usage: ./neuralNet <Train|Test|Identify A B>.");

    return 0;
}
