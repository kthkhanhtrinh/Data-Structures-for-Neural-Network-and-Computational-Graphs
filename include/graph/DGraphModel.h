/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DGraphModel.h
 * Author: LTSACH
 *
 * Created on 23 August 2020, 19:36
 */

#ifndef DGRAPHMODEL_H
#define DGRAPHMODEL_H
#include "graph/AbstractGraph.h"


//////////////////////////////////////////////////////////////////////
///////////// GraphModel: Directed Graph Model    ////////////////////
//////////////////////////////////////////////////////////////////////


template<class T>
class DGraphModel: public AbstractGraph<T>{
private:
public:
    DGraphModel(
            bool (*vertexEQ)(T&, T&), 
            string (*vertex2str)(T&) ): 
        AbstractGraph<T>(vertexEQ, vertex2str){
    }
    
    void connect(T from, T to, float weight=0){
        //TODO
        typename  AbstractGraph<T>::VertexNode* from_node = this->getVertexNode(from);
        typename  AbstractGraph<T>::VertexNode* to_node = this->getVertexNode(to);

        if(from_node == nullptr || to_node == nullptr)
            throw VertexNotFoundException("Vertex does not exist!");
        

        from_node->connect(to_node, weight);
        this->countEdge++;


    }
    void disconnect(T from, T to){
        //TODO
        typename  AbstractGraph<T>::VertexNode* from_node = this->getVertexNode(from);
        typename  AbstractGraph<T>::VertexNode* to_node = this->getVertexNode(to);
        
        if(from_node == nullptr || to_node == nullptr)
            throw VertexNotFoundException("Vertex does not exist!");

        typename AbstractGraph<T>::Edge* edge = from_node->getEdge(to_node);
        if (edge == nullptr)
            throw EdgeNotFoundException("Edge does not exist.");

        from_node->removeTo(to_node);
        this->countEdge--;
    }
    

    void remove(T vertex) {
        typename AbstractGraph<T>::VertexNode* target = this->getVertexNode(vertex);

        if (!target) {
            throw VertexNotFoundException("Vertex does not exist!");
        }

        for (auto* curr : this->nodeList) {
            if (curr->getEdge(target)) 
            {
                curr->removeTo(target); 
                this->countEdge--;      
            }

            if(target->getEdge(curr))
            {
                target->removeTo(curr);
                this->countEdge--;
            }
        }

        this->nodeList.removeItem(target);
        delete target;
        this->countVertex--;
    }

    
    static DGraphModel<T>* create(
            T* vertices, int nvertices, Edge<T>* edges, int nedges,
            bool (*vertexEQ)(T&, T&),
            string (*vertex2str)(T&)){
        //TODO
        auto* graph = new DGraphModel<T>(vertexEQ, vertex2str);
        for(int i = 0; i < nvertices; i++)
            graph->add(vertices[i]);
        
        for(int i = 0; i < nedges; i++)
            graph->connect(edges[i].from, edges[i].to, edges[i].weight);

        return graph;
    } 
};

#endif /* DGRAPHMODEL_H */

