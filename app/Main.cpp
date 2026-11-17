#include "app/MainWindow.h"
#include <iostream>

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

namespace gitara::app {

class GitaraApplication final : public juce::JUCEApplication {
public:
    const juce::String getApplicationName() override { return "Apka Gitarowa"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override {
        mainWindow_ = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override {
        mainWindow_.reset();
    }

    void systemRequestedQuit() override {
        quit();
    }

    class MainWindow final : public juce::DocumentWindow {
    public:
        explicit MainWindow(const juce::String& name)
            : DocumentWindow(name,
                             juce::Colour(0xFF14110F),
                             DocumentWindow::allButtons) {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainContentComponent(), true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow_;
};

} // namespace gitara::app

START_JUCE_APPLICATION(gitara::app::GitaraApplication)

#else

int main() {
    std::cout << "Apka Gitarowa (rdzen C++20: libs/theory, libs/score, libs/dsp, libs/engine).\n"
              << "Aby zbudowac pelne okno GUI, skonfiguruj CMake z opcja -DGITARA_BUILD_JUCE_APP=ON.\n";
    return 0;
}

#endif
