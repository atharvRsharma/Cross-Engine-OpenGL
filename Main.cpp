#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp> // For JSON serialization

#include <iostream>
#include <fstream>
#include <string>
#include "Camera.h"

#include <vector>        // <-- ADD THIS
#include <cstddef>       // <-- ADD THIS
#include "FileParser.h"


const int h = 800;
const int w = 800;

// Use nlohmann::json for convenience
using json = nlohmann::json;

// --- 1. Entity Definition ---
// This struct holds the state of our orb, matching the JSON schema [cite: 31]
struct GlowingOrb {
    std::string id = "entity_01";
    glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f); // Start mid-air
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    float energy = 0.5f; // Start at 50%
    std::string state = "active";
};

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    // We are already sending this, but now we'll use it
    layout (location = 2) in vec3 aNormal; 

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    // NEW: Outputs for the fragment shader
    out vec3 FragPos; 
    out vec3 Normal;  

    void main() {
        // Calculate world position
        FragPos = vec3(model * vec4(aPos, 1.0));
        
        // Calculate the correct normal in world space
        // This math handles non-uniform scaling, just in case
        Normal = mat3(transpose(inverse(model))) * aNormal; 
        
        // Standard position calculation
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    // NEW: Inputs from vertex shader
    in vec3 FragPos; 
    in vec3 Normal;  

    uniform float energy;
    uniform vec3 viewPos; // NEW: The camera's world position

    void main() {
        // Colors from your original shader
        vec3 lowEnergyColor = vec3(0.0, 0.1, 0.5);  // Dim blue
        vec3 highEnergyColor = vec3(1.0, 1.0, 0.0); // Bright yellow
        
        // Get the base color based on energy
        vec3 baseColor = mix(lowEnergyColor, highEnergyColor, energy);

        // --- FAKE "GLOW" LOGIC ---

        // 1. Get normalized directions
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);

        // 2. Calculate the "center factor"
        // This is 1.0 at the center (where normal faces camera)
        // and 0.0 at the edges (where normal is perpendicular)
        float centerFactor = dot(viewDir, norm);
        
        // 3. Use pow() to create a small, intense "hotspot"
        // A higher number (e.g., 3.0) makes the hotspot smaller.
        centerFactor = pow(centerFactor, 2.5); 

        // 4. Create the final color
        // Mix from the baseColor (at the edges, where centerFactor=0)
        // to pure white (at the center, where centerFactor=1)
        vec3 finalColor = mix(baseColor, vec3(1.0, 1.0, 1.0), centerFactor);

        // Use energy to control the *overall* brightness of the effect
        FragColor = vec4(finalColor * (0.5 + energy), 1.0);
    }
)";

// --- 3. Global Variables (for simplicity) ---
 // The one and only entity
bool shouldSave = false;
bool shouldInteract = false;




Camera camera(glm::vec3(0.0f, 0.0f, 15.0f)); // Start 15 units back
float lastX = h/2; // Set to center of screen
float lastY = w/2;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


// --- 4. Helper Functions ---

// GLFW error callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Shader compilation helper
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return id;
}

// Shader program linking helper
unsigned int createShaderProgram(const char* vsSource, const char* fsSource) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

// --- 5. Core Game Logic Functions ---

// Process input from GLFW
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Press 'S' to save the state
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        shouldSave = true;

    // Hold 'E' to "touch" the orb and increase energy 
    shouldInteract = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
}




void processInput(GLFWwindow* window, float deltaTime) // <-- ADD deltaTime
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Example for UP
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // Example for DOWN
        camera.ProcessKeyboard(DOWN, deltaTime);


    // Orb interaction
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        shouldSave = true;

    shouldInteract = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
}





// Update entity state (physics, interactions)
void update(GlowingOrb& orb, float deltaTime) {

    // 1. Interaction: "Touching" the orb 
    if (shouldInteract) {
        orb.energy += 1.0f * deltaTime; // Increase energy while 'E' is held
        orb.energy = glm::min(orb.energy, 1.0f); // Clamp at 1.0
    }
    // 2. Optional: Energy drain [cite: 29]
    else {
        orb.energy -= 0.1f * deltaTime; // Drains to 0 over 10 seconds
        orb.energy = glm::max(orb.energy, 0.0f); // Clamp at 0.0
    }

    // 3. Physics: Basic gravity [cite: 15]
    const float gravity = -9.8f;
    orb.velocity.y += gravity * deltaTime;
    orb.position += orb.velocity * deltaTime;

    // 4. Physics: Simple ground collision/bounce
    if (orb.position.y < -5.0f) {
        orb.position.y = -5.0f;
        orb.velocity.y = -orb.velocity.y * 0.6f; // Dampen bounce
        // Stop tiny bounces
        if (glm::abs(orb.velocity.y) < 0.1f) {
            orb.velocity.y = 0.0f;
        }
    }
}

// Render the entity
void render(const GlowingOrb& orb, unsigned int shaderProgram, unsigned int vao, unsigned int vertexCount) {
    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    // Use our shader
    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(h / w), 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // View
    glm::mat4 view = camera.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


    glUniform1f(glGetUniformLocation(shaderProgram, "energy"), orb.energy);

    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.Position));


    // Model (Apply the orb's position) [cite: 14]
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, orb.position);
    //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f)); // Add a spin
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // --- Set up state uniform ---
    // This is the core state-change link! [cite: 16]
    glUniform1f(glGetUniformLocation(shaderProgram, "energy"), orb.energy);

    // Draw the orb
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}


// Load the entity's state from a JSON file
// Returns a new GlowingOrb object with the loaded data.
GlowingOrb loadState() {
    // Start with a default orb, in case loading fails
    GlowingOrb orb;

    // 1. Try to open the file
    std::ifstream f("entity_state.json");

    // 2. Check if the file exists and is readable
    if (!f.is_open()) {
        std::cout << "INFO: No 'entity_state.json' found. Starting new simulation." << std::endl;
        return orb; // Return the default orb
    }

    // 3. Try to parse the file
    try {
        json j;
        f >> j; // This reads the file content into the json object
        f.close();

        // 4. Populate the orb struct from the JSON data
        //    We use .at() which will throw an exception if the key doesn't exist
        orb.id = j.at("id").get<std::string>();

        json pos_j = j.at("position");
        orb.position = glm::vec3(pos_j[0].get<float>(), pos_j[1].get<float>(), pos_j[2].get<float>());

        json vel_j = j.at("velocity");
        orb.velocity = glm::vec3(vel_j[0].get<float>(), vel_j[1].get<float>(), vel_j[2].get<float>());

        orb.energy = j.at("energy").get<float>();
        orb.state = j.at("state").get<std::string>();

        std::cout << "SUCCESS: Loaded previous state from 'entity_state.json'." << std::endl;

    }
    catch (json::exception& e) {
        // 5. If parsing fails (e.g., corrupt file), log it and return the default orb
        std::cerr << "ERROR: Failed to parse 'entity_state.json'. " << e.what() << std::endl;
        std::cerr << "Starting new simulation with default values." << std::endl;
        // We just return the default 'orb' we created at the beginning
    }

    return orb;
}



// Save the entity's state to a JSON file [cite: 17]
void saveState(const GlowingOrb& orb) {
    json j;

    // Populate the JSON object from the struct
    j["id"] = orb.id;
    j["position"] = { orb.position.x, orb.position.y, orb.position.z };
    j["velocity"] = { orb.velocity.x, orb.velocity.y, orb.velocity.z };
    j["energy"] = orb.energy;
    j["state"] = orb.state;

    // Write to file
    try {
        std::ofstream o("entity_state.json");
        o << j.dump(4); // .dump(4) pretty-prints the JSON
        o.close();
        std::cout << "SUCCESS: Entity state saved to entity_state.json" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to save state to JSON. " << e.what() << std::endl;
    }
}

// --- 6. Main Function ---
int main() {
    // --- GLFW & GLAD Initialization ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(h, w, "OpenGL Simulation", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture mouse
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GlowingOrb myOrb = loadState();

    glEnable(GL_DEPTH_TEST);

    // --- Shader Program Creation ---
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    std::vector<Primitives::Vertex> orbVertices;
    parseObj("sphere.txt", orbVertices);
    

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        orbVertices.size() * sizeof(Primitives::Vertex), // Total size in bytes
        orbVertices.data(),                            // Pointer to the data
        GL_STATIC_DRAW);

    // NEW: Tell OpenGL how to read the Primitives::Vertex struct

    // 1. Position Attribute (matches layout (location = 0) in shader)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Primitives::Vertex), // Stride: size of one whole vertex
        (void*)offsetof(Primitives::Vertex, position)); // Offset to this attribute
    glEnableVertexAttribArray(0);

    // 2. Texture Coord Attribute (we'll set this up for later)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        sizeof(Primitives::Vertex),
        (void*)offsetof(Primitives::Vertex, texCoord));
    glEnableVertexAttribArray(1);

    // 3. Normal Attribute (we'll set this up for later)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
        sizeof(Primitives::Vertex),
        (void*)offsetof(Primitives::Vertex, normal));
    glEnableVertexAttribArray(2);

    // --- Main Loop ---
    float lastFrame = 0.0f;

    unsigned int orbVertexCount = static_cast<unsigned int>(orbVertices.size());

    while (!glfwWindowShouldClose(window)) {
        // Delta time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 1. Input
        processInput(window, deltaTime);;

        // 2. Update
        update(myOrb, deltaTime);

        // 3. Render
        render(myOrb, shaderProgram, VAO, orbVertexCount);

        // 4. Check for Save
        if (shouldSave) {
            saveState(myOrb);
            shouldSave = false; // Only save once per press
        }

        // 5. Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- Cleanup ---
    // Final save on exit
    saveState(myOrb);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}