/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TopoSorter.h
 * Author: ltsach
 *
 * Created on July 11, 2021, 10:21 PM
 */

#ifndef TOPOSORTER_H
#define TOPOSORTER_H
#include "graph/DGraphModel.h"
#include "list/DLinkedList.h"
#include "sorting/DLinkedListSE.h"
#include "hash/xMap.h"
#include "stacknqueue/Queue.h"
#include "stacknqueue/Stack.h"


template<class T>
class TopoSorter{
public:
    static int DFS;
    static int BFS; 
    
protected:
    DGraphModel<T>* graph;
    int (*hash_code)(T&, int);
    
public:
    TopoSorter(DGraphModel<T>* graph, int (*hash_code)(T&, int)=0){
        //TODO
        this->graph = graph;
        this->hash_code = hash_code;
    }   

    DLinkedList<T> sort(int mode=0, bool sorted=true){
        //TODO
        if(mode == DFS)
            return dfsSort();
        else
            return bfsSort();
    }
    
    DLinkedList<T> bfsSort(bool sorted=true){ 
        //TODO
        DLinkedListSE<T> result;
        Queue<T> q;
        xMap<T, int> map = vertex2inDegree(this->hash_code);

        DLinkedListSE<T> zero_list = listOfZeroInDegrees();
        
        // if (sorted)
        //     zero_list.sort();

        for(auto& vertex : zero_list)
            q.push(vertex);

        while (!q.empty())
        {
            /* code */
            T curr = q.pop();
            result.add(curr);
            DLinkedList<T> tmp = graph->getOutwardEdges(curr);
            for(auto& p : tmp)
            {
                int new_degree = map.get(p) - 1;
                map.put(p, new_degree);
                
                if(new_degree == 0)
                    q.push(p);
            }
        }


        return result;
    }

    


    DLinkedList<T> dfsSort(bool sorted = true) 
    {
        DLinkedListSE<T> result;        
        Stack<T> st;                     
        xMap<T, int> inDegreeMap = vertex2inDegree(this->hash_code);  

        DLinkedListSE<T> zeroList = listOfZeroInDegrees();

        if(sorted)
            zeroList.sort();
        for (const auto& vertex : zeroList) {
            st.push(vertex);

            while (!st.empty()) 
            {
                T curr = st.pop();
                result.add(curr);

                DLinkedListSE<T> neighbors = graph->getOutwardEdges(curr);

                // if (sorted) {
                //     neighbors.sort();
                // }

                for (const auto& neighbor : neighbors) {
                    int newDegree = inDegreeMap.get(neighbor) - 1;
                    inDegreeMap.put(neighbor, newDegree);

                    if (newDegree == 0) {
                        st.push(neighbor);
                    }
                }
            }
        }

        return result;
    }




protected:

    //Helper functions
    
    xMap<T, int> vertex2inDegree(int (*hash)(T&, int))
    {
        xMap<T, int> inDegreeMap(hash);

        for(auto& vertex : graph->vertices())
        {
            int indegree = graph->inDegree(vertex);
            inDegreeMap.put(vertex, indegree);
        }

        return inDegreeMap;
    }
    
    xMap<T, int> vertex2outDegree(int (*hash)(T&, int))
    {
        xMap<T, int> outDegreeMap(hash); 

        for (auto& vertex : graph->vertices()) 
        {
            int outDegree = graph->outDegree(vertex);
            outDegreeMap.put(vertex, outDegree);      
        }

        return outDegreeMap; 
    }

    DLinkedList<T> listOfZeroInDegrees()
    {   
        DLinkedList<T> zero_list; 

        for (T vertex : graph->vertices()) {
            int inDegree = graph->inDegree(vertex); 
            
            if (inDegree == 0) {
                zero_list.add(vertex); 
            }
        }

        return zero_list; 

    }

}; //TopoSorter
template<class T>
int TopoSorter<T>::DFS = 0;
template<class T>
int TopoSorter<T>::BFS = 1;

/////////////////////////////End of TopoSorter//////////////////////////////////


#endif /* TOPOSORTER_H */

