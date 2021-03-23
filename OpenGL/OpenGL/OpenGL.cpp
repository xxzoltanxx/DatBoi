// OpenGL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* vertexShaderS = R"(
    #version 330 core
    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec2 texCoord;
    
    out vec2 TexCoord;
    uniform mat4 modelMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 projectionMatrix;
    void main()
    {
        gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);
        TexCoord = texCoord;
    }
)";

const char* fragmentShaderS = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 finalColor;

    uniform sampler2D text;
    void main()
    {
        if (texture(text,TexCoord).a == 0)
        {
            discard;
        }
        finalColor = texture(text, TexCoord);
    }
)";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void checkShaderInfoLog(unsigned int shader, bool isLink = false)
{
    int success;
    char infoLog[512];
    if (isLink)
    {
        glGetShaderiv(shader, GL_LINK_STATUS, &success);
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    }
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << infoLog;
    }
}

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

class Texture
{
public:
    Texture(const unsigned char* data, int width, int height);
    Texture(const std::string& filename);
    ~Texture();
    unsigned int getID() const { return id; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
private:
    unsigned int id;
    int width;
    int height;
};

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

Texture::Texture(const unsigned char* data, int width, int height)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
Texture::Texture(const std::string& filename)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

float quad[] =
{
    -0.5f, -0.5f, 0.0f, 0, 0,
    -0.5f, 0.5f, 0.0f, 0, 1,
    0.5f, -0.5f, 0.0f, 1 , 0,
   -0.5f, 0.5f, 0.0f, 0, 1,
    0.5f, -0.5f, 0.0f, 1, 0,
    0.5f,0.5f,0.0f, 1, 1
};

class Sprite
{
public:
    Sprite(const Texture& texture, unsigned int shader);
    void setTextureRect(glm::vec2 origin, glm::vec2 size);
    void setPosition(glm::vec2 position);
    glm::vec2 getPosition() const { return position; }
    void draw(glm::mat4& view, glm::mat4& projection);

private:
    const Texture* texture;
    glm::vec2 textOrigin;
    glm::vec2 size;
    glm::vec2 position;
    float rotation;
    glm::vec2 scale;
    glm::vec2 nativeScale;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int shader;
};

class SpriteSheet
{
public:
    SpriteSheet(Sprite& sprite, glm::vec2 cellSize, glm::vec2 offset, unsigned int frames, float frameTime);
    void update(float dt);
    void draw(glm::mat4& view, glm::mat4& proj);
    void setPosition(glm::vec2 newPos)
    {
        sprite->setPosition(newPos);
    }
    glm::vec2 getPosition() const {
        return sprite->getPosition();
    }
private:
    Sprite* sprite;
    glm::vec2 cellSize;
    glm::vec2 offset;
    unsigned int frames;
    unsigned int currentFrame;
    float frameTime;
    float elapsedTime;
};

class DatBoi
{
public:
    DatBoi(SpriteSheet* datBoiSheet);
    void updateDatBoi(float dt);
    void drawDatBoi(glm::mat4& view, glm::mat4& proj);
private:
    glm::vec2 datBoiVelocity = { 400.0, 560.0 };
    SpriteSheet* datBoiSpriteSheet;
};

DatBoi::DatBoi(SpriteSheet* datBoiSheet)
{
    this->datBoiSpriteSheet = datBoiSheet;
    datBoiSheet->setPosition(glm::vec2(0, 0));
}

void DatBoi::drawDatBoi(glm::mat4& view, glm::mat4& proj)
{
    datBoiSpriteSheet->draw(view, proj);
}

void DatBoi::updateDatBoi(float dt)
{
    datBoiSpriteSheet->update(dt);
    datBoiSpriteSheet->setPosition(datBoiSpriteSheet->getPosition() + datBoiVelocity * dt);
    if (datBoiSpriteSheet->getPosition().x < -3300)
    {
        datBoiVelocity.x = 400.0;
    }
    if (datBoiSpriteSheet->getPosition().x > 3300)
    {
        datBoiVelocity.x = -400.0;
    }
    if (datBoiSpriteSheet->getPosition().y < -700)
    {
        datBoiVelocity.y = 560.0;
    }
    if (datBoiSpriteSheet->getPosition().y > 700)
    {
        datBoiVelocity.y = -560.0;
    }
}

SpriteSheet::SpriteSheet(Sprite& sprite, glm::vec2 cellSize, glm::vec2 offset, unsigned int frames, float frameTime)
{
    this->frameTime = frameTime;
    this->frames = frames;
    this->cellSize = cellSize;
    this->offset = offset;
    this->sprite = &sprite;

    sprite.setTextureRect(offset, cellSize);
}

void SpriteSheet::update(float dt)
{
    elapsedTime += dt;
    if (elapsedTime > frameTime)
    {
        elapsedTime = 0;
        if (++currentFrame > frames)
        {
            currentFrame = 0;
        }
        offset = glm::vec2(cellSize.x * currentFrame, 0);
        sprite->setTextureRect(offset, cellSize);
    }
}

void SpriteSheet::draw(glm::mat4& view, glm::mat4& proj)
{
    sprite->draw(view, proj);
}

void Sprite::setPosition(glm::vec2 position)
{
    this->position = position;
}

void Sprite::setTextureRect(glm::vec2 origin, glm::vec2 size)
{
    this->textOrigin = origin;
    this->size = size;
}

Sprite::Sprite(const Texture& texture, unsigned int shader)
{
    this->shader = shader;
    textOrigin = glm::vec2(0, 0);
    size = glm::vec2(texture.getWidth(), texture.getHeight());
    this->texture = &texture;
    if (texture.getWidth() > texture.getHeight())
    {
        nativeScale = glm::vec2((float)texture.getWidth() / texture.getHeight(), 1.0f);
    }
    else
    {
        nativeScale = glm::vec2(1.0f, (float)texture.getHeight() / texture.getWidth());
    }
    scale = glm::vec2(1.0f);
    rotation = 0.0f;
    position = glm::vec2(0.0f);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

}

void Sprite::draw(glm::mat4& view, glm::mat4& projection)
{

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    quad[3] = textOrigin.x / texture->getWidth();
    quad[4] = textOrigin.y / texture->getHeight();
    quad[8] = textOrigin.x / texture->getWidth();
    quad[9] = (textOrigin.y + size.y) / texture->getHeight();
    quad[13] = (textOrigin.x + size.x) / texture->getWidth();
    quad[14] = textOrigin.y / texture->getHeight();
    quad[18] = textOrigin.x / texture->getWidth();
    quad[19] = (textOrigin.y + size.y) / texture->getHeight();
    quad[23] = (textOrigin.x + size.x) / texture->getWidth();
    quad[24] = textOrigin.y / texture->getHeight();
    quad[28] = (textOrigin.x + size.x) / texture->getWidth();
    quad[29] = (textOrigin.y + size.y) / texture->getHeight();
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glUseProgram(shader);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3((position.x) / 1920.0, (position.y) / 1080.0, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale.x * nativeScale.x * size.x / texture->getWidth(), scale.y * nativeScale.y * size.y / texture->getHeight(), 0));

    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->getID());

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "openGL", glfwGetPrimaryMonitor(), nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize glad";
        return -1;
    }
    glViewport(0, 0, 1920, 1080);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderS, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderS, nullptr);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    checkShaderInfoLog(vertexShader, false);
    checkShaderInfoLog(fragmentShader, false);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    checkShaderInfoLog(program, true);

    stbi_set_flip_vertically_on_load(true);

    glm::mat4 projection = glm::perspective(45.0f, 1920 / 1080.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0, 0, -2.0f));
    glEnable(GL_DEPTH_TEST);

    Texture texture("datboi.png");
    Sprite sprite(texture, program);
    SpriteSheet spriteSheet(sprite, glm::vec2(312, 390), glm::vec2(0, 0), 4, 0.1f);
    DatBoi datBoi(&spriteSheet);

    float elapsedTime = 0;
    float time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.5, 0.5, 0.5, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        processInput(window);
        while (elapsedTime > 1.0f / 60.0f)
        {
            elapsedTime -= 1.0f / 60.0f;
            datBoi.updateDatBoi(1.0f / 60.0f);
        }
        datBoi.drawDatBoi(view, projection);

        glfwSwapBuffers(window);
        float currentTime = glfwGetTime();
        elapsedTime += currentTime - time;
        time = currentTime;
    }
    glfwTerminate();
    return 0;
}