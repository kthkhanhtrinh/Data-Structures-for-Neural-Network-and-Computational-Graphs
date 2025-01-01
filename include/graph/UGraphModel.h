/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   UGraphModel.h
 * Author: LTSACH
 *
 * Created on 24 August 2020, 15:16
 */

#ifndef UGRAPHMODEL_H
#define UGRAPHMODEL_H

#include "graph/AbstractGraph.h"

//////////////////////////////////////////////////////////////////////
///////////// UGraphModel: Undirected Graph Model ////////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
class UGraphModel : public AbstractGraph<T>
{
private:
public:

    UGraphModel(
        bool (*vertexEQ)(T &, T &),
        string (*vertex2str)(T &)) : AbstractGraph<T>(vertexEQ, vertex2str)
    {
        
    }

    void connect(T from, T to, float weight = 0)
    {
        // TODO
        typename AbstractGraph<T>::VertexNode *from_node = this->getVertexNode(from);
        typename AbstractGraph<T>::VertexNode *to_node = this->getVertexNode(to);

        if(!from_node || !to_node)
            throw VertexNotFoundException("Vertex does not exist!");

        if(!from_node->getEdge(to_node))
        {
            from_node->connect(to_node, weight);
            to_node->connect(from_node, weight);
            this->countEdge += 2;
        }
        else
        {
            from_node->getEdge(to_node)->set_weight(weight);
            to_node->getEdge(from_node)->set_weight(weight);

        }

        // else
        // {
        //     typename AbstractGraph<T>::Edge* edge = from_node->getEdge(to_node);
        //     edge->weight = weight;
        // }

    }
    void disconnect(T from, T to)
    {
        // TODO
        typename AbstractGraph<T>::VertexNode *from_node = this->getVertexNode(from);
        typename AbstractGraph<T>::VertexNode *to_node = this->getVertexNode(to);

        if(!from_node || !to_node)
            throw VertexNotFoundException("Vertex does not exist!");

        if(!from_node->getEdge(to_node))
            throw EdgeNotFoundException("Edge does not found!");

        if(from_node == to_node)
            from_node->removeTo(to_node);
        else
        {
            from_node->removeTo(to_node);
            to_node->removeTo(from_node);
                
        }
        this->countEdge--;
        // delete from_node;
        // delete to_node;
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

            if (target->getEdge(curr)) 
            {
                target->removeTo(curr); 
                this->countEdge--;      
            }

        }

        this->nodeList.removeItem(target);
        delete target;
        this->countVertex--;
    }



    static UGraphModel<T> *create(
        T *vertices, int nvertices, Edge<T> *edges, int nedges,
        bool (*vertexEQ)(T &, T &),
        string (*vertex2str)(T &))
    {
        // TODO
        auto* graph = new UGraphModel<T>(vertexEQ, vertex2str);
        for(int i = 0; i < nvertices; i++)
            graph->add(vertices[i]);
        
        for(int i = 0; i < nedges; i++)
            graph->connect(edges[i].from, edges[i].to, edges[i].weight);

        return graph;
    }
};

#endif /* UGRAPHMODEL_H */
