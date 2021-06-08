#include <mlpack/core.hpp>
#include <mlpack/core/data/split_data.hpp>

#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/ann/ffn.hpp>

#include <ensmallen.hpp>  /* The numerical optimization library that mlpack uses */

using namespace mlpack;
using namespace mlpack::ann;

// Namespace for the armadillo library(linear algebra library).
using namespace arma;
using namespace std;

// Namespace for ensmallen.
using namespace ens;

//
arma::Row<size_t> getLabels(arma::mat predOut)
{
  arma::Row<size_t> predLabels(predOut.n_cols);
  for (arma::uword i = 0; i < predOut.n_cols; ++i)
  {
    predLabels(i) = predOut.col(i).index_max();
  }
  return predLabels;
}

int main(){
    std::cout << "Building FNN - convolutional neural network" << std::endl;
    /* setup constant training parameters */
    constexpr double RATIO = 0.1; // train set to validation set
    constexpr int MAX_ITERATIONS = 0; // set to zero to allow infinite iterations.
    constexpr double STEP_SIZE = 1.2e-3;// optimizer steps
    constexpr int BATCH_SIZE = 50;
    constexpr size_t EPOCH = 2;

    cout << "Reading VIsoModel data ..." << endl;

    mat mixed, isolated;
    data::Load("../data/train.csv",mixed, true);
    data::Load("../data/verif.csv",isolated, true);

    mat mTrain, iTrain, mValid, iValid;
    data::Split(mixed, mTrain, mValid, RATIO);
    data::Split(isolated, iTrain, iValid, RATIO);

    const mat trainX = mTrain.submat(0, 1, mTrain.n_rows - 1, mTrain.n_cols - 1);
    const mat validX = mValid.submat(0, 1, mValid.n_rows - 1, mValid.n_cols - 1);

    std::cout << trainX.n_rows << " " << trainX.n_cols << std::endl;
    //          ^^^ csv col ^^^         ^^^ csv row ^^^

    int m_csv_col = trainX.n_rows;
    int m_csv_row = trainX.n_cols;

    // // for(int i = 511;i<1044;i++){
    // //     std::cout << i << ": " << mixed[i] << std::endl;
    // // }
    // std::cout << 12824 << ": " << mixed[12824] << std::endl;

    std::cout << validX.n_rows << " " << validX.n_cols << std::endl;
    //          ^^^ csv col ^^^         ^^^ csv row ^^^

    const mat trainY = iTrain.submat(0, 1, iTrain.n_rows - 1, iTrain.n_cols - 1);
    const mat validY = iValid.submat(0, 1, iValid.n_rows - 1, iValid.n_cols - 1);

    // for(int i = 0;i<25;i++){
    //   std::cout << i << ": " << trainX[i] << " | " << trainY[i] << std::endl;
    // }

    std::cout << trainY.n_rows << " " << trainY.n_cols << std::endl;
    //          ^^^ csv col ^^^         ^^^ csv row ^^^


    std::cout << validY.n_rows << " " << validY.n_cols << std::endl;
    //          ^^^ csv col ^^^         ^^^ csv row ^^^

    int i_csv_col = trainY.n_rows;
    int i_csv_row = trainY.n_cols;

    // for(int i = 512;i<520;i++){
    //     std::cout << i << ": " << isolated[i] << std::endl;
    // }

    // for(int j = 0;j<csv_row;j++){
    //   std::cout << trainY[j] << ", ";
    // }
    // std::cout << "\n";
    //
    // for(int i = 0;i<csv_row;i++){
    //   for(int j = 0;j<csv_col;j++){
    //     std::cout << trainX[i*csv_col + j] << ", ";
    //   }
    //   std::cout << "\n";
    // }


    Sequential<> module;

    module.Add<IdentityLayer<>>();

    /* First Convolution Layer */
    module.Add<Convolution<>>(1,   // Number of input activation maps.
                             32,  // Number of output activation maps.
                             3,   // Filter width.
                             3,   // Filter height.
                             1,   // Stride along width.
                             1,   // Stride along height.
                             0,   // Padding width.
                             0,   // Padding height.
                             513, // Input width.
                             25   // Input height.
    );

    /* First ReLU */
    module.Add<LeakyReLU<>>();

    module.Add<IdentityLayer<>>();

    /* Second Convolution Layer */
    module.Add<Convolution<>>(32,   // Number of input activation maps.
                             16,  // Number of output activation maps.
                             3,   // Filter width.
                             3,   // Filter height.
                             1,   // Stride along width.
                             1,   // Stride along height.
                             0,   // Padding width.
                             0,   // Padding height.
                             513, // Input width.
                             25   // Input height.
    );

    /* Second ReLU */
    module.Add<LeakyReLU<>>();

    /* First Max Pooling */
    module.Add<MaxPooling<>>(3, 3, 3, 3, true);

    /* Dropout */
    module.Add<Dropout<>>(0.25);

    module.Add<IdentityLayer<>>();

    /* Third Convolution Layer */
    module.Add<Convolution<>>(16,   // Number of input activation maps.
                             64,  // Number of output activation maps.
                             3,   // Filter width.
                             3,   // Filter height.
                             1,   // Stride along width.
                             1,   // Stride along height.
                             0,   // Padding width.
                             0,   // Padding height.
                             171, // Input width.
                             8    // Input height.
    );

    /* Third  ReLU */
    module.Add<LeakyReLU<>>();

    module.Add<IdentityLayer<>>();

    /* Fourth Convolution Layer */
    module.Add<Convolution<>>(64,   // Number of input activation maps.
                             16,  // Number of output activation maps.
                             3,   // Filter width.
                             3,   // Filter height.
                             1,   // Stride along width.
                             1,   // Stride along height.
                             0,   // Padding width.
                             0,   // Padding height.
                             171, // Input width.
                             8    // Input height.
    );

    /* Fourth  ReLU */
    module.Add<LeakyReLU<>>();

    /* Second Max Pooling */
    module.Add<MaxPooling<>>(3, 3, 3, 3, true);

    /* Dropout */
    module.Add<Dropout<>>(0.25);

    /* Flatten */
    // model.Add<Flatten<>>(); // Not a layer in mlpack
    module.Add<Subview<>>();


    /* First Dense */
    module.Add<Linear<>>(1824, 128);

    /* Fifth LeakyReLU */
    module.Add<LeakyReLU<>>();

    /* Dropout */
    module.Add<Dropout<>>(0.5);

    /* Final Dense */
    module.Add<Linear<>>(128, 513);
    module.Add<LogSoftMax<>>(); // RM -> seg fault still happens




    // Specify the NN model. NegativeLogLikelihood is the output layer that
    // is used for classification problem. RandomInitialization means that
    // initial weights are generated randomly in the interval from -1 to 1.
    FFN<NegativeLogLikelihood<>, RandomInitialization> model;

    /* regression-classification model to classify pizels as either vocal or non-vocal */
    // Layers schema source: https://towardsdatascience.com/audio-ai-isolating-vocals-from-stereo-music-using-convolutional-neural-networks-210532383785

    // 513x25x01 --- conv(32  filters of size 3x3. stride = 1) ---> 513x25x32
    // 513x25x32 ------------------ LeakyReLU --------------------> 513x25x32

    // 513x25x32 --- conv(16  filters of size 3x3. stride = 1) ---> 513x25x16
    // 513x25x16 ------------------ LeakyReLU --------------------> 513x25x16

    // 513x25x16 --- max pooling* (over 3x3 fields, stride = 3) --> 171x08x16
    // 171x08x16 ------------------- dropout ---------------------> 171x08x16

    // 171x08x16 ---- conv(64 filters of size 3x3, stride = 1) ---> 171x08x64
    // 171x08x64 ------------------ LeakyReLU --------------------> 171x08x64

    // 171x08x64 ---- conv(16 filters of size 3x3, stride = 1) ---> 171x08x16
    // 171x08x16 ------------------ LeakyReLU ------------------- > 171x08x16

    // 171x08x16 ---- max pooling (over 3x3 fields, stride = 3) --> 057x02x16
    // 057x02x16 ------------------- dropout ---------------------> 057x02x16
    // 057x02x16 ------------------- flatten --------------------->      1824
    //      1824 ------------------- dense** --------------------->       128
    //       128 ------------------ LeakyReLU -------------------->       128
    //       128 ------------------- dropout --------------------->       128
    //       128 ------------------- dense** --------------------->       513

    // Note (*)
        // from Keras, unspecified stride defaults to pool size
        // i.e. height and width == 3

    // Note (**)
        // from Keras, Dense(1, activation = 'sigmoid')
        // "Generally, we use softmax activation instead of sigmoid with
        // the cross-entropy loss because softmax activation distributes
        // the probability throughout each output node.But, since it is
        // a binary classification, using sigmoid is same as softmax.
        // For multi-class classification use sofmax with cross-entropy."
        //
        //    https://medium.com/aidevnepal/for-sigmoid-funcion-f7a5da78fec2
        //
        // see mnist_cnn example for dense layer - add linear
        //    use LogSoftMax for 'sigmoid'





    cout << "Start training ..." << endl;

    // Set parameters for the Adam optimizer.
    ens::Adam optimizer(
        STEP_SIZE,  // Step size of the optimizer.
        BATCH_SIZE, // Batch size. Number of data points that are used in each
                    // iteration.
        0.9,        // Exponential decay rate for the first moment estimates.
        0.999, // Exponential decay rate for the weighted infinity norm estimates.
        1e-8,  // Value used to initialise the mean squared gradient parameter.
        MAX_ITERATIONS, // Max number of iterations.
        1e-8,           // Tolerance.
        true);

    model.Train(trainX,
            trainY,
            optimizer,
            ens::PrintLoss(),
            ens::ProgressBar(),
            // Stop the training using Early Stop at min loss.
            ens::EarlyStopAtMinLoss(
                [&](const arma::mat& /* param */)
                {
                  double validationLoss = model.Evaluate(validX, validY);
                  std::cout << "Validation loss: " << validationLoss
                      << "." << std::endl;
                  return validationLoss;
                })
          );



  return 0;
}
