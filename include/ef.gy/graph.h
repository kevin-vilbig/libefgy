/**\file
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined (EF_GY_GRAPH_H)
#define EF_GY_GRAPH_H

#include <vector>
#include <set>

namespace efgy
{
    namespace graph
    {   
        

       /// the adjacency matrix of a simple undirected graph
	    template <typename T, unsigned int size_increment=16>
        class adjacencymatrix
        {
        public:
            adjacencymatrix() : matrix_size(size_increment)
            {
               adjacency.resize(matrix_size);
               for(int i = 0; i < matrix_size; i++)
               {
                    adjacency[i].resize(matrix_size);
               }
               
            }

            adjacencymatrix(const adjacencymatrix<T, size_increment> &b)
            {
                matrix_size      = b.matrix_size;
                adjacency.resize(matrix_size);
                for(int i = 0; i < matrix_size; i++)
                {
                   adjacency[i].resize(matrix_size);
                    for(int k = 0; k < matrix_size; k++)
                    {
                        adjacency[i][k] = b.adjacency[i][k];
                    }
                }                
                nodes     = std::vector<T> (b.nodes);
            }

            adjacencymatrix<T> &operator = (const adjacencymatrix<T, size_increment> &b)
            {
                matrix_size = b.matrix_size;

            }

            /// add a new node without any connections 
            void add (T node)
            {
                nodes.push_back(node);
                if(nodes.matrix_size() > matrix_size)
                {
                    increaseSize();
                }
                int last = nodes.matrix_size();

                // the new node isn't connected to anything yet
                for(int i = 0; i < matrix_size; i++)
                {
                    adjacency[i][last-1] = false;
                    adjacency[last-1][i] = false;
                }
            }

        private:
            /// matrix_matrix_size is periodically increased to avoid 
            /// reconstructing the adjacency matrix too often.
            unsigned int matrix_size;
            
            std::vector< std::vector<bool> > adjacency;
            std::vector<T> nodes;

            /// number of nodes in the graph
            unsigned int size()
            {
                return nodes.size();
            }

            /// increases matrix_size and the adjacency matrix
            void increaseSize()
            {
                int old_size = matrix_size;
                matrix_size += size_increment;
                adjacency.resize(matrix_size);
                for(int i = 0; i < matrix_size; i++)
                {
                    adjacency[i].resize(matrix_size);
                }

                for(int i = 0; i < matrix_size; i++)
                {
                    for(int k = old_size; k < matrix_size; k++)
                    {
                        adjacency[i][k] = false;
                        adjacency[k][i] = false; 
                    }
                }

            }

            void erase(unsigned int i)
            {
                nodes.erase(i);
                for(int k = 0; k < matrix_size; k++)
                {
                    adjacency[i][k] = false;
                    adjacency[k][i] = false; 
                }
            }

            void connect(unsigned int i, unsigned int k)
            {
                setAdjacency(i, k, true);
            }

            void setAdjacency(unsigned int i, unsigned int k, bool adj)
            {
                if((i >= nodes.size()) || (k >= nodes.size()))
                {
                    // throw exception
                }
                else
                {
                    adjacency[i][k] = adj;
                    adjacency[i][k] = adj; 
                }
            }

            /// the value in the ith node
            T value(unsigned int i)
            {
                return nodes[i];
            }

            
            /// return all adjacent  nodes to the ith node
            std::vector<unsigned int> adjacentNodes(unsigned int i)
            {
               std::vector<unsigned int> ret;
                for(int k = 0; k < nodes.size(); k++)
                {
                    if(adjacency[i][k]) {
                        ret.push_back(k);
                    }
                }
                return ret;
            }

            /// does a path exist between nodes i and k?
            bool pathExists(unsigned int i, unsigned int k)
            {
                if(adjacency[i][k])
                { 
                   return true;
                }
                else
                {
                  bool found = false;
                  unsigned int a = 0;
                  while(!found && (a < nodes.size()))
                   {
                        found = (a != i) && adjacency[i][a] && pathExists(a, k);
                        a++;
                   }
                   return found;
                }
            }


            
        };

        template <typename T>
        class node
        {
            public:
                node(T _data) : data(_data), adjacentNodes(std::set());
                node(T _data, std::set< node<T>* > nodes) : data(_data), adjacentNodes(nodes);
                ~node();
                node(node that)
                {
                    data = that.data;
                    nodes = std::set(that.nodes);
                }
                
                /// connects nodes this and that
                void connect(node<T>* that)
                {
                    adjacentNodes.insert(that);
                }

                /// constructs new node from data that is connected to the current node 
                void insert(T data)
                {
                    node<T> *other = new node(data);
                    this.connect(other);
                }

                bool connectedTo(node<T>* that)
                {
                    return (adjacentNodes.find(that) != adjacentNodes.end);
                }

                void deleteConnection(node<T>* that)
                {
                    adjacentNodes.erase(that);
                }

                bool pathExists(node<T>* that)
                {
                    bool found = connectedTo(that);
                    std::set<node<T>* >::iterator it = adjacentNodes.begin();
                    while(it != adjacentNodes.end())
                    {
                        found = found || pathExists(*it);
                    }
                    return found;
                }

            private:
                T data;
                std::set< node<T>* > adjacentNodes;

                

        };
    }
}
#endif 
