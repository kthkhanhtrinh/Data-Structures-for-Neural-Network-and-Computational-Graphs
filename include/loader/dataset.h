#ifndef DATASET_H
#define DATASET_H

#include "tensor/xtensor_lib.h"
using namespace std;

template <typename DType, typename LType>
class DataLabel {
private:
	xt::xarray<DType> data;
	xt::xarray<LType> label;

public:
	DataLabel(xt::xarray<DType> data, xt::xarray<LType> label)
		: data(data), label(label) {}
	xt::xarray<DType> getData() const { return data; }
	xt::xarray<LType> getLabel() const { return label; }
};

template <typename DType, typename LType>
class Batch {
private:
	xt::xarray<DType> data;
	xt::xarray<LType> label;

public:
	Batch(xt::xarray<DType> data, xt::xarray<LType> label)
		: data(data), label(label) {}
	virtual ~Batch() {}
	xt::xarray<DType>& getData() { return data; }
	xt::xarray<LType>& getLabel() { return label; }
};

template <typename DType, typename LType>
class Dataset {
private:

public:
	Dataset() {};
	virtual ~Dataset() {};

	virtual int len() = 0;
	virtual DataLabel<DType, LType> getitem(int index) = 0;

	virtual xt::svector<unsigned long> get_data_shape() = 0;
	virtual xt::svector<unsigned long> get_label_shape() = 0;

	virtual xt::xarray<DType> get_data() = 0;
	virtual xt::xarray<LType> get_label() = 0;

};

//////////////////////////////////////////////////////////////////////
template <typename DType, typename LType>
class TensorDataset : public Dataset<DType, LType> {
private:
	xt::xarray<DType> data;
	xt::xarray<LType> label;
	xt::svector<unsigned long> data_shape, label_shape;

public:
	TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label) {
		// TODO implement
		this->data = data;
		this->label = label;
 		
		data_shape = xt::svector<unsigned long>(data.shape().begin(), data.shape().end());
		label_shape = xt::svector<unsigned long>(label.shape().begin(), label.shape().end());
		
	}

	int len() { return data.shape()[0];}

	DataLabel<DType, LType> getitem(int index) {
		if(index < 0 || index >= len())
			throw std::out_of_range("Index is out of range!");

		xt::xarray<DType> data_row = xt::view(data, index);
		xt::xarray<LType> label_row;
	
		if(label.shape().size() == 0)
			label_row = label;
		else
			label_row = xt::view(label,index);


		return DataLabel<DType, LType>(data_row, label_row);
	}

	xt::xarray<DType> get_data() { return data; }

	xt::xarray<LType> get_label() { return label; }

	xt::svector<unsigned long> get_data_shape() { return data_shape; }

	xt::svector<unsigned long> get_label_shape() { return label_shape; }

	Batch<DType, LType> get_batch(int start, int end) 
	{
		xt::xarray<DType> batch_data = xt::view(data, xt::range(start, end));
		xt::xarray<LType> batch_label = xt::view(label, xt::range(start, end));
		return Batch<DType, LType>(batch_data, batch_label);
	}

};

//////////////////////////////////////////////////////////////////////



#endif /* DATASET_H */
