/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Softmax.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:46 PM
 */

#include "layer/Softmax.h"
#include "ann/functions.h"
#include "sformat/fmt_lib.h"
#include <filesystem> //require C++17
namespace fs = std::filesystem;

Softmax::Softmax(int axis, string name): m_nAxis(axis) {
    if(trim(name).size() != 0) m_sName = name;
    else m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax& orig) {
}

Softmax::~Softmax() {}

xt::xarray<double> Softmax::forward(xt::xarray<double> X) {
    m_aCached_Y = softmax(X, m_nAxis);
    return m_aCached_Y;
}

xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
    auto shape = DY.shape();
    size_t n_samples = 1;
    if(shape.size() > 1) n_samples = shape[0];

    xt::xarray<double> dX = xt::zeros_like(DY);

    for(size_t i = 0; i < n_samples; ++i){
        xt::xarray<double> yi = xt::row(this->m_aCached_Y, i);
        xt::xarray<double> dyi = xt::row(DY, i);
        xt::xarray<double> jacobian = xt::diag(yi) - xt::linalg::outer(yi, yi);
        xt::row(dX, i) = xt::linalg::dot(dyi, jacobian);
    }
    return dX;
}
// xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
//     // Assume m_aCached_Y is the output of the softmax layer from the forward pass
//     // and has the same shape as DY

//     // Calculate DX using mean instead of sum
//     // xt::xarray<double> DX = m_aCached_Y * (DY - xt::mean(DY * m_aCached_Y, {1}, xt::keep_dims));
//     xt::xarray<double> dA_a = xt::sum(DY * m_aCached_Y, {1}, xt::keep_dims);
//     xt::xarray<double> dZ = m_aCached_Y * (DY - dA_a);

//     return dZ;
// }
string Softmax::get_desc(){
    string desc = fmt::format("{:<10s}, {:<15s}: {:4d}",
                    "Softmax", this->getname(), m_nAxis);
    return desc;
}
