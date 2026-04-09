#include <iostream>  
struct Vertex3D { float pos[3]; float uv[2]; };  
int main() { Vertex3D cbVertices[] = { {-0.5, -0.5, -0.5, 0, 1} }; std::cout << cbVertices[0].pos[0] << \","\ << cbVertices[0].pos[1] << \","\ << cbVertices[0].uv[0] << \","\ << cbVertices[0].uv[1] << std::endl; return 0; }  
