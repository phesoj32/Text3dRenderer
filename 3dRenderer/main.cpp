

#include "3dRenderer.h"
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef min
    #undef max
#else
    #include <unistd.h>
#endif
#include <iostream>
#include <vector>
#include <numbers>
#include <string>
#include <chrono>
#include <cmath>
#include <algorithm>




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
struct vec2
{
public:
    float x, y;

    vec2() : x(0), y(0) {}

    vec2(float X, float Y)
    {
        x = X;
        y = Y;
    }

    void operator=(const vec2i other)
    {
        x = other.x;
        y = other.y;
    }
    
};


class Camera3d
{
private:
    point3d camToWorld(point3d v) const
    {
        float sinTheta = std::sin(roll), cosTheta = std::cos(roll);
        MatrixCalc::mat3x3 rollRotMat{
            cosTheta, -sinTheta, 0,
            sinTheta,  cosTheta, 0,
            0,         0,        1
        };

        float sinLambda = std::sin(pitch), cosLambda = std::cos(pitch);
        MatrixCalc::mat3x3 pitchRotMat{
            1, 0,         0,
            0, cosLambda, -sinLambda,
            0, sinLambda,  cosLambda
        };

        float sinPsi = std::sin(yaw), cosPsi = std::cos(yaw);
        MatrixCalc::mat3x3 yawRotMat{
             cosPsi, 0, sinPsi,
             0,      1, 0,
            -sinPsi, 0, cosPsi
        };

        // Inverse of the (Yaw(-ψ)·Pitch(-λ)·Roll(-θ)) matrix used in calculateProjectedPoint
        // is Roll(θ)·Pitch(λ)·Yaw(ψ) — this keeps movement perfectly in sync with the view.

        //pitchRotMat, yawRotMat), rollRotMat
        MatrixCalc::mat3x3 invRotMatrix = 
        MatrixCalc::multiply3x3(MatrixCalc::multiply3x3(rollRotMat, yawRotMat), pitchRotMat);

        MatrixCalc::Mat3x1 in{ v.x, v.y, v.z };
        MatrixCalc::Mat3x1 out = MatrixCalc::matMult3x3_3x1(in, invRotMatrix);
        return point3d(out[0], out[1], out[2]);
    }

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
    float fov;

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
        float moveAmount = movSpeed * deltaTime;
        float rotAmount = rotSpeed * deltaTime;

        // Camera direction vectors
        float cosPitch = std::cos(pitch);
        float sinPitch = std::sin(pitch);
        float cosYaw = std::cos(yaw);
        float sinYaw = std::sin(yaw);


        point3d forward = camToWorld(point3d(0.0f, 0.0f, 1.0f));
        point3d right = camToWorld(point3d(1.0f, 0.0f, 0.0f));

        // Movement
        if (GetAsyncKeyState('W'))
        {
            x += forward.x * moveAmount;
            y += forward.y * moveAmount;
            z += forward.z * moveAmount;
        }

        if (GetAsyncKeyState('S'))
        {
            x -= forward.x * moveAmount;
            y -= forward.y * moveAmount;
            z -= forward.z * moveAmount;
        }

        if (GetAsyncKeyState('D'))
        {
            x += right.x * moveAmount;
            y += right.y * moveAmount;
            z += right.z * moveAmount;
        }

        if (GetAsyncKeyState('A'))
        {
            x -= right.x * moveAmount;
            y -= right.y * moveAmount;
            z -= right.z * moveAmount;
        }

        if (GetAsyncKeyState('Q'))
        {
            y += moveAmount;
        }

        if (GetAsyncKeyState('E'))
        {
            y -= moveAmount;
        }

        // Rotation
        if (GetAsyncKeyState(VK_UP))
            pitch -= rotAmount;

        if (GetAsyncKeyState(VK_DOWN))
            pitch += rotAmount;

        if (GetAsyncKeyState(VK_LEFT))
            yaw -= rotAmount;

        if (GetAsyncKeyState(VK_RIGHT))
            yaw += rotAmount;
           /* keeping for future refrence
        if (GetAsyncKeyState(VK_NEXT))
            roll -= rotAmount;

        if (GetAsyncKeyState(VK_PRIOR))
            roll += rotAmount;
            */
        if (GetAsyncKeyState('F'))
            fov += 5 * deltaTime;

        if (GetAsyncKeyState('F') && GetAsyncKeyState(VK_CONTROL))
            fov -= 5 * deltaTime;
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

struct triangle3d
{
public:
    vertPos3d verts[3];

    triangle3d() {}

    triangle3d(const vertPos3d &a, const vertPos3d &b, const vertPos3d &c)
    {
        verts[0] = a;
        verts[1] = b;
        verts[2] = c;
    }
};

struct sspTriangle2d
{
    vec2 verts[3];

    sspTriangle2d() {}

    sspTriangle2d(const vec2& a, const vec2& b, const vec2& c)
    {
        verts[0] = a;
        verts[1] = b;
        verts[2] = c;
    }
};

class Mesh
{
    private:

    public:
        std::vector<triangle3d> triangles;
        Mesh(const std::vector<triangle3d>& vertpoints)
        {
            triangles = vertpoints;
        }
};

class Renderer
{

    private:
        std::vector<Mesh> meshes;
        std::vector<sspTriangle2d> projectedTriangles;
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

        float edgeFunction(const vec2& a, const vec2& b, const vec2& c) {
            return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
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

        std::vector<vec2i> rasterizeAndConvertToFrameBufferCoords(const Mesh &curMesh)
        {

                for (int j = 0; j < curMesh.triangles.size(); j++)
                {
                    sspTriangle2d curTri;
                    bool valid = true;

                    for (int l = 0; l < 3; l++)
                    {
                        vertPos3d currentVert = curMesh.triangles[j].verts[l];
                        point3d point(currentVert.x, currentVert.y, currentVert.z);

                        vec2i screenCoords = screenCoordsToArrayCoords(calculateProjectedPoint(point, camera), screenDims);
                            curTri.verts[l] = screenCoords;
                        

                    }
                    
                    int minX = std::min({ curTri.verts[0].x, curTri.verts[1].x, curTri.verts[2].x });
                    int minY = std::min({ curTri.verts[0].y, curTri.verts[1].y, curTri.verts[2].y });
                    int maxX = std::max({ curTri.verts[0].x, curTri.verts[1].x, curTri.verts[2].x });
                    int maxY = std::max({ curTri.verts[0].y, curTri.verts[1].y, curTri.verts[2].y });

                    minX = std::max(minX, 0);
                    minY = std::max(minY, 0);
                    maxX = std::min(maxX, screenDims.x - 1);
                    maxY = std::min(maxY, screenDims.y - 1);


                    for (int y = minY; y <= maxY; y++)
                    {
                        for (int x = minX; x <= maxX; x++)
                        {
                            vec2 p(x + 0.5f, y + 0.5f);

                            float w0 = edgeFunction(curTri.verts[1], curTri.verts[2], p);
                            float w1 = edgeFunction(curTri.verts[2], curTri.verts[0], p);
                            float w2 = edgeFunction(curTri.verts[0], curTri.verts[1], p);

                            // If the triangle faces the opposite direction, reverse the sign check
                            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                                // Pixel is inside the triangle; paint it
                               
                                setPixel(vec2i(x,y), '#');
                            }
                        }
                    }
                }
            
                return std::vector<vec2i>(1, vec2i(1, 1));


        }

        vec2i screenCoordsToArrayCoords(const point2d &point, vec2i dimensions) 
        {
            point2d poin = point;
            poin.x = (point.x + 1.0f) / 2.0f * (dimensions.x - 1);
            poin.y = (1.0f - point.y) / 2.0f * (dimensions.y - 1);
            

            return vec2i(std::round(poin.x), std::round(poin.y));
           
            
        }

        
        
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

            MatrixCalc::mat3x3 fullRotMatrix = MatrixCalc::multiply3x3(MatrixCalc::multiply3x3(pitchRotMat, yawRotMat), rollRotMat);

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

            poin = follRotMatrixTransformation(poin, -cam.roll, -cam.pitch, -cam.yaw);
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
            //loops over all meshes vertices
            for (int i = 0; i < meshes.size(); i++)
            {
                rasterizeAndConvertToFrameBufferCoords(meshes[i]);
            }
           


            frameCount++;
        }
        void renderFrame(float deltaTime)
        {
            #if defined(_WIN32)
            // Move the cursor back to the top-left corner instead of clearing the screen
            static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD coord = { 0, 0 };
            SetConsoleCursorPosition(hConsole, coord);
            #else
            // POSIX terminal escape sequence to move cursor to home position (0, 0)
            std::cout << "\033[H";
            #endif
            
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

        void run(const std::vector<Mesh> &mesh)
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
    std::vector<triangle3d> squareMesh = {

        triangle3d(
        vertPos3d(1, 1, 1),
        vertPos3d(1, -1, 1),
        vertPos3d(-1, 1, 1)),

        triangle3d(
        vertPos3d(-1, -1, 1),
        vertPos3d(1, -1, 1),
        vertPos3d(-1, 1, 1)),

        triangle3d(
        vertPos3d(1, 1, -1),
        vertPos3d(1, -1, -1),
        vertPos3d(-1, 1, -1)),

        triangle3d(
        vertPos3d(-1, -1, -1),
        vertPos3d(1, -1, -1),
        vertPos3d(-1, 1,-1)),

        triangle3d(
        vertPos3d(1, 1, 1),
        vertPos3d(1, 1, -1),
        vertPos3d(1, -1, 1)),

        triangle3d(
        vertPos3d(1, -1, -1),
        vertPos3d(1, 1, -1),
        vertPos3d(1, -1, 1)),

        triangle3d(
        vertPos3d(-1, 1, 1),
        vertPos3d(-1, 1, -1),
        vertPos3d(-1, -1, 1)),

        triangle3d(
        vertPos3d(-1, -1, -1),
        vertPos3d(-1, 1, -1),
        vertPos3d(-1, -1, 1)),

        triangle3d(
        vertPos3d(1, 1, 1),
        vertPos3d(-1, 1, 1),
        vertPos3d(1, 1, -1)),

        triangle3d(
        vertPos3d(-1, 1, -1),
        vertPos3d(-1, 1, 1),
        vertPos3d(1, 1, -1)),

        triangle3d(
        vertPos3d(1, -1, 1),
        vertPos3d(-1, -1, 1),
        vertPos3d(1, -1, -1)),

        triangle3d(
        vertPos3d(-1, -1, -1),
        vertPos3d(-1, -1, 1),
        vertPos3d(1, -1, -1))
    };
    Mesh square(squareMesh);
    meshes.push_back(square);
    renderer.run(meshes);
}
