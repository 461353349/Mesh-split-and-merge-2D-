# Mesh-split-and-merge-2D-
Implementation of meshes that split and merge in real time (2D) based on the article http://pub.ist.ac.at/group_wojtan/projects/topo_goop/topo_goop.pdf

The purpose of this project was to understand and put in practise techniques presented in the above paper.

Meshes are represented in a 2D grid based space, in comparaison to the original implementation. The advantage of using a 2D representaton here is the real time concept, which is faster to use for devlopment than a 3D simulation.

The meshes sharing one or more cells of the grid are potentially meshes that can be merged between them. A signed distance field is calculated from the cells to the meshes in order to detect which meshes are or not in each cells.

Intersection detection between meshes is then processed when all the above conditions are present. The final step consist in collapsing former edges, creating new one(s) in order to merge, or split meshes.
