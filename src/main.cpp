#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "header/cube.h"
#include "header/Object.h"
#include "header/shader.h"
#include "header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
void updateCamera();
void applyOrbitDelta(float yawDelta, float pitchDelta, float radiusDelta);
unsigned int loadCubemap(std::vector<std::string> &mFileName);
unsigned int bubbleVAO, bubbleVBO;
// 10 個泡泡的隨機起始時間偏移量
float bubbleOffsets[] = { 0.0f, 0.5f, 1.2f, 2.8f, 3.1f, 4.5f, 0.8f, 2.2f, 3.6f, 1.9f };

struct material_t{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float gloss;
};

struct light_t{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct camera_t{
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    glm::vec3 target;

    float yaw;
    float pitch;
    float radius;
    float minRadius;
    float maxRadius;
    float orbitRotateSpeed;
    float orbitZoomSpeed;
    float minOrbitPitch;
    float maxOrbitPitch;
    bool enableAutoOrbit;
    float autoOrbitSpeed;
};

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// cube map 
unsigned int cubemapTexture;
unsigned int cubemapVAO, cubemapVBO;

// shader programs 
int shaderProgramIndex = 0;
std::vector<shader_program_t*> shaderPrograms;
shader_program_t* cubemapShader;

light_t light;
material_t material;
camera_t camera;

// SpongeBob model parts
Object* spongeBobBody = nullptr;
Object* spongeBobLeftHand = nullptr;
Object* spongeBobRightHand = nullptr;
Object* cubeModel = nullptr;
Object* floorModel = nullptr;
bool isCube = false;
bool isBlowing = false; // 吹泡泡
glm::mat4 modelMatrix(1.0f);

float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 丟漢堡炸彈
Object* burgerModel = nullptr;
struct Burger {
    glm::vec3 position;
    glm::vec3 velocity;
    bool isFlying;     
    float stateStartTime; 
};

std::vector<Burger> burgerList; 
bool hasFired = false;          
bool isThrowing = false;   
float throwStartTime = 0.0f;   

const glm::vec3 gravity(0.0f, -300.0f, 0.0f); 

float handRotationAngle = 0.0f;


void model_setup(){
#if defined(__linux__) || defined(__APPLE__)
    std::string body_path = "..\\..\\src\\asset\\obj\\Body.obj";
    std::string left_hand_path = "..\\..\\src\\asset\\obj\\Left_Hand.obj";
    std::string right_hand_path = "..\\..\\src\\asset\\obj\\Right_Hand.obj";
    std::string cube_obj_path = "..\\..\\src\\asset\\obj\\cube.obj";
    std::string texture_path = "..\\..\\src\\asset\\texture\\spongebob.png";
    std::string burger_path = "..\\..\\src\\asset\\obj\\burger.obj";
    #else
    std::string body_path = "..\\..\\src\\asset\\obj\\Body.obj";
    std::string left_hand_path = "..\\..\\src\\asset\\obj\\Left_Hand.obj";
    std::string right_hand_path = "..\\..\\src\\asset\\obj\\Right_Hand.obj";
    std::string cube_obj_path = "..\\..\\src\\asset\\obj\\cube.obj";
    std::string texture_path = "..\\..\\src\\asset\\texture\\spongebob.png";
    std::string burger_path = "..\\..\\src\\asset\\obj\\burger.obj";
#endif

    // Load SpongeBob body
    spongeBobBody = new Object(body_path);
    spongeBobBody->loadTexture(texture_path);
    
    // Load SpongeBob left hand
    spongeBobLeftHand = new Object(left_hand_path);
    spongeBobLeftHand->loadTexture(texture_path);
    
    // Load SpongeBob right hand
    spongeBobRightHand = new Object(right_hand_path);
    spongeBobRightHand->loadTexture(texture_path);
    
    // Load cube model
    cubeModel = new Object(cube_obj_path);

    // Load floor model
    floorModel = new Object(cube_obj_path); // 重複利用 cube.obj
    std::string floor_texture_path = "..\\..\\src\\asset\\texture\\skybox\\bottom.jpg";     
    floorModel->loadTexture(floor_texture_path);

    // Load burger model
    burgerModel = new Object(burger_path);
    std::string burger_texture_path = "..\\..\\src\\asset\\texture\\burger.png";
    burgerModel->loadTexture(burger_texture_path);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100.0f));
}

void camera_setup(){
    camera.worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.yaw = 90.0f;
    camera.pitch = 10.0f;
    camera.radius = 400.0f;
    camera.minRadius = 150.0f;
    camera.maxRadius = 800.0f;
    camera.orbitRotateSpeed = 60.0f;
    camera.orbitZoomSpeed = 400.0f;
    camera.minOrbitPitch = -80.0f;
    camera.maxOrbitPitch = 80.0f;
    camera.target = glm::vec3(0.0f);
    camera.enableAutoOrbit = true;
    camera.autoOrbitSpeed = 20.0f;

    updateCamera();
}

void updateCamera(){
    float yawRad = glm::radians(camera.yaw);
    float pitchRad = glm::radians(camera.pitch);
    float cosPitch = cos(pitchRad);

    camera.position.x = camera.target.x + camera.radius * cosPitch * cos(yawRad);
    camera.position.y = camera.target.y + camera.radius * sin(pitchRad);
    camera.position.z = camera.target.z + camera.radius * cosPitch * sin(yawRad);

    camera.front = glm::normalize(camera.target - camera.position);
    camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
}

void applyOrbitDelta(float yawDelta, float pitchDelta, float radiusDelta) {
    camera.yaw += yawDelta;
    camera.pitch = glm::clamp(camera.pitch + pitchDelta, camera.minOrbitPitch, camera.maxOrbitPitch);
    camera.radius = glm::clamp(camera.radius + radiusDelta, camera.minRadius, camera.maxRadius);
    updateCamera();
}

void light_setup(){
    light.position = glm::vec3(1000.0, 1000.0, 0.0);
    light.ambient = glm::vec3(1.0);
    light.diffuse = glm::vec3(1.0);
    light.specular = glm::vec3(1.0);
}

void material_setup(){
    material.ambient = glm::vec3(0.5);
    material.diffuse = glm::vec3(1.0);
    material.specular = glm::vec3(0.7);
    material.gloss = 50.0;
}

void shader_setup(){
#if defined(__linux__) || defined(__APPLE__)
    std::string shaderDir = "..\\..\\src\\shaders\\";
#else
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> shadingMethod = {
        "default", "bling-phong", "gouraud", "metallic", "glass_schlick", "bubble", "bomb"
    };

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";
        std::string gpath = shaderDir + shadingMethod[i] + ".geom";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
        if (shadingMethod[i] == "bubble") {
            shaderProgram->add_shader(gpath, GL_GEOMETRY_SHADER);
        }
        if (shadingMethod[i] == "bomb") {
            shaderProgram->add_shader(gpath, GL_GEOMETRY_SHADER);
        }
        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    }
}

void cubemap_setup(){
#if defined(__linux__) || defined(__APPLE__)
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#else
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> faces
    {
        cubemapDir + "right.jpg",
        cubemapDir + "left.jpg",
        cubemapDir + "top.jpg",
        cubemapDir + "bottom.jpg",
        cubemapDir + "front.jpg",
        cubemapDir + "back.jpg"
    };
    cubemapTexture = loadCubemap(faces);   

    std::string vpath = shaderDir + "cubemap.vert";
    std::string fpath = shaderDir + "cubemap.frag";
    
    cubemapShader = new shader_program_t();
    cubemapShader->create();
    cubemapShader->add_shader(vpath, GL_VERTEX_SHADER);
    cubemapShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    cubemapShader->link_shader();

    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void bubble_setup() {
    
    glGenVertexArrays(1, &bubbleVAO);
    glGenBuffers(1, &bubbleVBO);
    
    glBindVertexArray(bubbleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bubbleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bubbleOffsets), bubbleOffsets, GL_STATIC_DRAW);
    
    // layout(location = 0) in float startOffset;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void setup(){
    light_setup();
    model_setup();
    shader_setup();
    camera_setup();
    cubemap_setup();
    material_setup();
    bubble_setup();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void update(){
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    if (camera.enableAutoOrbit) {
        float yawDelta = camera.autoOrbitSpeed * deltaTime;
        applyOrbitDelta(yawDelta, 0.0f, 0.0f);
    }

    // 丟漢堡
    float now = glfwGetTime();
    // 手轉
    if (isThrowing) {
        float timePassed = now - throwStartTime;
        float totalAnimTime = 0.5f; 
        float currentAngleDeg = 360.0f * (timePassed / totalAnimTime);
        handRotationAngle = glm::radians(currentAngleDeg);

        //在 1/3 圈時發射
        if (currentAngleDeg >= 120.0f && !hasFired) {
            hasFired = true;

            glm::vec4 shoulderLocal = glm::vec4(0.4f, 0.4f, 0.0f, 1.0f);
            glm::vec3 shoulderWorld = glm::vec3(modelMatrix * shoulderLocal);
            glm::vec3 armVector = glm::vec3(-0.7f, -0.4f, 0.0f) * 100.0f;

            glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::vec3 rotatedArm = glm::vec3(rotMat * glm::vec4(armVector, 1.0f));

            glm::vec3 spawnPos = shoulderWorld + rotatedArm;

            glm::vec3 rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f); // 你的旋轉軸
            glm::vec3 tangentDir = glm::cross(rotationAxis, rotatedArm);
            tangentDir = glm::normalize(tangentDir);

            float throwPower = 250.0f; 

            Burger newBurger;
            newBurger.position = spawnPos;
            newBurger.velocity = tangentDir * throwPower;
            newBurger.isFlying = true;
            newBurger.stateStartTime = now;
            
            burgerList.push_back(newBurger);
        }

        if (timePassed >= totalAnimTime) {
            isThrowing = false;
            handRotationAngle = 0.0f;
        }
    }

    for(int i = 0; i < burgerList.size(); i++) {
        if (burgerList[i].isFlying) { // 漢堡飛
            float t = now - burgerList[i].stateStartTime; 
            float dt = deltaTime;            

            burgerList[i].velocity += gravity * dt;
            burgerList[i].position += burgerList[i].velocity * dt;

            float floorLevel = -20.0f;
            float burgerRadius = 20.0f;

            if (burgerList[i].position.y <= floorLevel + burgerRadius) {
                burgerList[i].isFlying = false;
                burgerList[i].stateStartTime = now; 
                burgerList[i].position.y = floorLevel + burgerRadius; 
            }
        }
        else{ //爆炸
            if (now - burgerList[i].stateStartTime > 1.0f) {
                burgerList.erase(burgerList.begin() + i);
                i--; 
            }
            else {
                continue;
            }
        }
    }

}

void render(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(camera.position - glm::vec3(0.0f, 0.2f, 0.1f), camera.position + camera.front, camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    // set matrix for view, projection, model transformation
    shaderPrograms[shaderProgramIndex]->use();
    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", modelMatrix);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("view", view);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("projection", projection);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("viewPos", camera.position - glm::vec3(0.0f, 0.2f, 0.1f));

    // TODO: set additional uniform value for shader program
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.position", light.position);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.ambient", light.ambient);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.diffuse", light.diffuse);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.specular", light.specular);

    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.ambient", material.ambient);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.diffuse", material.diffuse);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.specular", material.specular);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.gloss", material.gloss);

    shaderPrograms[shaderProgramIndex]->set_uniform_value("bias", 0.2f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("scale", 0.7f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("power", 2.0f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("alpha", 0.4f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("eta", 1.0f / 1.52f);

    // specifying sampler for shader program
    shaderPrograms[shaderProgramIndex]->set_uniform_value("ourTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("skybox", 1);

    glActiveTexture(GL_TEXTURE0);


    // 畫地板 
    glm::mat4 floorMatrix = glm::mat4(1.0f);
    floorMatrix = glm::translate(floorMatrix, glm::vec3(0.0f, -0.5f, 0.0f)); 
    floorMatrix = glm::scale(floorMatrix, glm::vec3(1000.0f, 1.0f, 1000.0f));

    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", floorMatrix);
    
    if(floorModel != nullptr) {
        floorModel->draw();
    }

    // 畫海綿寶寶
    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", modelMatrix);
    
    spongeBobBody->draw();
    spongeBobLeftHand->draw();

    // 右手旋轉
    glm::mat4 rightHandMatrix = modelMatrix;
    
    if (isThrowing) {
        rightHandMatrix = glm::translate(rightHandMatrix, glm::vec3(0.4f, 0.4f, 0.0f)); // 移到肩膀 (Local scale 1.0)
        rightHandMatrix = glm::rotate(rightHandMatrix, handRotationAngle, glm::vec3(1, 0, 0)); // 轉
        rightHandMatrix = glm::translate(rightHandMatrix, glm::vec3(-0.4f, -0.4f, 0.0f)); // 移回來
    }
    
    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", rightHandMatrix);
    spongeBobRightHand->draw();

    // 畫漢堡
    shaderPrograms[shaderProgramIndex]->use();

    for(int i = 0; i < burgerList.size(); i++) {
        if(burgerList[i].isFlying) {
            glm::mat4 burgerMat = glm::mat4(1.0f);
            burgerMat = glm::translate(burgerMat, burgerList[i].position);
            burgerMat = glm::scale(burgerMat, glm::vec3(40.0f)); 
            // 漢堡自轉
            burgerMat = glm::rotate(burgerMat, (float)glfwGetTime() * 5.0f, glm::vec3(1,1,1));

            shaderPrograms[shaderProgramIndex]->set_uniform_value("model", burgerMat);
            burgerModel->draw();
        }
    }

    // 畫爆炸 
    shader_program_t* bombShader = shaderPrograms[6]; 
    bombShader->use();
    
    bombShader->set_uniform_value("view", view);
    bombShader->set_uniform_value("projection", projection);
    bombShader->set_uniform_value("ourTexture", 0);

    for(int i = 0; i < burgerList.size(); i++) {
        if (!burgerList[i].isFlying) { // 爆炸中
            glm::mat4 explodeMat = glm::mat4(1.0f);
            explodeMat = glm::translate(explodeMat, burgerList[i].position); 
            explodeMat = glm::scale(explodeMat, glm::vec3(40.0f)); 

            bombShader->set_uniform_value("model", explodeMat);        
            float explodeTime = (float)glfwGetTime() - burgerList[i].stateStartTime;
            bombShader->set_uniform_value("time", explodeTime);
            bombShader->set_uniform_value("aExplosionColor", glm::vec3(1.0f, 0.2f, 0.0f));

            burgerModel->draw();
        }
    }
    
    shaderPrograms[shaderProgramIndex]->use();

    shaderPrograms[shaderProgramIndex]->release();

    // TODO 
    // Rendering cubemap environment
    // Hint:
    // 1. All the needed things are already set up in cubemap_setup() function.
    // 2. You can use the vertices in cubemapVertices provided in the header/cube.h
    // 3. You can use the cubemapShader to render the cubemap 
    //    (refer to the above code to get an idea of how to use the shader program)
    glDepthFunc(GL_LEQUAL);
    cubemapShader->use();
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    cubemapShader->set_uniform_value("view", viewNoTranslation);
    cubemapShader->set_uniform_value("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    cubemapShader->set_uniform_value("skybox", 0);

    glBindVertexArray(cubemapVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    cubemapShader->release();
    glDepthFunc(GL_LESS);

    // 畫泡泡
    if (isBlowing){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader_program_t* bubbleShader = shaderPrograms[5]; // 假設是 index 5
        bubbleShader->use();
        bubbleShader->set_uniform_value("view", view);
        bubbleShader->set_uniform_value("projection", projection);
        bubbleShader->set_uniform_value("time", (float)glfwGetTime());
        bubbleShader->set_uniform_value("model", modelMatrix); 

        glBindVertexArray(bubbleVAO);
        glDrawArrays(GL_POINTS, 0, 10); 
        
        glBindVertexArray(0);
        bubbleShader->release();

        glDisable(GL_BLEND);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW3-Static Model", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    setup();
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        update(); 
        render(); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete spongeBobBody;
    delete spongeBobLeftHand;
    delete spongeBobRightHand;
    delete cubeModel;
    for (auto shader : shaderPrograms) {
        delete shader;
    }
    delete cubemapShader;
    delete floorModel;

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec2 orbitInput(0.0f);
    float zoomInput = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        orbitInput.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        orbitInput.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        orbitInput.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        orbitInput.y -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        zoomInput -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        zoomInput += 1.0f;

    if (orbitInput.x != 0.0f || orbitInput.y != 0.0f || zoomInput != 0.0f) {
        float yawDelta = orbitInput.x * camera.orbitRotateSpeed * deltaTime;
        float pitchDelta = orbitInput.y * camera.orbitRotateSpeed * deltaTime;
        float radiusDelta = zoomInput * camera.orbitZoomSpeed * deltaTime;
        applyOrbitDelta(yawDelta, pitchDelta, radiusDelta);
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_0 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 0;
    if (key == GLFW_KEY_1 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 1;
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 2;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        shaderProgramIndex = 3;
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        shaderProgramIndex = 4;
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        shaderProgramIndex = 5;
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
        shaderProgramIndex = 6;
    if (key == GLFW_KEY_7 && action == GLFW_PRESS)
        shaderProgramIndex = 7;
    if (key == GLFW_KEY_8 && action == GLFW_PRESS)
        shaderProgramIndex = 8;
    if( key == GLFW_KEY_9 && action == GLFW_PRESS)
        isCube = !isCube;
    // 吹泡泡
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        isBlowing = !isBlowing;

    // 丟漢堡炸彈
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        if (!isThrowing) {
            isThrowing = true;
            throwStartTime = glfwGetTime();
            hasFired = false;
        }
    }
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

unsigned int loadCubemap(vector<std::string>& faces)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 3);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}  
