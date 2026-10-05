#pragma once

#include <glm/glm.hpp>
#include <string>
#include <cstdint>

namespace opengame
{
    using ObjectID = std::uint64_t;

    class Object
    {
    public:
        Object();
        ~Object();

        Object(const Object &) = default;
        Object &operator=(const Object &) = default;
        Object(Object &&) = default;
        Object &operator=(Object &&) = default;

        ObjectID getID() const { return id; }
        void setID(ObjectID newID) { id = newID; }

        const std::string &getType() const { return type; }
        void setType(const std::string &type) { this->type = type; }

        const std::string &getPath() const { return objpath; }
        void setPath(const std::string &path) { objpath = path; }

        const glm::vec3 &getPosition() const { return position; }
        const glm::vec3 &getRotation() const { return rotation; }
        const glm::vec3 &getScale() const { return scale; }

        void setPosition(const glm::vec3 &newPosition) { position = newPosition; }
        void setRotation(const glm::vec3 &newRotation) { rotation = newRotation; }
        void setScale(const glm::vec3 &newScale) { scale = newScale; }

        bool isActive() const { return activeState; }
        void setActive(bool active) { activeState = active; }

        void translate(const glm::vec3 &offset) { position += offset; }
        void rotate(const glm::vec3 &rotation) { this->rotation += rotation; }
        void scaleBy(const glm::vec3 &scale) { this->scale *= scale; }

        void update(float deltaTime);

    private:
        ObjectID id{0};
        std::string objpath;
        std::string type;
        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};
        bool activeState{true};
    };
}