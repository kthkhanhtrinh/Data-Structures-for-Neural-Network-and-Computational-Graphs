#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

#include <filesystem> //require C++17
namespace fs = std::filesystem;

#include "list/listheader.h"
#include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"
#include "ann/annheader.h"
#include "loader/dataset.h"
#include "loader/dataloader.h"
#include "loader/DataLoaderDemo.h"

#include "config/Config.h"
#include "dataset/DSFactory.h"
#include "optim/Adagrad.h"
#include "optim/Adam.h"
#include "modelzoo/twoclasses.h"
#include "modelzoo/threeclasses.h"

void test_forward_batch();
void test_backward_batch();
void test_softmax_backward();

int main(int argc, char** argv) {
    //dataloader:
    // case_data_wo_label_1();
    // case_data_wi_label_1();
    // case_batch_larger_nsamples();
    
    //Classification:
    twoclasses_classification();
    // threeclasses_classification();
    
    //test
    // test_forward_batch();
    // test_softmax_backward();

    // test_backward_batch();

    return 0;
}
void test_forward_batch() {
  // DATA
  FCLayer fc_layer(2, 3, true);
  xt::xarray<double> weights = {{0.2, 0.5}, {0.3, 0.7}, {0.4, 0.9}};
  xt::xarray<double> bias = {0.1, -0.1, 0.2};
  fc_layer.set_weights(weights);
  fc_layer.set_bias(bias);
  xt::xarray<double> X = {{1.0, 2.0}, {0.5, 1.5}, {1.5, 0.5}};

  // expected
  xt::xarray<double> expected_output = {
      {1.3, 1.6, 2.4}, {0.95, 1.1, 1.75}, {0.65, 0.7, 1.25}};

  xt::xarray<double> output = fc_layer.forward(X);

  // result
  cout << output << endl;
  assert(xt::allclose(output, expected_output));
  std::cout << "Forward batch test passed!" << std::endl;
}

void test_backward_batch() {
  // DATA
  FCLayer fc_layer(2, 3, true);
  xt::xarray<double> weights = {{0.2, 0.5}, {0.3, 0.7}, {0.4, 0.9}};
  xt::xarray<double> bias = {0.1, -0.1, 0.2};
  fc_layer.set_weights(weights);
  fc_layer.set_bias(bias);
  xt::xarray<double> X = {{1.0, 2.0}, {0.5, 1.5}, {1.5, 0.5}};
  fc_layer.set_working_mode(true);
  fc_layer.forward(X);
  xt::xarray<double> DY = {
      {1.0, 0.5, -0.5}, {0.5, -0.5, 0.0}, {0.0, 1.0, -1.0}};

  // expect use mean
  xt::xarray<double> expected_grad_W = {
      {0.416667, 0.916667}, {0.583333, 0.25}, {-0.666667, -0.5}};
  xt::xarray<double> expected_grad_b = {0.5, 0.333333, -0.5};
  xt::xarray<double> expected_dx = {{0.15, 0.4}, {-0.05, -0.1}, {-0.1, -0.2}};
  cout << "Doing backward\n";
  // Thực hiện backward
  xt::xarray<double> dX = fc_layer.backward(DY);

  // public m_aGrad_W and m_aGrad_b in FCLayer
  cout << fc_layer.m_aGrad_W << endl;
  cout << fc_layer.m_aGrad_b << endl;
  cout << dX << endl;
  assert(xt::allclose(fc_layer.m_aGrad_W, expected_grad_W));
  assert(xt::allclose(fc_layer.m_aGrad_b, expected_grad_b));
  assert(xt::allclose(dX, expected_dx));
  std::cout << "Backward batch test passed!" << std::endl;
}

void test_softmax_backward() {
  // DATA
  Softmax softmax_layer;
  softmax_layer.set_working_mode(true);
  xt::xarray<double> X = {{1.0, 2.0, 3.0}, {1.0, -1.0, 0.0}};
  softmax_layer.forward(X);
  xt::xarray<double> DY = {{0.1, 0.2, -0.3}, {-0.1, 0.3, 0.0}};

  // expect
  xt::xarray<double> expected_DX = {{0.021754, 0.083605, -0.105359},
                                    {-0.040237, 0.030567, 0.00967}};

  // Thực hiện backward
  xt::xarray<double> DX = softmax_layer.backward(DY);

  // approximately
  cout << "DX :" << DX << endl;
  cout << "approximately expected_DX: " << expected_DX << endl;
}
