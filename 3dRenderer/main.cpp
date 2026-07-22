
#include <iostream>
#include <vector>
#include <numbers>
#include <string>
#include <chrono>
#include <cmath>
#include "3dRenderer.h"
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <unistd.h>
#endif

bool debugMode = true;
double tau = std::numbers::pi * 2.0;





struct vec2i
{
    public: 
        int x;
        int y;

        vec2i() : x(0), y(0) {}
        
        vec2i(int X, int Y)
        {
            x = X;
            y = Y;
        }
};


class Camera3d
{
public:
    float x;
    float y;
    float z;

    //in radians
    float pitch;
    float roll;
    float yaw;

    float movSpeed;
    float rotSpeed;
    UINT16 fov;

    void setup()
    {

        x = 0;
        y = 0;
        z = 0;

        pitch = 0;
        roll = 0;
        yaw = 0;

        fov = 1;

        movSpeed = 5;
        rotSpeed = tau;
    }
    void pollEvents(float deltaTime)
    {

        float fps = 1 / deltaTime;
        
        if (GetAsyncKeyState('W'))
        {
            z += movSpeed / fps;
        }
        if (GetAsyncKeyState('S'))
        {
            z -= movSpeed / fps;
        }


        if (GetAsyncKeyState('D'))
        {
            x += movSpeed / fps;
        }
        if (GetAsyncKeyState('A'))
        {
            x -= movSpeed / fps;
        }


        if (GetAsyncKeyState('Q'))
        {
            y += movSpeed / fps;
        }
        if (GetAsyncKeyState('E'))
        {
            y -= movSpeed / fps;
        }


        if (GetAsyncKeyState(VK_UP))
        {
            pitch += rotSpeed / fps;
        }
        if (GetAsyncKeyState(VK_DOWN))
        {
            pitch -= rotSpeed / fps;
        }


        if (GetAsyncKeyState(VK_LEFT))
        {
            yaw += rotSpeed / fps;
        }
        if (GetAsyncKeyState(VK_RIGHT))
        {
            yaw -= rotSpeed / fps;
        }


        if (GetAsyncKeyState(VK_NEXT)) //page down
        {
            roll += rotSpeed / fps;
        }
        if (GetAsyncKeyState(VK_PRIOR)) //page up
        {
            roll -= rotSpeed / fps;
        }

        if (GetAsyncKeyState('F'))
        {
            fov += 5 / fps;
        }
        if (GetAsyncKeyState('F') && GetAsyncKeyState(VK_CONTROL))
        {
            fov -= 5 / fps;
        }
    }
};


void bPrint(const char* data, size_t size) {
    if (!data || size == 0) return;

#if defined(_WIN32)
    // Windows Kernel Write
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    WriteFile(hConsole, data, static_cast<DWORD>(size), &written, nullptr);
#else
    // POSIX (Linux / macOS) Kernel Write
    ::write(1, data, size);
#endif
}
        struct vertPos3d
        {
            float x;
            float y;
            float z;
            vertPos3d() : x(0), y(0), z(0) {}

            vertPos3d(float X, float Y, float Z)
            {
                x = X;
                y = Y;
                z = Z;
            }
        };
class Mesh
{
    private:

    public:
        std::vector<vertPos3d> vertPoints;
        Mesh(const std::vector<vertPos3d>& vertpoints)
        {
            vertPoints = vertpoints;
        }
};

class Renderer
{

    private:
        std::vector<Mesh> meshes;
        int frameCount;
        std::string asciiBrightnessPalette;
        vec2i screenDims;
        int charCount;
        std::vector<char> blankScreen;
        std::vector<char> screenBuffer;

        void setPixel(vec2i coords, char character) {
            if (coords.x >= 0 && coords.x < screenDims.x && coords.y >= 0 && coords.y < screenDims.y) {
                int stride = screenDims.x + 1;
                screenBuffer[coords.x + (coords.y * stride)] = character;
            }
        }

        Camera3d camera;
        struct point2d
        {
            float x;
            float y;

            point2d() : x(0), y(0) {}

            point2d(float X, float Y)
            {
                x = X;
                y = Y;
            }
        };

        vec2i screenCoordsToArrayCoords(const point2d point, vec2i dimensions) 
        {
            point2d poin = point;
            poin.x = (point.x + 1.0f) / 2.0f * (dimensions.x - 1);
            poin.y = (1.0f - point.y) / 2.0f * (dimensions.y - 1);
            
            if (poin.x < 0 || poin.x > dimensions.x - 1 || poin.y < 0 || poin.y > dimensions.y - 1)
            {
                return vec2i(-1,-1);
            }

            return vec2i(std::round(poin.x), std::round(poin.y));
           
            
        }

        struct point3d
        {
            float x;
            float y;
            float z;

            point3d() : x(0), y(0), z(0) {}

            point3d(float X, float Y, float Z)
            {
                x = X;
                y = Y;
                z = Z;
            }
        };
        
        MatrixCalc::Mat3x1 point3dtoMat3x1(point3d p)
        {
            return { p.x, p.y, p.z };
        }

        point3d Mat3x1topoint3d(MatrixCalc::Mat3x1 m)
        {
            return point3d(m[0], m[1], m[2]);
        }
        point3d follRotMatrixTransformation(point3d point, float theta, float lambda, float psi)
        {
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            MatrixCalc::mat3x3 rollRotMat{
                cosTheta,-sinTheta,0,
                sinTheta,cosTheta,0,
                0,0,1
            };

            float sinLambda = std::sin(lambda);
            float cosLambda = std::cos(lambda);

            MatrixCalc::mat3x3 pitchRotMat{
                1,0,0,
                0,cosLambda,-sinLambda,
                0,sinLambda,cosLambda
            };


            float sinPsi = std::sin(psi);
            float cosPsi = std::cos(psi);

            MatrixCalc::mat3x3 yawRotMat{
                cosPsi,0,sinPsi,
                0,1,0,
                -sinPsi,0,cosPsi
            };

            //yaw pitch roll method

            MatrixCalc::mat3x3 fullRotMatrix = MatrixCalc::multiply3x3(MatrixCalc::multiply3x3(yawRotMat, pitchRotMat), rollRotMat);

            return point3d(Mat3x1topoint3d(MatrixCalc::matMult3x3_3x1(point3dtoMat3x1(point), fullRotMatrix)));

        }
        point3d rollRotMatrixTransform(point3d point, float theta)
        {
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            MatrixCalc::mat3x3 rollRotMat{
                cosTheta,-sinTheta,0,
                sinTheta,cosTheta,0,
                0,0,1
            };




            return  Mat3x1topoint3d( MatrixCalc::matMult3x3_3x1(point3dtoMat3x1(point), rollRotMat));
        }

        point3d pitchRotMatrixTransform(point3d point, float theta)
        {
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            MatrixCalc::mat3x3 RotMat{
                1,0,0,
                0,cosTheta,-sinTheta,
                0,sinTheta,cosTheta
            };

            return  Mat3x1topoint3d(MatrixCalc::matMult3x3_3x1(point3dtoMat3x1(point), RotMat));
        }

        point3d yawRotMatrixTransform(point3d point, float theta)
        {
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            MatrixCalc::mat3x3 RotMat{
                cosTheta,0,sinTheta,
                0,1,0,
                -sinTheta,0,cosTheta
            };

            return  Mat3x1topoint3d(MatrixCalc::matMult3x3_3x1(point3dtoMat3x1(point), RotMat));
        }
        bool isRunnning;

        point2d calculateProjectedPoint(const point3d point, const Camera3d& cam)
        {
            point3d poin = point;

            poin.x = (point.x - cam.x);
            poin.y = (point.y - cam.y);
            poin.z = (point.z - cam.z);

            poin = follRotMatrixTransformation(poin, cam.roll, cam.pitch, cam.yaw);
            if (poin.z <= 0.1f) {
                return point2d(-999.0f, -999.0f); // makes it offscreen
            }

            float x = poin.x * cam.fov / poin.z;
            float y = poin.y * cam.fov / poin.z;

            

            return point2d(x, y);
        }
        void initialise(std::vector<Mesh> mesh)
        {
            camera.setup();
            frameCount = 0;
            asciiBrightnessPalette = "@%#*+=-:. ";
            screenDims = vec2i(100, 30);
            charCount = (screenDims.x + 1) * screenDims.y;
            isRunnning = true;
            meshes = mesh;
            blankScreen.assign(charCount, '.');
            screenBuffer.assign(charCount, '.');



            for (int y = 0; y < screenDims.y; y++) 
            {
                blankScreen[y * (screenDims.x + 1) + screenDims.x] = '\n';
            }


            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SHORT neededHeight = screenDims.y + 20; // room for debug lines
            SHORT neededWidth = screenDims.x + 2;

            // shrink window first so buffer resize never fails
            SMALL_RECT minRect = { 0, 0, 1, 1 };
            SetConsoleWindowInfo(hConsole, TRUE, &minRect);

            SetConsoleScreenBufferSize(hConsole, { neededWidth, neededHeight });

            SMALL_RECT rect = { 0, 0, (SHORT)(neededWidth - 1), (SHORT)(neededHeight - 1) };
            SetConsoleWindowInfo(hConsole, TRUE, &rect);

        }
        void updateFrame(float deltaTime)
        {
            camera.pollEvents(deltaTime);
            screenBuffer = blankScreen;
            for(int i = 0; i < meshes.size(); i++)
            {
                for (int j = 0; j < meshes[i].vertPoints.size(); j++)
                {
                    point3d point(meshes[i].vertPoints[j].x, meshes[i].vertPoints[j].y, meshes[i].vertPoints[j].z);
                    vec2i screenCoords = screenCoordsToArrayCoords(calculateProjectedPoint(point, camera), screenDims);
                    if (screenCoords.x != -1 && screenCoords.y != -1)
                    {
                        setPixel(screenCoords, 'a');
                    }
                }
            }
            
            
            frameCount++;
        }
        void renderFrame(float deltaTime)
        {
            system("cls");
            
            bPrint(screenBuffer.data(), screenBuffer.size());
            if (debugMode)
            {
                std::string output =
                    "\n-- Camera Debug --\n"
                    "x: " + std::to_string(camera.x) + "\n" +
                    "y: " + std::to_string(camera.y) + "\n" +
                    "z: " + std::to_string(camera.z) + "\n" +
                    "pitch: " + std::to_string(camera.pitch) + "\n" +
                    "yaw: " + std::to_string(camera.yaw) + "\n" +
                    "roll: " + std::to_string(camera.roll) + "\n" +
                    "fov: " + std::to_string(camera.fov) + "\n" +
                    "movSpeed: " + std::to_string(camera.movSpeed) + "\n" +
                    "rotSpeed: " + std::to_string(camera.rotSpeed) + "\n" +
                    "\n-- General --\n" +
                    "deltaTime: " + std::to_string(deltaTime) + "\n" +
                    "fps: " + std::to_string(1 / deltaTime) + "\n";


                bPrint(output.c_str(), output.size());
            }
        }
        void onEnd()
        {

        }
    public:

        void run(std::vector<Mesh> mesh)
        {
            auto lastTime = std::chrono::steady_clock::now();
            initialise(mesh);
            while (isRunnning)
            {
                auto currentTime = std::chrono::steady_clock::now();
                std::chrono::duration<float> elapsedTime = currentTime - lastTime;
                float deltaTime = elapsedTime.count();
                lastTime = currentTime;
                updateFrame(deltaTime);
                renderFrame(deltaTime);
            }
            onEnd();
        }
};


int main()
{
    Renderer renderer;
    std::vector<Mesh> meshes;
    std::vector<vertPos3d> squareVerts = {
        vertPos3d(-1, -1, 1),
        vertPos3d(1, -1, 1),
        vertPos3d(-1, 1, 1),
        vertPos3d(1, 1, 1),
        vertPos3d(-1, -1, 3),
        vertPos3d(1, -1, 3),
        vertPos3d(-1, 1, 3),
        vertPos3d(1, 1, 3),
    };
    Mesh square(squareVerts);
    meshes.push_back(square);
    renderer.run(meshes);
}
