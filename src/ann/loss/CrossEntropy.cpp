/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   CrossEntropy.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"
#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction): ILossLayer(reduction){
    
}

CrossEntropy::CrossEntropy(const CrossEntropy& orig):
ILossLayer(orig){
}

CrossEntropy::~CrossEntropy() {
}

// double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t) {

//     // Cache predictions and targets for backward pass
//     m_aCached_Ypred = X;
//     m_aYtarget = t;

//     // -sum(t * log(X + EPSILON))
//     const double EPSILON = 1e-7;  // Prevent log(0)
//     // auto loss = -xt::sum(t * xt::log(X + EPSILON))();
//     auto loss = -xt::sum(t * xt::log(X + EPSILON) + (1 - t) * xt::log(1 - X + EPSILON))() / X.shape()[0];

//     // Apply reduction strategy
//     if (m_eReduction == REDUCE_MEAN)
//         return loss / X.shape()[0];  // Average loss across the batch

//     else if (m_eReduction == REDUCE_SUM) 
//         return loss;  // Sum of losses across the batch

//     // cross_entropy(X, t);
    
// }
// xt::xarray<double> CrossEntropy::backward() {
//   // Todo CODE YOUR
//     // cout << "begin backward\n";
//     // xt::xarray<double> grad = m_aCached_Ypred - m_aYtarget;
//     const double EPSILON = 1e-7; 

//     // -t / (Y + EPSILON)
//     // xt::xarray<double> grad = -m_aYtarget / (m_aCached_Ypred + EPSILON);
//     xt::xarray<double> grad = (m_aCached_Ypred - m_aYtarget) / ((m_aCached_Ypred * (1 - m_aCached_Ypred)) + EPSILON);

//     if (m_eReduction == REDUCE_MEAN)
//         grad /= m_aCached_Ypred.shape()[0];
      
//     return grad;
//     // int nsample = m_aCached_Ypred.shape()[0];
//     // if(m_eReduction = REDUCE_MEAN) return (-1.0/nsample) * (m_aYtarget/(m_aCached_Ypred+EPSILON));
//     // else return m_aYtarget/(m_aCached_Ypred+EPSILON);
// }

// double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t) {
//     // Cache the predicted probabilities
//     m_aCached_Ypred = X;

//     // Determine if `t` is one-hot encoded or integer-labeled
//     bool is_onehot = t.dimension() == 2 && t.shape()[1] == X.shape()[1];

//     // If `t` is integer-labeled, convert it to one-hot encoding
//     if (!is_onehot) {
//         int nclasses = X.shape()[1];
//         m_aYtarget = onehot_enc(t, nclasses);
//     } else {
//         m_aYtarget = t;
//     }

//     // Compute the cross-entropy loss
//     return cross_entropy(X, m_aYtarget);
// }

double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t) {
    int nsamples = X.shape()[0];
    bool mean_reduced = (this->m_eReduction == REDUCE_MEAN);

    // Check if `t` is hard labels (integer array) or soft labels (float array).
    if (t.shape().size() == 1) {
        // Hard labels - we need to one-hot encode
        int nclasses = X.shape()[1];
        xt::xarray<double> Ytarget = onehot_enc(t, nclasses);
        return cross_entropy(X, Ytarget, mean_reduced);
    } else {
        // Soft labels
        return cross_entropy(X, t, mean_reduced);
    }
}


xt::xarray<double> CrossEntropy::backward() {
    // Compute the gradient of the cross-entropy loss with respect to the input
    int nsamples = m_aCached_Ypred.shape()[0];
    xt::xarray<double> dX = (m_aCached_Ypred - m_aYtarget) / nsamples;
    
    // Adjust gradient calculation based on the reduction method
    if (m_eReduction == REDUCE_SUM) {
        dX *= nsamples;   // Revert division if reduction was sum
    }
    
    return dX;
}
