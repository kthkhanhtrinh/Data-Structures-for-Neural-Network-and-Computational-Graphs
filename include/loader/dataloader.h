/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "tensor/xtensor_lib.h"
#include "loader/dataset.h"

using namespace std;

template<typename DType, typename LType>
class DataLoader{
public:
    class Iterator; //forward declaration for class Iterator
    
private:
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int nbatch;
    ulong_tensor item_indices;
    int m_seed;
    // TODO : add more member variables to support the iteration
	xt::svector<int> indices;
	int num_samples;
	int num_batches;
	int curr_batch;
	xt::xarray<int> array_index;
    
public:
    DataLoader(Dataset<DType, LType>* ptr_dataset, 
            int batch_size, bool shuffle=true, 
            bool drop_last=false, int seed=-1)
                : ptr_dataset(ptr_dataset), 
                batch_size(batch_size), 
                shuffle(shuffle),
                drop_last(drop_last),
                m_seed(seed)
    {
        nbatch = ptr_dataset->len()/batch_size;
        item_indices = xt::arange(0, ptr_dataset->len());
        
        // MY CODE
        this->num_samples = ptr_dataset->len();
		this->array_index = xt::arange<int>(ptr_dataset->len());
		

		if(this->batch_size > this->num_samples)
			this->batch_size = this->num_samples;

		this->num_batches = (this->num_samples) / this->batch_size; 

		// if(drop_last == true && batch_size > num_samples)
		// 	num_batches -= 1;
		if( batch_size > num_samples)
			num_batches -= 1;
		this->curr_batch = 0;


		indices.resize(num_samples);

		if (shuffle) {
            // xt::random::default_engine_type engine(0); 
			if(seed >= 0) {xt::random::seed(seed);}
            xt::random::shuffle(array_index); 
			for(int i = 0; i < num_samples; ++i)
				indices[i] = array_index(i);
		}
		else
		{
			for (int i = 0; i < num_samples; ++i)
				indices[i] = i;
		}

    }
    virtual ~DataLoader(){}
    
    //New method: from V2: begin
    int get_batch_size(){ return batch_size; }
    int get_sample_count(){ return ptr_dataset->len(); }
    int get_total_batch(){return int(ptr_dataset->len()/batch_size); }
    
    // MY CODE
    DataLabel<DType, LType> getBatch(int batch_index) const 
	{
		int start_idx = batch_index * batch_size;
		int end_idx = min(start_idx + batch_size, num_samples);
		
		if ((batch_index + 1) == (num_batches) && drop_last == false)
		{	
			end_idx = num_samples;
		}

		auto dataset_data = ptr_dataset->get_data(); 
		auto dataset_label = ptr_dataset->get_label(); 

		xt::xarray<DType> batch_data;
		xt::xarray<LType> batch_labels;

		auto data_shape = dataset_data.shape();
		auto label_shape = dataset_label.shape();

		xt::svector<size_t> data_batch_shape = {static_cast<size_t>(end_idx - start_idx)};
		xt::svector<size_t> label_batch_shape = {static_cast<size_t>(end_idx - start_idx)};

		for (size_t i = 1; i < data_shape.size(); i++) 
			data_batch_shape.push_back(data_shape[i]);
		
		for (size_t i = 1; i < label_shape.size(); ++i) 
			label_batch_shape.push_back(label_shape[i]);
		

		batch_data = xt::xarray<DType>::from_shape(data_batch_shape);
		batch_labels = xt::xarray<LType>::from_shape(label_batch_shape);


		for (int i = start_idx; i < end_idx; ++i) 
		{
			auto data_row = ptr_dataset->getitem(indices[i]).getData();  //  a row/slice
			auto label_row = ptr_dataset->getitem(indices[i]).getLabel();  // a scalar or 1D array
			
			if(ptr_dataset->get_label().shape().size() == 0)
			{
				batch_labels= label_row;
			}
			else 
			{
				xt::view(batch_labels, i - start_idx, xt::all()) = label_row;
			}
			
			xt::view(batch_data, i - start_idx, xt::all()) = data_row;

		}   

		return DataLabel<DType, LType>(batch_data, batch_labels);
	}
    //New method: from V2: end
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// START: Section                                                     //
    /////////////////////////////////////////////////////////////////////////
public:
    Iterator begin() {
    	return Iterator(this, 0);
  	}

  	Iterator end() {
    	return Iterator(this, num_batches);
  	}

    // TODO implement forech
  	class Iterator {
    private:
		DataLoader* data_loader;
		int curr_index;

    public:
		Iterator(DataLoader<DType, LType>* data_loader, int curr_index = 0)
		{
			this->data_loader = data_loader;
			this->curr_index = curr_index;
		}

    	Iterator& operator=(const Iterator& iterator) {
			if(this != &iterator)
			{
				data_loader = iterator.data_loader;
				curr_index = iterator.curr_index;
			}
			return *this;
    	}

		Iterator& operator++() {
			if(curr_index < data_loader->num_batches)
				++curr_index;
			return *this;
		}


    	Iterator operator++(int) {
			Iterator tmp = *this;
			++(*this);
			return tmp;
    	}

    	bool operator!=(const Iterator& other) const {
			return curr_index != other.curr_index;
    	}


		Batch<DType, LType> operator*() const {
		    if (data_loader) {
        		DataLabel<DType, LType> dataLabel = data_loader->getBatch(curr_index);
				// int batch_index = data_loader->getBatch(curr_index);



        		return Batch<DType, LType>(dataLabel.getData(), dataLabel.getLabel());
    		}
    		return Batch<DType, LType>(xt::xarray<DType>(), xt::xarray<LType>());
		}


    };
    
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// END: Section                                                       //
    /////////////////////////////////////////////////////////////////////////
};


#endif /* DATALOADER_H */

