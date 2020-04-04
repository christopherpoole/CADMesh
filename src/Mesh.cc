// The MIT License (MIT)
//
// Copyright (c) 2011-2020 Christopher M. Poole <mail@christopherpoole.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// CADMesh //
#include "Mesh.hh"

// STL //
#include <map>


namespace CADMesh
{

Mesh::Mesh(Points points, Triangles triangles, G4String name)
        : name_(name)
        , points_(points)
        , triangles_(triangles)
{
}


std::shared_ptr<Mesh> Mesh::New( Points points
                               , Triangles triangles
                               , G4String name)
{
    return std::make_shared<Mesh>(points, triangles, name);
}


std::shared_ptr<Mesh> Mesh::New( Triangles triangles
                               , G4String name)
{
    Points points;
    //TODO: Extract the unique set of points from the triangles.

    return New(points, triangles, name);
}


std::shared_ptr<Mesh> Mesh::New( std::shared_ptr<Mesh> mesh
                               , G4String name )
{
    return New(mesh->GetPoints(), mesh->GetTriangles(), name);
}


G4String Mesh::GetName()
{
    return name_;
}


Points Mesh::GetPoints()
{
    return points_;
}


Triangles Mesh::GetTriangles()
{
    return triangles_;
}


G4bool Mesh::IsValidForNavigation()
{
    std::map<G4ThreeVector, size_t> point_index;

    size_t index = 0;
    for (auto point : points_)
    {
        point_index[point] = index;
        index++;
    }

    typedef std::pair<size_t, size_t> Edge; // such that Edge.first < Edge.second

    std::map<Edge, G4int> edge_use_count;

    for(size_t i=0; i < triangles_.size(); i++)
    {
        auto triangle = triangles_[i];

        size_t a = point_index[triangle->GetVertex(0)];
        size_t b = point_index[triangle->GetVertex(1)];
        size_t c = point_index[triangle->GetVertex(2)];

        if (a < b)
        {
            edge_use_count[Edge(a, b)] += 1; 
        }

        else if (a > b)
        {
            edge_use_count[Edge(b, a)] += 1; 
        }

        if (b < c)
        {
            edge_use_count[Edge(b, c)] += 1; 
        }

        else if (b > c)
        {
            edge_use_count[Edge(c, b)] += 1; 
        }

        if (c < a)
        {
            edge_use_count[Edge(c, a)] += 1; 
        }

        else if (c > a)
        {
            edge_use_count[Edge(a, c)] += 1; 
 
        }
    }

    // Every edge should be shared with exactly two triangles.
    for (auto count : edge_use_count)
    {
        if (count.second != 2)
        {
            return false;
        }
    }
   
    return true; 

}

} // CADMesh namespace

