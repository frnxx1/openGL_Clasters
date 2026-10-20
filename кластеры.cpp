#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <windows.h> 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "opengl.hpp"
#include "string"

class Clusters
{
public:
    struct Point
    {
        float x;
        float y;
        float z;
        int clusterId;
    };
private:
    int points;
    int clusters;
    std::vector<Point> data;
    std::vector<std::pair<float, float>> center;
    
public:

    Clusters(int clusters, int points) : clusters(clusters), points(points)
    {
        srand(time(0));
        
        std::vector<float> spreads;
        for (int i = 0; i < clusters; ++i)
        {
            float cx = -5.0f + (float)rand() / RAND_MAX * 10.0f;
            float cy = -5.0f + (float)rand() / RAND_MAX * 10.0f;
            center.push_back({cx,cy});

            float spread = 0.5f + (float)rand() / RAND_MAX * 2.5f;
            spreads.push_back(spread);
        }
        for (int i = 0; i < points; ++i)
        {
            int cl = rand() % clusters;
           
            float spread = spreads[cl];
            /*float x = center[cl].first + (float)rand() / RAND_MAX * spread * 2 - spread;
            float y = center[cl].second + (float)rand() / RAND_MAX * spread * 2 - spread;
            float z = center[cl].first + (float)rand() / RAND_MAX * spread * 2 - spread;*/

           /* float u1 = (float)rand() / RAND_MAX;
            float u2 = (float)rand() / RAND_MAX;
            float u3 = (float)rand() / RAND_MAX;
            float z1 = sqrt(-2 * log(u1)) * cos(2 * 3.14 * u2);
            float z2 = sqrt(-2 * log(u1)) * sin(2 * 3.14 * u2);
            float z3 = sqrt(-2 * log(u1)) * sin(2 * 3.14 * u3);
            float x = center[cl].first + z1 * spread / 2;
            float y = center[cl].second + z2 * spread / 2;
            float z = center[cl].first + z3 * spread / 2;*/

            float theta = (float)rand() / RAND_MAX * 2 * 3.14159f;  
            float phi = acos(1 - 2 * (float)rand() / RAND_MAX);      
            float r = spread;
            float x = center[cl].first + r * sin(phi) * cos(theta);
            float y = center[cl].second + r * sin(phi) * sin(theta);
            float z = r * cos(phi);

            //float t = (float)rand() / RAND_MAX * 2 * 3.14159f;
            //float phi = (float)rand() / RAND_MAX * 2 * 3.14159f;  // для 3D

            //float x = 116.0f * pow(sin(t), 3);
            //float y = 13.0f * cos(t) - 5.0f * cos(2 * t) - 2.0f * cos(3 * t) - cos(4 * t);
            //float z = sin(phi) * 5.0f;  // глубина

            //x *= spread / 16.0f;
            //y *= spread / 16.0f;
            //z *= spread / 16.0f;

            //x += center[cl].first;
            //y += center[cl].second;
            data.push_back({(x),(y),(z),cl});
            
        }
        
    }
    void print()
    {
        for (int i = 0; i < data.size(); ++i)
        {
            std::cout << data[i].x << ";" << data[i].y << " -- " << data[i].clusterId << std::endl;
        }
    }
    void printStatus()
    {
        std::vector<int> count(clusters);
        for (auto i : data)
        {
            count[i.clusterId]++;
        }
        for (int i = 0; i < clusters; ++i)
        {
            std::cout << "Кластер " << i+1 << ": " << count[i] << std::endl;
        }
    }
    void draw()
    {
        float minX = data[0].x, maxX = data[0].x;
        float minY = data[0].y, maxY = data[0].y;
        float pad = 0.5f;
        const int size = 40;
        char** cluster = new char* [size]; 

        for (int i = 0; i < size; ++i)
        {
            cluster[i] = new char[size];  
        }

        char symbols[] = { '*', '#', '@', '&', '%', 'O', 'X', 'o' };
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                cluster[i][j] = ' ';
                
            }
        }
        for (auto p : data)
        {
            if (p.x < minX) minX = p.x;
            if (p.x > maxX) maxX = p.x;
            if (p.y < minY) minY = p.y;
            if (p.y > maxY) maxY = p.y;
        }
        float padding = 0.5f;
        minX -= padding;
        maxX += padding;
        minY -= padding;
        maxY += padding;
        float scaleX = (size - 2) / (maxX - minX);
        float scaleY = (size - 2) / (maxY - minY);
        float scale = std::min(scaleX, scaleY);

        int count = 0;
        for (auto i : data)
        {
            int pointX = size / 2 + (int)(i.x * scale);
            int pointY = size / 2 - (int)(i.y * scale);
            if (pointX >= 0 && pointX < size && pointY >= 0 && pointY < size)
            {
                cluster[pointY][pointX] = symbols[i.clusterId % 8];
               /* ++count;
                if (count % 15 == 0)
                {
                    printToPoint(size, cluster);
                    Sleep(1);
                    system("cls");
                }*/
            }
            
        }
        for (int i = 0; i < clusters; ++i)
        {
            int x = size / 2 + (int)(center[i].first * scale);
            int y = size / 2 - (int)(center[i].second * scale);
            cluster[y][x] = 'C';
            std::cout << "Кластер " << i + 1 << " = " << symbols[i] << std::endl;
        }
        //for (int i = 0; i < size; ++i)
        //{
        //    /*Sleep(10);*/
        //    for (int j = 0; j < size; ++j)
        //    {
        //        std::cout << cluster[i][j] << " ";
        //    }
        //    std::cout << std::endl;
        //}
        printToPoint(size, cluster);
        for (int i = 0; i < size; ++i)
        {
            delete[] cluster[i];
        }
        delete[] cluster;
    }
    void printToPoint(int size,char** cluster)
    {
        for (int i = 0; i < size; ++i)
        {
            /*Sleep(10);*/
            for (int j = 0; j < size; ++j)
            {
                std::cout << cluster[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    std::vector<Point>& get() 
    {
        return data;
    }
    size_t getSize()
    {
        return data.size();;
    }
};


int main()
{
    
    std::setlocale(LC_ALL, "Russian");
   
    Clusters c(6,40000);
    
    c.draw();
    float colors[][3] = {
        {1.0f, 0.2f, 0.2f},  
        {0.2f, 1.0f, 0.2f},  
        {0.2f, 0.2f, 1.0f},  
        {1.0f, 1.0f, 0.2f}, 
        {1.0f, 0.2f, 1.0f},  
        {0.2f, 1.0f, 1.0f}   
    };

    std::vector<Point3D> points;
    points.reserve(c.get().size());
    for (const auto& p : c.get())
    {
        int id = p.clusterId % 6;
        points.push_back({ p.x, p.y, p.z, p.clusterId,colors[id][0], colors[id][1] ,colors[id][2]});
    }

    Window w(points);
    
    while (!glfwWindowShouldClose(w.getWindow()))
    {
        w.windowColor();
        w.drawCluster();
        glfwSwapBuffers(w.getWindow());
        glfwPollEvents();
    }
    w.deleteShaders();
    
    return 0;
}
