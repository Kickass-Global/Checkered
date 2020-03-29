#pragma once



namespace Horn {

    const char module[] = "Horn";

    class hornSystem : public Engine::SystemInterface {

    public:
        void initialize() override;

        void update(Engine::deltaTime /*elapsed*/) override;

        std::shared_ptr<Component::EventHandler<int>> onKeyPressHandler;
        std::shared_ptr<Component::EventHandler<int>> onKeyDownHandler;
        std::shared_ptr<Component::EventHandler<int>> onKeyUpHandler;

        //std::set<int> keys;
    private:
        //std::set<int> keys;
        void onKeyDown(const Component::EventArgs<int> &args);

        void onKeyUp(const Component::EventArgs<int> &args);

        void onKeyPress(const Component::EventArgs<int> &args);
        // void sound();
    };
}
