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
#define EPOCHS 5000

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
void initialisation(double *w, int size)
{
    for (int i = 0; i < size; i++)
        w[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
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
    printf("A  B  | Expected  Output\n");
    printf("------|-----------------\n");

    for (int i = 0; i < 4; i++)
    {
        double out;
        Forward(in[i], &out);

        int ans = 0;
        if (out > 0.5) ans = 1;

        printf("%.0f  %.0f  | %.0f         %i\n", 
               in[i][0], in[i][1], expect[i], ans);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        errx(1, "Usage: ./neuralNet [Train|Test|Identify]");

    double inputs[4][2] = {{0,0}, {0,1}, {1,0}, {1,1}};
    double expected[4]  = {1, 0, 0, 1};

    if (!strcmp(argv[1], "Train"))
    {
        printf("Initialising the weights....\n");
        initialisation(hiddenWeights, INPUT_NODES * HIDDEN_NODES);
        initialisation(outputWeights, HIDDEN_NODES * OUTPUT_NODES);
        initialisation(hiddenBias, HIDDEN_NODES);
        initialisation(outputBias, OUTPUT_NODES);

        printf("Training Neural Network...\n\n");
        int epoch;
        double trainAccu = 0;
        for (epoch = 1; epoch < EPOCHS+1; epoch++)
        {
            trainAccu = train(4, inputs, expected);

            if (epoch % 500 == 0)
                printf("void Epoch(%d)\n{\n     Training accuracy: %.1f%%\n}\n\n", epoch, trainAccu * 100);

            if (trainAccu >= 0.9)
                break;
        }
                printf("void Epoch(%d)\n{\n     Training accuracy: %.1f%%\n}\n\n", epoch, trainAccu * 100);

        printf("Training completed!\nTesting Model...\n");
        test(4, inputs, expected);
    }
    else if (!strcmp(argv[1], "Test"))
    {
        printf("Initialising the weights....\n");
        initialisation(hiddenWeights, INPUT_NODES * HIDDEN_NODES);
        initialisation(outputWeights, HIDDEN_NODES * OUTPUT_NODES);
        initialisation(hiddenBias, HIDDEN_NODES);
        initialisation(outputBias, OUTPUT_NODES);

        //TODO
    }
    else if (!strcmp(argv[1], "Identify"))
    {
        printf("Initialising the weights....\n");
        initialisation(hiddenWeights, INPUT_NODES * HIDDEN_NODES);
        initialisation(outputWeights, HIDDEN_NODES * OUTPUT_NODES);
        initialisation(hiddenBias, HIDDEN_NODES);
        initialisation(outputBias, OUTPUT_NODES);

        //TODO
    }
    else 
        errx(1, "Usage: ./neuralNet [Train|Test|Identify]");

    return 0;
}
