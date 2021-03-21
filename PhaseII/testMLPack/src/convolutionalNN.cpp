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

    /* setup constant training parameters */
    constexpr double RATIO = 0.1; // train set to validation set
    constexpr int MAX_ITERATIONS = 0; // set to zero to allow infinite iterations.
    constexpr double STEP_SIZE = 1.2e-3;// optimizer steps
    constexpr int BATCH_SIZE = 50;
    constexpr size_t EPOCH = 2;

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
    /* First Convolution Layer */
    model.Add<Convolution<>>(1,   // Number of input activation maps.
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
    model.Add<LeakyReLU<>>();

    /* Second Convolution Layer */
    model.Add<Convolution<>>(32,   // Number of input activation maps.
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
    model.Add<LeakyReLU<>>();

    /* First Max Pooling */
    model.Add<MaxPooling<>>(3, 3, 3, 3, true);

    /* Dropout */
    model.Add<Dropout<>>(0.25);

    /* Third Convolution Layer */
    model.Add<Convolution<>>(16,   // Number of input activation maps.
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
    model.Add<LeakyReLU<>>();

    /* Fourth Convolution Layer */
    model.Add<Convolution<>>(64,   // Number of input activation maps.
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
    model.Add<LeakyReLU<>>();

    /* Second Max Pooling */
    model.Add<MaxPooling<>>(3, 3, 3, 3, true);

    /* Dropout */
    model.Add<Dropout<>>(0.25);

    /* Flatten */
    model.Add<Flatten<>>();

    /* First Dense */
    model.Add<Linear<>>(1824, 128);

    /* Fifth LeakyReLU */
    model.Add<LeakyReLU<>>();

    /* Dropout */
    model.Add<Dropout<>>(0.5);

    /* Final Dense */
    model.Add<Linear<>>(128, 513);
    model.Add<LogSoftMax<>>();



  return 0;
}
