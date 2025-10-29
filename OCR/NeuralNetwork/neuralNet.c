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
#define EPOCHS 50000

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

double sigDerivation(double x)
{
    return x * (1.0 - x);
}

void initialisation(double *w, int size)
{
    for (int i = 0; i < size; i++)
        w[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
}

void feedForward(double input[INPUT_NODES], double *out)
{
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

double train(int s, double in[s][INPUT_NODES], double expect[s])
{
    double correct = 0;

    for (int i = 0; i < s; i++)
    {
        double out;
        feedForward(in[i], &out);

        // Calculate accuracy
        int predicted = out > 0.5 ? 1 : 0;
        int actual = expect[i] > 0.5 ? 1 : 0;
        if (predicted == actual) correct++;

        // Backpropagation
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

void test(int s, double in[s][INPUT_NODES], double expect[s])
{
    printf("\nTesting XNOR Neural Network:\n");
    printf("A  B  | Expected  Output\n");
    printf("------|-----------------\n");

    for (int i = 0; i < 4; i++)
    {
        double out;
        feedForward(in[i], &out);

        int ans = 0;
        if (out > 0.5) ans = 1;

        printf("%.0f  %.0f  | %.0f         %i\n", 
               in[i][0], in[i][1], expect[i], ans);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        errx(1, "Usage: %s Train", argv[0]);

    double inputs[4][2] = {{0,0}, {0,1}, {1,0}, {1,1}};
    double expected[4]  = {1, 0, 0, 1};

    printf("Initialising the weights....\n");
    initialisation(hiddenWeights, INPUT_NODES * HIDDEN_NODES);
    initialisation(outputWeights, HIDDEN_NODES * OUTPUT_NODES);
    initialisation(hiddenBias, HIDDEN_NODES);
    initialisation(outputBias, OUTPUT_NODES);

    if (!strcmp(argv[1], "Train"))
    {
        printf("Training Neural Network...\n\n");
        for (int epoch = 1; epoch < EPOCHS+1; epoch++)
        {
            double train_accu = train(4, inputs, expected);

            if (epoch % 500 == 0)
            {
                printf("void Epoch(%d)\n{\n     Training accuracy: %.1f%%\n}\n\n", epoch, train_accu * 100);
            }
        }

        printf("\nTraining completed!\n");
        test(4, inputs, expected);
    }

    return 0;
}
